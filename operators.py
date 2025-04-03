from array import array
import math
import random
import time

import cython

from deap.base import Toolbox
from deap.tools import Logbook, Statistics, HallOfFame

# Compile this file with Cython to speed up the optimization
# cythonize -3ai operators.py

Individual = list[tuple[array, array]]

@cython.boundscheck(False)
@cython.wraparound(False)
@cython.initializedcheck(False)
def _fill_with(destination: Individual, source: Individual) -> None:
    """
    Fill `destination` with values from `source`.
    """
    order1: cython.ulong[:]
    times1: cython.ulong[:]
    order2: cython.ulong[:]
    times2: cython.ulong[:]

    for (order1, times1), (order2, times2) in zip(destination, source):
        for i in range(len(order1)):
            order1[i] = order2[i]
            times1[i] = times2[i]

@cython.boundscheck(False)
@cython.wraparound(False)
@cython.initializedcheck(False)
def _cxTwoPoint(ind1: cython.ulong[:], ind2: cython.ulong[:]):
    size: cython.Py_ssize_t = min(len(ind1), len(ind2))
    cxpoint1: cython.int = random.randint(1, size)
    cxpoint2: cython.int = random.randint(1, size - 1)

    if cxpoint2 >= cxpoint1:
        cxpoint2 += 1
    else:  # Swap the two cx points
        cxpoint1, cxpoint2 = cxpoint2, cxpoint1

    ind1[cxpoint1:cxpoint2], ind2[cxpoint1:cxpoint2] \
        = ind2[cxpoint1:cxpoint2], ind1[cxpoint1:cxpoint2]

    return ind1, ind2

@cython.boundscheck(False)
@cython.wraparound(False)
@cython.initializedcheck(False)
def _cxOrdered(ind1: cython.ulong[:], ind2: cython.ulong[:]):
    a: cython.int
    b: cython.int
    i: cython.int
    k1: cython.int
    k2: cython.int
    temp1: cython.ulong[:]
    temp2: cython.ulong[:]
    size: cython.Py_ssize_t = min(len(ind1), len(ind2))

    a, b = random.sample(range(size), 2)
    if a > b:
        a, b = b, a

    holes1, holes2 = [True] * size, [True] * size
    for i in range(size):
        if i < a or i > b:
            holes1[ind2[i]] = False
            holes2[ind1[i]] = False

    # We must keep the original values somewhere before scrambling everything
    temp1, temp2 = ind1, ind2
    k1, k2 = b + 1, b + 1
    for i in range(size):
        if not holes1[temp1[(i + b + 1) % size]]:
            ind1[k1 % size] = temp1[(i + b + 1) % size]
            k1 += 1

        if not holes2[temp2[(i + b + 1) % size]]:
            ind2[k2 % size] = temp2[(i + b + 1) % size]
            k2 += 1

    # Swap the content between a and b (included)
    for i in range(a, b + 1):
        ind1[i], ind2[i] = ind2[i], ind1[i]

    return ind1, ind2

@cython.boundscheck(False)
@cython.wraparound(False)
@cython.initializedcheck(False)
def mutation_change_by_one(individual: cython.ulong[:], indpb: cython.float, low: cython.uint, up: cython.uint):
    size: cython.Py_ssize_t = len(individual)

    for i in range(size):
        if random.random() < indpb:
            if random.random() < 0.5:
                # Make sure the number doesn't overflow
                if individual[i] < up:
                    individual[i] = individual[i] + 1
            else:
                # Make sure the number doesn't overflow
                if individual[i] > low:
                    individual[i] = individual[i] - 1
    return individual,

@cython.boundscheck(False)
@cython.wraparound(False)
@cython.initializedcheck(False)
def _mutShuffleIndexes(individual: cython.ulong[:], indpb: cython.float):
    swap_indx: cython.int
    size: cython.Py_ssize_t = len(individual)

    for i in range(size):
        if random.random() < indpb:
            swap_indx = random.randint(0, size - 2)
            if swap_indx >= i:
                swap_indx += 1
            individual[i], individual[swap_indx] = \
                individual[swap_indx], individual[i]

    return individual,

