TRIPLET=h8300-hitachi-elf
AS=$(TRIPLET)-as
OC=$(TRIPLET)-objcopy
LD=$(TRIPLET)-ld
AFLAGS=-mach=h8300h
LDFLAGS=-T shellcode.ld

INFILE=max_steps.s

%.o: %.s
	$(AS) $(AFLAGS) -o $@ $<

%.elf: %.o
	$(LD) $(LDFLAGS) -o $@ $<

%.bin: %.elf
	$(OC) -O binary $< $@

%.hex: %.elf
	$(OC) -O ihex $< $@

clean:
	rm *.hex *.bin *.elf
