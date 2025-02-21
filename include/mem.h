#ifndef __MEM_H__
#define __MEM_H__

#include <stdint.h>
#include "cart.h"

void mem_mmap(struct gbc_cart *cartridge);
void mem_munmap();

//cpu connects with these
uint8_t mem_read(uint16_t addr);
uint16_t mem_read16(uint16_t addr);
void mem_write(uint16_t addr, uint8_t d);
void mem_write16(uint16_t addr, uint16_t d);

void dump_mem();
uint8_t read_joystick();

#endif
