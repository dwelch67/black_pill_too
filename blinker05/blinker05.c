
void PUT32 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );
void dummy ( unsigned int );

#define GPIOEBASE 0x40021000

#define GPIOE_MODER  (GPIOEBASE+0x00)
#define GPIOE_OTYPER (GPIOEBASE+0x04)
#define GPIOE_BSRR   (GPIOEBASE+0x18)

#define RCCBASE 0x40023800
#define RCC_CR      (RCCBASE+0x00)
#define RCC_PLLCFGR (RCCBASE+0x04)
#define RCC_CFGR    (RCCBASE+0x08)
#define RCC_AHB1ENR (RCCBASE+0x30)

#define FLASH_BASE  0x40023C00
#define FLASH_ACR   (FLASH_BASE+0x00)

#define STK_CSR     0xE000E010
#define STK_RVR     0xE000E014
#define STK_CVR     0xE000E018
#define STK_MASK    0x00FFFFFF

#define DELAYVALUE 96

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
    //now using external clock.
    //----------

/*


*/
    if(1)
    {
        //should already be off
        ra=GET32(RCC_CR);
        ra&=(~(1<<24)); //PLLON
        PUT32(RCC_CR,ra);
        //wait for PLLRDY off
        while(1)
        {
            ra=GET32(RCC_CR);
            if((ra&(1<<25))==0) break;
        }
    }

    //PLLM 4
    //PLLN 96
    //PLLQ 4
    //PLLP 4

    //PPRE2 <90 not divided
    //PPRE1 <45 divide by 2
    //HPRE not divided

    //1 WS aggressive
    //2 WS conservative


    ra=GET32(RCC_PLLCFGR);
    ra&=(~(0xF<<24)); //PLLQ
    ra|=( (  4<<24)); //PLLQ

    ra&=(~(0x1<<22)); //PLLSRC
    ra|=( (  1<<22)); //PLLSRC

    ra&=(~(0x3<<16)); //PLLP
    ra|=( (0x1<<16)); //PLLP //0b01 -> 4

    ra&=(~(0x1FF<<6)); //PLLN
    ra|=( (   96<<6)); //PLLN

    ra&=(~(0x3F<<0)); //PLLM
    ra|=( (   4<<0)); //PLLM
    PUT32(RCC_PLLCFGR,ra);


    ra=GET32(RCC_CFGR);
    ra&=(~(0x1F<<16)); //RTCPRE
    ra|=( (   8<<16)); //RTCPRE

    ra&=(~(0x7<<13)); //PPRE2
    ra|=( (0x0<<13)); //PPRE2 0b000 not divided

    ra&=(~(0x7<<10)); //PPRE1
    ra|=( (0x4<<10)); //PPRE1 0b100 -> 2

    ra&=(~(0xF<<10)); //HPRE
    ra|=( (0x0<<10)); //HPRE 0b0000 -> no divisor

    PUT32(RCC_CFGR,ra);

    ra=GET32(RCC_CR);
    ra|=(1<<24); //PLLON
    PUT32(RCC_CR,ra);
    //wait for PLLRDY
    while(1)
    {
        ra=GET32(RCC_CR);
        if((ra&(1<<25))!=0) break;
    }

    //if we overclock the flash we can crash
    ra=GET32(FLASH_ACR);
    ra|=0x00000001; //aggresive setting
    //ra|=0x00000002; //more conservative setting
    PUT32(FLASH_ACR,ra);

    //switch to PLL
    ra=GET32(RCC_CFGR);
    ra&=~3;
    ra|= 2;
    PUT32(RCC_CFGR,ra);
    //wait for it
    while(1)
    {
        ra=GET32(RCC_CFGR);
        if((ra&3)==2) break;
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
dfu-util -a 0 -s 0x08000000 -D blinker05.bin
*/

