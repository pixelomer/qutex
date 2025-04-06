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
#include <functional>
#include "sprite_info.hpp"

namespace qutex {

class reader {
public:
    reader(std::filesystem::path path);
    void read_all_sprites(std::function<void(std::filesystem::path path,
        int width, int height)> texture_cb,
        std::function<void(int x, int y, sprite_info const& sprite)> sprite_cb);
private:
    std::filesystem::path m_root;
    std::vector<std::string> m_texture_names;
};

}