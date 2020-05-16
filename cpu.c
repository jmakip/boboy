#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

//suppress warnings when variable is not used
#define UNUSED(x) (void)(x)
#define FLAG_Z 0x80
#define FLAG_N 0x40
#define FLAG_H 0x20
#define FLAG_C 0x10

struct gb_reg {
  uint16_t AF; //accu+flag
  uint16_t BC;
  uint16_t DE;
  uint16_t HL; 
  uint16_t SP; //Stack Pointer
  uint16_t PC; //Program counter
};

struct gb_reg8 {
  uint8_t F; //flag
  uint8_t A; //accu
  uint8_t C;
  uint8_t B;
  uint8_t E;
  uint8_t D;
  uint8_t L;
  uint8_t H;
  uint16_t SP; //Stack Pointer
  uint16_t PC; //Program counter
};


struct op_code {
  uint8_t op;
  uint8_t width; //if op is more than 1bytes
  unsigned (*op_call) (uint8_t op0, uint8_t op1, uint8_t op2);
  const char *symbol; //for debug

};

struct gb_reg cpu;
uint32_t ime = 0;

unsigned nop(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  return 4;
}

unsigned halt(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  //TODO add some flag to sleep on
  return 4;
}
unsigned stop(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  //TODO add some flag to sleep on
  return 4;
}

//TODO interrupts should be disabled only instruction after this.
//but perhaps most games work without being cycle perfect
unsigned di(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  ime = 0;
  return 4;
}
unsigned ei(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  ime = 1;
  return 4;
}

unsigned xor_a(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A = r8->A ^ r8->A;
  r8->F = FLAG_Z;
  return 4;
}
unsigned cpl_a(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A = r8->A ^ 0xFF;
  r8->F |= (FLAG_N|FLAG_H);
  return 4;
}
unsigned ccf(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->F ^= FLAG_C;
  r8->F &= (FLAG_C|FLAG_Z);
  return 4;
}
/*
 *  INC A 3C 4
 *  INC B 04 4
 *  INC C 0C 4
 *  INC D 14 4
 *  INC E 1C 4
 *  INC H 24 4
 *  INC L 2C 4
 *  INC (HL) 34 12
 *  FLAGS
 *  Z - Set if result is zero.
 *  N - Reset.
 *  H - Set if carry from bit 3.
 *  C - Not affected.
 */
unsigned inc_a(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint8_t v = r8->A + 1;
  r8->F &= FLAG_C;
  if (!v) r8->F |= FLAG_Z;
  else if (((r8->A & 0x0F) + 1) & 0xF0) r8->F |= FLAG_H;

  r8->A = v;
  return 4;
}
unsigned inc_b(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint8_t v = r8->B + 1;
  r8->F &= FLAG_C;
  if (!v) r8->F |= FLAG_Z;
  else if (((r8->B & 0x0F) + 1) & 0xF0) r8->F |= FLAG_H;

  r8->B = v;
  return 4;
}
unsigned inc_c(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint8_t v = r8->C + 1;
  r8->F &= FLAG_C;
  if (!v) r8->F |= FLAG_Z;
  else if (((r8->C & 0x0F) + 1) & 0xF0) r8->F |= FLAG_H;

  r8->C = v;
  return 4;
}

unsigned inc_d(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint8_t v = r8->D + 1;
  r8->F &= FLAG_C;
  if (!v) r8->F |= FLAG_Z;
  else if (((r8->D & 0x0F) + 1) & 0xF0) r8->F |= FLAG_H;

  r8->D = v;
  return 4;
}
unsigned inc_e(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint8_t v = r8->E + 1;
  r8->F &= FLAG_C;
  if (!v) r8->F |= FLAG_Z;
  else if (((r8->E & 0x0F) + 1) & 0xF0) r8->F |= FLAG_H;

  r8->E = v;
  return 4;
}
unsigned inc_h(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint8_t v = r8->H + 1;
  r8->F &= FLAG_C;
  if (!v) r8->F |= FLAG_Z;
  else if (((r8->H & 0x0F) + 1) & 0xF0) r8->F |= FLAG_H;

  r8->H = v;
  return 4;
}
unsigned inc_l(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint8_t v = r8->L + 1;
  r8->F &= FLAG_C;
  if (!v) r8->F |= FLAG_Z;
  else if (((r8->L & 0x0F) + 1) & 0xF0) r8->F |= FLAG_H;

  r8->L = v;
  return 4;
}
unsigned inc_mem(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  uint8_t v = mem_read(cpu.HL);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->F &= FLAG_C;
  if (((v++ & 0x0F) + 1) & 0xF0) r8->F |= FLAG_H;
  else if (!v) r8->F |= FLAG_Z;

  mem_write(cpu.HL, v);
  return 12;
}

/*
 DEC A 3D 4
 DEC B 05 4
 DEC C 0D 4
 DEC D 15 4
 DEC E 1D 4
 DEC H 25 4
 DEC L 2D 4
 DEC (HL) 35 12
*/
/* Z - Set if reselt is zero.
 N - Set.
 H - Set if no borrow from bit 4.
 C - Not affected.*/
unsigned dec_a(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint8_t i = r8->A & 0xF0;
  r8->A--;

  r8->F &= FLAG_C;
  r8->F |= FLAG_N;
  if ((i - 1) & 0x0F) r8->F |= FLAG_H;
  if (!r8->A) r8->F |= FLAG_Z;

  return 4;
}

unsigned dec_b(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint8_t i = r8->B & 0xF0;
  r8->B--;

  r8->F &= FLAG_C;
  r8->F |= FLAG_N;
  if ((i - 1) & 0x0F) r8->F |= FLAG_H;
  if (!r8->B) r8->F |= FLAG_Z;

  return 4;
}

unsigned dec_c(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint8_t i = r8->C & 0xF0;
  r8->C--;
  r8->F &= FLAG_C;
  r8->F |= FLAG_N;
  if ((i - 1) & 0x0F) r8->F |= FLAG_H;
  if (!r8->C) r8->F |= FLAG_Z;

  return 4;
}

unsigned dec_d(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint8_t i = r8->D & 0xF0;
  r8->D--;
  r8->F &= FLAG_C;
  r8->F |= FLAG_N;
  if ((i - 1) & 0x0F) r8->F |= FLAG_H;
  if (!r8->D) r8->F |= FLAG_Z;

  return 4;
}

unsigned dec_e(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint8_t i = r8->E & 0xF0;
  r8->E--;

  r8->F &= FLAG_C;
  r8->F |= FLAG_N;
  if ((i - 1) & 0x0F) r8->F |= FLAG_H;
  if (!r8->E) r8->F |= FLAG_Z;

  return 4;
}

unsigned dec_h(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint8_t i = r8->H & 0xF0;
  r8->H--;

  r8->F &= FLAG_C;
  r8->F |= FLAG_N;
  if ((i - 1) & 0x0F) r8->F |= FLAG_H;
  if (!r8->H) r8->F |= FLAG_Z;

  return 4;
}

unsigned dec_l(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint8_t i = r8->L & 0xF0;
  r8->L--;
  r8->F &= FLAG_C;
  r8->F |= FLAG_N;
  if ((i - 1) & 0x0F) r8->F |= FLAG_H;
  if (!r8->L) r8->F |= FLAG_Z;

  return 4;
}

unsigned dec_hl(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint8_t val = mem_read(cpu.HL);
  uint8_t i = val & 0xF0;
  val--;
  r8->F &= FLAG_C;
  if ((i - 1) & 0x0F) r8->F |= FLAG_H;
  if (!val) r8->F |= FLAG_Z;
  r8->F |= FLAG_N;
  mem_write(cpu.HL, val);

  return 12;
}
unsigned dec16_bc(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
    cpu.BC--;
    return 8;
}
unsigned dec16_de(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
    cpu.DE--;
    return 8;
}

