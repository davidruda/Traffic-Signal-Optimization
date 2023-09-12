import os
import shutil
import time
import unittest

from parameterized import parameterized

from _resolve_imports import *

class TestTime(unittest.TestCase):
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
    def test_time(self, data):
        input = get_data_filename(data)
        output = f'{self.output_dir}/{data}.txt'

        print(f'\n{"-" * 31} DATA {data} {"-" * 31}')
        start = time.time()
        city_plan = CityPlan(input)
        print(f'CityPlan constructor: {time.time() - start:.4f}s')

        start = time.time()
        simulation = Simulation(city_plan)
        print(f'Simulation constructor: {time.time() - start:.4f}s')

        start = time.time()
        simulation.default_schedules()
        print(f'default_schedules: {time.time() - start:.4f}s')

        start = time.time()
        simulation.write_schedules(output)
        print(f'write_schedules: {time.time() - start:.4f}s')

        start = time.time()
        simulation.read_schedules(output)
        print(f'read_schedules: {time.time() - start:.4f}s')

        start = time.time()
        simulation.score()
        print(f'score: {time.time() - start:.4f}s')

if __name__ == '__main__':
    unittest.main()
