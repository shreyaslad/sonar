ENTRY(_start)

OUTPUT_FORMAT(elf64-x86-64)

KNL_HIGH_VMA = 0xFFFFFFFF80000000;

SECTIONS
{
    . = KNL_HIGH_VMA + 0x100000;

    __kernel_start = ALIGN(4K);

    .stivalehdr : ALIGN(4K) {
        KEEP(*(.stivalehdr))
    }

    __kernel_code_start = ALIGN(4K);
    .text : ALIGN(4K) {
        *(.text*)
    }
    __kernel_code_end = ALIGN(4K);

    .rodata : ALIGN(4K) {
        *(.rodata*)
    }

    .data : ALIGN(4K) {
        *(.data*)
    }

    .bss : ALIGN(4K) {
        *(.bss*)
        *(COMMON)
    }
    __kernel_end = ALIGN(4K);
}
