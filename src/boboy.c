#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>

#include "cart.h"
#include "cpu.h"
#include "gpu.h"
#include "timer.h"
#include "sdl.h"
#include "mem.h"


#define PROJECT_NAME "boboy"
#define VERSION_STR  "0.0.1"


//placeholder externs before we include them properly
extern void debug_window_init();
extern void debug_window_update();

int main(int argc, char **argv)
{
    char *rom_path = NULL;
    uint32_t next_cycle = 0;
    int debug = 0;
    int breakpoints = 0;
    int dissamble = 0;
    int step = 0;
    int disassemble = 0;
    int opt;
    struct gbc_cart *cart;

    printf("%s. ver.%s\n", PROJECT_NAME, VERSION_STR);
    // implement command line options using getopt
    // -d for debug mode
    // -b for breakpoints enabled
    // -S for dissasembly output enabled
    // -s for step mode
    while ((opt = getopt(argc, argv, "dbsS")) != -1) {
        switch (opt) {
        case 'd':
            debug = 1;
            break;
        case 'b':
            breakpoints = 1;
            break;
        case 's':
            step = 1;
            break;
        case 'S':
            disassemble = 1;
            break;
        default:
            fprintf(stderr, "Usage: %s [-dbsS] [romfile]", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    // if there is a rom file specified, load it
    // otherwise, print usage and exit
    // TODO: implement a breakpoint mode
    if (optind < argc) {
        rom_path = argv[optind];
    } else {
        fprintf(stderr, "Usage: %s [-dbsS] [romfile]", argv[0]);
        exit(EXIT_FAILURE);
    }

    // load the rom file
    cart = load_rom(rom_path);
    // DEBUG: print the rom header
    if (cart) print_cart_hdr(&cart->rom.bank0.hdr);

    // opens the SDL window
    init_window();
    // initialize the cpu
    cpu_reset();
    // initialize the ppu
    gpu_init();

    //maps the rom to the memory
    mem_mmap(cart);

    // run the cpu in normal mode
    if (!debug) {
        while (1) {
            timer_tick();
            if (!gpu_cycle()) {
                poll_events();
                render_tilemap();
            }

            if (!next_cycle) next_cycle = cpu_cycle();

            next_cycle--;
        }
    } else if (debug) {
        debug_window_init();
        // run the cpu in debug mode
        while (1) {
            timer_tick();
            if (!gpu_cycle()) {
                poll_events();
                //render_tilemap();
            }

            if (!next_cycle) next_cycle = cpu_cycle();

            next_cycle--;
            debug_window_update();
            // wait for a keypress after each instruction
            //getchar();
        }
    }
    return 0;
}
