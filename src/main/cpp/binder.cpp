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
        // .def("num_orders", &OrderBook::num_orders)
        // .def("num_limits", &OrderBook::num_limits)
        // .def("prominent_limit", &OrderBook::prominent_limit)
        // .def("get_limits", &OrderBook::get_limits)
        // .def("process", &OrderBook::process)
        .def("add_order", &OrderBook::add_order);
        // need to expose an enum class

    m.attr("__version__") = "0.0.1";
}