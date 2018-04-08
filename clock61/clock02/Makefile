
ARMGNU = arm-none-eabi
#ARMGNU = arm-linux-gnueabi

AOPS = --warn --fatal-warnings -mcpu=cortex-m4
COPS = -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding 
 -mcpu=cortex-m0

all : clock02.bin

clean:
	rm -f *.bin
	rm -f *.o
	rm -f *.elf
	rm -f *.list
	rm -f *.bc
	rm -f *.opt.s
	rm -f *.norm.s
	rm -f *.hex
	rm -f buildheader

#---------------------------------

twostuff.h : buildheader.c
	gcc -O2 buildheader.c -o buildheader
	./buildheader > twostuff.h

flash.o : flash.s
	$(ARMGNU)-as $(AOPS) flash.s -o flash.o

clock02.o : clock02.c twostuff.h
	$(ARMGNU)-gcc $(COPS) -mthumb -mcpu=cortex-m4 -c clock02.c -o clock02.o

clock02.bin : flash.ld flash.o clock02.o
	$(ARMGNU)-ld -o clock02.elf -T flash.ld flash.o clock02.o
	$(ARMGNU)-objdump -D clock02.elf > clock02.list
	$(ARMGNU)-objcopy clock02.elf clock02.bin -O binary

