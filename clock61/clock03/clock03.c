
void PUT32 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );
void dummy ( unsigned int );

#define GPIOABASE 0x40020000
#define GPIOA_MODER  (GPIOABASE+0x00)
#define GPIOA_OTYPER (GPIOABASE+0x04)
#define GPIOA_AFRH   (GPIOABASE+0x24)

#define GPIOEBASE 0x40021000
#define GPIOE_MODER  (GPIOEBASE+0x00)
#define GPIOE_OTYPER (GPIOEBASE+0x04)
#define GPIOE_BSRR   (GPIOEBASE+0x18)

#define RCCBASE         0x40023800
#define RCC_CR          (RCCBASE+0x00)
#define RCC_PLLCFGR     (RCCBASE+0x04)
#define RCC_CFGR        (RCCBASE+0x08)
#define RCC_AHB1ENR     (RCCBASE+0x30)
#define RCC_APB2ENR     (RCCBASE+0x44)
#define RCC_APB2RSTR    (RCCBASE+0x24)

#define FLASH_BASE  0x40023C00
#define FLASH_ACR   (FLASH_BASE+0x00)

#define USART1BASE      0x40011000
#define USART1_SR       (USART1BASE+0x00)
#define USART1_DR       (USART1BASE+0x04)
#define USART1_BRR      (USART1BASE+0x08)
#define USART1_CR1      (USART1BASE+0x0C)

#define STK_CSR     0xE000E010
#define STK_RVR     0xE000E014
#define STK_CVR     0xE000E018
#define STK_MASK    0x00FFFFFF

#include "twostuff.h"

static const unsigned char fives[60]=
{
    2,1,1,1,1,
    2,1,1,1,1,
    2,1,1,1,1,
    2,1,1,1,1,
    2,1,1,1,1,
    2,1,1,1,1,
    2,1,1,1,1,
    2,1,1,1,1,
    2,1,1,1,1,
    2,1,1,1,1,
    2,1,1,1,1,
    2,1,1,1,1,
};


static unsigned char xstring[32];

volatile unsigned int hour;
volatile unsigned int minute;
volatile unsigned int second;
volatile unsigned int counter;
volatile unsigned int sec;
void systick_handler ( void )
{
    if(counter==0)
    {
        sec++; if(sec>59) sec=0;
        PUT32(GPIOEBASE+0x00,0x28200000);
        PUT32(GPIOEBASE+0x18,0x00FF0000);
        PUT32(GPIOEBASE+0x00,twostuff[sec][0]);
        PUT32(GPIOEBASE+0x18,twostuff[sec][1]);
        counter=fives[sec];
        if(sec==second) counter=10;
        if(sec==minute) counter=15;
        if(sec==hour) counter=60;
    }
    counter--;
}

//PA9  USART1_TX
//PA10 USART1_RX

