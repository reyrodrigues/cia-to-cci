cmake_minimum_required(VERSION 3.18)

# Collect all makerom source files
file(GLOB MAKEROM_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../project_ctr/makerom/src/*.c
)

# Remove makerom.c (main) since we're building a library
list(FILTER MAKEROM_SOURCES EXCLUDE REGEX ".*/makerom\\.c$")

# Include directories for makerom and its dependencies
set(MAKEROM_DEPS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../project_ctr/makerom/deps)

set(MAKEROM_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../project_ctr/makerom/src
    ${MAKEROM_DEPS_DIR}/libmbedtls/include
    ${MAKEROM_DEPS_DIR}/libyaml/include
    ${MAKEROM_DEPS_DIR}/libblz/include
)

# Library directories for dependencies
set(MAKEROM_LIB_DIRS
    ${MAKEROM_DEPS_DIR}/libmbedtls/bin
    ${MAKEROM_DEPS_DIR}/libyaml/bin
    ${MAKEROM_DEPS_DIR}/libblz/bin
)

# Create static library from makerom sources + our wrapper
add_library(makerom_wrapper STATIC
    ${MAKEROM_SOURCES}
    makerom_lib.c
)

# Make this library depend on the makerom_deps custom target
add_dependencies(makerom_wrapper makerom_deps)

# Set include directories
target_include_directories(makerom_wrapper PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${MAKEROM_INCLUDE_DIRS}
)

# Link directories
target_link_directories(makerom_wrapper PUBLIC
    ${MAKEROM_LIB_DIRS}
)

# Link against makerom dependencies
target_link_libraries(makerom_wrapper PUBLIC
    mbedtls
    yaml
    blz
)

# Ensure position-independent code
set_target_properties(makerom_wrapper PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
)

# Set compile options
target_compile_options(makerom_wrapper PRIVATE
    -Wall
    -Wno-unused-value
    -Wno-unused-but-set-variable
)

# Platform-specific settings
if(APPLE)
    target_compile_options(makerom_wrapper PRIVATE -Wno-unused-private-field)
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(makerom_wrapper PRIVATE -D_GNU_SOURCE)
endif()
