import argparse
import os
import time
import unittest
import warnings

from parameterized import parameterized

from optimizer import Optimizer, parser

class TestOptimizer(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.args = [
            '--algorithm=ga',
            '--generations=10',
            f'--threads={max(2, os.cpu_count())}',
            '--no-save'
        ]
        # Don't show warnings from DEAP about redefining classes
        warnings.filterwarnings(
            'ignore',
            message='A class named \'(FitnessMax|Individual)\' has already been created',
            category=RuntimeWarning
        )

    def setUp(self):
        ...

    @parameterized.expand([
        ('a'),
        ('b'),
        ('c'),
        ('d'),
        ('e'),
        ('f')
    ])
    def test_genetic_algorithm(self, data):
        print('\n' + f' DATA {data} '.center(70, '-'))
        args = parser.parse_args(self.args + [f'--data={data}'])
        optimizer = Optimizer(args)
        optimizer.run(save_statistics=False)


if __name__ == '__main__':
    unittest.main()