
cmake_minimum_required(VERSION 3.22.1)
project(DiseaseSimulation)

# Set the C++ standard
#set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories for headers
include_directories(${CMAKE_SOURCE_DIR})

# Add coverage flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")


# Add the executable for the main program
add_executable(main
    ../main.cpp
    ../simulation.cpp
)

# Add the executable for the tests
add_executable(test
    ../TestCoverage.cpp
    ../simulation.cpp
    
)


