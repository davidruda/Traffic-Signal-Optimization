# Zadání

## Simulace [problému](https://codingcompetitions.withgoogle.com/hashcode/round/00000000004361e2/0000000000c617e4) z Google Hash Code 2021 a jeho optimalizace pomocí různých technik umělé inteligence ([Genetický algoritmus](https://en.wikipedia.org/wiki/Genetic_algorithm), [Local search](https://en.wikipedia.org/wiki/Local_search_(optimization)), [Náhradní modely](https://en.wikipedia.org/wiki/Surrogate_model)) a porovnání těchto přístupů

### Samotná simulace bude v C++, k ní ale budou vytvořeny Python bindings pomocí [pybind11](https://pybind11.readthedocs.io/en/stable/), aby bylo možné využít python knihovny - např. pro neuronové sítě při náhradních modelech
---
### Cílem je efektivně implementovat simulaci a vyzkoušet různé algoritmy pro její optimalizaci, nikoliv implementovat samotné optimalizační algoritmy (avšak pokud bude vhodnější genetické algoritmy/local search ručně naimplementovat než použít knihovnu, bude to možné, ale není to cílem)
---
### Program **nebude** obsahovat GUI s vizualizací, bude to pouze konzolová aplikace, multiplatformní
---
### Při optimalizaci budu nejprve zkoušet genetický algoritmus a local search, které jsou pro tento problém přímočaré a ideální se na něj hodí, a také abych získal dostatek dat pro náhradní modely - ty poté budou spíš experimentální