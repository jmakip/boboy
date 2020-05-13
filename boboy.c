#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#include "cart.h"
#include "cpu.h"
#include "gpu.h"

#define PROJECT_NAME "boboy"
#define VERSION_STR "0.0.1"

int main(int argc, char **argv) {
    struct gbc_cart *cart;
    printf("%s. ver.%s\n", PROJECT_NAME, VERSION_STR);
    if (argc < 2) {
        printf("%s [FILE]\n", argv[0]);
        return 1;
    }
    cart = load_rom(argv[argc-1]);
    if (cart) print_cart_hdr(&cart->rom.bank0.hdr);

    cpu_reset();
    gpu_init();
    mem_mmap(cart);
    while(1) {
        gpu_cycle();
        cpu_cycle(); 

    }
    return 0;
}
