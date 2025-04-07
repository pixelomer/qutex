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

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <filesystem>
#include <qutex/packer.hpp>
#include <qutex/sprite_info.hpp>
#include <qutex/reader.hpp>
#include <iostream>
#include <stdexcept>
#include <map>

std::ostream &operator<<(std::ostream &lhs, qutex::sprite_info const& rhs) {
    return lhs << "<sprite_info " <<
        "name:\"" << rhs.name << "\" " <<
        "width:" << rhs.width << " " <<
        "height:" << rhs.height << " " <<
        "offset_x:" << rhs.offset_x << " " <<
        "offset_y:" << rhs.offset_y << " " <<
        "real_width:" << rhs.real_width << " " <<
        "real_height:" << rhs.real_height << ">";
}

struct qutex_demo_sprite {
    int texture_x;
    int texture_y;
    SDL_Texture *texture;
    qutex::sprite_info info;
    std::filesystem::path texture_path;
    qutex_demo_sprite(int texture_x, int texture_y, SDL_Texture *texture,
        qutex::sprite_info const& info, std::filesystem::path texture_path):
        texture_x(texture_x), texture_y(texture_y), texture(texture), info(info),
        texture_path(texture_path) {}
};

class qutex_demo {
public:
    qutex_demo(std::filesystem::path path): m_sprite_idx(0), m_ok(true) {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
            m_ok = false;
            return;
        }
    
        m_window_width = 640;
        m_window_height = 480;
        if (!SDL_CreateWindowAndRenderer("qutex_demo", m_window_width,
            m_window_height, 0, &m_window,
            &m_renderer)) 
        {
            SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
            m_ok = false;
            return;
        }

        try {
            qutex::reader reader { path };
            SDL_Texture *current_texture;
            std::filesystem::path current_path;
            reader.read_all_sprites(
                [&](std::filesystem::path path, int, int) {
                    if (m_textures.count(path) == 0) {
                        current_path = path;
                        current_texture = m_textures[path] =
                            IMG_LoadTexture(m_renderer, path.c_str());
                        if (current_texture == NULL) {
                            throw std::runtime_error("could not load texture: "
                                + path.string());
                        }
                    }
                },
                [&](int x, int y, qutex::sprite_info const& info) {
                    m_sprites.emplace_back(x, y, current_texture, info, current_path);
                }
            );
        }
        catch (std::exception &ex) {
            SDL_Log("Couldn't read sprites: %s", ex.what());
            m_ok = false;
            return;
        }
        if (m_sprites.empty()) {
            SDL_Log("Couldn't find any sprites in this pack");
            m_ok = false;
            return;
        }
    }
    SDL_AppResult iterate() {
        SDL_SetRenderDrawColorFloat(m_renderer, 1, 1, 1, 1);
        SDL_RenderClear(m_renderer);
        auto &sprite = m_sprites[m_sprite_idx];

        //FIXME: SDL renders 1 column and 1 line before the sprite unless +1/-1 is added
        //       Is this a precision issue or a bug in qutex?
        SDL_FRect dest;
        dest.h = sprite.info.height - 1;
        dest.w = sprite.info.width - 1;
        dest.x = m_window_width / 2.0 - dest.w / 2.0;
        dest.y = m_window_height / 2.0 - dest.h / 2.0;
        SDL_FRect src;
        src.h = sprite.info.height - 1;
        src.w = sprite.info.width - 1;
        src.x = sprite.texture_x + 1;
        src.y = sprite.texture_y + 1;

        SDL_RenderTexture(m_renderer, sprite.texture,
            &src, &dest);
        SDL_RenderPresent(m_renderer);
        return SDL_APP_CONTINUE;
    }
    void update_title() {
        auto title = m_sprites[m_sprite_idx].info.name + " [" +
            std::to_string(m_sprites[m_sprite_idx].info.width) + "x" +
            std::to_string(m_sprites[m_sprite_idx].info.height) + "] (" +
            std::to_string(m_sprite_idx + 1) + " / " +
            std::to_string(m_sprites.size()) + ")";
        SDL_SetWindowTitle(m_window, title.c_str());
    }
    SDL_AppResult event(SDL_Event *event) {
        if (event->type == SDL_EVENT_QUIT) {
            return SDL_APP_SUCCESS;
        }
        else if (event->type == SDL_EVENT_KEY_DOWN) {
            switch (event->key.key) {
                case SDLK_LEFT:
                    if (m_sprite_idx > 0) --m_sprite_idx;
                    update_title();
                    break;
                case SDLK_RIGHT:
                    if (m_sprite_idx < m_sprites.size() - 1) ++m_sprite_idx;
                    update_title();
                    break;
                case SDLK_TAB: {
                    auto &sprite = m_sprites[m_sprite_idx];
                    std::cout << "{" << sprite.texture_x << ", " <<
                        sprite.texture_y << "} in " << sprite.texture_path << std::endl;
                    std::cout << sprite.info << std::endl;
                    break;
                }
                default:
                    break;
            }
        }
        return SDL_APP_CONTINUE;
    }
    bool ok() {
        return m_ok;
    }
private:
    std::vector<qutex_demo_sprite> m_sprites;
    size_t m_sprite_idx;
    bool m_ok;
    int m_window_width;
    int m_window_height;
    std::map<std::filesystem::path, SDL_Texture *> m_textures;
    SDL_Renderer *m_renderer;
    SDL_Window *m_window;
};

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    if (argc == 4 && strcmp(argv[1], "pack") == 0) {
        qutex::packer packer { true };
        packer.add_sprites(argv[2], ".png",
            [](qutex::sprite_info const& info){
                std::cout << info << std::endl;
            });
        packer.pack(1024, 1024, argv[3], 4, true);
        return SDL_APP_SUCCESS;
    }
    else if (argc == 3 && strcmp(argv[1], "read") == 0) {
        qutex::reader reader { argv[1] };
        reader.read_all_sprites(
            [](std::filesystem::path path, int, int) {
                std::cout << path << std::endl;
            },
            [](int x, int y, qutex::sprite_info const& info) {
                std::cout << x << "," << y << " ==> " << info << std::endl;
            }
        );
        return SDL_APP_SUCCESS;
    }
    else if (argc == 3 && strcmp(argv[1], "show") == 0) {
        auto app = new qutex_demo { argv[2] };
        if (!app->ok()) {
            return SDL_APP_FAILURE;
        }
        *appstate = app;
        return SDL_APP_CONTINUE;
    }
    else {
        std::cerr << "Usage:" << std::endl <<
            "  " << argv[0] << " pack <sprites/> <pack/>" << std::endl <<
            "  " << argv[0] << " view <pack/>" << std::endl <<
            "  " << argv[0] << " show <pack/>" << std::endl;
    }
    return SDL_APP_FAILURE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    auto &app = *(qutex_demo *)appstate;
    return app.iterate();
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    auto &app = *(qutex_demo *)appstate;
    return app.event(event);
}

void SDL_AppQuit(void *, SDL_AppResult) {}
