project(test2)

set(SOURCES main.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})

compileArgs (${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} ${ALL_LIBS})

