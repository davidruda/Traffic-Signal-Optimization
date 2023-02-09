
import argparse
from main import *


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('input', help='Input file.')
    parser.add_argument('plan', help='File with the traffic light plan.')
    args = parser.parse_args()

    simulation = read_input(args.input)
    score = simulation.read_plan(args.plan).run().score()
    print(score)