import argparse
import pandas as pd
from pathlib import Path
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import numpy as np
import seaborn as sns
sns.set_theme()
from scipy.interpolate import interp1d

from traffic_signaling import *

parser = argparse.ArgumentParser()
parser.add_argument('data', choices=['a', 'b', 'c', 'd', 'e', 'f'], help='Dataset to plot.')
parser.add_argument('experiments', nargs="+", type=str, help='List of experiment names to plot.')
parser.add_argument('--logdir', default='logs', type=str, help='Log directory of the experiments.')
parser.add_argument('--y_min', default=None, type=float, help='Minimum y-axis value.')
parser.add_argument('--y_max', default=None, type=float, help='Maximum y-axis value.')
parser.add_argument('--std', action='store_true', help='Plot standard deviation.')
parser.add_argument('--baseline', action='store_true', help='Plot baseline score.')
parser.add_argument('--max_known', action='store_true', help='Plot max known score.')
parser.add_argument('--downsample', default=1, type=int, help='Plot every N-th point to reduce size of the plot.')
args = parser.parse_args()

def get_all_runs(folder):
    dfs = []
    runs = [f for f in Path(folder).iterdir() if f.is_dir()]
    for run in runs:
        df = pd.read_csv(run / f'{args.data}.csv')
        df['evaluations'] = df['new_vals'].cumsum()
        dfs.append(df)
    return dfs

def get_interpolated_mean(folder):
    dfs = get_all_runs(folder)

    # Interpolation
    min_eval = 1
    max_eval = max(df['evaluations'].max() for df in dfs)

    common_evals = np.arange(min_eval, max_eval + 1, step=1)

    interps = []
    for df in dfs:
        # https://docs.scipy.org/doc/scipy/reference/generated/scipy.interpolate.interp1d.html
        interp_fn = interp1d(df['evaluations'], df['norm_max'], kind='linear', fill_value='extrapolate')
        interps.append(interp_fn(common_evals))

    mean_scores = np.stack(interps).mean(axis=0)
    std_scores = np.stack(interps).std(axis=0)
    df = pd.DataFrame({
        'evaluations': common_evals,
        'norm_max': mean_scores,
        'norm_max_std': std_scores,
    })

    return df

def get_mean(folder):
    dfs = get_all_runs(folder)

    evaluations = dfs[0]['evaluations']
    scores = [df['norm_max'] for df in dfs]

    mean_scores = np.stack(scores).mean(axis=0)
    std_scores = np.stack(scores).std(axis=0)
    df = pd.DataFrame({
        'evaluations': evaluations,
        'norm_max': mean_scores,
        'norm_max_std': std_scores,
    })

    return df

dfs = []
for experiment in args.experiments:
    folder = f'{args.logdir}/{args.data}/{experiment}'
    if experiment.startswith('ga'):
        # Genetic Algorithm always has a different number of evaluations at different times
        # so we need to interpolate the scores to get a mean score
        df = get_interpolated_mean(folder)
    else:
        df = get_mean(folder)

    # Downsample the data points
    df = df.iloc[::args.downsample]
    dfs.append(df)

# PLOT
fig, ax1 = plt.subplots()

for df, experiment in zip(dfs, args.experiments):
    ax1.plot(df['evaluations'], df['norm_max'], label=experiment)
    if args.std:
        ax1.fill_between(
            df['evaluations'],
            df['norm_max'] - df['norm_max_std'],
            df['norm_max'] + df['norm_max_std'],
            alpha=0.2
        )

if args.baseline:
    ax1.axhline(0, color='m', linestyle='--', label='baseline')
if args.max_known:
    ax1.axhline(1, color='y', linestyle='--', label='max known score')

norm_low = min(np.min(x['norm_max']) for x in dfs)
norm_high = max(np.max(x['norm_max']) for x in dfs)

ax1.set_title(f'Dataset {args.data.capitalize()}')
ax1.set_xlabel('Simulation evaluations')
ax1.set_ylabel('Normalized score')

if args.y_min is not None:
    ax1.set_ylim(bottom=args.y_min)
if args.y_max is not None:
    ax1.set_ylim(top=args.y_max)
ax1.legend()

ax2 = ax1.twinx()
ax2.set_ylim(*ax1.get_ylim())
ax2.grid(False)
ax2.set_ylabel('Score')

ticks = ax1.get_yticks()
labels = (f'{absolute_score(x, args.data):,}' for x in ticks)
ax2.yaxis.set_major_locator(ticker.FixedLocator(ax2.get_yticks()))
ax2.set_yticklabels(labels)

# Avoid scientific notation for x-axis ticks
xticks = ax1.get_xticks()
ax1.xaxis.set_major_locator(ticker.FixedLocator(xticks))
ax1.set_xticklabels([f'{x:,.0f}' for x in xticks], rotation=45)

fig.tight_layout()
plt.savefig(f'{args.data}_{'_'.join(args.experiments)}.pdf', bbox_inches='tight')
# plt.show()
