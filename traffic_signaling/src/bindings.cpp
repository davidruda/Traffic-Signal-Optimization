#include <pybind11/iostream.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "city_plan/city_plan.hpp"
#include "simulation/simulation.hpp"

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
            .def(py::pickle(
                    [](const city_plan::CityPlan &c) { // __getstate__
                        return py::make_tuple(
                                c.cars(),
                                c.intersections(),
                                c.streets(),
                                c.bonus(),
                                c.duration());
                    },
                    [](py::tuple t) { // __setstate__
                        if (t.size() != 5) {
                            throw std::runtime_error("Invalid state!");
                        }
                        city_plan::CityPlan c{
                                t[0].cast<std::vector<city_plan::Car>>(),
                                t[1].cast<std::vector<city_plan::Intersection>>(),
                                t[2].cast<std::vector<city_plan::Street>>(),
                                t[3].cast<size_t>(),
                                t[4].cast<size_t>()};
                        return c;
                    }));

    py::class_<city_plan::Car>(city_plan_submodule, "Car")
            .def_property_readonly("id", &city_plan::Car::id)
            .def_property_readonly("path", &city_plan::Car::path)
            .def(py::pickle(
                    [](const city_plan::Car &c) {
                        return py::make_tuple(c.id(), c.path());
                    },
                    [](py::tuple t) {
                        if (t.size() != 2) {
                            throw std::runtime_error("Invalid state!");
                        }
                        city_plan::Car c{
                                t[0].cast<size_t>(),
                                t[1].cast<std::vector<size_t>>(),
                        };
                        return c;
                    }));

    py::class_<city_plan::Intersection>(city_plan_submodule, "Intersection")
            .def_property_readonly("id", &city_plan::Intersection::id)
            .def_property_readonly("incoming", &city_plan::Intersection::incoming)
            .def_property_readonly("used", &city_plan::Intersection::is_used)
            .def_property_readonly("non_trivial", &city_plan::Intersection::is_non_trivial)
            .def(py::pickle(
                    [](const city_plan::Intersection &i) {
                        return py::make_tuple(
                                i.id(),
                                i.incoming(),
                                i.is_used(),
                                i.is_non_trivial());
                    },
                    [](py::tuple t) {
                        if (t.size() != 4) {
                            throw std::runtime_error("Invalid state!");
                        }
                        city_plan::Intersection i{
                                t[0].cast<size_t>()};
                        i.set_incoming(t[1].cast<std::vector<size_t>>());
                        i.set_used(t[2].cast<bool>());
                        i.set_non_trivial(t[3].cast<bool>());
                        return i;
                    }));

    py::class_<city_plan::Street>(city_plan_submodule, "Street")
            .def_property_readonly("id", &city_plan::Street::id)
            .def_property_readonly("name", &city_plan::Street::name)
            .def_property_readonly("used", &city_plan::Street::is_used)
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
                                s.is_used());
                    },
                    [](py::tuple t) {
                        if (t.size() != 6) {
                            throw std::runtime_error("Invalid state!");
                        }
                        city_plan::Street s{
                                t[0].cast<size_t>(),
                                t[1].cast<size_t>(),
                                t[2].cast<size_t>(),
                                t[3].cast<std::string>(),
                                t[4].cast<size_t>()};
                        s.set_used(t[5].cast<bool>());
                        return s;
                    }));

    auto simulation_submodule = m.def_submodule("simulation", "simulation submodule");
    py::class_<simulation::Simulation>(simulation_submodule, "Simulation")
            .def(py::init<const city_plan::CityPlan &>(), py::arg("city_plan"))
            .def("read_plan", &simulation::Simulation::read_plan, py::arg("filename"))
            .def("write_plan", &simulation::Simulation::write_plan, py::arg("filename"))
            .def("create_plan_default", &simulation::Simulation::create_plan_default)
            .def("score", &simulation::Simulation::score, py::arg("verbose") = false, py::call_guard<py::scoped_ostream_redirect>())
            .def_property("schedules", &simulation::Simulation::schedules, nullptr);

    py::class_<simulation::Schedule>(simulation_submodule, "Schedule")
        .def("set_schedule", &simulation::Schedule::set_schedule, py::arg("times"), py::arg("order"))
        .def_property_readonly("length", &simulation::Schedule::length)
        .def_property_readonly("duration", &simulation::Schedule::duration);
}
