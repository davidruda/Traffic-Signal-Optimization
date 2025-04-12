import os
import unittest
import warnings

from optimizer import Optimizer, parser

class TestOptimizer(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.args = [
            'c', # dataset
            '--indpb=0.0005',
            f'--threads={max(2, os.cpu_count())}',
            '--no-save',
        ]
        cls.algorithm_args = {
            'hc': ['--population=4', '--generations=100'],
            'sa': ['--population=4', '--generations=100'],
            'ga': ['--population=100', '--generations=10'],
        }
        cls.algorithm = {
            'hc': 'Hill Climbing',
            'sa': 'Simulated Annealing',
            'ga': 'Genetic Algorithm',
        }
        # Don't show warnings from DEAP about redefining classes
        warnings.filterwarnings(
            'ignore',
            message='A class named \'(FitnessMax|Individual)\' has already been created',
            category=RuntimeWarning
        )

    def setUp(self):
        ...

    def _run_algorithm(self, alg):
        print('\n' + f' {self.algorithm[alg]} '.center(70, '-'))
        args = parser.parse_args([
            alg,
            *self.args,
            *self.algorithm_args[alg],
        ])
        optimizer = Optimizer(args)
        optimizer.run(save_statistics=False)

    def test_hill_climbing(self):
        self._run_algorithm('hc')

    def test_simulated_annealing(self):
        self._run_algorithm('sa')

    def test_genetic_algorithm(self):
        self._run_algorithm('ga')


if __name__ == '__main__':
    unittest.main()
