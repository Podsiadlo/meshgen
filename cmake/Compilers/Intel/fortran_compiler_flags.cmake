set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -standard-semantics -gen_interfaces -warn all,nodec,interfaces -prec-div -prec-sqrt -fp-model precise ")
set(CMAKE_Fortran_FLAGS_RELEASE "${CMAKE_Fortran_FLAGS_RELEASE} -xHost -O3")
set(CMAKE_Fortran_FLAGS_DEBUG "${CMAKE_Fortran_FLAGS_DEBUG} -check all -check bounds -check uninit -traceback -fpe0 -fp-model except -fp-stack-check -fstack-security-check -g -debug full -O0")
