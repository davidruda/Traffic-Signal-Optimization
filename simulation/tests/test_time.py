import argparse
import os
import time

from simulation import *

DEFAULT_INPUT_FOLDER = 'input_data'
DEFAULT_OUTPUT_FOLDER = 'output'
parser = argparse.ArgumentParser()
parser.add_argument('--input', default=os.path.join(DEFAULT_INPUT_FOLDER, 'd.txt'), help='Input file.')
parser.add_argument('--plan', default=os.path.join(DEFAULT_OUTPUT_FOLDER, 'd.txt'), help='Plan file.')
args = parser.parse_args()

if __name__ == '__main__':
    start = time.time()
    sim = Simulation(args.input)
    print(f'Simulation constructor: {time.time() - start:.4f}s')

    start = time.time()
    instance = sim.create_instance()
    print(f'create_instance: {time.time() - start:.4f}s')

    # start = time.time()
    # instance.read_plan(args.plan)
    # print(f'read_plan: {time.time() - start:.4f}s')

    start = time.time()
    instance.create_plan_default()
    print(f'create_plan_default: {time.time() - start:.4f}s')

    start = time.time()
    instance.run()
    print(f'run: {time.time() - start:.4f}s')
