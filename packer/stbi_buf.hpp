#pragma once

#include <cstdint>
#include "stb_image.h"

// raii container for buffers returned from stb_image
class stbi_buf {
public:
    stbi_buf(stbi_buf const&) = delete;
    stbi_buf(stbi_buf&&) = delete;
    stbi_buf(stbi_uc *buf = nullptr): buf(buf) {}
    uint8_t &operator[](int idx) {
        return buf[idx];
    }
    bool operator==(uint8_t *rhs) {
        return buf == rhs;
    }
    ~stbi_buf() {
        if (buf != nullptr) {
            stbi_image_free(buf);
        }
    }
private:
    uint8_t *buf;
};
