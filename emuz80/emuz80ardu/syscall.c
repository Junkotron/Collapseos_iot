
// Implementation of z80 syscalls i.e. in and out 


#include "syscall.h"
#include "zextest.h"
#include "z80user.h"

static unsigned short pcstart;

#define MAXIMUM_STRING_LENGTH   100

static check_keypress_fn_type check_keypress_fn;
static debug_fn_type debug_fn;
static readch_fn_type readch_fn;
static int machine_id=0;

void SystemCall_init(short pc, int mid,
		     check_keypress_fn_type fn1,
		     debug_fn_type fn2,
		     readch_fn_type fn3)
{
  pcstart=pc;
  machine_id=mid;
  check_keypress_fn=fn1;
  debug_fn=fn2;
  readch_fn=fn3;
}

unsigned short SystemCall_start(){ return pcstart; }


/* Emulate CP/M bdos call 5 functions 2 (output character on screen) and 9
 * (output $-terminated string to screen).
 */

static unsigned char SystemCall_in_test (ZEXTEST *zextest, unsigned short port)
{
        if (zextest->state.registers.byte[Z80_C] == 2)

	  printf("%c", zextest->state.registers.byte[Z80_E]);

        else if (zextest->state.registers.byte[Z80_C] == 9) {

                int     i, c;

                for (i = zextest->state.registers.word[Z80_DE], c = 0; 
		     z80_read_byte(zextest, i) != '$';
                        i++) {

		  printf("%c", z80_read_byte(zextest, i & 0xffff));
                        if (c++ > MAXIMUM_STRING_LENGTH) {

			  // how to print to serial?
			  // function pointer? 
			  for (;;) ; // todo: Tjong pang
			  /*
                                fprintf(stderr,
                                        "String to print is too long!\n");
				exit(EXIT_FAILURE);
			  */
                        }

                }

        }
	return 0;
}

static void SystemCall_out_test(ZEXTEST* context, unsigned short port, unsigned char val,
			 unsigned *numcp)
{
  ((ZEXTEST *) context)->is_done = !0;				 
  *numcp = 0;                                              
}



static unsigned char SystemCall_in_collapse (ZEXTEST *zextest, unsigned short port)
{
  // This tells sio driver we are not transmitting
  //printf("in %.4x requested...\n", port);
  if (port==0x80) {

    if (check_keypress_fn == NULL)
      {
	return 0;
      }
    
    // check stdin here
    if (check_keypress_fn())
      {
	return 5; // we are ready to take chars and also at least one in buffer
      }
    else
      return 4; // nothing to read just ready to take output
  }

  if (port==0x81) {
    //    char ch=getchar();
    char ch=readch_fn();
    return ch;
  }

  // All other ports some dumb reply for now
  return 42;
}

static void SystemCall_out_collapse(ZEXTEST* context, unsigned short port, unsigned char val, unsigned *numcp)
{
  if (port == 0x81)
    {
      printf("%c", val);
      fflush(stdout);
    }
}


unsigned char SystemCall_in(ZEXTEST *zextest, unsigned short port)
{
  switch(machine_id)
    {
    case 1:
      return SystemCall_in_collapse(zextest, port);
      break;
    case 2:
    case 3:
      return SystemCall_in_test(zextest, port);
      break;
    }
  return 0;
}

void SystemCall_out(ZEXTEST* context, unsigned short port, unsigned char val, unsigned *numcp)
{
  switch(machine_id)
    {
    case 1:
      SystemCall_out_collapse(context, port, val, numcp);
      break;
    case 2:
    case 3:
      SystemCall_out_test(context, port, val, numcp);
      break;
    }
}
