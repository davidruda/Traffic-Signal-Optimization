import argparse
import array
import datetime
import random
import re
import time
from collections import namedtuple, defaultdict
from functools import partial

import numpy as np
from deap import base, creator, tools
from traffic_signaling.city_plan import *
from traffic_signaling.data import *
from traffic_signaling.simulation import *


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


def simulation_factory(city_plan):
    s = Simulation(city_plan)
    s.create_plan_default()
    return s

Pair = namedtuple('Pair', ['times', 'order'])

def evaluate_traffic_schedules(individual, city_plan, simulations, schedule_ids):
    simulation = simulations[os.getpid()]
    schedules = simulation.schedules
    for i, (times, order) in enumerate(individual):
        schedules[schedule_ids[i]].set_schedule(times, order)
    fitness = simulation.score()
    return fitness,

def main(args):
    toolbox = base.Toolbox()

    if args.parallel:
        import multiprocessing
        pool = multiprocessing.Pool(processes=args.parallel)
        toolbox.register('map', pool.map)

    # Create logdir name
    args.logdir = os.path.join('logs', '{}-{}-{}'.format(
        os.path.basename(globals().get('__file__', 'notebook')),
        datetime.datetime.now().strftime('%Y-%m-%d_%H%M%S'),
        ','.join(('{}={}'.format(re.sub('(.)[^_]*_?', r'\1', k), v) for k, v in sorted(vars(args).items())))
    ))

    creator.create('FitnessMax', base.Fitness, weights=(1.0,))
    creator.create('Individual', list, fitness=creator.FitnessMax)

    def get_random_times(num_streets):
        #return [np.random.choice([0, 1], p=[0.05, 0.95]) for _ in range(num_streets)]
        #return [random.randint(0, max_green_time) for _ in range(num_streets)]
        #return np.random.choice([1], size=num_streets)
        return array.array('I', [random.randint(1, 1) for _ in range(num_streets)])
        #return np.array([random.randint(1, 1) for _ in range(num_streets)])
    
    def get_random_order(num_streets):
        #return np.random.permutation(num_streets)
        return array.array('I', np.random.permutation(num_streets))
        #return np.random.permutation(num_streets)

    def create_individual(schedules):
        return [Pair(get_random_times(s.length), get_random_order(s.length)) for s in schedules]

    func = partial(create_individual, schedules=schedules)
    toolbox.register('individual', tools.initIterate, creator.Individual, func)
    toolbox.register('population', tools.initRepeat, list, toolbox.individual)

    func = partial(evaluate_traffic_schedules, city_plan=city_plan, simulations=simulations, schedule_ids=schedule_ids)
    toolbox.register('evaluate', func)

    def crossover(ind1, ind2):
        assert len(ind1) == len(ind2)
        for i in range(len(ind1)):
            #tools.cxTwoPoint(ind1[i].times, ind2[i].times)
            tools.cxOrdered(ind1[i].order, ind2[i].order)

        return ind1, ind2

    def mutation(individual, indpb):
        for times, order in individual:
            # for i in range(len(times)):
            #    if random.random() < indpb:
            #        if random.random() < 0.5:
            #            times[i] = min(times[i] + 1, max_green_time)
            #        else:
            #            times[i] = max(times[i] - 1, 0)
            tools.mutShuffleIndexes(order, indpb)
        return individual,

    # Define the genetic operators (crossover and mutation)
    toolbox.register('mate', crossover)
    toolbox.register('mutate', mutation, indpb=0.05)

    # Define the selection operator
    toolbox.register('select', tools.selTournament, tournsize=3)

    start_pop = time.time()
    # Create the initial population
    population = toolbox.population(n=args.population)
    print(f'Population created: {time.time() - start_pop:.4f}s')

    stats = tools.Statistics(lambda individual: individual.fitness.values)

    def print_population(population):
        for i, individual in enumerate(population):
            print(f'Individual {i}:')
            print_individual(individual)
        print()

    def print_individual(individual):
        for i, (times, order) in enumerate(individual):
            print(f'Street {i}: {times} {order}')

    # stats.register('print_population', print_population)
    stats.register('max', lambda x: f'{int(np.max(x)):,}')
    stats.register('min', lambda x: f'{int(np.min(x)):,}')
    stats.register('avg', lambda x: f'{int(np.mean(x)):,}')
    stats.register('std', lambda x: f'{int(np.std(x)):,}')

    hof = tools.HallOfFame(1)

    print(f'eaSimple START: {time.time() - start_sim:.4f}s')
    # Run the evolution using eaSimple
    population, logbook = eaSimple(population, toolbox, cxpb=args.crossover,
                                   mutpb=args.mutation, ngen=args.generations,
                                   stats=stats, halloffame=hof, verbose=True)
    #population, logbook = algorithms.eaSimple(population, toolbox, cxpb=args.crossover,
    #                                          mutpb=args.mutation, ngen=args.generations,
    #                                          stats=stats, halloffame=hof, verbose=True)

    def plot_optimization(logbook, show=False):
        import matplotlib.pyplot as plt
        import matplotlib.ticker as ticker
        gen, max, min, avg = logbook.select('gen', 'max', 'min', 'avg')
        to_int = lambda x: int(x.replace(',', ''))

        max, min, avg = (list(map(to_int, stats)) for stats in [max, min, avg])
        plt.plot(gen, max, label='max')
        plt.plot(gen, min, label='min')
        plt.plot(gen, avg, label='avg')
        plt.xlabel('Number of generations')
        plt.ylabel('Score')
        plt.legend()
        plt.title(f'Best score: {np.max(max):,}')

        yticks = plt.gca().get_yticks()
        plt.gca().yaxis.set_major_locator(ticker.FixedLocator(yticks))
        plt.gca().set_yticklabels([f'{x:,.0f}' for x in yticks])#, rotation=45)
        plt.tight_layout()

        os.makedirs(args.logdir, exist_ok=True)
        plt.savefig(os.path.join(args.logdir, f'{args.data}.pdf'), format='pdf')
        
        if show:
            plt.show()

    plot_optimization(logbook)

    def save_plan(individual):
        simulation = Simulation(city_plan)
        simulation.create_plan_default()
        schedules = simulation.schedules
        for i, (times, order) in enumerate(individual):
            schedules[schedule_ids[i]].set_schedule(times, order)

        os.makedirs(args.logdir, exist_ok=True)
        simulation.write_plan(os.path.join(args.logdir, f'{args.data}.out.txt'))

    best_individual = hof.items[0]
    best_fitness = hof.keys[0].values[0]

    save_plan(best_individual)
    print(f'Best fitness: {int(best_fitness):,}')

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--data', default='a', type=str, help='Input data.')
    parser.add_argument('--population', default=100, type=int, help='Number of individuals in a population.')
    parser.add_argument('--generations', default=100, type=int, help='Number of generations.')
    parser.add_argument('--crossover', default=0.5, type=float, help='Crossover probability.')
    parser.add_argument('--mutation', default=0.2, type=float, help='Mutation probability.')
    parser.add_argument('--parallel', default=None, type=int, help='Number of processes for parallel computation.')
    parser.add_argument('--seed', default=42, type=int, help='Random seed.')
    args = parser.parse_args()

    random.seed(args.seed)
    np.random.seed(args.seed)
    
    # Intersection details
    #max_green_time = 2
    max_green_time = 1

    start_sim = time.time()
    city_plan = CityPlan(get_data_filename(args.data))
    simulation = Simulation(city_plan)
    simulation.create_plan_default()
    print(f'Simulation prepared: {time.time() - start_sim:.4f}s')

    func = partial(simulation_factory, city_plan=city_plan)
    simulations = defaultdict(func)

    start_schedule = time.time()
    schedules = simulation.schedules
    non_trivial_schedules = [schedules[intersection.id] for intersection in city_plan.intersections if
                             intersection.non_trivial]
    schedule_ids = [intersection.id for intersection in city_plan.intersections if
                    intersection.non_trivial]
    schedules = non_trivial_schedules
    print(f'Schedules prepared: {time.time() - start_schedule:.4f}s')

    main(args)
    