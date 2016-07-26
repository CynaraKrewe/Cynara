/*
The MIT License (MIT)

Copyright (c) 2016 Cynara Krewe

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software, hardware and associated documentation files (the "Solution"), to deal
in the Solution without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Solution, and to permit persons to whom the Solution is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Solution.

THE SOLUTION IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOLUTION OR THE USE OR OTHER DEALINGS IN THE
SOLUTION.
 */

MEMORY 
{
    flash (rx) : ORIGIN = 0x00000000, LENGTH = 0x00100000 
    ram  (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00040000 
}

STACK_SIZE = DEFINED(STACK_SIZE) ? STACK_SIZE : 0x800;

SECTIONS 
{
    .text :
    {
        . = ALIGN(4);
        _text = .;
        KEEP(*(.isr_vector))
        *(.text .text* .gnu.linkonce.t.*)
        *(.glue_7t) *(.glue_7)
        *(.rodata .rodata* .gnu.linkonce.r.*)
        *(.ARM.extab* .gnu.linkonce.armextab.*)

        . = ALIGN(4);
        KEEP(*(.init))
        . = ALIGN(4);
        __preinit_array_start = .;
        KEEP (*(SORT(.preinit_array*)))
        KEEP (*(.preinit_array))
        __preinit_array_end = .;

        . = ALIGN(4);
        __init_array_start = .;
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array))
        __init_array_end = .;

        . = ALIGN(4);
        KEEP(*(.fini))

        . = ALIGN(4);
        __fini_array_start = .;
        KEEP (*(.fini_array))
        KEEP (*(SORT(.fini_array.*)))
        __fini_array_end = .;

        . = ALIGN(4);
        _etext = .;
    } > flash

    .stack (NOLOAD):
    {
        . = ALIGN(8);
        _stack = .;
        . = . + STACK_SIZE;
        . = ALIGN(8);
        _estack = .;
    } > ram

    .data : AT(ADDR(.text) + SIZEOF(.text))
    {
        . = ALIGN(4);
        _data = .;
        *(vtable)
        *(.data .data* .gnu.linkonce.d.*)
        _edata = .;
    } > ram

    .bss (NOLOAD):
    {
        . = ALIGN(4);
        _bss = .; 
        *(.bss .bss*) 
        *(COMMON)
        . = ALIGN(4); 
        _ebss = .; 
        . = ALIGN(8);
    } > ram

    PROVIDE_HIDDEN (__exidx_start = .);
    .ARM.exidx :
    {
      *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > ram
    PROVIDE_HIDDEN (__exidx_end = .);

    . = ALIGN(4);
    _heap = .;
    _eheap = ORIGIN(ram) + LENGTH(ram);
}

PROVIDE(heap = _heap);
PROVIDE(eheap = _eheap);
PROVIDE (estack = _estack);
