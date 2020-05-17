#include <time.h>
#include <stddef.h>
#include <stdint.h>

#include "gpu.h"
#include "cpu.h"

enum {
    LCDC_CTRL = 0xFF40,
    LCDC_STAT = 0xFF41,
    LCDC_SCY = 0xFF42,
    LCDC_SCX = 0xFF42,
    LCDC_LY = 0xFF44,
    LCDC_LYC = 0xFF45,
    LCDC_PGB = 0xFF47,
    LCDC_OBP0 = 0xFF48,
    LCDC_OBP1 = 0xFF49,
    LCDC_WY = 0xFF4A,
};

//CTRL register bits
#define LCD_ENABLE      0x80
#define WDW_TILE_SEL    0x40 // 0=9800, 1=9c00
#define WDW_ENABLE      0x20
#define BG_WDW_DATA_SEL 0x10 // 0=8800, 1=8000
#define BG_TILE_SEL     0x08 // 0=9800, 1=9c000
#define SPRITE_SIZE     0x04 // 0=8x8, 1=8x16
#define SPRITE_ENABLE   0x02
#define BG_WDW_PRIO     0x01

//STAT register bits
#define STAT_MODE     0x03 // Current state
#define STAT_COINC    0x04 // Set when LYC==LY
#define STAT_H_IE     0x08 // H_BLANK irq enable
#define STAT_V_IE     0x10 // V_BLANK irq enable
#define STAT_OAM_IE   0x20 // OAM irq enable
#define STAT_COINC_IE 0x40 // LYC==LY irq enable

//STAT_MODE BITS
enum { H_BLANK = 0, V_BLANK = 1, OAM = 2, OAM_VRAM = 3 };

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
    usleep(nano / 1000000);
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

    nano = tend->tv_sec * 1000000000LL + tend->tv_nsec;
    nano -= tstart->tv_sec * 1000000000LL + tstart->tv_nsec;
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

//Render buffer should be updated pixel at time since LCDC_CTRL can change even between
//lines and pixels
unsigned gpu_cycle()
{
    struct timespec now;
    uint64_t diff = 16740000;
    uint8_t stat = mem_read(LCDC_STAT);
    uint8_t lyc = mem_read(LCDC_STAT);
    scanline = mem_read(LCDC_LY);
    if (!v_dot && !scanline) {
        //get_time(&now);
        //diff = elapsed_nano(&prev, &now);
        if (diff < 16750000LL) nsleep(diff);

        //get_time(&prev);
    }
    if (lyc == scanline) stat |= 0x04;
    else stat &= 0xFB;

    //OAM
    if (!v_dot) {
        if (stat & STAT_OAM_IE) irq_request(0x48);
        stat = (stat & 0xFC) | OAM;
    } else if (v_dot == 80) {
        stat = (stat & 0xFC) | OAM_VRAM;
    } else if (v_dot == 248) {
        if (stat & STAT_H_IE) irq_request(0x48);
        stat = (stat & 0xFC) | H_BLANK;
    }
    if (v_dot >= 455) {
        v_dot = 0;
        scanline++;
        if (scanline > 153) scanline = 0;
    } else {
        v_dot++;
    }

    if (scanline == 144) {
        if (stat & STAT_V_IE) irq_request(0x40);
        stat = (stat & 0xFC) | V_BLANK;

        //call_actual_draw();
    }
    mem_write(LCDC_STAT, stat);
    mem_write(LCDC_LY, scanline);
    return v_dot+scanline;
}
