cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(bebarengan-sepur VERSION 1.0)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
SET(CMAKE_CXX_FLAGS "-Wall -O3")
add_compile_options(-fdiagnostics-color=always)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
INCLUDE_DIRECTORIES(websocketpp ${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

# Add source files to create textadventure
set(SRC_FILES_TXTAD
  src/main.cc
  src/objects/object.cc
  src/objects/attack.cc
  src/objects/detail.cc
  src/objects/exit.cc
  src/objects/item.cc
  src/objects/person.cc
  src/objects/player.cc
  src/objects/room.cc
  src/objects/helper/update.cc
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
  src/game/config/attributes.cc
)

# Add source files to create tests
set(SRC_FILES_TESTING
  src/objects/object.cc
  src/objects/attack.cc
  src/objects/detail.cc
  src/objects/exit.cc
  src/objects/item.cc
  src/objects/person.cc
  src/objects/player.cc
  src/objects/room.cc
  src/objects/helper/update.cc
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
  src/game/config/attributes.cc

  src/testing/main.cc
  src/testing/test_startup.cc
  src/testing/test_logic_parser.cc
)


add_executable(txtadventure ${SRC_FILES_TXTAD})
add_executable(testing.o ${SRC_FILES_TESTING})

find_package(laserpants_dotenv)

if (WSS)
  message("compiling websocket-server with ssl support...")
	target_compile_definitions(txtadventure PUBLIC _WSS_)
endif()
if (HTTPS)
  message("compiling http-server with ssl support...")
	target_compile_definitions(txtadventure PUBLIC _HTTPS_)
endif()


target_link_libraries(txtadventure PUBLIC laserpants::dotenv ${CONAN_LIBS})
target_link_libraries(testing.o PUBLIC ${CONAN_LIBS})

target_include_directories(txtadventure PUBLIC
  "src"
)
target_include_directories(testing.o PUBLIC
  "src"
)
