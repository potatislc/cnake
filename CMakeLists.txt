cmake_minimum_required(VERSION 3.30)
project(cnake)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(
  -Wall
  -Wextra
  -Wpedantic
  -Wconversion
  -Wsign-conversion
  -fsanitize=address
  -g
)

add_link_options(-fsanitize=address)

add_executable(test_number tests/test_number.cpp src/cnake.cpp)
add_executable(test_var tests/test_var.cpp src/cnake.cpp)