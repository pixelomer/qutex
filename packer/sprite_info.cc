#include <qutex/sprite_info.hpp>
#include "stbi_buf.hpp"

using namespace qutex;

sprite_info::sprite_info(std::filesystem::path path, std::string name,
    bool trim_transparency): name(name), path(path)
{
    offset_x = 0;
    offset_y = 0;
    int x, y, comp;
    if (trim_transparency) {
        stbi_buf buf = stbi_load(path.c_str(), &x, &y, &comp,
            4);
        if (buf == nullptr) {
            throw std::runtime_error("invalid image: " + path.string());
        }
        width = real_width = x;
        height = real_height = y;

        // only attempt to trim transparency if original image has transparency
        if (comp == 4) {
            bool done;

            // find left offset
            for (x=0, done=false; x<real_width && !done; ++x) {
                for (y=0; y<real_height && !done; ++y) {
                    if (buf[y*real_width*4 + x*4 + 3] != 0) {
                        done = true;
                    }
                }
                if (!done) {
                    --width;
                    ++offset_x;
                }
            }
            
            // find top offset
            for (y=0, done=false; y<real_height && !done; ++y) {
                for (x=offset_x; x<real_width && !done; ++x) {
                    if (buf[y*real_width*4 + x*4 + 3] != 0) {
                        done = true;
                    }
                }
                if (!done) {
                    --height;
                    ++offset_y;
                }
            }

            // find right offset, optimized with top offset
            for (x=real_width-1, done=false; x>=offset_x && !done; --x) {
                for (y=offset_y; y<real_height && !done; ++y) {
                    if (buf[y*real_width*4 + x*4 + 3] != 0) {
                        done = true;
                    }
                }
                if (!done) {
                    --width;
                }
            }

            // find bottom offset, optimized with left and right offsets
            for (y=real_height-1, done=false; y>=offset_y && !done; --y) {
                for (x=offset_x; x<(width + offset_x) && !done; ++x) {
                    if (buf[y*real_width*4 + x*4 + 3] != 0) {
                        done = true;
                    }
                }
                if (!done) {
                    --height;
                }
            }
        }
    }
    else {
        int ok = stbi_info(path.c_str(), &x, &y, &comp);
        if (!ok) {
            throw std::runtime_error("invalid image: " + path.string());
        }
        width = real_width = x;
        height = real_height = y;
    }
}
