
import copy
import os
import sys
from queue import Queue

class Intersection:
    def __init__(self, id: int):
        self.id = id
        self.incoming = []
        self.outgoing = []
        self.plan = []
        self._last_used_time = -1

    def __str__(self):
        lines = []
        lines.append(str(self.id))
        lines.append('Incoming streets:')
        lines.extend(map(str, self.incoming))
        lines.append('Outgoing streets:')
        lines.extend(map(str, self.outgoing))
        return '\n'.join(lines)

    def green(self, time):
        """
        Returns a street that has a green light at time.
        """
        return self.plan[time % len(self.plan)]

    def is_green(self, time, street):
        return self.green(time) == street

    def use(self, time):
        """
        Indicates that a car has passed the street with a green light at time.
        """
        self._last_used_time = time

    def car_passed(self, time):
        """
        Returns whether some car has already passed the green light at time.
        """
        if time == self._last_used_time:
            return True
        return False


class Street:
    def __init__(self, id: int, start: Intersection, end: Intersection, name: str, length: int):
        self.id = id
        self.start = start
        self.end = end
        self.name = name
        self.length = length
        self.used = False
        self.queueing_cars = Queue()

    def __str__(self):
        return f'{self.id} {self.name} {self.start.id} -> {self.end.id} length: {self.length}'


class Car:
    def __init__(self, id: int, path_length: int, path: list[Street]):
        self.id = id
        self.path_length = path_length
        self.path = path
        self.current_street_index = 0
        self.current_distance = path[0].length
        self.finished = False
        self.finish_time = -1
        self.queueing = False

    def __str__(self):
        return f'{self.id} length: {self.path_length} ' #+ ' '.join(self.path)

    def end_of_street(self):
        return self.path[self.current_street_index].length == self.current_distance

    def final_destination(self):
        return self.path[-1] == self.path[self.current_street_index]

    def move(self, time: int):
        if not self.finished and not self.queueing:
            self.current_distance += 1

            if self.end_of_street():
                if self.final_destination():
                    self.finished = True
                    self.finish_time = time + 1
                else:
                    street = self.path[self.current_street_index]
                    street.queueing_cars.put(self)
                    self.queueing = True

#    def move(self, time: int):
#        if not self.finished:
#            street = self.path[self.current_street_index]
#
#            if self.current_distance < street.length:
#                self.current_distance += 1
#
#
#            elif self.current_distance == street.length:
#                if self.final_destination():
#                    # The car is at the final destination
#                    self.finished = True
#                    self.finish_time = time
#                else:
#                    intersection = street.end
#                    if intersection.is_green(time, street) and not intersection.car_passed(time):
#                        intersection.use(time)
#                        self.current_street_index += 1
#                        self.current_distance = 1


class Simulation:
    def __init__(
        self, 
        duration: int, 
        intersections: list[Intersection], 
        streets: list[Street], 
        cars: list[Car], 
        street_mapping: dict[str, Street], 
        bonus: int
        ):
        self.duration = duration
        self.intersections = intersections
        self.streets = streets
        self.street_mapping = street_mapping
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

    def run(self):
        for car in self.cars:
            car.path[car.current_street_index].queueing_cars.put(car)
            car.queueing = True

            intersections_used = []
            for intersection in self.intersections:
                for street in intersection.incoming:
                    if street.used:
                        intersections_used.append(intersection)
                        break

        for t in range(self.duration):
            #print(t)
            for intersection in intersections_used:
                street = intersection.green(t)
                if not street.queueing_cars.empty():
                    car = street.queueing_cars.get()
                    car.queueing = False
                    car.current_street_index += 1
                    car.current_distance = 0

            for car in self.cars:
                car.move(t)

#    def run(self):
#        #moving_cars = copy.copy(self.cars)
#        #for t in range(self.duration):
#        for t in range(self.duration + 1):
#            #print(f'TIME {t}')
#            for car in self.cars:
#                car.move(t)
#                #if car.finished:
#                #    moving_cars.remove(car)
#            #if not moving_cars:
#            #    break
#            #for intersection in self.intersections:
#            #    if intersection.plan:
#            #        print(f'Intersection: {intersection.id}')
#            #        green_street = intersection.green(t)
#            #        print(f'Green: {green_street}')
#            #        print(f'Last used time: {intersection._last_used_time}')

    def score(self):
        score = 0
        finished = 0
        maximum = 0
        max_idx = -1
        minimum = float('inf')
        min_idx = -1
        time_driven = 0
        count = 0
        for car in self.cars:
            if car.finished:
                finished += 1
                car_score = self.bonus + (self.duration - car.finish_time)
                score += car_score
                time_driven += car.finish_time
                count += 1
                if maximum < car_score:
                    maximum = car_score
                    max_idx = car.id
                if minimum > car_score:
                    minimum = car_score
                    min_idx = car.id
        print('#########################################')
        print(f'Finished cars {finished}')
        print(f'Average ride time: {time_driven / count:0.2f}')
        print(f'Max Score {maximum} car ID {max_idx}')
        print(f'Arrival time: {self.cars[max_idx].finish_time}')
        print(f'Min Score {minimum} car ID {min_idx}')
        print(f'Arrival time: {self.cars[min_idx].finish_time}')
        return score

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
    OUTPUT_FOLDER = 'output'
    #with open(os.path.join(DATA_FOLDER, FILENAME + EXTENSION_IN), 'r') as f:
    simulation = read_input(sys.stdin)
    #print(simulation)
    os.makedirs(OUTPUT_FOLDER, exist_ok=True)
    #with open(os.path.join(OUTPUT_FOLDER, FILENAME + EXTENSION_OUT), 'w') as f:

    #create_output_default(simulation, sys.stdout)
    create_output_used(simulation, sys.stdout)