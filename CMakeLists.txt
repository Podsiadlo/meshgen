cmake_minimum_required(VERSION 3.9)

project(meshgen
	VERSION 0.1
	DESCRIPTION "Terrain Mesh Generator"
	LANGUAGES C
	)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1 )

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

include(cmake/compiler_flags.cmake)
include(cmake/ctags.cmake)

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

include(CheckIPOSupported)
check_ipo_supported(RESULT result)
if(${result})
    set_target_properties(meshgen PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
endif()


