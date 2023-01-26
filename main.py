
import os
import sys

class Street:
    def __init__(self, id: int, start: int, end: int, name: str, length: int):
        self.id = id
        self.start = start
        self.end = end
        self.name = name
        self.length = length

    def __str__(self):
        return f'{self.id} {self.name} {self.start} -> {self.end} length: {self.length}'

class Car:
    def __init__(self, id: int, path_length: int, path: list[str]):
        self.id = id
        self.path_length = path_length
        self.path = path

    def __str__(self):
        return f'{self.id} length: {self.path_length} ' + ' '.join(self.path)

class Intersection:
    def __init__(self, id: int):
        self.id = id
        self.incoming = []
        self.outgoing = []

    def __str__(self):
        lines = []
        lines.append(str(self.id))
        lines.append('Incoming streets:')
        lines.extend(map(str, self.incoming))
        lines.append('Outgoing streets:')
        lines.extend(map(str, self.outgoing))
        return '\n'.join(lines)

class Simulation:
    def __init__(
        self, duration: int, intersections: list[Intersection], 
        streets: list[Street], cars: list[Car], bonus: int
        ):
        self.duration = duration
        self.intersections = intersections
        self.streets = streets
        self.cars = cars
        self.bonus = bonus

    def __str__(self):
        lines = []
        lines.append(f'Duration: {self.duration}')
        lines.append(f'Number of intersections: {len(self.intersections)}')
        lines.append(f'Bonus points: {self.bonus}')
        lines.append(f'Streets: {len(self.streets)}')
        lines.extend(map(str, self.streets))
        lines.append(f'Cars: {len(self.cars)}')
        lines.extend(map(str, self.cars))
        return '\n'.join(lines)

def read_input(file):

    D, I, S, V, F = file.readline().split()

    duration = int(D)
    num_of_intersections = int(I)
    num_of_streets = int(S)
    num_of_cars = int(V)
    bonus = int(F)

    intersections = [Intersection(i) for i in range(num_of_intersections)]

    lines = file.read().splitlines()
    streets = []
    for i, line in enumerate(lines[:num_of_streets]):
        start, end, name, length = line.split()
        street = Street(i, int(start), int(end), name, int(length))
        streets.append(street)
        intersections[street.start].outgoing.append(street)
        intersections[street.end].incoming.append(street)

    cars = []
    for i, line in enumerate(lines[num_of_streets:]):
        tokens = line.split()
        cars.append(Car(i, int(tokens[0]), tokens[1:]))

    return Simulation(duration, intersections, streets, cars, bonus)

#def create_output_default(simulation: Simulation):
#    os.makedirs(OUTPUT_FOLDER, exist_ok=True)
#    with open(os.path.join(OUTPUT_FOLDER, FILENAME + EXTENSION_OUT), 'w') as f:
#        f.write(str(len(simulation.intersections)) + '\n')
#        for intersection in simulation.intersections:
#            f.write(str(intersection.id) + '\n')
#            f.write(str(len(intersection.incoming)) + '\n')
#            for incoming in intersection.incoming:
#                f.write(incoming.name + ' 1\n')

def create_output_default(simulation: Simulation, file):
    file.write(str(len(simulation.intersections)) + '\n')
    for intersection in simulation.intersections:
        file.write(str(intersection.id) + '\n')
        file.write(str(len(intersection.incoming)) + '\n')
        for incoming in intersection.incoming:
            file.write(incoming.name + ' 1\n')
    

if __name__ == '__main__':
    #DATA_FOLDER = 'input_data'
    OUTPUT_FOLDER = 'output'
    #EXTENSION_IN = '.in.txt'
    #EXTENSION_OUT = '.out.txt'
    #FILENAME = 'a_an_example'
    #FILENAME = 'b_by_the_ocean'
    #FILENAME = 'c_checkmate'
    #FILENAME = 'd_daily_commute'
    #FILENAME = 'e_etoile'
    #FILENAME = 'f_forever_jammed'
    #with open(os.path.join(DATA_FOLDER, FILENAME + EXTENSION_IN), 'r') as f:
    simulation = read_input(sys.stdin)
    #print(simulation)
    os.makedirs(OUTPUT_FOLDER, exist_ok=True)
    #with open(os.path.join(OUTPUT_FOLDER, FILENAME + EXTENSION_OUT), 'w') as f:
    create_output_default(simulation, sys.stdout)