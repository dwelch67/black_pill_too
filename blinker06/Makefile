
ARMGNU = arm-none-eabi
#ARMGNU = arm-linux-gnueabi

AOPS = --warn --fatal-warnings
COPS = -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding 

all : blinker06.bin

clean:
	rm -f *.bin
	rm -f *.o
	rm -f *.elf
	rm -f *.list
	rm -f *.bc
	rm -f *.opt.s
	rm -f *.norm.s
	rm -f *.hex

#---------------------------------

flash.o : flash.s
	$(ARMGNU)-as $(AOPS) flash.s -o flash.o

blinker06.o : blinker06.c
	$(ARMGNU)-gcc $(COPS) -mthumb -mcpu=cortex-m0 -march=armv6-m -c blinker06.c -o blinker06.o

blinker06.bin : flash.ld flash.o blinker06.o
	$(ARMGNU)-ld -o blinker06.elf -T flash.ld flash.o blinker06.o
	$(ARMGNU)-objdump -D blinker06.elf > blinker06.list
	$(ARMGNU)-objcopy blinker06.elf blinker06.bin -O binary

