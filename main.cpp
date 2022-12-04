#include <SDL2/SDL.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include "BMP.h"
#include <cmath>

#define WIDTH 800
#define HEIGHT 600


union Colour {
    struct Components {
        uint8_t alpha; // Unused
        uint8_t red;
        uint8_t green;
        uint8_t blue;
    } components; 
    uint32_t colour;

    bool gray_scale_2() {
        return components.red | components.green | components.blue;
    }

    Colour(uint32_t colour) : colour{colour} {}
};

namespace Colours {
    Colour White = Colour(0xFFFFFFFF);
    Colour Black = Colour(0x000000FF);
    Colour Red = Colour(0x0000FFFF);
    Colour Blue = Colour(0x00FF00FF);
    Colour Green = Colour(0xFF0000FF);
}

BMP *image;

SDL_Window *window;
SDL_Renderer *renderer;

void redraw() {
  SDL_RenderClear(renderer);
  // Draw a pixel
  for (int x = 0; x < WIDTH; x++) {
    for (int y = 0; y < HEIGHT; y++) {
        Colour colour = image->get_pixel(x, y);
        SDL_SetRenderDrawColor(renderer, /* RGBA: red */ colour.components.red, colour.components.green, colour.components.blue, 0xFF);
        SDL_RenderDrawPoint(renderer, x, y);
    }
  }
  SDL_RenderPresent(renderer);
}

uint32_t ticksForNextKeyDown = 0;

bool handle_events() {
  SDL_Event event;
  SDL_PollEvent(&event);
  if (event.type == SDL_QUIT) {
    return false;
  }
  if (event.type == SDL_KEYDOWN) {
    uint32_t ticksNow = SDL_GetTicks();
    if (SDL_TICKS_PASSED(ticksNow, ticksForNextKeyDown)) {
      // Throttle keydown events for 10ms.
      ticksForNextKeyDown = ticksNow + 10;
      redraw();
    }
  }
  return true;
}

void run_main_loop() {
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop([]() { handle_events(); }, 0, true);
#else
  while (handle_events())
    ;
#endif
}


int sdl_main() {
  SDL_Init(SDL_INIT_VIDEO);

  SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);

  redraw();
  run_main_loop();

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  SDL_Quit();
  return 0;
}

void set_pixel(uint32_t x, uint32_t y, Colour colour) {
    if (x >= WIDTH || y >= HEIGHT) return; 
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
    image = new BMP(WIDTH, HEIGHT, false);
    
    clear_image();
    plot_between_circles(400, 300, 100, 123, Colours::Black);
    plot_between_circles(200, 400, 100, 123, Colours::Red);
    plot_between_circles(600, 400, 100, 123, Colours::Green);
    plot_between_circles(400, 500, 100, 123, Colours::Blue);
    sdl_main();
    delete image;    
    return 0;
}