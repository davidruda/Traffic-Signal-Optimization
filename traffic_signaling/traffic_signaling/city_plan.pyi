class Car:
    """
    Car in the city plan.

    Contains information about a car known from the input data.
    """
    @property
    def id(self) -> int:
        """
        Return the ID of the car.
        """
        ...

    @property
    def path(self) -> list[Street]:
        """
        Return the path the car has to travel.
        """
        ...

    def path_duration(self) -> int:
        """
        Return the duration in seconds to travel the path without waiting at traffic lights.
        """
        ...

class Intersection:
    """
    Intersection in the city plan.

    Contains information about an intersection known from the input data.
    """
    @property
    def id(self) -> int:
        """
        Return the ID of the intersection.
        """
        ...

    @property
    def streets(self) -> list[Street]:
        """
        Return the incoming streets to the intersection.
        """
        ...

    @property
    def used_streets(self) -> list[Street]:
        """
        Return the used incoming streets to the intersection.
        """
        ...

    @property
    def used(self) -> bool:
        """
        Return True if the intersection is used.

        Used intersection has at least one used street.
        """
        ...

    @property
    def non_trivial(self) -> bool:
        """
        Return True if the intersection is non-trivial.

        Non-trivial intersection has two or more used streets.
        """
        ...

class Street:
    """
    Street in the city plan.

    Contains information about a street known from the input data.
    """
    @property
    def id(self) -> int:
        """
        Return the ID of the street.
        """
        ...

    @property
    def name(self) -> str:
        """
        Return the name of the street.
        """
        ...

    @property
    def used(self) -> bool:
        """
        Return True if the street is used.

        A street is used if it is a non-final street in the path of at least one car.
        """
        ...

    @property
    def length(self) -> int:
        """
        Return the duration in seconds to drive through the street.
        """
        ...

    @property
    def start(self) -> Intersection:
        """
        Return the starting intersection of the street.
        """
        ...

    @property
    def end(self) -> Intersection:
        """
        Return the ending intersection of the street.
        """
        ...

    @property
    def total_cars(self) -> int:
        """
        Return the total number of cars that use this street.
        """
        ...

class CityPlan:
    """
    City plan of a given dataset.

    Contains all information about intersections, streets, and cars known from the input data.
    """
    def __init__(self, filename: str) -> None:
        """
        Create a city plan from the input data file.

        :param filename: Path of the input data file.
        """
        ...

    @property
    def intersections(self) -> list[Intersection]:
        """
        Return the intersections in the city plan.
        """
        ...

    @property
    def streets(self) -> list[Street]:
        """
        Return the streets in the city plan.
        """
        ...

    @property
    def cars(self) -> list[Car]:
        """
        Return the cars in the city plan.
        """
        ...

    @property
    def duration(self) -> int:
        """
        Return the duration of the simulation for this city plan.
        """
        ...

    @property
    def bonus(self) -> int:
        """
        Return the bonus awarded for each car that reaches its destination before the end of the simulation.
        """
        ...

    def used_intersections(self) -> list[Intersection]:
        """
        Return the used intersections in this city plan.
        """
        ...

    def non_trivial_intersections(self) -> list[Intersection]:
        """
        Return the non-trivial intersections in this city plan.
        """
        ...

    def upper_bound(self) -> int:
        """
        Return the theoretical maximum score if none of the cars ever has to wait at a traffic light.
        """
        ...
