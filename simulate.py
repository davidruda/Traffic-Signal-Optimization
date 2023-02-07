
import argparse
from main import *
from io import FileIO

def read_plan(file: FileIO, simulation: Simulation):
    num_of_intersections = int(file.readline())
    for _ in range(num_of_intersections):
        intersection_id = int(file.readline())
        num_of_streets = int(file.readline())

        intersection = simulation.intersections[intersection_id]
        for _ in range(num_of_streets):
            street_name, green_time = file.readline().split()

            street = simulation.street_mapping[street_name]
            intersection.plan.extend([street] * int(green_time))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('input', help='Input file.')
    parser.add_argument('plan', help='File with the traffic light plan.')
    args = parser.parse_args()

    with open(args.input, 'r') as f:
        simulation = read_input(f)
    with open(args.plan, 'r') as f:
        read_plan(f, simulation)

    simulation.run()
    print(simulation.score())