#ifndef BINDINGS_HPP
#define BINDINGS_HPP

#include <sstream>

#include <pybind11/iostream.h>
#include <pybind11/pybind11.h>

#include "simulation.hpp"

namespace py = pybind11;

PYBIND11_MODULE(simulation, m) {
    m.doc() = "pybind11 simulation plugin";// optional module docstring

    py::class_<CityPlan>(m, "CityPlan")
            .def(py::init<const std::string &>(), py::arg("filename"))
            .def("__str__", [](const CityPlan &obj) {
                std::ostringstream oss;
                oss << obj;// Calls operator<<
                return oss.str();
            });

    py::class_<Simulation>(m, "Simulation")
            .def(py::init<const CityPlan &>(), py::arg("city_plan"))
            .def("read_plan", &Simulation::read_plan, py::arg("filename"))
            .def("write_plan", &Simulation::write_plan, py::arg("filename"))
            .def("create_plan_default", &Simulation::create_plan_default)
            .def("run", &Simulation::run)
            .def("score", &Simulation::score)
            .def("summary", &Simulation::summary, py::call_guard<py::scoped_ostream_redirect>());
}

#endif
