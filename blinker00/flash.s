



.equ GPIOEBASE, 0x40021000
.equ RCCBASE  , 0x40023800
.equ DELAYCOUNTS, 0x40000


.thumb

.thumb_func
.global _start
_start:
stacktop: .word 0x20001000
.word reset
.word hang
.word hang
.word hang
.word hang
.word hang
.word hang
.word hang
.word hang
.word hang
.word hang
.word hang
.word hang
.word hang
.word hang

.thumb_func
reset:
    ldr r0,=RCCBASE
    ldr r1,[r0,#0x30]
    ldr r2,=0x00000010
    orr r1,r2
    str r1,[r0,#0x30]

    ldr r0,=GPIOEBASE

    ldr r1,[r0,#0x00]
    ldr r2,=0x00000003
    bic r1,r2
    ldr r2,=0x00000001
    orr r1,r2
    str r1,[r0,#0x00]

    ldr r1,[r0,#0x04]
    ldr r2,=0x00000001
    bic r1,r2
    str r1,[r0,#0x04]

loop_top:
    ldr r1,=0x00000001
    str r1,[r0,#0x18]
    
    ldr r2,=DELAYCOUNTS
loop0:
    sub r2,#1 ;@ subs
    bne loop0
    
    ldr r1,=0x00010000
    str r1,[r0,#0x18]
    
    ldr r2,=DELAYCOUNTS
loop1:
    sub r2,#1 ;@ subs
    bne loop1

    b loop_top

.thumb_func
hang:   b .

.end

/*
dfu-util -a 0 -s 0x08000000 -D blinker00.bin
*/
