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

#include <vector>
#include <filesystem>
#include "sprite_info.hpp"

namespace qutex {

class canvas {
public:
    canvas(int width, int height);
    canvas(canvas const&);
    bool insert_sprite(sprite_info const& sprite, int x, int y);
    bool write_png(std::filesystem::path path, int size_multiplier,
        int &width, int &height);
    bool write_png(std::filesystem::path path);
    void clear();
private:
    int m_width;
    int m_height;
    int m_min_width;
    int m_min_height;
    std::vector<uint8_t> m_data;
};

}