# Simulator

This directory contains the source code of the simulator for the Traffic signaling problem from Google Hash Code 2021 competition. The simulator is written in C++ and wrapped as Python package named `traffic-signaling`. The Python bindings for the C++ code are created using `pybind11`.

## Directory structure

| directory / file | description |
|------------------|-------------|
| [`include/`](./include) | C++ header files of the simulator </br>  [`city_plan/`](./include/city_plan/) - headers of *city plan* part </br> [`simulation/`](./include/simulation/) - headers of *simulation* part |
| [`src/`](./src) | C++ source files of the simulator </br>  [`city_plan/`](./src/city_plan/) - source files of *city plan* part </br> [`simulation/`](./src/simulation/) - source files of *simulation* part </br> [`bindings/`](./src/bindings/) - pybind11 bindings |
| [`tests/`](./tests) | Unit tests for both C++ and Python verifying the simulator functionality |
| [`traffic_signaling/`](./traffic_signaling) | Contents of the Python package when installed with pip </br>  [`utils.py`](./traffic_signaling/utils.py) provides extra functionality for the simulator </br> [`data/`](./traffic_signaling/data) contains the datasets provided with the competition |
| [`pyproject.toml`](./pyproject.toml) </br> [`setup.py`](./setup.py) | Python configuration files for installing the `traffic-signaling` package using pip |
| [`CMakeLists.txt`](./CMakeLists.txt) | CMake configuration file for building the C++ code, building the Python package using CMake |

## API and docs

The C++ implementation is extensively documented with comments and docstrings inside the code.  
To view the API of the `traffic-signaling` Python package, see the following sources:

- [`city_plan.pyi`](./traffic_signaling/city_plan.pyi) (C++ extension module) - stores and provides all "static" data from the input file in the `CityPlan` class
- [`simulation.pyi`](./traffic_signaling/simulation.pyi) (C++ extension module) - uses the  `CityPlan` and to run the simulation using the `Simulation` class, and provides other useful functionality for working with traffic light schedules
- [`utils.py`](./traffic_signaling/utils.py) - provides extra utilities and helper functions