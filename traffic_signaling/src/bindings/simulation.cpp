#include <pybind11/iostream.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "city_plan/city_plan.hpp"
#include "simulation/simulation.hpp"

namespace py = pybind11;

PYBIND11_MODULE(simulation, m) {
    m.doc() = "pybind11 simulation module";

    py::class_<simulation::Simulation>(m, "Simulation")
        .def(py::init<const city_plan::CityPlan &>(), py::arg("city_plan"))
        .def("read_plan", &simulation::Simulation::read_plan, py::arg("filename"))
        .def("write_plan", &simulation::Simulation::write_plan, py::arg("filename"))
        .def("create_plan_default", &simulation::Simulation::create_plan_default)
        .def("score", &simulation::Simulation::score, py::arg("verbose") = false, py::call_guard<py::scoped_ostream_redirect>())
        .def_property("schedules", &simulation::Simulation::schedules, nullptr);

    py::class_<simulation::Schedule>(m, "Schedule")
        .def("set_schedule", &simulation::Schedule::set_schedule, py::arg("times"), py::arg("order"))
        .def_property_readonly("length", &simulation::Schedule::length)
        .def_property_readonly("duration", &simulation::Schedule::duration);
}