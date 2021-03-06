//*****************************************************************************
//
// startup_gcc.c - Startup code for use with GNU tools.
//
// Copyright (c) 2009-2012 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:
// 
//   Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the  
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

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

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "driverlib/debug.h"
#include "inc/hw_types.h"
#include "inc/hw_nvic.h"

// Forward declaration of the default fault handlers.
void ResetISR(void);
static void NmiSR(void);
static void FaultISR(void);
static void IntDefaultHandler(void);

// The entry point for the application.
extern int main(void);

extern void SysTickIntHandler(void);

// System stack start determined by the linker script.
extern unsigned estack __asm("estack");

// The vector table. Note: the proper constructs must be placed on this to
// ensure that it ends up at physical address 0x0000.0000.
__attribute__ ((section(".isr_vector")))
void (* const g_pfnVectors[])(void) =
{
    (void *)&estack,   // The initial stack pointer
    ResetISR,          // The reset handler
    NmiSR,             // The NMI handler
    FaultISR,          // The hard fault handler
    IntDefaultHandler, // The MPU fault handler
    IntDefaultHandler, // The bus fault handler
    IntDefaultHandler, // The usage fault handler
    0,                 // Reserved
    0,                 // Reserved
    0,                 // Reserved
    0,                 // Reserved
    IntDefaultHandler, // SVCall handler
    IntDefaultHandler, // Debug monitor handler
    0,                 // Reserved
    IntDefaultHandler, // The PendSV handler
    SysTickIntHandler, // The SysTick handler
	IntDefaultHandler, // GPIO Port A
	IntDefaultHandler, // GPIO Port B
	IntDefaultHandler, // GPIO Port C
	IntDefaultHandler, // GPIO Port D
	IntDefaultHandler, // GPIO Port E
	IntDefaultHandler, // UART0 Rx and Tx
	IntDefaultHandler, // UART1 Rx and Tx
    IntDefaultHandler, // SSI0 Rx and Tx
    IntDefaultHandler, // I2C0 Master and Slave
    IntDefaultHandler, // PWM Fault
    IntDefaultHandler, // PWM Generator 0
    IntDefaultHandler, // PWM Generator 1
    IntDefaultHandler, // PWM Generator 2
    IntDefaultHandler, // Quadrature Encoder 0
    IntDefaultHandler, // ADC Sequence 0
    IntDefaultHandler, // ADC Sequence 1
    IntDefaultHandler, // ADC Sequence 2
    IntDefaultHandler, // ADC Sequence 3
    IntDefaultHandler, // Watchdog timer
    IntDefaultHandler, // Timer 0 subtimer A
    IntDefaultHandler, // Timer 0 subtimer B
    IntDefaultHandler, // Timer 1 subtimer A
    IntDefaultHandler, // Timer 1 subtimer B
    IntDefaultHandler, // Timer 2 subtimer A
    IntDefaultHandler, // Timer 2 subtimer B
    IntDefaultHandler, // Analog Comparator 0
    IntDefaultHandler, // Analog Comparator 1
    IntDefaultHandler, // Analog Comparator 2
    IntDefaultHandler, // System Control (PLL, OSC, BO)
    IntDefaultHandler, // FLASH Control
	IntDefaultHandler, // GPIO Port F
	IntDefaultHandler, // GPIO Port G
	IntDefaultHandler, // GPIO Port H
	IntDefaultHandler, // UART2 Rx and Tx
    IntDefaultHandler, // SSI1 Rx and Tx
    IntDefaultHandler, // Timer 3 subtimer A
    IntDefaultHandler, // Timer 3 subtimer B
    IntDefaultHandler, // I2C1 Master and Slave
    IntDefaultHandler, // CAN0
    IntDefaultHandler, // CAN1
	IntDefaultHandler, // Ethernet
    IntDefaultHandler, // Hibernate
	IntDefaultHandler, // USB0
    IntDefaultHandler, // PWM Generator 3
    IntDefaultHandler, // uDMA Software Transfer
    IntDefaultHandler, // uDMA Error
    IntDefaultHandler, // ADC1 Sequence 0
    IntDefaultHandler, // ADC1 Sequence 1
    IntDefaultHandler, // ADC1 Sequence 2
    IntDefaultHandler, // ADC1 Sequence 3
    IntDefaultHandler, // External Bus Interface 0
	IntDefaultHandler, // GPIO Port J
	IntDefaultHandler, // GPIO Port K
	IntDefaultHandler, // GPIO Port L
    IntDefaultHandler, // SSI2 Rx and Tx
    IntDefaultHandler, // SSI3 Rx and Tx
	IntDefaultHandler, // UART3 Rx and Tx
	IntDefaultHandler, // UART4 Rx and Tx
	IntDefaultHandler, // UART5 Rx and Tx
	IntDefaultHandler, // UART6 Rx and Tx
	IntDefaultHandler, // UART7 Rx and Tx
    IntDefaultHandler, // I2C2 Master and Slave
    IntDefaultHandler, // I2C3 Master and Slave
	IntDefaultHandler, // Timer 4 subtimer A
    IntDefaultHandler, // Timer 4 subtimer B
	IntDefaultHandler, // Timer 5 subtimer A
    IntDefaultHandler, // Timer 5 subtimer B
    IntDefaultHandler, // FPU
    0,                 // Reserved
    0,                 // Reserved
    IntDefaultHandler, // I2C4 Master and Slave
    IntDefaultHandler, // I2C5 Master and Slave
	IntDefaultHandler, // GPIO Port M
	IntDefaultHandler, // GPIO Port N
    0,                 // Reserved
    IntDefaultHandler, // Tamper
	IntDefaultHandler, // GPIO Port P (Summary or P0)
	IntDefaultHandler, // GPIO Port P1
	IntDefaultHandler, // GPIO Port P2
	IntDefaultHandler, // GPIO Port P3
	IntDefaultHandler, // GPIO Port P4
	IntDefaultHandler, // GPIO Port P5
	IntDefaultHandler, // GPIO Port P6
	IntDefaultHandler, // GPIO Port P7
	IntDefaultHandler, // GPIO Port Q (Summary or Q0)
	IntDefaultHandler, // GPIO Port Q1
	IntDefaultHandler, // GPIO Port Q2
	IntDefaultHandler, // GPIO Port Q3
	IntDefaultHandler, // GPIO Port Q4
	IntDefaultHandler, // GPIO Port Q5
	IntDefaultHandler, // GPIO Port Q6
	IntDefaultHandler, // GPIO Port Q7
	IntDefaultHandler, // GPIO Port R
	IntDefaultHandler, // GPIO Port S
    IntDefaultHandler, // SHA/MD5 0
    IntDefaultHandler, // AES 0
    IntDefaultHandler, // DES3DES 0
    IntDefaultHandler, // LCD Controller 0
    IntDefaultHandler, // Timer 6 subtimer A
    IntDefaultHandler, // Timer 6 subtimer B
    IntDefaultHandler, // Timer 7 subtimer A
    IntDefaultHandler, // Timer 7 subtimer B
    IntDefaultHandler, // I2C6 Master and Slave
    IntDefaultHandler, // I2C7 Master and Slave
    IntDefaultHandler, // HIM Scan Matrix Keyboard 0
    IntDefaultHandler, // One Wire 0
    IntDefaultHandler, // HIM PS/2 0
    IntDefaultHandler, // HIM LED Sequencer 0
    IntDefaultHandler, // HIM Consumer IR 0
    IntDefaultHandler, // I2C8 Master and Slave
    IntDefaultHandler, // I2C9 Master and Slave
	IntDefaultHandler  // GPIO Port T
};

