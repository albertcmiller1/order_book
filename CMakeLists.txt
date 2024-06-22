cmake_minimum_required(VERSION 3.4...3.18)
project(order_book)

add_library(mymath STATIC src/book/mymath.cpp)
set_target_properties(mymath PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_subdirectory(pybind11) 
pybind11_add_module(order_book src/binder.cpp) # pybind wrapper around add_library cmake function

target_link_libraries(order_book PRIVATE mymath)