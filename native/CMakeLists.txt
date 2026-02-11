cmake_minimum_required(VERSION 3.18)

# Build Project_CTR dependencies using their existing makefile system
# This is the most reliable way to build the complex dependency tree

message(STATUS "Building Project_CTR dependencies...")

# Build ctrtool dependencies
add_custom_target(ctrtool_deps ALL
    COMMAND ${CMAKE_MAKE_PROGRAM} deps
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/project_ctr/ctrtool
    COMMENT "Building ctrtool dependencies via makefile"
)

# Build makerom dependencies
add_custom_target(makerom_deps ALL
    COMMAND ${CMAKE_MAKE_PROGRAM} deps
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/project_ctr/makerom
    COMMENT "Building makerom dependencies via makefile"
)

# Now build our wrapper libraries
add_subdirectory(ctrtool_wrapper)
add_subdirectory(makerom_wrapper)
