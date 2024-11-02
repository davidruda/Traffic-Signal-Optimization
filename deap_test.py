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

from operators import eaSimple, hill_climbing, simulated_annealing, crossover, mutation

parser = argparse.ArgumentParser()
parser.add_argument('--algorithm', required=True, choices=['ga', 'hc', 'sa'], help='Algorithm to use for optimization - Genetic Algorithm, Hill Climbing, Simulated Annealing.')
parser.add_argument('--data', default='d', type=str, help='Input data.')
parser.add_argument('--population', default=100, type=int, help='Number of individuals in a population.')
parser.add_argument('--generations', default=100, type=int, help='Number of generations.')
parser.add_argument('--crossover', default=0.5, type=float, help='Crossover probability.')
parser.add_argument('--mutation', default=0.2, type=float, help='Mutation probability.')
parser.add_argument('--indpb', default=0.05, type=float, help='Probability of mutating each bit.')
parser.add_argument('--order_init', default='random', choices=['adaptive', 'random', 'default'], help='Way of initializing order of streets.')
parser.add_argument('--times_init', default='default', choices=['scaled', 'default'], help='Way of initializing green times.')
parser.add_argument('--threads', default=None, type=int, help='Number of threads for parallel evaluation.')
parser.add_argument('--seed', default=42, type=int, help='Random seed.')
parser.add_argument('--no-save', default=False, action='store_true', help='Do not save results and plots.')

def save_data_plots(args, logdir, logbook, show_plot=False):
    import matplotlib.pyplot as plt
    import matplotlib.ticker as ticker
    import pandas as pd
    gen, norm_max, max, norm_avg, avg = logbook.select(
        'gen', 'norm_max', 'max', 'norm_avg', 'avg'
    )

    to_int = lambda x: int(x.replace(',', ''))
    max, avg = (list(map(to_int, stat)) for stat in [max, avg])

    os.makedirs(logdir, exist_ok=True)
    df = pd.DataFrame({
        'gen': gen, 'norm_max': norm_max, 'max': max, 'norm_avg': norm_avg,
        'avg': avg
    })
    df.to_csv(os.path.join(logdir, f'{args.data}.csv'), index=False)


    fig, ax1 = plt.subplots()
    ax1.plot(gen, norm_max, label='max score in a generation')
    ax1.plot(gen, norm_avg, label='avg score in a generation')
    #ax1.plot([0, args.generations], [0, 0], 'm--', label='baseline')
    #ax1.plot([0, args.generations], [1, 1], 'b--', label='max known score')
    ax1.fill_between(gen, norm_max, alpha=0.25)
    ax1.set_xlabel('Number of generations')
    ax1.set_ylabel('% of max known score')
    y_min = np.min([0, np.min(norm_max)])
    y_max = np.max([1, np.max(norm_max)])
    ax1.set_ylim(y_min, y_max)
    ax1.legend(framealpha=0.5)
    best_fitness = np.max(max)
    ax1.set_title(f'Best score: {best_fitness:,} ({100 * normalized_score(best_fitness, args.data):.2f} %)')

    baseline = DEFAULT_SCORE[args.data]
    best_known = MAX_KNOWN_SCORE[args.data]

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
    fig.savefig(os.path.join(logdir, f'{args.data}.pdf'), format='pdf')
    fig.savefig(os.path.join(logdir, f'{args.data}.svg'), format='svg')

    if show_plot:
        plt.show()

def save_schedules(logdir, plan, individual):
    simulation = default_simulation(plan)
    # Individual is in the relative_order format
    simulation.set_non_trivial_schedules(individual, relative_order=True)
    os.makedirs(logdir, exist_ok=True)
    simulation.save_schedules(os.path.join(logdir, f'{args.data}.out'))

def save_info(args, logdir, elapsed_time, best_fitness):
    os.makedirs(logdir, exist_ok=True)
    with open(os.path.join(logdir, 'info.txt'), 'w') as f:
        f.write(f'Best fitness: {int(best_fitness):,} ({100 * normalized_score(best_fitness, args.data):.2f} %)\n\n')
        for k, v in args.__dict__.items():
            f.write(f'{k}={v}\n')
        f.write(f'\nElapsed time: {elapsed_time}\n')

