#ifndef SYSCALL__H
#define SYSCALL__H

#ifdef __cplusplus
extern "C" {
#endif

#include "zextest.h"
#include "z80user.h"

typedef int (*check_keypress_fn_type)();
typedef void (*debug_fn_type)(const char*);
typedef char (*readch_fn_type)();

// mid is the "machine id" which should be the same as from the
// build system
void SystemCall_init(short pc, int mid,
		     check_keypress_fn_type fn1,
		     debug_fn_type fn2,
		     readch_fn_type fn3);
  // ...

unsigned short SystemCall_start();

unsigned char SystemCall_in (ZEXTEST *zextest, unsigned short port);

void SystemCall_out(ZEXTEST* context, unsigned short port, unsigned char val,
		    unsigned *numcp);

#ifdef __cplusplus
}
#endif

#endif
