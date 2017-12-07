include(GNUInstallDirs)

project(AMQPSerLib)

add_library(library_iface INTERFACE)

target_compile_definitions(library_iface INTERFACE LIBRARY_HEADER_ONLY)


FILE(GLOB_RECURSE LibFiles  "*.h")

install(FILES ${LibFiles}
        DESTINATION include/${PROJECT_NAME}
        )

# 'make install' to the correct locations (provided by GNUInstallDirs).
install(TARGETS library_iface EXPORT AMQPSerLibConfig)
#install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# This makes the project importable from the install directory
# Put config file in per-project dir (name MUST match), can also
# just go into 'cmake'.
install(EXPORT AMQPSerLibConfig DESTINATION share/AMQPSerLib/cmake)
