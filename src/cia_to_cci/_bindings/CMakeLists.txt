cmake_minimum_required(VERSION 3.18)

# Build ctrtool Python module
pybind11_add_module(_ctrtool ctrtool_bindings.cpp)

target_link_libraries(_ctrtool PRIVATE
    ctrtool_wrapper
)

# Build makerom Python module
pybind11_add_module(_makerom makerom_bindings.cpp)

target_link_libraries(_makerom PRIVATE
    makerom_wrapper
)
