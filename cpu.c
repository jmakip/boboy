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

unsigned jump_c3(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  cpu.PC = (uint16_t)op1+(((uint16_t)op2)<<8);
  return 12;
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

unsigned ld_b(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op1);
  UNUSED(op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  r8->A = r8->B;
  return 4;
}

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
// LD (HL), n
unsigned ld_im_mem(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  UNUSED(op2);
  mem_write(cpu.HL, op1);
  return 12;
}

unsigned ld_im_mem_a(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  struct gb_reg8 *r8 = (struct gb_reg8 *)&cpu;
  mem_write((uint16_t)op1+((uint16_t)op2)<<8, r8->A);
  return 16;
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

struct op_code ins_set[] = {
  {0x00, 0,  &nop, "NOP"},
  {0x01, 2,  &ld16_im_bc, "LD BC,nn"},
  {0x02, 0,  &nop, "TODO"}, //TODO
  {0x03, 0,  &nop, "TODO"}, //TODO
  {0x04, 0,  &nop, "TODO"}, //TODO
  {0x05, 0,  &dec_b, "DEC B"},
  {0x06, 1,  &ld_im_b, "ld B,n"},
  {0x07, 0,  &nop, "TODO"}, //TODO
  {0x08, 0,  &nop, "TODO"}, //TODO
  {0x09, 0,  &nop, "TODO"}, //TODO
  {0x0A, 0,  &nop, "TODO"}, //TODO
  {0x0B, 0,  &nop, "TODO"}, //TODO
  {0x0C, 0,  &nop, "TODO"}, //TODO
  {0x0D, 0,  &dec_c, "DEC C"},
  {0x0E, 1,  &ld_im_c, "ld C,n"},
  {0x0F, 0,  &nop, "TODO"}, //TODO
  {0x10, 0,  &nop, "TODO"}, //TODO
  {0x11, 2,  &ld16_im_de, "LD DE,nn"},
  {0x12, 0,  &nop, "TODO"}, //TODO
  {0x13, 0,  &nop, "TODO"}, //TODO
  {0x14, 0,  &nop, "TODO"}, //TODO
  {0x15, 0,  &dec_d, "DEC D"}, //TODO
  {0x16, 1,  &ld_im_d, "ld D,n"},
  {0x17, 0,  &nop, "TODO"}, //TODO
  {0x18, 0,  &nop, "TODO"}, //TODO
  {0x19, 0,  &nop, "TODO"}, //TODO
  {0x1A, 0,  &nop, "TODO"}, //TODO
  {0x1B, 0,  &nop, "TODO"}, //TODO
  {0x1C, 0,  &nop, "TODO"}, //TODO
  {0x1D, 0,  &dec_e, "DEC E"}, //TODO
  {0x1E, 1,  &ld_im_e, "ld E,n"},
  {0x1F, 0,  &nop, "TODO"}, //TODO
  {0x20, 1,  &jump_nz, "JR nz"},
  {0x21, 2,  &ld16_im_hl, "LD HL,nn"},
  {0x22, 0,  &nop, "TODO"}, //TODO
  {0x23, 0,  &nop, "TODO"}, //TODO
  {0x24, 0,  &nop, "TODO"}, //TODO
  {0x25, 0,  &dec_h, "DEC H"},
  {0x26, 1,  &ld_im_h, "ld H,n"},
  {0x27, 0,  &nop, "TODO"}, //TODO
  {0x28, 1,  &jump_z, "JR Z"},
  {0x29, 0,  &nop, "TODO"}, //TODO
  {0x2A, 0,  &ld_mem_inc, "LDI A,(HL)"},
  {0x2B, 0,  &nop, "TODO"}, //TODO
  {0x2C, 0,  &nop, "TODO"}, //TODO
  {0x2D, 0,  &dec_l, "DEC L"},
  {0x2E, 1,  &ld_im_l, "ld L,n"},
  {0x2F, 0,  &nop, "TODO"}, //TODO
  {0x30, 1,  &jump_nc, "JR NC"},
  {0x31, 2,  &ld16_im_sp, "LD SP,nn"},
  {0x32, 0,  &ldd_hl_a, "LDD (HL),A"}, 
  {0x33, 0,  &nop, "TODO"}, //TODO
  {0x34, 0,  &nop, "TODO"}, //TODO
  {0x35, 0,  &dec_hl, "DEC (HL)"},
  {0x36, 1,  &ld_im_mem, "LD (HL),n"}, //TODO
  {0x37, 0,  &nop, "TODO"}, //TODO
  {0x38, 1,  &jump_c, "JR C"}, //TODO
  {0x39, 0,  &nop, "TODO"}, //TODO
  {0x3A, 0,  &ldd_a_hl, "LDD A, (HL)"},
  {0x3B, 0,  &nop, "TODO"}, //TODO
  {0x3C, 0,  &nop, "TODO"}, //TODO
  {0x3D, 0,  &dec_a, "DEC A"},
  {0x3E, 1,  &ld_im, "LD A,#"},
  {0x3F, 0,  &nop, "TODO"}, //TODO
  {0x40, 0,  &nop, "TODO"}, //TODO
  {0x41, 0,  &nop, "TODO"}, //TODO
  {0x42, 0,  &nop, "TODO"}, //TODO
  {0x43, 0,  &nop, "TODO"}, //TODO
  {0x44, 0,  &nop, "TODO"}, //TODO
  {0x45, 0,  &nop, "TODO"}, //TODO
  {0x46, 0,  &nop, "TODO"}, //TODO
  {0x47, 0,  &nop, "TODO"}, //TODO
  {0x48, 0,  &nop, "TODO"}, //TODO
  {0x49, 0,  &nop, "TODO"}, //TODO
  {0x4A, 0,  &nop, "TODO"}, //TODO
  {0x4B, 0,  &nop, "TODO"}, //TODO
  {0x4C, 0,  &nop, "TODO"}, //TODO
  {0x4D, 0,  &nop, "TODO"}, //TODO
  {0x4E, 0,  &nop, "TODO"}, //TODO
  {0x4F, 0,  &nop, "TODO"}, //TODO
  {0x50, 0,  &nop, "TODO"}, //TODO
  {0x51, 0,  &nop, "TODO"}, //TODO
  {0x52, 0,  &nop, "TODO"}, //TODO
  {0x53, 0,  &nop, "TODO"}, //TODO
  {0x54, 0,  &nop, "TODO"}, //TODO
  {0x55, 0,  &nop, "TODO"}, //TODO
  {0x56, 0,  &nop, "TODO"}, //TODO
  {0x57, 0,  &nop, "TODO"}, //TODO
  {0x58, 0,  &nop, "TODO"}, //TODO
  {0x59, 0,  &nop, "TODO"}, //TODO
  {0x5A, 0,  &nop, "TODO"}, //TODO
  {0x5B, 0,  &nop, "TODO"}, //TODO
  {0x5C, 0,  &nop, "TODO"}, //TODO
  {0x5D, 0,  &nop, "TODO"}, //TODO
  {0x5E, 0,  &nop, "TODO"}, //TODO
  {0x5F, 0,  &nop, "TODO"}, //TODO
  {0x60, 0,  &nop, "TODO"}, //TODO
  {0x61, 0,  &nop, "TODO"}, //TODO
  {0x62, 0,  &nop, "TODO"}, //TODO
  {0x63, 0,  &nop, "TODO"}, //TODO
  {0x64, 0,  &nop, "TODO"}, //TODO
  {0x65, 0,  &nop, "TODO"}, //TODO
  {0x66, 0,  &nop, "TODO"}, //TODO
  {0x67, 0,  &nop, "TODO"}, //TODO
  {0x68, 0,  &nop, "TODO"}, //TODO
  {0x69, 0,  &nop, "TODO"}, //TODO
  {0x6A, 0,  &nop, "TODO"}, //TODO
  {0x6B, 0,  &nop, "TODO"}, //TODO
  {0x6C, 0,  &nop, "TODO"}, //TODO
  {0x6D, 0,  &nop, "TODO"}, //TODO
  {0x6E, 0,  &nop, "TODO"}, //TODO
  {0x6F, 0,  &nop, "TODO"}, //TODO
  {0x70, 0,  &nop, "TODO"}, //TODO
  {0x71, 0,  &nop, "TODO"}, //TODO
  {0x72, 0,  &nop, "TODO"}, //TODO
  {0x73, 0,  &nop, "TODO"}, //TODO
  {0x74, 0,  &nop, "TODO"}, //TODO
  {0x75, 0,  &nop, "TODO"}, //TODO
  {0x76, 0,  &nop, "TODO"}, //TODO
  {0x77, 0,  &nop, "TODO"}, //TODO
  {0x78, 0,  &nop, "TODO"}, //TODO
  {0x79, 0,  &nop, "TODO"}, //TODO
  {0x7A, 0,  &nop, "TODO"}, //TODO
  {0x7B, 0,  &nop, "TODO"}, //TODO
  {0x7C, 0,  &nop, "TODO"}, //TODO
  {0x7D, 0,  &nop, "TODO"}, //TODO
  {0x7E, 0,  &nop, "TODO"}, //TODO
  {0x7F, 0,  &nop, "TODO"}, //TODO
  {0x80, 0,  &nop, "TODO"}, //TODO
  {0x81, 0,  &nop, "TODO"}, //TODO
  {0x82, 0,  &nop, "TODO"}, //TODO
  {0x83, 0,  &nop, "TODO"}, //TODO
  {0x84, 0,  &nop, "TODO"}, //TODO
  {0x85, 0,  &nop, "TODO"}, //TODO
  {0x86, 0,  &nop, "TODO"}, //TODO
  {0x87, 0,  &nop, "TODO"}, //TODO
  {0x88, 0,  &nop, "TODO"}, //TODO
  {0x89, 0,  &nop, "TODO"}, //TODO
  {0x8A, 0,  &nop, "TODO"}, //TODO
  {0x8B, 0,  &nop, "TODO"}, //TODO
  {0x8C, 0,  &nop, "TODO"}, //TODO
  {0x8D, 0,  &nop, "TODO"}, //TODO
  {0x8E, 0,  &nop, "TODO"}, //TODO
  {0x8F, 0,  &nop, "TODO"}, //TODO
  {0x90, 0,  &nop, "TODO"}, //TODO
  {0x91, 0,  &nop, "TODO"}, //TODO
  {0x92, 0,  &nop, "TODO"}, //TODO
  {0x93, 0,  &nop, "TODO"}, //TODO
  {0x94, 0,  &nop, "TODO"}, //TODO
  {0x95, 0,  &nop, "TODO"}, //TODO
  {0x96, 0,  &nop, "TODO"}, //TODO
  {0x97, 0,  &nop, "TODO"}, //TODO
  {0x98, 0,  &nop, "TODO"}, //TODO
  {0x99, 0,  &nop, "TODO"}, //TODO
  {0x9A, 0,  &nop, "TODO"}, //TODO
  {0x9B, 0,  &nop, "TODO"}, //TODO
  {0x9C, 0,  &nop, "TODO"}, //TODO
  {0x9D, 0,  &nop, "TODO"}, //TODO
  {0x9E, 0,  &nop, "TODO"}, //TODO
  {0x9F, 0,  &nop, "TODO"}, //TODO
  {0xA0, 0,  &nop, "TODO"}, //TODO
  {0xA1, 0,  &nop, "TODO"}, //TODO
  {0xA2, 0,  &nop, "TODO"}, //TODO
  {0xA3, 0,  &nop, "TODO"}, //TODO
  {0xA4, 0,  &nop, "TODO"}, //TODO
  {0xA5, 0,  &nop, "TODO"}, //TODO
  {0xA6, 0,  &nop, "TODO"}, //TODO
  {0xA7, 0,  &nop, "TODO"}, //TODO
  {0xA8, 0,  &nop, "TODO"}, //TODO
  {0xA9, 0,  &nop, "TODO"}, //TODO
  {0xAA, 0,  &nop, "TODO"}, //TODO
  {0xAB, 0,  &nop, "TODO"}, //TODO
  {0xAC, 0,  &nop, "TODO"}, //TODO
  {0xAD, 0,  &nop, "TODO"}, //TODO
  {0xAE, 0,  &nop, "TODO"}, //TODO
  {0xAF, 0,  &xor_a, "XOR A"}, 
  {0xB0, 0,  &nop, "TODO"}, //TODO
  {0xB1, 0,  &nop, "TODO"}, //TODO
  {0xB2, 0,  &nop, "TODO"}, //TODO
  {0xB3, 0,  &nop, "TODO"}, //TODO
  {0xB4, 0,  &nop, "TODO"}, //TODO
  {0xB5, 0,  &nop, "TODO"}, //TODO
  {0xB6, 0,  &nop, "TODO"}, //TODO
  {0xB7, 0,  &nop, "TODO"}, //TODO
  {0xB8, 0,  &cp_b, "CP B"},
  {0xB9, 0,  &cp_c, "CP C"},
  {0xBA, 0,  &cp_d, "CP D"},
  {0xBB, 0,  &cp_e, "CP E"},
  {0xBC, 0,  &cp_h, "CP H"},
  {0xBD, 0,  &cp_l, "CP L"},
  {0xBE, 0,  &cp_hl, "CP (HL)"},
  {0xBF, 0,  &cp_a, "CP A"},
  {0xC0, 0,  &nop, "TODO"}, //TODO
  {0xC1, 0,  &nop, "TODO"}, //TODO
  {0xC2, 0,  &nop, "TODO"}, //TODO
  {0xC3, 2,  &jump_c3, "JUMP nn"}, 
  {0xC4, 0,  &nop, "TODO"}, //TODO
  {0xC5, 0,  &nop, "TODO"}, //TODO
  {0xC6, 0,  &nop, "TODO"}, //TODO
  {0xC7, 0,  &nop, "TODO"}, //TODO
  {0xC8, 0,  &nop, "TODO"}, //TODO
  {0xC9, 0,  &nop, "TODO"}, //TODO
  {0xCA, 0,  &nop, "TODO"}, //TODO
  {0xCB, 1,  &cb, "0xCB"},
  {0xCC, 0,  &nop, "TODO"}, //TODO
  {0xCD, 0,  &nop, "TODO"}, //TODO
  {0xCE, 0,  &nop, "TODO"}, //TODO
  {0xCF, 0,  &nop, "TODO"}, //TODO
  {0xD0, 0,  &nop, "TODO"}, //TODO
  {0xD1, 0,  &nop, "TODO"}, //TODO
  {0xD2, 0,  &nop, "TODO"}, //TODO
  {0xD3, 0,  &nop, "TODO"}, //TODO
  {0xD4, 0,  &nop, "TODO"}, //TODO
  {0xD5, 0,  &nop, "TODO"}, //TODO
  {0xD6, 0,  &nop, "TODO"}, //TODO
  {0xD7, 0,  &nop, "TODO"}, //TODO
  {0xD8, 0,  &nop, "TODO"}, //TODO
  {0xD9, 0,  &nop, "TODO"}, //TODO
  {0xDA, 0,  &nop, "TODO"}, //TODO
  {0xDB, 0,  &nop, "TODO"}, //TODO
  {0xDC, 0,  &nop, "TODO"}, //TODO
  {0xDD, 0,  &nop, "TODO"}, //TODO
  {0xDE, 0,  &nop, "TODO"}, //TODO
  {0xDF, 0,  &nop, "TODO"}, //TODO
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
  {0xF6, 0,  &nop, "TODO"}, //TODO
  {0xF7, 0,  &nop, "TODO"}, //TODO
  {0xF8, 0,  &nop, "TODO"}, //TODO
  {0xF9, 0,  &nop, "TODO"}, //TODO
  {0xFA, 0,  &nop, "TODO"}, //TODO
  {0xFB, 0,  &ei, "EI"},
  {0xFC, 0,  &nop, "TODO"}, //TODO
  {0xFD, 0,  &nop, "TODO"}, //TODO
  {0xFE, 1,  &cp_n, "CP n"},
  {0xFF, 0,  &nop, "TODO"}, //TODO
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
   cpu.SP = 0xFFFE; //Stack Pointer=$FFFE
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

void cpu_cycle()
{
    uint8_t op[4];
    uint8_t *p = op;
    unsigned width;
    //run_timers()
    
    //if not halted
    //if prev op cycles expired
    //DEBUG
    printf("PC 0x%04X: AF 0x%04X SP 0x%04X", cpu.PC, cpu.AF, cpu.SP);
    //printf("PC 0x%04X: ", cpu.PC);

    *p++ = mem_read(cpu.PC++);
    width = op_width(*op);
    
    if (width>1) *p++ = mem_read(cpu.PC++);
    if (width) *p++ = mem_read(cpu.PC++);

    ins_set[*op].op_call(op[0], op[1], op[2]);

    //DEBUG
    printf(" 0x%02X, %s OP1:%02X OP2:%02X\n", *op, op_info(*op), op[1], op[2]);
    //hangs at unimplemented
    if (!strcmp(op_info(*op),"TODO")) for (;;) sleep(1);

}
