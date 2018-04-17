check_and_add_c_flags("-Weverything")
check_and_add_c_flags("-pedantic-errors")

if(CMAKE_BUILD_TYPE STREQUAL "Release")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
endif()
