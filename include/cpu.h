#ifndef __CPU_H_
#define __CPU_H_

void cpu_reset();

void cpu_cycle();

void irq_request(uint8_t irq);

#endif
