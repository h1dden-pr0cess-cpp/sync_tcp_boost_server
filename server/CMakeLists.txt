cmake_minimum_required(VERSION 3.16)

project(sync_server
    VERSION 1.0
    LANGUAGES C CXX
)

# ===== C++ standard =====
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ===== Build type =====
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# ===== SQLite (amalgamation) =====
add_library(sqlite3 STATIC
    third_party/sqlite/sqlite3.c
)

target_include_directories(sqlite3 PUBLIC
    third_party/sqlite
)

# ===== OpenSSL =====

find_package(OpenSSL REQUIRED)

# ===== Server executable =====
add_executable(sync_server
    src/main.cpp
    src/Server.cpp
    src/Session.cpp
    src/Database.cpp
)

target_include_directories(sync_server PRIVATE
    src
)

target_link_libraries(sync_server PRIVATE
    sqlite3
)

target_link_libraries(sync_server PRIVATE OpenSSL::SSL OpenSSL::Crypto)

# ===== Warnings =====
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(sync_server PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()
