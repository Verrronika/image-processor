#include "filters.h"

namespace image::filters {

    Crop::Crop(size_t width, size_t height) {
        width_ = width;
        height_ = height;
    }

    Image Crop::Apply(Image& img) {
        size_t new_width = img.GetWidth();
        size_t new_height = img.GetHeight();
        if (static_cast<int>(width_) < 0 || static_cast<int>(height_) < 0) {
            throw std::runtime_error("Negative number");
        }
        if (static_cast<size_t>(width_) < new_width) {
            new_width = static_cast<size_t>(width_);
        }
        if (static_cast<size_t>(height_) < new_height) {
            new_height = static_cast<size_t>(height_);
        }
        Image new_img = Image(new_width, new_height);
        size_t start_height = img.GetHeight() - new_height;
        for (size_t i = start_height; i < img.GetHeight(); ++i) {
            for (size_t j = 0; j < new_img.GetWidth(); ++j) {
                new_img.SetColor(i - start_height, j, img.GetColor(i, j));
            }
        }
        return new_img;
    }

    Image Grayscale::Apply(Image& img) {
        Image new_img = Image(img.GetWidth(), img.GetHeight());
        for (size_t i = 0; i < new_img.GetHeight(); ++i) {
            for (size_t j = 0; j < new_img.GetWidth(); ++j) {
                uint8_t color = 0.229 * img.GetColor(i, j).red + 0.587 * img.GetColor(i, j).green +
                                0.114 * img.GetColor(i, j).blue;
                new_img.GetColor(i, j).red = color;
                new_img.GetColor(i, j).green = color;
                new_img.GetColor(i, j).blue = color;
            }
        }
        return new_img;
    }

    Image Negative::Apply(Image& img) {
        Image new_img = Image(img.GetWidth(), img.GetHeight());
        for (size_t i = 0; i < new_img.GetHeight(); ++i) {
            for (size_t j = 0; j < new_img.GetWidth(); ++j) {
                new_img.GetColor(i, j).red = 255 - img.GetColor(i, j).red;
                new_img.GetColor(i, j).green = 255 - img.GetColor(i, j).green;
                new_img.GetColor(i, j).blue = 255 - img.GetColor(i, j).blue;
            }
        }
        return new_img;
    }

    Image Sharpening::Apply(Image& img) {
        Image new_img = Image(img.GetWidth(), img.GetHeight());
        for (size_t i = 0; i < new_img.GetHeight(); ++i) {
            for (size_t j = 0; j < new_img.GetWidth(); ++j) {
                // шаги = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};
                Color color_maxi_j = img.GetColor(std::max(static_cast<int>(i - 1), 0), j);
                Color color_i_maxj = img.GetColor(i, std::max(static_cast<int>(j - 1), 0));
                Color color_mini_j = img.GetColor(std::min(i + 1, img.GetHeight() - 1), j);
                Color color_i_minj = img.GetColor(i, std::min(j + 1, img.GetWidth() - 1));
                Color color_i_j = img.GetColor(i, j);
                new_img.GetColor(i, j).red = std::min(255, std::max(0, -color_maxi_j.red -
                                                    color_i_maxj.red - color_mini_j.red -
                                                    color_i_minj.red + 5 * color_i_j.red));
                new_img.GetColor(i, j).blue = std::min(255, std::max(0, - color_maxi_j.blue -
                                                    color_i_maxj.blue - color_mini_j.blue -
                                                    color_i_minj.blue + 5 * color_i_j.blue));
                new_img.GetColor(i, j).green = std::min(255, std::max(0, - color_maxi_j.green -
                                                    color_i_maxj.green - color_mini_j.green -
                                                    color_i_minj.green + 5 * color_i_j.green));
            }
        }
        return new_img;
    }

    EdgeDetection::EdgeDetection(double threshold) {
        threshold_ = threshold;
    }

