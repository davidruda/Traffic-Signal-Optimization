import argparse
import array
from collections import namedtuple, defaultdict
import datetime
import random
import re
import time
import threading

from deap import base, creator, tools
import numpy as np
from scipy.stats import rv_discrete

from traffic_signaling.city_plan import *
from traffic_signaling.data import *
from traffic_signaling.simulation import *

parser = argparse.ArgumentParser()
parser.add_argument('--data', default='a', type=str, help='Input data.')
parser.add_argument('--population', default=100, type=int, help='Number of individuals in a population.')
parser.add_argument('-g', '--generations', default=100, type=int, help='Number of generations.')
parser.add_argument('--crossover', default=0.5, type=float, help='Crossover probability.')
parser.add_argument('--mutation', default=0.2, type=float, help='Mutation probability.')
#parser.add_argument('--mutation', default=0.5, type=float, help='Mutation probability.')
parser.add_argument('--indpb', default=0.05, type=float, help='Probability of mutating each bit.')
#parser.add_argument('--indpb', default=0.1, type=float, help='Probability of mutating each bit.')
parser.add_argument('--mut_bits', default=1, type=float, help='Expected number of mutated bits per intersection.')
parser.add_argument('--tournsize', default=3, type=int, help='Tournament size for selection.')
parser.add_argument('--parallel', default=None, type=int, help='Number of threads for parallel evaluation.')
parser.add_argument('--seed', default=42, type=int, help='Random seed.')
parser.add_argument('--init_times', default='scaled', choices=['scaled', 'default'], help='Way of initializing green times.')

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


def normalized_score(x):
    baseline = DEFAULT_SCORE[args.data]
    max_known = MAX_KNOWN_SCORE[args.data]
    normalized = (x - baseline) / (max_known - baseline)
    return np.round(normalized, decimals=2)

def save_statistics(logdir, logbook, show_plot=False):
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

    if show_plot:
        plt.show()

def save_schedules(logdir, individual):
    simulation = Simulation(city_plan)
    simulation.default_schedules()
    simulation.update_schedules(individual)
    os.makedirs(logdir, exist_ok=True)
    simulation.write_schedules(os.path.join(logdir, f'{args.data}.txt'))

def default_simulation():
    s = Simulation(city_plan)
    s.default_schedules()
    return s

def evaluate(individual):
    simulation = simulations[threading.get_ident()]
    simulation.update_schedules(individual)
    fitness = simulation.score()
    return fitness,

def crossover(ind1, ind2):
    for id in ind1:
        tools.cxTwoPoint(ind1[id].times, ind2[id].times)
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
        #indpb = mut_bits / len(times)
        mutation_change_by_one(times, args.green_min, args.green_max, indpb)
        #tools.mutUniformInt(times, args.green_min, args.green_max, indpb)
        tools.mutShuffleIndexes(order, indpb)
    return individual,

def times_distribution(n):
    times = car_counts_distribution.rvs(size=n)
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
    return array.array('I', length * [1])

def random_order(length):
    return array.array('I', np.random.permutation(length))

def create_individual():
    individual = {
        id: 
        Pair(
            times_distribution(len(car_counts))
            #scaled_times_squared(car_counts) # scaled_times(car_counts) 
            if args.init_times == 'scaled'
            else default_times(len(car_counts)),
            random_order(len(car_counts))
        )
        for id, car_counts in non_trivial_intersections
    }
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
    creator.create('Individual', dict, fitness=creator.FitnessMax)

    toolbox.register('individual', tools.initIterate, creator.Individual, create_individual)
    toolbox.register('population', tools.initRepeat, list, toolbox.individual)

    toolbox.register('evaluate', evaluate)

    toolbox.register('mate', crossover)
    toolbox.register('mutate', mutation, indpb=args.indpb)

    toolbox.register('select', tools.selTournament, tournsize=args.tournsize)

    stats = tools.Statistics(lambda individual: individual.fitness.values)

    stats.register('norm_max', lambda x: normalized_score(np.max(x)))
    # This weird manipulation is necessary in order to avoid scientific notation
    # and use thousands separator when printing statistics with tools.Statistics
    stats.register('max', lambda x: f'{int(np.max(x)):,}')
    stats.register('norm_avg', lambda x: normalized_score(np.mean(x)))
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

    save_statistics(logdir, logbook, show_plot=False)
    save_schedules(logdir, best_individual)

if __name__ == '__main__':
    args = parser.parse_args()

    random.seed(args.seed)
    np.random.seed(args.seed)
    

    city_plan = CityPlan(get_data_filename(args.data))

    simulations = defaultdict(default_simulation)

    cars = city_plan.cars
    streets = city_plan.streets
    intersections = city_plan.intersections

    car_counts = np.zeros(len(streets), dtype=int)
    for c in cars:
        car_counts[c.path[:-1]] += 1

    non_trivial_intersections = [
        (i.id, car_counts[[s for s in i.streets if streets[s].used]])
        for i in intersections if i.non_trivial
    ]

    non_trivial_street_ids = [
        s
        for i in intersections if i.non_trivial
        for s in i.streets if streets[s].used
    ]
 
    car_counts = car_counts[non_trivial_street_ids]
    counts_normalized = np.sqrt(car_counts / np.min(car_counts)).astype(int)
    values, counts = np.unique(counts_normalized, return_counts=True)
    probabilities = counts / counts.sum()
    car_counts_distribution = rv_discrete(values=(values, probabilities))

    args.green_max = city_plan.duration
    args.green_min = 0

    main(args)
