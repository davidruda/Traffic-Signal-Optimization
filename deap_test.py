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
import scipy.stats
from scipy.stats import rv_discrete, truncnorm

from traffic_signaling import *

from operators import eaSimple, crossover, mutation

parser = argparse.ArgumentParser()
parser.add_argument('--data', default='d', type=str, help='Input data.')
parser.add_argument('--population', default=100, type=int, help='Number of individuals in a population.')
parser.add_argument('--generations', default=100, type=int, help='Number of generations.')
parser.add_argument('--crossover', default=0.5, type=float, help='Crossover probability.')
parser.add_argument('--mutation', default=0.2, type=float, help='Mutation probability.')
parser.add_argument('--indpb', default=0.05, type=float, help='Probability of mutating each bit.')
parser.add_argument('--tournsize', default=3, type=int, help='Tournament size for selection.')
parser.add_argument('--threads', default=None, type=int, help='Number of threads for parallel evaluation.')
parser.add_argument('--seed', default=42, type=int, help='Random seed.')
parser.add_argument('--init_times', default='default', choices=['scaled', 'default'], help='Way of initializing green times.')

def normalize(x, min, max):
    """
    Normalize `x` to the range [0, 1] and round to 2 decimal places.
    """
    x_norm = (x - min) / (max - min)
    return np.round(x_norm, decimals=2)

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
    simulation.save_schedules(os.path.join(logdir, f'{args.data}.txt'))

def evaluate(individual, simulations: dict[int, Simulation]):
    simulation = simulations[threading.get_ident()]
    simulation.set_non_trivial_schedules(individual, relative_order=True)
    fitness = simulation.score()
    return fitness, 

def times_distribution(distribution, n):
    times = np.round(distribution.rvs(size=n)).astype(int, copy=False)
    return array('L', times)

def normalized_times(car_counts, min, max):
    times = car_counts + scipy.stats.norm().rvs(len(car_counts))
    times = np.clip(np.round(times), min, max).astype(int, copy=False)
    return array('L', times)

def scaled_times_squared(car_counts):
    #times = np.sqrt(car_counts).astype(int)
    #times = np.round(np.sqrt(car_counts / np.gcd.reduce(car_counts))).astype(int, copy=False)
    times = np.round(0.51 * np.sqrt(car_counts / np.min(car_counts))).astype(int, copy=False)
    #times = np.clip(np.round(0.51 * np.sqrt(car_counts / np.min(car_counts))).astype(int, copy=False), a_min=1, a_max=None)
    return array('L', times)
    
def scaled_times(car_counts):
    # scaling
    #car_counts -= np.min(car_counts) - 1
    times = np.round(car_counts / np.min(car_counts)).astype(int, copy=False)
    return array('L', times)

def random_times(length, min, max, weights=None):
    return array('L', random.choices(range(min, max + 1), weights, k=length))
    #return array('L', length * [random.randrange(min, max + 1)])

def random_order(length):
    return array('L', np.random.permutation(length))

def default_times(length):
    return array('L', length * [1])

#def create_individual():
#    individual = [
#        (
#            random_order(len(car_counts)),
#            times_distribution(len(car_counts))
#            #scaled_times_squared(car_counts) # scaled_times(car_counts) 
#            if args.init_times == 'scaled'
#            else default_times(len(car_counts))
#        )
#        for car_counts in non_trivial_intersections
#    ]
#    return individual

def create_individual(non_trivial_intersections, total_cars):
    individual = [
        (
            # order
            random_order(len(intersection.used_streets)),
            # times
            default_times(len(intersection.used_streets))
            if args.init_times == 'default' else
            #times_distribution(time_distributions[i], len(intersection.used_streets))
            #array('L', next(times_generators[i]))
            #normalized_times(total_cars[i])
            #array('L', total_cars[i])
            scaled_times_squared(total_cars[i])
        )
        for i, intersection in enumerate(non_trivial_intersections)
    ]
    return individual

