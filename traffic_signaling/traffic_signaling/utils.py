from importlib.resources import files

try:
    from .city_plan import CityPlan
    _anchor_str = 'traffic_signaling.data'
except ImportError:
    from city_plan import CityPlan
    _anchor_str = 'data'

# Short names of the datasets.
TEST_DATA = ['a', 'b', 'c', 'd', 'e', 'f']
# Long names of the datasets.
DATA_NICKNAMES = {
    'a': 'An example',
    'b': 'By the ocean',
    'c': 'Checkmate',
    'd': 'Daily commute',
    'e': 'Etoile',
    'f': 'Forever jammed'
}

# Score for each dataset with default schedules - default order and default times.
DEFAULT_SCORE = {
    'a': 1_001,
    'b': 4_566_576,
    'c': 1_299_357,
    'd': 1_573_100,
    'e': 684_769,
    'f': 819_083
}

# Score for each dataset with adaptive schedules - adaptive order and default times.
ADAPTIVE_SCORE = {
    'a': 2_002,
    'b': 4_568_568,
    'c': 1_305_874,
    'd': 2_477_913,
    'e': 702_570,
    'f': 824_879
}

# Maximum known score for each dataset.
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

# Number of parameters for each dataset to optimize.
#
# Computed as twice the sum of used streets in all non-trivial intersections.
# Unused and trivial intersections are not counted because they don't need to be optimized.
# The factor of 2 is used because each street has two parameters - order and time (duration).
PARAMETERS = {
    'a': 4,
    'b': 5_974,
    'c': 14_008,
    'd': 167_748,
    'e': 1_386,
    'f': 10_002
}

# Best divisor (with the highest score) for scaled schedules for each dataset.
BEST_DIVISOR = {
    'a': 1,
    'b': 5,
    'c': 11,
    'd': 5,
    'e': 2,
    'f': 27,
}

def normalized_score(score: int, data: str) -> float:
    """
    Normalize the absolute score value between 0 and 1 for the given dataset.

    :param score: Absolute score value to normalize.
    :param data: Dataset name; ('a', 'b', 'c', 'd', 'e', 'f').

    The score is normalized between the default score (baseline) and the maximum known score.
    """
    return (score - DEFAULT_SCORE[data]) / (MAX_KNOWN_SCORE[data] - DEFAULT_SCORE[data])

def absolute_score(norm_score: float, data: str) -> int:
    """
    Convert the normalized score back to the absolute score for the given dataset.

    :param norm_score: Normalized score value between 0 and 1.
    :param data: Dataset name; ('a', 'b', 'c', 'd', 'e', 'f').
    """
    return int(norm_score * (MAX_KNOWN_SCORE[data] - DEFAULT_SCORE[data]) + DEFAULT_SCORE[data])

def get_data_filename(data: str) -> str:
    """
    Return the path to the data file for the given dataset.

    :param data: Dataset name; ('a', 'b', 'c', 'd', 'e', 'f').
    """
    data = data.lower()
    if data not in TEST_DATA:
        msg = f'Invalid data! Possible values are {", ".join(TEST_DATA)}'
        raise ValueError(msg)
    # https://setuptools.pypa.io/en/latest/userguide/datafiles.html#accessing-data-files-at-runtime
    return str(files(_anchor_str).joinpath(f'{data}.txt'))

def create_city_plan(data: str) -> CityPlan:
    """
    Create a CityPlan object from the given dataset.

    :param data: Dataset name; ('a', 'b', 'c', 'd', 'e', 'f').
    """
    return CityPlan(get_data_filename(data))
