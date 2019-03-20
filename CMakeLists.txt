cmake_minimum_required(VERSION 3.12)
project(fiar_3d_cpp)

set(CMAKE_CXX_STANDARD 14)

add_executable(fiar_3d_cpp main.cpp Game.cpp Game.h BoardState.cpp BoardState.h)