import os

TEST_DATA = ['a', 'b', 'c', 'd', 'e', 'f']

def get_data_filename(data: str) -> str:
    data = data.lower()
    if data not in TEST_DATA:
        raise ValueError(f'Invalid data! Possible values are {", ".join(TEST_DATA)}')
    return f'{os.path.dirname(__file__)}/data/{data}.txt'