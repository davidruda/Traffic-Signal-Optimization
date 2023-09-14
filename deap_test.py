import argparse
import array
from collections import namedtuple, defaultdict
import datetime
import random
import re
import time
import threading

import numpy as np
from deap import base, creator, tools

from traffic_signaling.city_plan import *
from traffic_signaling.data import *
from traffic_signaling.simulation import *

parser = argparse.ArgumentParser()
parser.add_argument('--data', default='a', type=str, help='Input data.')
parser.add_argument('--population', default=100, type=int, help='Number of individuals in a population.')
parser.add_argument('--generations', default=100, type=int, help='Number of generations.')
parser.add_argument('--crossover', default=0.5, type=float, help='Crossover probability.')
parser.add_argument('--mutation', default=0.2, type=float, help='Mutation probability.')
parser.add_argument('--parallel', default=None, type=int, help='Number of threads for parallel evaluation.')
parser.add_argument('--seed', default=42, type=int, help='Random seed.')
parser.add_argument('--optimize_params', default='order', choices=['all', 'times', 'order'], help='Which parameters to optimize.')
parser.add_argument('--green_max', default=1, type=int, help='Maximum possible green time for a street.')
parser.add_argument('--green_min', default=0, type=int, help='Minimum possible green time for a street.')
parser.add_argument('--intersections', default=None, nargs='+', help='Which specific intersections to optimize (if not all).')

Pair = namedtuple('Pair', ['times', 'order'])

def varAnd(population, toolbox, cxpb, mutpb):
    offspring = [toolbox.clone(ind) for ind in population]

    # Apply crossover and mutation on the offspring
    for i in range(1, len(offspring), 2):
        if random.random() < cxpb:
            offspring[i - 1], offspring[i] = toolbox.mate(offspring[i - 1],
                                                          offspring[i])
            del offspring[i - 1].fitness.values, offspring[i].fitness.values

    for i in range(len(offspring)):
        if random.random() < mutpb:
            offspring[i], = toolbox.mutate(offspring[i])
            del offspring[i].fitness.values

    return offspring

def eaSimple(population, toolbox, cxpb, mutpb, ngen, stats=None,
             halloffame=None, verbose=__debug__):
    logbook = tools.Logbook()
    logbook.header = ['gen', 'nevals'] + (stats.fields if stats else [])

    start_evaluate = time.time()
    # Evaluate the individuals with an invalid fitness
    invalid_ind = [ind for ind in population if not ind.fitness.valid]
    fitnesses = toolbox.map(toolbox.evaluate, invalid_ind)
    for ind, fit in zip(invalid_ind, fitnesses):
        ind.fitness.values = fit
    print(f'Evaluation: {time.time() - start_evaluate:.4f}s')

    if halloffame is not None:
        halloffame.update(population)

    record = stats.compile(population) if stats else {}
    logbook.record(gen=0, nevals=len(invalid_ind), **record)
    if verbose:
        print(logbook.stream)

    # Begin the generational process
    for gen in range(1, ngen + 1):
        start = time.time()

        # Select the next generation individuals
        offspring = toolbox.select(population, len(population))

        start_mutate = time.time()
        # Vary the pool of individuals
        offspring = varAnd(offspring, toolbox, cxpb, mutpb)
        print(f'Cross+Mut: {time.time() - start_mutate:.4f}s')

        start_evaluate = time.time()
        # Evaluate the individuals with an invalid fitness
        invalid_ind = [ind for ind in offspring if not ind.fitness.valid]

        fitnesses = toolbox.map(toolbox.evaluate, invalid_ind)
        for ind, fit in zip(invalid_ind, fitnesses):
            ind.fitness.values = fit
        print(f'Evaluation: {time.time() - start_evaluate:.4f}s')

        # Update the hall of fame with the generated individuals
        if halloffame is not None:
            halloffame.update(offspring)

        # Replace the current population by the offspring
        population[:] = offspring

        # Append the current generation statistics to the logbook
        record = stats.compile(population) if stats else {}
        logbook.record(gen=gen, nevals=len(invalid_ind), **record)
        if verbose:
            print(logbook.stream)

        print(f'Generation {gen}: {time.time() - start:.4f}s')

    return population, logbook


def save_statistics(logbook, show_plot=False):
    import matplotlib.pyplot as plt
    import matplotlib.ticker as ticker
    import pandas as pd
    gen, max, min, avg, std = logbook.select('gen', 'max', 'min', 'avg', 'std')
    to_int = lambda x: int(x.replace(',', ''))

    max, min, avg, std = (list(map(to_int, stat)) for stat in [max, min, avg, std])

    os.makedirs(args.logdir, exist_ok=True)
    df = pd.DataFrame({'gen': gen, 'max': max, 'min': min, 'std': std})
    df.to_csv(os.path.join(args.logdir, f'{args.data}.csv'), index=False)

    plt.plot(gen, max, label='max')
    plt.plot(gen, min, label='min')
    plt.plot(gen, avg, label='avg')
    plt.xlabel('Number of generations')
    plt.ylabel('Score')
    plt.legend()
    plt.title(f'Best score: {np.max(max):,}')

    # Avoid scientific notation
    #yticks = plt.gca().get_yticks()
    #plt.gca().yaxis.set_major_locator(ticker.FixedLocator(yticks))
    #plt.gca().set_yticklabels([f'{x:,.0f}' for x in yticks])#, rotation=45)
    
    #plt.tight_layout()

    # Center the axes regardless of the labels
    # pos = plt.gca().get_position()
    # left = pos.x0
    # right = pos.x1
    # plt.subplots_adjust(right=(1 - left))

    plt.savefig(os.path.join(args.logdir, f'{args.data}.pdf'), format='pdf')

    if show_plot:
        plt.show()

