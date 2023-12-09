/* z80user.h
 * Add your code here to interface the emulated system with z80emu. See towards
 * the end of the file for an example for running zextest.
 *
 * Copyright (c) 2016, 2017 Lin Ke-Fong
 *
 * This code is free, do whatever you want with it.
 */

#ifndef __Z80USER_TOP_INCLUDED__
#define __Z80USER_TOP_INCLUDED__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "zextest.h"
#include "syscall.h"

// also needs changes in zextest.h
#include "z80user_new.h" 
//#include "z80user_old.h" 


#define Z80_INPUT_BYTE(port, x)                                         \
{                                                                       \
  (x) = SystemCall_in((ZEXTEST *) context, port);			\
}

#define Z80_OUTPUT_BYTE(port, val)					\
{                                                                       \
  SystemCall_out((ZEXTEST *) context, port, val, &number_cycles);	\
}



#endif
