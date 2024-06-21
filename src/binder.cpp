#include <pybind11/pybind11.h>
#include "mymath.hpp"

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;

PYBIND11_MODULE(cmake_example, m) {
    m.doc() = R"pbdoc(
        Pybind11 example plugin
        -----------------------

        .. currentmodule:: cmake_example

        .. autosummary::
           :toctree: _generate

           add
           subtract
           val_in_set
    )pbdoc";

    m.def("add", &add, R"pbdoc(
        Add two numbers

        Some other explanation about the add function.
    )pbdoc");


    m.def("return_str", &return_str, "return the string!");



    m.def("subtract", [](int i, int j) { return i - j; }, R"pbdoc(
        Subtract two numbers

        Some other explanation about the subtract function.
    )pbdoc");

    m.def("val_in_set", &val_in_set, R"pbdoc(
        val in set
    )pbdoc");

    py::class_<Pet>(m, "Pet")
        .def(py::init<const std::string &>())   // constructor
        .def("setName", &Pet::setName)          // expose member methods
        .def("getName", &Pet::getName)          
        .def_readwrite("name", &Pet::name);     // expose member variables


    m.attr("__version__") = "0.0.1";
}
