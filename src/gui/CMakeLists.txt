cmake_minimum_required(VERSION 2.8)

project(game_builder.o)

# Conan setup
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)  #Includes stuff added by conan
conan_basic_setup()  #links include directories and libaries

# Specify C++ standards and complitation flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall -Werror -O3")
add_compile_options(-fdiagnostics-color=always)

# Add source-files for server
set(SRC_FILES_SERVER
  src/main.cc 
  src/server/server_frame.cc 
  src/users/user_manager.cc 
  src/users/user.cc 
  src/world/worlds.cc
  src/world/world.cc
  src/page/category.cc
  src/page/page.cc
  src/page/sub_category.cc
  src/page/area.cc
  src/util/func.cc
)

# Add source-files for testing
set(SRC_FILES_TESTING
  src/server/server_frame.cc
  src/server/test_server.cc
  src/users/user_manager.cc
  src/users/test_user_manager.cc
  src/users/user.cc
  src/users/test_user.cc
  src/world/worlds.cc
  src/world/world.cc
  src/page/page.cc
  src/page/category.cc
  src/page/sub_category.cc
  src/page/area.cc
  src/util/func.cc
  src/util/test_func.cc
)

# Specify location for executables 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/webserver)

# Create executables.
add_executable(game_builder.o ${SRC_FILES_SERVER})
add_executable(builder_tests.o ${SRC_FILES_TESTING})

# Add directories to be included easily
target_include_directories(game_builder.o PUBLIC "src")
target_include_directories(builder_tests.o PUBLIC "src")

# Link conan imports.
target_link_libraries(game_builder.o ${CONAN_LIBS})
target_link_libraries(builder_tests.o ${CONAN_LIBS})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
