/* z80user.h
 * Add your code here to interface the emulated system with z80emu. See towards
 * the end of the file for an example for running zextest.
 *
 * Copyright (c) 2016, 2017 Lin Ke-Fong
 *
 * This code is free, do whatever you want with it.
 */

#ifndef __Z80USER_NEW_INCLUDED__
#define __Z80USER_NEW_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

  /* Write the following macros for memory access and input/output on the Z80. 
   *
   * Z80_FETCH_BYTE() and Z80_FETCH_WORD() are used by the emulator to read the
   * code (opcode, constants, displacement, etc). The upper 16-bit of the address
   * parameters is undefined and must be reset to zero before actually reading 
   * memory (use & 0xffff). The value x read, must be an unsigned 8-bit or 16-bit
   * value in the endianness of the host processor. 
   *
   * Z80_READ_BYTE(), Z80_WRITE_BYTE(), Z80_READ_WORD(), and Z80_WRITE_WORD()
   * are used for general memory access. They obey the same rules as the code 
   * reading macros. The upper bits of the value x to write may be non-zero.
   * Z80_READ_WORD_INTERRUPT() and Z80_WRITE_WORD_INTERRUPT() are same as 
   * respectively Z80_READ_WORD() and Z80_WRITE_WORD(), except they are only used
   * for interrupt generation.
   * 
   * Z80_INPUT_BYTE() and Z80_OUTPUT_BYTE() are for input and output. The upper
   * bits of the port number to read or write are always zero. The input byte x 
   * must be an unsigned 8-bit value. The value x to write is an unsigned 8-bit 
   * with its upper bits zeroed.
   *
   * All macros have access to the following three variables:
   *
   *      state           Pointer to the current Z80_STATE. Because the 
   *			instruction is currently executing, its members may not
   *			be fully up to date, depending on when the macro is 
   *			called in the process. It is rather suggested to access 
   *			the state only when the emulator is stopped. 
   *
   *      elapsed_cycles  Number of cycles emulated. If needed, you may add wait 
   *			states to it for slow memory accesses. Because the 
   *			macros are called during the execution of the current 
   *			instruction, this number is only precise up to the 
   *			previous one.
   *
   *	context 	This is the (void *) context passed to the emulation 
   *			functions.
   *
   * Except for Z80_READ_WORD_INTERRUPT and Z80_WRITE_WORD_INTERRUPT, all macros 
   * also have access to: 
   *
   *      number_cycles   Number of cycles to emulate. After executing each
   *			instruction, the emulator checks if elapsed_cycles is
   *			greater or equal to number_cycles, and will stops if 
   *			so. Hence you may decrease or increase the value of 
   *			number_cycles to stop the emulation earlier or later.
   * 			In particular, if you set it to zero, the emulator will
   * 			stop after completion of the current instruction. 
   *
   *      registers       Current register decoding table, use it to determine if
   * 			the current instruction is prefixed. It points on:
   *                      
   *				state->dd_register_table for 0xdd prefixes; 
   *                      	state->fd_register_table for 0xfd prefixes;
   *				state->register_table otherwise.
   *
   *      pc              Current PC register (upper bits are undefined), points
   *                      on the opcode, the displacement or constant to read for
   *                      Z80_FETCH_BYTE() and Z80_FETCH_WORD(), or on the next
   *                      instruction otherwise.
   *
   * Except for Z80_FETCH_BYTE(), Z80_FETCH_WORD(), Z80_READ_WORD_INTERRUPT, and 
   * Z80_WRITE_WORD_INTERRUPT, all other macros can know which instruction is 
   * currently executing:
   *
   *      opcode          Opcode of the currently executing instruction.
   *
   *      instruction     Type of the currently executing instruction, see
   *                      instructions.h for a list.
   */

  /* Here are macros for emulating zexdoc and zexall. Read/write memory macros
   * are written for a linear 64k RAM. Input/output port macros are used as 
   * "traps" to simulate system calls. 
   */

static const uint16_t ramstart=0xc000;

