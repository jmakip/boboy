
//suppress warnings when variable is not used
#define UNUSED(...) (void)(__VA_ARGS__)

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

unsigned nop(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0, op1, op2);
  return 4;
}

unsigned cb(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0, op1, op2);
  return 8;
}

unsigned ld_im_b(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0, op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)cpu;
  r8->B = op1;
  return 8;
}

unsigned ld_im_c(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0, op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)cpu;
  r8->C = op1;
  return 8;
}

unsigned ld_im_d(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0, op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)cpu;
  r8->D = op1;
  return 8;
}

unsigned ld_im_e(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0, op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)cpu;
  r8->E = op1;
  return 8;
}

unsigned ld_im_h(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0, op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)cpu;
  r8->H = op1;
  return 8;
}

unsigned ld_im_l(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0, op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)cpu;
  r8->L = op1;
  return 8;
}

//same as NOP
unsigned ld_a(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0, op1, op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)cpu;
  r8->A = r8->A;
  return 4;
}

unsigned ld_b(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0, op1, op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)cpu;
  r8->A = r8->B;
  return 4;
}

unsigned ld_c(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0, op1, op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)cpu;
  r8->A = r8->C;
  return 4;
}

// LD A,D
unsigned ld_d(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0, op1, op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)cpu;
  r8->A = r8->D;
  return 4;
}

// LD A,E
unsigned ld_e(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0, op1, op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)cpu;
  r8->A = r8->E;
  return 4;
}

// LD A,H
unsigned ld_h(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0, op1, op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)cpu;
  r8->A = r8->H;
  return 4;
}

// LD A,L
unsigned ld_l(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0, op1, op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)cpu;
  r8->A = r8->l;
  return 4;
}

// LD A,(HL)
unsigned ld_mem(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0, op1, op2);
  struct gb_reg8 *r8 = (struct gb_reg8 *)cpu;
  r8->A = mem_fetch(cpu.HL);
  return 8;
}

unsigned ld16_im_bc(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  struct gb_reg8 *r8 = (struct gb_reg8 *)cpu;
  r8->B = op2;
  r8->C = op1;
  return 12;
}

unsigned ld16_im_de(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  struct gb_reg8 *r8 = (struct gb_reg8 *)cpu;
  r8->D = op2;
  r8->E = op1;
  return 12;
}

unsigned ld16_im_hl(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  struct gb_reg8 *r8 = (struct gb_reg8 *)cpu;
  r8->H = op2;
  r8->L = op1;
  return 12;
}

unsigned ld16_im_sp(uint8_t op0, uint8_t op1, uint8_t op2)
{ 
  UNUSED(op0);
  cpu.SP = ((uint16_t)op1<<8)+op2;
  return 12;
}

