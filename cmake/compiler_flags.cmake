include(myCompilerFlagsCheck)
foreach(COMPILER
		GNU
		Clang
		)

	#Check and load the corresponding flags
	if("${CMAKE_C_COMPILER_ID}" STREQUAL ${COMPILER})
		include(cmake/Compilers/${COMPILER}/c_compiler_flags.cmake)
	endif()

endforeach(COMPILER)
