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
        .def_property_readonly("bonus", &city_plan::CityPlan::bonus)
        .def(py::pickle(
                    [](const city_plan::CityPlan &c) {
                        return py::make_tuple(
                                c.cars(),
                                c.intersections(),
                                c.streets(),
                                c.bonus(),
                                c.duration());
                    },
                    [](const py::tuple &t) {
                        if (t.size() != 5) {
                        throw std::runtime_error("Invalid state!");
                    }
                        //return city_plan::CityPlan {
                        //    t[0].cast<std::vector<city_plan::Car> &&>(),
                        //    t[1].cast<std::vector<city_plan::Intersection> &&>(),
                        //    t[2].cast<std::vector<city_plan::Street> &&>(),
                        //    t[3].cast<size_t>(),
                        //    t[4].cast<size_t>()
                        //};
                        return city_plan::CityPlan{
                                t[0].cast<std::vector<city_plan::Car>>(),
                                t[1].cast<std::vector<city_plan::Intersection>>(),
                                t[2].cast<std::vector<city_plan::Street>>(),
                                t[3].cast<size_t>(),
                                t[4].cast<size_t>()};
                    }
        ));

    py::class_<city_plan::Car>(m, "Car")
        .def_property_readonly("id", &city_plan::Car::id)
        .def_property_readonly("path", &city_plan::Car::path)
        .def(py::pickle(
                [](const city_plan::Car &c) {
                    return py::make_tuple(c.id(), c.path());
                },
                    [](const py::tuple &t) {
                        if (t.size() != 2) {
                        throw std::runtime_error("Invalid state!");
                    }
                        //return city_plan::Car {
                        //    t[0].cast<size_t>(),
                        //    t[1].cast<std::vector<size_t> &&>(),
                        //};
                        return city_plan::Car{
                                t[0].cast<size_t>(),
                                t[1].cast<std::vector<size_t>>(),
                        };
                }
        ));

    py::class_<city_plan::Intersection>(m, "Intersection")
        .def_property_readonly("id", &city_plan::Intersection::id)
            .def_property_readonly("incoming_streets", &city_plan::Intersection::incoming_streets)
            .def_property_readonly("used", &city_plan::Intersection::used)
            .def_property_readonly("non_trivial", &city_plan::Intersection::non_trivial)
            .def(py::pickle(
                [](const city_plan::Intersection &i) {
                    return py::make_tuple(
                                i.id(),
                                i.incoming_streets(),
                                i.used(),
                                i.non_trivial());
                    },
                    [](const py::tuple &t) {
                        if (t.size() != 4) {
                        throw std::runtime_error("Invalid state!");
                    }
                        city_plan::Intersection i{t[0].cast<size_t>()};
                        //i.set_incoming(t[1].cast<std::vector<size_t> &&>());
                        i.set_incoming_streets(t[1].cast<std::vector<size_t>>());
                        i.set_used(t[2].cast<bool>());
                    i.set_non_trivial(t[3].cast<bool>());
                    return i;
                }
        ));

    py::class_<city_plan::Street>(m, "Street")
        .def_property_readonly("id", &city_plan::Street::id)
        .def_property_readonly("name", &city_plan::Street::name)
            .def_property_readonly("used", &city_plan::Street::used)
            .def_property_readonly("length", &city_plan::Street::length)
        .def_property_readonly("start", &city_plan::Street::start)
        .def_property_readonly("end", &city_plan::Street::end)
        .def(py::pickle(
                [](const city_plan::Street &s) {
                    return py::make_tuple(
                                s.id(),
                                s.start(),
                                s.end(),
                                s.name(),
                                s.length(),
                                s.used());
                    },
                    [](const py::tuple &t) {
                        if (t.size() != 6) {
                        throw std::runtime_error("Invalid state!");
                    }
                        //city_plan::Street s{
                        //    t[0].cast<size_t>(),
                        //    t[1].cast<size_t>(),
                        //    t[2].cast<size_t>(),
                        //    t[3].cast<std::string &&>(),
                        //    t[4].cast<size_t>()
                        //};
                        city_plan::Street s{
                            t[0].cast<size_t>(),
                            t[1].cast<size_t>(),
                            t[2].cast<size_t>(),
                            t[3].cast<std::string>(),
                            t[4].cast<size_t>()};
                    s.set_used(t[5].cast<bool>());
                    return s;
                }
        ));
}