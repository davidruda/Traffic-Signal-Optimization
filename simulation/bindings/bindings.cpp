#ifndef BINDINGS_HPP
#define BINDINGS_HPP

#include <sstream>

#include <pybind11/pybind11.h>

#include "simulation.hpp"

namespace py = pybind11;

PYBIND11_MODULE(simulation, m) {
    m.doc() = "pybind11 simulation plugin";// optional module docstring

    py::class_<Simulation>(m, "Simulation")
            .def(py::init<const std::string &>(), py::arg("filename"))
            .def("create_instance", &Simulation::create_instance)
            .def("__str__", [](const Simulation &obj) {
                std::ostringstream oss;
                oss << obj;// Calls operator<<
                return oss.str();
            });

    py::class_<Simulation::Instance>(m, "SimulationInstance")
            .def("read_plan", &Simulation::Instance::read_plan, py::arg("filename"))
            .def("write_plan", &Simulation::Instance::write_plan, py::arg("filename"))
            .def("create_plan_default", &Simulation::Instance::create_plan_default)
            .def("run", &Simulation::Instance::run)
            .def("score", &Simulation::Instance::score, py::arg("verbose") = false);
}

#endif
