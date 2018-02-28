
ARMGNU = arm-none-eabi
#ARMGNU = arm-linux-gnueabi

AOPS = --warn --fatal-warnings -mcpu=cortex-m0
COPS = -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding 
 -mcpu=cortex-m0

all : blinker07.bin

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

blinker07.o : blinker07.c
	$(ARMGNU)-gcc $(COPS) -mthumb -mcpu=cortex-m0 -march=armv6-m -c blinker07.c -o blinker07.o

blinker07.bin : flash.ld flash.o blinker07.o
	$(ARMGNU)-ld -o blinker07.elf -T flash.ld flash.o blinker07.o
	$(ARMGNU)-objdump -D blinker07.elf > blinker07.list
	$(ARMGNU)-objcopy blinker07.elf blinker07.bin -O binary

