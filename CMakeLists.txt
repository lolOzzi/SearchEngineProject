cmake_minimum_required(VERSION 3.28.3)
project(SearchEngineProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 1. Gather all C++ source files in the src directory
file(GLOB_RECURSE ALL_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# 2. Identify test files that likely have their own main() functions
set(TEST_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/test.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/TestDynamicArray.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/components/sorters/testSorter.cpp"
)
file(GLOB_RECURSE COMPONENT_TESTS "${CMAKE_CURRENT_SOURCE_DIR}/src/components/test/*.cpp")
list(APPEND TEST_FILES ${COMPONENT_TESTS})

# 3. Separate core library files from tests and the main entry point
set(CORE_SRC_FILES ${ALL_SRC_FILES})
list(REMOVE_ITEM CORE_SRC_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Main.cpp"
        ${TEST_FILES}
)

# 4. Define the main executable
add_executable(SearchEngineProject
        src/Main.cpp
        ${CORE_SRC_FILES}
)

# 5. Link include directories so headers can be resolved easily
# This allows you to use `#include "BasicHasher.h"` instead of `#include "../../components/hashers/BasicHasher.h"`
target_include_directories(SearchEngineProject PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/components/hashers
        ${CMAKE_CURRENT_SOURCE_DIR}/src/components/preprocessors
        ${CMAKE_CURRENT_SOURCE_DIR}/src/components/rankers
        ${CMAKE_CURRENT_SOURCE_DIR}/src/components/searchers
        ${CMAKE_CURRENT_SOURCE_DIR}/src/components/sorters
        ${CMAKE_CURRENT_SOURCE_DIR}/src/components/stores
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core
        ${CMAKE_CURRENT_SOURCE_DIR}/src/extras/basic
        ${CMAKE_CURRENT_SOURCE_DIR}/src/extras/basic/bursttrie
        ${CMAKE_CURRENT_SOURCE_DIR}/src/extras/basic/recsplit
        ${CMAKE_CURRENT_SOURCE_DIR}/src/extras/basic/recsplit/hashfunctions
        ${CMAKE_CURRENT_SOURCE_DIR}/src/indexes
)

# ==============================================================================
# OPTIONAL: Uncomment below if you want to compile your test files into
# their own separate standalone testing executables.
# ==============================================================================
# add_executable(SearchEngineTests ${TEST_FILES} ${CORE_SRC_FILES})
# target_include_directories(SearchEngineTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src ...)