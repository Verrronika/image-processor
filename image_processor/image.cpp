#include "image.h"

namespace image {

    Image::Image() {
        width_ = 0;
        height_ = 0;
        data_ = {};
    }

    Image::Image(size_t width, size_t height) {
        width_ = width;
        height_ = height;
        data_ = std::vector<std::vector<Color>>(height_);
        for (size_t i = 0; i < height_; ++i) {
            data_[i].resize(width_);
        }
    }

    Image::~Image() {
        width_ = 0;
        height_ = 0;
        data_.clear();
    }

    size_t Image::GetWidth() const {
        return width_;
    }

    size_t Image::GetHeight() const {
        return height_;
    }

    const Color& Image::GetColor(size_t x, size_t y) const {
        return data_[x][y];
    }

    Color& Image::GetColor(size_t x, size_t y) {
        return data_[x][y];
    }

    void Image::SetColor(size_t x, size_t y, Color c) {
        data_[x][y] = c;
    }

}
