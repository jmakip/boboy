#ifndef __IRQ_H__
#define __IRQ_H__
#include <stdint.h>

void irq_request(uint8_t irq);
unsigned start_isr(uint8_t irq);

#endif
