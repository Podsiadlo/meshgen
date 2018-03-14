#Iterate over the three languages
foreach(LANGUAGE
		C
		CXX
		Fortran
		)

	#I need the lower case version of languages to call the files (I prefer a low case filename)
	string(TOLOWER ${LANGUAGE} language)

	#Now I iterate over the three compilers I use
	foreach(COMPILER
			GNU
			Intel
			NAG)

		#Check and load the corresponding flags
		if("${CMAKE_${LANGUAGE}_COMPILER_ID}" STREQUAL ${COMPILER})
			include(cmake/Compilers/${COMPILER}/${language}_compiler_flags.cmake)
		endif()

	endforeach(COMPILER)
endforeach(LANGUAGE)
