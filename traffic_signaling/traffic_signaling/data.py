import os

try:
    from .city_plan import *
except ImportError:
    from city_plan import *

TEST_DATA = ['a', 'b', 'c', 'd', 'e', 'f']
DEFAULT_SCORE = {
    'a': 1_001,
    'b': 4_566_576,
    'c': 1_299_357,
    'd': 1_573_100,
    'e': 684_769,
    'f': 819_083
}

# Sources:
# https://github.com/sagishporer/hashcode-2021-qualification#score
# (https://codeforces.com/blog/entry/88188#comment-768121)
MAX_KNOWN_SCORE = {
    'a': 2_002,
    'b': 4_570_431,
    'c': 1_315_372,
    'd': 2_610_027,
    'e': 779_279,
    'f': 1_480_489
}

# Theoretical maximum score if none of the cars
# ever has to wait at a traffic light.
UPPER_BOUND = {
    'a': 2_002,
    'b': 4_576_202,
    'c': 1_328_389,
    'd': 3_986_591,
    'e': 921_203,
    'f': 1_765_068
}

def get_data_filename(data: str) -> str:
    data = data.lower()
    if data not in TEST_DATA:
        msg = f'Invalid data! Possible values are {", ".join(TEST_DATA)}'
        raise ValueError(msg)
    return os.path.join(os.path.dirname(__file__), 'data', f'{data}.txt')

def calculate_upper_bound(data: str) -> int:
    """
    Theoretical maximum score if none of the cars
    ever has to wait at a traffic light.
    """
    def get_path(path_ids):
        return [streets[id] for id in path_ids]  
              
    def path_duration(path_ids):
        path = get_path(path_ids)
        # The car path begins at the end of the first street
        return sum(street.length for street in path[1:])
        
    def car_score(car):
        return city_plan.bonus + city_plan.duration - path_duration(car.path)

    city_plan = CityPlan(get_data_filename(data))
    streets = city_plan.streets
    upper_bound = sum([
        car_score(c) for c in city_plan.cars if car_score(c) > 0
    ])
    return upper_bound