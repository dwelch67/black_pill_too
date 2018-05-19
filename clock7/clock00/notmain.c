
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

#include "seven.h"

static unsigned char xstring[32];
volatile unsigned int num[4];
volatile unsigned int counter;
void systick_handler ( void )
{
    unsigned int x;

    PUT32(GPIOABASE+0x00,0xA8200000);
    x=(counter++)&3;
    PUT32(GPIOEBASE+0x18,seven[num[x]]);
//    PUT32(GPIOEBASE+0x18,seven[xstring[x]&0xF]);
    PUT32(GPIOABASE+0x00,0xA8200000|(1<<(x<<1)));
    //GET32(STK_CSR);
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

    //8000000/(16*115200) = 4.34  4+5/16
    //PUT32(USART1_BRR,0x045); //8MHz 115200

    //8000000/(16*4800) = 104.16666  104.2.55
    PUT32(USART1_BRR,0x682); //8MHz 4800

    //8000000/(16*9600) = 52.08333   52 1/16
    //PUT32(USART1_BRR,0x341); //8MHz 9600

    //48000000/(16*115200) = 26.04  26 1/16
    //PUT32(USART1_BRR,0x1A1); //48Mhz 115200
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
                if(ra=='R') state++;
                else state=0;
                break;
            }
            case 4:
            {
                if(ra=='M') state++;
                else state=0;
                break;
            }
            case 5:
            {
                if(ra=='C') state++;
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
                    num[0]=xstring[0]&0xF;
                    if(num[0]!=1) num[0]=10;
                    num[1]=xstring[1]&0xF;
                    num[2]=xstring[2]&0xF;
                    num[3]=xstring[3]&0xF;
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

    clock_init();
    uart_init();

    PUT32(STK_CSR,4);
    PUT32(STK_RVR,10000-1);
    PUT32(STK_CVR,0x00000000);

    counter = 0;
    num[0]=1;
    num[1]=2;
    num[2]=3;
    num[3]=4;
    for(ra=0;ra<6;ra++) xstring[ra]=0;

    PUT32(STK_CSR,7);
    do_nmea();

    return(0);
}

/*

dfu-util -a 0 -s 0x08000000 -D notmain.bin

*/


