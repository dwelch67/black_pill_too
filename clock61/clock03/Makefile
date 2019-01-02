
ARMGNU = arm-none-eabi
#ARMGNU = arm-linux-gnueabi

AOPS = --warn --fatal-warnings -mcpu=cortex-m4
COPS = -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding 
 -mcpu=cortex-m0

all : clock03.bin

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

twostuff.h : buildheader.c
	gcc -O2 buildheader.c -o buildheader
	./buildheader > twostuff.h

flash.o : flash.s
	$(ARMGNU)-as $(AOPS) flash.s -o flash.o

clock03.o : clock03.c twostuff.h
	$(ARMGNU)-gcc $(COPS) -mthumb -mcpu=cortex-m4 -c clock03.c -o clock03.o

clock03.bin : flash.ld flash.o clock03.o
	$(ARMGNU)-ld -o clock03.elf -T flash.ld flash.o clock03.o
	$(ARMGNU)-objdump -D clock03.elf > clock03.list
	$(ARMGNU)-objcopy clock03.elf clock03.bin -O binary

