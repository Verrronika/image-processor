#include "image.h"
#include "filters.h"
#include "inout.h"

#include <iostream>

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cout << "Not enough arguments "<< std::endl;
        return 1;
    }
    const char* filename = argv[1];
    if (!std::filesystem::exists(filename)) {
        std::cout << "There is no such file" << std::endl;
        return 1;
    }

    std::vector<image::filters::Filter*> filter_stream;
    image::Image img;
    img = image::io::Read(filename);
    size_t i = 3;
    while (i < argc) {
        std::string filter = argv[i];
        if (filter == "-crop") {
            if (i + 2 >= argc) {
                throw std::invalid_argument("Less than two parameters for crop");
            }
            filter_stream.push_back(new image::filters::Crop(std::stoi(argv[i + 1]), std::stoi(argv[i + 2])));
        } else if (filter == "-gs") {
            filter_stream.push_back(new image::filters::Grayscale());
        } else if (filter == "-neg") {
            filter_stream.push_back(new image::filters::Negative());
        } else if (filter == "-sharp") {
            filter_stream.push_back(new image::filters::Sharpening());
        } else if (filter == "-edge") {
            if (i + 1 >= argc) {
                throw std::invalid_argument("Less than one parameter for crop");
            }
            filter_stream.push_back(new image::filters::EdgeDetection(std::stod(argv[i + 1])));
        } else if (filter == "-blur") {
            if (i + 1 >= argc) {
                throw std::invalid_argument("Less than one parameter for crop");
            }
            filter_stream.push_back(new image::filters::GaussianBlur(std::stoi(argv[i + 1])));
        }
        ++i;
    }
    for (auto item : filter_stream) {
        img = item->Apply(img);
    }
    image::io::Write(argv[2], img);
}
