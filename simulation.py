
from intersection import Intersection
from street import Street
from car import Car

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

        return self

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

    def create_default_plan(self):
        """
        For every intersection and every incoming street, set green for 1 second.
        """
        for intersection in self.intersections:
            intersection.plan.extend(intersection.incoming)

        return self