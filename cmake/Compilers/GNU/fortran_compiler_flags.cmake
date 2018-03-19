set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -std=f2008 -fall-intrinsics -W -Wall -Wimplicit-interface -pedantic-errors -frealloc-lhs -Wuse-without-only")
set(CMAKE_Fortran_FLAGS_RELEASE "${CMAKE_Fortran_FLAGS_RELEASE} -O3 -march=native -funroll-loops")
set(CMAKE_Fortran_FLAGS_DEBUG "${CMAKE_Fortran_FLAGS_DEBUG} -Og -fcheck=all -Wunderflow -fbacktrace -fbounds-check -g -finit-real=nan")

