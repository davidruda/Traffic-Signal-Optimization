# time py deap_test.py --data d --generations 10 --parallel 16 --init_times default
import argparse
import array
from collections import defaultdict
from functools import partial
import datetime
import random
import re
import time
import threading

from deap import base, creator, tools, algorithms
import numpy as np
from scipy.stats import rv_discrete

from traffic_signaling.city_plan import *
from traffic_signaling.data import *
from traffic_signaling.simulation import *

#from operators import fill_with, crossover, mutation
from operators import eaSimple, crossover, mutation

parser = argparse.ArgumentParser()
parser.add_argument('--data', default='a', type=str, help='Input data.')
parser.add_argument('--population', default=100, type=int, help='Number of individuals in a population.')
parser.add_argument('-g', '--generations', default=100, type=int, help='Number of generations.')
parser.add_argument('--crossover', default=0.5, type=float, help='Crossover probability.')
parser.add_argument('--mutation', default=0.2, type=float, help='Mutation probability.')
#parser.add_argument('--mutation', default=0.5, type=float, help='Mutation probability.')
parser.add_argument('--indpb', default=0.05, type=float, help='Probability of mutating each bit.')
#parser.add_argument('--indpb', default=0.1, type=float, help='Probability of mutating each bit.')
parser.add_argument('--tournsize', default=3, type=int, help='Tournament size for selection.')
parser.add_argument('--parallel', default=None, type=int, help='Number of threads for parallel evaluation.')
parser.add_argument('--seed', default=42, type=int, help='Random seed.')
parser.add_argument('--init_times', default='default', choices=['scaled', 'default'], help='Way of initializing green times.')

def normalized_score(x, min, max):
    normalized = (x - min) / (max - min)
    return np.round(normalized, decimals=2)

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
    #simulation.update_schedules(individual)
    simulation.update_schedules(non_trivial_ids, individual)
    os.makedirs(logdir, exist_ok=True)
    simulation.save_schedules(os.path.join(logdir, f'{args.data}.txt'))

def evaluate(individual, simulations):
    simulation = simulations[threading.get_ident()]
    #simulation.update_schedules(individual)
    simulation.update_schedules(non_trivial_ids, individual)
    fitness = simulation.score()
    return fitness, 

def times_distribution(distribution, n):
    times = distribution.rvs(size=n)
    return array.array('I', times)

def scaled_times_squared(car_counts):
    #times = np.sqrt(car_counts).astype(int)
    times = np.sqrt(car_counts / np.min(car_counts)).astype(int)
    return array.array('I', times)
    
def scaled_times(car_counts):
    # scaling
    #car_counts -= np.min(car_counts) - 1
    times = np.round(car_counts / np.min(car_counts)).astype(int)
    #times = (car_counts / np.gcd.reduce(car_counts)).astype(int)
    return array.array('I', times)

def random_times(length, min, max, weights=None):
    return array.array('I', random.choices(range(min, max + 1), weights, k=length))
    #return array.array('I', length * [random.randint(1, 1)])
    
def default_times(length):
    #return np.ones(length, dtype=int)
    return array.array('I', length * [1])

def random_order(length):
    #return np.random.permutation(length)
    return array.array('I', np.random.permutation(length))

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

def create_individual():
    individual = [
        (
            # order
            random_order(len(i.used_streets)),
            # times
            default_times(len(i.used_streets))            
        )
        for i in non_trivial_intersections
    ]
    return individual

def main(args):
    toolbox = base.Toolbox()

    if args.parallel and args.parallel > 0:
        import concurrent.futures
        pool = concurrent.futures.ThreadPoolExecutor(max_workers=args.parallel)
        toolbox.register('map', pool.map)

    logdir = os.path.join('logs', args.data, '{}-{}-{}'.format(
        os.path.basename(globals().get('__file__', 'notebook')),
        datetime.datetime.now().strftime('%Y-%m-%d_%H%M%S'),
        ','.join(('{}={}'.format(re.sub('(.)[^_]*_?', r'\1', k), v) for k, v in sorted(vars(args).items())))
    ))

    creator.create('FitnessMax', base.Fitness, weights=(1.0,))
    #creator.create('Individual', dict, fitness=creator.FitnessMax)
    creator.create('Individual', list, fitness=creator.FitnessMax)

    toolbox.register('individual', tools.initIterate, creator.Individual, create_individual)
    toolbox.register('population', tools.initRepeat, list, toolbox.individual)

    toolbox.register('evaluate', evaluate, simulations=simulations)

    toolbox.register('mate', crossover)
    #toolbox.register('mutate', mutation, indpb=args.indpb)
    toolbox.register('mutate', mutation, indpb=args.indpb, low=args.green_min, up=args.green_max)

    toolbox.register('select', tools.selTournament, tournsize=args.tournsize)

    stats = tools.Statistics(lambda individual: individual.fitness.values)


    norm_score = partial(normalized_score, min=DEFAULT_SCORE[args.data], max=MAX_KNOWN_SCORE[args.data])
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

    #population, logbook = algorithms.eaSimple(
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

    random.seed(args.seed)
    np.random.seed(args.seed)

    plan = create_city_plan(args.data)
    default_sim = partial(default_simulation, city_plan=plan)
    simulations = defaultdict(default_sim)

    cars = plan.cars
    streets = plan.streets
    intersections = plan.intersections
    non_trivial_intersections = plan.non_trivial_intersections()

    car_counts = [
        streets[street_id].total_cars
        for i in non_trivial_intersections
        for street_id in i.used_streets
    ]
    
    non_trivial_ids = [i.id for i in non_trivial_intersections]

    if args.init_times == 'scaled':
        counts_normalized = np.sqrt(car_counts / np.min(car_counts)).astype(int)
        values, counts = np.unique(counts_normalized, return_counts=True)
        probabilities = counts / counts.sum()
        car_counts_distribution = rv_discrete(values=(values, probabilities))

    args.green_max = 1#plan.duration
    args.green_min = 0

    main(args)
