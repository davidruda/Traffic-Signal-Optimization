import argparse
import os
import unittest

from traffic_signaling.city_plan import CityPlan
from traffic_signaling.simulation import Simulation

DEFAULT_INPUT_FOLDER = 'traffic_signaling/input_data'
DEFAULT_OUTPUT_FOLDER = 'output'
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
parser.add_argument(
    '--plan-files',
    nargs=6,
    default=list(map(
        lambda f: os.path.join(DEFAULT_OUTPUT_FOLDER, f),
        ['a.txt', 'b.txt', 'c.txt', 'd.txt', 'e.txt', 'f.txt']
    )),
    help='Files with plan: a, b, c, d, e, f respectively.'
)
args = parser.parse_args()


class TestIO(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.data = [
            {'city_plan': CityPlan(args.input_files[0]), 'plan_file': args.plan_files[0]},
            {'city_plan': CityPlan(args.input_files[1]), 'plan_file': args.plan_files[1]},
            {'city_plan': CityPlan(args.input_files[2]), 'plan_file': args.plan_files[2]},
            {'city_plan': CityPlan(args.input_files[3]), 'plan_file': args.plan_files[3]},
            {'city_plan': CityPlan(args.input_files[4]), 'plan_file': args.plan_files[4]},
            {'city_plan': CityPlan(args.input_files[5]), 'plan_file': args.plan_files[5]}
        ]

    def test_io(self):
        os.makedirs(DEFAULT_OUTPUT_FOLDER, exist_ok=True)
        for d in self.data:
            simulation_0 = Simulation(d['city_plan'])
            simulation_0.create_plan_default()
            simulation_0.write_plan(d['plan_file'])
            score_0 = simulation_0.run().score()

            simulation_1 = Simulation(d['city_plan'])
            simulation_1.read_plan(d['plan_file'])
            score_1 = simulation_1.run().score()

            self.assertEqual(score_0, score_1)

    def test_io_same_instance(self):
        os.makedirs(DEFAULT_OUTPUT_FOLDER, exist_ok=True)
        for d in self.data:
            simulation = Simulation(d['city_plan'])
            simulation.create_plan_default()
            simulation.write_plan(d['plan_file'])
            score_0 = simulation.run().score()

            simulation.read_plan(d['plan_file'])
            score_1 = simulation.run().score()

            self.assertEqual(score_0, score_1)


if __name__ == '__main__':
    unittest.main()
