# Specification

## Main features
- effective and stand-alone working C++ implementation of the simulation of the Traffic signaling problem from Google Hashcode 2021
- Python bindings for the C++ implementation using [pybind11](https://pybind11.readthedocs.io/), making the simulation accessible from Python and available for use together with Python libraries
- optimization of the simulation using genetic algorithms and local search algorithms, preferably by using Python libraries such as [DEAP](https://deap.readthedocs.io/), [PyGMO](https://esa.github.io/pygmo2/), etc., or by implementing the algorithms manually
---
- optimizing the simulation using surrogate models - this part is uncertain because
  - it depends on the C++ implementation of the simulation and may not make much sense if the evaluation of the
    simulation is not the bottleneck of the optimization process
  - I may not collect enough data_ to train the surrogate model
  - the feature vector may be too large, especially for the largest instances of the simulation (tens of thousands of
    features)

## Some implementation notes
- to make the evaluation as fast as possible, the simulation algorithm should be implemented by a priority queue of events sorted by time, so that only the necessary events are processed at the required times
    - `car event` - a car arrives at the end of a street at a given time
    - `street event`- the first car in the queue at the end of this street goes through an intersection at a given time
- streets that are not used by any car should be removed
- for intersections with exactly one used street, the optimal schedule is fixed - set green for 1 second for this street and 0 seconds for the rest of the streets - streets within this intersection should also be excluded from the schedule optimization to avoid unnecessary computations
- no explicit parallelization of the simulation, only within the libraries

## Python API for the simulation

- mostly just straightforward wrappers for the C++ classes
- the only exception is the `PlanInstance` class, which needs to be able to get the underlying datastructure of green
  times and street orderings from Python easily and efficiently

## Rough outline of the classes in the simulation
- written in Cpp-like pseudocode

```cpp
// static data_ - read once at the beginning, never changed
class Intersection {
    const int id;
    const std::vector<const Street{*&} const> incoming;
    const std::vector<const Street{*&} const> outgoing; // redundant
};

class Instance {
    // static data_
    const Intersection{*&} const data_;

    // dynamic data_
    auto schedule;

    void print();

    // Returns a street that has a green light at `time`.
    Instance green(int time);

    // Is the `street` green at `time`?
    bool is_green(Instance street, int time);
};
```

```cpp
// static data_ - read once at the beginning, never changed
class Street {
    const int id;
    const Intersection{*&} start;
    const Intersection{*&} end;
    const std::string name;
    const int length;
}

class Instance {
    // static data_
    const Street{*&} const data_;

    // dynamic data_
    bool used;
    std::queue<Instance> queueing_cars;
    int _last_used_time;
    // interval_type green_interval; // interval of time when the street is green (modulo the cycle of the intersection)

    void print();

    // Returns the next time when the street has a green light.
    int next_green(int time);

    // Returns a car that goes through the end intersection at time.
    Instance go(int time);
};
```

```cpp
// static data_ - read once at the beginning, never changed
class Car {
    const int id;
    const int path_length; // number of streets in the car's path (redundant)
    const std::vector<const Street{*&} const> path;
};

class Instance {
    // static data_
    const Car{*&} const data_;
    // dynamic data_
    ?int current_street_index;
    bool finished_;
    int finish_time;

    void print();

    bool is_final_destination();

    Instance current_street();

    void move_to_next_street();
    void move_to_next_street(?int time);
};
```

```cpp
// static data_
class Plan {

};

// API for getting green times for each intersection and each incoming street and the ordering
class PlanInstance {
    // static data_ - may be redundant
    const Plan{*&} const data_;

    // precompute the green intervals once at the initialization of the schedule
    PlanInstance(...);

    int get_next_green_time(int time);

    // underlying structure with all raw data_
    // green times and permutations for each intersection and each incoming street
    std::vector<int> data_;
};
```

```cpp
// static data_ - read once at the beginning, never changed
class SimulationShared {
    const int duration;
    const std::vector<const Intersection{*&} const> intersections_data;
    const std::vector<const Street{*&} const> streets_data;
    const std::vector<const Car{*&} const> cars_data;
    const std::map<std::string, const Street{*&} const> street_mappping;
    const int bonus;

    // Initialize simulation data_ from file.
    SimulationShared(std::string filename);
};

class SimulationInstance {
    // static data_
    const SimulationShared{*&} const data_;

    // dynamic data_
    std::vector<Instance> intersections;
    std::vector<Instance> streets;
    std::vector<Instance> cars;

    PlanInstance schedule;

    // functions
    void print();

    // Run the simulation.
    // Use StablePriorityQueue for the event queue.
    void run();

    // Calculate and return the score.
    float score();

    // Read and initialize the schedule of the simulation from file.
    void read_schedule(std::string filename);

    // Initialize the schedule of the simulation.
    // For every intersection and every incoming street, set green for 1 second.
    void create_schedule_default();

    // Initialize the schedule of the simulation.
    // Set green for 1 second for every used incoming street. 
    // Exclude streets that aren't used.
    void create_schedule_used();

    // Create the output file.
    void create_output(std::string filename);

    Instance get_street_by_name(std::string name);
};
```

```cpp
// abstract class for events
class Event {
    int time;
};

class CAR_EVENT_TYPE : public Event {
    Instance *car;
};

class STREET_EVENT_TYPE : public Event {
    Instance *street;
};
```

```cpp
// wrapper for std::priority_queue with stable sorting
// needed for the event queue
class StablePriorityQueue<T> {

};
```
