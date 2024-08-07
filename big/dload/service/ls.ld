SECTIONS
{
    PROVIDE (__executable_start = SEGMENT_START("text-segment", 0));
    . = SEGMENT_START("text-segment", 0) + SIZEOF_HEADERS;

    /*. = ALIGN(CONSTANT (MAXPAGESIZE));*/
    .text : {
        *(.text*)
        *(.rodata*)
        *(.data*)
    }
}
/*
SECTIONS
{
  . = 0x0000;
  .text : {
    *(.text*)
    }
  .data : {
    *(.data*)
    *(.rodata*) 
  }
  .bss : { *(.bss*) }
}
*/
