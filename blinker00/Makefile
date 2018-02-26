
ARMGNU ?= arm-none-eabi
#ARMGNU ?= arm-linux-gnueabi

AOPS = --warn --fatal-warnings -mcpu=cortex-m0

all : blinker00.bin

clean:
	rm -f *.bin
	rm -f *.o
	rm -f *.elf
	rm -f *.list

#---------------------------------

flash.o : flash.s
	$(ARMGNU)-as $(AOPS) flash.s -o flash.o

blinker00.bin :     flash.ld flash.o
	$(ARMGNU)-ld -T flash.ld flash.o -o blinker00.elf
	$(ARMGNU)-objdump -D blinker00.elf > blinker00.list
	$(ARMGNU)-objcopy blinker00.elf blinker00.bin -O binary

