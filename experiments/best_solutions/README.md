# Best Solutions

Best schedules for each dataset found during the experiments.

| Dataset | Score |
| ------- | ----: |
| [`b.txt`](./b.txt) | 4,570,346 |
| [`c.txt`](./c.txt) | 1,315,702 |
| [`d.txt`](./d.txt) | 2,528,954 |
| [`e.txt`](./e.txt) | 782,044 |
| [`f.txt`](./f.txt) | 1,443,333 |

You can simply load these schedules and verify the scores using the following code snippet:

```python
from traffic_signaling import *

for dataset in ['b', 'c', 'd', 'e', 'f']:
    plan = create_city_plan(data=dataset)
    sim = Simulation(plan)

    sim.load_schedules(f"experiments/best_solutions/{dataset}.txt")

    score = sim.score()
    print(f"Score for dataset {dataset}: {score:,}")
```
