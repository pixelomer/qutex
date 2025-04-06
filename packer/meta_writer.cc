#include <qutex/meta_writer.hpp>

using namespace qutex;

void meta_writer::write_header() {
    static const char header[] = { 'Q', 'U', 'T', 'E', 'X', 0, 0, 0 };
    m_stream.write(header, sizeof(header));
}

meta_writer &meta_writer::operator<<(uint16_t x) {
    uint8_t buf[2] = { (uint8_t)(x >> 8), (uint8_t)x };
    m_stream.write((const char *)&buf[0], 2);
    return *this;
}

meta_writer &meta_writer::operator<<(std::string const& str) {
    *this << str.size();
    m_stream.write(str.c_str(), str.size());
    return *this;
}

void meta_writer::add_sprite(int x, int y, sprite_info const& info) {
    *this << info.real_width << info.real_height << info.width <<
        info.height << info.offset_x << x << y << info.offset_y << info.name;
}

void meta_writer::finalize(int width, int height) {
    m_stream.seekp(8);
    *this << width << height;
    m_stream.close();
}

meta_writer::meta_writer() {}

void meta_writer::open(std::filesystem::path path) {
    if (is_open()) {
        m_stream.close();
        std::runtime_error("open() called before meta_writer::finalize()");
    }
    m_stream.open(path, std::ios::binary | std::ios::out | std::ios::trunc);
    if (m_stream.fail()) {
        throw std::runtime_error("failed to open meta writer at " + path.string());
    }
    write_header();
    *this << 0 << 0;
}

meta_writer::~meta_writer() {
    if (is_open()) {
        m_stream.close();
        std::runtime_error("destructor called before meta_writer::finalize()");
    }
}

bool meta_writer::is_open() {
    return m_stream.is_open();
}
