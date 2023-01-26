
import os
import sys

class Street:
    def __init__(self, id: int, start: int, end: int, name: str, length: int):
        self.id = id
        self.start = start
        self.end = end
        self.name = name
        self.length = length
        self.used = False

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
        self, 
        duration: int, 
        intersections: list[Intersection], 
        streets: list[Street], 
        cars: list[Car], 
        bonus: int
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
    street_mapping = {}
    for i, line in enumerate(lines[:num_of_streets]):
        start, end, name, length = line.split()
        street = Street(i, int(start), int(end), name, int(length))
        streets.append(street)
        street_mapping[street.name] = street.id
        intersections[street.start].outgoing.append(street)
        intersections[street.end].incoming.append(street)

    cars = []
    for i, line in enumerate(lines[num_of_streets:]):
        tokens = line.split()
        car = Car(i, int(tokens[0]), tokens[1:])
        cars.append(car)
        for streetname in car.path:
            streets[street_mapping[streetname]].used = True

    return Simulation(duration, intersections, streets, cars, bonus)

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
            file.write(streetname + ' 1 \n')

if __name__ == '__main__':
    OUTPUT_FOLDER = 'output'
    #with open(os.path.join(DATA_FOLDER, FILENAME + EXTENSION_IN), 'r') as f:
    simulation = read_input(sys.stdin)
    #print(simulation)
    os.makedirs(OUTPUT_FOLDER, exist_ok=True)
    #with open(os.path.join(OUTPUT_FOLDER, FILENAME + EXTENSION_OUT), 'w') as f:

    #create_output_default(simulation, sys.stdout)
    create_output_used(simulation, sys.stdout)