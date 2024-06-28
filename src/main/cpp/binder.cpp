#include <pybind11/pybind11.h>
#include "order_book.hpp"

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;

PYBIND11_MODULE(book, m) {
    m.doc() = R"pbdoc(
        .. currentmodule:: order_book
        .. autosummary::
           :toctree: _generate

           generate_order_id
    )pbdoc";

    py::class_<OrderBook>(m, "OrderBook")
        .def(py::init<>())  
        .def("generate_order_id", &OrderBook::generate_order_id)
        .def("add_order", &OrderBook::add_order)
        .def("get_limits_dll", &OrderBook::get_limits_dll);

    m.attr("__version__") = "0.0.1";
}