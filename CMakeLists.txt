# Note for developers: PROJECT_VERSION is set in the file named "VERSION"
# at the root of the project. You are encouraged to increment this once in
# a while, if you feel like it, when pushing an update to the repository.
# In CMake, project versioning has the following syntax:
#     MAJOR[.MINOR[.PATCH[.TWEAK]]]
cmake_minimum_required(VERSION 3.15)
project(xia4ids)
file(READ ${CMAKE_SOURCE_DIR}/VERSION CMAKE_PROJECT_VERSION) # set XIA4IDS project version

# Fetch custom CMake scripts commonly used across AUSA projects for source code distribution
include(FetchContent)
FetchContent_Declare(cmake_common GIT_REPOSITORY https://gitlab.au.dk/ausa/cmake_common.git)
FetchContent_MakeAvailable(cmake_common)

# Procure required and optional packages
include(cmake/dependencies.cmake)

# Define project targets, specify their dependencies, prepare their linkage, etc.
include(cmake/target_definitions.cmake)

# Define and prepare installation targets
include(cmake/install_targets.cmake)