static int uart_init ( void )
{
    unsigned int ra;

    ra=GET32(RCC_AHB1ENR);
    ra|=1<<0; //enable port a
    PUT32(RCC_AHB1ENR,ra);

    ra=GET32(RCC_APB2ENR);
    ra|=1<<4; //enable USART1
    PUT32(RCC_APB2ENR,ra);

    ra=GET32(GPIOA_MODER);
    ra&=(~(3<<18)); //PA9
    ra|=( (2<<18)); //PA9
    ra&=(~(3<<20)); //PA10
    ra|=( (2<<20)); //PA10
    PUT32(GPIOA_MODER,ra);

    ra=GET32(GPIOA_OTYPER);
    ra&=~(1<<9); //PA9
    ra&=~(1<<10); //PA10
    PUT32(GPIOA_OTYPER,ra);
    
    ////ospeedr 11
    //ra=GET32(GPIOABASE+0x08);
    //ra|=3<<18; //PA9
    //ra|=3<<20; //PA10
    //PUT32(GPIOABASE+0x08,ra);
    ////pupdr 00
    //ra=GET32(GPIOABASE+0x0C);
    //ra&=~(3<<18); //PA9
    //ra&=~(3<<20); //PA10
    //PUT32(GPIOABASE+0x0C,ra);

    //AF7 0111
    ra=GET32(GPIOA_AFRH);
    ra&=(~(0xF<<4)); //PA9
    ra|=( (0x7<<4)); //PA9
    ra&=(~(0xF<<8)); //PA10
    ra|=( (0x7<<8)); //PA10
    PUT32(GPIOA_AFRH,ra);

    //reset USART1
    ra=GET32(RCC_APB2RSTR);
    ra|=( (1<<4));
    PUT32(RCC_APB2RSTR,ra);
    ra&=(~(1<<4));
    PUT32(RCC_APB2RSTR,ra);

    //48000000/(16*4800) = 625.0   0*16 = 0  
    //PUT32(USART1_BRR,(625<<4)|(0)); //48MHz 4800
    //84000000/(16*4800) = 1093.75   0.75*16 = 12  
    PUT32(USART1_BRR,(1093<<4)|(12)); //84MHz 4800

    PUT32(USART1_CR1,(1<<3)|(1<<2)|(1<<13)); //only need rx.

    return(0);
}

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

    //PLLM    4
    //PLLN    168
    //PLLQ    7
    //PLLP    2
    //PPRE2   2
    //PPRE1   4

    ra=GET32(RCC_PLLCFGR);
    ra&=(~(0xF<<24)); //PLLQ
    ra|=( (  7<<24)); //PLLQ

    ra&=(~(0x1<<22)); //PLLSRC
    ra|=( (  1<<22)); //PLLSRC

    ra&=(~(0x3<<16)); //PLLP
    ra|=( (0x0<<16)); //PLLP //0b00 -> 2

    ra&=(~(0x1FF<<6)); //PLLN
    ra|=( (  168<<6)); //PLLN

    ra&=(~(0x3F<<0)); //PLLM
    ra|=( (   4<<0)); //PLLM
    PUT32(RCC_PLLCFGR,ra);


    ra=GET32(RCC_CFGR);
    ra&=(~(0x1F<<16)); //RTCPRE
    ra|=( (   8<<16)); //RTCPRE

    ra&=(~(0x7<<13)); //PPRE2
    ra|=( (0x4<<13)); //PPRE2 0b100 -> 2

    ra&=(~(0x7<<10)); //PPRE1
    ra|=( (0x5<<10)); //PPRE1 0b101 -> 4

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
    ra|=0x00000005; //aggresive setting
    //ra|=0x00000007; //more conservative setting
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



static unsigned int uart_recv ( void )
{
    while(1) if((GET32(USART1_SR))&(1<<5)) break;
    return(GET32(USART1_DR));
}

const unsigned char mod3[16]={0,0,0,1,1,1,2,2,2,3,3,3,4,4,4,0};

