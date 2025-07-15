
VENV=.venv

# https://stackoverflow.com/questions/70675848/makefile-throws-an-error-for-checking-the-os
# https://venthur.de/2021-03-31-python-makefiles.html
# OS agnostic
ifeq ($(OS), Windows_NT)
	VENV_BIN=../$(VENV)/Scripts
	PYTHON=$(VENV_BIN)/python
else # Linux
	VENV_BIN=../$(VENV)/bin
	PYTHON=$(VENV_BIN)/python3
endif

# Set the number of threads
THREADS=16
# Set the parallel execution, leave empty to disable
PARALLEL=--parallel
# Plot standard deviation, leave empty to disable
STD=--std
# Downsample the data points, plot every N-th point
DOWNSAMPLE=100

all: init_experiment plots

.PHONY: all init_experiment plots plot_b plot_c plot_d plot_e plot_f run_b run_c run_d run_e run_f

init_experiment:
	$(PYTHON) init_experiment.py

plots: plot_b plot_c plot_d plot_e plot_f

##################################### Plot #####################################
plot_b: run_b
	$(PYTHON) plot_experiments.py b "Genetic Algorithm" "Hill Climbing" "Simulated Annealing" $(STD) --downsample $(DOWNSAMPLE)

plot_c: run_c
	$(PYTHON) plot_experiments.py c "Genetic Algorithm" "Hill Climbing" "Simulated Annealing" $(STD) --downsample $(DOWNSAMPLE)

plot_d: run_d
	$(PYTHON) plot_experiments.py d "Genetic Algorithm" "Hill Climbing" "Simulated Annealing" $(STD) --downsample $(DOWNSAMPLE)

plot_e: run_e
	$(PYTHON) plot_experiments.py e "Genetic Algorithm" "Hill Climbing" "Simulated Annealing" $(STD) --downsample $(DOWNSAMPLE)

plot_f: run_f
	$(PYTHON) plot_experiments.py f "Genetic Algorithm" "Hill Climbing" "Simulated Annealing" $(STD) --downsample $(DOWNSAMPLE)

##################################### Run ######################################
run_b: run_b_ga run_b_hc run_b_sa

run_c: run_c_ga run_c_hc run_c_sa

run_d: run_d_ga run_d_hc run_d_sa

run_e: run_e_ga run_e_hc run_e_sa

run_f: run_f_ga run_f_hc run_f_sa

############################## Run single method ###############################
run_b_ga: logs/b/Genetic\ Algorithm
run_b_hc: logs/b/Hill\ Climbing
run_b_sa: logs/b/Simulated\ Annealing

run_c_ga: logs/c/Genetic\ Algorithm
run_c_hc: logs/c/Hill\ Climbing
run_c_sa: logs/c/Simulated\ Annealing

run_d_ga: logs/d/Genetic\ Algorithm
run_d_hc: logs/d/Hill\ Climbing
run_d_sa: logs/d/Simulated\ Annealing

run_e_ga: logs/e/Genetic\ Algorithm
run_e_hc: logs/e/Hill\ Climbing
run_e_sa: logs/e/Simulated\ Annealing

run_f_ga: logs/f/Genetic\ Algorithm
run_f_hc: logs/f/Hill\ Climbing
run_f_sa: logs/f/Simulated\ Annealing

#################################### Data b ####################################
logs/b/Genetic\ Algorithm:
	$(PYTHON) optimize_experiment.py "Genetic Algorithm" \
		"ga b --order_init adaptive --times_init default --population 10 --generations 60000 --mutation_bit_rate 2 --crossover 0.6 --mutation 0.4 --tournsize 3 --elitism 0.05 --threads $(THREADS)" \
		$(PARALLEL)

logs/b/Hill\ Climbing:
	$(PYTHON) optimize_experiment.py "Hill Climbing" \
		"hc b --order_init adaptive --times_init default --instances 1 --iterations 450000 --mutation_bit_rate 4" \
		$(PARALLEL)

