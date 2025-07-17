# Traffic Signal Optimization

This repository contains the code of my bachelor thesis.

## Repository structure

| directory / file | description |
|------------------|-------------|
| [`traffic_signaling/`](./traffic_signaling/README.md) | Contains the source code of the simulator for the Traffic signaling problem; see the [`README`](./traffic_signaling/README.md) for more details. |
| [`optimizer.py`](./optimizer.py) </br> [`operators.py`](./operators.py) | Scripts for running the optimization using Genetic Algorithm, Hill Climbing, and Simulated Annealing. |
| | |
| [`experiments/`](./experiments/README.md) | Contains scripts for running the experiments presented in the thesis, see the [`README`](./experiments/README.md) for more details. </br> Also see [`best_solutions`](./experiments/best_solutions/README.md) for the best schedules found during the experiments. |
| [`Makefile`](./Makefile) | Provides a convenient way to set up the environment for optimization and running experiments (see [Optimization](#optimization)). Also enables to easily run unit tests.  |

## Prerequisites

When listing version requirements for the tools below, we specify the *minimum* supported version. As of July 2025, the latest available versions (e.g., Python 3.13) are also compatible. However, we cannot guarantee compatibility with all future versions.

To build and install the [`traffic-signaling`](./traffic_signaling/README.md) package, you need:

- **C++ compiler** with C++20 code support; e.g., gcc 11, clang 16, or MSVC 19.29 (or later versions)
- **Python** 3.10 or later
- **Python headers** (most likely already installed with Python)
  - You can verify that the headers are available by inspecting their expected location with e.g.:
  
    ```bash
    python3 -c "import sysconfig; print(sysconfig.get_path('include'))"
    ```
  - If not available, install the headers with e.g.:
  
    ```bash
    sudo apt install python3-dev
    ```

To easily set up the environment for optimization and run the experiments, you further need:

- **GNU Make**

Optionally, if you want to run all unit tests for both C++ and Python, you additionally need:

- **Git**
- **CMake** 3.24 or later

## Simulator

After satisfying the prerequisites, you can simply build and install  
the `traffic-signaling` Python package by running the following command in the top-level directory:

```bash
pip install ./traffic_signaling
```
This will install the package into your Python environment (preferably into a virtual environment), making it available for use in your Python scripts.

To run Python unit tests for the package using Python's `unittest` built-in framework, use the following command:

```bash
python -m unittest discover -s traffic_signaling/tests
```

To run both C++ and Python unit tests using CMake, use the following command:

```bash
make test_package_cmake
```

Note that running the make commands will create a virtual environment `.venv` and install all required packages there using pip.

## Optimization

**To quickly set up the environment for optimization, run the following command in the top-level directory:**

```bash
make setup
```

This will create a virtual environment `.venv`, build and install the simulator and other necessary packages into it using pip, and compile the [`operators.py`](./operators.py) file using [Cython](https://cython.org/) for better performance during optimization.  
Do not forget to activate the virtual environment before running any scripts, e.g.:

```bash
source .venv/bin/activate
```

If you encounter any issues, try running `make clean` and then `make setup` again.

Now you can run the optimization algorithms using the [`optimizer.py`](./optimizer.py) script. The script has two required positional arguments:

- `algorithm` – algorithm to use for optimization; possible values are `ga`, `hc`, `sa`
- `data` – input dataset to use; possible values are `a`, `b`, `c`, `d`, `e`, `f`

After specifying the required arguments, you can easily run the script with, e.g.:

```bash
python3 optimizer.py hc e
```

This will run the Hill Climbing algorithm on dataset E using default values. However, you probably want to explicitly set some parameters, especially the hyperparameter values.  
If you prefer, you can run:

```bash
python3 optimizer.py --help
```

to see the full usage. Below is a concise list of the parameters:

- `--order_init` – *order initialization* hyperparameter; possible values: `adaptive`, `random`, `default`
- `--times_init` – *times initialization* hyperparameter; possible values: `scaled`, `default`
- `--mutation_bit_rate` – *mutation bit rate* hyperparameter
- `--population` – *population size* hyperparameter (GA only)
- `--generations` – *generations* hyperparameter (GA only)
- `--crossover` – *crossover probability* hyperparameter (GA only)
- `--mutation` – *mutation probability* hyperparameter (GA only)
- `--elitism` – *elitism* hyperparameter (GA only)
- `--tournsize` – *tournament size* hyperparameter (GA only)
- `--iterations` – *iterations* hyperparameter (HC and SA)
- `--temperature` – *initial temperature* hyperparameter (SA only)
- `--seed` – value of the random seed for reproducibility
- `--threads` – number of threads for parallel evaluation
- `--logdir` – custom name of the directory with results and logs
- `--verbose` – whether to print detailed output during optimization
- `--no-save` – skip saving results to the log directory

An example of running the script with more parameters could look like this:

```bash
python3 optimizer.py ga e \
    --order_init random --times_init scaled \
    --mutation_bit_rate 5 --population 100 --generations 200 \
    --threads 16 --seed 21 --verbose
```

When the optimization finishes (and if the `--no-save` option was not used), the optimizer will save the following files in the log directory:

- A CSV file containing statistics for each iteration/generation of the algorithm
- A file with the best schedules found, stored in the competition format
- A PDF file visualizing the optimization process
- An information file listing all parameters, their values, and additional details

Optionally, you can run unit tests for the optimizer with the following command in the top-level directory:

```bash
make test_optimizer
```
### Running the experiments

To replicate the experiments presented in this thesis, ensure that you have already run the `make setup` command, then navigate to the [`experiments`](./experiments) directory.  
We recommend reading the [`experiments/README.md`](./experiments/README.md) file for more details, but for convenience, we also include the list of commands below. We strongly suggest running each algorithm and dataset separately.

- `make test` – run a simple sanity check experiment
- `make init_experiment` – run a quick experiment comparing different initialization methods
- `make run_{b,c,d,e,f}_{ga,hc,sa}` – run a specific algorithm on a specific dataset (using 10 runs with different fixed seeds)
- `make run_{b,c,d,e,f}` – run all algorithms on a specific dataset
- `make plot_{b,c,d,e,f}` – plot the results of a specific dataset
- `make plots` – plot all datasets
- `make all` – run everything and plot all results

The schedules corresponding to the best scores achieved for each dataset during optimization are stored in the [`experiments/best_solutions`](./experiments/best_solutions) directory.