//$GPRMC,000143.00,A,4030.97866,N,07955.13947,W,0.419,,020416,,,A*6E

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------
static unsigned int timezone;
//------------------------------------------------------------------------
static int do_nmea ( void )
{
    unsigned int ra;
    unsigned int rb;
    unsigned int rc;
    unsigned int state;
    unsigned int off;
    unsigned int sum;
    unsigned int savesum;
    unsigned int xsum;

    state=0;
    off=0;
//$GPRMC,054033.00,V,
    sum=0;
    savesum=0;
    xsum=0;
    while(1)
    {
        ra=uart_recv();
        //uart_send(ra);
        //uart_send(0x30+state);
        //PUT32(USART1BASE+0x28,ra);

        if(ra!='*') sum^=ra;

        switch(state)
        {
            case 0:
            {
                if(ra=='$') state++;
                else state=0;
                sum=0;
                break;
            }
            case 1:
            {
                if(ra=='G') state++;
                else state=0;
                break;
            }
            case 2:
            {
                if(ra=='P') state++;
                else state=0;
                break;
            }
            case 3:
            {
                if(ra=='G') state++;
                else state=0;
                break;
            }
            case 4:
            {
                if(ra=='G') state++;
                else state=0;
                break;
            }
            case 5:
            {
                if(ra=='A') state++;
                else state=0;
                break;
            }
            case 6:
            {
                off=0;
                if(ra==',') state++;
                else state=0;
                break;
            }
            case 7:
            {
                if(ra==',')
                {
                    if(off>7)
                    {
                        //xstring[0] //tens of hours
                        //xstring[1] //ones of hours
                        //xstring[2] //tens of minutes
                        //xstring[3] //ones of minutes
                        //xstring[4] //tens of seconds
                        //xstring[5] //ones of seconds

                        rb=xstring[0]&0xF;
                        rc=xstring[1]&0xF;
                        //1010
                        rb=/*rb*10*/(rb<<3)+(rb<<1); //times 10
                        rb+=rc;
                        if(rb>12) rb-=12;
                        //ra=5; //time zone adjustment winter
                        //ra=4; //time zone adjustment summer
                        ra=timezone;
                        if(rb<=ra) rb+=12;
                        rb-=ra;
                        if(rb>9)
                        {
                            xstring[0]='1';
                            rb-=10;
                        }
                        else
                        {
                            xstring[0]='0';
                        }
                        rb&=0xF;
                        xstring[1]=0x30+rb;
                        rb=0;
                        //hour  =((xstring[0]&0xF)*10)+(xstring[1]&0xF);
                        //minute=((xstring[2]&0xF)*10)+(xstring[3]&0xF);
                        //second=((xstring[4]&0xF)*10)+(xstring[5]&0xF);
                        //hexstrings(hour);
                        //hexstrings(minute);
                        //hexstring(second);
                    }
                    off=0;
                    state++;
                }
                else
                {
                    if(off<16)
                    {
                        xstring[off++]=ra;
                    }
                }
                break;
            }
            //could check for A vs V here...
            case 8:
            {
                if(ra=='*')
                {
                    savesum=sum;
                    state++;
                }
                break;
            }
            case 9:
            {
                if(ra>0x39) ra-=0x37;
                xsum=(ra&0xF)<<4;
                state++;
                break;
            }
            case 10:
            {
                if(ra>0x39) ra-=0x37;
                xsum|=(ra&0xF);
                if(savesum==xsum)
                {
                    hour  =((xstring[0]&0xF)*10)+(xstring[1]&0xF);
                    //if(hour>12) hour=12;
                    minute=((xstring[2]&0xF)*10)+(xstring[3]&0xF);
                    //if(minute>59) minute=59;
                    second=((xstring[4]&0xF)*10)+(xstring[5]&0xF);
                    if(second>59) second=59;
                    hour=/*hour*5*/ (hour<<2)+(hour<<0);
                    //divide by 12
                          //if(minute<12) z=0;
                    //else  if(minute<24) z=1;
                    //else  if(minute<36) z=2;
                    //else  if(minute<48) z=3;
                    //else  /*if(minute<60)*/ z=4;
                    if(minute>59) minute=59;
                    hour+=mod3[minute>>2];
                    if(hour>59) hour=0;
                }
                for(rb=0;rb<6;rb++) xstring[rb]=0;
                state=0;
                break;
            }
        }
    }
    return(0);
}

int notmain ( void )
{
    unsigned int ra;
    unsigned int rb;

    timezone=4;
    rb=0x44444444;
    ra=GET32(0x20000D00);
    if(ra==0x44444444)
    {
        timezone=5;
        rb=0x55555555;
    }
    else
    if(ra==0x55555555)
    {
        timezone=4;
        rb=0x44444444;
    }
    PUT32(0x20000D00,rb);

    ra=GET32(RCC_AHB1ENR);
    ra|=1<<4; //enable port e
    PUT32(RCC_AHB1ENR,ra);


    //moder
    PUT32(GPIOEBASE+0x00,0x00000000);
    //OTYPER
    PUT32(GPIOEBASE+0x04,0x00000000);
    //BSRR
    PUT32(GPIOEBASE+0x18,0x00FF0000);

    clock_init();
    uart_init();

    PUT32(STK_CSR,4);
    PUT32(STK_RVR,10000-1);
    PUT32(STK_CVR,0x00000000);

    counter = 0;
    hour = 0;
    minute = 15;
    second = 27;
    sec=0;
    for(ra=0;ra<6;ra++) xstring[ra]=0;

    PUT32(STK_CSR,7);

    for(ra=0;ra<3;ra++)
    {
        for(second=0;second<60;second++)
        {
            for(rb=0;rb<10000;rb++) dummy(rb);
        }
    }

    do_nmea();

    return(0);
}


/*

dfu-util -a 0 -s 0x08000000 -D clock03.bin



halt
flash write_image erase clock61/clock02/clock02.elf
reset

*/
