#ifndef __GPU_H__
#define __GPU_H__

unsigned gpu_cycle();
void gpu_init();

void assemble_bg_map();
void get_bg_map(void *dest);
#endif __GPU_H__
