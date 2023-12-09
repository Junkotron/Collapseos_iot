
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>



int main(int argc, char *argv[])
{
  int i;

  FILE* incf = stdout;


  // simple counter program somwtimes useful
  /*
  const int pgm0[] =
    {
     0xd3, 0x00, // out (n),a  ; finish
     0x00, 0x00, 0x00, // padding (nops)
     0xdb, 0x00, // in a,(n)
     0xc9, // ret
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // pad to even 8
     -1
    };
  */

  // Now we do the 256 first bytes with the bdos patching


  const int pgm0[] =
    {
     0xd3, 0x00, // out (n),a  ; finish
     0x00, 0x00, 0x00, // padding (nops)
     0x18, 0x01, // jr skip ; stack now starts at 0x3f01
     0x3f,      // 
     0xdb, 0x00, // skip: in a,(n)
     0xc9, // ret
     0x00, 0x00, 0x00, 0x00, 0x00, // pad to even 8
     -1
    };

  for (i=0; pgm0[i] != -1; i++)
    {
      fprintf(incf, "%c", pgm0[i]);
    }

  
  // Zero pad up until org 100h
  for (i=0;i<30*8;i++)
    {
      fprintf(incf, "%c", 0);
    }

}
