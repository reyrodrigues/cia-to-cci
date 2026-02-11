cmake_minimum_required(VERSION 3.18)
project(cia_to_cci VERSION 0.1.0 LANGUAGES C CXX)

# Set C++ and C standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Enable position-independent code globally (required for Python extensions)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Find Python - use find_package(Python3) for better compatibility
find_package(Python3 COMPONENTS Interpreter Development.Module REQUIRED)

# Find pybind11 - use Python to locate it
execute_process(
    COMMAND "${Python3_EXECUTABLE}" -c "import pybind11; print(pybind11.get_cmake_dir())"
    OUTPUT_VARIABLE pybind11_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE pybind11_RESULT
)

if(pybind11_RESULT EQUAL 0)
    list(APPEND CMAKE_PREFIX_PATH "${pybind11_DIR}")
endif()

find_package(pybind11 CONFIG REQUIRED)

# Add the native code subdirectory
add_subdirectory(native)

# Add the Python bindings subdirectory
add_subdirectory(src/cia_to_cci/_bindings)
