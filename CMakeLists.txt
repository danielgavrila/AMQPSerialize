cmake_minimum_required(VERSION 3.10)

project(AMQPSerialize)

set(CMAKE_VERBOSE_MAKEFILE TRUE)
#needed packages from Proton,which were build with CMake
find_package(Proton CONFIG REQUIRED)
find_package(ProtonCpp CONFIG REQUIRED)
find_package(Qpid CONFIG REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include
                    ${PROJECT_SOURCE_DIR}/testUnit
                    ${PROJECT_SOURCE_DIR}/magic_get-master/include
                    ${Proton_INCLUDE_DIRS}
                    ${ProtonCpp_INCLUDE_DIRS}
                    ${Qpid_INCLUDE_DIRS}
                    )


#all external library necessary
set (ALL_LIBS
    stdc++
    ${Qpid_LIBRARIES}
    ${Proton_LIBRARIES}
    ${Proton_Core_LIBRARIES}
    ${ProtonCpp_LIBRARIES}
    )

include(./cmake/util.cmake)
add_subdirectory(./include/AMQPSerialize)
add_subdirectory(./tests/test1)
add_subdirectory(./tests/test2)










