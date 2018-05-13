
void PUT32 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );
void dummy ( unsigned int );

#define GPIOABASE 0x40020000
#define GPIOEBASE 0x40021000
#define RCCBASE 0x40023800

#include "seven.h"

static unsigned int num[4];

void delay ( void )
{
    unsigned int ra;
    for(ra=0;ra<0x1000;ra++) dummy(ra);
}

int notmain ( void )
{
    unsigned int ra;
    unsigned int rb;
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

    num[0]=1;
    num[1]=2;
    num[2]=3;
    num[3]=4;

    while(1)
    {
        for(rb=0;rb<10;rb++)
        {
            for(rx=0;rx<4;rx++)
            {
                PUT32(GPIOABASE+0x00,0xA8000000);
                PUT32(GPIOEBASE+0x18,seven[num[rx]]);
                PUT32(GPIOABASE+0x00,0xA8000000|(1<<(rx<<1)));
                delay();
            }
        }
        num[3]++;
        if(num[3]>9) { num[3]=0; num[2]++; }
        if(num[2]>9) { num[2]=0; num[1]++; }
        if(num[1]>9) { num[1]=0; num[0]++; }
        if(num[0]>9) { num[0]=0;  }
    }
    return(0);
}


/*
dfu-util -a 0 -s 0x08000000 -D notmain.bin

*/


