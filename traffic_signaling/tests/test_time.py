import argparse
import os
import time

from traffic_signaling.city_plan import CityPlan
from traffic_signaling.simulation import Simulation

DEFAULT_INPUT_FOLDER = 'traffic_signaling/data'
DEFAULT_OUTPUT_FOLDER = 'output'
parser = argparse.ArgumentParser()
parser.add_argument('--input', default=os.path.join(DEFAULT_INPUT_FOLDER, 'd.txt'), help='Input file.')
parser.add_argument('--plan', default=os.path.join(DEFAULT_OUTPUT_FOLDER, 'd.txt'), help='Plan file.')

def main(args):
    start = time.time()
    city_plan = CityPlan(args.input)
    print(f'CityPlan constructor: {time.time() - start:.4f}s')

    start = time.time()
    simulation = Simulation(city_plan)
    print(f'Simulation constructor: {time.time() - start:.4f}s')

    start = time.time()
    simulation.create_plan_default()
    print(f'create_plan_default: {time.time() - start:.4f}s')

    start = time.time()
    simulation.write_plan(args.plan)
    print(f'write_plan: {time.time() - start:.4f}s')

    start = time.time()
    simulation.read_plan(args.plan)
    print(f'read_plan: {time.time() - start:.4f}s')

    start = time.time()
    simulation.score()
    print(f'score: {time.time() - start:.4f}s')

if __name__ == '__main__':
    args = parser.parse_args()
    main(args)