    Image EdgeDetection::Apply(Image& img) {
        Grayscale gs;
        Image new_img = gs.Apply(img);
        int threshold = static_cast<int>(threshold_ * 255);
        for (size_t i = 0; i < new_img.GetHeight(); ++i) {
            for (size_t j = 0; j < new_img.GetWidth(); ++j) {
                // шаги = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};
                size_t max_i = std::max(static_cast<int>(i - 1), 0);
                size_t max_j = std::max(static_cast<int>(j - 1), 0);
                size_t min_i = std::min(i + 1, img.GetHeight() - 1);
                size_t min_j = std::min(j + 1, img.GetWidth() - 1);
                uint8_t new_red = std::min(255, std::max(0, - img.GetColor(max_i, j).red -
                                    img.GetColor(i, max_j).red - img.GetColor(min_i, j).red -
                                    img.GetColor(i, min_j).red + 4 * img.GetColor(i, j).red));
                if (new_red > threshold) {
                    new_img.GetColor(i, j) = Color{255, 255, 255};
                } else {
                    new_img.GetColor(i, j) = Color{0, 0, 0};
                }
            }
        }
        return new_img;
    }

    GaussianBlur::GaussianBlur(int sigma) {
        sigma_ = sigma;
    }

    std::vector<double> GaussianBlur::ECount(int sigma) {
        double pi_divider = std::sqrt(2 * M_PI * sigma * sigma);
        double divider = 2 * sigma * sigma;
        size_ = 2 * (sigma * static_cast<size_t>(std::sqrt(2 * log(255))) - 1) + 1; // связь радиуса и сигмы
        std::vector<double> exponents(size_);
        for (size_t i = 0; i < size_; ++i) {
            double x = i - size_ / 2;
            exponents[i] = std::exp(static_cast<double>((-1) * x * x) / divider) / pi_divider;
            summa_ += exponents[i];
        }
        return exponents;
    }

    Image GaussianBlur::Apply(Image &img) {
        Image middle_img = Image(img.GetWidth(), img.GetHeight());
        std::vector<double> exponents = ECount(sigma_);
        for (size_t i = 0; i < img.GetHeight(); ++i) {
            for (size_t j = 0; j < img.GetWidth(); ++j) {
                double red = 0;
                double green = 0;
                double blue = 0;
                for (size_t k = 0; k < exponents.size(); ++k) {
                    int l = std::max(0, std::min(static_cast<int>(j + k - size_ / 2),
                                                 static_cast<int>(img.GetWidth() - 1)));
                    red += static_cast<double>(img.GetColor(l, i).red) * exponents[k] / summa_;
                    green += static_cast<double>(img.GetColor(l, i).green) * exponents[k] / summa_;
                    blue += static_cast<double>(img.GetColor(l, i).blue) * exponents[k] / summa_;
                }
                middle_img.GetColor(j, i).red = static_cast<uint8_t>(red);
                middle_img.GetColor(j, i).green = static_cast<uint8_t>(green);
                middle_img.GetColor(j, i).blue = static_cast<uint8_t>(blue);
            }
        }
        Image new_img = Image(img.GetWidth(), img.GetHeight());
        for (size_t i = 0; i < middle_img.GetHeight(); ++i) {
            for (size_t j = 0; j < middle_img.GetWidth(); ++j) {
                double red = 0;
                double green = 0;
                double blue = 0;
                for (size_t k = 0; k < exponents.size(); ++k) {
                    int l = std::max(0, std::min(static_cast<int>(i + k - size_ / 2),
                                                 static_cast<int>(middle_img.GetHeight() - 1)));
                    red += static_cast<double>(middle_img.GetColor(j, l).red) * exponents[k] / summa_;
                    green += static_cast<double>(middle_img.GetColor(j, l).green) * exponents[k] / summa_;
                    blue += static_cast<double>(middle_img.GetColor(j, l).blue) * exponents[k] / summa_;
                }
                new_img.GetColor(j, i).red = static_cast<uint8_t>(red);
                new_img.GetColor(j, i).green = static_cast<uint8_t>(green);
                new_img.GetColor(j, i).blue = static_cast<uint8_t>(blue);
            }
        }
        return new_img;
    }

}
