#pragma once

#include "image.h"

#include <iostream>
#include <cmath>
#include <memory>
#include <algorithm>
#include <string>
#include <vector>

namespace image::filters {

    class Filter {
    public:
        virtual Image Apply(Image& img) = 0;
        virtual ~Filter() = default;
    };

    class Crop : public Filter {
    public:
        Crop(size_t width, size_t height);
        Image Apply(Image& img) override;

    private:
        size_t width_;
        size_t height_;
    };

    class Grayscale : public Filter {
    public:
        Image Apply(Image& img) override;
    };

    class Negative : public Filter {
    public:
        Image Apply(Image& img) override;
    };

    class Sharpening : public Filter {
    public:
        Image Apply(Image& img) override;
    };

    class EdgeDetection : public Filter {
    public:
        EdgeDetection(double threshold);
        Image Apply(Image& img) override;

    private:
        double threshold_;
    };

    class GaussianBlur : public Filter {
    public:
        GaussianBlur(int sigma);
        Image Apply(Image& img) override;
        std::vector<double> ECount(int sigma);
    private:
        int sigma_ = 0;
        double summa_ = 0;
        size_t size_ = 0;
    };

}
