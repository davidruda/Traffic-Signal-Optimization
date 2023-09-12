#include <pybind11/iostream.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "city_plan/city_plan.hpp"
#include "simulation/simulation.hpp"

namespace py = pybind11;

PYBIND11_MODULE(simulation, m) {
    m.doc() = "pybind11 simulation module";

    py::class_<simulation::Simulation>(m, "Simulation")
        .def(py::init<const city_plan::CityPlan &>(),
            py::arg("city_plan"),
            py::keep_alive<1, 2>()
        )
        .def("read_schedules",
            &simulation::Simulation::read_schedules,
            py::arg("filename"),
            py::call_guard<py::gil_scoped_release>()
        )
        .def("write_schedules",
            &simulation::Simulation::write_schedules,
            py::arg("filename"),
            py::call_guard<py::gil_scoped_release>()
        )
        .def("default_schedules", &simulation::Simulation::default_schedules)
        .def("update_schedules",
            &simulation::Simulation::update_schedules,
            py::arg("schedules"),
            py::call_guard<py::gil_scoped_release>()
        )
        .def("score",
            &simulation::Simulation::score,
            py::arg("verbose") = false,
            py::call_guard<py::scoped_ostream_redirect, py::gil_scoped_release>()
        )
        .def_property("schedules",
            &simulation::Simulation::schedules,
            &simulation::Simulation::set_schedules
        )
        .def_property_readonly("city_plan", &simulation::Simulation::city_plan);

    py::class_<simulation::Schedule>(m, "Schedule")
        .def("set_schedule",
            &simulation::Schedule::set_schedule,
            py::arg("times"),
            py::arg("order")
        )
        .def_property_readonly("length", &simulation::Schedule::length)
        .def_property_readonly("duration", &simulation::Schedule::duration);
}