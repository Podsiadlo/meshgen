set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -W -Wall -Wcheck -Wdeprecated -Wformat-security -Wreorder -Wshadow -prec-div -prec-sqrt -fp-model precise")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -xHost")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fp-stack-check -fstack-security-check -traceback -fp-model except -fp-stack-check -fstack-security-check -g -debug full -O0")
