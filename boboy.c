#include <stdio.h>
#include <stddef.h>
#include <stdint.h>


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define PROJECT_NAME "boboy"
#define VERSION_STR "0.0.1"

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


void* load_rom(const char *path)
{
  struct stat st;
  int fd;
  uint8_t *rom = 0;

  if ((fd = open(path, O_RDONLY))<0) return 0;
  if (fstat(fd, &st)) { 
    close(fd);
    return 0;
  }

  if ((rom = malloc(st.st_size))) 
      read(fd, rom, st.st_size);

  close(fd);
  return rom;
}

void print_cart_hdr(struct cart_hdr *hdr) 
{
  struct gbc_cart_hdr *gbc_hdr = hdr;
  if (!hdr) return;

  printf("entry_point: %02X %02X %02X %02X\n", *hdr->entry_point, *hdr->entry_point+1, 
      *hdr->entry_point+2, *hdr->entry_point+3);
  printf("logo: \n");
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 16; j++)
      printf("%02X ", hdr->logo[i+j]);
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

int main(int argc, char **argv) {
    void *rom;
    printf("%s. ver.%s\n", PROJECT_NAME, VERSION_STR);
    if (argc < 2) {
        printf("%s [FILE]\n", argv[0]);
        return 1;
    }
    rom = load_rom(argv[argc-1]);
    if (rom) print_cart_hdr((struct cart_hdr*)(rom+0x100));
    return 0;
}
