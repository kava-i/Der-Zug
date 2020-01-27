cmake_minimum_required(VERSION 3.10)


# set the project name and version
project(bebarengan-sepur VERSION 1.0)
find_package(Boost)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
SET(CMAKE_CXX_FLAGS "-Wall -Werror -O3")
set(EXECUTABLE_OUTPUT_PATH ../bin)
INCLUDE_DIRECTORIES(websocketpp ${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
set(SOURCES_PROGRAMM main.cpp Attack.cpp Character.cpp Detail.cpp Dialog.cpp Fight.cpp Game.cpp Item.cpp Person.cpp Player.cpp Room.cpp World.cpp func.cpp fuzzy.cpp Context.cpp WorldContext.cpp StandardContext.cpp FightContext.cpp DialogContext.cpp ChoiceContext.cpp SortedContext.cpp ChatContext.cpp GameContext.cpp)

add_executable(txtadventure ${SOURCES_PROGRAMM})

add_executable(unittest ${SOURCES_PROGRAMM})
add_executable(txtadventure_server ${SOURCES_PROGRAMM})

target_compile_definitions(txtadventure_server PRIVATE _COMPILE_FOR_SERVER_=1)

target_link_libraries(unittest PRIVATE boost_system)
target_link_libraries(txtadventure PRIVATE boost_system)
target_link_libraries(txtadventure_server
    PRIVATE boost_system)


target_compile_definitions(unittest PRIVATE _COMPILE_UNIT_TEST_=1)
target_link_libraries(unittest PRIVATE pthread ssl crypto stdc++fs)

target_link_libraries(txtadventure
	PRIVATE pthread ssl crypto stdc++fs
)

target_link_libraries(txtadventure_server
	PRIVATE pthread ssl crypto stdc++fs
)
