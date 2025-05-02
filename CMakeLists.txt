cmake_minimum_required(VERSION 3.15)
project(SensorHub_UART)


include_directories (
        inc
        src
)

file(GLOB_RECURSE SOURCES "src/*.*")
file(GLOB_RECURSE INCLUDE "inc/*.*")

set(CMAKE_CXX_STANDARD 20)

add_executable(SensorHub_UART main.cpp
        ${SOURCES}
        ${INCLUDE}
)
