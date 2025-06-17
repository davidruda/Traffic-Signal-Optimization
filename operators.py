from array import array
from functools import partial
import math
import random
import time

import cython

from deap.base import Toolbox
from deap.tools import Logbook, Statistics, HallOfFame, selBest, selTournament

# Compile this file with Cython to speed up the optimization
# cythonize -3ai operators.py

Order = array
Times = array
Individual = tuple[Order, Times]

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
def _cxTwoPoint(schedule1: tuple, schedule2: tuple):
    """
    Modified version of `cxTwoPoint` from DEAP.

    Source: https://github.com/DEAP/deap/blob/master/deap/tools/crossover.py
    """
    times1: cython.ulong[:] = schedule1[1]
    times2: cython.ulong[:] = schedule2[1]
    size: cython.Py_ssize_t = min(len(times1), len(times2))
    cxpoint1: cython.int = random.randint(1, size)
    cxpoint2: cython.int = random.randint(1, size - 1)

    if cxpoint2 >= cxpoint1:
        cxpoint2 += 1
    else:  # Swap the two cx points
        cxpoint1, cxpoint2 = cxpoint2, cxpoint1

    times1[cxpoint1:cxpoint2], times2[cxpoint1:cxpoint2] = times2[cxpoint1:cxpoint2], times1[cxpoint1:cxpoint2]


@cython.boundscheck(False)
@cython.wraparound(False)
@cython.initializedcheck(False)
def _cxOrdered(schedule1: tuple, schedule2: tuple):
    """
    Modified version of `cxOrdered` from DEAP.

    Source: https://github.com/DEAP/deap/blob/master/deap/tools/crossover.py
    """
    order1: cython.ulong[:] = schedule1[0]
    times1: cython.ulong[:] = schedule1[1]
    order2: cython.ulong[:] = schedule2[0]
    times2: cython.ulong[:] = schedule2[1]
    a: cython.int
    b: cython.int
    i: cython.int
    k1: cython.int
    k2: cython.int
    order1_temp: cython.ulong[:]
    order2_temp: cython.ulong[:]
    size: cython.Py_ssize_t = min(len(order1), len(order2))

    # Note that we must do the same changes to times because they are based on orders
    a, b = random.sample(range(size), 2)
    if a > b:
        a, b = b, a

    holes1, holes2 = [True] * size, [True] * size
    for i in range(size):
        if i < a or i > b:
            holes1[order2[i]] = False
            holes2[order1[i]] = False

    # We must keep the original values somewhere before scrambling everything
    order1_temp, order2_temp = order1, order2
    times1_temp, times2_temp = times1, times2
    k1, k2 = b + 1, b + 1
    for i in range(size):
        if not holes1[order1_temp[(i + b + 1) % size]]:
            order1[k1 % size] = order1_temp[(i + b + 1) % size]
            times1[k1 % size] = times1_temp[(i + b + 1) % size]
            k1 += 1

        if not holes2[order2_temp[(i + b + 1) % size]]:
            order2[k2 % size] = order2_temp[(i + b + 1) % size]
            times2[k2 % size] = times2_temp[(i + b + 1) % size]
            k2 += 1

    # Swap the content between a and b (included)
    for i in range(a, b + 1):
        order1[i], order2[i] = order2[i], order1[i]
        times1[i], times2[i] = times2[i], times1[i]


@cython.boundscheck(False)
@cython.wraparound(False)
@cython.initializedcheck(False)
def mutation_change_by_one(schedule: tuple, indpb: cython.float, low: cython.uint, up: cython.uint):
    """
    Mutate individual by changing each value by +-1 in the range [`low`, `up`] with probability `indpb`.
    """
    times: cython.ulong[:] = schedule[1]
    size: cython.Py_ssize_t = len(times)

    for i in range(size):
        if random.random() < indpb:
            if random.random() < 0.5:
                # Make sure the number doesn't overflow
                if times[i] < up:
                    times[i] = times[i] + 1
            else:
                # Make sure the number doesn't overflow
                if times[i] > low:
                    times[i] = times[i] - 1


@cython.boundscheck(False)
@cython.wraparound(False)
@cython.initializedcheck(False)
def _mutShuffleIndexes(schedule: tuple, indpb: cython.float):
    """
    Modified version of `mutShuffleIndexes` from DEAP.

    Source: https://github.com/DEAP/deap/blob/master/deap/tools/mutation.py
    """
    order: cython.ulong[:] = schedule[0]
    times: cython.ulong[:] = schedule[1]
    swap_indx: cython.int
    size: cython.Py_ssize_t = len(order)

    for i in range(size):
        if random.random() < indpb:
            swap_indx = random.randint(0, size - 2)
            if swap_indx >= i:
                swap_indx += 1

            # Swap both order and times because times are based on order
            order[i], order[swap_indx] = order[swap_indx], order[i]
            times[i], times[swap_indx] = times[swap_indx], times[i]


