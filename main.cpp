#include "BMP.h"
#include <cmath>

BMP* image;

union Colour {
    struct Components {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
        uint8_t alpha; // Unused
    } components; 
    uint32_t colour;

    bool gray_scale_2() {
        return components.red | components.green | components.blue;
    }

    Colour(uint32_t colour) : colour{colour} {}
};

namespace Colours {
    Colour White = Colour(0xFFFFFFFF);
    Colour Black = Colour(0x00000000);
}

void set_pixel(uint32_t x, uint32_t y, Colour colour) {
    image->set_pixel(x, y, colour.components.red, colour.components.green, colour.components.blue, 255);
}

void x_line(uint32_t x1, uint32_t x2, uint32_t y, Colour colour)
{
    while (x1 <= x2) set_pixel(x1++, y, colour);
}

void y_line(uint32_t x, uint32_t y1, uint32_t y2, Colour colour)
{
    while (y1 <= y2) set_pixel(x, y1++, colour);
}

// (uint32_t x, uint32t_t y, uint32t_t radius, uint32t_t width, Pixel pixel) is probably a better interface
void plot_between_circles(uint32_t xc, uint32_t yc, uint32_t inner, uint32_t outer, Colour colour)
{
    uint32_t xo = outer;
    uint32_t xi = inner;
    uint32_t y = 0;
    int32_t erro = 1 - xo;
    int32_t erri = 1 - xi;

    while(xo >= y) {
        x_line(xc + xi, xc + xo, yc + y,  colour);
        y_line(xc + y,  yc + xi, yc + xo, colour);
        x_line(xc - xo, xc - xi, yc + y,  colour);
        y_line(xc - y,  yc + xi, yc + xo, colour);
        x_line(xc - xo, xc - xi, yc - y,  colour);
        y_line(xc - y,  yc - xo, yc - xi, colour);
        x_line(xc + xi, xc + xo, yc - y,  colour);
        y_line(xc + y,  yc - xo, yc - xi, colour);

        y++;

        if (erro < 0) {
            erro += 2 * y + 1;
        } else {
            xo--;
            erro += 2 * (y - xo + 1);
        }

        if (y > inner) {
            xi = y;
        } else {
            if (erri < 0) {
                erri += 2 * y + 1;
            } else {
                xi--;
                erri += 2 * (y - xi + 1);
            }
        }
    }
}

void clear_image(Colour colour = Colours::White) {
    for (uint32_t x = 0; x < image->bmp_info_header.width; x++) {
        for (uint32_t y = 0; y < image->bmp_info_header.height; y++) {
            set_pixel(x, y, colour);
        }
    }
}

// TODO: Implement the rest of http://members.chello.at/~easyfilter/bresenham.html

int main() {
    image = new BMP(800, 600, false);
    clear_image();
    plot_between_circles(400, 300, 100, 123, Colours::Black);
    image->write("circle.bmp");
    delete image;
    return 0;
}