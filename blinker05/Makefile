
ARMGNU = arm-none-eabi
#ARMGNU = arm-linux-gnueabi

AOPS = --warn --fatal-warnings -mcpu=cortex-m3
COPS = -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding 
 -mcpu=cortex-m0

all : blinker05.bin

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

blinker05.o : blinker05.c
	$(ARMGNU)-gcc $(COPS) -mthumb -mcpu=cortex-m0 -march=armv6-m -c blinker05.c -o blinker05.o

blinker05.bin : flash.ld flash.o blinker05.o
	$(ARMGNU)-ld -o blinker05.elf -T flash.ld flash.o blinker05.o
	$(ARMGNU)-objdump -D blinker05.elf > blinker05.list
	$(ARMGNU)-objcopy blinker05.elf blinker05.bin -O binary

