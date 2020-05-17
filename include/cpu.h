#ifndef __CPU_H_
#define __CPU_H_

void cpu_reset();

unsigned cpu_cycle();

void irq_request(uint8_t irq);

#endif
