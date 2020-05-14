#include "cart.h"

//General Memory Map
//  0000-3FFF   16KB ROM Bank 00     (in cartridge, fixed at bank 00)
//  4000-7FFF   16KB ROM Bank 01..NN (in cartridge, switchable bank number)
//  8000-9FFF   8KB Video RAM (VRAM) (switchable bank 0-1 in CGB Mode)
//  A000-BFFF   8KB External RAM     (in cartridge, switchable bank, if any)
//  C000-CFFF   4KB Work RAM Bank 0 (WRAM)
//  D000-DFFF   4KB Work RAM Bank 1 (WRAM)  (switchable bank 1-7 in CGB Mode)
//  E000-FDFF   Same as C000-DDFF (ECHO)    (typically not used)

//  FE00-FE9F   Sprite Attribute Table (OAM)
//  FEA0-FEFF   Not Usable
//  FF00-FF7F   I/O Ports
//  FF80-FFFE   High RAM (HRAM)
//  FFFF        Interrupt Enable Register
struct mem_map {
  uint8_t rom_bank00[0x4000];
  uint8_t rom_bank01[0x4000];
  uint8_t vram[0x2000];
  uint8_t extram[0x2000];
  uint8_t wram0[0x1000];
  uint8_t wram1[0x1000];
  uint8_t echo[0x1E00];
  uint8_t sat[0xA0];
  uint8_t nc[0x60];
  uint8_t io[0x80];
  uint8_t hram[0x7F];
  uint8_t ier[0x01];
};

#define OAM_DMA 0xFF46
#define SERIAL_SB 0xFF01
#define SERIAL_SC 0xFF02

struct mem_map map;
struct gbc_cart *cart = 0;
uint8_t mem_read(uint16_t addr)
{
  //TODO add some checks
  uint8_t *data = (uint8_t*)&map;
  return data[addr];
}

uint16_t mem_read16(uint16_t addr)
{
  //TODO add some checks
  uint16_t result;
  uint8_t *data = (uint8_t*)&map;
  result = data[addr]+(data[addr+1]<<8);
  return result;
}

void mem_write(uint16_t addr, uint8_t d)
{
  //TODO add some checks
  //only spesific areas should be writable, 
  //and some areas should trigger bank changes etc.
  uint8_t *data = (uint8_t*)&map;
  if (addr == OAM_DMA) {
      memcpy(data+0XFE00, data+((uint16_t)(d)<<8),0xF1);
  } if (addr == SERIAL_SC) {
    printf("%c",data+SERIAL_SB);
    return;
  }
  data[addr] = d;
}

void mem_write16(uint16_t addr, uint16_t d)
{
  //TODO add some checks
  //only spesific areas should be writable, 
  //and some areas should trigger bank changes etc.
  uint16_t *p;
  uint8_t *data = (uint8_t*)&map;
  data += addr; 
  p = (uint16_t *)data; //not sure are unaligned access ok
  p[0] = d;
}

void mem_mmap(struct gbc_cart *cartridge) 
{
    cart = cartridge;
    memcpy(map.rom_bank00, &cart->rom.bank0, 0x4000);
    memcpy(map.rom_bank01, &cart->rom.bank[1], 0x4000);

}
void mem_munmap() 
{
    cart = 0;
}
