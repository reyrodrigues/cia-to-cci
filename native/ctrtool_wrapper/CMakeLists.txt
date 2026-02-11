cmake_minimum_required(VERSION 3.18)

# Collect all ctrtool source files (both .cpp and .c)
file(GLOB CTRTOOL_CPP_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../project_ctr/ctrtool/src/*.cpp
)
file(GLOB CTRTOOL_C_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../project_ctr/ctrtool/src/*.c
)

# Remove main.cpp since we're building a library, not an executable
list(FILTER CTRTOOL_CPP_SOURCES EXCLUDE REGEX ".*/main\\.cpp$")

# Combine all sources
set(CTRTOOL_SOURCES ${CTRTOOL_CPP_SOURCES} ${CTRTOOL_C_SOURCES})

# Include directories for ctrtool and its dependencies
set(CTRTOOL_DEPS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../project_ctr/ctrtool/deps)

set(CTRTOOL_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../project_ctr/ctrtool/src
    ${CTRTOOL_DEPS_DIR}/libtoolchain/include
    ${CTRTOOL_DEPS_DIR}/libnintendo-n3ds/include
    ${CTRTOOL_DEPS_DIR}/libbroadon-es/include
    ${CTRTOOL_DEPS_DIR}/libmbedtls/include
    ${CTRTOOL_DEPS_DIR}/libfmt/include
)

# Library directories for dependencies
set(CTRTOOL_LIB_DIRS
    ${CTRTOOL_DEPS_DIR}/libtoolchain/bin
    ${CTRTOOL_DEPS_DIR}/libnintendo-n3ds/bin
    ${CTRTOOL_DEPS_DIR}/libbroadon-es/bin
    ${CTRTOOL_DEPS_DIR}/libmbedtls/bin
    ${CTRTOOL_DEPS_DIR}/libfmt/bin
)

# Create static library from ctrtool sources + our wrapper
add_library(ctrtool_wrapper STATIC
    ${CTRTOOL_SOURCES}
    ctrtool_lib.cpp
)

# Make this library depend on the ctrtool_deps custom target
add_dependencies(ctrtool_wrapper ctrtool_deps)

# Set include directories
target_include_directories(ctrtool_wrapper PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CTRTOOL_INCLUDE_DIRS}
)

# Link directories
target_link_directories(ctrtool_wrapper PUBLIC
    ${CTRTOOL_LIB_DIRS}
)

# Link against ctrtool dependencies
target_link_libraries(ctrtool_wrapper PUBLIC
    toolchain
    nintendo-n3ds
    broadon-es
    mbedtls
    fmt
)

# Ensure position-independent code
set_target_properties(ctrtool_wrapper PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Set compile options
target_compile_options(ctrtool_wrapper PRIVATE
    -Wall
    -Wno-unused-value
    -Wno-unused-but-set-variable
)

# Platform-specific settings
if(APPLE)
    target_compile_options(ctrtool_wrapper PRIVATE -Wno-unused-private-field)
endif()
