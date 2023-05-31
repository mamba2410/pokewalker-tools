; vim: ft=gas

common_prologue = 0xba42
common_epilogue = 0xba62
irCommsEventLoop = 0x08d6
sendPacket = 0x0772
wdt_maybe_pet = 0x259e
setEventLoopFunc = 0x693a

.global _start
.byte 0x56
_start:
    jsr     common_prologue
    sub.w   r5, r5
lbl_big_loop:
    mov.w   #0xf8d6, r6
    mov.b   #0x80, r4l
    eepmov.b
    mov.w   #0xaa80, r0
    jsr     sendPacket
    jsr     wdt_maybe_pet
    cmp.w   #0xc000, r5
    bne     lbl_big_loop
    mov.w   #irCommsEventLoop, r0
    jsr     setEventLoopFunc
    jmp     common_epilogue

