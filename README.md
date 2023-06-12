# Traffic signaling

## Links

### [Problem description](https://storage.googleapis.com/coding-competitions.appspot.com/HC/2021/hashcode_2021_online_qualification_round.pdf)

[Problem submission and interactive page](https://codingcompetitions.withgoogle.com/hashcode/round/00000000004361e2/0000000000c617e4)  
[Google Hash Code archive](https://codingcompetitions.withgoogle.com/hashcode/archive)  
[Kaggle competition with additional data](https://www.kaggle.com/competitions/hashcode-2021-oqr-extension/overview)

---

## How to build the simulation for use in Python

- simply run `pip install ./simulation` (ideally in a virtual environment)
- verify by running `python test.py`

---

## Solutions

- `7,885,740` - 1 sec green for every incoming street
- `8,943,886` - 1 sec green for every used (with at least one car going through) incoming street

---

## References

### Algorithms - general

[**Essentials of Metaheuristics**](https://cs.gmu.edu/~sean/book/metaheuristics/)

[**Introduction to Evolutionary Computing**](https://link.springer.com/book/10.1007/978-3-662-44874-8)

[**An Introduction to Genetic Algorithms**](https://www.boente.eti.br/fuzzy/ebook-fuzzy-mitchell.pdf)  

[**Artificial Intelligence: A Modern Approach**](https://aima.cs.berkeley.edu/) ([PDF](https://github.com/yanshengjia/ml-road/blob/master/resources/Artificial%20Intelligence%20-%20A%20Modern%20Approach%20(3rd%20Edition).pdf))

---

### Surrogate models

[Feature Extraction for Surrogate Models in Genetic Programming](https://github.com/martinpilat/gp-surrogate)  

[Managing computational complexity using surrogate models: a critical review](https://link.springer.com/article/10.1007/s00163-020-00336-7)  

[Learning surrogate models for simulation-based optimization](https://aiche.onlinelibrary.wiley.com/doi/10.1002/aic.14418)  

---

### Competition-related papers

[**Building a Simulator and Emulator for Traffic Signalling**](https://victorzxy.github.io/project/traffic-sim/) ([PDF](https://victorzxy.github.io/project/traffic-sim/TrafficSim-Report.pdf))  
- based on the same problem from the Hash Code 2021 qualification round  

[**Iterated Local Search with Genetic Algorithms for the Photo Slideshow Problem**](https://link.springer.com/chapter/10.1007/978-3-031-26504-4_45)  
- optimization of a problem from the Hash Code 2019 qualification round - the task and techniques used are very similar to our problem  

---

[A heuristics based approach for optimizing delivery schedule of an Unmanned Aerial Vehicle (Drone) based delivery system](https://ieeexplore.ieee.org/abstract/document/8593145)  
- optimization of a problem from the Hash Code 2016 qualification round - the task is quite different from our case

[Applied routing problem for a fleet of delivery drones using
a modified parallel genetic algorithm](https://dspace.spbu.ru/bitstream/11701/36938/1/135-148.pdf)  
- optimization of a problem from the Hash Code 2016 qualification round

[A parallel memetic algorithm with explicit management of diversity for the job shop scheduling problem](https://link.springer.com/article/10.1007/s10489-021-02406-2)  
- uses a problem from the Hash Code 2020 finals

[Towards Efficient Solvers for Optimisation Problems](https://ieeexplore.ieee.org/abstract/document/8752700)  
- uses a problem from the Hash Code 2017 qualification round  

[Genetic algorithms and greedy-randomized adaptive search procedure for router placement problem in wireless networks](https://www.researchgate.net/publication/335374967_Genetic_algorithms_and_greedy-randomized_adaptive_search_procedure_for_router_placement_problem_in_wireless_networks)  
-  also uses some problem from Hash Code, but it's not publicly available (only upon request from the authors)