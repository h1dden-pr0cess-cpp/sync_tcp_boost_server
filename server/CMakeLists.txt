cmake_minimum_required(VERSION 3.16)
project(sync_server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(sync_server
    main.cpp
	Server.cpp
	Session.cpp
)

target_include_directories(sync_server PRIVATE
    /usr/include
)

