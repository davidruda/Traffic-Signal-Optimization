#include <sstream>

#include <pybind11/iostream.h>
#include <pybind11/pybind11.h>

#include "simulation.hpp"

namespace py = pybind11;

PYBIND11_MODULE(traffic_signaling, m) {
    m.doc() = "pybind11 traffic signaling plugin"; // optional module docstring

    auto city_plan_submodule = m.def_submodule("city_plan", "city_plan submodule");
    py::class_<city_plan::CityPlan>(city_plan_submodule, "CityPlan")
            .def(py::init<const std::string &>(), py::arg("filename"))
            .def("__str__", [](const city_plan::CityPlan &obj) {
                std::ostringstream oss;
                oss << obj; // Calls operator<<
                return oss.str();
            });

    auto simulation_submodule = m.def_submodule("simulation", "simulation submodule");
    py::class_<simulation::Simulation>(simulation_submodule, "Simulation")
            .def(py::init<const city_plan::CityPlan &>(), py::arg("city_plan"))
            .def("read_plan", &simulation::Simulation::read_plan, py::arg("filename"))
            .def("write_plan", &simulation::Simulation::write_plan, py::arg("filename"))
            .def("create_plan_default", &simulation::Simulation::create_plan_default)
            .def("run", &simulation::Simulation::run)
            .def("score", &simulation::Simulation::score)
            .def("summary", &simulation::Simulation::summary, py::call_guard<py::scoped_ostream_redirect>());
}
