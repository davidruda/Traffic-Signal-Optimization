import argparse
from collections import defaultdict
import concurrent.futures
import os
import time
import threading

from _resolve_imports import *

#TODO: Update this file to use unittest

parser = argparse.ArgumentParser()
parser.add_argument('--data', default='a', type=str, help='Input data.')
parser.add_argument('--population', default=100, type=int, help='Number of individuals in a population.')
parser.add_argument('--parallel', default=os.cpu_count(), type=int, help='Number of threads for parallel computation.')

def eval(_):
    return simulations[threading.get_ident()].score()

def test_run_times(args, map_func):
    start = time.time()
    scores = list(map_func(eval, args.population * [0]))
    end = time.time() - start
    print(f'{args.population} simulations finished: {end:.4f}s')

    return scores, end

def simulation_factory():
    s = Simulation(city_plan)
    s.default_schedules()
    return s

def main(args):
    times = []
    for n in range(1, args.parallel + 1):
        print(f'{24 * "-"} Parallel: {n:2} threads {24 * "-"}')
        pool = concurrent.futures.ThreadPoolExecutor(max_workers=n)
        global simulations
        simulations = defaultdict(simulation_factory)
        _, elapsed_time = test_run_times(args, pool.map)
        times.append(elapsed_time)
        pool.shutdown()
        print(70 * '-')

    if times:
        import matplotlib.pyplot as plt

        plt.plot(range(1, args.parallel + 1), times)
        plt.xticks(range(1, args.parallel + 1))
        plt.xlabel('Threads')
        plt.ylabel('Elapsed time in seconds')
        plt.title('Multithreading Performance Analysis')
        plt.show()

if __name__ == '__main__':
    args = parser.parse_args()
    city_plan = CityPlan(get_data_filename(args.data))
    simulations = None
    main(args)
    