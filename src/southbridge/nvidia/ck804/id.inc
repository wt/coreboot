	.section ".id", "a", @progbits

	.globl __id_start
__id_start:
vendor:
	.asciz CONFIG_MAINBOARD_VENDOR
part:
	.asciz CONFIG_MAINBOARD_PART_NUMBER
.long __id_end + 0x80 - vendor       /* Reverse offset to the vendor ID */
.long __id_end + 0x80 - part         /* Reverse offset to the part number */
.long CONFIG_ROM_SIZE                /* Size of this ROM image */
	.globl __id_end

__id_end:
.previous