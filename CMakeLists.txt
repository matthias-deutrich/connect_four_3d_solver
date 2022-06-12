cmake_minimum_required(VERSION 3.12)
project(connect_four_3d_solver)

set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")

add_executable(connect_four_3d_solver main.cpp Game.cpp Game.h BoardState.cpp BoardState.h)
