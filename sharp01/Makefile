
ARMGNU = arm-none-eabi
#ARMGNU = arm-linux-gnueabi

AOPS = --warn --fatal-warnings -mcpu=cortex-m3
COPS = -Wall -O2 -nostdlib -nostartfiles -ffreestanding 
 -mcpu=cortex-m0

all : sharp01.bin

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

sharp01.o : sharp01.c
	$(ARMGNU)-gcc $(COPS) -mthumb -mcpu=cortex-m0 -march=armv6-m -c sharp01.c -o sharp01.o

sharp01.bin : flash.ld flash.o sharp01.o
	$(ARMGNU)-ld -o sharp01.elf -T flash.ld flash.o sharp01.o
	$(ARMGNU)-objdump -D sharp01.elf > sharp01.list
	$(ARMGNU)-objcopy sharp01.elf sharp01.bin -O binary

