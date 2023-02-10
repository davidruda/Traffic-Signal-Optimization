
class Intersection:
    def __init__(self, id: int):
        self.id = id
        self.incoming = []
        self.outgoing = []
        self.plan = []
        self._last_used_time = -1
        self._cars_waiting = 0

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