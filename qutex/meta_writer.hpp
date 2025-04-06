// 
// qutex - Quick and dirty texture packer
// Copyright (C) 2025 pixelomer
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
// 

#pragma once

#include <filesystem>
#include <fstream>
#include <cstdint>
#include "sprite_info.hpp"

namespace qutex {

class meta_writer {
public:
    meta_writer();
    void open(std::filesystem::path path);
    void add_sprite(int x, int y, sprite_info const& info);
    bool is_open();
    void finalize(int width, int height);
    ~meta_writer();
private:
    meta_writer &operator<<(uint16_t x);
    meta_writer &operator<<(std::string const& str);
    void write_header();
    std::ofstream m_stream;
};

}