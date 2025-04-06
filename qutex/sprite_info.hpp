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

#include <string>
#include <filesystem>

namespace qutex {

struct sprite_info {
    std::string name;
    std::filesystem::path path;
    int real_width;
    int real_height;
    int width;
    int height;
    int offset_x;
    int offset_y;

    sprite_info() {}
    sprite_info(std::filesystem::path path, std::string name,
        bool trim_transparency);
    bool operator>(sprite_info const& rhs) const {
        return height < rhs.height || (height == rhs.height &&
            name > rhs.name);
    }
    bool operator<(sprite_info const& rhs) const {
        return height > rhs.height || (height == rhs.height &&
            name < rhs.name);
    }
};

}
