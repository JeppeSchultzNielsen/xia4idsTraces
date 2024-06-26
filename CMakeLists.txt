# Note for developers: PROJECT_VERSION is set in the file named "VERSION"
# at the root of the project. You are encouraged to increment this once in
# a while, if you feel like it, when pushing an update to the repository.
# In CMake, project versioning has the following syntax:
#     MAJOR[.MINOR[.PATCH[.TWEAK]]]
cmake_minimum_required(VERSION 3.15)
project(xia4idsTraces)
file(READ ${CMAKE_SOURCE_DIR}/VERSION CMAKE_PROJECT_VERSION) # set XIA4IDS project version

# Procure required and optional packages
include(cmake/dependencies.cmake)

# Define project targets, specify their dependencies, prepare their linkage, etc.
#include(cmake/target_definitions.cmake)

# Define and prepare installation targets
#include(cmake/install_targets.cmake)

include_directories(${CMAKE_SOURCE_DIR}/inc)
file(GLOB SOURCES "src/*.cpp" "src/*.c" "src/xia4idsMethods/*.cpp")
file(GLOB HEADERS "inc/*.h" "inc/*.hh")

add_executable(xia4idsTraces ${SOURCES} ${HEADERS} "src/exec/main.cpp")
target_link_libraries(xia4idsTraces PUBLIC ROOT::Tree ROOT::Hist ROOT::Gpad GSL::gsl)

add_executable(go4pixie ${SOURCES} ${HEADERS} "src/exec/go4pixie.cpp")
target_link_libraries(go4pixie PUBLIC ROOT::Tree ROOT::Hist ROOT::Gpad GSL::gsl)