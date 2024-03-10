#include <pybind11/iostream.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "city_plan/city_plan.hpp"
#include "simulation/simulation.hpp"

namespace py = pybind11;

using namespace simulation;

PYBIND11_MODULE(simulation, m) {
    m.doc() = "pybind11 simulation module";

    py::class_<Simulation>(m, "Simulation")
        .def(
            py::init<const city_plan::CityPlan &>(),
            py::arg("city_plan"),
            // Not really sure if the keep_alive is necessary here (I tested it without it)
            // but it matches the second example in the reference and it's a safe bet
            // https://pybind11.readthedocs.io/en/stable/advanced/functions.html#keep-alive
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
            "default_schedules",
            &Simulation::default_schedules,
            py::call_guard<py::gil_scoped_release>()
        )
        .def(
            "update_schedules",
            &Simulation::update_schedules,
            py::arg("schedules"),
            py::arg("relative") = true,
            py::call_guard<py::gil_scoped_release>()
        )
        //.def(
        //    "update_schedules",
        //    [](const Simulation &s, const std::vector<std::pair<std::vector<size_t>, std::vector<size_t>>> &schedules) {
        //        s.c
        //    }
        //)
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
        //.def_property(
        //    "schedules",
        //    &Simulation::schedules,
        //    &Simulation::set_schedules
        //)
        .def_property_readonly(
            "schedules",
            &Simulation::schedules
        );

    // Factory function is ok
    // https://pybind11.readthedocs.io/en/stable/advanced/classes.html?highlight=factory#custom-constructors
    m.def(
        "default_simulation",
        &default_simulation,
        py::arg("city_plan")//,
        //py::keep_alive<1, 2>()
    );

    py::class_<Schedule>(m, "Schedule")
        //.def(
        //    "set",
        //    &Schedule::set,
        //    py::arg("order"),
        //    py::arg("times")
        //)
        //.def(
        //    "get",
        //    &Schedule::get
        //)
        .def_property_readonly(
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
        );
}