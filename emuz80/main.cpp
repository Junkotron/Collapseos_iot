// If we get any name collisions a lot of this can be hid
// by putting it after include .ino

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <stdint.h>



// We only do the most essential here
// things need adding here if we do more print types
const int HEX=16;

uint32_t millis(){
  //fprintf(stderr, "millis() write me!\n");
  return 42;
//  exit(1);
}

class EmulSerial {
public:

  // TODO: this emulation is not very good but useable
  int available()
  {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    fd_set savefds = readfds;
    
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    
    int chr;
    
    int sel_rv = select(1, &readfds, NULL, NULL, &timeout);
    
    return sel_rv;
  }

  char read() { return getchar(); }
  
  void begin(int baud) {}
  
  void print(const char *msg) { printf("%s", msg); }
  void println(const char *msg) { printf("%s\n", msg); }

  void println(uint16_t num)
  {
    printf("%d\n", num);
  }

  // Just HEX for now
  void println(uint16_t num, int base)
  {
    printf("%x\n", num);
  }
  
};

EmulSerial Serial;

#include "emuz80ardu/emuz80ardu.ino"

extern ZEXTEST	context;

int main()
{
  setup();
  while (1)
    {
      loop();

      // An "artificial" trick to stop execution
      if (context.is_done) break;
    }
}