unsigned dec16_hl(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
    cpu.HL--;
    return 8;
}

unsigned dec16_sp(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
    cpu.SP--;
    return 8;
}
unsigned inc16_bc(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
    cpu.BC++;
    return 8;
}
unsigned inc16_de(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
    cpu.DE++;
    return 8;
}

unsigned inc16_hl(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
    cpu.HL++;
    return 8;
}

unsigned inc16_sp(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
    cpu.SP++;
    return 8;
}
/* jump if
 JR NZ,* 20 8
 JR Z,* 28 8
 JR NC,* 30 8
 JR C,* 38 8
 */
unsigned jump_nz(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  if (!(r8->F & FLAG_Z)) cpu.PC += (int8_t)op1;
  return 8;
}

unsigned jump_z(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  if ((r8->F & FLAG_Z)) cpu.PC += (int8_t)op1;
  return 8;
}

unsigned jump_c(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  if (r8->F & FLAG_C) cpu.PC += (int8_t)op1;
  return 8;
}

unsigned jump_nc(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  if (!(r8->F & FLAG_C)) cpu.PC += (int8_t)op1;
  return 8;
}
unsigned jr_n(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  cpu.PC += (int8_t)op1;
  return 8;
}

unsigned jump_c3(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  cpu.PC = (uint16_t)op1+(((uint16_t)op2)<<8);
  return 12;
}
unsigned rst(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
    UNUSED(op1);
    UNUSED(op2);
    mem_write16(cpu.SP, cpu.PC);
    cpu.SP -= 2;
    cpu.PC = 0x0000 + (op0-0xc7);
    return 24;
}
unsigned call(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
    UNUSED(op0);
    mem_write16(cpu.SP, cpu.PC);
    cpu.SP -= 2;
    cpu.PC = (uint16_t)op1+(((uint16_t)op2)<<8);
    return 12;
}
unsigned ret(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
    UNUSED(op0);
    UNUSED(op1);
    UNUSED(op2);
    cpu.PC = mem_read16(cpu.SP);
    cpu.SP += 2;
    return 8;
}
unsigned ret_nz(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
    UNUSED(op0);
    UNUSED(op1);
    UNUSED(op2);
    struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
    if (!(r8->F & FLAG_Z)) {
        cpu.PC = mem_read16(cpu.SP);
        cpu.SP += 2;
    }
    return 8;
}
unsigned ret_z(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
    UNUSED(op0);
    UNUSED(op1);
    UNUSED(op2);
    struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
    if ((r8->F & FLAG_Z)) {
        cpu.PC = mem_read16(cpu.SP);
        cpu.SP += 2;
    }
    return 8;
}
unsigned ret_nc(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
    UNUSED(op0);
    UNUSED(op1);
    UNUSED(op2);
    struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
    if (!(r8->F & FLAG_C)) {
        cpu.PC = mem_read16(cpu.SP);
        cpu.SP += 2;
    }
    return 8;
}
unsigned ret_c(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
    UNUSED(op0);
    UNUSED(op1);
    UNUSED(op2);
    struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
    if ((r8->F & FLAG_C)) {
        cpu.PC = mem_read16(cpu.SP);
        cpu.SP += 2;
    }
    return 8;
}

unsigned cb(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  return 8;
}

unsigned ld_im_b(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->B = op1;
  return 8;
}

unsigned ld_im_c(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->C = op1;
  return 8;
}

unsigned ld_im_d(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->D = op1;
  return 8;
}

unsigned ld_im_e(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->E = op1;
  return 8;
}

unsigned ld_im_h(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->H = op1;
  return 8;
}

unsigned ld_im_l(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->L = op1;
  return 8;
}

//same as NOP
unsigned ld_a(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A = r8->A;
  return 4;
}

// LD A,B
unsigned ld_b(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A = r8->B;
  return 4;
}

// LD A,C
unsigned ld_c(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A = r8->C;
  return 4;
}

// LD A,D
unsigned ld_d(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A = r8->D;
  return 4;
}

// LD A,E
unsigned ld_e(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A = r8->E;
  return 4;
}

// LD A,H
unsigned ld_h(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A = r8->H;
  return 4;
}

// LD A,L
unsigned ld_l(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A = r8->L;
  return 4;
}

// LD A,# //not sure about this
unsigned ld_im(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A = op1;
  return 4;
}

// LD A,(HL)
unsigned ld_mem(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A = mem_read(cpu.HL);
  return 8;
}
// LD A,(HL+)
unsigned ld_mem_inc(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A = mem_read(cpu.HL++);
  return 8;
}
// LD (HL+),A
unsigned ld_mem_inc_a(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  mem_write(cpu.HL++, r8->A);
  return 8;
}
// LD A,(BC)
unsigned ld_mem_bc(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A = mem_read(cpu.BC);
  return 8;
}
// LD A,(DE)
unsigned ld_mem_de(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A = mem_read(cpu.DE);
  return 8;
}
// LD (HL), n
unsigned ld_im_mem(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op2);
  mem_write(cpu.HL, op1);
  return 12;
}

// LD B, A
unsigned ld_b_a(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->B = r8->A;
  return 4;
}
// LD B, B
unsigned ld_b_b(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->B = r8->B;
  return 4;
}
// LD B, C
unsigned ld_b_c(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->B = r8->C;
  return 4;
}
// LD B, D
unsigned ld_b_d(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->B = r8->D;
  return 4;
}
// LD B, E
unsigned ld_b_e(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->B = r8->E;
  return 4;
}
// LD B, H
unsigned ld_b_h(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->B = r8->H;
  return 4;
}
// LD B, L
unsigned ld_b_l(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->B = r8->L;
  return 4;
}
// LD B, (HL)
unsigned ld_b_hl(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->B = mem_read(cpu.HL);
  return 4;
}

