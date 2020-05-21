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

//bg map can change between scanlines since LCDC_CTRL can be written any time
//and SCY SCX can be changed between scanlines

uint32_t bg_map[65536];

//bg map can change between scanlines since LCDC_CTRL can be written any time
//and SCY SCX can be changed between scanlines
//but lets do just simple version
uint16_t tile_data_offset(uint16_t mapping, uint16_t block, uint8_t index)
{
    uint8_t offset;
    offset = mem_read(mapping + index);
    //    printf("index %d, mapping %04X, block %04X offset %d\n",index, mapping,block,offset);

    if (block == 0x8000)
        return 0x8000 + offset * 16;
    else if (offset > 127)
        return 0x8800 + (offset - 127) * 16;
    else
        return 0x9000 + offset * 16;
}

void get_bg_map(void *dest)
{
    memcpy(dest, bg_map, 65536 * 4);
}
void assemble_bg_map()
{
    uint8_t ty, tx, bg_y, bg_x;
    uint16_t bg_width = 256;
    uint16_t index = 0;
    uint16_t data_addr;
    uint16_t mapping_addr;
    uint8_t lcdc = mem_read(LCDC_CTRL);
    printf("ldcd %02X \n", lcdc);
    data_addr = ((lcdc & BG_WDW_DATA_SEL)) ? 0x8000 : 0x8800;
    mapping_addr = ((lcdc & BG_TILE_SEL)) ? 0x9C00 : 0x9800;
    if (!(lcdc & BG_WDW_PRIO)) return;

    for (bg_y = 0; bg_y < 32; bg_y++) {
        for (bg_x = 0; bg_x < 32; bg_x++) {
            uint16_t tile_offset =
                tile_data_offset(mapping_addr, data_addr, index);
            for (ty = 0; ty < 8; ty++) {
                int i;
                uint32_t pix8[8];
                uint16_t dot = mem_read16(tile_offset + 2 * ty);
                for (i = 0; i < 8; i++) {
                    pix8[i] = ((dot & (0x80 >> i)) >> (7 - i)) * 32;
                    pix8[i] |= ((dot & (0x8000 >> i)) >> 14 - i) * 32;
                    pix8[i] += (pix8[i] << 16) + (pix8[i] << 8);
                    pix8[i] ^= 0x00FFFFFF;
                }
                memcpy(
                    &bg_map[(8 * bg_x + ty * bg_width) + bg_y * bg_width * 8],
                    pix8, 8 * 4);
            }
            index++;
        }
    }
}

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
    usleep(nano / 1000);
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
    uint8_t lyc = mem_read(LCDC_LYC);
    uint8_t lcdc = mem_read(LCDC_CTRL);
    scanline = mem_read(LCDC_LY);
    if (!(lcdc & LCD_ENABLE)) return 1;

    if (++v_dot >= 455) {
        v_dot = 0;
        if (++scanline > 153) {
            scanline = 0;
            if (diff < 16750000LL) nsleep(diff);
        }
    }

    if (scanline >= 144) {
        if (scanline == 144) {
            //if (stat & STAT_V_IE) irq_request(0x40);
            //V_BLANK should trigger both 0x40 and 0x48 interrupts?
            irq_request(0x40);
            if (stat & STAT_V_IE) irq_request(0x48);
        }
        stat = (stat & 0xFC) | V_BLANK;

        //call_actual_draw();
    } else if (!v_dot) {
        if (stat & STAT_OAM_IE) irq_request(0x48);
        stat = (stat & 0xFC) | OAM;
    } else if (v_dot == 80) {
        stat = (stat & 0xFC) | OAM_VRAM;
    } else if (v_dot == 248) {
        if (stat & STAT_H_IE) irq_request(0x48);
        stat = (stat & 0xFC) | H_BLANK;
    }
    if (lyc == scanline) {
        stat |= STAT_COINC;
        if (stat & STAT_COINC_IE) irq_request(0x48);
    } else
        stat &= ~(STAT_COINC);

    mem_write(LCDC_STAT, stat);
    mem_write(LCDC_LY, scanline);

    if (scanline == 143 && v_dot == 200) return 0;

    return 1;
}
