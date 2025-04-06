#pragma once

#include <filesystem>
#include <set>
#include <functional>
#include "sprite_info.hpp"

namespace qutex {

class packer {
public:
    packer(bool trim_transparency);
    sprite_info add_sprite(std::filesystem::path path);
    sprite_info add_sprite(std::filesystem::path path, std::string name);
    int add_sprites(std::filesystem::path input_directory,
        std::string extension = ".png",
        std::function<void(sprite_info const&)> cb = nullptr);
    void pack(int width, int height, std::filesystem::path output_directory,
        int size_multiplier = 0);
private:
    bool m_trim_transparency;
    std::set<sprite_info> m_sprites;
    sprite_info get_sprite_info(std::filesystem::path path);
};

}