// LD C, A
unsigned ld_c_a(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->C = r8->A;
  return 4;
}
// LD C, B
unsigned ld_c_b(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->C = r8->B;
  return 4;
}
// LD C, C
unsigned ld_c_c(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->C = r8->C;
  return 4;
}
// LD C, D
unsigned ld_c_d(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->C = r8->D;
  return 4;
}
// LD C, E
unsigned ld_c_e(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->C = r8->E;
  return 4;
}
// LD C, H
unsigned ld_c_h(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->C = r8->H;
  return 4;
}
// LD C, L
unsigned ld_c_l(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->C = r8->L;
  return 4;
}
// LD C, (HL)
unsigned ld_c_mem(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->C = mem_read(cpu.HL);
  return 8;
}
// LD D, A
unsigned ld_d_a(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->D = r8->A;
  return 4;
}
// LD D, B
unsigned ld_d_b(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->D = r8->B;
  return 4;
}
// LD D, C
unsigned ld_d_c(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->D = r8->C;
  return 4;
}
// LD D, D
unsigned ld_d_d(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->D = r8->D;
  return 4;
}
// LD D, E
unsigned ld_d_e(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->D = r8->E;
  return 4;
}
// LD D, H
unsigned ld_d_h(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->D = r8->H;
  return 4;
}
// LD D, L
unsigned ld_d_l(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->D = r8->L;
  return 4;
}
// LD D,(HL)
unsigned ld_d_mem(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->D = mem_read(cpu.HL);
  return 8;
}
// LD E, A
unsigned ld_e_a(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->E = r8->A;
  return 4;
}
// LD E, B
unsigned ld_e_b(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->E = r8->B;
  return 4;
}
// LD E, C
unsigned ld_e_c(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->E = r8->C;
  return 4;
}
unsigned ld_e_d(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->E = r8->D;
  return 4;
}
unsigned ld_e_e(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->E = r8->E;
  return 4;
}
// LD E, H
unsigned ld_e_h(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->E = r8->H;
  return 4;
}
// LD E, L
unsigned ld_e_l(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->E = r8->L;
  return 4;
}
// LD E,(HL)
unsigned ld_e_mem(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->E = mem_read(cpu.HL);
  return 8;
}
// LD H, A
unsigned ld_h_a(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->H = r8->A;
  return 4;
}
// LD H, B
unsigned ld_h_b(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->H = r8->B;
  return 4;
}
// LD H, c
unsigned ld_h_c(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->H = r8->C;
  return 4;
}
// LD H, D
unsigned ld_h_d(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->H = r8->D;
  return 4;
}
// LD H, E
unsigned ld_h_e(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->H = r8->E;
  return 4;
}
// LD H, H
unsigned ld_h_h(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->H = r8->H;
  return 4;
}
// LD H, L
unsigned ld_h_l(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->H = r8->L;
  return 4;
}
// LD H,(HL)
unsigned ld_h_mem(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->H = mem_read(cpu.HL);
  return 8;
}
// LD L, A
unsigned ld_l_a(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->L = r8->A;
  return 4;
}
// LD L, B
unsigned ld_l_b(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->L = r8->B;
  return 4;
}
// LD L, C
unsigned ld_l_c(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->L = r8->C;
  return 4;
}
// LD L, D
unsigned ld_l_d(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->L = r8->D;
  return 4;
}
// LD L, E
unsigned ld_l_e(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->L = r8->E;
  return 4;
}
// LD L, H
unsigned ld_l_h(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->L = r8->H;
  return 4;
}
// LD L, L
unsigned ld_l_l(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->L = r8->L;
  return 4;
}
// LD L, (HL)
unsigned ld_l_mem(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->L = mem_read(cpu.HL);
  return 8;
}
// LD (BC), A
unsigned ld_bc_mem_a(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  mem_write(cpu.BC, r8->A);
  return 8;
}
// LD (DE), A
unsigned ld_de_mem_a(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  mem_write(cpu.DE, r8->A);
  return 8;
}
// LD (HL), A
unsigned ld_mem_a(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  mem_write(cpu.HL, r8->A);
  return 8;
}
// LD (HL), B
unsigned ld_mem_b(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  mem_write(cpu.HL, r8->B);
  return 8;
}
// LD (HL), C
unsigned ld_mem_c(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  mem_write(cpu.HL, r8->C);
  return 8;
}
// LD (HL), D
unsigned ld_mem_d(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  mem_write(cpu.HL, r8->D);
  return 8;
}
// LD (HL), E
unsigned ld_mem_e(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  mem_write(cpu.HL, r8->E);
  return 8;
}
// LD (HL), H
unsigned ld_mem_h(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  mem_write(cpu.HL, r8->H);
  return 8;
}
// LD (HL), L
unsigned ld_mem_l(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  mem_write(cpu.HL, r8->L);
  return 8;
}
// LD (nn), A
unsigned ld_im_mem_a(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  mem_write(((uint16_t)op1+((uint16_t)op2))<<8, r8->A);
  return 16;
}
// LD (nn), SP
unsigned ld_im_mem_sp(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  mem_write16(((uint16_t)op1+((uint16_t)op2))<<8, cpu.SP);
  return 20;
}

unsigned ld16_im_bc(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->B = op2;
  r8->C = op1;
  return 12;
}

unsigned ld16_im_de(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->D = op2;
  r8->E = op1;
  return 12;
}

unsigned ld16_im_hl(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->H = op2;
  r8->L = op1;
  return 12;
}

unsigned ld16_im_sp(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  cpu.SP = ((uint16_t)op1<<8)+op2;
  return 12;
}

// LDD (HL),A 32 8 
unsigned ldd_hl_a(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  mem_write(cpu.HL, r8->A);
  cpu.HL--;
  return 8;
}

unsigned ldd_a_hl(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A = mem_read(cpu.HL);
  cpu.HL--;
  return 8;
}

//Put A into memory address $FF00+n.
unsigned ldh_a_ff00(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  mem_write(0xff00+op1, r8->A);
  return 12;
}

//Put memory address $FF00+n into A
unsigned ldh_ff00_a(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A = mem_read(0xff00+op1);
  return 12;
}

//Put A into memory address $FF00+C.
unsigned ldh_ff00_C_a(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  mem_write(0xff00+r8->C, r8->A);
  return 8;
}

//Put memory address $FF00+C into A
unsigned ldh_a_ff00_C(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A = mem_read(0xff00+r8->C);
  return 8;
}

unsigned or_a(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A |= r8->A;
  r8->F = (r8->A) ? 0 : FLAG_Z;
  return 4;
}
unsigned or_b(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A |= r8->B;
  r8->F = (r8->A) ? 0 : FLAG_Z;
  return 4;
}
unsigned or_c(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A |= r8->C;
  r8->F = (r8->A) ? 0 : FLAG_Z;
  return 4;
}
unsigned or_d(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A |= r8->D;
  r8->F = (r8->A) ? 0 : FLAG_Z;
  return 4;
}
unsigned or_e(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A |= r8->E;
  r8->F = (r8->A) ? 0 : FLAG_Z;
  return 4;
}
unsigned or_h(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A |= r8->H;
  r8->F = (r8->A) ? 0 : FLAG_Z;
  return 4;
}
unsigned or_l(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A |= r8->L;
  r8->F = (r8->A) ? 0 : FLAG_Z;
  return 4;
}
unsigned or_mem(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A |= mem_read(cpu.HL);
  r8->F = (r8->A) ? 0 : FLAG_Z;
  return 8;
}
unsigned or_n(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A |= op1;
  r8->F = (r8->A) ? 0 : FLAG_Z;
  return 8;
}
unsigned and_a(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A &= r8->A;
  r8->F = (r8->A) ? FLAG_H : (FLAG_Z|FLAG_H);
  return 4;
}
unsigned and_b(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A &= r8->B;
  r8->F = (r8->A) ? FLAG_H : (FLAG_Z|FLAG_H);
  return 4;
}
unsigned and_c(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A &= r8->C;
  r8->F = (r8->A) ? FLAG_H : (FLAG_Z|FLAG_H);
  return 4;
}
unsigned and_d(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A &= r8->D;
  r8->F = (r8->A) ? FLAG_H : (FLAG_Z|FLAG_H);
  return 4;
}
unsigned and_e(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A &= r8->E;
  r8->F = (r8->A) ? FLAG_H : (FLAG_Z|FLAG_H);
  return 4;
}
unsigned and_h(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A &= r8->H;
  r8->F = (r8->A) ? FLAG_H : (FLAG_Z|FLAG_H);
  return 4;
}
unsigned and_l(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A &= r8->L;
  r8->F = (r8->A) ? FLAG_H : (FLAG_Z|FLAG_H);
  return 4;
}
unsigned and_mem(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A &= mem_read(cpu.HL);
  r8->F = (r8->A) ? FLAG_H : (FLAG_Z|FLAG_H);
  return 8;
}
unsigned and_n(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A &= op1;
  r8->F = (r8->A) ? FLAG_H : (FLAG_Z|FLAG_H);
  return 8;
}

 //TODO
 /* 
 CP A BF 4
 CP B B8 4
 CP C B9 4
 CP D BA 4
 CP E BB 4
 CP H BC 4
 CP L BD 4
 CP (HL) BE 8
 CP # FE 8
*/
// CP A
unsigned cp_a(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->F = FLAG_Z | FLAG_N;

  return 4;
}

