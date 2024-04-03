find_package(ROOT REQUIRED)
find_package(GSL REQUIRED)
find_package(AUSALIB) # AUSAlib provides ROOT
if(NOT AUSALIB_FOUND)
    message(FATAL_ERROR "System-wide installation of AUSAlib from the Subatomic Group at Aarhus University is required. Get it here: https://gitlab.au.dk/ausa/ausalib")
endif()