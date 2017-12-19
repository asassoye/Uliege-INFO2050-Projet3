cmake_minimum_required(VERSION 3.8)
project(Projet_3)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        Code/PNM.c
        Code/PNM.h
        Code/SeamCarving.h
        Code/Comic.h
        Code/mainComic.c)

add_executable(Projet_3 ${SOURCE_FILES})