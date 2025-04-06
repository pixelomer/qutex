cmake_minimum_required(VERSION 3.14)
project(qutex)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
find_package(SDL3_image REQUIRED)

add_subdirectory(packer)
add_subdirectory(loader)
include_directories(.)

add_executable(qutex
    main.cc
)

target_compile_options(qutex PRIVATE
    -Wall -Wextra -Wpedantic -Werror
)

add_dependencies(qutex qutex-packer qutex-loader)
target_link_libraries(qutex
    qutex-packer qutex-loader SDL3_image::SDL3_image SDL3::SDL3)

