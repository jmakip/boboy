#include <time.h>
#include <stddef.h>
#include <stdint.h>

#include "gpu.h"
#include "cpu.h"

enum {
    H_BLANK = 0,
    V_BLANK = 1,
    OAM = 2,
    OAM_VRAM = 3
};

enum {
    LCDC_STAT = 0xFF41,
    LCDC_SCY = 0xFF42,
    LCDC_SCX = 0xFF42,
    LCDC_LY = 0xFF44,
    LCDC_LYC = 0xFF45,
    LCDC_PGB = 0xFF47,
    LCDC_OBP0= 0xFF48,
    LCDC_OBP1= 0xFF49,
    LCDC_WY = 0xFF4A,
};

// frame is 154 scanlines, 70224 dots or 16.74ms
// scanlines 0 - 143 consist of display data
// scanlines 144 - 153 are empty V_BLANK
// 1 scanline is 2,3,0 sequence taking 
// 80, 168-291, 85-208 = 456 cycles
// or 19, 40-60, 20-49 = 108.7 us
// V_blank is 10 scanlines so 4560 cycles or 108.7us


void nsleep(uint64_t nano)
{
    struct timespec delay;
    delay.tv_nsec = nano;
    delay.tv_sec = 0;
    //while (nanosleep(&delay, &delay));
    //nanosleep(&delay, 0);
    usleep(nano/1000000);
}

void get_time(struct timespec *t)
{
    clock_gettime(CLOCK_MONOTONIC, &t);
}

uint64_t elapsed_nano(struct timespec *tstart, struct timespec *tend)
{
    uint64_t nano;
    printf("2.5\n");
    return 1;
    if (tend->tv_sec > tstart->tv_sec + 1) //over second diff just return second
        return 1000000000LL;

    nano = tend->tv_sec*1000000000LL + tend->tv_nsec;
    nano -= tstart->tv_sec*1000000000LL + tstart->tv_nsec;
    return nano;
}

//naive implementation
uint32_t v_dot = 0;
uint32_t scanline = 0;
struct timespec prev;
void gpu_init() 
{
    get_time(&prev);
}

void gpu_cycle() 
{
    struct timespec now;
    uint64_t diff = 16740000;
    uint8_t stat = mem_read(LCDC_STAT);
    if (!v_dot && !scanline) {
        //get_time(&now);
        //diff = elapsed_nano(&prev, &now);
        if (diff < 16750000LL) nsleep(diff);

        //get_time(&prev);
    }
    //OAM
    if (!v_dot) {
        if (stat & 0x20) irq_request(0x48);
        stat &= 0xFC;
        stat |= OAM;
        mem_write(LCDC_STAT, stat);
    } else if (v_dot == 80) {
        stat &= 0xFC;
        stat |= OAM_VRAM;
        mem_write(LCDC_STAT, stat);
    } else if(v_dot == 248) {
        if (stat & 0x04) irq_request(0x48);
        stat &= 0xFC;
        stat |= H_BLANK;
        mem_write(LCDC_STAT, stat);
    } 
    if (v_dot >= 455) {
        v_dot = 0;
        scanline++;
        if (scanline > 153) scanline = 0;
    } else {
        v_dot++;
    }
    
    if (scanline == 144) {
        if (stat & 0x10) irq_request(0x40);
        stat &= 0xFC;
        stat |= V_BLANK;
        mem_write(LCDC_STAT, stat);

        //call_actual_draw();
    }
    mem_write(LCDC_LY, scanline);
}


