
void PUT32 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );
void dummy ( unsigned int );

#define GPIOEBASE 0x40021000

#define GPIOE_MODER  (GPIOEBASE+0x00)
#define GPIOE_OTYPER (GPIOEBASE+0x04)
#define GPIOE_BSRR   (GPIOEBASE+0x18)

#define RCCBASE 0x40023800
#define RCC_CR      (RCCBASE+0x00)
#define RCC_CFGR    (RCCBASE+0x08)
#define RCC_AHB1ENR (RCCBASE+0x30)

#define STK_CSR 0xE000E010
#define STK_RVR 0xE000E014
#define STK_CVR 0xE000E018
#define STK_MASK 0x00FFFFFF

#define DELAYVALUE 32

static void clock_init ( void )
{
    unsigned int ra;

    ra=GET32(RCC_CR);
    ra|=(1<<16); //HSEON
    PUT32(RCC_CR,ra);
    //wait for HSERDY
    while(1)
    {
        ra=GET32(RCC_CR);
        if(ra&(1<<17)) break;
    }
    //switch to HSE
    ra=GET32(RCC_CFGR);
    ra&=~3;
    ra|=1;
    PUT32(RCC_CFGR,ra);
    //wait for it
    while(1)
    {
        ra=GET32(RCC_CFGR);
        if((ra&3)==1) break;
    }
}


int delay ( unsigned int n )
{
    unsigned int ra;

    while(n--)
    {
        while(1)
        {
            ra=GET32(STK_CSR);
            if(ra&(1<<16)) break;
        }
    }
    return(0);
}


int notmain ( void )
{
    unsigned int ra;
    unsigned int rx;

    clock_init();


    ra=GET32(RCC_AHB1ENR);
    ra|=1<<4; //enable port e
    PUT32(RCC_AHB1ENR,ra);

    ra=GET32(GPIOE_MODER);
    ra&=~(3<<(0<<1));
    ra|= (1<<(0<<1));
    PUT32(GPIOE_MODER,ra);

    ra=GET32(GPIOE_OTYPER);
    ra&=~(1<<0);
    PUT32(GPIOE_OTYPER,ra);

    PUT32(STK_CSR,4);
    PUT32(STK_RVR,1000000-1);
    PUT32(STK_CVR,0x00000000);
    PUT32(STK_CSR,5);

    for(rx=0;;rx++)
    {
        PUT32(GPIOE_BSRR, (1<<(0+ 0)) );
        delay(DELAYVALUE);
        PUT32(GPIOE_BSRR, (1<<(0+16)) );
        delay(DELAYVALUE);
    }
    return(0);
}

/*
dfu-util -a 0 -s 0x08000000 -D blinker03.bin
*/
