
import argparse
import os
import sys
import unittest

# Add parent directory to path
# TODO: Ugly - Find a better way to do this
sys.path.append('.')

from simulation import Simulation

class TestIO(unittest.TestCase):

    def test_input_output(self):
        data = [
            {'input': args.input_a, 'output': args.output_a},
            {'input': args.input_b, 'output': args.output_b},
            {'input': args.input_c, 'output': args.output_c},
            {'input': args.input_d, 'output': args.output_d},
            {'input': args.input_e, 'output': args.output_e},
            {'input': args.input_f, 'output': args.output_f}
        ]
        os.makedirs(DEFAULT_OUTPUT_FOLDER, exist_ok=True)
        for d in data:
            score_0 = Simulation \
                .from_file(d['input']) \
                .create_plan_default() \
                .create_output(d['output']) \
                .run() \
                .score()
            score_1 = Simulation \
                .from_file(d['input']) \
                .read_plan(d['output']) \
                .run() \
                .score()
            self.assertEqual(score_0, score_1)

if __name__ == '__main__':
    DEFAULT_INPUT_FOLDER = 'input_data'
    DEFAULT_OUTPUT_FOLDER = 'output'
    parser = argparse.ArgumentParser()
    parser.add_argument('--input-a', default=os.path.join(DEFAULT_INPUT_FOLDER, 'a_an_example.in.txt'), help='Test input a.')
    parser.add_argument('--input-b', default=os.path.join(DEFAULT_INPUT_FOLDER, 'b_by_the_ocean.in.txt'), help='Test input b.')
    parser.add_argument('--input-c', default=os.path.join(DEFAULT_INPUT_FOLDER, 'c_checkmate.in.txt'), help='Test input c.')
    parser.add_argument('--input-d', default=os.path.join(DEFAULT_INPUT_FOLDER, 'd_daily_commute.in.txt'), help='Test input d.')
    parser.add_argument('--input-e', default=os.path.join(DEFAULT_INPUT_FOLDER, 'e_etoile.in.txt'), help='Test input e.')
    parser.add_argument('--input-f', default=os.path.join(DEFAULT_INPUT_FOLDER, 'f_forever_jammed.in.txt'), help='Test input f.')
    parser.add_argument('--output-a', default=os.path.join(DEFAULT_OUTPUT_FOLDER, 'a_an_example.out.txt'), help='Test output a.')
    parser.add_argument('--output-b', default=os.path.join(DEFAULT_OUTPUT_FOLDER, 'b_by_the_ocean.out.txt'), help='Test output b.')
    parser.add_argument('--output-c', default=os.path.join(DEFAULT_OUTPUT_FOLDER, 'c_checkmate.out.txt'), help='Test output c.')
    parser.add_argument('--output-d', default=os.path.join(DEFAULT_OUTPUT_FOLDER, 'd_daily_commute.out.txt'), help='Test output d.')
    parser.add_argument('--output-e', default=os.path.join(DEFAULT_OUTPUT_FOLDER, 'e_etoile.out.txt'), help='Test output e.')
    parser.add_argument('--output-f', default=os.path.join(DEFAULT_OUTPUT_FOLDER, 'f_forever_jammed.out.txt'), help='Test output f.')
    args = parser.parse_args()
    unittest.main()