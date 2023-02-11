
from street import Street

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

    def current_street(self):
        return self.path[self.current_street_index]

    def move_to_next_street(self):
        self.current_street_index += 1
        return self

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
