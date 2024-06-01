import os
import shutil
import unittest

from parameterized import parameterized

from _resolve_imports import *

class TestIO(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        test_name = __file__.split(os.sep)[-1].removesuffix('.py')
        cls.output_dir = f'{os.path.dirname(__file__)}{os.sep}out_{test_name}'
        os.makedirs(cls.output_dir, exist_ok=True)

    @classmethod
    def tearDownClass(cls):
        shutil.rmtree(cls.output_dir)
    
    @parameterized.expand([
        ('a'),
        ('b'),
        ('c'),
        ('d'),
        ('e'),
        ('f')
    ])
    def test_io(self, data):
        output = f'{self.output_dir}/{data}.txt'

        plan = create_city_plan(data)
        for schedule_option in ['default', 'adaptive']:
            simulation = Simulation(plan)
            getattr(simulation, f'{schedule_option}_schedules')()

            simulation.save_schedules(output)
            score = simulation.score()

            simulation = Simulation(plan)
            simulation.load_schedules(output)
            self.assertEqual(score, simulation.score())

    @parameterized.expand([
        ('a'),
        ('b'),
        ('c'),
        ('d'),
        ('e'),
        ('f')
    ])
    def test_io_same_instance(self, data):
        output = f'{self.output_dir}/{data}.txt'

        plan = create_city_plan(data)
        for schedule_option in ['default', 'adaptive']:
            simulation = Simulation(plan)
            getattr(simulation, f'{schedule_option}_schedules')()

            simulation.save_schedules(output)
            score = simulation.score()

            simulation.load_schedules(output)
            self.assertEqual(score, simulation.score())

if __name__ == '__main__':
    unittest.main()
