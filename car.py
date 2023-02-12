
from street import Street

class Car:
    def __init__(self, id: int, path_length: int, path: list[Street]):
        self.id = id
        self.path_length = path_length
        self.path = path
        self._current_street_index = 0
        self.finished = False
        self.finish_time = None

    def __str__(self):
        return f'{self.id} length: {self.path_length} ' #+ ' '.join(self.path)

    def is_final_destination(self):
        return self.path[-1] == self.path[self._current_street_index]

    def current_street(self):
        return self.path[self._current_street_index]

    def move_to_next_street(self):
        self._current_street_index += 1
        return self
