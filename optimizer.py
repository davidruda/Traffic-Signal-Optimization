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

from operators import genetic_algorithm, hill_climbing, simulated_annealing, crossover, mutation

parser = argparse.ArgumentParser()
parser.add_argument('--algorithm', required=True, choices=['ga', 'hc', 'sa'], help='Algorithm to use for optimization - Genetic Algorithm, Hill Climbing, Simulated Annealing.')
parser.add_argument('--data', default='d', type=str, help='Input data.')
parser.add_argument('--population', default=100, type=int, help='Number of individuals in a population.')
parser.add_argument('--generations', default=100, type=int, help='Number of generations.')
parser.add_argument('--crossover', default=0.5, type=float, help='Crossover probability.')
parser.add_argument('--mutation', default=0.2, type=float, help='Mutation probability.')
parser.add_argument('--indpb', default=0.05, type=float, help='Probability of mutating each bit.')
parser.add_argument('--order_init', default='random', choices=['adaptive', 'random', 'default'], help='Method for initializing the order of streets.')
parser.add_argument('--times_init', default='default', choices=['scaled', 'default'], help='Method for initializing green light durations.')
parser.add_argument('--threads', default=None, type=int, help='Number of threads for parallel evaluation.')
parser.add_argument('--seed', default=42, type=int, help='Random seed.')
parser.add_argument('--no-save', default=False, action='store_true', help='Do not save results and plots.')

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

        self._toolbox.register('select', tools.selTournament, tournsize=3)
        self._toolbox.register('evaluate', self._evaluate)
        self._toolbox.register('mate', crossover)
        
        green_min = 0
        green_max = self.plan.duration
        self._toolbox.register('mutate', mutation, indpb=self._args.indpb, low=green_min, up=green_max)

        norm_score = partial(normalized_score, data=self._args.data)
        self._stats.register('norm_max', lambda x: norm_score(np.max(x)))
        # This weird manipulation is necessary in order to avoid scientific notation
        # and use thousand separator when printing statistics with tools.Statistics
        self._stats.register('max', lambda x: f'{int(np.max(x)):,}')
        self._stats.register('norm_avg', lambda x: norm_score(np.mean(x)))
        self._stats.register('avg', lambda x: f'{int(np.mean(x)):,}')

    def _create_individual(self, simulation):
        if self._args.order_init == "random":
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
        gen, norm_max, max, norm_avg, avg = self._logbook.select(
            'gen', 'norm_max', 'max', 'norm_avg', 'avg'
        )

        to_int = lambda x: int(x.replace(',', ''))
        max, avg = (list(map(to_int, stat)) for stat in [max, avg])

        df = pd.DataFrame({
            'gen': gen, 'norm_max': norm_max, 'max': max, 'norm_avg': norm_avg,
            'avg': avg
        })
        df.to_csv(os.path.join(logdir, f'{self._args.data}.csv'), index=False)


        fig, ax1 = plt.subplots()
        ax1.plot(gen, norm_max, label='max score in a generation')
        ax1.plot(gen, norm_avg, label='avg score in a generation')
        #ax1.plot([0, self._args.generations], [0, 0], 'm--', label='baseline')
        #ax1.plot([0, self._args.generations], [1, 1], 'b--', label='max known score')
        ax1.fill_between(gen, norm_max, alpha=0.25)
        ax1.set_xlabel('Number of generations')
        ax1.set_ylabel('% of max known score')
        y_min = np.min([0, np.min(norm_max)])
        y_max = np.max([1, np.max(norm_max)])
        ax1.set_ylim(y_min, y_max)
        ax1.legend(framealpha=0.5)
        best_fitness = np.max(max)
        ax1.set_title(f'Best score: {best_fitness:,} ({100 * normalized_score(best_fitness, self._args.data):.2f} %)')

        baseline = DEFAULT_SCORE[self._args.data]
        best_known = MAX_KNOWN_SCORE[self._args.data]

        ax2 = ax1.twinx()
        ax2.set_ylabel('Score')
        y_min = np.min([baseline, np.min(max)])
        y_max = np.max([best_known, np.max(max)])
        ax2.set_ylim(y_min, y_max)

        ticks = np.linspace(y_min, y_max, num=6)
        labels = map(lambda x: f'{int(x):,}', ticks)
        ax2.set_yticks(ticks, labels) #, rotation=45

        # Avoid scientific notation
        #yticks = plt.gca().get_yticks()
        #plt.gca().yaxis.set_major_locator(ticker.FixedLocator(yticks))
        #plt.gca().set_yticklabels([f'{x:,.0f}' for x in yticks])#, rotation=45)

        fig.tight_layout()
        fig.savefig(os.path.join(logdir, f'{self._args.data}.pdf'), format='pdf')
        fig.savefig(os.path.join(logdir, f'{self._args.data}.svg'), format='svg')

        if show_plot:
            plt.show()

    def _save_best_schedules(self, logdir):
        best_individual = self._hof.items[0]
        simulation = default_simulation(self.plan)
        # Individual is in the relative_order format
        simulation.set_non_trivial_schedules(best_individual, relative_order=True)
        simulation.save_schedules(os.path.join(logdir, f'{self._args.data}.out'))

    def _save_info(self, logdir):
        best_fitness = self._hof.keys[0].values[0]
        with open(os.path.join(logdir, 'info.txt'), 'w') as f:
            f.write(f'Best fitness: {int(best_fitness):,} ({100 * normalized_score(best_fitness, self._args.data):.2f} %)\n\n')
            for k, v in self._args.__dict__.items():
                f.write(f'{k}={v}\n')
            f.write(f'\nElapsed time: {self._elapsed_time}\n')

    def save_statistics(self, show_plot=False):
        if self._logbook is None:
            print('Run the optimizer first!')
            return

        logdir = os.path.join('logs', self._args.data, '{}-{}-{}'.format(
            os.path.basename(globals().get('__file__', 'notebook')),
            datetime.datetime.now().strftime('%Y-%m-%d_%H%M%S'),
            ','.join(('{}={}'.format(re.sub('(.)[^_]*_?', r'\1', k), v) for k, v in sorted(vars(self._args).items())))
        ))
        os.makedirs(logdir, exist_ok=True)
        self._save_data_plots(logdir, show_plot)
        self._save_best_schedules(logdir)
        self._save_info(logdir)

    def run(self):
        start = time.time()
        kwargs = {
            'stats': self._stats,
            'halloffame': self._hof,
            'verbose': True
        }
        if self._args.algorithm == 'ga':
            population = self._toolbox.population(n=self._args.population)
            print(f'Population created: {time.time() - start:.4f}s')

            _, self._logbook = genetic_algorithm(
                population, self._toolbox, self._args.crossover, self._args.mutation, self._args.generations, **kwargs
            )
        elif self._args.algorithm == 'hc':
            _, self._logbook = hill_climbing(
                self._toolbox.individual(), self._toolbox, self._args.generations, **kwargs
            )
        elif self._args.algorithm == 'sa':
            _, self._logbook = simulated_annealing(
                self._toolbox.individual(), self._toolbox, self._args.generations, **kwargs
            )
        self._elapsed_time = datetime.timedelta(seconds=int(time.time() - start))
        print(f'Elapsed time: {self._elapsed_time}')

        best_fitness = self._hof.keys[0].values[0]
        print(f'Best fitness: {int(best_fitness):,} ({100 * normalized_score(best_fitness, self._args.data):.2f} %)')


if __name__ == '__main__':
    args = parser.parse_args()
    optimizer = Optimizer(args)
    optimizer.run()
    if not args.no_save:
        optimizer.save_statistics()