static uint8_t * getmemoff(ZEXTEST* context, uint16_t address, uint16_t* offset)
{
  switch (address >> 14)
    {
    case 0:
      {
	*offset=0;
	return context->rom;
      }
    case 3:
      {
	*offset = address - ramstart;
	return context->ram;
      }
    default:
      {
	fprintf(stderr, "Memory access error at: %.4x\n", address);
	exit(1);
      }
    }
}


static uint8_t z80_read_byte(ZEXTEST* context, uint16_t address)                {
  uint16_t offset;                                                                uint8_t* memp = getmemoff(context, address, &offset);                           return memp[(address-offset) & 0xffff];                                       }
  

#define Z80_READ_BYTE(address, x)                                       \
{                                                                       \
  (x) =									\
    address>=ramstart ?						\
    ((ZEXTEST *) context)->ram[(address-ramstart) & 0xffff]	\
    :									\
    address<0x4000 ?							\
    ((ZEXTEST *) context)->rom[(address) & 0xffff]			\
    :									\
    z80_read_byte(context, address);					\
}

  

#define Z80_READ_WORD(address, x)                                       \
{                                                                       \
 unsigned char	*memory;						\
 if (address<0x3fff)							\
   {									\
     memory = ((ZEXTEST *) context)->rom;				\
     (x)=   memory[(address) & 0xffff]					\
       | (memory[((address) + 1) & 0xffff] << 8);			\
   }									\
 else if (address>=ramstart && address<0xffff) 				\
   {									\
     memory = ((ZEXTEST *) context)->ram;				\
     (x) =   memory[(address-ramstart) & 0xffff]			\
       | (memory[((address-ramstart) + 1) & 0xffff] << 8);		\
   }									\
 else									\
   {									\
     z80_read_word(context, address);					\
   }									\
}

  
#define Z80_FETCH_BYTE(address, x) Z80_READ_BYTE((address), (x))

  
static uint16_t z80_read_word(ZEXTEST* context, uint16_t address)
{
  unsigned char	*memp1, *memp2;
	uint16_t offset;
	uint16_t x;
	memp1=getmemoff(context, address, &offset);
	memp2=getmemoff(context, address+1, &offset);

	x = memp1[(address-offset) & 0xffff]                                
	  | (memp2[((address-offset) + 1) & 0xffff] << 8);
	return x;
}


#define Z80_FETCH_WORD(address, x) Z80_READ_WORD((address), (x))

  
static void z80_write_byte(ZEXTEST* context, int address, uint8_t x)
{ 
 if (address<=0x3fff)							
   {									
     ((ZEXTEST *) context)->rom[(address) & 0xffff] = (x);		
   }									
 else if (address>=ramstart)						
   {									
     ((ZEXTEST *) context)->ram[(address-ramstart) & 0xffff] = (x);	
   }
 else
   {
     fprintf(stderr, "Memory access error at: %.4x\n", address);
     exit(1);
   }
}

#define Z80_WRITE_BYTE(address, x) z80_write_byte(context, address, x);


static void z80_write_word(ZEXTEST* context, int address, uint16_t x)
{
  if (address<0x3fff)							
    {									
      unsigned char	*memory;					
									
      memory = ((ZEXTEST *) context)->rom;				
      memory[(address) & 0xffff] = (x); 				
      memory[((address) + 1) & 0xffff] = (x) >> 8; 			
    }									
 else if (address>=ramstart && address<0xffff)
   {									
      unsigned char	*memory;					
									
      memory = ((ZEXTEST *) context)->ram;
      memory[(address-ramstart) & 0xffff] = (x);
      memory[((address-ramstart) + 1) & 0xffff] = (x) >> 8;
   }
 else
   {
     fprintf(stderr, "Memory access error at: %.4x\n", address);
     exit(1);
   }
}


  
#define Z80_WRITE_WORD(address, x) z80_write_word(context, address, x);
  
  
#define Z80_READ_WORD_INTERRUPT(address, x)	Z80_READ_WORD((address), (x))

#define Z80_WRITE_WORD_INTERRUPT(address, x)	Z80_WRITE_WORD((address), (x))



#ifdef __cplusplus
}
#endif

#endif
