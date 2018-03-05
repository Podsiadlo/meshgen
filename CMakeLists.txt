cmake_minimum_required(VERSION 3.8)
project(meshgen)

set(CMAKE_CXX_STANDARD 11)

add_executable(meshgen
	Src/main.c
	Src/point.c
	Src/input.c
	Src/triangle.c
	Src/output.c
	Src/output.h
	Src/mesh.c
	Src/mesh.h
	Src/point.h
	Src/triangle.h
	Src/input.h
	Src/refinement.c
	Src/refinement.h)


target_link_libraries(meshgen m)
