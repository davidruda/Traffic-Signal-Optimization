
import os

class Street:
    def __init__(self, id, start, end, name, length):
        self.id = id
        self.start = start
        self.end = end
        self.name = name
        self.length = length

    def __str__(self):
        return f'{self.id} {self.name} {self.start} -> {self.end} length: {self.length}'

class Car:
    def __init__(self, id, path_length, path):
        self.id = id
        self.path_length = path_length
        self.path = path

    def __str__(self):
        return f'{self.id} length: {self.path_length} ' + ' '.join(self.path)

def read_input(file):

    D, I, S, V, F = file.readline().split()

    duration = int(D)
    num_of_intersections = int(I)
    num_of_streets = int(S)
    num_of_cars = int(V)
    bonus = int(F)

    lines = file.read().splitlines()
    streets = []
    for i, line in enumerate(lines[:num_of_streets]):
        streets.append(Street(i, *line.split()))

    cars = []
    for i, line in enumerate(lines[num_of_streets:]):
        tokens = line.split()
        cars.append(Car(i, tokens[0], tokens[1:]))

    print(f'Duration: {duration}')
    print(f'Number of intersections: {num_of_intersections}')
    print(f'Bonus points: {bonus}')
    print(f'Streets: {num_of_streets}')
    for street in streets:
        print(street)
    print(f'Cars: {num_of_cars}')
    for car in cars:
        print(car)


if __name__ == '__main__':
    DATA_FOLDER = 'input_data'
    FILENAME = 'a_an_example.in.txt'
    with open(os.path.join(DATA_FOLDER, FILENAME)) as f:
        read_input(f)