def save_schedules(individual):
    simulation = Simulation(city_plan)
    simulation.default_schedules()
    simulation.update_schedules(individual)
    os.makedirs(args.logdir, exist_ok=True)
    simulation.write_schedules(os.path.join(args.logdir, f'{args.data}.txt'))

def default_simulation():
    s = Simulation(city_plan)
    s.default_schedules()
    return s

def evaluate(individual):
    simulation = simulations[threading.get_ident()]
    simulation.update_schedules(individual)
    fitness = simulation.score()
    return fitness,

def main(args):
    toolbox = base.Toolbox()

    if args.parallel and args.parallel > 0:
        import concurrent.futures
        pool = concurrent.futures.ThreadPoolExecutor(max_workers=args.parallel)
        toolbox.register('map', pool.map)

    # Create logdir name
    args.logdir = os.path.join('logs', args.data, '{}-{}-{}'.format(
        os.path.basename(globals().get('__file__', 'notebook')),
        datetime.datetime.now().strftime('%Y-%m-%d_%H%M%S'),
        ','.join(('{}={}'.format(re.sub('(.)[^_]*_?', r'\1', k), v) for k, v in sorted(vars(args).items())))
    ))

    creator.create('FitnessMax', base.Fitness, weights=(1.0,))
    creator.create('Individual', dict, fitness=creator.FitnessMax)

    def random_times(length, min, max, weights=None):
        return array.array('I', random.choices(range(min, max + 1), weights, k=length))
        #return array.array('I', length * [random.randint(1, 1)])
    
    def random_order(length):
        return array.array('I', np.random.permutation(length))

    def create_individual():
        min = args.green_min if args.green_min > 0 else 1
        individual = {
            id: 
            Pair(
                random_times(length, min, args.green_max),
                random_order(length)
            )
            for id, length in non_trivial_intersections_lengths
        }
        return individual

    toolbox.register('individual', tools.initIterate, creator.Individual, create_individual)
    toolbox.register('population', tools.initRepeat, list, toolbox.individual)

    toolbox.register('evaluate', evaluate)

    def crossover(ind1, ind2):
        for id in ind1:
            if args.optimize_params != 'order':
                tools.cxTwoPoint(ind1[id].times, ind2[id].times)
            if args.optimize_params != 'times':
                tools.cxOrdered(ind1[id].order,  ind2[id].order)
        return ind1, ind2

    def mutation(individual, indpb):
        def mutation_change_by_one(individual, low, up, indpb):
            for i in range(len(individual)):
                if random.random() < indpb:
                    if random.random() < 0.5:
                        individual[i] = min(individual[i] + 1, up)
                    else:
                        individual[i] = max(individual[i] - 1, low)
            return individual,

        for times, order in individual.values():
            if args.optimize_params != 'order':
                mutation_change_by_one(times, args.green_min, args.green_max, indpb)
                #tools.mutUniformInt(times, args.green_min, args.green_max, indpb)
            if args.optimize_params != 'times':
                tools.mutShuffleIndexes(order, indpb)
        return individual,

    toolbox.register('mate', crossover)
    toolbox.register('mutate', mutation, indpb=0.05)

    toolbox.register('select', tools.selTournament, tournsize=3)

    start_pop = time.time()
    population = toolbox.population(n=args.population)
    print(f'Population created: {time.time() - start_pop:.4f}s')

    stats = tools.Statistics(lambda individual: individual.fitness.values)

    # This weird manipulation is necessary in order to avoid scientific notation
    # and use thousands separator when printing statistics with tools.Statistics
    stats.register('max', lambda x: f'{int(np.max(x)):,}')
    stats.register('min', lambda x: f'{int(np.min(x)):,}')
    stats.register('avg', lambda x: f'{int(np.mean(x)):,}')
    stats.register('std', lambda x: f'{int(np.std(x)):,}')

    hof = tools.HallOfFame(1)

    #population, logbook = algorithms.eaSimple(
    population, logbook = eaSimple(
        population, toolbox, cxpb=args.crossover, mutpb=args.mutation,
        ngen=args.generations, stats=stats, halloffame=hof, verbose=True
    )

    best_individual = hof.items[0]
    best_fitness = hof.keys[0].values[0]
    print(f'Best fitness: {int(best_fitness):,}')

    save_statistics(logbook)
    save_schedules(best_individual)

if __name__ == '__main__':
    args = parser.parse_args()

    random.seed(args.seed)
    np.random.seed(args.seed)

    start = time.time()
    city_plan = CityPlan(get_data_filename(args.data))

    simulations = defaultdict(default_simulation)

    streets = city_plan.streets
    non_trivial_intersections_lengths = [
        (i.id, len([s for s in i.incoming_streets if streets[s].used]))
        for i in city_plan.intersections if i.non_trivial
    ]

    main(args)
    with open(os.path.join(args.logdir, 'info.txt'), 'w') as f:
        for k, v in args.__dict__.items():
            f.write(f'{k}={v}\n')
        elapsed_time = datetime.timedelta(seconds=int(time.time() - start))
        f.write(f'\nElapsed time: {elapsed_time}\n')
