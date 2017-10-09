cmake_minimum_required(VERSION 3.8)
project(meshgen)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        main.c
        point.c
        input.c
        triangle.c
        output.c
        output.h
        mesh.c
        mesh.h
        point.h
        triangle.h
        input.h)


add_executable(meshgen ${SOURCE_FILES})
target_link_libraries(meshgen m)
