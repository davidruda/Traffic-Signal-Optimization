#include <vector>
#include <functional>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "city_plan/city_plan.hpp"

namespace py = pybind11;

using namespace city_plan;

PYBIND11_MODULE(city_plan, m) {
    m.doc() = "pybind11 city_plan module";

    // Avoid C++ types in docstrings
    // https://pybind11.readthedocs.io/en/latest/advanced/misc.html#avoiding-cpp-types-in-docstrings
    auto py_Car = py::class_<Car>(
        m,
        "Car",
        // https://en.cppreference.com/w/cpp/language/string_literal
        R"doc(
        Car in the city plan.

        Contains information about a car known from the input data.
        )doc"
    );

    auto py_Intersection = py::class_<Intersection>(
        m,
        "Intersection",
        R"doc(
        Intersection in the city plan.

        Contains information about an intersection known from the input data.
        )doc"
    );

    auto py_Street = py::class_<Street>(
        m,
        "Street",
        R"doc(
        Street in the city plan.

        Contains information about a street known from the input data.
        )doc"
    );

    auto py_CityPlan = py::class_<CityPlan>(
        m,
        "CityPlan",
        R"doc(
        City plan of a given dataset.

        Contains all information about intersections, streets, and cars known from the input data.
        )doc"
    );

    py_Car.def_property_readonly(
        "id",
        &Car::id,
        "Return the ID of the car."
    )
    .def_property_readonly(
        "path",
        &Car::path,
        "Return the path the car has to travel."
    )
    .def(
        "path_duration",
        &Car::path_duration,
        "Return the duration in seconds to travel the path without waiting at traffic lights."
    );

    py_Intersection.def_property_readonly(
        "id",
        &Intersection::id,
        "Return the ID of the intersection."
    )
    .def_property_readonly(
        "streets",
        &Intersection::streets,
        "Return the incoming streets to the intersection."
    )
    .def_property_readonly(
        "used_streets",
        &Intersection::used_streets,
        "Return the used incoming streets to the intersection."
    )
    .def_property_readonly(
        "used",
        &Intersection::used,
        R"doc(
        Return True if the intersection is used.

        Used intersection has at least one used street.
        )doc"
    )
    .def_property_readonly(
        "non_trivial",
        &Intersection::non_trivial,
        R"doc(
        Return True if the intersection is non-trivial.

        Non-trivial intersection has two or more used streets.
        )doc"
    );

    py_Street.def_property_readonly(
        "id",
        &Street::id,
        "Return the ID of the street."
    )
    .def_property_readonly(
        "name",
        &Street::name,
        "Return the name of the street."
    )
    .def_property_readonly(
        "used",
        &Street::used,
        R"doc(
        Return True if the street is used.

        A street is used if it is a non-final street in the path of at least one car.
        )doc"
    )
    .def_property_readonly(
        "length",
        &Street::length,
        "Return the duration in seconds to drive through the street."
    )
    .def_property_readonly(
        "start",
        &Street::start,
        "Return the starting intersection of the street."
    )
    .def_property_readonly(
        "end",
        &Street::end,
        "Return the ending intersection of the street."
    )
    .def_property_readonly(
        "total_cars",
        &Street::total_cars,
        "Return the total number of cars that use this street."
    );

    py_CityPlan.def(
        py::init<const std::string &>(),
        py::arg("filename"),
        py::call_guard<py::gil_scoped_release>(),
        R"doc(
        Create a city plan from the input data file.

        :param filename: Path of the input data file.
        )doc"
    )
    .def_property_readonly(
        "intersections",
        &CityPlan::intersections,
        "Return the intersections in the city plan."
    )
    .def_property_readonly(
        "streets",
        &CityPlan::streets,
        "Return the streets in the city plan."
    )
    .def_property_readonly(
        "cars",
        &CityPlan::cars,
        "Return the cars in the city plan."
    )
    .def_property_readonly(
        "duration",
        &CityPlan::duration,
        "Return the duration of the simulation for this city plan."
    )
    .def_property_readonly(
        "bonus",
        &CityPlan::bonus,
        "Return the bonus awarded for each car that reaches its destination before the end of the simulation."
    )
    // Binding lambda functions
    // https://pybind11.readthedocs.io/en/stable/classes.html?highlight=lambda#binding-lambda-functions
    .def(
        "used_intersections",
        // necessary conversion because pybind doesn't support C++20 ranges/views
        [](const CityPlan &cp) -> std::vector<std::reference_wrapper<const Intersection>> {
            auto &&ui_view = cp.used_intersections();
            return {ui_view.begin(), ui_view.end()};
        },
        "Return the used intersections in this city plan."
    )
    .def(
        "non_trivial_intersections",
        // necessary conversion because pybind doesn't support C++20 ranges/views
        [](const CityPlan &cp) -> std::vector<std::reference_wrapper<const Intersection>> {
            auto &&nt_view = cp.non_trivial_intersections();
            return {nt_view.begin(), nt_view.end()};
        },
        "Return the non-trivial intersections in this city plan."
    )
    .def(
        "upper_bound",
        &CityPlan::upper_bound,
        "Return the theoretical maximum score if none of the cars ever has to wait at a traffic light."
    );
}