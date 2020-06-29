/*
 *
 * linker for hub75
 *
 */
ENTRY(stm32f4_core_reset)

ld_top_of_c_stack	= 0x10010000;

MEMORY {
    BOOT_ROM (rx)		: ORIGIN = 0x8000000,				LENGTH = 16K
    CONFIG_ROM	(rx)	: ORIGIN = (0x8000000 + 16K),		LENGTH = 16K
    ROM	(rx)			: ORIGIN = (0x8000000 + 32K),		LENGTH = (1024K - 32K)
    RAM	(rwx)			: ORIGIN = 0x10000000,				LENGTH = 64K
    CCM	(rwx)			: ORIGIN = 0x20000000,				LENGTH = 128K
}

SECTIONS {
	.isr_vector : {
		. = ALIGN(4);
		KEEP(*(.isr_vector))
		KEEP(*(.boot_sector))
		. = ALIGN(4);
	} >BOOT_ROM

	.text : {
		. = ALIGN(4);
		*(.text)           /* .text sections (code) */
		*(.text*)          /* .text* sections (code) */
		*(.glue_7)         /* glue arm to thumb code */
		*(.glue_7t)        /* glue thumb to arm code */
		*(.eh_frame)

		KEEP (*(.init))
		KEEP (*(.fini))

		. = ALIGN(4);
		ld_end_of_code = .;
	} >ROM

	.rodata : {
		. = ALIGN(4);
		*(.rodata)
		*(.rodata*)
		. = ALIGN(4);
	} >ROM

	.ARM.extab : { 
		. = ALIGN(4);
		*(.ARM.extab* .gnu.linkonce.armextab.*)
		. = ALIGN(4);
	} >ROM

	.ARM : {
		. = ALIGN(4);
		__exidx_start = .;
		*(.ARM.exidx*)
		__exidx_end = .;
		. = ALIGN(4);
	} >ROM

	.ram_vectors (NOLOAD) : {
		. = ALIGN(256);
		KEEP(*(.ram_vector_table*));
	} >RAM

	ld_start_of_sdata_in_flash = LOADADDR(.data);

	.data : {
		. = ALIGN(4);
		ld_start_of_sdata_in_ram = .;
		*(.data)
		*(.data*)
		. = ALIGN(4);
		ld_end_of_sdata_in_ram = .; 
	} >RAM AT> ROM

	. = ALIGN(4);
	.bss : {
		ld_start_of_bss = .; 
		*(.bss)
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
		ld_end_of_bss = .;
		__bss_end__ = ld_end_of_bss;
	} >RAM

	._for_stdlib : {
		. = ALIGN(8);
		PROVIDE ( end = . );
		PROVIDE ( _end = . );
		. = ALIGN(8);
	} >RAM

	.umm_heap (NOLOAD) : {
		. = ALIGN(4);
		*(.umm)
		. = ALIGN(4);
		ld_end_of_static_ram_allocations = .;
	} >RAM

	.matrix (NOLOAD) : {
		. = ALIGN(4);
		KEEP(*(.matrix_ram));
		. = ALIGN(256);
	} >CCM

	.ARM.attributes 0 : { *(.ARM.attributes) }
}

