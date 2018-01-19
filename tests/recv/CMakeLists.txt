project(recv)

set(SOURCES recv) #unitCatch.cpp service_bus selected_recv recv filter_subject client

add_executable(${PROJECT_NAME} ${SOURCES})

compileArgs (${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} ${ALL_LIBS}  )


#message("CXX_STANDARD: ${CXX_STANDARD}")
#get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)

#foreach(dir ${dirs})
#  message(STATUS "dir='${dir}'")
#endforeach()


#get_target_property(link_lib ${PROJECT_NAME} LINK_LIBRARIES)
#message("link_lib ${link_lib}")

#get_property(varCompiler VARIABLE PROPERTY CMAKE_CXX_COMPILER)
#message("varCompiler ${varCompiler}")
