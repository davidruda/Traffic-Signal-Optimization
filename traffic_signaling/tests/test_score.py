import unittest

from parameterized import parameterized

from _resolve_imports import *

class TestScore(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        ...

    @classmethod
    def tearDownClass(cls):
        print(f'\n{"-" * 70}')
        print(f'TOTAL DEFAULT SCORE:\t{sum(DEFAULT_SCORE.values()):12,}')
        print(f'TOTAL ADAPTIVE SCORE:\t{sum(ADAPTIVE_SCORE.values()):12,}')
        print(f'TOTAL MAX KNOWN SCORE:\t{sum(MAX_KNOWN_SCORE.values()):12,}')
        print(f'TOTAL UPPER BOUND:\t{sum(UPPER_BOUND.values()):12,}')

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
        self.assertEqual(score, ADAPTIVE_SCORE[data])

        set_seed(42)
        simulation.random_schedules()
        score = simulation.score()
        print('\n' + ' random_schedules '.center(70, '*'))
        simulation.summary()
        set_seed(42)
        simulation.random_schedules()
        self.assertEqual(score, simulation.score())

        simulation.scaled_schedules(divisor=BEST_DIVISOR[data])
        score = simulation.score()
        print('\n' + ' scaled_schedules '.center(70, '*'))
        simulation.summary()
        self.assertEqual(score, simulation.score())

    @parameterized.expand([
        ('a'),
        ('b'),
        ('c'),
        ('d'),
        ('e'),
        ('f')
    ])
    def test_upper_bound(self, data):
        plan = create_city_plan(data)
        upper_bound = plan.upper_bound()
        self.assertEqual(upper_bound, UPPER_BOUND[data])

if __name__ == '__main__':
    unittest.main()
