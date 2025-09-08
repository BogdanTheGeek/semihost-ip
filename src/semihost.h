//------------------------------------------------------------------------------
//       Filename: semihost.h
//------------------------------------------------------------------------------
//       Bogdan Ionescu (c) 2025
//------------------------------------------------------------------------------
//       Purpose : Defines the ARM Semihosting API
//------------------------------------------------------------------------------
//       Notes : None
//------------------------------------------------------------------------------
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// Module includes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Module exported defines
//------------------------------------------------------------------------------
#define SEMIHOST_STDIN  0
#define SEMIHOST_STDOUT 1
#define SEMIHOST_STDERR 2

//------------------------------------------------------------------------------
// Module exported type definitions
//------------------------------------------------------------------------------
typedef enum
{
    SYS_OPEN = 0x01,
    SYS_CLOSE = 0x02,
    SYS_WRITEC = 0x03,
    SYS_WRITE0 = 0x04,
    SYS_WRITE = 0x05,
    SYS_READ = 0x06,
    SYS_READC = 0x07,
    SYS_ISERROR = 0x08,
    SYS_ISTTY = 0x09,
    SYS_SEEK = 0x0A,
    SYS_FLEN = 0x0C,
    SYS_CLOCK = 0x10,
    SYS_TIME = 0x11,
    SYS_GET_CMDLINE = 0x15,
    SYS_HEAPINFO = 0x16,
    SYS_ENTER_SVC = 0x17,
    SYS_EXCEPTION = 0x18,
    SYS_ELLAPSED = 0x30,
    SYS_TICKFREQ = 0x31,
} SEMIHOST_Reason_e;

//------------------------------------------------------------------------------
// Module exported variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Module exported functions
//------------------------------------------------------------------------------
/**
 * @brief  Perform a semihosting system call.
 * @param  reason - the semihosting operation code
 * @param  arg - pointer to the argument array (if needed)
 * @return The result of the semihosting operation, typically stored in R0.
 */
static inline int __attribute__((always_inline)) SEMIHOST_SysCall(SEMIHOST_Reason_e reason, void *arg)
{
    int value;
    __asm volatile(
        "mov r0, %[rsn] \n" // place semihost operation code into R0
        "mov r1, %[arg] \n" // R1 points to the argument array
        "bkpt 0xAB      \n" // call debugger
        "mov %[val], r0 \n" // debugger has stored result code in R0

        : [val] "=r"(value)                 // outputs
        : [rsn] "r"(reason), [arg] "r"(arg) // inputs
        : "r0", "r1", "r2", "memory");
    return value; // return result code, stored in R0
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif
