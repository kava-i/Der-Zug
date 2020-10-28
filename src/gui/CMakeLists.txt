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
set(SRC_FILES 
  src/server/main.cc 
  src/server/server_frame.cc 
  src/users/user_manager.cc 
  src/users/user.cc 
  src/util/func.cc
)

# Specify location for executables 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/webserver)

# Create executables.
add_executable(game_builder.o ${SRC_FILES})

# Add directories to be included easaly
target_include_directories(game_builder.o PUBLIC "src")

# Link conan imports.
target_link_libraries(game_builder.o ${CONAN_LIBS})
