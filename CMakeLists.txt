cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(bebarengan-sepur VERSION 1.0)

find_package(Boost)
enable_testing()

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
SET(CMAKE_CXX_FLAGS "-Wall -Werror -O3")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
INCLUDE_DIRECTORIES(websocketpp ${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
file(GLOB SOURCES_PROGRAMM
    "src/*.h"
    "src/*.cpp"
)

add_executable(txtadventure ${SOURCES_PROGRAMM})

add_executable(txtadventure_server EXCLUDE_FROM_ALL ${SOURCES_PROGRAMM})

target_compile_definitions(txtadventure_server PRIVATE _COMPILE_FOR_SERVER_=1)
add_test(MainTest ../bin/txtadventure -d true)

target_link_libraries(txtadventure PRIVATE boost_system)
target_link_libraries(txtadventure_server
    PRIVATE boost_system)

target_link_libraries(txtadventure
	PRIVATE pthread ssl crypto stdc++fs
)

target_link_libraries(txtadventure_server
	PRIVATE pthread ssl crypto stdc++fs
)
