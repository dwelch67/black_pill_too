
void PUT32 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );
void dummy ( unsigned int );

#define GPIOEBASE 0x40021000
#define RCCBASE 0x40023800

int notmain ( void )
{
    unsigned int ra;
    unsigned int rx;

    ra=GET32(RCCBASE+0x30);
    ra|=1<<4; //enable port e
    PUT32(RCCBASE+0x30,ra);

    //moder
    ra=GET32(GPIOEBASE+0x00);
    ra&=~(3<<(0<<1));
    ra|= (1<<(0<<1));
    PUT32(GPIOEBASE+0x00,ra);
    //OTYPER
    ra=GET32(GPIOEBASE+0x04);
    ra&=~(1<<0); 
    PUT32(GPIOEBASE+0x04,ra);

    for(rx=0;;rx++)
    {
        PUT32(GPIOEBASE+0x18, (1<<(0+ 0)) );
        for(ra=0;ra<200000;ra++) dummy(ra);
        PUT32(GPIOEBASE+0x18, (1<<(0+16)) );
        for(ra=0;ra<200000;ra++) dummy(ra);
    }
    return(0);
}

/*
dfu-util -a 0 -s 0x08000000 -D blinker01.bin
*/