struct op_code ins_set[] {
  {0x00, 0,  &nop, "NOP"},
  {0x01, 2,  &ld16_im_bc, "LD BC,nn"},
  {0x02, 0,  &nop, "TODO"}, //TODO
  {0x03, 0,  &nop, "TODO"}, //TODO
  {0x04, 0,  &nop, "TODO"}, //TODO
  {0x05, 0,  &nop, "TODO"}, //TODO
  {0x06, 1,  &ld_im_b, "ld B,n"},
  {0x07, 0,  &nop, "TODO"}, //TODO
  {0x08, 0,  &nop, "TODO"}, //TODO
  {0x09, 0,  &nop, "TODO"}, //TODO
  {0x0A, 0,  &nop, "TODO"}, //TODO
  {0x0B, 0,  &nop, "TODO"}, //TODO
  {0x0C, 0,  &nop, "TODO"}, //TODO
  {0x0D, 0,  &nop, "TODO"}, //TODO
  {0x0E, 1,  &ld_im_c, "ld C,n"},
  {0x0F, 0,  &nop, "TODO"}, //TODO
  {0x10, 0,  &nop, "TODO"}, //TODO
  {0x12, 0,  &nop, "TODO"}, //TODO
  {0x13, 0,  &nop, "TODO"}, //TODO
  {0x14, 0,  &nop, "TODO"}, //TODO
  {0x15, 0,  &nop, "TODO"}, //TODO
  {0x16, 1,  &ld_im_d, "ld D,n"},
  {0x17, 0,  &nop, "TODO"}, //TODO
  {0x18, 0,  &nop, "TODO"}, //TODO
  {0x19, 0,  &nop, "TODO"}, //TODO
  {0x1A, 0,  &nop, "TODO"}, //TODO
  {0x1B, 0,  &nop, "TODO"}, //TODO
  {0x1C, 0,  &nop, "TODO"}, //TODO
  {0x1D, 0,  &nop, "TODO"}, //TODO
  {0x1E, 1,  &ld_im_e, "ld E,n"},
  {0x1F, 0,  &nop, "TODO"}, //TODO
  {0x20, 0,  &nop, "TODO"}, //TODO
  {0x22, 0,  &nop, "TODO"}, //TODO
  {0x23, 0,  &nop, "TODO"}, //TODO
  {0x24, 0,  &nop, "TODO"}, //TODO
  {0x25, 0,  &nop, "TODO"}, //TODO
  {0x26, 1,  &ld_im_h, "ld H,n"},
  {0x27, 0,  &nop, "TODO"}, //TODO
  {0x28, 0,  &nop, "TODO"}, //TODO
  {0x29, 0,  &nop, "TODO"}, //TODO
  {0x2A, 0,  &nop, "TODO"}, //TODO
  {0x2B, 0,  &nop, "TODO"}, //TODO
  {0x2C, 0,  &nop, "TODO"}, //TODO
  {0x2D, 0,  &nop, "TODO"}, //TODO
  {0x2E, 1,  &ld_im_L, "ld L,n"},
  {0x2F, 0,  &nop, "TODO"}, //TODO
  {0x30, 0,  &nop, "TODO"}, //TODO
  {0x31, 0,  &nop, "TODO"}, //TODO
  {0x32, 0,  &nop, "TODO"}, //TODO
  {0x33, 0,  &nop, "TODO"}, //TODO
  {0x34, 0,  &nop, "TODO"}, //TODO
  {0x35, 0,  &nop, "TODO"}, //TODO
  {0x36, 0,  &nop, "TODO"}, //TODO
  {0x37, 0,  &nop, "TODO"}, //TODO
  {0x38, 0,  &nop, "TODO"}, //TODO
  {0x39, 0,  &nop, "TODO"}, //TODO
  {0x3A, 0,  &nop, "TODO"}, //TODO
  {0x3B, 0,  &nop, "TODO"}, //TODO
  {0x3C, 0,  &nop, "TODO"}, //TODO
  {0x3D, 0,  &nop, "TODO"}, //TODO
  {0x3E, 0,  &nop, "TODO"}, //TODO
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
  {0xAF, 0,  &nop, "TODO"}, //TODO
  {0xB0, 0,  &nop, "TODO"}, //TODO
  {0xB1, 0,  &nop, "TODO"}, //TODO
  {0xB2, 0,  &nop, "TODO"}, //TODO
  {0xB3, 0,  &nop, "TODO"}, //TODO
  {0xB4, 0,  &nop, "TODO"}, //TODO
  {0xB5, 0,  &nop, "TODO"}, //TODO
  {0xB6, 0,  &nop, "TODO"}, //TODO
  {0xB7, 0,  &nop, "TODO"}, //TODO
  {0xB8, 0,  &nop, "TODO"}, //TODO
  {0xB9, 0,  &nop, "TODO"}, //TODO
  {0xBA, 0,  &nop, "TODO"}, //TODO
  {0xBB, 0,  &nop, "TODO"}, //TODO
  {0xBC, 0,  &nop, "TODO"}, //TODO
  {0xBD, 0,  &nop, "TODO"}, //TODO
  {0xBE, 0,  &nop, "TODO"}, //TODO
  {0xBF, 0,  &nop, "TODO"}, //TODO
  {0xC0, 0,  &nop, "TODO"}, //TODO
  {0xC1, 0,  &nop, "TODO"}, //TODO
  {0xC2, 0,  &nop, "TODO"}, //TODO
  {0xC3, 0,  &nop, "TODO"}, //TODO
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
  {0xE0, 0,  &nop, "TODO"}, //TODO
  {0xE1, 0,  &nop, "TODO"}, //TODO
  {0xE2, 0,  &nop, "TODO"}, //TODO
  {0xE3, 0,  &nop, "TODO"}, //TODO
  {0xE4, 0,  &nop, "TODO"}, //TODO
  {0xE5, 0,  &nop, "TODO"}, //TODO
  {0xE6, 0,  &nop, "TODO"}, //TODO
  {0xE7, 0,  &nop, "TODO"}, //TODO
  {0xE8, 0,  &nop, "TODO"}, //TODO
  {0xE9, 0,  &nop, "TODO"}, //TODO
  {0xEA, 0,  &nop, "TODO"}, //TODO
  {0xEB, 0,  &nop, "TODO"}, //TODO
  {0xEC, 0,  &nop, "TODO"}, //TODO
  {0xED, 0,  &nop, "TODO"}, //TODO
  {0xEE, 0,  &nop, "TODO"}, //TODO
  {0xEF, 0,  &nop, "TODO"}, //TODO
  {0xF0, 0,  &nop, "TODO"}, //TODO
  {0xF1, 0,  &nop, "TODO"}, //TODO
  {0xF2, 0,  &nop, "TODO"}, //TODO
  {0xF3, 0,  &nop, "TODO"}, //TODO
  {0xF4, 0,  &nop, "TODO"}, //TODO
  {0xF5, 0,  &nop, "TODO"}, //TODO
  {0xF6, 0,  &nop, "TODO"}, //TODO
  {0xF7, 0,  &nop, "TODO"}, //TODO
  {0xF8, 0,  &nop, "TODO"}, //TODO
  {0xF9, 0,  &nop, "TODO"}, //TODO
  {0xFA, 0,  &nop, "TODO"}, //TODO
  {0xFB, 0,  &nop, "TODO"}, //TODO
  {0xFC, 0,  &nop, "TODO"}, //TODO
  {0xFD, 0,  &nop, "TODO"}, //TODO
  {0xFE, 0,  &nop, "TODO"}, //TODO
  {0xFF, 0,  &nop, "TODO"}, //TODO
};

