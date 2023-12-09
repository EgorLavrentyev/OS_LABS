project(wolf-and-goats)
cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Werror)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include_directories(host client conn utils)
FILE(GLOB HOST host/*.cpp)

add_executable(${PROJECT_NAME}-pipe ${HOST} conn/conn_pipe.cpp)
target_link_libraries(${PROJECT_NAME}-pipe Threads::Threads rt)

add_executable(${PROJECT_NAME}-sock ${HOST} conn/conn_sock.cpp)
target_link_libraries(${PROJECT_NAME}-sock Threads::Threads rt)

add_executable(${PROJECT_NAME}-fifo ${HOST} conn/conn_fifo.cpp)
target_link_libraries(${PROJECT_NAME}-fifo Threads::Threads rt)