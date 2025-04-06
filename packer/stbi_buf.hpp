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

#include <cstdint>
#include "stb_image.h"

// raii container for buffers returned from stb_image
class stbi_buf {
public:
    stbi_buf(stbi_buf const&) = delete;
    stbi_buf(stbi_buf&&) = delete;
    stbi_buf(stbi_uc *buf = nullptr): buf(buf) {}
    uint8_t &operator[](int idx) {
        return buf[idx];
    }
    bool operator==(uint8_t *rhs) {
        return buf == rhs;
    }
    ~stbi_buf() {
        if (buf != nullptr) {
            stbi_image_free(buf);
        }
    }
private:
    uint8_t *buf;
};
