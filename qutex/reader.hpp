#pragma once

#include <filesystem>
#include <functional>
#include "sprite_info.hpp"

namespace qutex {

class reader {
public:
    reader(std::filesystem::path path);
    void read_all_sprites(std::function<void(std::filesystem::path path)> texture_cb,
        std::function<void(int x, int y, sprite_info const& sprite)> sprite_cb);
private:
    std::filesystem::path m_root;
    std::vector<std::string> m_texture_names;
};

}