cmake_minimum_required(VERSION 3.21)
project(Algos)

set(CMAKE_CXX_STANDARD 14)

#add_executable(Algos main.cpp)


project(CodeGames)
add_executable(CodeGames cg.cpp)

project(TheLabyrinth)
add_executable(TheLabyrinth the-labyrinth/main.cpp)

project(JPS-Preprocessing)
add_executable(JPS-Preprocessing jps-preprocessing/main.cpp jps-preprocessing/main.cpp jps-preprocessing/jps_element.cpp jps-preprocessing/jps_element.h jps-preprocessing/point.h)

project(JPS-Runtime)
add_executable(JPS-Runtime jps-runtime/main.cpp jps-runtime/jps_element.h jps-runtime/jps_element.cpp jps-runtime/point.h)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
project(BrainFork)
add_executable(BrainFork brain-fork/main.cpp brain-fork/game_simple.cpp brain-fork/game_simple.h brain-fork/game_genetic.cpp brain-fork/game_genetic.h)

project(NotDFS)
add_executable(NotDFS death-first-search-2/main.cpp)

project(Minesweeper)
add_executable(Minesweeper minesweeper/main.cpp)

project(Nintendo)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
add_executable(Nintendo nintendo-contest/main.cpp)