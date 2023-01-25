SECTIONS
{
	. = 0x00000000;
	.vector	: {
		*(.vector);
	}
	.text	ALIGN(4): {
		*(.text .text.*);
	}
	.rodata	ALIGN(4): {
		__rodata_start = .;
		*(.rodata .rodata.*);
		__rodata_end = .;
	}
	.data	ALIGN(4): {
		__data_start = .;
		*(.data .data.*);
		__data_end = .;
	}
	.bss	ALIGN(4): {
		__bss_start = .;
		*(.bss .bss.*);
		__bss_end = .;
	}
	.stack	ALIGN(4): {
		__stack_start = .;
		*(.stack);
		__stack_end = .;
	}
}