logs/b/Simulated\ Annealing:
	$(PYTHON) optimize_experiment.py "Simulated Annealing" \
		"sa b --order_init adaptive --times_init default --instances 1 --iterations 450000 --mutation_bit_rate 1 --temperature 0.25" \
		$(PARALLEL)

#################################### Data c ####################################
logs/c/Genetic\ Algorithm:
	$(PYTHON) optimize_experiment.py "Genetic Algorithm" \
		"ga c --order_init adaptive --times_init default --population 20 --generations 30000 --mutation_bit_rate 2 --crossover 0.6 --mutation 0.4 --tournsize 3 --elitism 0.05 --threads $(THREADS)" \
		$(PARALLEL)

logs/c/Hill\ Climbing:
	$(PYTHON) optimize_experiment.py "Hill Climbing" \
		"hc c --order_init adaptive --times_init default --instances 1 --iterations 450000 --mutation_bit_rate 4" \
		$(PARALLEL)

logs/c/Simulated\ Annealing:
	$(PYTHON) optimize_experiment.py "Simulated Annealing" \
		"sa c --order_init adaptive --times_init default --instances 1 --iterations 450000 --mutation_bit_rate 2 --temperature 0.1" \
		$(PARALLEL)

#################################### Data d ####################################
logs/d/Genetic\ Algorithm:
	$(PYTHON) optimize_experiment.py "Genetic Algorithm" \
		"ga d --order_init adaptive --times_init default --population 40 --generations 15000 --mutation_bit_rate 2 --crossover 0.6 --mutation 0.4 --tournsize 3 --elitism 0.05 --threads $(THREADS)" \
		$(PARALLEL)

logs/d/Hill\ Climbing:
	$(PYTHON) optimize_experiment.py "Hill Climbing" \
		"hc d --order_init adaptive --times_init default --instances 1 --iterations 450000 --mutation_bit_rate 2" \
		$(PARALLEL)

logs/d/Simulated\ Annealing:
	$(PYTHON) optimize_experiment.py "Simulated Annealing" \
		"sa d --order_init adaptive --times_init default --instances 1 --iterations 450000 --mutation_bit_rate 2 --temperature 0.1" \
		$(PARALLEL)

#################################### Data e ####################################
logs/e/Genetic\ Algorithm:
	$(PYTHON) optimize_experiment.py "Genetic Algorithm" \
		"ga e --order_init random --times_init scaled --population 90 --generations 6667 --mutation_bit_rate 9 --crossover 0.6 --mutation 0.4 --tournsize 3 --elitism 0.05 --threads $(THREADS)" \
		$(PARALLEL)

logs/e/Hill\ Climbing:
	$(PYTHON) optimize_experiment.py "Hill Climbing" \
		"hc e --order_init random --times_init scaled --instances 1 --iterations 450000 --mutation_bit_rate 15" \
		$(PARALLEL)

logs/e/Simulated\ Annealing:
	$(PYTHON) optimize_experiment.py "Simulated Annealing" \
		"sa e --order_init random --times_init scaled --instances 1 --iterations 450000 --mutation_bit_rate 5 --temperature 275" \
		$(PARALLEL)

#################################### Data f ####################################
logs/f/Genetic\ Algorithm:
	$(PYTHON) optimize_experiment.py "Genetic Algorithm" \
		"ga f --order_init random --times_init scaled --population 50 --generations 12000 --mutation_bit_rate 2 --crossover 0.6 --mutation 0.4 --tournsize 3 --elitism 0.05 --threads $(THREADS)" \
		$(PARALLEL)

logs/f/Hill\ Climbing:
	$(PYTHON) optimize_experiment.py "Hill Climbing" \
		"hc f --order_init random --times_init scaled --instances 1 --iterations 450000 --mutation_bit_rate 5" \
		$(PARALLEL)

logs/f/Simulated\ Annealing:
	$(PYTHON) optimize_experiment.py "Simulated Annealing" \
		"sa f --order_init random --times_init scaled --instances 1 --iterations 450000 --mutation_bit_rate 3 --temperature 15" \
		$(PARALLEL)
