#include "qutex/meta_writer.hpp"
#include <filesystem>
#include <stdexcept>
#include <iostream>
#include <string>
#include <fstream>
#include <qutex/packer.hpp>
#include <qutex/canvas.hpp>
#include <qutex/sprite_info.hpp>

using namespace qutex;

packer::packer(bool trim_transparency):
    m_trim_transparency(trim_transparency) {}

sprite_info packer::add_sprite(std::filesystem::path path) {
    return add_sprite(path, path.stem());
}

sprite_info packer::add_sprite(std::filesystem::path path, std::string name) {
    sprite_info info { path, name, m_trim_transparency };
    if (m_sprites.count(info)) {
        std::cerr << "warning: not inserting duplicate sprite" << std::endl;
    }
    else {
        m_sprites.insert(info);
    }
    return info;
}

int packer::add_sprites(std::filesystem::path input_directory,
    std::string extension, std::function<void(sprite_info const&)> cb)
{
    std::filesystem::directory_iterator iter { input_directory };
    int count = 0;
    for (auto &entry : iter) {
        if (entry.is_regular_file() && entry.path().extension() == extension) {
            auto sprite = add_sprite(entry.path());
            ++count;
            if (cb != nullptr) {
                cb(sprite);
            }
        }
    }
    return count;
}

void packer::pack(int width, int height, std::filesystem::path output_directory,
    int size_multiplier)
{
    if (std::filesystem::exists(output_directory) &&
        !std::filesystem::remove(output_directory))
    {
        throw std::runtime_error("output directory already exists");
    }
    if (!std::filesystem::create_directory(output_directory)) {
        throw std::runtime_error("failed to create output directory");
    }
    auto png_name = [](int sequence){
        auto name = std::to_string(sequence);
        while (name.size() < 4) name = "0" + name;
        name = "tex" + name + ".png";
        return name;
    };
    std::ofstream texture_list;
    texture_list.open(output_directory / "qutex.txt");
    meta_writer meta;
    canvas canvas { width, height };
    int x, y, tex_num = 1;
    auto flush_textures = [&](){
        auto name = png_name(tex_num++);
        canvas.write_png(output_directory / name,
            size_multiplier);
        texture_list << name << std::endl;
        canvas.clear();
        meta.close();
    };
    for (auto &sprite : m_sprites) {
        bool did_insert = canvas.insert_sprite(sprite, x, y);
        if (!did_insert) {
            flush_textures();
            did_insert = canvas.insert_sprite(sprite, x, y);
            if (!did_insert) {
                throw std::runtime_error("sprite too big for texture");
            }
        }
        if (!meta.is_open()) {
            meta = meta_writer { output_directory / (png_name(tex_num) + ".meta") };
        }
        meta.add_sprite(x, y, sprite);
    }
    flush_textures();
    texture_list.close();
}
