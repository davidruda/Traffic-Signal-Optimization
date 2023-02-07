
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
        self.queueing_cars = Queue()

    def __str__(self):
        return f'{self.id} {self.name} {self.start.id} -> {self.end.id} length: {self.length}'