// The following are constructs created by the linker, indicating where the
// the "data" and "bss" segments reside in memory. The initializers for the
// for the "data" segment resides immediately following the "text" segment.
extern unsigned long _etext;
extern unsigned long _data;
extern unsigned long _edata;
extern unsigned long _bss;
extern unsigned long _ebss;
extern void (*__preinit_array_start[])(void);
extern void (*__preinit_array_end[])(void);
extern void (*__init_array_start[])(void);
extern void (*__init_array_end[])(void);
extern void _init(void);

// This is the code that gets called when the processor first starts execution
// following a reset event.  Only the absolutely necessary set is performed,
// after which the application supplied entry() routine is called.  Any fancy
// actions (such as making decisions based on the reset cause register, and
// resetting the bits in that register) are left solely in the hands of the
// application.
void ResetISR(void)
{
    unsigned long *pulSrc, *pulDest;
    unsigned i, cnt;

    // Copy the data segment initializers from flash to SRAM.
    pulSrc = &_etext;
    for (pulDest = &_data; pulDest < &_edata;) {
        *pulDest++ = *pulSrc++;
    }

    // Zero fill the bss segment.
    __asm(  "    ldr     r0, =_bss\n"
            "    ldr     r1, =_ebss\n"
            "    mov     r2, #0\n"
            "    .thumb_func\n"
            "1:\n"
            "    cmp     r0, r1\n"
            "    it      lt\n"
            "    strlt   r2, [r0], #4\n"
            "    blt     1b"
    );
    (void)_bss; (void)_ebss;

    // Enable the floating-point unit before calling c++ ctors

    HWREG(NVIC_CPAC) = ((HWREG(NVIC_CPAC) &
                         ~(NVIC_CPAC_CP10_M | NVIC_CPAC_CP11_M)) |
                         NVIC_CPAC_CP10_FULL | NVIC_CPAC_CP11_FULL);

    // Call any global C++ constructors.
    cnt = __preinit_array_end - __preinit_array_start;
    for (i = 0; i < cnt; i++)
        __preinit_array_start[i]();

    _init();

    cnt = __init_array_end - __init_array_start;
    for (i = 0; i < cnt; i++)
        __init_array_start[i]();

    // Run the actual application.
    main();
}

// This is the code that gets called when the processor receives a NMI.  This
// simply enters an infinite loop, preserving the system state for examination
// by a debugger.
static void NmiSR(void)
{
    while(1);
}

// This is the code that gets called when the processor receives a fault
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
static void FaultISR(void)
{
    while(1);
}

// This is the code that gets called when the processor receives an unexpected
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
static void IntDefaultHandler(void)
{
    while(1);
}

typedef char *caddr_t;

// _sbrk - newlib memory allocation routine.
caddr_t _sbrk(int increment)
{
    extern char heap __asm("heap"); // See linker script.
    __attribute__((unused))
    extern char eheap __asm("eheap"); // See linker script.
    static char* currentHeapTop = &heap;
    char* previousHeapTop = currentHeapTop;

    ASSERT(currentHeapTop + increment < &eheap);

    currentHeapTop += increment;

    return (caddr_t)previousHeapTop;
}

extern int link( char *cOld, char *cNew )
{
    return -1;
}

extern int _close( int file )
{
    return -1;
}

extern int _fstat( int file, struct stat *st )
{
    st->st_mode = S_IFCHR ;

    return 0;
}

extern int _isatty( int file )
{
    return 1;
}

extern int _lseek( int file, int ptr, int dir )
{
    return 0;
}

extern int _read(int file, char *ptr, int len)
{
    return 0;
}

extern int _write( int file, char *ptr, int len )
{
    return len;
}

extern void _kill( int pid, int sig )
{}

extern int _getpid ( void )
{
    return -1;
}

extern void _exit (unsigned int code)
{
	(void)code;
	while(1);
}
