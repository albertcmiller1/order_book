cmake_minimum_required(VERSION 3.4...3.18)
set(CMAKE_CXX_STANDARD 20 CACHE STRING "C++ version selection")

project(book)
add_library(order_book STATIC src/main/cpp/order_book.cpp)
set_target_properties(order_book PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_subdirectory(pybind11) 
pybind11_add_module(book src/main/cpp/binder.cpp) # pybind wrapper around add_library cmake function

target_link_libraries(book PRIVATE order_book)