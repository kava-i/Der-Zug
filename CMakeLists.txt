cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(bebarengan-sepur VERSION 1.0)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

enable_testing()

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
SET(CMAKE_CXX_FLAGS "-Wall -Werror -O3")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
INCLUDE_DIRECTORIES(websocketpp ${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
file(GLOB SOURCES_PROGRAMM
    "src/textadventure/*.h"
    "src/textadventure/*.cc"
    "src/textadventure/objects/*.cc"
    "src/textadventure/actions/*.cc"
    "src/textadventure/tools/*.cc"
    "src/textadventure/concepts/*.cc"
    "src/textadventure/eventmanager/*.cc"
    "src/textadventure/game/*.cc"
)



add_executable(txtadventure ${SOURCES_PROGRAMM})
add_executable(txtadventure_server EXCLUDE_FROM_ALL ${SOURCES_PROGRAMM})
target_link_libraries(txtadventure PUBLIC ${CONAN_LIBS})
target_link_libraries(txtadventure_server PUBLIC ${CONAN_LIBS})

target_include_directories(txtadventure PUBLIC
  "src/textadventure"
)
target_include_directories(txtadventure_server PUBLIC
  "src/textadventure"
)

add_test(MainTest ../bin/txtadventure -d true)


