
import argparse
import copy
import os
import sys

from intersection import Intersection
from street import Street
from car import Car
from simulation import Simulation

def read_input(filename):
    with open(filename, 'r') as file:
        D, I, S, V, F = file.readline().split()

        duration = int(D)
        num_of_intersections = int(I)
        num_of_streets = int(S)
        num_of_cars = int(V)
        bonus = int(F)

        intersections = [Intersection(i) for i in range(num_of_intersections)]

        lines = file.read().splitlines()
        streets = []
        street_mapping = {}
        for i, line in enumerate(lines[:num_of_streets]):
            start, end, name, length = line.split()
            street = Street(
                i, intersections[int(start)], intersections[int(end)], name, int(length)
                )
            streets.append(street)
            street_mapping[street.name] = street
            street.start.outgoing.append(street)
            street.end.incoming.append(street)

        cars = []
        for i, line in enumerate(lines[num_of_streets:]):
            street_names = line.split()[1:]
            path = list(map(street_mapping.get , street_names))
            car = Car(i, len(path), path)
            cars.append(car)
            for street in car.path:
                street.used = True

        return Simulation(duration, intersections, streets, cars, street_mapping, bonus)

def create_output_default(simulation: Simulation, file):
    """
    For every intersection and every incoming street, set green for 1 second.
    """
    file.write(str(len(simulation.intersections)) + '\n')
    for intersection in simulation.intersections:
        file.write(str(intersection.id) + '\n')
        file.write(str(len(intersection.incoming)) + '\n')
        for incoming in intersection.incoming:
            file.write(incoming.name + ' 1\n')
    
def create_output_used(simulation: Simulation, file):
    """
    Set green for 1 second for every used incoming street. 
    Exclude streets that aren't used.
    """
    intersections_used = []
    for intersection in simulation.intersections:
        for street in intersection.incoming:
            if street.used:
                intersections_used.append(intersection.id)
                break

    file.write(str(len(intersections_used)) + '\n')

    for i in intersections_used:
        intersection = simulation.intersections[i]
        file.write(str(intersection.id) + '\n')
        streets_used = []
        for incoming in intersection.incoming:
            if incoming.used:
                streets_used.append(incoming.name)
        file.write(str(len(streets_used)) + '\n')
        for streetname in streets_used:
            file.write(streetname + ' 1\n')

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('input', help='Input file.')
    #parser.add_argument('output', help='Output file.')
    args = parser.parse_args()

    OUTPUT_FOLDER = 'output'
    simulation = read_input(args.input)
    #print(simulation)

    #os.makedirs(OUTPUT_FOLDER, exist_ok=True)
    #OUT_FILE = args.input.split(os.sep)[-1].replace('.in.', '.out.')

    #with open(os.path.join(OUTPUT_FOLDER, OUT_FILE), 'w') as f:
    ##with open(args.output, 'w') as f:
    #    #create_output_default(simulation, f)
    #    create_output_used(simulation, f)

    create_output_default(simulation, sys.stdout)
    #create_output_used(simulation, sys.stdout)