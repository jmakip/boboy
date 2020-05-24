#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#include "cart.h"
#include "cpu.h"
#include "gpu.h"
#include "timer.h"

#define PROJECT_NAME "boboy"
#define VERSION_STR  "0.0.1"

int main(int argc, char **argv)
{
    struct gbc_cart *cart;
    uint32_t next_cycle = 0;
    printf("%s. ver.%s\n", PROJECT_NAME, VERSION_STR);
    if (argc < 2) {
        printf("%s [FILE]\n", argv[0]);
        return 1;
    }
    cart = load_rom(argv[argc - 1]);
    if (cart) print_cart_hdr(&cart->rom.bank0.hdr);

    init_window();
    cpu_reset();
    gpu_init();
    mem_mmap(cart);
    while (1) {
        timer_tick();
        if (!gpu_cycle()) {
            poll_events();
            render_tilemap();
        }

        if (!next_cycle) next_cycle = cpu_cycle();

        next_cycle--;
    }
    return 0;
}
