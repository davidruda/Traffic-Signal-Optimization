import argparse
from array import array
from collections import defaultdict
import datetime
from functools import partial
import os
import random
import re
import time
import threading

from deap import base, creator, tools
import numpy as np

from traffic_signaling import *

from operators import (
    genetic_algorithm, hill_climbing, simulated_annealing,
    crossover, mutation, LinearSchedule, tournament_selection_with_elitism
)

parser = argparse.ArgumentParser()
parser.add_argument('algorithm', choices=['ga', 'hc', 'sa'], help='Algorithm to use for optimization - Genetic Algorithm, Hill Climbing, Simulated Annealing.')
parser.add_argument('data', choices=TEST_DATA, help='Input dataset - a, b, c, d, e, f.')

# Common parameters for all algorithms
parser.add_argument('--order_init', default='default', choices=['adaptive', 'random', 'default'], help='Method for initializing the order of streets.')
parser.add_argument('--times_init', default='default', choices=['scaled', 'default'], help='Method for initializing green light durations.')
parser.add_argument('--mutation_bit_rate', default=10, type=float, help='If between 0-1, it defines the probability of mutating each bit. If >= 1, it defines the expected value of bits to mutate.')

parser.add_argument('--seed', default=42, type=int, help='Random seed.')
parser.add_argument('--threads', default=None, type=int, help='Number of threads for parallel execution.')
parser.add_argument('--verbose', default=False, action='store_true', help='Print detailed information during execution.')
parser.add_argument('--no-save', default=False, action='store_true', help='Do not save results and plots.')
parser.add_argument('--logdir', default=None, type=str, help='Custom name for the log directory.')

# Hyperparameters for Genetic Algorithm
parser.add_argument('--population', default=100, type=int, help='Number of individuals in a population (Genetic Algorithm only).')
parser.add_argument('--generations', default=100, type=int, help='Number of generations (Genetic Algorithm only).')
parser.add_argument('--crossover', default=0.6, type=float, help='Crossover probability (Genetic Algorithm only).')
parser.add_argument('--mutation', default=0.4, type=float, help='Mutation probability (Genetic Algorithm only).')
parser.add_argument('--elitism', default=0.05, type=float, help='Elitism rate (Genetic Algorithm only).')
parser.add_argument('--tournsize', default=3, type=int, help='Tournament size for selection (Genetic Algorithm only).')

# Hyperparameters for Hill Climbing and Simulated Annealing
parser.add_argument('--instances', default=1, type=int, help='Number of independent instances to run in parallel (Hill Climbing and Simulated Annealing).')
parser.add_argument('--iterations', default=5000, type=int, help='Number of iterations (Hill Climbing and Simulated Annealing).')
parser.add_argument('--temperature', default=100, type=float, help='Initial temperature for cooling schedule (Simulated annealing only).')


