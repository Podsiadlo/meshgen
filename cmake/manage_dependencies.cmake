#Git is required since we are downloading sources from the net using git
find_package(Git REQUIRED)

INCLUDE(ExternalProject)
## Set default ExternalProject root directory
SET_DIRECTORY_PROPERTIES(PROPERTIES EP_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/Deps/CmakeAux)

find_package(PkgConfig)

pkg_check_modules(Check QUIET IMPORTED_TARGET check)

if (NOT check_FOUND)
	ExternalProject_Add(
		down_check
		GIT_REPOSITORY https://github.com/libcheck/check.git
		GIT_TAG "0.12.0"
		GIT_SHALLOW TRUE
		TIMEOUT 10
		SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Deps/Check
		INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Deps/Install
		#No need to compile or install since it's header only
		UPDATE_COMMAND ""
		#CONFIGURE_COMMAND ""
		#BUILD_COMMAND ""
		#INSTALL_COMMAND ""
		CMAKE_ARGS
		-DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
		)

	set_target_properties(down_check PROPERTIES EXCLUDE_FROM_ALL TRUE)

	ExternalProject_Get_Property(down_check install_dir)
	set(CHECK_INCLUDE_DIRS ${install_dir}/include CACHE INTERNAL "Path to include folder for Eigen")
	set(CHECK_LIBRARIES ${install_dir}/lib/libcheck.a CACHE INTERNAL "Path to include folder for Eigen")



	add_library(PkgConfig::Check STATIC IMPORTED)

	add_dependencies(PkgConfig::Check down_check)


	file(MAKE_DIRECTORY ${CHECK_INCLUDE_DIRS})

	set_target_properties(PkgConfig::Check PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES ${CHECK_INCLUDE_DIRS})

	set_target_properties(PkgConfig::Check PROPERTIES
		IMPORTED_LOCATION
		${CHECK_LIBRARIES}
		)

	find_library(MATH_LIB m)
	find_library(RT_LIB rt)
	set_target_properties(PkgConfig::Check PROPERTIES
		INTERFACE_LINK_LIBRARIES "${RT_LIB};${MATH_LIB}"
		)

endif()
