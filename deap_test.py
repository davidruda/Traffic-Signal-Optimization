import random
from collections import namedtuple
import time

import numpy as np
from deap import base, creator, tools, algorithms
from traffic_signaling.city_plan import *
from traffic_signaling.simulation import *

SEED = 42
random.seed(SEED)
np.random.seed(SEED)

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

    # Evaluate the individuals with an invalid fitness
    invalid_ind = [ind for ind in population if not ind.fitness.valid]
    fitnesses = toolbox.map(toolbox.evaluate, invalid_ind)
    for ind, fit in zip(invalid_ind, fitnesses):
        ind.fitness.values = fit

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

start_sim = time.time()
city_plan = CityPlan("traffic_signaling/data/d.txt")
simulation = Simulation(city_plan)
simulation.create_plan_default()
print(f'Simulation prepared: {time.time() - start_sim:.4f}s')

start_schedule = time.time()
schedules = simulation.schedules
non_trivial_schedules = [schedules[intersection.id] for intersection in city_plan.intersections if
                         intersection.non_trivial]

schedules = non_trivial_schedules
print(f'Schedules prepared: {time.time() - start_schedule:.4f}s')

Schedule = namedtuple('Schedule', ['times', 'order'])

# Intersection details
#max_green_time = 2
max_green_time = 1

# Genetic Algorithm parameters
population_size = 100
num_generations = 100
crossover_probability = 0.5
mutation_probability = 0.2


# Define the evaluation function (fitness function)
def evaluate_traffic_schedules(individual):
    for i, (times, order) in enumerate(individual):
        schedules[i].set_schedule(times, order)
    fitness = simulation.run().score()
    return fitness,


def main():
    # Create the fitness and individual classes
    creator.create("FitnessMax", base.Fitness, weights=(1.0,))
    creator.create("Individual", list, fitness=creator.FitnessMax)

    # Initialize the DEAP toolbox
    toolbox = base.Toolbox()

    # Function to generate random times for an intersection
    def get_random_times(num_streets):
        #return [np.random.choice([0, 1], p=[0.05, 0.95]) for _ in range(num_streets)]
        #return [random.randint(0, max_green_time) for _ in range(num_streets)]
        #return np.random.choice([1], size=num_streets)
        return [random.randint(1, 1) for _ in range(num_streets)]
    
    def get_random_order(num_streets):
        #return np.random.permutation(num_streets)
        return list(np.random.permutation(num_streets))

    # Create a function to generate an individual
    def create_individual():
        return [Schedule(get_random_times(s.length), get_random_order(s.length)) for s in schedules]

    # Define the individual and population creation functions
    toolbox.register("individual", tools.initIterate, creator.Individual, create_individual)
    toolbox.register("population", tools.initRepeat, list, toolbox.individual)

    # Register the evaluation function
    toolbox.register("evaluate", evaluate_traffic_schedules)

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
    toolbox.register("mate", crossover)
    toolbox.register("mutate", mutation, indpb=0.05)

    # Define the selection operator
    toolbox.register("select", tools.selTournament, tournsize=3)

    start_pop = time.time()
    # Create the initial population
    population = toolbox.population(n=population_size)
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

    hof = tools.HallOfFame(3)

    print(f'eaSimple START: {time.time() - start_sim:.4f}s')
    # Run the evolution using eaSimple
    population, logbook = eaSimple(population, toolbox, cxpb=crossover_probability,
                                   mutpb=mutation_probability, ngen=num_generations,
                                   stats=stats, halloffame=hof, verbose=True)
    #population, logbook = algorithms.eaSimple(population, toolbox, cxpb=crossover_probability,
    #                                          mutpb=mutation_probability, ngen=num_generations,
    #                                          stats=stats, halloffame=hof, verbose=True)

    # Get the best individual from the population
    best_individual = max(population, key=lambda x: x.fitness.values[0])

    #print('Best individual:')
    # print_individual(best_individual)
    print(f'Best fitness: {int(best_individual.fitness.values[0]):,}')

    # for item in hof:
    #    print(item.fitness, item)

if __name__ == '__main__':
    main()
