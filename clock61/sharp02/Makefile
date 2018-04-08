
ARMGNU = arm-none-eabi
#ARMGNU = arm-linux-gnueabi

AOPS = --warn --fatal-warnings -mcpu=cortex-m3
COPS = -Wall -O2 -nostdlib -nostartfiles -ffreestanding 
 -mcpu=cortex-m0

all : sharp02.bin

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

sharp02.o : sharp02.c
	$(ARMGNU)-gcc $(COPS) -mthumb -mcpu=cortex-m0 -march=armv6-m -c sharp02.c -o sharp02.o

sharp02.bin : flash.ld flash.o sharp02.o
	$(ARMGNU)-ld -o sharp02.elf -T flash.ld flash.o sharp02.o
	$(ARMGNU)-objdump -D sharp02.elf > sharp02.list
	$(ARMGNU)-objcopy sharp02.elf sharp02.bin -O binary

