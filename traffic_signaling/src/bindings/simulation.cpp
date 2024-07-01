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

    auto py_Schedule = py::class_<Schedule>(m, "Schedule");
    auto py_Simulation = py::class_<Simulation>(m, "Simulation");

        py_Schedule.def_property_readonly(
            "length",
            &Schedule::length
        )
        .def_property_readonly(
            "duration",
            &Schedule::duration
        )
        .def_property_readonly(
            "order",
            &Schedule::order
        )
        .def_property_readonly(
            "times",
            &Schedule::times
        )
        .def(
            "relative_order",
            // necessary conversion because pybind doesn't support C++20 ranges/views
            [](const Schedule &s) -> std::vector<unsigned long> {
                auto &&ro_view = s.relative_order();
                return {ro_view.begin(), ro_view.end()};
            }
        )
        .def(
            "set",
            // necessary cast to choose the right overload
            // https://pybind11.readthedocs.io/en/stable/classes.html#overloaded-methods
            py::overload_cast<std::vector<unsigned long> &&, std::vector<unsigned long> &&, bool>(&Schedule::set),
            py::arg("order"),
            py::arg("times"),
            py::arg("relative_order") = false
        )
        .def_property_readonly_static(
            "DEFAULT_DIVISOR",
            // https://pybind11.readthedocs.io/en/stable/advanced/classes.html#static-properties
            [](py::object /* self */) {
                return Schedule::DEFAULT_DIVISOR;
            }
        );

    m.def(
        "set_seed",
        &set_seed,
        py::arg("seed")
    );

        py_Simulation.def(
            py::init<const city_plan::CityPlan &>(),
            py::arg("city_plan"),
            // https://pybind11.readthedocs.io/en/stable/advanced/functions.html#keep-alive
            //
            // 1: this pointer (Simulation), 2 - first argument (CityPlan)
            py::keep_alive<1, 2>()
        )
        .def(
            "load_schedules",
            &Simulation::load_schedules,
            py::arg("filename"),
            py::call_guard<py::gil_scoped_release>()
        )
        .def(
            "save_schedules",
            &Simulation::save_schedules,
            py::arg("filename"),
            py::call_guard<py::gil_scoped_release>()
        )
        .def(
            "create_schedules",
            &Simulation::create_schedules,
            py::arg("order"),
            py::arg("times"),
            py::arg("divisor") = Schedule::DEFAULT_DIVISOR,
            py::call_guard<py::gil_scoped_release>()
        )
        .def(
            "default_schedules",
            &Simulation::default_schedules,
            py::call_guard<py::gil_scoped_release>()
        )
        .def(
            "adaptive_schedules",
            &Simulation::adaptive_schedules,
            py::call_guard<py::gil_scoped_release>()
        )
        .def(
            "random_schedules",
            &Simulation::random_schedules,
            py::call_guard<py::gil_scoped_release>()
        )
        .def(
            "scaled_schedules",
            &Simulation::scaled_schedules,
            py::arg("divisor") = Schedule::DEFAULT_DIVISOR,
            py::call_guard<py::gil_scoped_release>()
        )
        .def(
            "set_non_trivial_schedules",
            &Simulation::set_non_trivial_schedules,
            py::arg("schedules"),
            py::arg("relative_order") = false,
            py::call_guard<py::gil_scoped_release>()
        )
        .def(
            "non_trivial_schedules",
            &Simulation::non_trivial_schedules,
            py::arg("relative_order") = false
        )
        .def(
            "score",
            &Simulation::score,
            // Release the GIL when running the simulation
            // This is especially important for this method because it
            // allows the simulation to run in parallel from Python
            // https://pybind11.readthedocs.io/en/stable/advanced/misc.html#global-interpreter-lock-gil
            py::call_guard<py::gil_scoped_release>()
        )
        .def(
            "summary",
            &Simulation::summary,
            // DO NOT release the GIL when redirecting stdout!
            // https://pybind11.readthedocs.io/en/stable/advanced/pycpp/utilities.html#capturing-standard-output-from-ostream
            py::call_guard<py::scoped_ostream_redirect>()
        )
        .def_property_readonly(
            "schedules",
            &Simulation::schedules
        );

    // Factory function is ok
    // https://pybind11.readthedocs.io/en/stable/advanced/classes.html?highlight=factory#custom-constructors
    m.def(
        "default_simulation",
        &default_simulation,
        py::arg("city_plan"),
        // 0: return value (Simulation), 1 - first argument (CityPlan)
        py::keep_alive<0, 1>()
    );

    m.def(
        "adaptive_simulation",
        &adaptive_simulation,
        py::arg("city_plan"),
        // 0: return value (Simulation), 1 - first argument (CityPlan)
        py::keep_alive<0, 1>()
    );
}