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

#include <cstdint>
#include <cstring>
#include <qutex/meta_reader.hpp>
#include <stdexcept>
#include <vector>
#include <fstream>

using namespace qutex;

meta_reader::meta_reader(std::filesystem::path path, int &width, int &height) {
    m_stream.open(path, std::ios::binary);
    if (m_stream.fail()) {
        throw std::runtime_error("failed to open meta reader at " + path.string());
    }
    if (!read_header(width, height)) {
        throw std::runtime_error("invalid meta file header");
    }
}

bool meta_reader::read_header(int &width, int &height) {
    static const char header[] = { 'Q', 'U', 'T', 'E', 'X', 0, 0, 0 };
    char buf[sizeof(header)];
    m_stream.read(buf, sizeof(buf));
    *this >> width >> height;
    return m_stream.good() && memcmp(buf, header, sizeof(buf)) == 0;
}

meta_reader &meta_reader::operator>>(int &x) {
    uint8_t buf[2];
    m_stream.read((char *)buf, 2);
    if (m_stream.good()) {
        x = (int)(((uint16_t)buf[0] << 8) | (uint16_t)buf[1]);
    }
    return *this;
}

meta_reader &meta_reader::operator>>(std::string &str) {
    int len;
    *this >> len;
    if (m_stream.good()) {
        std::vector<char> buf(len, 0);
        m_stream.read(&buf[0], len);
        if (m_stream.good()) {
            str = { &buf[0], (size_t)len };
        }
    }
    return *this;
}

bool meta_reader::read_next(int &x, int &y, sprite_info &info) {
    *this >> info.real_width >> info.real_height >> info.width >>
        info.height >> info.offset_x >> x >> y >> info.offset_y >> info.name;
    return m_stream.good();
}

void meta_reader::close() {
    m_stream.close();
}
