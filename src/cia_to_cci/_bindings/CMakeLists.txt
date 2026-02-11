cmake_minimum_required(VERSION 3.18)

# Build ctrtool Python module
# pybind11_add_module automatically detects Python version and sets correct suffix
pybind11_add_module(_ctrtool ctrtool_bindings.cpp)

target_link_libraries(_ctrtool PRIVATE
    ctrtool_wrapper
)

# Ensure module goes to the correct location for wheel packaging
# Let pybind11 handle the module suffix automatically
set_target_properties(_ctrtool PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
)

# Build makerom Python module
# pybind11_add_module automatically detects Python version and sets correct suffix
pybind11_add_module(_makerom makerom_bindings.cpp)

target_link_libraries(_makerom PRIVATE
    makerom_wrapper
)

# Ensure module goes to the correct location for wheel packaging
# Let pybind11 handle the module suffix automatically
set_target_properties(_makerom PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
)
