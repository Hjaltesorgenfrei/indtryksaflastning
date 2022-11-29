#include "BMP.h"
#include <cmath>

int main() {
    auto image = BMP(800, 600, false);
    int circle_r = 100;
    int circle_x = 400;
    int circle_y = 300;
    for (uint32_t x = 0; x < 800; x++) {
        for (uint32_t y = 0; y < 600; y++) {
            if ((x- circle_x) * (x- circle_x) + (y - circle_y) * (y - circle_y) < circle_r * circle_r) {
                image.set_pixel(x, y, 0, 0, 0, 255);
            }
            else {
                image.set_pixel(x, y, 255, 255, 255, 255);
            }
        }
    }
    image.write("circle.bmp");
    return 0;
}