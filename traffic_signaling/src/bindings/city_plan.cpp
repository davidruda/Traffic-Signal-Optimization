#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "city_plan/city_plan.hpp"

namespace py = pybind11;

PYBIND11_MODULE(city_plan, m) {
    m.doc() = "pybind11 city_plan module";

    py::class_<city_plan::CityPlan>(m, "CityPlan")
        .def(py::init<const std::string &>(), py::arg("filename"))
        .def_property_readonly("intersections", &city_plan::CityPlan::intersections)
        .def_property_readonly("streets", &city_plan::CityPlan::streets)
        .def_property_readonly("cars", &city_plan::CityPlan::cars)
        .def_property_readonly("duration", &city_plan::CityPlan::duration)
        .def_property_readonly("bonus", &city_plan::CityPlan::bonus);

    py::class_<city_plan::Car>(m, "Car")
        .def_property_readonly("id", &city_plan::Car::id)
        .def_property_readonly("path", &city_plan::Car::path);

    py::class_<city_plan::Intersection>(m, "Intersection")
        .def_property_readonly("id", &city_plan::Intersection::id)
        .def_property_readonly("incoming_streets", &city_plan::Intersection::incoming_streets)
        .def_property_readonly("used", &city_plan::Intersection::used)
        .def_property_readonly("non_trivial", &city_plan::Intersection::non_trivial);

    py::class_<city_plan::Street>(m, "Street")
        .def_property_readonly("id", &city_plan::Street::id)
        .def_property_readonly("name", &city_plan::Street::name)
        .def_property_readonly("used", &city_plan::Street::used)
        .def_property_readonly("length", &city_plan::Street::length)
        .def_property_readonly("start", &city_plan::Street::start)
        .def_property_readonly("end", &city_plan::Street::end);
}