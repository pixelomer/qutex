#pragma once

#include <filesystem>
#include <fstream>
#include "sprite_info.hpp"

namespace qutex {

class meta_reader {
public:
    meta_reader(std::filesystem::path path);
    bool read_next(int &x, int &y, sprite_info &info);
    void close();
private:
    meta_reader &operator>>(int &x);
    meta_reader &operator>>(std::string &str);
    bool read_header();
    std::ifstream m_stream;
};

}