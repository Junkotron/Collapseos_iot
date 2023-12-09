#/bin/bash

set -e

# just add -g 
GFLAGS=

function usage
{
    echo "usage: $0 <target>"
    echo " where:"
    echo "1 - the collapseos target"
    echo "2 - zexdoc opcode test.."
    echo "3 - zexall opcode test.."
}

if [ -z "$@" ]; then
    usage
    exit 1
fi

if [ -d z80emu ]; then
    echo "z80emu already exists so wont checkout again..."
else
    git clone https://github.com/anotherlin/z80emu.git
fi


# select 1 of three

case $1 in

  1)
      echo "1 selected"

      # todo if dir
      if [ -d collapseos-rc2014 ]; then
	  echo "collapseos-rc2014 already exists so wont checkout again..."
      else	
	  git clone https://git.sr.ht/~vdupras/collapseos-rc2014
      fi


      # build sio with modification 8k ram only at himem
      cat collapseos-rc2014/conf/sio.fs | sed 's/PS_ADDR $8000/PS_ADDR $c000/g' > collapseos-rc2014/conf/siohimem.fs
      
      cp collapseos-rc2014/conf/emul_sio.args collapseos-rc2014/conf/emul_siohimem.args
      pushd collapseos-rc2014
      make CONF=siohimem
      popd

      cat collapseos-rc2014/rc2014_siohimem.rom > tmp.rom

      echo "static const int INITVALS_STARTPC=0x0;" > emuz80ardu/initvals.h
      echo "static const int INITVALS_MACHINE=1;" >> emuz80ardu/initvals.h
    ;;

  2)
      echo "2 selected"
      cat comboot.rom z80emu/testfiles/zexdoc.com > tmp.rom      

      echo "static const int INITVALS_STARTPC=0x100;" > emuz80ardu/initvals.h
      echo "static const int INITVALS_MACHINE=2;" >> emuz80ardu/initvals.h
    ;;

  3)
    echo "3 selected"
    cat comboot.rom z80emu/testfiles/zexall.com > tmp.rom

    echo "static const int INITVALS_STARTPC=0x100;" > emuz80ardu/initvals.h
    echo "static const int INITVALS_MACHINE=3;" >> emuz80ardu/initvals.h
    ;;

  *)
      echo "Unknown choice $1"
      usage
      exit 1
    ;;
esac


set -e

# Fix the code inside an include...
gcc -o mk_com_boot mk_com_boot.c
gcc -o mk_inline_code mk_inline_code.c

./mk_com_boot > comboot.rom



# silly util cant take paths...
./mk_inline_code tmp.rom zex_datagen
mv zex_datagen.h emuz80ardu/

# x86 o-files go here
mkdir -p ardux86

# move required files from z80emu
cp z80emu/instructions.h z80emu/macros.h z80emu/tables.h z80emu/z80config.h z80emu/z80emu.c z80emu/z80emu.h emuz80ardu


gcc $GFLAGS -o ardux86/z80emu.o -c emuz80ardu/z80emu.c

g++ $GFLAGS -o ardux86/main.o -c main.cpp


gcc $GFLAGS -o ardux86/syscall.o -c emuz80ardu/syscall.c
gcc $GFLAGS -o ardux86/main ardux86/main.o ardux86/syscall.o ardux86/z80emu.o
