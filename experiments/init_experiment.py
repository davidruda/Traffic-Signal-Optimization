import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns
sns.set_theme()

from traffic_signaling import *

data = {'dataset': [], 'method': [], 'score': []}

for dataset in TEST_DATA:
    print(f"Running dataset {dataset}")
    # Default score (baseline) is the same for all datasets - 0.0

    sim = Simulation(create_city_plan(dataset))
    sim.adaptive_schedules()
    adaptive = normalized_score(sim.score(), data=dataset)
    data['dataset'].append(dataset.upper())
    data['method'].append('adaptive')
    data['score'].append(adaptive)

    # Find the best divisor for scaled schedules
    scaled = []
    for divisor in range(1, 101):
        sim.scaled_schedules(divisor=divisor)
        scaled.append(normalized_score(sim.score(), data=dataset))
    best_divisor = np.argmax(scaled)
    best_scaled = scaled[best_divisor]
    print(f"Best divisor for {dataset} is {best_divisor + 1} with score {best_scaled:.2f}")
    data['dataset'].append(dataset.upper())
    data['method'].append('scaled')
    data['score'].append(best_scaled)

    set_seed(42)
    RANDOM_REPEATS = 100
    random = []
    for _ in range(RANDOM_REPEATS):
        sim.random_schedules()
        random.append(normalized_score(sim.score(), data=dataset))
    data['dataset'].extend([dataset.upper()] * RANDOM_REPEATS)
    data['method'].extend(['random'] * RANDOM_REPEATS)
    data['score'].extend(random)

df = pd.DataFrame(data)

colors = sns.color_palette('tab10')
plt.axhline(0, linestyle='--', color=colors[0], label='default (baseline)')
ax = sns.barplot(data=df, x='dataset', y='score', hue='method', hue_order=['adaptive', 'random', 'scaled'], palette=colors[1:4])

# Create bar text labels
ax.bar_label(ax.containers[0], labels=np.round(ax.containers[0].datavalues, 2), fontsize=10, color=colors[1])
ax.bar_label(ax.containers[1], labels=np.round(ax.containers[1].datavalues, 2), fontsize=10, color=colors[2])
ax.bar_label(ax.containers[2], labels=np.round(ax.containers[2].datavalues, 2), fontsize=10, color=colors[3])

plt.axhline(1, color=colors[4], linestyle='--', label='max known score')

y_min = -0.03
y_max = 1.03
plt.ylim(y_min, y_max)
plt.xlabel('Dataset')
plt.ylabel('Normalized score')

plt.legend()
plt.tight_layout()
plt.savefig('init_experiment.pdf', bbox_inches='tight')
# plt.plot()
