
import argparse

from simulation import Simulation

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-i', '--input', help='Input file.')
    parser.add_argument('-o', '--output', help='Output file.')
    args = parser.parse_args()

    simulation = Simulation.from_file(args.input)
    #print(simulation)

    simulation.create_plan_used().create_output(args.output)
    #simulation.create_plan_default().create_output(args.output)