def _varAnd(
    population: list[Individual], pop2: list[Individual], toolbox: Toolbox, cxpb: float, mutpb: float
) -> list[Individual]:
    i: cython.int

    #offspring = [toolbox.clone(ind) for ind in population]
    for new, old in zip(pop2, population):
        _fill_with(new, old)
        new.fitness.values = old.fitness.values
    offspring = pop2

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

def _eaSimple(
    population: list[Individual], toolbox: Toolbox, cxpb: float, mutpb: float, ngen: int,
    stats: Statistics | None = None, halloffame: HallOfFame | None = None, verbose: bool | None = __debug__
) -> tuple[list[Individual], Logbook]:
    logbook = Logbook()
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

    pop2 = [toolbox.clone(ind) for ind in population]

    # Begin the generational process
    for gen in range(1, ngen + 1):
        start = time.time()

        # Select the next generation individuals
        offspring = toolbox.select(population, len(population))

        start_mutate = time.time()
        # Vary the pool of individuals
        offspring = _varAnd(offspring, pop2, toolbox, cxpb, mutpb)
        #offspring = varAnd(offspring, toolbox, cxpb, mutpb)
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
        #population[:] = offspring
        population, pop2 = offspring, population


        # Append the current generation statistics to the logbook
        record = stats.compile(population) if stats else {}
        logbook.record(gen=gen, nevals=len(invalid_ind), **record)
        if verbose:
            print(logbook.stream)

        print(f'Generation {gen}: {time.time() - start:.4f}s')

    return population, logbook

if cython.compiled:
    cxTwoPoint = _cxTwoPoint
    cxOrdered = _cxOrdered
    mutShuffleIndexes = _mutShuffleIndexes
    genetic_algorithm = _eaSimple
else:
    from deap.tools import cxTwoPoint, cxOrdered, mutShuffleIndexes
    from deap.algorithms import eaSimple as eaSimple_deap
    genetic_algorithm = eaSimple_deap

def crossover(ind1: Individual, ind2: Individual) -> tuple[Individual, Individual]:
    choice: cython.int

    for (order1, times1), (order2, times2) in zip(ind1, ind2):
        # crossover only order, or only times, or both
        choice = random.randint(1, 3)
        if choice & 0b01:
            cxOrdered(order1, order2)
        if choice & 0b10:
            cxTwoPoint(times1, times2)

    return ind1, ind2

def mutation(individual: Individual, indpb: float, low: int, up: int) -> tuple[Individual]:
    choice: cython.int

    for order, times in individual:
        # mutation only order, or only times, or both
        choice = random.randint(1, 3)
        if choice & 0b01:
            mutShuffleIndexes(order, indpb)
        if choice & 0b10:
            mutation_change_by_one(times, indpb, low, up)
    return individual,

# First-choice hill climbing
def hill_climbing(
    individual: Individual, toolbox: Toolbox, ngen: int, stats: Statistics | None = None,
    halloffame: HallOfFame | None = None, verbose: bool | None = __debug__
) -> tuple[Individual, Logbook]:
    logbook = Logbook()
    logbook.header = ['gen', 'nevals'] + (stats.fields if stats else [])

    individual.fitness.values = toolbox.evaluate(individual)

    if halloffame is not None:
        halloffame.update([individual])

    record = stats.compile([individual]) if stats else {}
    logbook.record(gen=0, nevals=1, **record)
    if verbose:
        print(logbook.stream)

    new_individual = toolbox.clone(individual)

    # Begin the generational process
    for gen in range(1, ngen + 1):
        nevals: int = 0
        start = time.time()
        _fill_with(new_individual, individual)

        new_individual, = toolbox.mutate(new_individual)

        new_individual.fitness.values = toolbox.evaluate(new_individual)

        if new_individual.fitness.values[0] > individual.fitness.values[0]:
            individual, new_individual = new_individual, individual
            nevals += 1

        # Update the hall of fame with the generated individuals
        if halloffame is not None:
            halloffame.update([individual])

        # Append the current generation statistics to the logbook
        record = stats.compile([individual]) if stats else {}
        logbook.record(gen=gen, nevals=nevals, **record)
        if verbose:
            print(logbook.stream)

        print(f'Generation {gen}: {time.time() - start:.4f}s')

    return individual, logbook

