
ARMGNU = arm-none-eabi
#ARMGNU = arm-linux-gnueabi

AOPS = --warn --fatal-warnings -mcpu=cortex-m4
COPS = -Wall -O2 -nostdlib -nostartfiles -ffreestanding 
 -mcpu=cortex-m0

all : spinner01.bin

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

seven.h : buildheader.c
	gcc -O2 buildheader.c -o buildheader
	./buildheader > seven.h

flash.o : flash.s
	$(ARMGNU)-as $(AOPS) flash.s -o flash.o

spinner01.o : spinner01.c seven.h
	$(ARMGNU)-gcc $(COPS) -mthumb -mcpu=cortex-m4 -c spinner01.c -o spinner01.o

spinner01.bin : flash.ld flash.o spinner01.o
	$(ARMGNU)-ld -o spinner01.elf -T flash.ld flash.o spinner01.o
	$(ARMGNU)-objdump -D spinner01.elf > spinner01.list
	$(ARMGNU)-objcopy spinner01.elf spinner01.bin -O binary

