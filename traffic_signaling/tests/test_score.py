import argparse
import os
import unittest

from traffic_signaling.city_plan import CityPlan
from traffic_signaling.simulation import Simulation

DEFAULT_INPUT_FOLDER = 'traffic_signaling/data'
parser = argparse.ArgumentParser()
parser.add_argument(
    '--input-files',
    nargs=6,
    default=list(map(
        lambda f: os.path.join(DEFAULT_INPUT_FOLDER, f),
        ['a.txt', 'b.txt', 'c.txt', 'd.txt', 'e.txt', 'f.txt']
    )),
    help='Input data files: a, b, c, d, e, f respectively.'
)
args = parser.parse_args()


class TestScore(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.data = [
            {'city_plan': CityPlan(args.input_files[0]), 'score': 1_001},
            {'city_plan': CityPlan(args.input_files[1]), 'score': 4_566_576},
            {'city_plan': CityPlan(args.input_files[2]), 'score': 1_299_357},
            {'city_plan': CityPlan(args.input_files[3]), 'score': 1_573_100},
            {'city_plan': CityPlan(args.input_files[4]), 'score': 684_769},
            {'city_plan': CityPlan(args.input_files[5]), 'score': 819_083}
        ]

    def test_score_default(self):
        for d in self.data:
            simulation = Simulation(d['city_plan'])
            simulation.create_plan_default()
            score = simulation.score(verbose=True)
            print('-' * 70)
            self.assertEqual(score, d['score'])

        print(f'TOTAL SUM: {sum(d["score"] for d in self.data):,}')


if __name__ == '__main__':
    unittest.main()
