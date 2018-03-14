find_program(CTAGS_PATH ctags)
if(CTAGS_PATH)
        message(STATUS "Found ctags: ${CTAGS_PATH}")
        set_source_files_properties(tags PROPERTIES GENERATED true)
        add_custom_target(tags
                COMMAND ${CTAGS_PATH} -R ${CMAKE_SOURCE_DIR}
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

endif(CTAGS_PATH)
