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

#include "qutex/meta_writer.hpp"
#include <filesystem>
#include <stdexcept>
#include <iostream>
#include <string>
#include <fstream>
#include <qutex/packer.hpp>
#include <qutex/canvas.hpp>
#include <qutex/sprite_info.hpp>
#include "stb_rect_pack.h"
#include <algorithm>

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
    int size_multiplier, bool verbose)
{
    if (std::filesystem::exists(output_directory) &&
        !std::filesystem::remove(output_directory))
    {
        throw std::runtime_error("output directory already exists");
    }
    if (!std::filesystem::create_directory(output_directory)) {
        throw std::runtime_error("failed to create output directory");
    }
    if (m_sprites.size() == 0) {
        throw std::runtime_error("packer::pack() called with no sprites");
    }

    static const auto png_name = [](int sequence){
        auto name = std::to_string(sequence);
        while (name.size() < 4) name = "0" + name;
        name = "tex" + name + ".png";
        return name;
    };

    std::ofstream texture_list;
    texture_list.open(output_directory / "qutex.txt");

    std::vector<sprite_info> sprites(m_sprites.begin(), m_sprites.end());
    std::vector<stbrp_rect> rects(sprites.size());
    for (size_t i=0; i<sprites.size(); ++i) {
        rects[i] = { (int)i, sprites[i].width, sprites[i].height,
            0, 0, 0 };
    }

    stbrp_context ctx;
    canvas canvas { width, height };
    meta_writer meta;
    std::vector<stbrp_node> nodes(width);
    int tex_num = 1;
    int overall_final_pixels = 0, overall_trimmed_pixels = 0,
        overall_real_pixels = 0;
    while (rects.size() > 0) {
        stbrp_init_target(&ctx, width, height, &nodes[0], nodes.size());
        stbrp_pack_rects(&ctx, &rects[0], rects.size());
        std::sort(rects.begin(), rects.end(),
            [](stbrp_rect &a, stbrp_rect &b){ return a.was_packed < b.was_packed; });
        auto name = png_name(tex_num++);
        meta.open(output_directory / (name + ".meta"));
        texture_list << name << std::endl;
        canvas.clear();
        int i;
        int trimmed_pixels = 0, real_pixels = 0;
        for (i=rects.size()-1; i>=0 && rects[i].was_packed; --i) {
            auto &rect = rects[i];
            auto &sprite = sprites[rect.id];
            canvas.insert_sprite(sprite, rect.x, rect.y);
            meta.add_sprite(rect.x, rect.y, sprite);
            trimmed_pixels += sprite.width * sprite.height;
            real_pixels += sprite.real_height * sprite.real_width;
        }
        rects.resize(i+1);
        int final_width, final_height, final_pixels;
        canvas.write_png(output_directory / name, size_multiplier,
            final_width, final_height);
        meta.finalize(final_width, final_height);
        final_pixels = final_width * final_height;
        if (verbose) {
            auto trimmed_save = (trimmed_pixels - final_pixels) / (double)trimmed_pixels;
            auto real_save = (real_pixels - final_pixels) / (double)real_pixels;
            std::cout << "wrote " << name << std::endl;
            std::cout << "  trimmed:  " << final_pixels << " vs " << trimmed_pixels
                << " (" << (trimmed_save * 100) << "% saved)" << std::endl;
            std::cout << "  original: " << final_pixels << " vs " << real_pixels
                << " (" << (real_save * 100) << "% saved)" << std::endl;
        }
        overall_real_pixels += real_pixels;
        overall_trimmed_pixels += trimmed_pixels;
        overall_final_pixels += final_pixels;
    }
    if (verbose) {
        auto trimmed_save = (overall_trimmed_pixels - overall_final_pixels) / (double)overall_trimmed_pixels;
        auto real_save = (overall_real_pixels - overall_final_pixels) / (double)overall_real_pixels;
        std::cout << "final stats" << std::endl;
        std::cout << "  trimmed:  " << overall_final_pixels << " vs " << overall_trimmed_pixels
            << " (" << (trimmed_save * 100) << "% saved)" << std::endl;
        std::cout << "  original: " << overall_final_pixels << " vs " << overall_real_pixels
            << " (" << (real_save * 100) << "% saved)" << std::endl;
    }
    texture_list.close();
}
