cmake_minimum_required(VERSION 3.8)
project(Projet_3)

set(CMAKE_C_STANDARD 99)
set(GCC_COVERAGE_COMPILE_FLAGS "--std=c99 --pedantic -Wextra -Wmissing-prototypes -DNDEBUG")

set(SOURCE_FILES
        Code/PNM.c
        Code/PNM.h
        Code/SeamCarving.c
        Code/SeamCarving.h
        Code/Comic.h
        Code/Comic.c Code/mainSeamCarving.c)

add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})

add_executable(Projet_3 ${SOURCE_FILES})