unsigned cp_b(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->F = FLAG_N;
  if (r8->A < r8->B) r8->F |= FLAG_C;
  if (r8->A == r8->B) r8->F |= FLAG_Z;
  if (((r8->A & 0xF0) - r8->B) & 0x0F) r8->F |= FLAG_H;

  return 4;
}

unsigned cp_c(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->F = FLAG_N;
  if (r8->A < r8->C) r8->F |= FLAG_C;
  if (r8->A == r8->C) r8->F |= FLAG_Z;
  if (((r8->A & 0xF0) - r8->C) & 0x0F) r8->F |= FLAG_H;

  return 4;
}

unsigned cp_d(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->F = FLAG_N;
  if (r8->A < r8->D) r8->F |= FLAG_C;
  if (r8->A == r8->D) r8->F |= FLAG_Z;
  if (((r8->A & 0xF0) - r8->D) & 0x0F) r8->F |= FLAG_H;

  return 4;
}

unsigned cp_e(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->F = FLAG_N;
  if (r8->A < r8->E) r8->F |= FLAG_C;
  if (r8->A == r8->E) r8->F |= FLAG_Z;
  if (((r8->A & 0xF0) - r8->E) & 0x0F) r8->F |= FLAG_H;

  return 4;
}

unsigned cp_h(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->F = FLAG_N;
  if (r8->A < r8->H) r8->F |= FLAG_C;
  if (r8->A == r8->H) r8->F |= FLAG_Z;
  if (((r8->A & 0xF0) - r8->H) & 0x0F) r8->F |= FLAG_H;

  return 4;
}

unsigned cp_l(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->F = FLAG_N;
  if (r8->A < r8->L) r8->F |= FLAG_C;
  if (r8->A == r8->L) r8->F |= FLAG_Z;
  if (((r8->A & 0xF0) - r8->L) & 0x0F) r8->F |= FLAG_H;

  return 4;
}

unsigned cp_hl(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint8_t x = mem_read(cpu.HL);
  r8->F = FLAG_N;
  if (r8->A < x) r8->F |= FLAG_C;
  if (r8->A == x) r8->F |= FLAG_Z;
  if (((r8->A & 0xF0) - x) & 0x0F) r8->F |= FLAG_H;

  return 8;
}

unsigned cp_n(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint8_t x = op1;
  r8->F = FLAG_N;
  if (r8->A < x) r8->F |= FLAG_C;
  if (r8->A == x) r8->F |= FLAG_Z;
  if (((r8->A & 0xF0) - x) & 0x0F) r8->F |= FLAG_H;

  return 8;
}

unsigned sub_a(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->F = FLAG_Z | FLAG_N;
  r8->A = 0;//TODO could optimize by using AF

  return 4;
}

unsigned sub_b(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->F = FLAG_N;
  if (r8->A < r8->B) r8->F |= FLAG_C;
  if (r8->A == r8->B) r8->F |= FLAG_Z;
  if (((r8->A & 0xF0) - r8->B) & 0x0F) r8->F |= FLAG_H;
  r8->A -= r8->B;

  return 4;
}

unsigned sub_c(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->F = FLAG_N;
  if (r8->A < r8->C) r8->F |= FLAG_C;
  if (r8->A == r8->C) r8->F |= FLAG_Z;
  if (((r8->A & 0xF0) - r8->C) & 0x0F) r8->F |= FLAG_H;
  r8->A -= r8->C;

  return 4;
}

unsigned sub_d(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->F = FLAG_N;
  if (r8->A < r8->D) r8->F |= FLAG_C;
  if (r8->A == r8->D) r8->F |= FLAG_Z;
  if (((r8->A & 0xF0) - r8->D) & 0x0F) r8->F |= FLAG_H;
  r8->A -= r8->D;

  return 4;
}

unsigned sub_e(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->F = FLAG_N;
  if (r8->A < r8->E) r8->F |= FLAG_C;
  if (r8->A == r8->E) r8->F |= FLAG_Z;
  if (((r8->A & 0xF0) - r8->E) & 0x0F) r8->F |= FLAG_H;
  r8->A -= r8->E;

  return 4;
}

unsigned sub_h(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->F = FLAG_N;
  if (r8->A < r8->H) r8->F |= FLAG_C;
  if (r8->A == r8->H) r8->F |= FLAG_Z;
  if (((r8->A & 0xF0) - r8->H) & 0x0F) r8->F |= FLAG_H;
  r8->A -= r8->H;

  return 4;
}

unsigned sub_l(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->F = FLAG_N;
  if (r8->A < r8->L) r8->F |= FLAG_C;
  if (r8->A == r8->L) r8->F |= FLAG_Z;
  if (((r8->A & 0xF0) - r8->L) & 0x0F) r8->F |= FLAG_H;
  r8->A -= r8->L;

  return 4;
}

unsigned sub_hl(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint8_t x = mem_read(cpu.HL);
  r8->F = FLAG_N;
  if (r8->A < x) r8->F |= FLAG_C;
  if (r8->A == x) r8->F |= FLAG_Z;
  if (((r8->A & 0xF0) - x) & 0x0F) r8->F |= FLAG_H;
  r8->A -= x;
  

  return 8;
}

unsigned sub_n(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint8_t x = op1;
  r8->F = FLAG_N;
  if (r8->A < x) r8->F |= FLAG_C;
  if (r8->A == x) r8->F |= FLAG_Z;
  if (((r8->A & 0xF0) - x) & 0x0F) r8->F |= FLAG_H;
  r8->A -= x;

  return 8;
}
unsigned sbc_b(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint16_t x = r8->B;
  x += (r8->F & FLAG_C) ? 1: 0;
  r8->F = FLAG_N;
  if (r8->A < x) r8->F |= FLAG_C;
  if (r8->A == x) r8->F |= FLAG_Z;
  if (((r8->A & 0xF0) - x) & 0x0F) r8->F |= FLAG_H;
  r8->A -= x;

  return 4;
}
unsigned sbc_c(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint16_t x = r8->C;
  x += (r8->F & FLAG_C) ? 1: 0;
  r8->F = FLAG_N;
  if (r8->A < x) r8->F |= FLAG_C;
  if (r8->A == x) r8->F |= FLAG_Z;
  if (((r8->A & 0xF0) - x) & 0x0F) r8->F |= FLAG_H;
  r8->A -= x;

  return 4;
}

 /*
 ADD A,A 87 4
 ADD A,B 80 4
 ADD A,C 81 4
 ADD A,D 82 4
 ADD A,E 83 4
 ADD A,H 84 4
 ADD A,L 85 4
 ADD A,(HL) 86 8
 ADD A,# C6 8
 Flags affected:
 Z - Set if result is zero.
 N - Reset.
 H - Set if carry from bit 3.
 C - Set if carry from bit 7.
 */
