import os
import shutil
import unittest

from parameterized import parameterized

from _resolve_imports import *

class TestIO(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.output_dir = f'{os.path.dirname(__file__)}/out'
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
        input = get_data_filename(data)
        output = f'{self.output_dir}/{data}.txt'

        city_plan = CityPlan(input)
        simulation = Simulation(city_plan)
        simulation.default_schedules()
        simulation.write_schedules(output)
        score = simulation.score()

        simulation = Simulation(city_plan)
        simulation.read_schedules(output)
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
        input = get_data_filename(data)
        output = f'{self.output_dir}/{data}.txt'

        city_plan = CityPlan(input)
        simulation = Simulation(city_plan)
        simulation.default_schedules()
        simulation.write_schedules(output)
        score = simulation.score()

        simulation.read_schedules(output)
        self.assertEqual(score, simulation.score())

if __name__ == '__main__':
    unittest.main()
