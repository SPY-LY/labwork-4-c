cmake_minimum_required(VERSION 3.23)
project(test_lab4)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(console_parser_lib)
add_subdirectory(ham_arc_lib)

add_executable(test_lab4 main.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC console_parser)
target_link_libraries(${PROJECT_NAME} PUBLIC ham_arc)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})