# def hill_climbing_para(
#     population: list[Individual], toolbox: Toolbox, ngen: int, stats: Statistics | None = None,
#     halloffame: HallOfFame | None = None, verbose: bool | None = __debug__
# ) -> tuple[Individual, Logbook]:
#     logbook = Logbook()
#     logbook.header = ['gen', 'nevals'] + (stats.fields if stats else [])

#     start_evaluate = time.time()
#     # Evaluate the individuals with an invalid fitness
#     fitnesses = toolbox.map(toolbox.evaluate, population)
#     for ind, fit in zip(population, fitnesses):
#         ind.fitness.values = fit
#     print(f'Evaluation: {time.time() - start_evaluate:.4f}s')

#     if halloffame is not None:
#         halloffame.update(population)

#     record = stats.compile(population) if stats else {}
#     logbook.record(gen=0, nevals=len(population), **record)
#     if verbose:
#         print(logbook.stream)

#     #new_individual = toolbox.clone(individual)
#     pop2 = [toolbox.clone(ind) for ind in population]

#     # Begin the generational process
#     for gen in range(1, ngen + 1):
#         nevals: int = 0
#         start = time.time()
#         # _fill_with(new_individual, individual)
#         for new, old in zip(pop2, population):
#             _fill_with(new, old)
#             # new.fitness.values = old.fitness.values

#         # new_individual = toolbox.mutate(new_individual)[0]
#         for i in range(len(pop2)):
#             pop2[i], = toolbox.mutate(pop2[i])
#             # del offspring[i].fitness.values

#         # new_individual.fitness.values = toolbox.evaluate(new_individual)
#         fitnesses = toolbox.map(toolbox.evaluate, pop2)
#         for ind, fit in zip(pop2, fitnesses):
#             ind.fitness.values = fit

#         # if new_individual.fitness.values > individual.fitness.values:
#         #     individual, new_individual = new_individual, individual
#         #     nevals += 1
#         for i in range(len(pop2)):
#             if pop2[i].fitness.values[0] > population[i].fitness.values[0]:
#                 population[i], pop2[i] = pop2[i], population[i]
#                 nevals += 1

#         # Update the hall of fame with the generated individuals
#         if halloffame is not None:
#             # halloffame.update([individual])
#             halloffame.update(population)

#         # Append the current generation statistics to the logbook
#         # record = stats.compile([individual]) if stats else {}
#         record = stats.compile(population) if stats else {}
#         logbook.record(gen=gen, nevals=nevals, **record)
#         if verbose:
#             print(logbook.stream)

#         print(f'Generation {gen}: {time.time() - start:.4f}s')

#     # return individual, logbook
#     return population, logbook

def simulated_annealing(
    individual: Individual, toolbox: Toolbox, ngen: int, schedule, stats: Statistics | None = None,
    halloffame: HallOfFame | None = None, verbose: bool | None = __debug__
) -> tuple[Individual, Logbook]:
    logbook = Logbook()
    logbook.header = ['gen', 'nevals'] + (stats.fields if stats else [])

    individual.fitness.values = toolbox.evaluate(individual)

    if halloffame is not None:
        halloffame.update([individual])

    record = stats.compile([individual]) if stats else {}
    logbook.record(gen=0, nevals=1, **record)
    if verbose:
        print(logbook.stream)

    new_individual = toolbox.clone(individual)

    # Begin the generational process
    for gen in range(1, ngen + 1):
        nevals: int = 0
        start = time.time()
        _fill_with(new_individual, individual)

        new_individual, = toolbox.mutate(new_individual)

        new_individual.fitness.values = toolbox.evaluate(new_individual)

        delta = new_individual.fitness.values[0] - individual.fitness.values[0]
        if delta > 0 or random.random() < math.exp(delta / schedule(gen)):
            if delta <= 0:
                print(f'Accepting worse solution with p={math.exp(delta / schedule(gen))}')
            individual, new_individual = new_individual, individual
            nevals += 1

        # Update the hall of fame with the generated individuals
        if halloffame is not None:
            halloffame.update([individual])

        # Append the current generation statistics to the logbook
        record = stats.compile([individual]) if stats else {}
        logbook.record(gen=gen, nevals=nevals, **record)
        if verbose:
            print(logbook.stream)

        print(f'Generation {gen}: {time.time() - start:.4f}s')

    return individual, logbook
