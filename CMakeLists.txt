
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
cmake_minimum_required(VERSION 3.10)
project(WebServer)
set(CMAKE_CXX_STANDARD 20)
aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC)
include_directories(${PROJECT_SOURCE_DIR}/include)
add_executable(server ${SRC})
target_compile_options(server PRIVATE -Wall
-Wextra
-Wpedantic
-Wconversion
-Wsign-conversion
)