
from collections import Counter
import sys

import numpy as np

from intersection import Intersection
from street import Street
from car import Car

from stable_priority_queue import StablePriorityQueue

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

    @classmethod
    def from_file(cls, filename):
        if filename is not None:
            with open(filename, 'r') as file:
                return cls._from_file(file)
        else:
            return cls._from_file(sys.stdin)

    @classmethod
    def _from_file(cls, file):
        D, I, S, V, F = file.readline().split()
        duration = int(D)
        num_of_intersections = int(I)
        num_of_streets = int(S)
        num_of_cars = int(V)
        bonus = int(F)
        intersections = np.empty(num_of_intersections, dtype=Intersection)
        streets = np.empty(num_of_streets, dtype=Street)
        street_mapping = {}
        for i in range(num_of_streets):
            start, end, name, length = file.readline().split()
            start = int(start)
            end = int(end)
            length = int(length)
            if intersections[start] is None:
                intersections[start] = Intersection(start)
            if intersections[end] is None:
                intersections[end] = Intersection(end)
            street = Street(i, intersections[start], intersections[end], name, length)
            streets[i] = street
            street_mapping[street.name] = street
            street.start.outgoing.append(street)
            street.end.incoming.append(street)

        cars = np.empty(num_of_cars, dtype=Car)
        for i in range(num_of_cars):
            street_names = file.readline().split()[1:]
            path = [street_mapping[name] for name in street_names]
            car = Car(i, len(path), path)
            cars[i] = car
            for street in car.path:
                street.used = True

        return cls(duration, intersections, streets, cars, street_mapping, bonus)

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
        time = 0
        event_queue = StablePriorityQueue(maxsize=1000)
        for car in self.cars:
            street = car.current_street()
            street.queueing_cars.put(car)
            if street.queueing_cars.qsize() == 1:
                event_queue.put((street.next_green(time), street))

        while not event_queue.empty():
            time, object = event_queue.get()
            if time > self.duration:
                break
            if isinstance(object, Street):
                street = object
                car = street.go(time)
                event_queue.put((time + car.current_street().length, car))
                if not street.queueing_cars.empty():
                    event_queue.put((street.next_green(time), street))

            elif isinstance(object, Car):
                car = object
                if car.final_destination():
                    car.finished = True
                    car.finish_time = time
                else:
                    street = car.current_street()
                    street.queueing_cars.put(car)
                    if street.queueing_cars.qsize() == 1:
                        event_queue.put((street.next_green(time), street))              

        return self

#    def run(self):
#        for car in self.cars:
#            car.path[car.current_street_index].queueing_cars.put(car)
#            car.queueing = True
#
#            intersections_used = []
#            for intersection in self.intersections:
#                for street in intersection.incoming:
#                    if street.used:
#                        intersections_used.append(intersection)
#                        break
#
#        for t in range(self.duration):
#            #print(t)
#            for intersection in intersections_used:
#                street = intersection.green(t)
#                if not street.queueing_cars.empty():
#                    car = street.queueing_cars.get()
#                    car.queueing = False
#                    car.current_street_index += 1
#                    car.current_distance = 0
#
#            for car in self.cars:
#                car.move(t)
#
#        return self

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

    def read_plan(self, filename):
        with open(filename, 'r') as file:
            num_of_intersections = int(file.readline())
            for _ in range(num_of_intersections):
                intersection_id = int(file.readline())
                num_of_streets = int(file.readline())

                intersection = self.intersections[intersection_id]
                for _ in range(num_of_streets):
                    street_name, green_time = file.readline().split()

                    street = self.street_mapping[street_name]
                    intersection.plan.extend([street] * int(green_time))
                    
        return self

    def create_plan_default(self):
        """
        For every intersection and every incoming street, set green for 1 second.
        """
        for intersection in self.intersections:
            intersection.plan.extend(intersection.incoming)

        return self

    def create_plan_used(self):
        """
        Set green for 1 second for every used incoming street. 
        Exclude streets that aren't used.
        """
        # TODO: Maybe not ideal, try to fix this
        for intersection in self.intersections:
            for street in intersection.incoming:
                if street.used:
                    intersection.plan.append(street)

        return self

    def create_output(self, filename):
        if filename is not None:
            with open(filename, 'w') as file:
                self._create_output(file)
        else:
            self._create_output(sys.stdout)

    def _create_output(self, file):
        # TODO: Not ideal, try to come up with a better solution
        intersections_used = []
        for intersection in self.intersections:
            if intersection.plan:
                intersections_used.append(intersection)

        file.write(str(len(intersections_used)) + '\n')


        for intersection in intersections_used:
            file.write(str(intersection.id) + '\n')
            counter = Counter(intersection.plan)
            file.write(str(len(counter)) + '\n')
            for street, time in counter.items():
                file.write(f'{street.name} {str(time)}\n')