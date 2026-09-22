
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

enable_testing()
add_executable(http_test tests/http_test.cpp src/http.cpp)

target_include_directories(http_test PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_compile_options(http_test PRIVATE -Wall
-Wextra
-Wpedantic
-Wconversion
-Wsign-conversion
)

add_test(NAME http_test COMMAND http_test)

find_package(Threads REQUIRED)
target_link_libraries(server PRIVATE Threads::Threads)

add_executable(thread_pool_test
    tests/thread_pool_test.cpp
)

target_include_directories(thread_pool_test PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(thread_pool_test PRIVATE
    Threads::Threads
)

target_compile_options(thread_pool_test PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wsign-conversion
)

add_test(NAME thread_pool_test COMMAND thread_pool_test)

add_executable(notify_test
    tests/notify_test.cpp
)

target_include_directories(notify_test PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(notify_test PRIVATE
    Threads::Threads
)

target_compile_options(notify_test PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wsign-conversion
)

add_test(NAME notify_test COMMAND notify_test)
set_tests_properties(notify_test PROPERTIES TIMEOUT 10)

add_executable(static_file_test tests/static_file_test.cpp src/static_file.cpp)

target_include_directories(static_file_test PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_compile_options(static_file_test PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wsign-conversion
)

add_test(NAME static_file_test COMMAND static_file_test)

add_executable(timer_test tests/timer_test.cpp)

target_compile_options(timer_test PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wsign-conversion
)

add_test(NAME timer_test COMMAND timer_test)
set_tests_properties(timer_test PROPERTIES TIMEOUT 10)