#include <pybind11/iostream.h>
#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>

#include "city_plan/city_plan.hpp"
#include "simulation/simulation.hpp"

namespace py = pybind11;

using namespace simulation;

PYBIND11_MODULE(simulation, m) {
    m.doc() = "pybind11 simulation module";

    auto py_Schedule = py::class_<Schedule>(
        m,
        "Schedule",
        "Schedule of traffic lights for one intersection in the simulation."
    );

    auto py_Simulation = py::class_<Simulation>(
        m,
        "Simulation",
        "Simulation of the Traffic signaling problem for a given city plan."
    );

    py_Schedule.def_property_readonly(
        "length",
        &Schedule::length,
        "Return the number of streets in the schedule."
    )
    .def_property_readonly(
        "duration",
        &Schedule::duration,
        "Return the cycle duration of this schedule."
    )
    .def_property_readonly(
        "order",
        &Schedule::order,
        R"doc(
        Return the order of streets in the schedule.

        The streets are represented by their IDs.
        )doc"
    )
    .def_property_readonly(
        "times",
        &Schedule::times,
        "Return the green light times for each street in the order."
    )
    .def(
        "relative_order",
        // necessary conversion because pybind doesn't support C++20 ranges/views
        [](const Schedule &s) -> std::vector<unsigned long> {
            auto &&ro_view = s.relative_order();
            return {ro_view.begin(), ro_view.end()};
        },
        R"doc(
        Return the order of streets in the schedule.

        The streets are represented by their indices relative to the intersection.

        This method is only valid if the schedule contains used streets.
        )doc"
    )
    .def(
        "set",
        // necessary cast to choose the right overload
        // https://pybind11.readthedocs.io/en/stable/classes.html#overloaded-methods
        py::overload_cast<std::vector<unsigned long> &&, std::vector<unsigned long> &&, bool>(&Schedule::set),
        py::arg("order"),
        py::arg("times"),
        py::arg("relative_order") = false,
        R"doc(
        Set the schedule using the given order and times.

        :param order: Order of streets in the schedule.
        :param times: Green light times for each street in the order.
        :param relative_order: If True, `order` must be a list of street indices relative to the intersection.
        Otherwise, `order` must be a list of street IDs.
        )doc"
    )
    .def_property_readonly_static(
        "DEFAULT_DIVISOR",
        // https://pybind11.readthedocs.io/en/stable/advanced/classes.html#static-properties
        [](py::object /* self */) {
            return Schedule::DEFAULT_DIVISOR;
        },
        "Default divisor for scaled times schedule option."
    );

    m.def(
        "set_seed",
        &set_seed,
        py::arg("seed"),
        R"doc(
        Set the random seed used for schedules generation.

        :param seed: Value of the random seed.
        )doc"
    );

    py_Simulation.def(
        py::init<const city_plan::CityPlan &>(),
        py::arg("city_plan"),
        // https://pybind11.readthedocs.io/en/stable/advanced/functions.html#keep-alive
        //
        // 1: this pointer (Simulation), 2 - first argument (CityPlan)
        py::keep_alive<1, 2>(),
        R"doc(
        Create a simulation object for the given city plan.

        :param city_plan: City plan containing information from the input file.
        )doc"
    )
    .def(
        "load_schedules",
        &Simulation::load_schedules,
        py::arg("filename"),
        py::call_guard<py::gil_scoped_release>(),
        R"doc(
        Load and create schedules from the given file.

        :param filename: Path of the file containing schedules in the competition format.
        )doc"
    )
    .def(
        "save_schedules",
        &Simulation::save_schedules,
        py::arg("filename"),
        py::call_guard<py::gil_scoped_release>(),
        R"doc(
         Save the current schedules in the competition format to the given file.

        :param filename: Path of the file to save the schedules to.
        )doc"
    )
    .def(
        "create_schedules",
        &Simulation::create_schedules,
        py::arg("order"),
        py::arg("times"),
        py::arg("divisor") = Schedule::DEFAULT_DIVISOR,
        py::call_guard<py::gil_scoped_release>(),
        R"doc(
        Create schedules for all used intersections and used streets.

        :param order: Initialization option for the order of streets in the schedules.
        :param times: Initialization option for the green light times in the schedules.
        :param divisor: Divisor for the scaled times option.
        )doc"
    )
    .def(
        "default_schedules",
        &Simulation::default_schedules,
        py::call_guard<py::gil_scoped_release>(),
        R"doc(
        Create default schedules.

        This is an alias for `create_schedules('default', 'default')`.
        )doc"
    )
    .def(
        "adaptive_schedules",
        &Simulation::adaptive_schedules,
        py::call_guard<py::gil_scoped_release>(),
        R"doc(
        Create adaptive schedules.

        This is an alias for `create_schedules('adaptive', 'default')`.
        )doc"
    )
    .def(
        "random_schedules",
        &Simulation::random_schedules,
        py::call_guard<py::gil_scoped_release>(),
        R"doc(
        Create random schedules.

        This is an alias for `create_schedules('random', 'default')`.
        )doc"
    )
    .def(
        "scaled_schedules",
        &Simulation::scaled_schedules,
        py::arg("divisor") = Schedule::DEFAULT_DIVISOR,
        py::call_guard<py::gil_scoped_release>(),
        R"doc(
        Create scaled schedules.

        This is an alias for `create_schedules('default', 'scaled', divisor)`.

        :param divisor: Divisor for the scaled times option.
        )doc"
    )
    .def(
        "set_non_trivial_schedules",
        &Simulation::set_non_trivial_schedules,
        py::arg("schedules"),
        py::arg("relative_order") = false,
        py::call_guard<py::gil_scoped_release>(),
        R"doc(
        Set the schedules of non-trivial intersections.

        :param schedules: List of `(order, times)` tuples, where each tuple is a schedule for one intersection.
        :param relative_order: If True, `order` must be a list of street indices relative to each intersection.
        Otherwise, `order` must be a list of street IDs.

        This method assumes the input is valid and performs no validation.
        )doc"
    )
    .def(
        "non_trivial_schedules",
        &Simulation::non_trivial_schedules,
        py::arg("relative_order") = false,
        R"doc(
        Return the schedules of non-trivial intersections as a list of `(order, times)` tuples,
        where each tuple is a schedule for one intersection.

        :param relative_order: If True, `order` is a list of street indices relative to each intersection.
        Otherwise, `order` is a list of street IDs.
        )doc"
    )
    .def(
        "score",
        &Simulation::score,
        // Release the GIL when running the simulation
        //
        // This is especially important for this method because it
        // allows to run multiple simulations on multiple threads in parallel from Python
        // https://pybind11.readthedocs.io/en/stable/advanced/misc.html#global-interpreter-lock-gil
        py::call_guard<py::gil_scoped_release>(),
        R"doc(
        Calculate the score for the current setting of schedules.

        This method runs the simulation.
        )doc"
    )
    .def(
        "summary",
        &Simulation::summary,
        // DO NOT release the GIL when redirecting stdout!
        // https://pybind11.readthedocs.io/en/stable/advanced/pycpp/utilities.html#capturing-standard-output-from-ostream
        py::call_guard<py::scoped_ostream_redirect>(),
        "Print a summary of the simulation statistics."
    )
    .def_property_readonly(
        "schedules",
        &Simulation::schedules,
        "Return the current schedules as a dictionary indexed by intersection IDs."
    );

    // Factory function is ok
    // https://pybind11.readthedocs.io/en/stable/advanced/classes.html?highlight=factory#custom-constructors
    m.def(
        "default_simulation",
        &default_simulation,
        py::arg("city_plan"),
        // 0: return value (Simulation), 1 - first argument (CityPlan)
        py::keep_alive<0, 1>(),
        R"doc(
        Create a simulation with default schedules for the given city plan.

        This is a shortcut for creating a Simulation object and calling its `default_schedules()` method.

        :param city_plan: City plan containing information from the input file.
        )doc"
    );

    m.def(
        "adaptive_simulation",
        &adaptive_simulation,
        py::arg("city_plan"),
        // 0: return value (Simulation), 1 - first argument (CityPlan)
        py::keep_alive<0, 1>(),
        R"doc(
        Create a simulation with adaptive schedules for the given city plan.

        This is a shortcut for creating a Simulation object and calling its `adaptive_schedules()` method.

        :param city_plan: City plan containing information from the input file.
        )doc"
    );
}