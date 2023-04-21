#include "inout.h"

namespace image::io {

    const uint16_t PADDING_BYTES = 4;
    const uint16_t BYTES_PER_PIXEL = 3;
    const size_t FILE_HEADER_SIZE = 14;
    const size_t FILE_INFO_HEADER_SIZE = 40;

    Image Read(const char* filename) {
        std::ifstream in{filename, std::ios::binary};
        if (in) {
            // проверяем тот ли тип
            unsigned char file_header[FILE_HEADER_SIZE];
            in.read(reinterpret_cast<char*>(&file_header), sizeof(file_header));

            if (file_header[0] != 'B' || file_header[1] != 'M') {
                throw std::runtime_error("Error: is not BMP file.");
            }

            // читаем инфо
            unsigned char file_info_header[FILE_INFO_HEADER_SIZE];
            in.read(reinterpret_cast<char*>(&file_info_header), sizeof(file_info_header));

            // узнаем длину и ширину
            size_t width = file_info_header[4] + (file_info_header[5] << 8) + (file_info_header[6] << 16) +
                           (file_info_header[7] << 24);
            size_t height = file_info_header[8] + (file_info_header[9] << 8) + (file_info_header[10] << 16) +
                            (file_info_header[11] << 24);

            // размер отступа
            int line_padding = (PADDING_BYTES - width * BYTES_PER_PIXEL % PADDING_BYTES) % PADDING_BYTES;

            Image new_img(width, height);
            uint8_t bufer[3];

            // сохраняем
            for (size_t i = 0; i < height; ++i) {
                for (size_t j = 0; j < width; ++j) {
                    in.read(reinterpret_cast<char*>(&bufer), BYTES_PER_PIXEL);

                    new_img.GetColor(i, j).red = bufer[2];
                    new_img.GetColor(i, j).green = bufer[1];
                    new_img.GetColor(i, j).blue = bufer[0];
                }
                in.seekg(line_padding, std::ios_base::cur);
            }

            return new_img;
        } else {
            throw std::runtime_error("Error opening file");
        }
    }

    void Write(const char* filename, const Image& img) {
        std::ofstream out;
        out.open(filename, std::ios::binary);
        if (out) {
            // узнаем длину и ширину
            size_t width = img.GetWidth();
            size_t height = img.GetHeight();

            // размер отступа
            int line_padding = (PADDING_BYTES - width * BYTES_PER_PIXEL % PADDING_BYTES) % PADDING_BYTES;

            // размер файла
            const size_t file_size = FILE_HEADER_SIZE + FILE_INFO_HEADER_SIZE + (BYTES_PER_PIXEL * width +
                    line_padding) * height;

            // создаем fileHeader
            unsigned char file_header[] = {
                    0, 0,                     // тип
                    0, 0, 0, 0,      // размер файла в байтах
                    0, 0, 0, 0,      // резерв
                    0, 0, 0, 0,   // отступ для пикселей
            };

            file_header[0] = 'B';
            file_header[1] = 'M';
            file_header[2] = file_size;
            file_header[3] = file_size >> 8;
            file_header[4] = file_size >> 16;
            file_header[5] = file_size >> 24;
            file_header[10] = FILE_HEADER_SIZE + FILE_INFO_HEADER_SIZE;

            // создаем fileInfoHeader
            static unsigned char file_info_header[] = {
                    0, 0, 0, 0,       // размер хедера
                    0, 0, 0, 0,      // ширина изображения
                    0, 0, 0, 0,     // длина изображения
                    0, 0,                    // кол-во цветовых плоскостей
                    0, 0,                    // бит на пиксель
                    0, 0, 0, 0,    // сжатие
                    0, 0, 0, 0,   // размер изображения
                    0, 0, 0, 0,   // разрешение по горизонтали
                    0, 0, 0, 0,   // разрешение по вертикали
                    0, 0, 0, 0,   // цвета в таблице цветов
                    0, 0, 0, 0,   // счетчик цветов
            };

            file_info_header[0] = FILE_INFO_HEADER_SIZE;
            file_info_header[4] = width;
            file_info_header[5] = width >> 8;
            file_info_header[6] = width >> 16;
            file_info_header[7] = width >> 24;
            file_info_header[8] = height;
            file_info_header[9] = height >> 8;
            file_info_header[10] = height >> 16;
            file_info_header[11] = height >> 24;
            file_info_header[12] = 1;
            file_info_header[14] = 24;

            // записываем хедеры
            out.write(reinterpret_cast<char*>(file_header), FILE_HEADER_SIZE);
            out.write(reinterpret_cast<char*>(file_info_header), FILE_INFO_HEADER_SIZE);

            // пишем картинку
            uint8_t bmppass[3] = {0, 0, 0};
            for (size_t i = 0; i < height; ++i) {
                for (size_t j = 0; j < width; ++j) {
                    uint8_t red_color = img.GetColor(i, j).red;
                    uint8_t green_color = img.GetColor(i, j).green;
                    uint8_t blue_color = img.GetColor(i, j).blue;
                    uint8_t bufer[3] = {blue_color, green_color, red_color};
                    out.write(reinterpret_cast<char*>(bufer), 3);
                }
                out.write(reinterpret_cast<char*>(bmppass), line_padding);
            }
        } else {
            throw std::runtime_error("Error writing file");
        }
    }

}

