#include <stddef.h>
#include <stdint.h>

#include "timer.h"
#include "mem.h"
#include "cpu.h"

#define DIVIDER_COUNT 0xFF04
#define TIM_TIMA      0xFF05
#define TIM_TMA       0xFF06
#define TIM_TAC       0xFF07

#define TIM_ENABLE   0x04
#define TIM_DIV_MASK 0x03
#define TIM_DIV16    0x01
#define TIM_DIV64    0x10
#define TIM_DIV256   0x11
#define TIM_DIV1024  0x00

// Internal counters for timers
// div and tim counters should be internally connected
// and include glitch when resetting div but this implementation does not have
// that functionality yet.
struct tim_cnt {
    uint8_t div_count; //8 bit up counter that updates 0xFF04 on overflow
    uint16_t tim_count;
    uint16_t tim_arr;
};

struct tim_cnt tim = {
    .div_count = 0,
    .tim_count = 0,
    .tim_arr = 0,
};

// This could be improved to have less function calls to mem handler
// by having event based change system for 0xFF0X peripheral addresses
// now we poll 4 million times per second when timer is enabled...
void timer_tick()
{
    uint8_t tac = mem_read(TIM_TAC);
    if (++tim.div_count) {
        uint8_t div = mem_read(DIVIDER_COUNT);
        mem_write(DIVIDER_COUNT, ++div);
    }
    if (tac & TIM_ENABLE) {
        switch (tac & TIM_DIV_MASK) {
        case TIM_DIV1024:
            tim.tim_arr = 1024;
            break;
        case TIM_DIV16:
            tim.tim_arr = 16;
            break;
        case TIM_DIV64:
            tim.tim_arr = 64;
            break;
        case TIM_DIV256:
            tim.tim_arr = 256;
            break;
        }
        if (++tim.tim_count >= tim.tim_arr) {
            uint8_t tima = mem_read(TIM_TIMA);
            if (tima == 0xFF) {
                uint8_t tma = mem_read(TIM_TMA);
                mem_write(TIM_TMA, ++tma);
                irq_request(0x50);
            }
            mem_write(TIM_TIMA, ++tima);
            tim.tim_count = 0;
        }
    }
}