def tournament_selection_with_elitism(
    population: list[Individual], k: int, tournsize: int, elitism: float
) -> list[Individual]:
    # https://groups.google.com/g/deap-users/c/iannnLI2ncE
    num_best = int(elitism * k)
    return selBest(population, num_best) + selTournament(population, k - num_best, tournsize)


def _varAnd(
    population: list[Individual], pop2: list[Individual], toolbox: Toolbox, cxpb: float, mutpb: float
) -> list[Individual]:
    """
    Modified version of `varAnd` from DEAP.

    Source: https://github.com/DEAP/deap/blob/master/deap/algorithms.py
    """
    i: cython.int

    # Fill the offspring with values from the population to avoid expensive cloning
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
    """
    Modified version of `eaSimple` from DEAP.

    Source: https://github.com/DEAP/deap/blob/master/deap/algorithms.py
    """
    logbook = Logbook()
    logbook.header = ['gen', 'nevals'] + (stats.fields if stats else [])

    start_evaluate = time.time()
    # Evaluate the individuals with an invalid fitness
    invalid_ind = [ind for ind in population if not ind.fitness.valid]
    fitnesses = toolbox.map(toolbox.evaluate, invalid_ind)
    for ind, fit in zip(invalid_ind, fitnesses):
        ind.fitness.values = fit
    if verbose:
        print(f'Evaluation: {time.time() - start_evaluate:.4f}s')

    if halloffame is not None:
        halloffame.update(population)

    record = stats.compile(population) if stats else {}
    logbook.record(gen=0, nevals=len(invalid_ind), **record)
    if verbose:
        print(logbook.stream)

    # Create a full copy of the population
    # From now on, keep filling values between population and pop2 to avoid expensive cloning
    pop2 = [toolbox.clone(ind) for ind in population]

    # Begin the generational process
    for gen in range(1, ngen + int(1)): # the int call is there to avoid Cython warning
        start = time.time()

        # Select the next generation individuals
        offspring = toolbox.select(population, len(population))

        start_mutate = time.time()
        # Vary the pool of individuals
        offspring = _varAnd(offspring, pop2, toolbox, cxpb, mutpb)
        if verbose:
            print(f'Cross+Mut: {time.time() - start_mutate:.4f}s')

        start_evaluate = time.time()
        # Evaluate the individuals with an invalid fitness
        invalid_ind = [ind for ind in offspring if not ind.fitness.valid]

        fitnesses = toolbox.map(toolbox.evaluate, invalid_ind)
        for ind, fit in zip(invalid_ind, fitnesses):
            ind.fitness.values = fit
        if verbose:
            print(f'Evaluation: {time.time() - start_evaluate:.4f}s')

        # Update the hall of fame with the generated individuals
        if halloffame is not None:
            halloffame.update(offspring)

        # Replace the current population by the offspring
        # Swap the only two populations we have
        population, pop2 = offspring, population


        # Append the current generation statistics to the logbook
        record = stats.compile(population) if stats else {}
        logbook.record(gen=gen, nevals=len(invalid_ind), **record)
        if verbose:
            print(logbook.stream)

        if verbose:
            print(f'Generation {gen}: {time.time() - start:.4f}s')

    return population, logbook


cxTwoPoint = _cxTwoPoint
cxOrdered = _cxOrdered
mutShuffleIndexes = _mutShuffleIndexes
genetic_algorithm = _eaSimple

def crossover(ind1: Individual, ind2: Individual) -> tuple[Individual, Individual]:
    """
    Crossover that combines two individuals.

    For each corresponding part of the individual (schedule of one intersection),
    crossover only the order, or only the times, or both at random.
    """
    choice: cython.int

    for i in range(len(ind1)):
        # crossover only order, or only times, or both
        choice = random.randint(1, 3)
        if choice & 0b01:
            cxOrdered(ind1[i], ind2[i])
        if choice & 0b10:
            cxTwoPoint(ind1[i], ind2[i])

    return ind1, ind2


