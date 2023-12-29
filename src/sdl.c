#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include "mem.h"
#include "gpu.h"
#include "sdl.h"

//The window we'll be rendering to
SDL_Window *window = NULL;

//The surface contained by the window
SDL_Surface *screen_surface = NULL;

//Current displayed image
SDL_Surface *tile_map = NULL;

uint32_t pixels[256 * 256];

void init_window()
{
    //if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        exit(1);
    }

    window = SDL_CreateWindow("tilemap", SDL_WINDOWPOS_UNDEFINED,
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
void draw_bg()
{
    uint16_t dot;
    int x, y, yy;
    uint32_t width = 128 * 8;
    uint32_t rbga = 0;

    memset(pixels, 0xff, sizeof(pixels));
    for (yy = 0; yy < 16; yy++) {
        for (x = 0; x < 16; x++) {
            for (y = 0; y < 8; y++) {
                int i;
                uint32_t pix8[8];
                dot = mem_read16(0x8000 + (16 * x + 2 * y) + yy * 16 * 8 * 8 * 2);
                for (i = 0; i < 8; i++) {
                    pix8[i] = ((dot & (0x80 >> (i - 1))) << (i - 1)) * 128;
                    pix8[i] += ((dot & (0x8000 >> (i - 1))) << (i - 1)) * 64;
                    pix8[i] ^= 0x00FFFFFF;
                }
                memcpy(&pixels[(8 * x + y * 128) + yy * 16 * 8 * 8], pix8,
                       8 * 4);
            }
        }
    }
}
void dump_OAM() 
{
    for (int i = 0; i < 40; i++) {
        //y pos, x pos, tile index, attributes
        uint8_t y = mem_read(0xFE00 + i * 4);
        uint8_t x = mem_read(0xFE00 + i * 4 + 1);
        uint8_t tile = mem_read(0xFE00 + i * 4 + 2);
        uint8_t attr = mem_read(0xFE00 + i * 4 + 3);
        printf("OAM[%d] = y = %d, x = %d, tile = %d, attr = %d\n", i, y, x, tile, attr);
    }

}


void render_tilemap()
{
    //convert_pixels();
    assemble_bg_map();
    draw_sprites();
    get_bg_map(pixels);
    SDL_Surface *loadedSurface = SDL_CreateRGBSurfaceFrom(
        pixels, 256, 256, 32, 4 * 256,
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

extern uint8_t keyboard[8];
void poll_events()
{
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) { exit(1); }
        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_0) {
                printf("dumping memory\n");
                dump_mem();
            } else if (e.key.keysym.sym == SDLK_1) {
                print_reg();
            } else {
                printf("keydown key sym: %d\n", (int32_t)e.key.keysym.sym);
                //97, 100, 119, 115, 106, 107, 117, 105
                switch (e.key.keysym.sym) {
                    case 97:
                        keyboard[1] = 1;
                        break;
                    case 100:
                        keyboard[0] = 1;
                        break;
                    case 119:
                        keyboard[2] = 1;
                        break;
                    case 115:
                        keyboard[3] = 1;
                        break;
                    case 106:
                        keyboard[4] = 1;
                        break;
                    case 107:
                        keyboard[5] = 1;
                        break;
                    case 117:
                        keyboard[6] = 1;
                        break;
                    case 105:
                        keyboard[7] = 1;
                        break;
                }



            }
        }
        if (e.type == SDL_KEYUP) {
            printf("keyup key sym: %d\n", (int32_t)e.key.keysym.sym);
            //97, 100, 119, 115, 106, 107, 117, 105
            switch (e.key.keysym.sym) {
                    case 97:
                        keyboard[1] = 0;
                        break;
                    case 100:
                        keyboard[0] = 0;
                        break;
                    case 119:
                        keyboard[2] = 0;
                        break;
                    case 115:
                        keyboard[3] = 0;
                        break;
                    case 106:
                        keyboard[4] = 0;
                        break;
                    case 107:
                        keyboard[5] = 0;
                        break;
                    case 117:
                        keyboard[6] = 0;
                        break;
                    case 105:
                        keyboard[7] = 0;
                        break;
            }

        }
    }
}


//structure that holds SDL2 texture along with its width and height
//used to render text
struct texture {
    SDL_Texture *texture;
    SDL_Texture *text;
    int width;
    int height;
};
//structure that holds state of debug window
//debug window is a separate window that shows the state of the cpu
//and memory around SP and PC
//it is updated every time the cpu is stepped on debug mode
//window is created using SDL2
//Debug info is displayed as text and text is rendered using SDL2_ttf
struct dbg_window {
    SDL_Window *window; //Debug window
    SDL_Renderer *renderer; //Debug window renderer
    SDL_Surface *surface; //Debug window surface
    TTF_Font *font; //Debug window font
    struct texture text_texture; //Debug window text texture
};

//initialize debug window
//create window, renderer, font and text texture
//return 0 on success, -1 on failure
int32_t init_dbg_window(struct dbg_window *dbg_win)
{
    if (TTF_Init() < 0) {
        printf("TTF_Init Error: %s", TTF_GetError());
        return -1;
    }
    //initialize SDL2
    //if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    //    printf("Could not initialize SDL2: %s\n", SDL_GetError());
    //    return -1;
    //}
    //create debug window
    dbg_win->window = SDL_CreateWindow("Debug", SDL_WINDOWPOS_UNDEFINED,
                                       SDL_WINDOWPOS_UNDEFINED, 640, 480,
                                       SDL_WINDOW_SHOWN);
    if (!dbg_win->window) {
        printf("Could not create debug window: %s\n", SDL_GetError());
        return -1;
    }

    if (TTF_Init() < 0) {
        printf("Could not initialize SDL2_ttf: %s\n", TTF_GetError());
        return -1;
    }

    //initialize SDL2 TTF_font
    //dbg_win->font = TTF_OpenFont("GamergirlClassic-9MVj.ttf", 16);
    dbg_win->font = TTF_OpenFont("font.ttf", 16);


    if (!dbg_win->font) {
        printf("Could not open font: %s\n", SDL_GetError());
        return -1;
    }
    //get debug window surface
    dbg_win->surface = SDL_GetWindowSurface(dbg_win->window);
    
    return 0;
}
//draw debug window
int32_t draw_dbg_window(struct dbg_window *dbg_win)
{
   
    const char *dbg_text = "A = 0x00, X = 0x00, Y = 0x00, SP = 0x00, PC = 0x00";
    SDL_Color text_color = {0, 0, 0, 0};
    SDL_Surface *text_surface =
        TTF_RenderText_Solid(dbg_win->font, dbg_text, text_color);
    if (!text_surface) {
        printf("Could not create text surface: %s", SDL_GetError());
        return -1;
    }
    SDL_Rect stretchRect;
    stretchRect.x = 0;
    stretchRect.y = 0;
    stretchRect.w = 640;
    stretchRect.h = 240;
    SDL_BlitScaled(text_surface, NULL, dbg_win->surface, &stretchRect);
    SDL_UpdateWindowSurface(dbg_win->window);
    //free text surface
    SDL_FreeSurface(text_surface);

    return 0;
}

// debug debug interface
struct dbg_window dbg_win;
void debug_window_init()
{
    init_dbg_window(&dbg_win);
}
void debug_window_update()
{
    draw_dbg_window(&dbg_win);
}




