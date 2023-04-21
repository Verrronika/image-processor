#pragma once

#include <algorithm>
#include <vector>
#include <cstdint>

namespace image {

    struct Color {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
    };

    class Image {
    public:
        Image();
        Image(size_t width, size_t height);
        ~Image();
        size_t GetWidth() const;
        size_t GetHeight() const;
        const Color& GetColor(size_t x, size_t y) const;
        Color& GetColor(size_t x, size_t y);
        void SetColor(size_t x, size_t y, Color c);

    private:
        std::vector<std::vector<Color>> data_;
        size_t width_;
        size_t height_;
    };

}
