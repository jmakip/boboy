#include <SDL2/SDL.h>
#include "mem.h"

//The window we'll be rendering to
SDL_Window *window = NULL;

//The surface contained by the window
SDL_Surface *screen_surface = NULL;

//Current displayed image
SDL_Surface *tile_map = NULL;

uint32_t pixels[128 * 8 * 128 * 8];

void init_window()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        exit(1);
    }

    window =
        SDL_CreateWindow("tilemap", SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);

    if (!window) {
        printf("SDL_CreateWindow null\n");
        exit(1);
    }

    screen_surface = SDL_GetWindowSurface(window);
}

//Block 0 is $8000-87FF
//Block 1 is $8800-8FFF
//Block 2 is $9000-97FF
void convert_pixels()
{
    uint16_t dot;
    int x, y;
    uint32_t width = 128 * 8;
    uint32_t rbga = 0;

    memset(pixels, 0xff, sizeof(pixels));
    for (x = 0; x < 16; x++) {
        for (y = 0; y < 8; y++) {
            int i; 
            uint32_t pix8[8]; 
            dot = mem_read16(0x8000 + (16 * x + 2 * y));
            for (i = 0; i < 8;i++) {
                pix8[i] = ((dot & (0x01<<(i-1)))>>(i-1)) *64  ;
                pix8[i] += ((dot & (0x0100<<(i-1))) >>(i-1)) *64;
                
                
            }
            memcpy(&pixels[(8 * x + y*128)], pix8, 8 * 4);
        }
    }
}

void render_tilemap()
{
    convert_pixels();
    SDL_Surface *loadedSurface = SDL_CreateRGBSurfaceFrom(
        pixels, 128 , 128 , 32, 4 * 128,
        //SDL_CreateRGBSurfaceFrom(pixels, 128, 128, 32, 4 * 128,
        0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);

    tile_map = SDL_ConvertSurface(loadedSurface, screen_surface->format, 0);
    SDL_Rect stretchRect;
    stretchRect.x = 0;
    stretchRect.y = 0;
    stretchRect.w = 640;
    stretchRect.h = 480;
    SDL_BlitScaled(tile_map, NULL, screen_surface, &stretchRect);
    SDL_UpdateWindowSurface(window);
    SDL_FreeSurface(tile_map);
}

void poll_events()
{
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) { exit(1); }
    }
}