unsigned add_a(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint8_t x = r8->A;
  uint8_t y = r8->A;
  uint16_t c = x+y;
  r8->A = x+y;
  r8->F = 0;
  if (c & 0xFF00) r8->F |= FLAG_C;
  if (!r8->A) r8->F |= FLAG_Z;
  if (((x & 0x0F) + y) & 0xF0) r8->F |= FLAG_H;

  return 4;
}
unsigned add_b(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint8_t x = r8->A;
  uint8_t y = r8->B;
  uint16_t c = x+y;
  r8->A = x+y;
  r8->F = 0;
  if (c & 0xFF00) r8->F |= FLAG_C;
  if (!r8->A) r8->F |= FLAG_Z;
  if (((x & 0x0F) + y) & 0xF0) r8->F |= FLAG_H;

  return 4;
}
unsigned add_c(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint8_t x = r8->A;
  uint8_t y = r8->C;
  uint16_t c = x+y;
  r8->A = x+y;
  r8->F = 0;
  if (c & 0xFF00) r8->F |= FLAG_C;
  if (!r8->A) r8->F |= FLAG_Z;
  if (((x & 0x0F) + y) & 0xF0) r8->F |= FLAG_H;

  return 4;
}
unsigned add_d(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint8_t x = r8->A;
  uint8_t y = r8->D;
  uint16_t c = x+y;
  r8->A = x+y;
  r8->F = 0;
  if (c & 0xFF00) r8->F |= FLAG_C;
  if (!r8->A) r8->F |= FLAG_Z;
  if (((x & 0x0F) + y) & 0xF0) r8->F |= FLAG_H;

  return 4;
}
unsigned add_e(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint8_t x = r8->A;
  uint8_t y = r8->E;
  uint16_t c = x+y;
  r8->A = x+y;
  r8->F = 0;
  if (c & 0xFF00) r8->F |= FLAG_C;
  if (!r8->A) r8->F |= FLAG_Z;
  if (((x & 0x0F) + y) & 0xF0) r8->F |= FLAG_H;

  return 4;
}
unsigned add_h(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint8_t x = r8->A;
  uint8_t y = r8->H;
  uint16_t c = x+y;
  r8->A = x+y;
  r8->F = 0;
  if (c & 0xFF00) r8->F |= FLAG_C;
  if (!r8->A) r8->F |= FLAG_Z;
  if (((x & 0x0F) + y) & 0xF0) r8->F |= FLAG_H;

  return 4;
}
unsigned add_l(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint8_t x = r8->A;
  uint8_t y = r8->L;
  uint16_t c = x+y;
  r8->A = x+y;
  r8->F = 0;
  if (c & 0xFF00) r8->F |= FLAG_C;
  if (!r8->A) r8->F |= FLAG_Z;
  if (((x & 0x0F) + y) & 0xF0) r8->F |= FLAG_H;

  return 4;
}
unsigned add_hl(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint8_t x = r8->A;
  uint8_t y = mem_read(cpu.HL);
  uint16_t c = x+y;
  r8->A = x+y;
  r8->F = 0;
  if (c & 0xFF00) r8->F |= FLAG_C;
  if (!r8->A) r8->F |= FLAG_Z;
  if (((x & 0x0F) + y) & 0xF0) r8->F |= FLAG_H;

  return 8;
}
unsigned add_n(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint8_t x = r8->A;
  uint8_t y = op1;
  uint16_t c = x+y;
  r8->A = x+y;
  r8->F = 0;
  if (c & 0xFF00) r8->F |= FLAG_C;
  if (!r8->A) r8->F |= FLAG_Z;
  if (((x & 0x0F) + y) & 0xF0) r8->F |= FLAG_H;

  return 8;
}

unsigned add16_bc(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint16_t x = cpu.HL;
  uint16_t y = cpu.BC;
  uint32_t c = x+y;
  cpu.HL = x + y;
  r8->F &= FLAG_Z;
  if (c & 0xFFFF0000) r8->F |= FLAG_C;
  if (((x & 0x0FFF) + y) & 0xF000) r8->F |= FLAG_H;

  return 8;
}
unsigned add16_de(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint16_t x = cpu.HL;
  uint16_t y = cpu.DE;
  uint32_t c = x+y;
  cpu.HL = x + y;
  r8->F &= FLAG_Z;
  if (c & 0xFFFF0000) r8->F |= FLAG_C;
  if (((x & 0x0FFF) + y) & 0xF000) r8->F |= FLAG_H;

  return 8;
}
unsigned add16_hl(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint16_t x = cpu.HL;
  uint16_t y = cpu.HL;
  uint32_t c = x+y;
  cpu.HL = x + y;
  r8->F &= FLAG_Z;
  if (c & 0xFFFF0000) r8->F |= FLAG_C;
  if (((x & 0x0FFF) + y) & 0xF000) r8->F |= FLAG_H;

  return 8;
}
unsigned add16_sp(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint16_t x = cpu.HL;
  uint16_t y = cpu.SP;
  uint32_t c = x+y;
  cpu.HL = x + y;
  r8->F &= FLAG_Z;
  if (c & 0xFFFF0000) r8->F |= FLAG_C;
  if (((x & 0x0FFF) + y) & 0xF000) r8->F |= FLAG_H;

  return 8;
}

