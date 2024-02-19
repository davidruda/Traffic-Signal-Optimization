from array import array
import random
import time

import cython

from deap.base import Toolbox
from deap.tools import Logbook, Statistics, HallOfFame
#from deap.tools import cxTwoPoint, cxOrdered, mutShuffleIndexes, Logbook
#from deap.algorithms import eaSimple, varAnd

IND_TYPE = list[tuple[array, array]]

@cython.boundscheck(False)
@cython.wraparound(False)
@cython.initializedcheck(False)
def fill_with(ind1: IND_TYPE, ind2: IND_TYPE) -> None:
    """
    Fill `ind1` with values from `ind2`.
    """
    order1: cython.uint[:]
    times1: cython.uint[:]
    order2: cython.uint[:]
    times2: cython.uint[:]
    for (order1, times1), (order2, times2) in zip(ind1, ind2):
        for i in range(len(order1)):
            order1[i] = order2[i]
            times1[i] = times2[i]

@cython.boundscheck(False)
@cython.wraparound(False)
@cython.initializedcheck(False)
def cxTwoPoint(ind1: cython.uint[:], ind2: cython.uint[:]):
    size: cython.Py_ssize_t = min(len(ind1), len(ind2))
    cxpoint1: cython.int = random.randint(1, size)
    cxpoint2: cython.int = random.randint(1, size - 1)
    
    if cxpoint2 >= cxpoint1:
        cxpoint2 += 1
    else:  # Swap the two cx points
        cxpoint1, cxpoint2 = cxpoint2, cxpoint1

    for i in range(cxpoint1, cxpoint2):
        ind1[i], ind2[i] = ind2[i], ind1[i]

    return ind1, ind2

@cython.boundscheck(False)
@cython.wraparound(False)
@cython.initializedcheck(False)
def cxOrdered(ind1: cython.uint[:], ind2: cython.uint[:]):
    a: cython.int
    b: cython.int
    i: cython.int
    k1: cython.int
    k2: cython.int
    temp1: cython.uint[:]
    temp2: cython.uint[:]
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
def mutation_change_by_one(individual: cython.uint[:], indpb: cython.float, low: cython.int, up: cython.int):
    size: cython.Py_ssize_t = len(individual)

    for i in range(size):
        #if crandom() < indpb:
        if random.random() < indpb:
            #if crandom() < 0.5:
            if random.random() < 0.5:
                individual[i] = min(individual[i] + 1, up)
            else:
                individual[i] = max(individual[i] - 1, low)
    return individual,

@cython.boundscheck(False)
@cython.wraparound(False)
@cython.initializedcheck(False)
def mutShuffleIndexes(individual: cython.uint[:], indpb: cython.float):
    swap_indx: cython.Py_ssize_t
    size: cython.Py_ssize_t = len(individual)
    
    for i in range(size):
        #if crandom() < indpb:
        if random.random() < indpb:
            #swap_indx = crandrange(0, size - 1)
            swap_indx = random.randint(0, size - 2)
            if swap_indx >= i:
                swap_indx += 1
            individual[i], individual[swap_indx] = \
                individual[swap_indx], individual[i]

    return individual,

def crossover(ind1: IND_TYPE, ind2: IND_TYPE) -> tuple[IND_TYPE, IND_TYPE]:
    for (order1, times1), (order2, times2) in zip(ind1, ind2):
        cxOrdered(order1, order2)
        cxTwoPoint(times1, times2)

    return ind1, ind2

def mutation(individual: IND_TYPE, indpb: float, low: int, up: int) -> tuple[IND_TYPE]:
    for order, times in individual:
        mutShuffleIndexes(order, indpb)
        #tools.mutUniformInt(times, low, up, indpb)
        mutation_change_by_one(times, indpb, low, up)
    return individual,

#def varAnd(population, toolbox, cxpb, mutpb):
def varAnd(population: list[IND_TYPE], offspring: list[IND_TYPE], toolbox: Toolbox, cxpb: float, mutpb: float) -> list[IND_TYPE]:
#def varAnd(population, offspring, toolbox, cxpb, mutpb):
    #offspring = [toolbox.clone(ind) for ind in population]
    for new, old in zip(offspring, population):
        fill_with(new, old)

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

def eaSimple(population: list[IND_TYPE], toolbox: Toolbox, cxpb: float, 
             mutpb: float, ngen: int, stats: Statistics | None = None,
             halloffame: HallOfFame | None = None, verbose: bool | None = __debug__) -> tuple[list[IND_TYPE], Logbook]:
#def eaSimple(population, toolbox, cxpb, mutpb, ngen, stats=None,
#             halloffame=None, verbose=__debug__):
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

    offspring = [toolbox.clone(ind) for ind in population]

    # Begin the generational process
    for gen in range(1, ngen + 1):
        start = time.time()

        # Select the next generation individuals
        #offspring = toolbox.select(population, len(population))
        selected = toolbox.select(population, len(population))

        start_mutate = time.time()
        # Vary the pool of individuals
        offspring = varAnd(selected, offspring, toolbox, cxpb, mutpb)
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
        population, offspring = offspring, population

        # Append the current generation statistics to the logbook
        record = stats.compile(population) if stats else {}
        logbook.record(gen=gen, nevals=len(invalid_ind), **record)
        if verbose:
            print(logbook.stream)

        print(f'Generation {gen}: {time.time() - start:.4f}s')

    return population, logbook