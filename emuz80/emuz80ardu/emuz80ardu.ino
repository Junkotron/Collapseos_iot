/* zextest.c
 * Example program using z80emu to run the zexall and zexdoc tests. This will 
 * check if the Z80 is correctly emulated.
 *
 * Copyright (c) 2012, 2016 Lin Ke-Fong
 * Copyright (c) 2012 Chris Pressey
 *
 * This code is free, do whatever you want with it.
 */
#include <time.h>
#include "zextest.h"
#include "z80emu.h"
#include "z80user.h"
#include "instructions.h"
#include "macros.h"
#include "syscall.h"

// If this is missing you need to run the "compile_x86.sh" at least once!
#include "zex_datagen.h"

#define Z80_CPU_SPEED           4000000   /* In Hz. */
#define CYCLES_PER_STEP         (Z80_CPU_SPEED / 50)


void debug(const char* msg)
{
  Serial.println(msg);
}

void debugn(int num)
{
  Serial.println(num, HEX);
}

char sbuf[100];

void dumpregs(ZEXTEST* context)
{
  
  sprintf(sbuf, "A: 0x%.2x "  , context->state.registers.byte[Z80_A]);
  Serial.print(sbuf);
  sprintf(sbuf, "F: 0x%.2x "  , context->state.registers.byte[Z80_F]);
  Serial.print(sbuf);
  sprintf(sbuf, "A': 0x%.2x " , context->state.alternates[Z80_AF]<<8);
  Serial.print(sbuf);
  sprintf(sbuf, "F': 0x%.2x\n", context->state.alternates[Z80_AF]&0xff);
  Serial.print(sbuf);

  sprintf(sbuf, "B: 0x%.2x "  , context->state.registers.byte[Z80_B]);
  Serial.print(sbuf);
  sprintf(sbuf, "C: 0x%.2x "  , context->state.registers.byte[Z80_C]);
  Serial.print(sbuf);
  sprintf(sbuf, "B': 0x%.2x " , context->state.alternates[Z80_BC]<<8);
  Serial.print(sbuf);
  sprintf(sbuf, "C': 0x%.2x\n", context->state.alternates[Z80_BC]&0xff);
  Serial.print(sbuf);

  sprintf(sbuf, "D: 0x%.2x "  , context->state.registers.byte[Z80_D]);
  Serial.print(sbuf);
  sprintf(sbuf, "E: 0x%.2x "  , context->state.registers.byte[Z80_E]);
  Serial.print(sbuf);
  sprintf(sbuf, "D': 0x%.2x " , context->state.alternates[Z80_DE]<<8);
  Serial.print(sbuf);
  sprintf(sbuf, "E': 0x%.2x\n", context->state.alternates[Z80_DE]&0xff);
  Serial.print(sbuf);

  sprintf(sbuf, "H: 0x%.2x "  , context->state.registers.byte[Z80_H]);
  Serial.print(sbuf);
  sprintf(sbuf, "L: 0x%.2x "  , context->state.registers.byte[Z80_L]);
  Serial.print(sbuf);
  sprintf(sbuf, "H': 0x%.2x " , context->state.alternates[Z80_HL]<<8);
  Serial.print(sbuf);
  sprintf(sbuf, "L': 0x%.2x\n", context->state.alternates[Z80_HL]&0xff);
  Serial.print(sbuf);

  // Do these actually get touched?
  sprintf(sbuf, "I: 0x%.2x "  , 0);
  Serial.print(sbuf);
  sprintf(sbuf, "R: 0x%.2x "  , 0);
  Serial.print(sbuf);
  
  sprintf(sbuf, "IX: 0x%.4x " , context->state.alternates[Z80_IX]);
  Serial.print(sbuf);
  sprintf(sbuf, "IY: 0x%.4x\n", context->state.alternates[Z80_HL]);
  Serial.print(sbuf);

  uint16_t pc=context->state.pc;
  sprintf(sbuf, "SP: 0x%.4x, PC: 0x%.4x\n", context->state.registers.word[Z80_SP], pc);
  Serial.print(sbuf);

  uint8_t f=context->state.registers.byte[Z80_F];
  
  sprintf(sbuf, "S: %d Z: %d Y: %d H: %d X: %d PV: %d N: %d C: %d\n",
	  (f&Z80_S_FLAG) !=0,
	  (f&Z80_Z_FLAG) !=0,
	  (f&Z80_Y_FLAG) !=0,
	  (f&Z80_H_FLAG) !=0,
	  (f&Z80_X_FLAG) !=0,
	  (f&Z80_PV_FLAG) !=0,
	  (f&Z80_N_FLAG) !=0,
	  (f&Z80_C_FLAG) !=0);
  Serial.print(sbuf);

  sprintf(sbuf, "%.4x: %.2x %.2x %.2x %.2x\n",
	  pc,
	  z80_read_byte(context, pc),
	  z80_read_byte(context, pc+1),
	  z80_read_byte(context, pc+2),
	  z80_read_byte(context, pc+3)
	  );
  Serial.print(sbuf);

}




/* Emulate "zexdoc.com" or "zexall.com". */

// Better of in global space seems to go up to 16k at least
ZEXTEST	context;

static int check_avail()
{
  return Serial.available();
}

static char readch()
{
  return Serial.read();
}

#include "initvals.h"
void setup()
{
  SystemCall_init(INITVALS_STARTPC, INITVALS_MACHINE, check_avail, debug, readch);


  Serial.begin(9600);

  long   	l;
  double 	total;
  
  for (int i=0;i<zexsize;i++)
    {
      context.rom[i] = zexbuff[i];
    }

    // only do the first test for speedup debugging
    /*
    context.memory[0x13c]=0;
    context.memory[0x13d]=0;
    */

    // Really simple counter test program
  /*
    int ptr=0;
    context.memory[ptr++] = 0x11; // ld de, 0
    context.memory[ptr++] = 0x00; 
    context.memory[ptr++] = 0x00; 
    context.memory[ptr++] = 0x06; // ld b, 0xff
    context.memory[ptr++] = 0xff;
    context.memory[ptr++] = 0x10; // hit: djnz hit
    context.memory[ptr++] = 0xfe; // -2 (from add 6)
    context.memory[ptr++] = 0x13; // inc de
    context.memory[ptr++] = 0xc3; // jp 3 reload b
    context.memory[ptr++] = 0x03; // 
    context.memory[ptr++] = 0x00; //
  */
  /* Emulate. */

  Z80Reset(&context.state);
  context.state.pc = SystemCall_start();
  total = 0.0;
}

// we need to jump out of here every now and then to avoid watchdog

void loop()
{
  // Jump out directly if sim has ended for now
  //
  if (context.is_done) return;
  double 	total;

  for (int i=0;i<200;i++)
    {
      // dumpregs(&context);
      // total += Z80Emulate(&context.state, 1, &context);
      total += Z80Emulate(&context.state, 10000, &context);
      // total += Z80Emulate(&context.state, CYCLES_PER_STEP, &context);
      if (context.is_done) return;
    }


  static int cnt=0;
  if (cnt++ == 300)
  {
    Serial.print("."); // Send some signs of life about each minute...
    cnt=0;
  }

  /*
    sprintf(sbuf, "\n%.0f cycle(s) emulated.\n" 
    "For a Z80 running at %.2fMHz, "
    "that would be %d second(s) or %.2f hour(s).\n",
    total,
    Z80_CPU_SPEED / 1000000.0,
    (int) (total / Z80_CPU_SPEED),
    total / ((double) 3600 * Z80_CPU_SPEED));
    Serial.print(sbuf);
  */
}