unsigned adc_a(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint16_t c;
  uint8_t x = r8->A;
  uint8_t y = r8->A;
  if (r8->F & FLAG_C) y++;
  c = x+y;
  r8->A = x+y;
  r8->F = 0;
  if (c & 0xFF00) r8->F |= FLAG_C;
  if (!r8->A) r8->F |= FLAG_Z;
  if (((x & 0x0F) + y) & 0xF0) r8->F |= FLAG_H;

  return 4;
}
unsigned adc_b(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint16_t c;
  uint8_t x = r8->A;
  uint8_t y = r8->B;
  if (r8->F & FLAG_C) y++;
  c = x+y;
  r8->A = x+y;
  r8->F = 0;
  if (c & 0xFF00) r8->F |= FLAG_C;
  if (!r8->A) r8->F |= FLAG_Z;
  if (((x & 0x0F) + y) & 0xF0) r8->F |= FLAG_H;

  return 4;
}
unsigned adc_c(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint16_t c;
  uint8_t x = r8->A;
  uint8_t y = r8->C;
  if (r8->F & FLAG_C) y++;
  c = x+y;
  r8->A = x+y;
  r8->F = 0;
  if (c & 0xFF00) r8->F |= FLAG_C;
  if (!r8->A) r8->F |= FLAG_Z;
  if (((x & 0x0F) + y) & 0xF0) r8->F |= FLAG_H;

  return 4;
}
unsigned adc_d(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint16_t c;
  uint8_t x = r8->A;
  uint8_t y = r8->D;
  if (r8->F & FLAG_C) y++;
  c = x+y;
  r8->A = x+y;
  r8->F = 0;
  if (c & 0xFF00) r8->F |= FLAG_C;
  if (!r8->A) r8->F |= FLAG_Z;
  if (((x & 0x0F) + y) & 0xF0) r8->F |= FLAG_H;

  return 4;
}
unsigned adc_e(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint16_t c;
  uint8_t x = r8->A;
  uint8_t y = r8->E;
  if (r8->F & FLAG_C) y++;
  c = x+y;
  r8->A = x+y;
  r8->F = 0;
  if (c & 0xFF00) r8->F |= FLAG_C;
  if (!r8->A) r8->F |= FLAG_Z;
  if (((x & 0x0F) + y) & 0xF0) r8->F |= FLAG_H;

  return 4;
}
unsigned adc_h(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint16_t c;
  uint8_t x = r8->A;
  uint8_t y = r8->H;
  if (r8->F & FLAG_C) y++;
  c = x+y;
  r8->A = x+y;
  r8->F = 0;
  if (c & 0xFF00) r8->F |= FLAG_C;
  if (!r8->A) r8->F |= FLAG_Z;
  if (((x & 0x0F) + y) & 0xF0) r8->F |= FLAG_H;

  return 4;
}
unsigned adc_l(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint16_t c;
  uint8_t x = r8->A;
  uint8_t y = r8->L;
  if (r8->F & FLAG_C) y++;
  c = x+y;
  r8->A = x+y;
  r8->F = 0;
  if (c & 0xFF00) r8->F |= FLAG_C;
  if (!r8->A) r8->F |= FLAG_Z;
  if (((x & 0x0F) + y) & 0xF0) r8->F |= FLAG_H;

  return 4;
}
unsigned adc_mem(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint16_t c;
  uint8_t x = r8->A;
  uint8_t y = mem_read(cpu.HL);
  if (r8->F & FLAG_C) y++;
  c = x+y;
  r8->A = x+y;
  r8->F = 0;
  if (c & 0xFF00) r8->F |= FLAG_C;
  if (!r8->A) r8->F |= FLAG_Z;
  if (((x & 0x0F) + y) & 0xF0) r8->F |= FLAG_H;

  return 8;
}
//C <- [7 <- 0] <- [7]
unsigned rlca(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A = (r8->A<<1)|(r8->A>>7);
  r8->F = (r8->A & 0x01)? FLAG_C : 0;
  //TODO GBCPUman.pdf states Z is set if zero but others sources say 0

  return 4;
}
//[0] -> [7 -> 0] -> C
unsigned rrca(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->F = (r8->A & 0x01)? FLAG_C : 0;
  r8->A = (r8->A<<7)|((r8->A>>1)&0x7F);

  return 4;
}
//rotate through carry
unsigned rra(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  uint8_t b7 = (r8->F & FLAG_C) << 3;
  r8->F = (r8->A & 0x01) ? FLAG_C : 0;
  r8->A = b7 |((r8->A >> 1) & 0x7F);

  return 4;
}
//convert to bcd
//--------------------------------------------------------------------------------
//|           | C Flag  | HEX value in | H Flag | HEX value in | Number  | C flag|
//| Operation | Before  | upper digit  | Before | lower digit  | added   | After |
//|           | DAA     | (bit 7-4)    | DAA    | (bit 3-0)    | to byte | DAA   |
//|------------------------------------------------------------------------------|
//|           |    0    |     0-9      |   0    |     0-9      |   00    |   0   |
//|   ADD     |    0    |     0-8      |   0    |     A-F      |   06    |   0   |
//|           |    0    |     0-9      |   1    |     0-3      |   06    |   0   |
//|   ADC     |    0    |     A-F      |   0    |     0-9      |   60    |   1   |
//|           |    0    |     9-F      |   0    |     A-F      |   66    |   1   |
//|   INC     |    0    |     A-F      |   1    |     0-3      |   66    |   1   |
//|           |    1    |     0-2      |   0    |     0-9      |   60    |   1   |
//|           |    1    |     0-2      |   0    |     A-F      |   66    |   1   |
//|           |    1    |     0-3      |   1    |     0-3      |   66    |   1   |
//|------------------------------------------------------------------------------|
//|   SUB     |    0    |     0-9      |   0    |     0-9      |   00    |   0   |
//|   SBC     |    0    |     0-8      |   1    |     6-F      |   FA    |   0   |
//|   DEC     |    1    |     7-F      |   0    |     0-9      |   A0    |   1   |
//|   NEG     |    1    |     6-F      |   1    |     6-F      |   9A    |   1   |
//|------------------------------------------------------------------------------|
unsigned daa(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
    UNUSED(op0);
    UNUSED(op1);
    UNUSED(op2);
    struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
    uint16_t x = r8->A;
    //TODO this could be branchless by shift and add operations only 
    if (r8->F & FLAG_N) {
        if (r8->F & FLAG_H) x = (x - 0x06) & 0xff;
        if (r8->F & FLAG_C) x -= 0x60;
    } else {
        if ((r8->F & FLAG_H) || (x & 0x0F)>0x09) x += 0x06; 
        if ((r8->F & FLAG_C) || x>0x9F) x += 0x60; 
    }
    if (!(x & 0xFF)) r8->F |= FLAG_Z;
    if (x & 0x100) r8->F |= FLAG_C;

    r8->F &= (FLAG_C|FLAG_N|FLAG_C);
    r8->A = (x & 0xFF);

    return 4;
}

