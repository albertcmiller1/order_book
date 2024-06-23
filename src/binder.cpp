#include <pybind11/pybind11.h>
#include "book/order_book.hpp"
// #include "book/order_book.hpp"

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;

PYBIND11_MODULE(book, m) {
    m.doc() = R"pbdoc(
        Pybind11 example plugin 
        -----------------------

        .. currentmodule:: order_book

        .. autosummary::
           :toctree: _generate

           add
           subtract
           val_in_set
    )pbdoc";

    py::class_<OrderBook>(m, "OrderBook")
        .def(py::init<>())  
        .def("generate_order_id", &OrderBook::generate_order_id);

    m.attr("__version__") = "0.0.1";
}
