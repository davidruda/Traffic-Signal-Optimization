import unittest

from parameterized import parameterized

from _resolve_imports import *

class TestScore(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.scores = {
            'a': 1_001,
            'b': 4_566_576,
            'c': 1_299_357,
            'd': 1_573_100,
            'e': 684_769,
            'f': 819_083
        }
        cls.calculated_scores = {}

    @classmethod
    def tearDownClass(cls):
        total_score = sum(score for score in cls.calculated_scores.values())
        print(f'\n{"-" * 70}')
        print(f'TOTAL SCORE: {total_score:,}')

    @parameterized.expand([
        ('a'),
        ('b'),
        ('c'),
        ('d'),
        ('e'),
        ('f')
    ])
    def test_score_default(self, data):
        city_plan = CityPlan(get_data_filename(data))
        simulation = Simulation(city_plan)
        simulation.create_plan_default()
        print(f'\n{"-" * 31} DATA {data} {"-" * 31}')
        score = simulation.score(verbose=True)
        self.assertEqual(score, self.scores[data])
        self.calculated_scores[data] = score

if __name__ == '__main__':
    unittest.main()
