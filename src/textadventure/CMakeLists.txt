cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(bebarengan-sepur VERSION 1.0)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
SET(CMAKE_CXX_FLAGS "-Wall -Werror -O3")
add_compile_options(-fdiagnostics-color=always)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
INCLUDE_DIRECTORIES(websocketpp ${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

# Add source files to create textadventure
set(SRC_FILES_TXTAD
    src/main.cc
    src/objects/attack.cc 
    src/objects/detail.cc 
    src/objects/exit.cc 
    src/objects/item.cc 
    src/objects/person.cc 
    src/objects/player.cc 
    src/objects/room.cc
    src/actions/fight.cc 
    src/actions/dialog.cc
    src/tools/func.cc 
    src/tools/fuzzy.cc 
    src/tools/logic_parser.cc 
    src/tools/webcmd.cc
    src/concepts/inventory.cc 
    src/concepts/quest.cc 
    src/concepts/text.cc
    src/eventmanager/context.cc 
    src/eventmanager/listener.cc 
    src/eventmanager/parser.cc 
    src/game/game.cc
    src/game/world.cc
)

# Add source files to create tests
set(SRC_FILES_TESTING
    src/objects/attack.cc 
    src/objects/detail.cc 
    src/objects/exit.cc 
    src/objects/item.cc 
    src/objects/person.cc 
    src/objects/player.cc 
    src/objects/room.cc
    src/actions/fight.cc 
    src/actions/dialog.cc
    src/tools/func.cc 
    src/tools/fuzzy.cc 
    src/tools/logic_parser.cc 
    src/tools/webcmd.cc
    src/concepts/inventory.cc 
    src/concepts/quest.cc 
    src/concepts/text.cc
    src/eventmanager/context.cc 
    src/eventmanager/listener.cc 
    src/eventmanager/parser.cc 
    src/game/game.cc
    src/game/world.cc

    src/testing/main.cc
    src/testing/test_startup.cc
)


add_executable(txtadventure ${SRC_FILES_TXTAD})
add_executable(txtadventure_server EXCLUDE_FROM_ALL ${SRC_FILES_TXTAD})
add_executable(testing.o ${SRC_FILES_TESTING})

if (COMPILE_FOR_SERVER)
  message("compiling for server...")
  target_compile_definitions(txtadventure PUBLIC _COMPILE_FOR_SERVER_)
else()
  message("compiling for localhost...")
endif()

target_link_libraries(txtadventure PUBLIC ${CONAN_LIBS})
target_link_libraries(txtadventure_server PUBLIC ${CONAN_LIBS})
target_link_libraries(testing.o PUBLIC ${CONAN_LIBS})

target_include_directories(txtadventure PUBLIC
  "src"
)
target_include_directories(txtadventure_server PUBLIC
  "src"
)
target_include_directories(testing.o PUBLIC
  "src"
)
