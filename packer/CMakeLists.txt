cmake_minimum_required(VERSION 3.14)
project(qutex-packer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(..)

add_library(
    qutex-packer
    canvas.cc
    meta_writer.cc
    packer.cc
    sprite_info.cc
    stb_image_write.cc
    stb_image.cc
    stb_rect_pack.cc
)

target_compile_options(qutex-packer PRIVATE
    -Wall -Wextra -Wpedantic -Werror -Wno-missing-field-initializers
)
