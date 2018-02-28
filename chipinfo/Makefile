
ARMGNU = arm-none-eabi
#ARMGNU = arm-linux-gnueabi

AOPS = --warn --fatal-warnings
COPS = -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding 
 -mcpu=cortex-m0

all : chipinfo.bin

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

chipinfo.o : chipinfo.c
	$(ARMGNU)-gcc $(COPS) -mthumb -mcpu=cortex-m0 -march=armv6-m -c chipinfo.c -o chipinfo.o

chipinfo.bin : flash.ld flash.o chipinfo.o
	$(ARMGNU)-ld -o chipinfo.elf -T flash.ld flash.o chipinfo.o
	$(ARMGNU)-objdump -D chipinfo.elf > chipinfo.list
	$(ARMGNU)-objcopy chipinfo.elf chipinfo.bin -O binary

