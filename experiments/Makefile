
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

.PHONY: all init_experiment plots plot_b plot_c plot_d plot_e plot_f run_b run_c run_d run_e run_f test

# Simple sanity check
test:
	$(PYTHON) optimize_experiment.py "Test" \
		"ga e --order_init random --times_init scaled --population 50 --generations 30 --mutation_bit_rate 5 --crossover 0.6 --mutation 0.4 --tournsize 3 --elitism 0.05 --threads $(THREADS)" \
		$(PARALLEL)
	cat logs/e/Test/seed_*/output.log
	

init_experiment:
	$(PYTHON) init_experiment.py

plots: plot_b plot_c plot_d plot_e plot_f

##################################### Plot #####################################
plot_b: run_b
	$(PYTHON) plot_experiments.py b "Genetic_Algorithm" "Hill_Climbing" "Simulated_Annealing" $(STD) --downsample $(DOWNSAMPLE)

plot_c: run_c
	$(PYTHON) plot_experiments.py c "Genetic_Algorithm" "Hill_Climbing" "Simulated_Annealing" $(STD) --downsample $(DOWNSAMPLE)

plot_d: run_d
	$(PYTHON) plot_experiments.py d "Genetic_Algorithm" "Hill_Climbing" "Simulated_Annealing" $(STD) --downsample $(DOWNSAMPLE)

plot_e: run_e
	$(PYTHON) plot_experiments.py e "Genetic_Algorithm" "Hill_Climbing" "Simulated_Annealing" $(STD) --downsample $(DOWNSAMPLE)

plot_f: run_f
	$(PYTHON) plot_experiments.py f "Genetic_Algorithm" "Hill_Climbing" "Simulated_Annealing" $(STD) --downsample $(DOWNSAMPLE)

##################################### Run ######################################
run_b: run_b_ga run_b_hc run_b_sa

run_c: run_c_ga run_c_hc run_c_sa

run_d: run_d_ga run_d_hc run_d_sa

run_e: run_e_ga run_e_hc run_e_sa

run_f: run_f_ga run_f_hc run_f_sa

############################## Run single method ###############################
run_b_ga: logs/b/Genetic_Algorithm
run_b_hc: logs/b/Hill_Climbing
run_b_sa: logs/b/Simulated_Annealing

run_c_ga: logs/c/Genetic_Algorithm
run_c_hc: logs/c/Hill_Climbing
run_c_sa: logs/c/Simulated_Annealing

run_d_ga: logs/d/Genetic_Algorithm
run_d_hc: logs/d/Hill_Climbing
run_d_sa: logs/d/Simulated_Annealing

run_e_ga: logs/e/Genetic_Algorithm
run_e_hc: logs/e/Hill_Climbing
run_e_sa: logs/e/Simulated_Annealing

run_f_ga: logs/f/Genetic_Algorithm
run_f_hc: logs/f/Hill_Climbing
run_f_sa: logs/f/Simulated_Annealing

#################################### Data b ####################################
logs/b/Genetic_Algorithm:
	$(PYTHON) optimize_experiment.py "Genetic_Algorithm" \
		"ga b --order_init adaptive --times_init default --population 10 --generations 60000 --mutation_bit_rate 2 --crossover 0.6 --mutation 0.4 --tournsize 3 --elitism 0.05 --threads $(THREADS)" \
		$(PARALLEL)

logs/b/Hill_Climbing:
	$(PYTHON) optimize_experiment.py "Hill_Climbing" \
		"hc b --order_init adaptive --times_init default --instances 1 --iterations 450000 --mutation_bit_rate 4" \
		$(PARALLEL)

logs/b/Simulated_Annealing:
	$(PYTHON) optimize_experiment.py "Simulated_Annealing" \
		"sa b --order_init adaptive --times_init default --instances 1 --iterations 450000 --mutation_bit_rate 1 --temperature 0.25" \
		$(PARALLEL)

#################################### Data c ####################################
logs/c/Genetic_Algorithm:
	$(PYTHON) optimize_experiment.py "Genetic_Algorithm" \
		"ga c --order_init adaptive --times_init default --population 20 --generations 30000 --mutation_bit_rate 2 --crossover 0.6 --mutation 0.4 --tournsize 3 --elitism 0.05 --threads $(THREADS)" \
		$(PARALLEL)

logs/c/Hill_Climbing:
	$(PYTHON) optimize_experiment.py "Hill_Climbing" \
		"hc c --order_init adaptive --times_init default --instances 1 --iterations 450000 --mutation_bit_rate 4" \
		$(PARALLEL)

logs/c/Simulated_Annealing:
	$(PYTHON) optimize_experiment.py "Simulated_Annealing" \
		"sa c --order_init adaptive --times_init default --instances 1 --iterations 450000 --mutation_bit_rate 2 --temperature 0.1" \
		$(PARALLEL)

#################################### Data d ####################################
logs/d/Genetic_Algorithm:
	$(PYTHON) optimize_experiment.py "Genetic_Algorithm" \
		"ga d --order_init adaptive --times_init default --population 40 --generations 15000 --mutation_bit_rate 2 --crossover 0.6 --mutation 0.4 --tournsize 3 --elitism 0.05 --threads $(THREADS)" \
		$(PARALLEL)

logs/d/Hill_Climbing:
	$(PYTHON) optimize_experiment.py "Hill_Climbing" \
		"hc d --order_init adaptive --times_init default --instances 1 --iterations 450000 --mutation_bit_rate 2" \
		$(PARALLEL)

logs/d/Simulated_Annealing:
	$(PYTHON) optimize_experiment.py "Simulated_Annealing" \
		"sa d --order_init adaptive --times_init default --instances 1 --iterations 450000 --mutation_bit_rate 2 --temperature 0.1" \
		$(PARALLEL)

#################################### Data e ####################################
logs/e/Genetic_Algorithm:
	$(PYTHON) optimize_experiment.py "Genetic_Algorithm" \
		"ga e --order_init random --times_init scaled --population 90 --generations 6667 --mutation_bit_rate 9 --crossover 0.6 --mutation 0.4 --tournsize 3 --elitism 0.05 --threads $(THREADS)" \
		$(PARALLEL)

logs/e/Hill_Climbing:
	$(PYTHON) optimize_experiment.py "Hill_Climbing" \
		"hc e --order_init random --times_init scaled --instances 1 --iterations 450000 --mutation_bit_rate 15" \
		$(PARALLEL)

logs/e/Simulated_Annealing:
	$(PYTHON) optimize_experiment.py "Simulated_Annealing" \
		"sa e --order_init random --times_init scaled --instances 1 --iterations 450000 --mutation_bit_rate 5 --temperature 275" \
		$(PARALLEL)

#################################### Data f ####################################
logs/f/Genetic_Algorithm:
	$(PYTHON) optimize_experiment.py "Genetic_Algorithm" \
		"ga f --order_init random --times_init scaled --population 50 --generations 12000 --mutation_bit_rate 2 --crossover 0.6 --mutation 0.4 --tournsize 3 --elitism 0.05 --threads $(THREADS)" \
		$(PARALLEL)

logs/f/Hill_Climbing:
	$(PYTHON) optimize_experiment.py "Hill_Climbing" \
		"hc f --order_init random --times_init scaled --instances 1 --iterations 450000 --mutation_bit_rate 5" \
		$(PARALLEL)

logs/f/Simulated_Annealing:
	$(PYTHON) optimize_experiment.py "Simulated_Annealing" \
		"sa f --order_init random --times_init scaled --instances 1 --iterations 450000 --mutation_bit_rate 3 --temperature 15" \
		$(PARALLEL)
