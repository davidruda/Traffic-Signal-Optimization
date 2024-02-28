#include <vector>
#include <functional>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "city_plan/city_plan.hpp"

namespace py = pybind11;

using namespace city_plan;

PYBIND11_MODULE(city_plan, m) {
    m.doc() = "pybind11 city_plan module";

    py::class_<CityPlan>(m, "CityPlan")
        .def(
            py::init<const std::string &>(),
            py::arg("filename"),
            py::call_guard<py::gil_scoped_release>()
        )
        .def_property_readonly(
            "intersections",
            &CityPlan::intersections
        )
        .def_property_readonly(
            "streets",
            &CityPlan::streets
        )
        .def_property_readonly(
            "cars",
            &CityPlan::cars
        )
        .def_property_readonly(
            "duration",
            &CityPlan::duration
        )
        .def_property_readonly(
            "bonus",
            &CityPlan::bonus
        )
        // Binding lambda functions
        // https://pybind11.readthedocs.io/en/stable/classes.html?highlight=lambda#binding-lambda-functions
        .def(
            "used_intersections",
            // necessary conversion because pybind doesn't support C++20 ranges/views
            [](const CityPlan &cp) {
                auto &&ui = cp.used_intersections();
                return std::vector<std::reference_wrapper<const Intersection>>{ui.begin(), ui.end()};
            }
        )
        .def(
            "non_trivial_intersections",
            // necessary conversion because pybind doesn't support C++20 ranges/views
            [](const CityPlan &cp) {
                auto &&nt = cp.non_trivial_intersections();
                return std::vector<std::reference_wrapper<const Intersection>>{nt.begin(), nt.end()};
            }
        );

    py::class_<Car>(m, "Car")
        .def_property_readonly(
            "id",
            &Car::id
        )
        .def_property_readonly(
            "path",
            &Car::path
        )
        .def(
            "path_duration",
            &Car::path_duration
        );

    py::class_<Intersection>(m, "Intersection")
        .def_property_readonly(
            "id",
            &Intersection::id
        )
        .def_property_readonly(
            "streets",
            &Intersection::streets
        )
        .def_property_readonly(
            "used_streets",
            &Intersection::used_streets
        )
        .def_property_readonly(
            "used",
            &Intersection::used
        )
        .def_property_readonly(
            "non_trivial",
            &Intersection::non_trivial
        );

    py::class_<Street>(m, "Street")
        .def_property_readonly(
            "id",
            &Street::id
        )
        .def_property_readonly(
            "name",
            &Street::name
        )
        .def_property_readonly(
            "used",
            &Street::used
        )
        .def_property_readonly(
            "length",
            &Street::length
        )
        .def_property_readonly(
            "start",
            &Street::start
        )
        .def_property_readonly(
            "end",
            &Street::end
        )
        .def_property_readonly(
            "total_cars",
            &Street::total_cars
        );
}