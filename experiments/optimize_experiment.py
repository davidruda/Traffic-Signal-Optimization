import argparse
import os
import platform
import subprocess

parser = argparse.ArgumentParser()
parser.add_argument('name', type=str, help='Name of the experiment.')
parser.add_argument('data', choices=['a', 'b', 'c', 'd', 'e', 'f'], help='Dataset of the experiment.')
parser.add_argument('parameters', type=str, help='All parameters for `optimizer.py` in one string.' )
parser.add_argument('--parallel', action='store_true', help='Run different seeds in parallel, not sequentially.')
args = parser.parse_args()

# Set how many and which seeds to use for the experiment
SEEDS = [42, 1, 1999, 27, 9, 123, 21, 11, 7, 16384]

if platform.system() == 'Windows':
    PYTHON = '../.venv/Scripts/python'
else:
    PYTHON = '../.venv/bin/python'

EXPERIMENT_DIR = f'logs/{args.data}/{args.name}'

processes = []
# Run command for each seed
for seed in SEEDS:
    logdir = f'{EXPERIMENT_DIR}/seed_{seed}'
    # Use -u for unbuffered output to the log file
    command = f'{PYTHON} -u ../optimizer.py {args.parameters} --seed {seed} --logdir {logdir}'

    os.makedirs(logdir, exist_ok=True)
    logf = open(f'{logdir}/output.log', 'w')

    print(f'Running seed {seed}')
    logf.write(f'Running seed {seed}'.center(80, '-') + '\n')
    logf.write(command + '\n')
    logf.write('-' * 80 + '\n')
    logf.flush()
    if args.parallel:
        # Run in parallel
        p = subprocess.Popen(command, stdout=logf, stderr=subprocess.STDOUT)
        processes.append((p, logf))
    else:
        # Run sequentially
        subprocess.run(command, stdout=logf, stderr=subprocess.STDOUT)
        logf.close()

# This is only reached if running in parallel
for p, logf in processes:
    p.wait()
    logf.close()
