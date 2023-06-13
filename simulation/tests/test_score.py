import argparse
import os
import unittest

from simulation import *

DEFAULT_INPUT_FOLDER = 'input_data'
parser = argparse.ArgumentParser()
parser.add_argument('--input-a', default=os.path.join(DEFAULT_INPUT_FOLDER, 'a.txt'), help='Input file a.')
parser.add_argument('--input-b', default=os.path.join(DEFAULT_INPUT_FOLDER, 'b.txt'), help='Input file b.')
parser.add_argument('--input-c', default=os.path.join(DEFAULT_INPUT_FOLDER, 'c.txt'), help='Input file c.')
parser.add_argument('--input-d', default=os.path.join(DEFAULT_INPUT_FOLDER, 'd.txt'), help='Input file d.')
parser.add_argument('--input-e', default=os.path.join(DEFAULT_INPUT_FOLDER, 'e.txt'), help='Input file e.')
parser.add_argument('--input-f', default=os.path.join(DEFAULT_INPUT_FOLDER, 'f.txt'), help='Input file f.')
args = parser.parse_args()


class TestScore(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.data = [
            {'simulation': Simulation(args.input_a), 'score': 1_001},
            {'simulation': Simulation(args.input_b), 'score': 4_566_576},
            {'simulation': Simulation(args.input_c), 'score': 1_299_357},
            {'simulation': Simulation(args.input_d), 'score': 1_573_100},
            {'simulation': Simulation(args.input_e), 'score': 684_769},
            {'simulation': Simulation(args.input_f), 'score': 819_083}
        ]

    # def test_default_solution(self):
    #    data = [
    #        {'input': Simulation(args.input_a), 'output': 1_001},
    #        {'input': Simulation(args.input_b), 'output': 4_565_642},
    #        {'input': Simulation(args.input_c), 'output': 1_231_878},
    #        {'input': Simulation(args.input_d), 'output': 969_685},
    #        {'input': Simulation(args.input_e), 'output': 661_797},
    #        {'input': Simulation(args.input_f), 'output': 455_737}
    #    ]
    #    for d in data:
    #        score = d['input'].create_plan_default().run().score(verbose=True)
    #        self.assertEqual(score, d['output'])
    #    print(f'TOTAL SUM: {sum(d["output"] for d in data):,}')

    def test_score_default(self):
        for d in self.data:
            simulation_instance = d['simulation'].create_instance()
            simulation_instance.create_plan_default()
            score = simulation_instance.run().score(verbose=True)
            self.assertEqual(score, d['score'])
        print()
        print(f'TOTAL SUM: {sum(d["score"] for d in self.data):,}')


if __name__ == '__main__':
    unittest.main()
