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
#include <set>
#include <functional>
#include "sprite_info.hpp"

namespace qutex {

class packer {
public:
    packer(bool trim_transparency);
    sprite_info add_sprite(std::filesystem::path path);
    sprite_info add_sprite(std::filesystem::path path, std::string name);
    int add_sprites(std::filesystem::path input_directory,
        std::string extension = ".png",
        std::function<void(sprite_info const&)> cb = nullptr);
    void pack(int width, int height, std::filesystem::path output_directory,
        int size_multiplier = 0);
private:
    bool m_trim_transparency;
    std::set<sprite_info> m_sprites;
    sprite_info get_sprite_info(std::filesystem::path path);
};

}
