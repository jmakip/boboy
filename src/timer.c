#include <stddef.h>
#include <stdint.h>

#include "timer.h"
#include "mem.h"
#include "cpu.h"

#define DIVIDER_COUNT 0xFF04u
#define TIM_TIMA      0xFF05u
#define TIM_TMA       0xFF06u
#define TIM_TAC       0xFF07u

#define TIM_ENABLE   0x04u
#define TIM_DIV_MASK 0x03u
#define TIM_DIV16    0x01u
#define TIM_DIV64    0x02u
#define TIM_DIV256   0x03u
#define TIM_DIV1024  0x00u

// Internal counters for timers
// div and tim counters should be internally connected
// and include glitch when resetting div but this implementation does not have
// that functionality yet.
struct tim_cnt {
    uint32_t div_count; //22bit up counter,
    uint16_t tim_count;
    uint16_t tim_arr;
};


// This could be improved to have less function calls to mem handler
// by having event based change system for 0xFF0X peripheral addresses
// now we poll 4 million times per second when timer is enabled...
void timer_tick()
{
    static struct tim_cnt tim = {
        .div_count = 0,
        .tim_count = 0,
        .tim_arr = 0,
    };
    uint8_t tac = mem_read(TIM_TAC);

    tim.div_count = (tim.div_count + 1u) & 0x3FFFFFu;
    mem_write(DIVIDER_COUNT, (uint8_t)((tim.div_count >> 14) & 0xFFu));

    if ((tac & TIM_ENABLE) != NULL) {
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
        default:
            //dead code
            break;
        }
        if (++tim.tim_count >= tim.tim_arr) {
            uint8_t tima = mem_read(TIM_TIMA);
            if (tima == 0xFFu) {
                uint8_t tma = mem_read(TIM_TMA);
                mem_write(TIM_TMA, ++tma);
                irq_request(0x50);
            }
            mem_write(TIM_TIMA, ++tima);
            tim.tim_count = 0;
        }
    }
}
