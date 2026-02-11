cmake_minimum_required(VERSION 3.18)

# Construct correct Python module suffix explicitly
# This prevents pybind11 from using cached/incorrect version detection
set(PYTHON_MODULE_SUFFIX ".cpython-${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR}")
if(APPLE)
    set(PYTHON_MODULE_SUFFIX "${PYTHON_MODULE_SUFFIX}-darwin${CMAKE_SHARED_MODULE_SUFFIX}")
elseif(UNIX)
    set(PYTHON_MODULE_SUFFIX "${PYTHON_MODULE_SUFFIX}-${CMAKE_SYSTEM_PROCESSOR}-linux-gnu${CMAKE_SHARED_MODULE_SUFFIX}")
endif()

# Build ctrtool Python module
pybind11_add_module(_ctrtool ctrtool_bindings.cpp)

target_link_libraries(_ctrtool PRIVATE
    ctrtool_wrapper
)

# Explicitly set the correct suffix to prevent version detection issues
set_target_properties(_ctrtool PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
    SUFFIX "${PYTHON_MODULE_SUFFIX}"
)

# Build makerom Python module
pybind11_add_module(_makerom makerom_bindings.cpp)

target_link_libraries(_makerom PRIVATE
    makerom_wrapper
)

# Explicitly set the correct suffix to prevent version detection issues
set_target_properties(_makerom PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
    SUFFIX "${PYTHON_MODULE_SUFFIX}"
)
