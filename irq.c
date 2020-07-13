

void irq_request(uint8_t irq)
{
    uint8_t req = mem_read(0xFF0F);
    switch (irq) {
    case 0x40:
        req |= 0x01;
        break;
    case 0x48:
        req |= 0x02;
        break;
    case 0x50:
        req |= 0x04;
        break;
    case 0x58:
        req |= 0x08;
        break;
    case 0x60:
        req |= 0x10;
        break;
    default:
        break;
    }
    mem_write(0xFF0F, req);
}

//interrupt service routine
//
// IME is set to 0
// two wait states are executed NOP NOP 4+4cycles
// current PC is pushed to stack  8 cycles
// PC is set to 0x00(irq) 4 cycle
//
//
unsigned start_isr(uint8_t irq)
{
    //mem_write(cpu.SP, cpu.PC)
    //cpu.SP++;
    //cpu.PC = 0x0000 + irq;
    return 20;
}
