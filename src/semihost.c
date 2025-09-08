//------------------------------------------------------------------------------
//       Filename: semihost.c
//------------------------------------------------------------------------------
//       Bogdan Ionescu (c) 2025
//------------------------------------------------------------------------------
//       Purpose : Implements the ARM semihosting API
//------------------------------------------------------------------------------
//       Notes : None
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Module includes
//------------------------------------------------------------------------------
#include "semihost.h"

//------------------------------------------------------------------------------
// Module constant defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// External variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// External functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Module type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Module static variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Module static function prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Module externally exported functions
//------------------------------------------------------------------------------

/**
 * @brief  Handle Hard Fault exceptions. (with support for semihosting)
 * @param  None
 * @return None
 */
void HardFault_Handler(void)
{
    __asm volatile(
        ".syntax unified\n"  // needed for the 'adds r1,#2' below
        " movs r0,#4\n"      // load bit mask into R0
        " mov r1, lr\n"      // load link register into R1
        " tst r0, r1\n"      // compare with bitmask
        " beq _MSP\n"        // if bitmask is set: stack pointer is in PSP. Otherwise in MSP
        " mrs r0, psp\n"     // otherwise: stack pointer is in PSP
        " b _GetPC\n"        // go to part which loads the PC
        "_MSP:\n"            // stack pointer is in MSP register
        " mrs r0, msp\n"     // load stack pointer into R0
        "_GetPC:\n"          // find out where the hard fault happened
        " ldr r1,[r0,#24]\n" // load program counter into R1. R1 contains address of the next instruction where the hard fault happened

        // The following code checks if the hard fault is caused by a semihosting BKPT instruction which is "BKPT 0xAB" (opcode: 0xBE
        // The idea is taken from the MCUXpresso IDE/SDK code, so credits and kudos to the MCUXpresso IDE team!
        " ldrh r2,[r1]\n"       // load opcode causing the fault
        " ldr r3,=0xBEAB\n"     // load constant 0xBEAB (BKPT 0xAB) into R3"
        " cmp r2,r3\n"          // is it the BKPT 0xAB?
        " beq _HandleSmihost\n" // if yes, return from semihosting
        " b _LoopForever\n"     // if no, dump the register values and halt the system
        "_HandleSmihost:\n"     // returning from semihosting fault
        " adds r1,#2\n"         // r1 points to the semihosting BKPT instruction. Adjust the PC to skip it (2 bytes)
        " str r1,[r0,#24]\n"    // store back the adjusted PC value to the interrupt stack frame
        " movs r1,#32\n"        // need to pass back a return value to emulate a successful semihosting operation. 32 is an arbitrary value
        " str r1,[r0,#0]\n"     // store the return value on the stack frame
        " bx lr\n"              // return from the exception handler back to the application
        "_LoopForever:\n");
    while (1)
    {
        // Infinite loop to halt the system
    }
}

//------------------------------------------------------------------------------
// Module static functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
