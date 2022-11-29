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
};

void set_pixel(int x, int y, int colour) {
    image->set_pixel(x, y, colour, colour, colour, 255);
}

void x_line(int x1, int x2, int y, int colour)
{
    while (x1 <= x2) set_pixel(x1++, y, colour);
}

void y_line(int x, int y1, int y2, int colour)
{
    while (y1 <= y2) set_pixel(x, y1++, colour);
}

// (uint32_t x, uint32t_t y, uint32t_t radius, uint32t_t width, Pixel pixel) is probably a better interface
void plot_between_circles(int xc, int yc, int inner, int outer, int colour)
{
    int xo = outer;
    int xi = inner;
    int y = 0;
    int erro = 1 - xo;
    int erri = 1 - xi;

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

void clear_image(int clearColour = 255) {
    for (int x = 0; x < image->bmp_info_header.width; x++) {
        for (int y = 0; y < image->bmp_info_header.height; y++) {
            set_pixel(x, y, clearColour);
        }
    }
}

// TODO: Implement the rest of http://members.chello.at/~easyfilter/bresenham.html

int main() {
    image = new BMP(800, 600, false);
    clear_image();
    plot_between_circles(400, 300, 100, 123, 0);
    image->write("circle.bmp");
    delete image;
    return 0;
}