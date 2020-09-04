cmake_minimum_required(VERSION 3.15)
project(gitgitgit)

set(CMAKE_CXX_STANDARD 11)


SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}-pthread")


add_executable(gitgitgit main.cpp Command.cpp Command.h ex1.cpp ex1.h Interpreter.cpp Interpreter.h)