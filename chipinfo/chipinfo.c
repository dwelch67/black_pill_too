
void PUT32 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );
void dummy ( unsigned int );

unsigned int fsingle ( unsigned int, unsigned int );
unsigned int fdouble ( unsigned int, unsigned int );

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

//PA9  USART1_TX
//PA10 USART1_RX




//0xE000ED88 CPACR

//0xE000ED00 CPUID
//0xE000ED40 through ed84


//Address Name Type Reset Description
//0xE000EF34 FPCCR RW - a Floating Point Context Control Register, FPCCR on page B3-671
//0xE000EF38 FPCAR RW UNKNOWN Floating Point Context Address Register, FPCAR on page B3-673
//0xE000EF3C FPDSCR RW 0x00000000 Floating Point Default Status Control Register, FPDSCR on page B3-674
//0xE000EF40 MVFR0 RO 0x10110021 or Media and FP Feature Register 0, MVFR0 on page B4-720
//0x10110221 b
//0xE000EF44
//MVFR1
//RO
//0x11000011 or
//Media and FP Feature Register 1, MVFR1 on page B4-721
//0x12000011 c
//0xE000EF48
//MVFR2
//RO
//0x00000040 or
//Media and FP Feature Register 2, MVFR2 on page B4-722
//0x00000000 d
//a. See Floating Point Context Control Register, FPCCR on page B3-671.
//b. Defined as 0x10110221 if double-precision is implemented, otherwise 0x10110021 .
//c. Defined as 0x12000011 if double-precision is implemented, otherwise 0x11000011 .
//d. Defin



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
    PUT32(USART1_BRR,0x045); //8MHz 115200
    //48000000/(16*115200) = 26.04  26 1/16
    //PUT32(USART1_BRR,0x1A1); //48Mhz 115200
    PUT32(USART1_CR1,(1<<3)|(1<<2)|(1<<13));

    return(0);
}

static void uart_send ( unsigned int x )
{
    while(1) if(GET32(USART1_SR)&(1<<7)) break;
    PUT32(USART1_DR,x);
}

//static
//unsigned int uart_recv ( void )
//{
    //while(1) if((GET32(USART1_SR))&(1<<5)) break;
    //return(GET32(USART1_DR));
//}

static void hexstrings ( unsigned int d )
{
    //unsigned int ra;
    unsigned int rb;
    unsigned int rc;

    rb=32;
    while(1)
    {
        rb-=4;
        rc=(d>>rb)&0xF;
        if(rc>9) rc+=0x37; else rc+=0x30;
        uart_send(rc);
        if(rb==0) break;
    }
    uart_send(0x20);
}

//static
void hexstring ( unsigned int d )
{
    hexstrings(d);
    uart_send(0x0D);
    uart_send(0x0A);
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



void hardfault ( void )
{
    hexstring(0xBADBAD);
    while(1) continue;
}

int notmain ( void )
{
    unsigned int rx;

    clock_init();
    uart_init();

    hexstring(0x12345678);

    hexstrings(0xE000ED00);
    hexstring(GET32(0xE000ED00));
    for(rx=0xE000ED40;rx<=0xE000ED84;rx+=4)
    {
        hexstrings(rx);
        hexstring(GET32(rx));
    }
    for(rx=0xE000EF34;rx<=0xE000EF48;rx+=4)
    {
        hexstrings(rx);
        hexstring(GET32(rx));
    }

    hexstrings(0xE000ED88);
    hexstrings(GET32(0xE000ED88));
    PUT32(0xE000ED88,0xFFFFFFFF);
    hexstring(GET32(0xE000ED88));

if(1)
{
    hexstrings(GET32(0xE000ED88));
    PUT32(0xE000ED88,0x00300000);
    hexstring(GET32(0xE000ED88));
}

    
    hexstring(fsingle(3,5));
    hexstring(fdouble(3,5));


    hexstring(0x12345678);
    return(0);
}
/*
dfu-util -a 0 -s 0x08000000 -D chipinfo.bin
*/

