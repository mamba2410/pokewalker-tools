; vim: ft=gas
common_prologue = 0xba42
common_epilogue = 0xba62
wdt_maybe_pet = 0x259e
irCommsEventLoop = 0x08d6
setEventLoopFunc = 0x693a
addWatts16 = 0x1f40

.global _start
_start:
    jsr common_prologue
    jsr wdt_maybe_pet

    mov.w   #9999, r0
    jsr     addWatts16

    mov.w   #irCommsEventLoop, r0
    jsr     setEventLoopFunc
    jmp     common_epilogue     ; jmp NOT jsr

