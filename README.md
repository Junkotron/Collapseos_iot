Making Collapseos run on an iot esp8266 intelligent lightbulb chip.

I started this as a middle way between the rather dry running of CollapseOS on an emulator under Linux and the super-ambitious way of building your own microcontroller system of vintage chips or home made radio tubes.

Here you could get collapse running on some bare metal of its own, soldering is optional.

In this project, i combined three things: the Arduino environment, the CollapseOS collapseos-rc2014 (from Virgil Dupras), and the Z80 Emulator, z80emu, (from "anotherlin" i.e. Lin Ke-Fong)

Currently, there is no Makefile.

Also, there has been no attempt to make the esp8266 self-replicant with collapseos.

There is a compiler script that does "everything", checks out parts from CollapseOS and z80emu, does some transformer rebuild magic and produces an executable for x86 (ardux86) and in the emuz80ardu/ directory puts files suitable for the limited Arduino build environment.

The command "./compile_x86.sh 1" should build an x86 binary in ardux86/main and make the arduino dir emuz80ardu compilable with the Arduino gui.

The shell handling on x86 is currently horrible since this was only ever used for debugging. The native collapseos-rc2014 emulator is better if that is what you want to do. For the arduino built-in console it does as expected

I do not go into any detail here about the arduino since there are better tutorials out there. Before you start scavenging light bulbs, though, I must warn you that the process of setting up the Arduino environment can be tedious. The best start would probably be to get an esp8266 starter kit from some 'duino shop make hello world compile and download on the device and then come back here for the .ino sketch.

Expect this speed demon to slightly exceed that of a ZX81, Speccy or Jupiter ACE running on Z80 @3.2768 MHz

Keeping the original memory access macros (in z80user_old.h) one could get about twice the speed but with the split memory structure of Collapse (ROM going into bottom and RAM into top) combined with some problem I had using a full 64k map on the ESP made me add a switch clause in the macros (z80user_new.h) which seemed to slow things down.
The memory structure is currently in zextest.h

Z80test, serious torture!!
Anotherlin has included two test suites for the z80 opcodes themselves which are rather merciless, it takes a few minutes to run on x86 decent laptop and hours on the esp target. These two "zexdoc" and "zexall" can be compiled via the script using arguments "2" and "3" respectively.

Windows? If you do your 'duinoing on Windows, one option might be to build your emuz80 on cygwin, you need make, gcc and g++. It might also work on those novelty Linux/bashista environments in windows but this has not been tried. 

Other 'duinos? It might be possible the popular esp32 might also work out of the box. The Uno, Nano and friends simply seem to have to little of everything, an heroic attempt was made by Mohamed Rashad with the ArduZ80 project, it can run simple Z80 programs on a Nano but does not have all opcodes.

Known bugs:

- Though there is some access checks on memory, the esp8266 crashes on some illegal accesses

- Shell is awful on x86


