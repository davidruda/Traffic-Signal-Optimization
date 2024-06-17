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

from operators import eaSimple, crossover, mutation

parser = argparse.ArgumentParser()
parser.add_argument('--data', default='d', type=str, help='Input data.')
parser.add_argument('--population', default=100, type=int, help='Number of individuals in a population.')
parser.add_argument('--generations', default=100, type=int, help='Number of generations.')
parser.add_argument('--crossover', default=0.5, type=float, help='Crossover probability.')
parser.add_argument('--mutation', default=0.2, type=float, help='Mutation probability.')
parser.add_argument('--indpb', default=0.05, type=float, help='Probability of mutating each bit.')
parser.add_argument('--threads', default=None, type=int, help='Number of threads for parallel evaluation.')
parser.add_argument('--seed', default=42, type=int, help='Random seed.')
parser.add_argument('--order_init', default='random', choices=['adaptive', 'random'], help='Way of initializing order of streets.')
parser.add_argument('--times_init', default='default', choices=['scaled', 'default'], help='Way of initializing green times.')

def save_statistics(args, logdir, logbook, show_plot=False):
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
    ax1.set_title(f'Best score: {np.max(max):,}')

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
    simulation.set_non_trivial_schedules(individual, relative_order=True)
    os.makedirs(logdir, exist_ok=True)
    simulation.save_schedules(os.path.join(logdir, f'{args.data}.out'))

def evaluate(individual, simulations: dict[int, Simulation]):
    simulation = simulations[threading.get_ident()]
    simulation.set_non_trivial_schedules(individual, relative_order=True)
    fitness = simulation.score()
    return fitness,

#def scaled_times_squared(car_counts):
#    #times = np.sqrt(car_counts).astype(int)
#    #times = np.round(np.sqrt(car_counts / np.gcd.reduce(car_counts))).astype(int, copy=False)
#    times = np.round(0.51 * np.sqrt(car_counts / np.min(car_counts))).astype(int, copy=False)
#    #times = np.clip(np.round(0.51 * np.sqrt(car_counts / np.min(car_counts))).astype(int, copy=False), a_min=1, a_max=None)
#    return array('L', times)
#    
#def scaled_times(car_counts):
#    # scaling
#    #car_counts -= np.min(car_counts) - 1
#    times = np.round(car_counts / np.min(car_counts)).astype(int, copy=False)

def scaled_times(car_counts, divisor):
    times = np.clip(np.round(car_counts / divisor).astype(int, copy=False), a_min=1, a_max=None)
    return array('L', times)

def create_individual(args, simulation: Simulation, total_cars):
    if args.order_init == "random":
        simulation.random_schedules()
    schedules = simulation.non_trivial_schedules(relative_order=True)
    individual = [
        (
            # order
            array('L', order),
            # times
            scaled_times(total_cars[i], divisor=330)
            if args.times_init == 'scaled' else
            array('L', times)
        )
        for i, (order, times) in enumerate(schedules)
    ]
    return individual

def main(args):
    random.seed(args.seed)
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

    cars = plan.cars
    streets = plan.streets
    intersections = plan.intersections
    non_trivial_intersections = plan.non_trivial_intersections()

    green_max = plan.duration
    green_min = 0

    total_cars = [
        np.array([street.total_cars for street in intersection.used_streets])
        for intersection in non_trivial_intersections
    ]

    creator.create('FitnessMax', base.Fitness, weights=(1.0,))
    creator.create('Individual', list, fitness=creator.FitnessMax)

    if args.order_init == 'adaptive':
        sim = adaptive_simulation(plan)
    elif args.order_init == 'random':
        sim = Simulation(plan)

    toolbox.register(
        'individual', tools.initIterate, creator.Individual,
        partial(create_individual, args=args, simulation=sim, total_cars=total_cars)
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
    # and use thousands separator when printing statistics with tools.Statistics
    stats.register('max', lambda x: f'{int(np.max(x)):,}')
    stats.register('norm_avg', lambda x: norm_score(np.mean(x)))
    stats.register('avg', lambda x: f'{int(np.mean(x)):,}')

    hof = tools.HallOfFame(1)

    start = time.time()
    population = toolbox.population(n=args.population)
    print(f'Population created: {time.time() - start:.4f}s')

    population, logbook = eaSimple(
        population, toolbox, cxpb=args.crossover, mutpb=args.mutation,
        ngen=args.generations, stats=stats, halloffame=hof, verbose=True
    )
    os.makedirs(logdir, exist_ok=True)
    with open(os.path.join(logdir, 'info.txt'), 'w') as f:
        for k, v in args.__dict__.items():
            f.write(f'{k}={v}\n')
        elapsed_time = datetime.timedelta(seconds=int(time.time() - start))
        f.write(f'\nElapsed time: {elapsed_time}\n')

    best_individual = hof.items[0]
    best_fitness = hof.keys[0].values[0]
    print(f'Best fitness: {int(best_fitness):,}')

    save_statistics(args, logdir, logbook, show_plot=False)
    save_schedules(logdir, plan, best_individual)

if __name__ == '__main__':
    args = parser.parse_args()
    main(args)