struct op_code ins_set[] = {
  {0x00, 0,  &nop, "NOP"},
  {0x01, 2,  &ld16_im_bc, "LD BC,nn"},
  {0x02, 0,  &ld_bc_mem_a, "LD (BC),A"},
  {0x03, 0,  &inc16_bc, "INC BC"},
  {0x04, 0,  &inc_b, "INC B"},
  {0x05, 0,  &dec_b, "DEC B"},
  {0x06, 1,  &ld_im_b, "ld B,n"},
  {0x07, 0,  &rlca, "RLCA"},
  {0x08, 2,  &ld_im_mem_sp, "LD (nn),SP"},
  {0x09, 0,  &add16_bc, "ADD HL,BC"},
  {0x0A, 0,  &ld_mem_bc, "LD (BC)"},
  {0x0B, 0,  &dec16_bc, "DEC BC"},
  {0x0C, 0,  &inc_c, "INC C"},
  {0x0D, 0,  &dec_c, "DEC C"},
  {0x0E, 1,  &ld_im_c, "ld C,n"},
  {0x0F, 0,  &rrca, "RRCA"},
  {0x10, 1,  &stop, "STOP"},
  {0x11, 2,  &ld16_im_de, "LD DE,nn"},
  {0x12, 0,  &ld_de_mem_a, "LD (DE),A"},
  {0x13, 0,  &inc16_de, "INC DE"},
  {0x14, 0,  &inc_d, "INC D"},
  {0x15, 0,  &dec_d, "DEC D"}, //TODO
  {0x16, 1,  &ld_im_d, "ld D,n"},
  {0x17, 0,  &nop, "TODO"}, //TODO
  {0x18, 1,  &jr_n, "JR n"},
  {0x19, 0,  &add16_de, "ADD HL,DE"},
  {0x1A, 0,  &ld_mem_de, "LD (DE)"},
  {0x1B, 0,  &dec16_de, "DEC DE"},
  {0x1C, 0,  &inc_e, "INC E"},
  {0x1D, 0,  &dec_e, "DEC E"}, //TODO
  {0x1E, 1,  &ld_im_e, "ld E,n"},
  {0x1F, 0,  &rra, "RRA"},
  {0x20, 1,  &jump_nz, "JR nz"},
  {0x21, 2,  &ld16_im_hl, "LD HL,nn"},
  {0x22, 0,  &ld_mem_inc_a, "LD (HL+),A"},
  {0x23, 0,  &inc16_hl, "INC HL"},
  {0x24, 0,  &inc_h, "INC H"},
  {0x25, 0,  &dec_h, "DEC H"},
  {0x26, 1,  &ld_im_h, "ld H,n"},
  {0x27, 0,  &daa, "DAA"},
  {0x28, 1,  &jump_z, "JR Z"},
  {0x29, 0,  &add16_hl, "ADD HL,HL"},
  {0x2A, 0,  &ld_mem_inc, "LDI A,(HL)"},
  {0x2B, 0,  &dec16_hl, "DEC HL"},
  {0x2C, 0,  &inc_l, "INC L"},
  {0x2D, 0,  &dec_l, "DEC L"},
  {0x2E, 1,  &ld_im_l, "ld L,n"},
  {0x2F, 0,  &cpl_a, "CPL"},
  {0x30, 1,  &jump_nc, "JR NC"},
  {0x31, 2,  &ld16_im_sp, "LD SP,nn"},
  {0x32, 0,  &ldd_hl_a, "LDD (HL),A"}, 
  {0x33, 0,  &inc16_sp, "INC SP"},
  {0x34, 0,  &inc_mem, "INC (HL)"},
  {0x35, 0,  &dec_hl, "DEC (HL)"},
  {0x36, 1,  &ld_im_mem, "LD (HL),n"},
  {0x37, 0,  &nop, "TODO"}, //TODO
  {0x38, 1,  &jump_c, "JR C"},
  {0x39, 0,  &add16_sp, "ADD HL,SP"},
  {0x3A, 0,  &ldd_a_hl, "LDD A, (HL)"},
  {0x3B, 0,  &dec16_sp, "DEC SP"},
  {0x3C, 0,  &inc_a, "INC A"},
  {0x3D, 0,  &dec_a, "DEC A"},
  {0x3E, 1,  &ld_im, "LD A,#"},
  {0x3F, 0,  &ccf, "CCF"},
  {0x40, 0,  &ld_b_b, "LD B,B"},
  {0x41, 0,  &ld_b_c, "LD B,C"},
  {0x42, 0,  &ld_b_d, "LD B,D"},
  {0x43, 0,  &ld_b_e, "LD B,E"},
  {0x44, 0,  &ld_b_h, "LD B,H"},
  {0x45, 0,  &ld_b_l, "LD B,L"},
  {0x46, 0,  &ld_b_hl, "LD B,(HL)"},
  {0x47, 0,  &ld_b_a, "LD B,A"},
  {0x48, 0,  &ld_c_b, "LD C,B"},
  {0x49, 0,  &ld_c_c, "LD C,C"},
  {0x4A, 0,  &ld_c_d, "LD C,D"},
  {0x4B, 0,  &ld_c_e, "LD C,E"},
  {0x4C, 0,  &ld_c_h, "LD C,H"},
  {0x4D, 0,  &ld_c_l, "LD C,L"},
  {0x4E, 0,  &ld_c_mem, "LD C,(HL)"},
  {0x4F, 0,  &ld_c_a, "LD C,A"},
  {0x50, 0,  &ld_d_b, "LD D,B"},
  {0x51, 0,  &ld_d_c, "LD D,C"},
  {0x52, 0,  &ld_d_d, "LD D,D"},
  {0x53, 0,  &ld_d_e, "LD D,E"},
  {0x54, 0,  &ld_d_h, "LD D,H"},
  {0x55, 0,  &ld_d_l, "LD D,L"},
  {0x56, 0,  &ld_d_mem, "LD D,(HL)"},
  {0x57, 0,  &ld_d_a, "LD D,A"},
  {0x58, 0,  &ld_e_b, "LD E,B"},
  {0x59, 0,  &ld_e_c, "LD E,C"},
  {0x5A, 0,  &ld_e_d, "LD E,D"},
  {0x5B, 0,  &ld_e_e, "LD E,E"},
  {0x5C, 0,  &ld_e_h, "LD E,H"},
  {0x5D, 0,  &ld_e_l, "LD E,L"},
  {0x5E, 0,  &ld_e_mem, "LD E,(HL)"},
  {0x5F, 0,  &ld_e_a, "LD E,A"},
  {0x60, 0,  &ld_h_b, "LD H,B"},
  {0x61, 0,  &ld_h_c, "LD H,C"},
  {0x62, 0,  &ld_h_d, "LD H,D"},
  {0x63, 0,  &ld_h_e, "LD H,E"},
  {0x64, 0,  &ld_h_h, "LD H,H"},
  {0x65, 0,  &ld_h_l, "LD H,L"},
  {0x66, 0,  &ld_h_mem, "LD H,(HL)"},
  {0x67, 0,  &ld_h_a, "LD H,A"},
  {0x68, 0,  &ld_l_b, "LD L,B"},
  {0x69, 0,  &ld_l_c, "LD L,C"},
  {0x6A, 0,  &ld_l_d, "LD L,D"},
  {0x6B, 0,  &ld_l_e, "LD L,E"},
  {0x6C, 0,  &ld_l_h, "LD L,H"},
  {0x6D, 0,  &ld_l_l, "LD L,L"},
  {0x6E, 0,  &ld_l_mem, "LD L,(HL)"},
  {0x6F, 0,  &ld_l_a, "LD L,A"},
  {0x70, 0,  &ld_mem_b, "LD (HL),B"},
  {0x71, 0,  &ld_mem_c, "LD (HL),C"},
  {0x72, 0,  &ld_mem_d, "LD (HL),D"},
  {0x73, 0,  &ld_mem_e, "LD (HL),E"},
  {0x74, 0,  &ld_mem_h, "LD (HL),H"},
  {0x75, 0,  &ld_mem_l, "LD (HL),L"},
  {0x76, 0,  &halt, "HALT"},
  {0x77, 0,  &ld_mem_a, "LD (HL),A"},
  {0x78, 0,  &ld_b, "LD A,B"},
  {0x79, 0,  &ld_c, "LD A,C"},
  {0x7A, 0,  &ld_d, "LD A,D"},
  {0x7B, 0,  &ld_e, "LD A,E"},
  {0x7C, 0,  &ld_h, "LD A,H"},
  {0x7D, 0,  &ld_l, "LD A,L"},
  {0x7E, 0,  &ld_mem, "LD A,(HL)"},
  {0x7F, 0,  &ld_a, "LD A,A"},
  {0x80, 0,  &add_b, "ADD B"},
  {0x81, 0,  &add_c, "ADD C"},
  {0x82, 0,  &add_d, "ADD D"},
  {0x83, 0,  &add_e, "ADD E"},
  {0x84, 0,  &add_h, "ADD H"},
  {0x85, 0,  &add_l, "ADD L"},
  {0x86, 0,  &add_hl, "ADD (HL)"},
  {0x87, 0,  &add_a, "ADD A"},
  {0x88, 0,  &adc_b, "ADC A,B"},
  {0x89, 0,  &adc_c, "ADC A,C"},
  {0x8A, 0,  &adc_d, "ADC A,D"},
  {0x8B, 0,  &adc_e, "ADC A,E"},
  {0x8C, 0,  &adc_h, "ADC A,H"},
  {0x8D, 0,  &adc_l, "ADC A,L"},
  {0x8E, 0,  &adc_mem, "ADC A,(HL)"},
  {0x8F, 0,  &adc_a, "ADC A"},
  {0x90, 0,  &sub_b, "SUB B"},
  {0x91, 0,  &sub_c, "SUB C"},
  {0x92, 0,  &sub_d, "SUB D"},
  {0x93, 0,  &sub_e, "SUB E"},
  {0x94, 0,  &sub_h, "SUB H"},
  {0x95, 0,  &sub_l, "SUB L"},
  {0x96, 0,  &sub_hl, "SUB (HL)"},
  {0x97, 0,  &sub_a, "SUB A"},
  {0x98, 0,  &sbc_b, "SBC C"},
  {0x99, 0,  &sbc_c, "SBC C"},
  {0x9A, 0,  &nop, "TODO"}, //TODO
  {0x9B, 0,  &nop, "TODO"}, //TODO
  {0x9C, 0,  &nop, "TODO"}, //TODO
  {0x9D, 0,  &nop, "TODO"}, //TODO
  {0x9E, 0,  &nop, "TODO"}, //TODO
  {0x9F, 0,  &nop, "TODO"}, //TODO
  {0xA0, 0,  &and_b, "AND B"},
  {0xA1, 0,  &and_c, "AND C"},
  {0xA2, 0,  &and_d, "AND D"},
  {0xA3, 0,  &and_e, "AND E"},
  {0xA4, 0,  &and_h, "AND H"},
  {0xA5, 0,  &and_l, "AND L"},
  {0xA6, 0,  &and_mem, "AND (HL)"},
  {0xA7, 0,  &and_a, "AND A"},
  {0xA8, 0,  &nop, "TODO"}, //TODO
  {0xA9, 0,  &nop, "TODO"}, //TODO
  {0xAA, 0,  &nop, "TODO"}, //TODO
  {0xAB, 0,  &nop, "TODO"}, //TODO
  {0xAC, 0,  &nop, "TODO"}, //TODO
  {0xAD, 0,  &nop, "TODO"}, //TODO
  {0xAE, 0,  &nop, "TODO"}, //TODO
  {0xAF, 0,  &xor_a, "XOR A"}, 
  {0xB0, 0,  &or_b, "OR B"},
  {0xB1, 0,  &or_c, "OR C"},
  {0xB2, 0,  &or_d, "OR D"},
  {0xB3, 0,  &or_e, "OR E"},
  {0xB4, 0,  &or_h, "OR H"},
  {0xB5, 0,  &or_l, "OR L"},
  {0xB6, 0,  &or_mem, "OR (HL)"},
  {0xB7, 0,  &or_a, "OR A"},
  {0xB8, 0,  &cp_b, "CP B"},
  {0xB9, 0,  &cp_c, "CP C"},
  {0xBA, 0,  &cp_d, "CP D"},
  {0xBB, 0,  &cp_e, "CP E"},
  {0xBC, 0,  &cp_h, "CP H"},
  {0xBD, 0,  &cp_l, "CP L"},
  {0xBE, 0,  &cp_hl, "CP (HL)"},
  {0xBF, 0,  &cp_a, "CP A"},
  {0xC0, 0,  &ret_nz, "RET NZ"},
  {0xC1, 0,  &nop, "TODO"}, //TODO
  {0xC2, 0,  &nop, "TODO"}, //TODO
  {0xC3, 2,  &jump_c3, "JUMP nn"}, 
  {0xC4, 0,  &nop, "TODO"}, //TODO
  {0xC5, 0,  &nop, "TODO"}, //TODO
  {0xC6, 1,  &add_n, "ADD #"},
  {0xC7, 0,  &rst, "RST C7"},
  {0xC8, 0,  &ret_z, "RET Z"},
  {0xC9, 0,  &ret, "RET"},
  {0xCA, 0,  &nop, "TODO"}, //TODO
  {0xCB, 1,  &cb, "0xCB"},
  {0xCC, 0,  &nop, "TODO"}, //TODO
  {0xCD, 2,  &call, "CALL nn"},
  {0xCE, 0,  &nop, "TODO"}, //TODO
  {0xCF, 0,  &rst, "RST CF"},
  {0xD0, 0,  &ret_nc, "RET NC"},
  {0xD1, 0,  &nop, "TODO"}, //TODO
  {0xD2, 0,  &nop, "TODO"}, //TODO
  {0xD3, 0,  &nop, "TODO"}, //TODO
  {0xD4, 0,  &nop, "TODO"}, //TODO
  {0xD5, 0,  &nop, "TODO"}, //TODO
  {0xD6, 1,  &sub_n, "SUB n"},
  {0xD7, 0,  &rst, "RST D7"},
  {0xD8, 0,  &ret_c, "RET C"},
  {0xD9, 0,  &nop, "TODO"}, //TODO
  {0xDA, 0,  &nop, "TODO"}, //TODO
  {0xDB, 0,  &nop, "TODO"}, //TODO
  {0xDC, 0,  &nop, "TODO"}, //TODO
  {0xDD, 0,  &nop, "TODO"}, //TODO
  {0xDE, 0,  &nop, "TODO"}, //TODO
  {0xDF, 0,  &rst, "RST DF"},
  {0xE0, 1,  &ldh_a_ff00, "LDH (n), A"},
  {0xE1, 0,  &nop, "TODO"}, //TODO
  {0xE2, 0,  &ldh_ff00_C_a, "LD (FF00+C),A"},
  {0xE3, 0,  &nop, "TODO"}, //TODO
  {0xE4, 0,  &nop, "TODO"}, //TODO
  {0xE5, 0,  &nop, "TODO"}, //TODO
  {0xE6, 0,  &nop, "TODO"}, //TODO
  {0xE7, 0,  &nop, "TODO"}, //TODO
  {0xE8, 0,  &nop, "TODO"}, //TODO
  {0xE9, 0,  &nop, "TODO"}, //TODO
  {0xEA, 2,  &ld_im_mem_a, "LD (nn),A"},
  {0xEB, 0,  &nop, "TODO"}, //TODO
  {0xEC, 0,  &nop, "TODO"}, //TODO
  {0xED, 0,  &nop, "TODO"}, //TODO
  {0xEE, 0,  &nop, "TODO"}, //TODO
  {0xEF, 0,  &nop, "TODO"}, //TODO
  {0xF0, 1,  &ldh_ff00_a, "LDH A, (n)"},
  {0xF1, 0,  &nop, "TODO"}, //TODO
  {0xF2, 0,  &ldh_a_ff00_C, "LD (FF00+C),A"},
  {0xF3, 0,  &di, "DI"},
  {0xF4, 0,  &nop, "TODO"}, //TODO
  {0xF5, 0,  &nop, "TODO"}, //TODO
  {0xF6, 1,  &or_n, "OR #"},
  {0xF7, 0,  &nop, "TODO"}, //TODO
  {0xF8, 0,  &nop, "TODO"}, //TODO
  {0xF9, 0,  &nop, "TODO"}, //TODO
  {0xFA, 0,  &nop, "TODO"}, //TODO
  {0xFB, 0,  &ei, "EI"},
  {0xFC, 0,  &nop, "TODO"}, //TODO
  {0xFD, 0,  &nop, "TODO"}, //TODO
  {0xFE, 1,  &cp_n, "CP n"},
  {0xFF, 0,  &rst, "RST FF"}, //TODO
};

