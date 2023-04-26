# Specification

## Main features
- effective and stand-alone working C++ implementation of the simulation of the Traffic signaling problem from Google Hashcode 2021
- Python bindings for the C++ implementation using [pybind11](https://pybind11.readthedocs.io/), making the simulation accessible from Python and available for use together with Python libraries
- optimization of the simulation using genetic algorithms and local search algorithms, preferably by using Python libraries such as [DEAP](https://deap.readthedocs.io/), [PyGMO](https://esa.github.io/pygmo2/), etc., or by implementing the algorithms manually
---
- optimizing the simulation using surrogate models - this part is uncertain because
    - it depends on the C++ implementation of the simulation and may not make much sense if the evaluation of the simulation is not the bottleneck of the optimization process
    - I may not collect enough data to train the surrogate model
    - the feature vector may be too large, especially for the largest instances of the simulation (tens of thousands of features)

## Some implementation notes
- to make the evaluation as fast as possible, the simulation algorithm should be implemented by a priority queue of events sorted by time, so that only the necessary events are processed at the required times
    - `car event` - a car arrives at the end of a street at a given time
    - `street event`- the first car in the queue at the end of this street goes through an intersection at a given time
- streets that are not used by any car should be removed
- for intersections with exactly one used street, the optimal plan is fixed - set green for 1 second for this street and 0 seconds for the rest of the streets - streets within this intersection should also be excluded from the plan optimization to avoid unnecessary computations
- no explicit parallelization of the simulation, only within the libraries

## Python API for the simulation
- mostly just straightforward wrappers for the C++ classes
- the only exception is the `Plan` class, which needs to be able to get the underlying datastructure of green times and street orderings from Python easily and efficiently

## Rough outline of the classes in the simulation
- written in Cpp-like pseudocode

```cpp
// static data - read once at the beginning, never changed
class IntersectionData {
    const int id;
    const std::vector<const StreetData{*&} const> incoming;
    const std::vector<const StreetData{*&} const> outgoing; // redundant
};

class Intersection {
    // static data
    const IntersectionData{*&} const data;

    // dynamic data
    auto plan;

    void print();

    // Returns a street that has a green light at `time`.
    Street green(int time);

    // Is the `street` green at `time`?
    bool is_green(Street street, int time);
};
```

```cpp
// static data - read once at the beginning, never changed
class StreetData {
    const int id;
    const IntersectionData{*&} start;
    const IntersectionData{*&} end;
    const std::string name;
    const int length;
}

class Street {
    // static data
    const StreetData{*&} const data;

    // dynamic data
    bool used;
    std::queue<Car> queueing_cars;
    int _last_used_time;
    // interval_type green_interval; // interval of time when the street is green (modulo the cycle of the intersection)

    void print();

    // Returns the next time when the street has a green light.
    int next_green(int time);

    // Returns a car that goes through the end intersection at time.
    Car go(int time);
};
```

```cpp
// static data - read once at the beginning, never changed
class CarData {
    const int id;
    const int path_length; // number of streets in the car's path (redundant)
    const std::vector<const StreetData{*&} const> path;
};

class Car {
    // static data
    const CarData{*&} const data;
    // dynamic data
    ?int current_street_index;
    bool finished;
    int finish_time;

    void print();

    bool is_final_destination();

    Street current_street();

    void move_to_next_street();
    void move_to_next_street(?int time);
};
```

```cpp
// static data
class PlanData {

};

// API for getting green times for each intersection and each incoming street and the ordering
class Plan {
    // static data - may be redundant
    const PlanData{*&} const data;

    // precompute the green intervals once at the initialization of the plan
    Plan(...);

    int get_next_green_time(int time);

    // underlying structure with all raw data
    // green times and permutations for each intersection and each incoming street
    std::vector<int> data;
};
```

```cpp
// static data - read once at the beginning, never changed
class SimulationData {
    const int duration;
    const std::vector<const IntersectionData{*&} const> intersections;
    const std::vector<const StreetData{*&} const> streets;
    const std::vector<const CarData{*&} const> cars;
    const std::map<std::string, const StreetData{*&} const> street_mappping;
    const int bonus;

    // Initialize simulation data from file.
    SimulationData(std::string filename);
};

class Simulation {
    // static data
    const SimulationData{*&} const data;

    // dynamic data
    std::vector<Intersection> intersections;
    std::vector<Street> streets;
    std::vector<Car> cars;

    Plan plan;

    // functions
    void print();

    // Run the simulation.
    // Use StablePriorityQueue for the event queue.
    void run();

    // Calculate and return the score.
    float score();

    // Read and initialize the plan of the simulation from file.
    void read_plan(std::string filename);

    // Initialize the plan of the simulation.
    // For every intersection and every incoming street, set green for 1 second.
    void create_plan_default();

    // Initialize the plan of the simulation.
    // Set green for 1 second for every used incoming street. 
    // Exclude streets that aren't used.
    void create_plan_used();

    // Create the output file.
    void create_output(std::string filename);

    Street get_street_by_name(std::string name);
};
```

```cpp
// abstract class for events
class Event {
    int time;
};

class CarEvent : public Event {
    Car *car;
};

class StreetEvent : public Event {
    Street *street;
};
```

```cpp
// wrapper for std::priority_queue with stable sorting
// needed for the event queue
class StablePriorityQueue<T> {

};
```
