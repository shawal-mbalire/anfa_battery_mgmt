/*
 * Linker command file for MSPM0G3507
 * This is a generic example. Specific memory sizes and section placements
 * might need adjustment based on your exact device variant and project.
 */

/* Entry Point */
ENTRY(Reset_Handler)

/* Memory Regions */
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 128K /* MSPM0G3507 typically has 128KB Flash */
    RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 32K  /* MSPM0G3507 typically has 32KB RAM */
}

/* Section Definitions */
SECTIONS
{
    /* Interrupt Vector Table */
    .intvecs :
    {
        KEEP(*(.intvecs)) /* Keep the interrupt vector table at the beginning of Flash */
    } > FLASH

    /* Code Section */
    .text :
    {
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        *(.constdata)
        *(.constdata*)
        . = ALIGN(4);
    } > FLASH

    /* Initialized Data Section */
    .data : AT(LOADADDR(.text) + SIZEOF(.text))
    {
        . = ALIGN(4);
        *(.data)
        *(.data*)
        . = ALIGN(4);
    } > RAM

    /* Uninitialized Data Section */
    .bss :
    {
        . = ALIGN(4);
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
    } > RAM

    /* Heap and Stack */
    .heap (NOLOAD):
    {
        . = ALIGN(8);
        _heap_start = .;
        . += _HEAP_SIZE; /* Define _HEAP_SIZE in your project settings or startup file */
        _heap_end = .;
    } > RAM

    .stack (NOLOAD):
    {
        . = ALIGN(8);
        _stack_end = .;
        . += _STACK_SIZE; /* Define _STACK_SIZE in your project settings or startup file */
        _stack_start = .;
    } > RAM

    /* DWARF sections (for debugging) */
    .debug_arranges  0 : { *(.debug_arranges) }
    .debug_aranges   0 : { *(.debug_aranges) }
    .debug_info      0 : { *(.debug_info) }
    .debug_abbrev    0 : { *(.debug_abbrev) }
    .debug_line      0 : { *(.debug_line) }
    .debug_frame     0 : { *(.debug_frame) }
    .debug_str       0 : { *(.debug_str) }
    .debug_loc       0 : { *(.debug_loc) }
    .debug_macinfo   0 : { *(.debug_macinfo) }
    .debug_pubnames  0 : { *(.debug_pubnames) }
    .debug_pubtypes  0 : { *(.debug_pubtypes) }
    .debug_ranges    0 : { *(.debug_ranges) }
    .debug_static_relocs 0 : { *(.debug_static_relocs) }
    .debug_weaknames 0 : { *(.debug_weaknames) }
    .debug_funcnames 0 : { *(.debug_funcnames) }
    .debug_typenames 0 : { *(.debug_typenames) }
    .debug_varnames  0 : { *(.debug_varnames) }
}