def main(args):
    random.seed(args.seed)
    np.random.seed(args.seed)
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

    toolbox.register(
        'individual', tools.initIterate, creator.Individual,
        partial(create_individual, non_trivial_intersections=non_trivial_intersections, total_cars=total_cars)
    )
    toolbox.register('population', tools.initRepeat, list, toolbox.individual)

    toolbox.register('evaluate', evaluate, simulations=simulations)
    toolbox.register('mate', crossover)
    toolbox.register('mutate', mutation, indpb=args.indpb, low=green_min, up=green_max)
    toolbox.register('select', tools.selTournament, tournsize=args.tournsize)

    stats = tools.Statistics(lambda individual: individual.fitness.values)


    norm_score = partial(normalize, min=DEFAULT_SCORE[args.data], max=MAX_KNOWN_SCORE[args.data])
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

    #start = time.time()
    #time_distributions = np.empty(len(non_trivial_intersections), dtype=object)
    #for i, intersection in enumerate(non_trivial_intersections):
    #    car_stats = [street.total_cars for street in intersection.used_streets]
    #    loc = np.mean(car_stats)
    #    # Avoid division by zero
    #    scale = 1 if (scale := np.std(car_stats)) == 0 else scale
    #
    #    a, b = (green_min - loc) / scale, (green_max - loc) / scale
    #    time_distributions[i] = truncnorm(a, b, loc=loc, scale=scale)#, seed=args.seed)
    #    #distr = truncnorm(a, b, loc=loc, scale=scale)
    #    #x = np.linspace(green_min, green_max, 100)
    #    #x = np.linspace(distr.ppf(0.01), distr.ppf(0.99), 100)
    #    #import matplotlib.pyplot as plt
    #    #plt.plot(x, distr.pdf(x), lw=2, label='truncnorm')
    #    #start, stop = np.floor(x[0]), np.ceil(x[-1])
    #    #bins = np.clip(np.arange(start - 0.5, (stop + 1) + 0.5, 1), start, stop)
    #    ##plt.hist(distr.rvs(1000), density=True, bins='auto', alpha=0.2)
    #    #plt.hist(distr.rvs(10000), density=True, bins=bins, alpha=0.2)
    #    #plt.show()

    #print(f'Times created: {time.time() - start:.4f}s')
    #start = time.time()
    #times_generators = []
    #for i, intersection in enumerate(non_trivial_intersections):
    #    shape = args.population, len(intersection.used_streets)        
    #    batch = np.round(time_distributions[i].rvs(size=shape)).astype(int, copy=False)
    #    # normalized times test
    #    #batch = time_distributions[i].rvs(size=shape)
    #    #batch = np.round(np.sqrt(0.1 * batch / np.min(batch, axis=1).reshape(-1, 1))).astype(int, copy=False)
    #    times_generators.append(iter(batch))
    #print(f'Test x100: {time.time() - start:.4f}s')
    #start = time.time()
    #for _ in range(100):
    #    for i, intersection in enumerate(non_trivial_intersections):
    #        times_distribution(time_distributions[i], len(intersection.used_streets))            
    #print(f'Individual Times created: {time.time() - start:.4f}s')


    #if args.init_times == 'scaled':
    #    counts_normalized = np.sqrt(car_counts / np.min(car_counts)).astype(int)
    #    values, counts = np.unique(counts_normalized, return_counts=True)
    #    probabilities = counts / counts.sum()
    #    car_counts_distribution = rv_discrete(values=(values, probabilities))
    
    #start = time.time()
    #normal = scipy.stats.norm()
    #distributions = {}
    #times_generators = []
    #for car_counts in total_cars:
    #    shape = args.population, len(car_counts)
    #    batched_times = car_counts + normal.rvs(size=shape)
    #    #batched_times = np.clip(np.round(batched_times), green_min, green_max).astype(int, copy=False)
    #    print(np.min(batched_times), np.max(batched_times), np.mean(batched_times), np.std(batched_times))
    #    times_generators.append(iter(batched_times))
    #print(f'Test norm: {time.time() - start:.4f}s')

    main(args)
