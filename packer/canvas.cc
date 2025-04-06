#include <algorithm>
#include <qutex/canvas.hpp>
#include <cstring>
#include "stbi_buf.hpp"
#include "stb_image.h"
#include "stb_image_write.h"

using namespace qutex;

canvas::canvas(int width, int height): m_width(width), m_height(height),
    m_x(0), m_y(0), m_data(width * height * 4, 0),
    m_bottom_y(width, 0), m_right_x(0) {}

bool canvas::insert_sprite(sprite_info const& sprite, int &out_x, int &out_y) {
    m_y = m_bottom_y[m_x];
    if (sprite.width > m_width - m_x || sprite.height > m_height - m_y) {
        m_x = 0;
        m_y = m_bottom_y[0];
        if (sprite.width > m_width - m_x || sprite.height > m_height - m_y) {
            // no space available
            return false;
        }
    }
    int x, y, comp;
    stbi_buf buf = stbi_load(sprite.path.c_str(), &x, &y, &comp, 4);
    if (buf == nullptr || x != sprite.real_width || y != sprite.real_height) {
        throw std::runtime_error("size mismatch (sprite changed on disk?)");
    }
    for (int i=m_x; i<=m_x+sprite.width && i<(int)m_bottom_y.size(); ++i) {
        m_bottom_y[i] = m_y + sprite.height + 1;
    }
    for (y=0; y<sprite.height; ++y) {
        int dest_offset = (m_y + y) * m_width * 4 + m_x * 4;
        int src_offset = (sprite.offset_y + y) * sprite.real_width * 4 + sprite.offset_x * 4;
        memcpy(&m_data[dest_offset], &buf[src_offset], sprite.width * 4);
    }
    out_x = m_x;
    out_y = m_y;
    m_x += sprite.width + 1;
    if (m_x >= m_width) {
        m_right_x = m_width;
    }
    else {
        m_right_x = std::max(m_x, m_right_x);
    }
    return true;
}

bool canvas::write_png(std::filesystem::path path, int size_multiplier) {
    int width, height;
    if (size_multiplier <= 0) {
        height = m_height;
        width = m_width;
    }
    else {
        height = m_bottom_y[0];
        height += size_multiplier - height % size_multiplier;
        width = m_right_x;
        width += size_multiplier - width % size_multiplier;
    }
    int ok = stbi_write_png(path.c_str(), width, height,
        4, &m_data[0], m_width * 4);
    return ok == 1;
}

void canvas::clear() {
    for (size_t i=0; i<m_data.size(); ++i) {
        m_data[i] = 0;
    }
    for (size_t i=0; i<m_bottom_y.size(); ++i) {
        m_bottom_y[i] = 0;
    }
    m_x = 0;
    m_y = 0;
    m_right_x = 0;
}