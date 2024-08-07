SECTIONS
{
    PROVIDE (__executable_start = SEGMENT_START("text-segment", 0));
    . = SEGMENT_START("text-segment", 0) + SIZEOF_HEADERS;

    /*. = ALIGN(CONSTANT (MAXPAGESIZE));*/
    .text : {
        *(.text*)
        *(.rodata*)
        *(.data*)
        *(.bss*)
    }
}

