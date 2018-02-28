
ARMGNU = arm-none-eabi
#ARMGNU = arm-linux-gnueabi

AOPS = --warn --fatal-warnings -mcpu=cortex-m0
COPS = -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding 
 -mcpu=cortex-m0

all : uart02.bin

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

uart02.o : uart02.c
	$(ARMGNU)-gcc $(COPS) -mthumb -mcpu=cortex-m0 -march=armv6-m -c uart02.c -o uart02.o

uart02.bin : flash.ld flash.o uart02.o
	$(ARMGNU)-ld -o uart02.elf -T flash.ld flash.o uart02.o
	$(ARMGNU)-objdump -D uart02.elf > uart02.list
	$(ARMGNU)-objcopy uart02.elf uart02.bin -O binary

