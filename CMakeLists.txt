cmake_minimum_required(VERSION 3.8)

project(meshgen
	VERSION 0.1
	DESCRIPTION "Terrain Mesh Generator"
	LANGUAGES C
	)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

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

find_program(CTAGS_PATH ctags)
if(CTAGS_PATH)
        message(STATUS "Found ctags: ${CTAGS_PATH}")
        set_source_files_properties(tags PROPERTIES GENERATED true)
        add_custom_target(tags
                COMMAND ${CTAGS_PATH} -R ${CMAKE_SOURCE_DIR}
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

endif(CTAGS_PATH)

