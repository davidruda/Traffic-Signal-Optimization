import random
from collections import namedtuple

import numpy as np
from deap import base, creator, tools, algorithms
from traffic_signaling.city_plan import *
from traffic_signaling.simulation import *

random.seed(42)

city_plan = CityPlan("traffic_signaling/input_data/b.txt")
simulation = Simulation(city_plan)
simulation.create_plan_default()

schedules = [simulation.schedules[intersection.id] for intersection in city_plan.intersections if
             intersection.non_trivial]

Schedule = namedtuple('Schedule', ['times', 'order'])

# Intersection details
max_green_time = 2

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
        return [random.randint(0, max_green_time) for _ in range(num_streets)]
        # return [random.randint(1, 1) for _ in range(num_streets)]

    # Function to generate a random order for an intersection
    def get_random_order(num_streets):
        return random.sample(range(num_streets), num_streets)

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
            tools.cxTwoPoint(ind1[i].times, ind2[i].times)
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

    # Create the initial population
    population = toolbox.population(n=population_size)

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
    stats.register('max', np.max)
    stats.register('min', np.min)
    stats.register('avg', np.mean)
    stats.register('std', np.std)

    hof = tools.HallOfFame(3)

    # Run the evolution using eaSimple
    algorithms.eaSimple(population, toolbox, cxpb=crossover_probability,
                        mutpb=mutation_probability, ngen=num_generations,
                        stats=stats, halloffame=hof, verbose=True)

    # Get the best individual from the population
    best_individual = max(population, key=lambda x: x.fitness.values[0])
    # print(population)

    print('Best individual:')
    # print_individual(best_individual)
    print(f'Fitness: {best_individual.fitness.values[0]:,}')

    # for item in hof:
    #    print(item.fitness, item)


if __name__ == '__main__':
    main()
