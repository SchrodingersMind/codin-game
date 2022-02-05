cmake_minimum_required(VERSION 3.21)
project(Algos)

set(CMAKE_CXX_STANDARD 14)

add_executable(Algos main.cpp)


project(CodeGames)
add_executable(CodeGames cg.cpp)

project(TheLabyrinth)
add_executable(TheLabyrinth the-labyrinth/main.cpp)

project(JPS-Preprocessing)
add_executable(JPS-Preprocessing jps-preprocessing/main.cpp jps-preprocessing/main.cpp jps-preprocessing/jps_element.cpp jps-preprocessing/jps_element.h jps-preprocessing/point.h)

project(JPS-Runtime)
add_executable(JPS-Runtime jps-runtime/main.cpp jps-runtime/jps_element.h jps-runtime/jps_element.cpp jps-runtime/point.h)
