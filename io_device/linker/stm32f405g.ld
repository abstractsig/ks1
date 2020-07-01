/*
 *
 * linker for hub75
 *
 */
ENTRY(stm32f4_core_reset)

/* stack at top of 'code' RAM */
ld_top_of_c_stack	= 0x10010000;

MEMORY {
    BOOT_ROM (rx)			: ORIGIN = 0x8000000,				LENGTH = 16K
    CONFIG_FLASH	(rx)	: ORIGIN = (0x8000000 + 16K),		LENGTH = 16K
    IO_CODE	(rx)			: ORIGIN = (0x8000000 + 32K),		LENGTH = (448K)
    QJS_CODE	(rx)		: ORIGIN = (0x8000000 + 32K + 448K),	LENGTH = (1024K - 448K - 32K)
    RAM	(rwx)				: ORIGIN = 0x10000000,				LENGTH = 64K
    CCM	(rw)				: ORIGIN = 0x20000000,				LENGTH = 128K
}

SECTIONS {
	.isr_vector : {
		. = ALIGN(4);
		KEEP(*(.isr_vector))
		KEEP(*(.boot_sector))
		. = ALIGN(4);
	} >BOOT_ROM

	.config : {
		FILL(0xff)
		KEEP(*(.io_config));
	} >CONFIG_FLASH

	.js_text : {
		js.o (.text .text*) 
		. = ALIGN(4);
	} > QJS_CODE

	.io_text : {
		. = ALIGN(4);
		*(.text)
		*(.text*)
		*(.glue_7)         /* glue arm to thumb code */
		*(.glue_7t)        /* glue thumb to arm code */
		*(.eh_frame)

		KEEP (*(.init))
		KEEP (*(.fini))

		. = ALIGN(4);
		ld_end_of_code = .;
	} >IO_CODE
	
	.rodata : {
		. = ALIGN(4);
		*(.rodata)
		*(.rodata*)
		. = ALIGN(4);
	} >IO_CODE

	.ARM.extab : { 
		. = ALIGN(4);
		*(.ARM.extab* .gnu.linkonce.armextab.*)
		. = ALIGN(4);
	} >IO_CODE

	.ARM : {
		. = ALIGN(4);
		__exidx_start = .;
		*(.ARM.exidx*)
		__exidx_end = .;
		. = ALIGN(4);
	} >IO_CODE

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
	} >RAM AT> IO_CODE

	. = ALIGN(4);
	.bss : {
		ld_start_of_bss = .; 
		*(.bss)
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
		ld_end_of_bss = .;
		__bss_end__ = ld_end_of_bss;
		ld_end_of_static_ram_allocations = .;
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
	} >CCM

	.ARM.attributes 0 : { *(.ARM.attributes) }
}

