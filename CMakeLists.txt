cmake_minimum_required(VERSION 4.1)
project(conway C)

set(CMAKE_C_STANDARD 11)

add_executable(conway main.c
        Headers/InputManager.h
        src/InputManager.c)

find_package(SDL3 REQUIRED CONFIG)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)