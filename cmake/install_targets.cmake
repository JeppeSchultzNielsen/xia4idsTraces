# SO versioning
set_target_properties(xia4ids PROPERTIES SOVERSION ${PROJECT_VERSION})

# Make the install targets
install(TARGETS xia4ids xia4ids_runner
    EXPORT xia4idsTargets         # note: preliminary step to exporting the library
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT lib
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT lib)
# Install headers, preserving directory hierarchy
install(DIRECTORY ${CMAKE_SOURCE_DIR}/inc/ # use https://cmake.org/cmake/help/latest/command/target_sources.html#file-sets when CMake version >= 3.23 can be expected
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xia4ids
    COMPONENT include)
# For use with print_install_digest() at bottom of file
list(APPEND INSTALL_COMPONENTS lib bin)
list(APPEND INSTALL_PATHS ${CMAKE_INSTALL_LIBDIR} ${CMAKE_INSTALL_BINDIR})

# Export targets -- i.e. make find_package() work in other CMake projects
set(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR}) # generic name, allowing easy reuse of xia4idsConfig.cmake.in
set(LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}) # generic name, allowing easy reuse of xia4idsConfig.cmake.in
install(EXPORT xia4idsTargets DESTINATION ${PROJECT_CMAKEDIR} NAMESPACE xia4ids:: COMPONENT dev)
add_library(xia4ids::xia4ids ALIAS xia4ids) # this is good practice; using the library internally (e.g. in this CMakeLists.txt file) and externally (as an exported namespace, to be imported) is equivalent
add_executable(AUSA::xia4ids_runner ALIAS xia4ids_runner)
include(CMakePackageConfigHelpers)
write_basic_package_version_file(${PROJECT_BINARY_DIR}/${PROJECT_CMAKEDIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION} COMPATIBILITY AnyNewerVersion)
configure_package_config_file(${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
    ${PROJECT_BINARY_DIR}/${PROJECT_CMAKEDIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION ${PROJECT_CMAKEDIR}
    PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR)
install(FILES ${PROJECT_BINARY_DIR}/${PROJECT_CMAKEDIR}/${PROJECT_NAME}Config.cmake
    ${PROJECT_BINARY_DIR}/${PROJECT_CMAKEDIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION ${PROJECT_CMAKEDIR} COMPONENT dev)
# For use with print_install_digest() at bottom of file
list(APPEND INSTALL_COMPONENTS dev)
list(APPEND INSTALL_PATHS ${PROJECT_CMAKEDIR})

# Make local install feasible, reusing the install and export targets
file(WRITE ${PROJECT_BINARY_DIR}/${PROJECT_CMAKEDIR}/Find${PROJECT_NAME}.cmake
    "include(\${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}Config.cmake)")
add_custom_command(TARGET xia4ids PRE_BUILD # this step is quite hacky, please report it to the developers if you receive an error from this line
    COMMAND ${CMAKE_COMMAND} -E copy
    ${PROJECT_BINARY_DIR}/CMakeFiles/Export/${PROJECT_CMAKEDIR}/${PROJECT_NAME}Targets*.cmake
    ${PROJECT_BINARY_DIR}/${PROJECT_CMAKEDIR}
    BYPRODUCTS ${PROJECT_BINARY_DIR}/${PROJECT_CMAKEDIR}/${PROJECT_NAME}Targets*.cmake)

# Report components to be installed to user
print_install_digest("${INSTALL_COMPONENTS}" "${INSTALL_PATHS}") # Custom function

# Package library with CPack
package_project() # Custom function