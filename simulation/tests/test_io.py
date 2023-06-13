import argparse
import os
import unittest

from simulation import *

DEFAULT_INPUT_FOLDER = 'input_data'
DEFAULT_OUTPUT_FOLDER = 'output'
parser = argparse.ArgumentParser()
parser.add_argument('--input-a', default=os.path.join(DEFAULT_INPUT_FOLDER, 'a.txt'), help='Input file a.')
parser.add_argument('--input-b', default=os.path.join(DEFAULT_INPUT_FOLDER, 'b.txt'), help='Input file b.')
parser.add_argument('--input-c', default=os.path.join(DEFAULT_INPUT_FOLDER, 'c.txt'), help='Input file c.')
parser.add_argument('--input-d', default=os.path.join(DEFAULT_INPUT_FOLDER, 'd.txt'), help='Input file d.')
parser.add_argument('--input-e', default=os.path.join(DEFAULT_INPUT_FOLDER, 'e.txt'), help='Input file e.')
parser.add_argument('--input-f', default=os.path.join(DEFAULT_INPUT_FOLDER, 'f.txt'), help='Input file f.')
parser.add_argument('--plan-a', default=os.path.join(DEFAULT_OUTPUT_FOLDER, 'a.txt'), help='Plan file a.')
parser.add_argument('--plan-b', default=os.path.join(DEFAULT_OUTPUT_FOLDER, 'b.txt'), help='Plan file b.')
parser.add_argument('--plan-c', default=os.path.join(DEFAULT_OUTPUT_FOLDER, 'c.txt'), help='Plan file c.')
parser.add_argument('--plan-d', default=os.path.join(DEFAULT_OUTPUT_FOLDER, 'd.txt'), help='Plan file d.')
parser.add_argument('--plan-e', default=os.path.join(DEFAULT_OUTPUT_FOLDER, 'e.txt'), help='Plan file e.')
parser.add_argument('--plan-f', default=os.path.join(DEFAULT_OUTPUT_FOLDER, 'f.txt'), help='Plan file f.')
args = parser.parse_args()


class TestIO(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.data = [
            {'simulation': Simulation(args.input_a), 'plan_file': args.plan_a},
            {'simulation': Simulation(args.input_b), 'plan_file': args.plan_b},
            {'simulation': Simulation(args.input_c), 'plan_file': args.plan_c},
            {'simulation': Simulation(args.input_d), 'plan_file': args.plan_d},
            {'simulation': Simulation(args.input_e), 'plan_file': args.plan_e},
            {'simulation': Simulation(args.input_f), 'plan_file': args.plan_f}
        ]

    def test_io(self):
        os.makedirs(DEFAULT_OUTPUT_FOLDER, exist_ok=True)
        for d in self.data:
            instance_0 = d['simulation'].create_instance()
            instance_0.create_plan_default()
            instance_0.write_plan(d['plan_file'])
            score_0 = instance_0.run().score()

            instance_1 = d['simulation'].create_instance()
            instance_1.read_plan(d['plan_file'])
            score_1 = instance_1.run().score()

            self.assertEqual(score_0, score_1)

    def test_io_same_instance(self):
        os.makedirs(DEFAULT_OUTPUT_FOLDER, exist_ok=True)
        for d in self.data:
            instance = d['simulation'].create_instance()
            instance.create_plan_default()
            instance.write_plan(d['plan_file'])
            score_0 = instance.run().score()

            instance.read_plan(d['plan_file'])
            score_1 = instance.run().score()

            self.assertEqual(score_0, score_1)


if __name__ == '__main__':
    unittest.main()
