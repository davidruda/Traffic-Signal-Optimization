# Experiments

This directory contains scripts to replicate the experiments and the [`best solutions`](./best_solutions/README.md) subdirectory contains the overall best schedules found for each dataset.

_Note that, to run the experiments using `make`, you should first execute `make setup` in the parent directory first. This command sets up the environment and installs all necessary dependencies._

Each algorithm is run with 10 times with different fixed seeds.

Be aware that running `make all` would take a very long time and you don't want to do this. Instead, you want to split the runs into individual steps like a specific algorithm on a specific dataset, e.g. `make run_e_ga`. Below is a list of available commands:

| command | description |
|---------|-------------|
| `make test` | run a simple sanity check experiment |
| `make init_experiment` | run a quick experiment comparing different initialization methods |
| `make run_{b,c,d,e,f}_{ga,hc,sa}` | run a specific algorithm on a specific dataset (using 10 runs with different fixed seeds) |
| `make run_{b,c,d,e,f}` | run all algorithms on a specific dataset |
| `make plot_{b,c,d,e,f}` | plot the results of a specific dataset |
| `make plots` | plot all datasets |
| `make all` | run everything and plot all results |

If interested in specific parameters of the algorithms, or want to change some settings for the experiments, inspect the [`Makefile`](./Makefile) file in this directory.

### Scripts

- [`init_experiment.py`](./init_experiment.py) - run a quick experiment comparing different initialization methods and generate PDF plots of the results
- [`optimize_experiment.py`](./optimize_experiment.py) - run a specific algorithm on a specific dataset 10 times with different fixed seeds
- [`plot_experiment.py`](./plot_experiment.py) - generate a PDF summarizing the results of all algorithms on a specific dataset
