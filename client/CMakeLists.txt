cmake_minimum_required(VERSION 3.16)

project(client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(Boost REQUIRED)

add_executable(client
	ClientChunk.cpp
)

target_include_directories(client
    PRIVATE ${Boost_INCLUDE_DIRS}
)