class Optimizer:
    def __init__(self, args):
        self.plan = create_city_plan(args.data)
        self._args = args
        self._simulations = defaultdict(partial(default_simulation, city_plan=self.plan))
        self._toolbox = base.Toolbox()
        self._stats = tools.Statistics(lambda ind: ind.fitness.values)
        self._hof = tools.HallOfFame(1)
        self._logbook = None
        self._elapsed_time = None

        random.seed(args.seed)
        np.random.seed(args.seed)
        # also set seed for the simulation module
        set_seed(args.seed)

        self._register_deap_functions()

    def _register_deap_functions(self):
        if self._args.threads:
            import concurrent.futures
            pool = concurrent.futures.ThreadPoolExecutor(max_workers=self._args.threads)
            self._toolbox.register('map', pool.map)

        creator.create('FitnessMax', base.Fitness, weights=(1.0,))
        creator.create('Individual', list, fitness=creator.FitnessMax)

        sim = Simulation(self.plan)
        sim.create_schedules(order=self._args.order_init, times=self._args.times_init)

        self._toolbox.register(
            'individual', tools.initIterate, creator.Individual, partial(self._create_individual, simulation=sim)
        )
        self._toolbox.register('population', tools.initRepeat, list, self._toolbox.individual)
        self._toolbox.register('evaluate', self._evaluate)

        if self._args.algorithm == 'ga':
            selection = partial(
                tournament_selection_with_elitism, tournsize=self._args.tournsize, elitism=self._args.elitism
            )
            self._toolbox.register('select', selection)
            self._toolbox.register('mate', crossover)

        if self._args.algorithm == 'sa':
            # Cooling schedule for simulated annealing
            cooling_schedule = LinearSchedule(start=self._args.temperature, steps=self._args.iterations)
            self._toolbox.register('schedule', cooling_schedule)

        # Mininum value for green light duration
        green_min = 0
        # Maximum value for green light duration
        green_max = self.plan.duration

        mutation_bit_rate = self._args.mutation_bit_rate
        if mutation_bit_rate >= 0 and mutation_bit_rate < 1:
            # Probability of mutating each bit
            indpb = mutation_bit_rate
        elif mutation_bit_rate >= 1:
            # Expected number of bits to mutate
            indpb = mutation_bit_rate / PARAMETERS[self._args.data]
        else:
            raise ValueError('Mutation bit rate must be >= 0.')
        self._toolbox.register('mutate', mutation, indpb=indpb, low=green_min, up=green_max)

        norm_score = partial(normalized_score, data=self._args.data)
        self._stats.register('norm_max', lambda x: norm_score(np.max(x)))
        # This weird manipulation is necessary to avoid scientific notation
        # and use thousand separator when printing statistics with tools.Statistics
        self._stats.register('max', lambda x: f'{int(np.max(x)):,}')
        self._stats.register('norm_avg', lambda x: norm_score(np.mean(x)))
        self._stats.register('avg', lambda x: f'{int(np.mean(x)):,}')

    def _create_individual(self, simulation):
        if self._args.order_init == 'random':
            # For random order initialization, we need to create new schedules each time
            simulation.create_schedules(order=self._args.order_init, times=self._args.times_init)
        # Relative order is required for the operators (order crossover) to work correctly
        schedules = simulation.non_trivial_schedules(relative_order=True)

        # Convert schedules from lists to array.arrays to speed up the optimization
        individual = [
            (array('L', order), array('L', times)) for order, times in schedules
        ]
        return individual

    def _evaluate(self, individual):
        # Keep reusing the same simulation object for the same thread
        simulation = self._simulations[threading.get_ident()]
        # Individual is in the relative_order format
        simulation.set_non_trivial_schedules(individual, relative_order=True)
        fitness = simulation.score()
        return fitness,

    def _save_data_plots(self, logdir, show_plot=False):
        import matplotlib.pyplot as plt
        import matplotlib.ticker as ticker
        import pandas as pd
        gen, new_vals, norm_max, max, norm_avg, avg = self._logbook.select(
            'gen', 'nevals', 'norm_max', 'max', 'norm_avg', 'avg'
        )

        def to_int(x):
            return int(x.replace(',', ''))
        max, avg = (list(map(to_int, stat)) for stat in [max, avg])

        df = pd.DataFrame({
            'gen': gen, 'new_vals': new_vals, 'norm_max': norm_max, 'max': max, 'norm_avg': norm_avg,
            'avg': avg
        })
        df.to_csv(os.path.join(logdir, f'{self._args.data}.csv'), index=False)


        fig, ax1 = plt.subplots()
        ax1.plot(gen, norm_max, label='max score in a generation')
        ax1.plot(gen, norm_avg, label='avg score in a generation')
        ax1.axhline(0, color='m', linestyle='--', label='baseline')
        ax1.axhline(1, color='y', linestyle='--', label='max known score')

        ax1.set_xlabel('Number of generations/iterations')
        ax1.set_ylabel('Normalized score')

        # Keep some slack around the y-axis limits to display the baseline and max known score
        y_min = np.min([-0.03, np.min(norm_max)])
        y_max = np.max([1.03, np.max(norm_max)])
        ax1.set_ylim(y_min, y_max)
        ax1.legend()
        best_fitness = np.max(max)
        ax1.set_title(f'Best score: {best_fitness:,} ({100 * normalized_score(best_fitness, self._args.data):.2f} %)')

        ax2 = ax1.twinx()
        ax2.set_ylim(*ax1.get_ylim())
        ax2.grid(False)
        ax2.set_ylabel('Score')

        ticks = ax1.get_yticks()
        labels = (f'{absolute_score(x, args.data):,}' for x in ticks)
        ax2.yaxis.set_major_locator(ticker.FixedLocator(ax2.get_yticks()))
        ax2.set_yticklabels(labels)

        # Avoid scientific notation for x-axis ticks
        xticks = ax1.get_xticks()
        ax1.xaxis.set_major_locator(ticker.FixedLocator(xticks))
        ax1.set_xticklabels([f'{x:,.0f}' for x in xticks])

        fig.tight_layout()
        fig.savefig(os.path.join(logdir, f'{self._args.data}.pdf'), bbox_inches='tight')

        if show_plot:
            plt.show()

    def _save_best_schedules(self, logdir):
        best_individual = self._hof.items[0]
        simulation = default_simulation(self.plan)
        # Individual is in the relative_order format
        simulation.set_non_trivial_schedules(best_individual, relative_order=True)
        simulation.save_schedules(os.path.join(logdir, f'{self._args.data}.out'))

    def _save_info(self, logdir):
        best_fitness = int(self._hof.keys[0].values[0])
        with open(os.path.join(logdir, 'info.txt'), 'w') as f:
            f.write(f'Best fitness: {best_fitness:,} ({100 * normalized_score(best_fitness, self._args.data):.2f} %)\n')

            if self._args.algorithm == 'ga':
                # Only count new individuals, not the unchanged ones from the previous generation
                total_evaluations = sum(self._logbook.select('nevals'))
            else:
                # Iterations + 1 because we start from 0
                total_evaluations = (self._args.iterations + 1) * self._args.instances

            f.write(f'Total evaluations: {total_evaluations:,}\n')
            f.write('\n')
            for k, v in self._args.__dict__.items():
                f.write(f'{k}={v}\n')
            f.write(f'\nElapsed time: {self._elapsed_time}\n')

    def _save_statistics(self, show_plot=False):
        if self._logbook is None:
            raise ValueError('Run the optimizer first!')

        if self._args.logdir is None:
            logdir = os.path.join('logs', self._args.data, '{}-{}-{}'.format(
                os.path.basename(globals().get('__file__', 'notebook')),
                datetime.datetime.now().strftime('%Y-%m-%d_%H%M%S'),
                ','.join(('{}={}'.format(re.sub('(.)[^_]*_?', r'\1', k), v) for k, v in sorted(vars(self._args).items())))
            ))
        else:
            logdir = self._args.logdir
        os.makedirs(logdir, exist_ok=True)
        self._save_data_plots(logdir, show_plot)
        self._save_best_schedules(logdir)
        self._save_info(logdir)

    def run(self, save_statistics=True):
        verbose = self._args.verbose
        kwargs = {
            'stats': self._stats,
            'halloffame': self._hof,
            'verbose': verbose
        }
        start = time.time()
        num_instances = self._args.population if self._args.algorithm == 'ga' else self._args.instances
        population = self._toolbox.population(n=num_instances)
        if verbose:
            print(f'Population created: {time.time() - start:.4f}s')

        if self._args.algorithm == 'ga':
            population, self._logbook = genetic_algorithm(
                population, self._toolbox, self._args.crossover, self._args.mutation, self._args.generations, **kwargs
            )

        elif self._args.algorithm == 'hc':
            population, self._logbook = hill_climbing(
                population, self._toolbox, self._args.iterations, **kwargs
            )

        elif self._args.algorithm == 'sa':
            population, self._logbook = simulated_annealing(
                population, self._toolbox, self._args.iterations, **kwargs
            )

        self._elapsed_time = datetime.timedelta(seconds=int(time.time() - start))
        print(f'Elapsed time: {self._elapsed_time}')

        best_fitness = int(self._hof.keys[0].values[0])
        print(f'Best fitness: {best_fitness:,} ({100 * normalized_score(best_fitness, self._args.data):.2f} %)')

        if save_statistics:
            self._save_statistics()


if __name__ == '__main__':
    args = parser.parse_args()
    optimizer = Optimizer(args)
    optimizer.run(save_statistics=not args.no_save)
