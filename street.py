
from queue import Queue
from intersection import Intersection

class Street:
    def __init__(self, id: int, start: Intersection, end: Intersection, name: str, length: int):
        self.id = id
        self.start = start
        self.end = end
        self.name = name
        self.length = length
        self.used = False
        self.queueing_cars = Queue(maxsize=1000)
        self._last_used_time = None
        self._green_interval = None

    def __str__(self):
        return f'{self.id} {self.name} {self.start.id} -> {self.end.id} length: {self.length}'

    def next_green(self, time):
        """
        Returns the next time when the street has a green light.
        """
        if time == self._last_used_time:
            time += 1
        time_index = time % len(self.end.plan)
        if time_index in self._green_interval:
            return time
        if time_index < self._green_interval.start:
            return time + self._green_interval.start - time_index
        return time + len(self.end.plan) - time_index + self._green_interval.start

    def go(self, time):
        """
        Returns a car that goes through the end intersection at time.
        """
        car = self.queueing_cars.get()
        car.move_to_next_street()
        self._last_used_time = time
        return car