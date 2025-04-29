
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

all: init_experiment plot_b plot_c plot_d plot_e plot_f

.PHONY: all init_experiment plot_b plot_c plot_d plot_e plot_f run_b run_c run_d run_e run_f

init_experiment:
	$(PYTHON) init_experiment.py

##################################### Plot #####################################
plot_b: run_b
	$(PYTHON) plot_experiments.py b ga_adaptive hc_adaptive sa_adaptive

plot_c: run_c
	$(PYTHON) plot_experiments.py c ga_adaptive hc_adaptive sa_adaptive

plot_d: run_d
	$(PYTHON) plot_experiments.py d ga_adaptive hc_adaptive sa_adaptive

plot_e: run_e
	$(PYTHON) plot_experiments.py e ga_random hc_random sa_random

plot_f: run_f
	$(PYTHON) plot_experiments.py f ga_random_scaled hc_random_scaled sa_random_scaled

##################################### Run ######################################
run_b: logs/b/ga_adaptive logs/b/hc_adaptive logs/b/sa_adaptive

run_c: logs/c/ga_adaptive logs/c/hc_adaptive logs/c/sa_adaptive

run_d: logs/d/ga_adaptive logs/d/hc_adaptive logs/d/sa_adaptive

run_e: logs/e/ga_random logs/e/hc_random logs/e/sa_random

run_f: logs/f/ga_random_scaled logs/f/hc_random_scaled logs/f/sa_random_scaled

#################################### Data b ####################################
logs/b/ga_adaptive:
	$(PYTHON) optimize_experiment.py ga_adaptive \
		"ga b --population 100 --generations 3000 --order_init adaptive --times_init default --threads 16" \
		--parallel

logs/b/hc_adaptive:
	$(PYTHON) optimize_experiment.py hc_adaptive \
		"hc b --instances 1 --iterations 228000 --order_init adaptive --times_init default" \
		--parallel

logs/b/sa_adaptive:
	$(PYTHON) optimize_experiment.py sa_adaptive \
		"sa b --instances 1 --iterations 228000 --order_init adaptive --times_init default --temperature 1" \
		--parallel

#################################### Data c ####################################
logs/c/ga_adaptive:
	$(PYTHON) optimize_experiment.py ga_adaptive \
		"ga c --population 100 --generations 1500 --order_init adaptive --times_init default --threads 16" \
		--parallel

logs/c/hc_adaptive:
	$(PYTHON) optimize_experiment.py hc_adaptive \
		"hc c --instances 1 --iterations 114000 --order_init adaptive --times_init default" \
		--parallel

logs/c/sa_adaptive:
	$(PYTHON) optimize_experiment.py sa_adaptive \
		"sa c --instances 1 --iterations 114000 --order_init adaptive --times_init default --temperature 10" \
		--parallel

#################################### Data d ####################################
logs/d/ga_adaptive:
	$(PYTHON) optimize_experiment.py ga_adaptive \
		"ga d --population 100 --generations 200 --order_init adaptive --times_init default --mutation_bit_rate 1e-5 --threads 16" \
		--parallel

logs/d/hc_adaptive:
	$(PYTHON) optimize_experiment.py hc_adaptive \
		"hc d --instances 1 --iterations 15200 --order_init adaptive --times_init default --mutation_bit_rate 1e-5" \
		--parallel

logs/d/sa_adaptive:
	$(PYTHON) optimize_experiment.py sa_adaptive \
		"sa d --instances 1 --iterations 15200 --order_init adaptive --times_init default --mutation_bit_rate 1e-5 --temperature 10" \
		--parallel

#################################### Data e ####################################
logs/e/ga_random:
	$(PYTHON) optimize_experiment.py ga_random \
		"ga e --population 100 --generations 4000 --order_init random --times_init default --threads 16" \
		--parallel

logs/e/hc_random:
	$(PYTHON) optimize_experiment.py hc_random \
		"hc e --instances 1 --iterations 304000 --order_init random --times_init default" \
		--parallel

logs/e/sa_random:
	$(PYTHON) optimize_experiment.py sa_random \
		"sa e --instances 1 --iterations 304000 --order_init random --times_init default --temperature 100" \
		--parallel

#################################### Data f ####################################
logs/f/ga_random_scaled:
	$(PYTHON) optimize_experiment.py ga_random_scaled \
		"ga f --population 100 --generations 1500 --order_init random --times_init scaled --threads 16" \
		--parallel

logs/f/hc_random_scaled:
	$(PYTHON) optimize_experiment.py hc_random_scaled \
		"hc f --instances 1 --iterations 114000 --order_init random --times_init scaled" \
		--parallel

logs/f/sa_random_scaled:
	$(PYTHON) optimize_experiment.py sa_random_scaled \
		"sa f --instances 1 --iterations 114000 --order_init random --times_init scaled --temperature 100" \
		--parallel
