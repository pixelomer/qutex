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

#include <qutex/canvas.hpp>
#include <cstring>
#include "stbi_buf.hpp"
#include "stb_image.h"
#include "stb_image_write.h"

using namespace qutex;

canvas::canvas(int width, int height): m_width(width), m_height(height),
    m_min_width(0), m_min_height(0), m_data(width * height * 4, 0) {}

bool canvas::insert_sprite(sprite_info const& sprite, int x, int y) {
    if (x + sprite.width > m_width || y + sprite.height > m_height) {
        // sprite doesn't fit
        return false;
    }
    int comp, sx, sy;
    stbi_buf buf = stbi_load(sprite.path.c_str(), &sx, &sy,
        &comp, 4);
    if (buf == nullptr || sx != sprite.real_width || sy != sprite.real_height) {
        throw std::runtime_error("size mismatch (sprite changed on disk?)");
    }
    for (sy=0; sy<sprite.height; ++sy) {
        int dest_offset = (y + sy) * m_width * 4 + x * 4;
        int src_offset = (sprite.offset_y + sy) * sprite.real_width * 4 + sprite.offset_x * 4;
        memcpy(&m_data[dest_offset], &buf[src_offset], sprite.width * 4);
    }
    m_min_width = std::max(m_min_width, x + sprite.width);
    m_min_height = std::max(m_min_height, y + sprite.height);
    return true;
}

bool canvas::write_png(std::filesystem::path path, int size_multiplier,
    int &width, int &height)
{
    if (size_multiplier >= 1) {
        width = m_min_width;
        height = m_min_height;
        if (width % size_multiplier != 0) {
            width = (width / size_multiplier + 1) * size_multiplier;
        }
        if (height % size_multiplier != 0) {
            height = (height / size_multiplier + 1) * size_multiplier;
        }
    }
    else {
        width = m_width;
        height = m_height;
    }
    int ok = stbi_write_png(path.c_str(), width, height,
        4, &m_data[0], m_width * 4);
    return ok == 1;
}

bool canvas::write_png(std::filesystem::path path) {
    int width, height;
    return write_png(path, 0, width, height);
}

void canvas::clear() {
    for (size_t i=0; i<m_data.size(); ++i) {
        m_data[i] = 0;
    }
    m_min_height = 0;
    m_min_width = 0;
}
