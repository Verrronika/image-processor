#pragma once

#include "image.h"

#include <iostream>
#include <fstream>
#include <ostream>

namespace image::io {

    Image Read(const char* filename);
    void Write(const char* filename, const Image& img);

}
