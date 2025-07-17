from typing import Literal

from .city_plan import CityPlan

class Schedule:
    """
    Schedule of traffic lights for one intersection in the simulation.
    """

    DEFAULT_DIVISOR: int = 27
    "Default divisor for scaled times schedule option."

    @property
    def length(self) -> int:
        """
        Return the number of streets in the schedule.
        """
        ...

    @property
    def duration(self) -> int:
        """
        Return the cycle duration of this schedule.
        """
        ...

    @property
    def order(self) -> list[int]:
        """
        Return the order of streets in the schedule.

        The streets are represented by their IDs.
        """
        ...

    @property
    def times(self) -> list[int]:
        """
        Return the green light times for each street in the order.
        """
        ...

    def relative_order(self) -> list[int]:
        """
        Return the order of streets in the schedule.

        The streets are represented by their indices relative to the intersection.

        This method is only valid if the schedule contains used streets.
        """
        ...

    def set(self, order: list[int], times: list[int], relative_order: bool = False) -> None:
        """
        Set the schedule using the given order and times.

        :param order: Order of streets in the schedule.
        :param times: Green light times for each street in the order.
        :param relative_order: If True, `order` must be a list of street indices relative to the intersection.
        Otherwise, `order` must be a list of street IDs.
        """
        ...

def set_seed(seed: int) -> None:
    """
    Set the random seed used for schedules generation.

    :param seed: Value of the random seed.
    """
    ...

class Simulation:
    """
    Simulation of the Traffic signaling problem for a given city plan.
    """
    def __init__(self, city_plan: CityPlan) -> None:
        """
        Create a simulation object for the given city plan.

        :param city_plan: City plan containing information from the input file.
        """
        ...

    def load_schedules(self, filename: str) -> None:
        """
        Load and create schedules from the given file.

        :param filename: Path of the file containing schedules in the competition format.
        """
        ...

    def save_schedules(self, filename: str) -> None:
        """
        Save the current schedules in the competition format to the given file.

        :param filename: Path of the file to save the schedules to.
        """
        ...

    def create_schedules(
        self, order: Literal['default', 'adaptive', 'random'], times: Literal['default', 'scaled'],
        divisor: int = Schedule.DEFAULT_DIVISOR,
    ) -> None:
        """
        Create schedules for all used intersections and used streets.

        :param order: Initialization option for the order of streets in the schedules.
        :param times: Initialization option for the green light times in the schedules.
        :param divisor: Divisor for the scaled times option.
        """
        ...

    def default_schedules(self) -> None:
        """
        Create default schedules.

        This is an alias for `create_schedules('default', 'default')`.
        """
        ...

    def adaptive_schedules(self) -> None:
        """
        Create adaptive schedules.

        This is an alias for `create_schedules('adaptive', 'default')`.
        """
        ...

    def random_schedules(self) -> None:
        """
        Create random schedules.

        This is an alias for `create_schedules('random', 'default')`.
        """
        ...

    def scaled_schedules(self, divisor: int = Schedule.DEFAULT_DIVISOR) -> None:
        """
        Create scaled schedules.

        This is an alias for `create_schedules('default', 'scaled', divisor)`.

        :param divisor: Divisor for the scaled times option.
        """
        ...

    def non_trivial_schedules(self, relative_order: bool = False) -> list[tuple[list[int], list[int]]]:
        """
        Return the schedules of non-trivial intersections as a list of `(order, times)` tuples,
        where each tuple is a schedule for one intersection.

        :param relative_order: If True, `order` is a list of street indices relative to each intersection.
        Otherwise, `order` is a list of street IDs.
        """
        ...

    def set_non_trivial_schedules(
        self, schedules: list[tuple[list[int], list[int]]], relative_order: bool = False
    ) -> None:
        """
        Set the schedules of non-trivial intersections.

        :param schedules: List of `(order, times)` tuples, where each tuple is a schedule for one intersection.
        :param relative_order: If True, `order` must be a list of street indices relative to each intersection.
        Otherwise, `order` must be a list of street IDs.

        This method assumes the input is valid and performs no validation.
        """
        ...

    def score(self) -> int:
        """
        Calculate the score for the current setting of schedules.

        This method runs the simulation.
        """
        ...

    def summary(self) -> None:
        """
        Print a summary of the simulation statistics.
        """
        ...

    @property
    def schedules(self) -> dict[int, Schedule]:
        """
        Return the current schedules as a dictionary indexed by intersection IDs.
        """
        ...

def default_simulation(city_plan: CityPlan) -> Simulation:
    """
    Create a simulation with default schedules for the given city plan.

    This is a shortcut for creating a simulation object and calling its `default_schedules` method.

    :param city_plan: City plan containing information from the input file.
    """
    ...

def adaptive_simulation(city_plan: CityPlan) -> Simulation:
    """
    Create a simulation with adaptive schedules for the given city plan.

    This is a shortcut for creating a simulation object and calling its `adaptive_schedules` method.

    :param city_plan: City plan containing information from the input file.
    """
    ...
