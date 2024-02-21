import argparse
from collections import defaultdict
import concurrent.futures
from functools import partial
import os
import time
import threading
import unittest

from _resolve_imports import *

parser = argparse.ArgumentParser()
parser.add_argument('--data', default='a', type=str, help='Input data.')
parser.add_argument('--simulations', default=100, type=int, help='Number of simulations for parallel computation.')
parser.add_argument('--parallel', default=max(2, os.cpu_count()), type=int, help='Number of threads for parallel computation.')

class TestMultithreading(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.data = 'd'
        cls.parallel = max(2, os.cpu_count())
        cls.simulations = cls.parallel #40

    def test_multithreading(self):
        _test_multithreading(self.data, self.simulations, self.parallel)


def _test_multithreading(data, count, parallel):
    def eval(_):
        return simulations[threading.get_ident()].score()
        
    def run_simulations(map_func):
        start = time.time()
        scores = list(map_func(eval, count * [0]))
        end = time.time() - start
        print(f'{count} simulations finished: {end:.4f}s')
        return scores, end

    plan = create_city_plan(data)
    simulation_factory = partial(default_simulation, plan)
    times = []
    print(f'\n{"-" * 31} DATA {data} {"-" * 31}')
    for n in range(1, parallel + 1):
        print(f'{24 * "-"} Parallel: {n:2} threads {24 * "-"}')
        pool = concurrent.futures.ThreadPoolExecutor(max_workers=n)
        simulations = defaultdict(simulation_factory)
        _, elapsed_time = run_simulations(pool.map)
        times.append(elapsed_time)
        pool.shutdown()
        print(70 * '-')
    return times

if __name__ == '__main__':
    #unittest.main()
    # Make it possible to run this test directly
    # from the command line with arguments
    args = parser.parse_args()
    times = _test_multithreading(args.data, args.simulations, args.parallel)
    if times:
        import matplotlib.pyplot as plt
        plt.plot(range(1, len(times) + 1), times)
        plt.xticks(range(1, len(times) + 1))
        plt.xlabel('Threads')
        plt.ylabel('Elapsed time in seconds')
        plt.title('Multithreading Performance Analysis')
        plt.show()
    