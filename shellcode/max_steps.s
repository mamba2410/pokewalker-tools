; vim: ft=gas
ram_today_steps = 0xf784
step_count_today_so_far = 0xf79c
addWatts16 = 0x1f40
accountForStepTakenLikely = 0x24ac
clampTotalStepToLimit = 0xa32e

wdt_maybe_off = 0x245e
wdt_maybe_on = 0x246c

common_prologue = 0xba42
common_epilogue = 0xba62
wdt_maybe_pet = 0x259e
irCommsEventLoop = 0x08d6
setEventLoopFunc = 0x693a

.global _start
_start:
    jsr     common_prologue
    jsr     wdt_maybe_pet

    mov.l   #99999, er5
    mov.l   er5, @step_count_today_so_far

    mov.w   #irCommsEventLoop, r0
    jsr     setEventLoopFunc
    jmp     common_epilogue     ; jmp NOT jsr

