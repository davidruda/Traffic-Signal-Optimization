import time

from simulation import *

DATA_FILE = "input_data/d.txt"
PLAN_FILE = "output/d.txt"

start = time.time()
sim = Simulation(DATA_FILE)
print(f'Smulation: {time.time() - start}s')

start = time.time()
instance = sim.create_instance()
print(f'create_instance: {time.time() - start}s')

start = time.time()
instance.read_plan(PLAN_FILE)
print(f'read_plan: {time.time() - start}s')

start = time.time()
instance.run()
print(f'run: {time.time() - start}s')

print(instance.score())

# start = time.time()
# N = 100
# for _ in range(N):
#    i = sim.create_instance()
#    i.create_plan_default()
#    i.run().score()
# print(f'{N} instances: {time.time() - start}s')
