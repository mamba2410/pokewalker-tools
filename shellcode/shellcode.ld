OUTPUT_FORMAT("elf32-h8300")
OUTPUT_ARCH(h8300h);
ENTRY(_start);

SHELLCODE_ORIGIN = 0xf956;

SECTIONS {

    . = SHELLCODE_ORIGIN;

    .text : {
        *(.text.boot* .text.start* .text._start*);
        *(.text.*);
    }
}

