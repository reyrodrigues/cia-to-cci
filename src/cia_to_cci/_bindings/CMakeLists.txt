cmake_minimum_required(VERSION 3.18)

# Use plain .so/.pyd suffix with no Python version tag.
# Python can load extension modules without the cpython-3XX tag.
if(WIN32)
    set(PYTHON_MODULE_EXT ".pyd")
else()
    set(PYTHON_MODULE_EXT ".so")
endif()

# Build ctrtool Python module
pybind11_add_module(_ctrtool ctrtool_bindings.cpp)

target_link_libraries(_ctrtool PRIVATE
    ctrtool_wrapper
)

set_target_properties(_ctrtool PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
    PREFIX ""
    SUFFIX "${PYTHON_MODULE_EXT}"
)

# Build makerom Python module
pybind11_add_module(_makerom makerom_bindings.cpp)

target_link_libraries(_makerom PRIVATE
    makerom_wrapper
)

set_target_properties(_makerom PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
    PREFIX ""
    SUFFIX "${PYTHON_MODULE_EXT}"
)
