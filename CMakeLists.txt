cmake_minimum_required(VERSION 3.9)

project(meshgen
	VERSION 0.1
	DESCRIPTION "Terrain Mesh Generator"
	LANGUAGES C
	)

list(INSERT CMAKE_MODULE_PATH 0
	"${CMAKE_CURRENT_LIST_DIR}/cmake")

set(CMAKE_EXPORT_COMPILE_COMMANDS 1 )

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

include(cmake/compiler_flags.cmake)
include(cmake/ctags.cmake)
include(cmake/manage_dependencies.cmake)

find_library(MATH_LIB m)

add_library(meshgenlib
	Src/point.c
	Src/input.c
	Src/asc_reader.c
	Src/triangle.c
	Src/output.c
	Src/mesh.c
	Src/refinement.c
	Src/utils.c
	)


target_compile_options(meshgenlib PRIVATE
	${MY_CUSTOM_C_FLAGS}
	)

target_link_libraries(meshgenlib PUBLIC
	${MATH_LIB}
	)

target_include_directories(meshgenlib INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/Src
	)

add_executable(meshgen
	Src/main.c)

target_link_libraries(meshgen PRIVATE
	meshgenlib)

target_compile_options(meshgen PRIVATE
	${MY_CUSTOM_C_FLAGS})


add_executable(Test EXCLUDE_FROM_ALL
	Tests/main.c)

target_compile_options(Test PUBLIC
	${MY_CUSTOM_C_FLAGS})

target_link_libraries(Test 
	PkgConfig::Check meshgenlib)

include(CheckIPOSupported)
check_ipo_supported(RESULT result)
if(result)
	set_target_properties(meshgenlib PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
	set_target_properties(meshgen PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
endif()
