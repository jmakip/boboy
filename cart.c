#include "cart.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

struct gbc_cart *load_rom(const char *path)
{
    struct stat st;
    int fd;
    struct gbc_cart *cart = 0;

    if ((fd = open(path, O_RDONLY)) < 0) return 0;
    if (fstat(fd, &st)) {
        close(fd);
        return 0;
    }

    if ((cart = malloc(sizeof(struct gbc_cart)))) {
        memset(cart, 0xFF, sizeof(struct gbc_cart));
        read(fd, cart->rom.bank, st.st_size);
    }

    close(fd);
    return cart;
}

void print_cart_hdr(struct cart_hdr *hdr)
{
    struct gbc_cart_hdr *gbc_hdr = hdr;
    if (!hdr) return;

    printf("entry_point: %02X %02X %02X %02X\n", hdr->entry_point[0],
           hdr->entry_point[1], hdr->entry_point[2], hdr->entry_point[3]);
    printf("logo: \n");
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 16; j++)
            printf("%02X ", hdr->logo[i + j]);
        printf("\n");
    }
    printf("title: %s\n", hdr->title);
    printf("rom_size: %02X\n", hdr->rom_size);
    printf("ram_size: %02X\n", hdr->ram_size);
    printf("region: %02X\n", hdr->region);
    printf("CGB: %02X\n", gbc_hdr->CGB);
    printf("SGB: %02X\n", hdr->SGB);
    printf("type: %02X\n", hdr->type);
}
