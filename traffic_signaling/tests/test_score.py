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
    def test_scores(self, data):
        plan = create_city_plan(data)
        simulation = default_simulation(plan)
        print('\n' + f' DATA {data} '.center(70, '-'))
        score = simulation.score()
        print(' default_schedules '.center(70, '*'))
        simulation.summary()
        self.assertEqual(score, DEFAULT_SCORE[data])
        simulation.adaptive_schedules()
        score = simulation.score()
        print('\n' + ' adaptive_schedules '.center(70, '*'))
        simulation.summary()

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
