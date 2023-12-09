/* zextest.h
 * Header for zextest example.
 *
 * Copyright (c) 2012, 2016 Lin Ke-Fong
 *
 * This code is free, do whatever you want with it.
 */

#ifndef __ZEXTEST_INCLUDED__
#define __ZEXTEST_INCLUDED__

#include "z80emu.h"

typedef struct ZEXTEST {
  Z80_STATE	state;

  // to use with z80user_old.h
  // unsigned char	rom[1 << 16];
  
  
  // lower 16k
  unsigned char	rom[1 << 14];

  // highest 8k
  unsigned char	ram[1 << 14];
  
  int 		is_done;

} ZEXTEST;

extern void     SystemCall (ZEXTEST *zextest);

#endif
