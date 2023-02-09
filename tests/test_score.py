
import argparse
import os
import sys
import unittest

# Add parent directory to path
sys.path.append('.')

from main import read_input

class TestScore(unittest.TestCase):

    def setUp(self):
        self.data = [
            {'input': read_input(args.input_a), 'output': 1_001},
            {'input': read_input(args.input_b), 'output': 4_565_642},
            {'input': read_input(args.input_c), 'output': 1_231_878},
            {'input': read_input(args.input_d), 'output': 969_685},
            {'input': read_input(args.input_e), 'output': 661_797},
            {'input': read_input(args.input_f), 'output': 455_737}
        ]

    def test_default_solution(self):
        for data in self.data:
            score = data['input'].create_default_plan().run().score()
            self.assertEqual(score, data['output'])

if __name__ == '__main__':
    DEFAULT_INPUT_FOLDER = 'input_data'
    parser = argparse.ArgumentParser()
    parser.add_argument('--input-a', default=os.path.join(DEFAULT_INPUT_FOLDER, 'a_an_example.in.txt'), help='Test input a.')
    parser.add_argument('--input-b', default=os.path.join(DEFAULT_INPUT_FOLDER, 'b_by_the_ocean.in.txt'), help='Test input b.')
    parser.add_argument('--input-c', default=os.path.join(DEFAULT_INPUT_FOLDER, 'c_checkmate.in.txt'), help='Test input c.')
    parser.add_argument('--input-d', default=os.path.join(DEFAULT_INPUT_FOLDER, 'd_daily_commute.in.txt'), help='Test input d.')
    parser.add_argument('--input-e', default=os.path.join(DEFAULT_INPUT_FOLDER, 'e_etoile.in.txt'), help='Test input e.')
    parser.add_argument('--input-f', default=os.path.join(DEFAULT_INPUT_FOLDER, 'f_forever_jammed.in.txt'), help='Test input f.')
    args = parser.parse_args()
    unittest.main()