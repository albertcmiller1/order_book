#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
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

    py::enum_<OrderType>(m, "OrderType")
        .value("bid", OrderType::bid)
        .value("ask", OrderType::ask);

    py::class_<OrderBook>(m, "OrderBook")
        .def(py::init<>())  
        .def("num_orders", &OrderBook::num_orders)
        .def("num_limits", &OrderBook::num_limits)
        .def("prominent_limit", &OrderBook::prominent_limit)
        .def("get_limits", &OrderBook::get_limits)
        .def("process", &OrderBook::process)
        .def("add_order", &OrderBook::add_order)
        .def("cancel_order", &OrderBook::cancel_order)
        .def("order_in_queue", &OrderBook::order_in_queue);

    py::class_<Match>(m, "Match")
        .def(py::init<>())  
        .def_readwrite("match_id", &Match::match_id)
        .def_readwrite("bid_order_id", &Match::bid_order_id)
        .def_readwrite("ask_order_id", &Match::ask_order_id)
        .def_readwrite("shares", &Match::shares)
        .def_readwrite("limit_price", &Match::limit_price);

    py::class_<Money>(m, "Money")
        .def(py::init<const long &, const int>())
        .def("getDollars", &Money::getDollars)
        .def("getCents", &Money::getCents);
        // add operators and default constructors

    m.attr("__version__") = "0.0.1";
}