def mutation(ind: Individual, indpb: float, low: int, up: int) -> tuple[Individual]:
    """
    Mutation that applies random changes to an individual.

    For each part of the individual (schedule of one intersection),
    mutate only the order, or only the times, or both at random.
    """
    choice: cython.int

    for i in range(len(ind)):
        # mutation only order, or only times, or both
        choice = random.randint(1, 3)
        if choice & 0b01:
            mutShuffleIndexes(ind[i], indpb)
        if choice & 0b10:
            mutation_change_by_one(ind[i], indpb, low, up)

    return ind,


class LinearSchedule:
    """
    Linear cooling schedule for simulated annealing.
    """
    def __init__(self, start: float, steps: int) -> None:
        self.start = start
        self.steps = steps

    def __call__(self, current: int) -> float:
        return self.start * (1.0 - current / self.steps) + 1e-9


class InverseSchedule:
    """
    Inverse / hyperbolic cooling schedule for simulated annealing.
    """
    def __init__(self, start: float, steps: int) -> None:
        self.start = start
        self.steps = steps

    def __call__(self, current: int) -> float:
        return self.start * (1.0 / current) + 1e-9


def _hill_climbing_compare(
    individual: Individual, new_individual: Individual, toolbox: Toolbox, gen: int, verbose: bool
) -> bool:
    """
    Hill climbing comparison function.

    Returns True if the new individual is better and should be accepted.
    """
    return new_individual.fitness.values[0] > individual.fitness.values[0]


def _simulated_annealing_compare(
    individual: Individual, new_individual: Individual, toolbox: Toolbox, gen: int, verbose: bool
) -> bool:
    """
    Simulated annealing comparison function.

    Returns True if the new individual is better or randomly accepted as worse.
    """
    delta = new_individual.fitness.values[0] - individual.fitness.values[0]
    if delta > 0:
        return True
    threshold = math.exp(delta / toolbox.schedule(gen))
    if random.random() < threshold:
        if verbose:
            print(f'Accepting worse solution with p={threshold}')
        return True
    return False


def _single_state_algorithm(
    population: list[Individual], toolbox: Toolbox, ngen: int, stats: Statistics | None = None,
    halloffame: HallOfFame | None = None, verbose: bool | None = __debug__,
    compare_fn: callable = None
) -> tuple[Individual, Logbook]:
    """
    Template function for single-state algorithms like hill climbing and simulated annealing.
    `compare_fn` has to be given to provide the comparison method between individuals.

    Based on `eaSimple` function from DEAP.
    """

    def process_individual(individual: Individual, new_individual: Individual):
        _fill_with(new_individual, individual)

        new_individual, = toolbox.mutate(new_individual)
        new_individual.fitness.values = toolbox.evaluate(new_individual)

        # Compare returns True if new_individual is accepted, based on the algorithm
        if compare_fn(individual, new_individual, toolbox, gen, verbose):
            individual, new_individual = new_individual, individual

        return individual, new_individual

    logbook = Logbook()
    logbook.header = ['gen', 'nevals'] + (stats.fields if stats else [])

    start_evaluate = time.time()
    # Evaluate the individuals with an invalid fitness
    fitnesses = toolbox.map(toolbox.evaluate, population)
    for ind, fit in zip(population, fitnesses):
        ind.fitness.values = fit
    if verbose:
        print(f'Evaluation: {time.time() - start_evaluate:.4f}s')

    if halloffame is not None:
        halloffame.update(population)

    record = stats.compile(population) if stats else {}
    logbook.record(gen=0, nevals=len(population), **record)
    if verbose:
        print(logbook.stream)

    # Create a full copy of the population
    # From now on, keep filling values between population and pop2 to avoid expensive cloning
    pop2 = [toolbox.clone(ind) for ind in population]

    # Begin the generational process
    for gen in range(1, ngen + int(1)): # the int call is there to avoid Cython warning
        start = time.time()

        # This has to be written as a one-liner or Cython won't compile it
        population, pop2 = map(list, zip(*toolbox.map(process_individual, population, pop2)))

        # Update the hall of fame with the generated individuals
        if halloffame is not None:
            halloffame.update(population)

        # Append the current generation statistics to the logbook
        record = stats.compile(population) if stats else {}
        logbook.record(gen=gen, nevals=len(population), **record)
        if verbose:
            print(logbook.stream)

        if verbose:
            print(f'Iteration {gen}: {time.time() - start:.4f}s')

    return population, logbook

hill_climbing = partial(_single_state_algorithm, compare_fn=_hill_climbing_compare)
simulated_annealing = partial(_single_state_algorithm, compare_fn=_simulated_annealing_compare)