def evaluate(individual, simulations: dict[int, Simulation]):
    # Keep reusing the same simulation object for the same thread
    simulation = simulations[threading.get_ident()]
    # Individual is in the relative_order format
    simulation.set_non_trivial_schedules(individual, relative_order=True)
    fitness = simulation.score()
    return fitness,

def create_individual(args: argparse.Namespace, simulation: Simulation):
    if args.order_init == "random":
        # For random order initialization, we need to create new schedules each time
        simulation.create_schedules(order=args.order_init, times=args.times_init)
    # Relative order is required for the operators (order crossover) to work correctly
    schedules = simulation.non_trivial_schedules(relative_order=True)

    # Convert schedules from lists to array.arrays to speed up the optimization
    individual = [
        (array('L', order), array('L', times)) for order, times in schedules
    ]
    return individual

def main(args):
    random.seed(args.seed)
    np.random.seed(args.seed)
    # also set seed for the simulation module
    set_seed(args.seed)

    toolbox = base.Toolbox()

    if args.threads:
        import concurrent.futures
        pool = concurrent.futures.ThreadPoolExecutor(max_workers=args.threads)
        toolbox.register('map', pool.map)

    logdir = os.path.join('logs', args.data, '{}-{}-{}'.format(
        os.path.basename(globals().get('__file__', 'notebook')),
        datetime.datetime.now().strftime('%Y-%m-%d_%H%M%S'),
        ','.join(('{}={}'.format(re.sub('(.)[^_]*_?', r'\1', k), v) for k, v in sorted(vars(args).items())))
    ))

    plan = create_city_plan(args.data)
    default_sim = partial(default_simulation, city_plan=plan)
    simulations = defaultdict(default_sim)

    green_max = plan.duration
    green_min = 0

    creator.create('FitnessMax', base.Fitness, weights=(1.0,))
    creator.create('Individual', list, fitness=creator.FitnessMax)

    sim = Simulation(plan)
    sim.create_schedules(order=args.order_init, times=args.times_init)

    toolbox.register(
        'individual', tools.initIterate, creator.Individual,
        partial(create_individual, args=args, simulation=sim)
    )
    toolbox.register('population', tools.initRepeat, list, toolbox.individual)

    toolbox.register('evaluate', evaluate, simulations=simulations)
    toolbox.register('mate', crossover)
    toolbox.register('mutate', mutation, indpb=args.indpb, low=green_min, up=green_max)
    toolbox.register('select', tools.selTournament, tournsize=3)

    stats = tools.Statistics(lambda individual: individual.fitness.values)

    norm_score = partial(normalized_score, data=args.data)
    stats.register('norm_max', lambda x: norm_score(np.max(x)))
    # This weird manipulation is necessary in order to avoid scientific notation
    # and use thousand separator when printing statistics with tools.Statistics
    stats.register('max', lambda x: f'{int(np.max(x)):,}')
    stats.register('norm_avg', lambda x: norm_score(np.mean(x)))
    stats.register('avg', lambda x: f'{int(np.mean(x)):,}')

    hof = tools.HallOfFame(1)

    start = time.time()

    if args.algorithm == 'ga':
        population = toolbox.population(n=args.population)
        print(f'Population created: {time.time() - start:.4f}s')

        _, logbook = eaSimple(
            population, toolbox, cxpb=args.crossover, mutpb=args.mutation,
            ngen=args.generations, stats=stats, halloffame=hof, verbose=True
        )
    elif args.algorithm == 'hc':
        _, logbook = hill_climbing(
            toolbox.individual(), toolbox, ngen=args.generations, stats=stats, halloffame=hof, verbose=True
        )
    elif args.algorithm == 'sa':
        _, logbook = simulated_annealing(
            toolbox.individual(), toolbox, ngen=args.generations, stats=stats, halloffame=hof, verbose=True
        )
    elapsed_time = datetime.timedelta(seconds=int(time.time() - start))
    print(f'Elapsed time: {elapsed_time}')

    best_individual = hof.items[0]
    best_fitness = hof.keys[0].values[0]
    print(f'Best fitness: {int(best_fitness):,} ({100 * normalized_score(best_fitness, args.data):.2f} %)')

    if not args.no_save:
        save_data_plots(args, logdir, logbook, show_plot=False)
        save_schedules(logdir, plan, best_individual)
        save_info(args, logdir, elapsed_time, best_fitness)

if __name__ == '__main__':
    args = parser.parse_args()
    main(args)
