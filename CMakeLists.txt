cmake_minimum_required(VERSION 3.8)
project(Projet_3)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES
        Code/PNM.c
        Code/PNM.h
        Code/SeamCarving.h
        Code/Comic.h
        Code/mainComic.c Code/Comic.c)

add_executable(Projet_3 ${SOURCE_FILES})