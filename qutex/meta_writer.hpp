#pragma once

#include <filesystem>
#include <fstream>
#include <cstdint>
#include "sprite_info.hpp"

namespace qutex {

class meta_writer {
public:
    meta_writer();
    void open(std::filesystem::path path);
    void add_sprite(int x, int y, sprite_info const& info);
    bool is_open();
    void finalize(int width, int height);
    ~meta_writer();
private:
    meta_writer &operator<<(uint16_t x);
    meta_writer &operator<<(std::string const& str);
    void write_header();
    std::ofstream m_stream;
};

}