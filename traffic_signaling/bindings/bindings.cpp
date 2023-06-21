#include <sstream>

#include <pybind11/iostream.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "city_plan.hpp"
#include "simulation.hpp"

namespace py = pybind11;

PYBIND11_MODULE(traffic_signaling, m) {
    m.doc() = "pybind11 traffic signaling plugin"; // optional module docstring

    auto city_plan_submodule = m.def_submodule("city_plan", "city_plan submodule");
    py::class_<city_plan::CityPlan>(city_plan_submodule, "CityPlan")
            .def(py::init<const std::string &>(), py::arg("filename"))
            .def_property_readonly("intersections", &city_plan::CityPlan::intersections)
            .def_property_readonly("streets", &city_plan::CityPlan::streets)
            .def_property_readonly("cars", &city_plan::CityPlan::cars)
            .def_property_readonly("duration", &city_plan::CityPlan::duration)
            .def_property_readonly("bonus", &city_plan::CityPlan::bonus)
            .def("__str__", [](const city_plan::CityPlan &obj) {
                std::ostringstream oss;
                oss << obj; // Calls operator<<
                return oss.str();
            });

    py::class_<city_plan::Car>(city_plan_submodule, "Car")
            .def_property_readonly("id", &city_plan::Car::id)
            .def_property_readonly("path", &city_plan::Car::path);

    py::class_<city_plan::Intersection>(city_plan_submodule, "Intersection")
            .def_property_readonly("id", &city_plan::Intersection::id)
            .def_property_readonly("incoming", &city_plan::Intersection::incoming);

    py::class_<city_plan::Street>(city_plan_submodule, "Street")
            .def_property_readonly("id", &city_plan::Street::id)
            .def_property_readonly("name", &city_plan::Street::name)
            .def_property_readonly("used", &city_plan::Street::is_used)
            .def_property_readonly("length", &city_plan::Street::length);

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
