#pragma once

#include <string>
#include <filesystem>

namespace qutex {

struct sprite_info {
    std::string name;
    std::filesystem::path path;
    int real_width;
    int real_height;
    int width;
    int height;
    int offset_x;
    int offset_y;

    sprite_info() {}
    sprite_info(std::filesystem::path path, std::string name,
        bool trim_transparency);
    bool operator>(sprite_info const& rhs) const {
        return height < rhs.height || (height == rhs.height &&
            name > rhs.name);
    }
    bool operator<(sprite_info const& rhs) const {
        return height > rhs.height || (height == rhs.height &&
            name < rhs.name);
    }
};

}
