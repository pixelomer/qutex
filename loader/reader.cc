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

#include <qutex/reader.hpp>
#include <qutex/meta_reader.hpp>
#include <qutex/sprite_info.hpp>
#include <stdexcept>
#include <string>
#include <vector>
#include <fstream>

using namespace qutex;

reader::reader(std::filesystem::path path): m_root(path) {
    std::ifstream stream;
    stream.open(path / "qutex.txt");
    if (!stream.good()) {
        throw std::runtime_error("failed to open qutex.txt");
    }
    std::string name;
    while (std::getline(stream, name)) {
        m_texture_names.push_back(name);
    }
}

void reader::read_all_sprites(
    std::function<void(std::filesystem::path path, int width, int height)> texture_cb,
    std::function<void(int x, int y, sprite_info const& sprite)> sprite_cb)
{
    for (auto const& name : m_texture_names) {
        int width, height;
        meta_reader meta { m_root / (name + ".meta"), width, height };
        if (texture_cb != nullptr) {
            texture_cb(m_root / name, width, height);
        }
        int x, y;
        sprite_info info;
        while (meta.read_next(x, y, info)) {
            sprite_cb(x, y, info);
        }
        meta.close();
    }
}
