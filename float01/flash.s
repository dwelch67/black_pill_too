
.cpu cortex-m4
.fpu vfp
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
    bl notmain
    b hang
.thumb_func
hang:   b .


.align

.thumb_func
.globl PUT16
PUT16:
    strh r1,[r0]
    bx lr

.thumb_func
.globl PUT32
PUT32:
    str r1,[r0]
    bx lr

.thumb_func
.globl GET32
GET32:
    ldr r0,[r0]
    bx lr

.thumb_func
.globl dummy
dummy:
    bx lr

.thumb_func
.globl fsingle
fsingle:
    vmov s0,r0          ;@ copy the contents of r0 to s0
    vcvt.f32.u32 s0,s0  ;@ convert s0 from unsigned int to float
    vmov s1,r1          ;@ copy r1 to s1
    vcvt.f32.u32 s1,s1  ;@ convert s1 from unsigned int to float
    vmul.f32 s0,s0,s1   ;@ multiply s0 = s0 * s1
    vcvt.u32.f32 s0,s0  ;@ convert s0 from float to unsigned int
    vmov r0,s0          ;@ copy s0 to r0
    bx lr
.thumb_func
.globl fdouble
fdouble:
    vmov s0,r0          ;@ copy the contents of r0 to s0
    vcvt.f64.u32 d0,s0  ;@ convert s0 from unsigned int to float
    vmov s1,r1          ;@ copy r1 to s1
    vcvt.f64.u32 d1,s1  ;@ convert s1 from unsigned int to float
    vmul.f64 d0,d0,d1   ;@ multiply s0 = s0 * s1
    vcvt.u32.f64 s0,d0  ;@ convert s0 from float to unsigned int
    vmov r0,s0          ;@ copy s0 to r0
    bx lr
.end

