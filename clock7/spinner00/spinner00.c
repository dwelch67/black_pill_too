
void PUT32 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );
void dummy ( unsigned int );

#define GPIOABASE 0x40020000
#define GPIOEBASE 0x40021000
#define RCCBASE 0x40023800

#include "seven.h"

void delay ( void )
{
    unsigned int ra;
    for(ra=0;ra<0x80000;ra++) dummy(ra);
}

int notmain ( void )
{
    unsigned int ra;
    unsigned int rx;

    ra=GET32(RCCBASE+0x30);
    ra|=1<<4; //enable port e
    ra|=1<<0; //enable port a
    PUT32(RCCBASE+0x30,ra);


    //moder
    PUT32(GPIOABASE+0x00,0xA8000000);
    //OTYPER
    PUT32(GPIOABASE+0x04,0x00000000);
    //BSRR
    PUT32(GPIOABASE+0x18,0x00FF0000);

    //moder
    PUT32(GPIOEBASE+0x00,0x00005555);
    //OTYPER
    PUT32(GPIOEBASE+0x04,0x00000000);
    //BSRR
    PUT32(GPIOEBASE+0x18,0x00FF0000);

    while(1)
    {
        for(rx=0;rx<4;rx++)
        {
            PUT32(GPIOABASE+0x00,0xA8000000|(1<<(rx<<1)));
            for(ra=0;ra<10;ra++)
            {
                PUT32(GPIOEBASE+0x18,seven[ra]);
                delay();
            }
        }
    }
    return(0);
}


/*
dfu-util -a 0 -s 0x08000000 -D spinner00.bin

*/


