OUTPUT_FORMAT("elf32-h8300")
OUTPUT_ARCH(h8300h)
ENTRY(_start)

MEMORY {
	vectors  (rx!w): ORIGIN = 0x0000, LENGTH = 0x0050
	flashrom (rx!w): ORIGIN = 0x0050, LENGTH = 48K - 0x0050

	io1      (rw  ): ORIGIN = 0xf000, LENGTH = 0x0780
	ram      (rwx ): ORIGIN = 0xf780, LENGTH =  2K
	io2      (rw  ): ORIGIN = 0xff80, LENGTH = 0x0080
}

PHDRS {
	main PT_LOAD FLAGS(7);
}

ORIGIN_SPLOIT = /*0xf956*/ 0xf8f0 + 0x180;

SECTIONS {
	. = ORIGIN_SPLOIT;

	.text . : {
		KEEP(*(.text.startup* .text.entry* .text.main* .text._start*))
		/*KEEP(*(SORT_NONE(.init)))*/ /* crt stuff */
		*(.text .text.* .stub .gnu.linkonce.t*)
		/*KEEP(*(.text.personality*))*/ /* exn handling stuff - not impld */
		/**(.gnu.warning)*/
	} >ram :main

	/*.fini . : {
		KEEP(*(.fini))
	} >ram AT>flashrom*/ /* crt stuff */

	.rodata . : {
		*(.rodata .rodata.* .gnu.linkonce.r*)
		/*SORT(CONSTRUCTORS)*/ /* crt stuff */
	} >ram :main

	/* TODO: preinit_array, init_array, fini_array, ctors, dtors */
	/* TODO: eh_frame, gcc_except_table */

	.data . : {
		__data_start = ABSOLUTE(.);
		*(.data .data.* .gnu.linkonce.t*)
		__data_end = ABSOLUTE(.);
	} >ram :main
	.bss . : {
		__bss_start = ABSOLUTE(.);
		*(.bss .bss.* .sbss .sbss.* COMMON .gnu.linkonce.b*)
		__bss_end = ABSOLUTE(.);
		__heap_start = ABSOLUTE(.);
	} >ram :main

	/DISCARD/ : {
		/* exn stuff */
		*(.eh_frame*)
		*(.gcc_except_table*)
		/* debugging info - no thanks. */
		*(.stab*)
		*(.comment*)
		*(.debug*)
		*(.line*)
	}
}

