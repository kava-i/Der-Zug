cmake_minimum_required(VERSION 2.8)

project(game_builder.o)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)  #Includes stuff added by conan
conan_basic_setup()  #links include directories and libaries

set(CMAKE_CXX_STANDARD 17)
set(SRC_FILES src/server/main.cc src/server/server_frame.cc src/users/user_manager.cc src/users/user.cc src/util/func.cc)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/webserver)

add_executable(game_builder.o ${SRC_FILES})

target_include_directories(game_builder.o PUBLIC "src")

target_link_libraries(game_builder.o ${CONAN_LIBS})
