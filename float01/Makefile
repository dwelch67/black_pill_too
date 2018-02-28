
ARMGNU = arm-none-eabi
#ARMGNU = arm-linux-gnueabi

AOPS = --warn --fatal-warnings
COPS = -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding -mcpu=cortex-m4 -mfpu=vfp -mfloat-abi=hard
  -mcpu=cortex-m0 -march=armv6-m
all : float01.bin

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

float01.o : float01.c
	$(ARMGNU)-gcc $(COPS) -mthumb -c float01.c -o float01.o

ftest.o : ftest.c
	$(ARMGNU)-gcc $(COPS) -mthumb -c ftest.c -o ftest.o

float01.bin :                          flash.ld flash.o float01.o ftest.o
	$(ARMGNU)-ld -o float01.elf -T flash.ld flash.o float01.o ftest.o
	$(ARMGNU)-objdump -D float01.elf > float01.list
	$(ARMGNU)-objcopy float01.elf float01.bin -O binary

