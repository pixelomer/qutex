#pragma once

#include <vector>
#include <filesystem>
#include "sprite_info.hpp"

namespace qutex {

class canvas {
public:
    canvas(int width, int height);
    canvas(canvas const&);
    bool insert_sprite(sprite_info const& sprite, int &x, int &y);
    bool write_png(std::filesystem::path path, int size_multiplier = 0);
    bool write_png(std::filesystem::path path, int size_multiplier,
        int &wrote_width, int &wrote_height);
    void clear();
private:
    int m_width;
    int m_height;
    int m_x;
    int m_y;
    std::vector<uint8_t> m_data;
    std::vector<int> m_bottom_y;
    int m_right_x;
};

}