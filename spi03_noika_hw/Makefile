
ARMGNU ?= arm-none-eabi
#ARMGNU = arm-linux-gnueabi

AOPS = --warn --fatal-warnings -mcpu=cortex-m4
COPS = -Wall -O2 -nostdlib -nostartfiles -ffreestanding  -mcpu=cortex-m4

all : notmain.bin

clean:
	rm -f *.bin
	rm -f *.o
	rm -f *.elf
	rm -f *.list

#---------------------------------

flash.o : flash.s
	$(ARMGNU)-as $(AOPS) flash.s -o flash.o

notmain.o : notmain.c
	$(ARMGNU)-gcc $(COPS) -mthumb -c notmain.c -o notmain.o

notmain.bin :       flash.ld flash.o notmain.o
	$(ARMGNU)-ld -T flash.ld flash.o notmain.o -o notmain.elf
	$(ARMGNU)-objdump -D notmain.elf > notmain.list
	$(ARMGNU)-objcopy notmain.elf notmain.bin -O binary


