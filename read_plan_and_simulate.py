
import argparse
from simulation import Simulation

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('input', help='Input file.')
    parser.add_argument('plan', help='File with the traffic light plan.')
    args = parser.parse_args()

    simulation = Simulation.from_file(args.input)
    score = simulation.read_plan(args.plan).run().score(verbose=True)
    print(score)