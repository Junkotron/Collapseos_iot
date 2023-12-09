
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>



int main(int argc, char *argv[])
{
  if (argc!=3)
    {
      fprintf(stderr, "Usage: %s <name of binary file> <name of generated include (excluding .h extension)>", argv[0]);
      exit(1);
    }

  FILE* binf = fopen(argv[1], "rb");
  if (binf == NULL)
    {
      fprintf(stderr, "Can't open file: ``%s''\n", argv[1]);
      exit(1);
    }

  // TODO: MAXPATH something
  char fname[4096];

  char macroupper[4096];

  sprintf(fname, "%s.h", argv[2]);

  int i;

  // Including terminating null...
  for (i=0;i<1+strlen(argv[2]);i++)
    {
      macroupper[i]=toupper(argv[2][i]);
    }

  FILE* incf = fopen(fname, "w");

  if (incf == NULL)
    {
      fprintf(stderr, "Can't open file: ``%s''\n", fname);
      exit(1);
    }

  fprintf(incf, "#ifndef %s__H\n", macroupper);
  fprintf(incf, "#define %s__H\n", macroupper);
  fprintf(incf, "\n");

  fseek(binf, 0, SEEK_END);
  int l = ftell(binf);

  fseek(binf, 0, SEEK_SET);
  // TODO: marker of zexsize automated
  fprintf(incf, "static const unsigned zexsize=%d;\n", l+256);
  fprintf(incf, "static const unsigned char zexbuff[] = {\n");

  fprintf(incf, "        ");
  
  int colcnt=0;  
  while (1)
    {
      unsigned char ch=fgetc(binf);
      if (feof(binf)) break;

      // TODO: some prepeek to not get a comma on last item...
      // easiest to just store whole binary in a buf
      fprintf(incf, "0x%.2x,", ch);

      colcnt++;
      
      if (colcnt==8)
	{
	  colcnt=0;
	  fprintf(incf, "\n");
	  fprintf(incf, "        ");
	}
      else
	{
	  fprintf(incf, " ");
	}

    }
  
  fprintf(incf, "\n");
  fprintf(incf, "};\n");
  fprintf(incf, "#endif");
}
