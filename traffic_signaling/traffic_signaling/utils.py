import os

try:
    from .city_plan import CityPlan
except ImportError:
    from city_plan import CityPlan


TEST_DATA = ['a', 'b', 'c', 'd', 'e', 'f']
DEFAULT_SCORE = {
    'a': 1_001,
    'b': 4_566_576,
    'c': 1_299_357,
    'd': 1_573_100,
    'e': 684_769,
    'f': 819_083
}

ADAPTIVE_SCORE = {
    'a': 2_002,
    'b': 4_568_568,
    'c': 1_305_874,
    'd': 2_477_913,
    'e': 702_570,
    'f': 824_879
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

def normalized_score(score: int, data: str) -> float:
    score_norm = (score - DEFAULT_SCORE[data]) / (MAX_KNOWN_SCORE[data] - DEFAULT_SCORE[data])
    return round(score_norm, ndigits=2)

def get_data_filename(data: str) -> str:
    data = data.lower()
    if data not in TEST_DATA:
        msg = f'Invalid data! Possible values are {", ".join(TEST_DATA)}'
        raise ValueError(msg)
    return os.path.join(os.path.dirname(__file__), 'data', f'{data}.txt')

# factory function creating a CityPlan object
def create_city_plan(data: str) -> CityPlan:
    return CityPlan(get_data_filename(data))
