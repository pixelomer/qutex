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
