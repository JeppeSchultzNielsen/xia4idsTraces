# Prepare library 'xia4ids'
file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE HEADERS RELATIVE ${CMAKE_SOURCE_DIR} CONFIGURE_DEPENDS "inc/*.h*")
list(REMOVE_ITEM SOURCES src/xia4ids.cpp)
list(REMOVE_ITEM HEADERS inc/xia4ids.hh)
add_library(xia4ids ${SOURCES} ${HEADERS})
target_link_libraries(xia4ids PUBLIC ROOT::Tree ROOT::Hist GSL::gsl)
target_include_directories(xia4ids PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/inc>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# Prepare executable 'xia4ids_runner' with file name 'xia4ids'
add_executable(xia4ids_runner ${SOURCES} ${HEADERS} src/xia4ids.cpp inc/xia4ids.hh)
target_link_libraries(xia4ids_runner PRIVATE xia4ids)
set_target_properties(xia4ids_runner PROPERTIES OUTPUT_NAME xia4ids)