import unittest

from parameterized import parameterized

from _resolve_imports import *

class TestScore(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        ...

    @classmethod
    def tearDownClass(cls):
        total_score = sum(DEFAULT_SCORE.values())
        total_upper_bound = sum(UPPER_BOUND.values())
        print(f'\n{"-" * 70}')
        print(f'TOTAL DEFAULT SCORE:\t{total_score:12,}')
        print(f'TOTAL UPPER BOUND:\t{total_upper_bound:12,}')

    @parameterized.expand([
        ('a'),
        ('b'),
        ('c'),
        ('d'),
        ('e'),
        ('f')
    ])
    def test_default_score(self, data):
        city_plan = CityPlan(get_data_filename(data))
        simulation = Simulation(city_plan)
        simulation.default_schedules()
        print(f'\n{"-" * 31} DATA {data} {"-" * 31}')
        score = simulation.score()
        simulation.summary()
        self.assertEqual(score, DEFAULT_SCORE[data])

    @parameterized.expand([
        ('a'),
        ('b'),
        ('c'),
        ('d'),
        ('e'),
        ('f')
    ])
    def test_upper_bound(self, data):
        upper_bound = calculate_upper_bound(data)
        self.assertEqual(upper_bound, UPPER_BOUND[data])

if __name__ == '__main__':
    unittest.main()
