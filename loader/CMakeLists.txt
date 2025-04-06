cmake_minimum_required(VERSION 3.14)
project(qutex-loader)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(..)

add_library(
    qutex-loader
    reader.cc
    meta_reader.cc
)

target_compile_options(qutex-loader PRIVATE
    -Wall -Wextra -Wpedantic -Werror
)
