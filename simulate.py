
import argparse
from simulation import Simulation

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('input', help='Input file.')
    parser.add_argument('--plan-type', help='Type of plan to use.', choices=['default', 'used'], default='default')
    args = parser.parse_args()

    simulation = Simulation.from_file(args.input)
    if args.plan_type == 'used':
        simulation.create_plan_used()
    else:
        simulation.create_plan_default()
    score = simulation.run().score(verbose=True)
    print(score)