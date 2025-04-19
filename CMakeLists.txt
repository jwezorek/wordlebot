cmake_minimum_required(VERSION 3.16)

project(wordlebot VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(wordlebot
    wordle_state.cpp
    wordlebot.cpp
    words.cpp
)