unsigned op_width(uint8_t op) 
{
    return ins_set[op].width;
}
char *op_info(uint8_t op) 
{
    return ins_set[op].symbol;
}

void cpu_reset()
{
   //this is crude reset with some default values to force jump into rom
   //real cpu should have BIOS rom that set these up and loads cart into rom
   cpu.AF = 0x1100; //
   cpu.BC = 0x0100;
   cpu.DE = 0x0008;
   cpu.HL = 0x007c;//0x014D;
   cpu.SP = 0xCFFE; //Stack Pointer=$FFFE
   cpu.PC = 0x0100; //


}

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
    ime = 0;
    mem_write16(cpu.SP, cpu.PC);
    cpu.SP -= 2;
    cpu.PC = 0x0000 + irq;
    return 20;
}
void dbg_pc_sp()
{
    printf("PC 0x%04X: SP 0x%04X, ", cpu.PC, cpu.SP);
}
void dbg_reg()
{
    printf("AF 0x%04X, BC 0x%04X, DE 0x%04X, HL 0x%04X,", cpu.AF, cpu.BC, cpu.DE, cpu.HL);
}
void dbg_op(uint8_t op0, uint8_t op1, uint8_t op2)
{
    printf(" 0x%02X, %s OP1:%02X OP2:%02X\n", op0, op_info(op0), op1, op2);
}
void cpu_cycle()
{
    uint8_t op[4];
    uint8_t *p = op;
    unsigned width;
    //run_timers()
    
    //if not halted
    //if prev op cycles expired
    //DEBUG
dbg_pc_sp();

    *p++ = mem_read(cpu.PC++);
    width = op_width(*op);
    
    if (width>1) *p++ = mem_read(cpu.PC++);
    if (width) *p++ = mem_read(cpu.PC++);

    ins_set[*op].op_call(op[0], op[1], op[2]);

    //DEBUG
dbg_reg();
dbg_op(*op, op[1], op[2]);
    //hangs at unimplemented
    if (!strcmp(op_info(*op),"TODO")) {
        cpu.PC--;
        dbg_pc_sp();
        dbg_reg();
        dbg_op(*op, op[1], op[2]);
        exit(1);
        //for (;;) sleep(1);
    }
    //if(cpu.PC > 0x2000) sleep(1);
    

}
