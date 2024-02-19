#include <pybind11/iostream.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "city_plan/city_plan.hpp"
#include "simulation/simulation.hpp"

namespace py = pybind11;

PYBIND11_MODULE(simulation, m) {
    m.doc() = "pybind11 simulation module";

    py::class_<simulation::Simulation>(m, "Simulation")
        .def(
            py::init<const city_plan::CityPlan &>(),
            py::arg("city_plan"),
            py::keep_alive<1, 2>()
        )
        .def(
            "load_schedules",
            &simulation::Simulation::load_schedules,
            py::arg("filename"),
            py::call_guard<py::gil_scoped_release>()
        )
        .def(
            "save_schedules",
            &simulation::Simulation::save_schedules,
            py::arg("filename"),
            py::call_guard<py::gil_scoped_release>()
        )
        .def(
            "default_schedules",
            &simulation::Simulation::default_schedules,
            py::call_guard<py::gil_scoped_release>()
        )
        .def(
            "update_schedules",
            &simulation::Simulation::update_schedules,
            py::arg("ids"),
            py::arg("schedules"),
            py::arg("relative") = true,
            py::call_guard<py::gil_scoped_release>()
        )
        .def(
            "score",
            &simulation::Simulation::score,
            py::call_guard<py::gil_scoped_release>()
        )
        .def(
            "summary",
            &simulation::Simulation::summary,
            py::call_guard<py::scoped_ostream_redirect>()
        )
        //.def_property(
        //    "schedules",
        //    &simulation::Simulation::schedules,
        //    &simulation::Simulation::set_schedules
        //)
        .def_property_readonly(
            "schedules",
            &simulation::Simulation::schedules
        );

    py::class_<simulation::Schedule>(m, "Schedule")
        //.def(
        //    "set",
        //    &simulation::Schedule::set,
        //    py::arg("order"),
        //    py::arg("times")
        //)
        //.def(
        //    "get",
        //    &simulation::Schedule::get
        //)
        .def_property_readonly(
            "length",
            &simulation::Schedule::length
        )
        .def_property_readonly(
            "duration",
            &simulation::Schedule::duration
        );
}