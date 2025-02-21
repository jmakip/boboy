#ifndef __CART_H_
#define __CART_H_

#include <stddef.h>
#include <stdint.h>

struct cart_hdr {
    uint8_t entry_point[4]; //0x0100-0x0103
    uint8_t logo[0x30]; //0x0104-0x0133
    uint8_t title[0x10]; //0x0134-0x0143
    //uint8_t manufacturer[0x4]; //0x013f-0x0142
    //uint8_t CGB; //0x0143
    uint8_t licensee[0x2]; //0x0144-0x0145
    uint8_t SGB; //0x0146;
    uint8_t type; //0x0147;
    uint8_t rom_size; //0x0148
    uint8_t ram_size; //0x0149
    uint8_t region; //0x014A
    uint8_t licensee_old; //0x014B
    uint8_t rom_version; //0x014C
    uint8_t hdr_sum; //0x014D
    uint8_t gl_sum[0x2]; //0x014E-0x014F
};

struct gbc_cart_hdr {
    uint8_t entry_point[4]; //0x0100-0x0103
    uint8_t logo[0x30]; //0x0104-0x0133
    uint8_t title[0x0B]; //0x0134-0x0143
    uint8_t manufacturer[0x4]; //0x013f-0x0142
    uint8_t CGB; //0x0143
    uint8_t licensee[0x2]; //0x0144-0x0145
    uint8_t SGB; //0x0146;
    uint8_t type; //0x0147;
    uint8_t rom_size; //0x0148
    uint8_t ram_size; //0x0149
    uint8_t region; //0x014A
    uint8_t licensee_old; //0x014B
    uint8_t rom_version; //0x014C
    uint8_t hdr_sum; //0x014D
    uint8_t gl_sum[0x2]; //0x014E-0x014F
};

struct rom_bank {
    uint8_t data[0x4000];
};

struct rom_bank0 {
    uint8_t bank_start[0x100];
    struct cart_hdr hdr;
    uint8_t bank_end[0x7EB0];
};

struct ram_bank {
    uint8_t data[0x2000];
};
union rom_banks {
    struct rom_bank0 bank0;
    struct rom_bank bank[256]; //1-256 depending variant
};

union ram_banks {
    struct ram_bank bank[4]; //1-4 depending variant
};

struct gbc_cart {
    union rom_banks rom;
    union ram_banks ram;
};

//loads rom file at path, allocates gbc_cart, returns rom file copied to gbc_cart
struct gbc_cart *load_rom(const char *path);

//copies .sav file into gbc_cart ram region
//unsigned load_nvram(gbc_cart *cart, const char *path);
void print_cart_hdr(struct cart_hdr *hdr);

#endif
