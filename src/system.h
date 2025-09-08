// ===================================================================================
// Basic System Functions for PY32F002, PY32F003, and PY32F030                * v1.2 *
// ===================================================================================
//
// This file must be included!!! The system configuration and the system clock are 
// set up automatically on system start. Configure system options below.
//
// System clock functions available:
// ---------------------------------
// CLK_init_HSI()           init internal oscillator (non PLL) as system clock source
// CLK_init_HSI_PLL()       init internal oscillator with PLL as system clock source
// CLK_init_HSE()           init external crystal (non PLL) as system clock source
// CLK_init_HSE_PLL()       init external crystal with PLL as system clock source
// CLK_reset()              reset system clock to default state
//
// HSI_enable()             enable internal 8MHz high-speed clock (HSI)
// HSI_disable()            disable HSI
// HSI_ready()              check if HSI is stable
//
// HSE_enable()             enable external high-speed clock (HSE)
// HSE_disable()            disable HSE
// HSE_ready()              check if HSE is stable
// HSE_bypass_on()          enable HSE clock bypass
// HSE_bypass_off()         disable HSE clock bypass
// HSE_CSS_on()             enable HSE clock security system
// HSE_CSS_off()            disable HSE clock security system
//
// LSI_enable()             enable internal 32.768kHz low-speed clock (LSI)
// LSI_disable()            disable LSI
// LSI_ready()              check if LSI is stable
//
// LSE_enable()             enable external 32.768kHz low-speed clock (LSE)
// LSE_disable()            disable LSE
// LSE_ready()              check if LSE is stable
// LSE_bypass_on()          enable LSE clock bypass
// LSE_bypass_off()         disable LSE clock bypass
// LSE_CSS_on()             enable LSE clock security system
// LSE_CSS_off()            disable LSE clock security system
//
// PLL_enable()             enable phase-locked loop (PLL)
// PLL_disable()            disable PLL
// PLL_ready()              check if PLL is stable
// PLL_setHSI()             set HSI as PLL input
// PLL_setHSE()             set HSE as PLL input
//
// MCO_setSYS()             enable output system clock (SYS_CLK) on MCO
// MCO_setHSI()             enable output internal high-speed clock (HSI) on MCO
// MCO_setHSE()             enable output external high-speed clock (HSE) on MCO
// MCO_setPLL()             enable output PLL on MCO
// MCO_setLSI()             enable output internal low-speed clock (LSI) on MCO
// MCO_setLSE()             enable output external low-speed clock (LSE) on MCO
// MCO_setPRE(n)            set MCO prescaler to 2^n (set before enabling MCO)
// MCO_stop()               disable clock output (MCO)
//
// Real-Time Clock (RTC) functions available:
// ------------------------------------------
// RTC_init()               init RTC with LSI as clock source and 1s clock period
// RTC_init_LSE()           init RTC with LSE as clock source and 1s clock period
// RTC_setPrescaler(p)      set RTC prescaler (default: 32768)
// RTC_setCounter(c)        set RTC counter value
// RTC_setAlarm(a)          set RTC alarm value (default: 0xffffffff)
// RTC_getCounter()         get RTC counter value
// RTC_getAlarm()           get RTC alarm value
//
// Delay (DLY) and SysTick (STK) functions available:
// --------------------------------------------------
// DLY_ticks(n)             delay n+1 clock cycles
// DLY_us(n)                delay n microseconds
// DLY_ms(n)                delay n milliseconds
//
// STK_enable()             enable SYSTICK at system frequency
// STK_disable()            disable SYSTICK
//
// Independent Watchdog Timer (IWDG) functions available:
// ------------------------------------------------------
// IWDG_start(n)            start independent watchdog timer, n milliseconds, n<=8191
// IWDG_reload(n)           reload watchdog counter with n milliseconds, n<=8191
// IWDG_feed()              feed the dog (reload last time)
//
// Low-Power Timer (LPT) functions available:
// ------------------------------------------
// LPT_init()               init and enable low-power timer (LPT)
// LPT_shot(n)              start LPT single shot with period in ms
// LPT_sleep(n)             put device in to SLEEP for period in ms
// LPT_stop(n)              put device in to STOP for period in ms
//
// Sleep functions available:
// --------------------------
// SLEEP_WFI_now()          put device into sleep, wake up by interrupt
// SLEEP_WFE_now()          put device into sleep, wake up by event
// STOP_WFI_now()           put device into stop (deep sleep), wake by interrupt
// STOP_WFE_now()           put device into stop (deep sleep), wake by event
// STOP_lowPower()          set reduced power in stop mode
//
// Reset (RST) and Bootloader (BOOT) functions available:
// ------------------------------------------------------
// BOOT_now()               jump to bootloader
// RST_now()                conduct software reset
// RST_clearFlags()         clear all reset flags
// RST_wasWWDG()            check if last reset was caused by window watchdog
// RST_wasIWDG()            check if last reset was caused by independent watchdog
// RST_wasSoftware()        check if last reset was caused by software
// RST_wasPower()           check if last reset was caused by BOR/POR/PDR
// RST_wasPin()             check if last reset was caused by RST pin low
// RST_wasOption()          check if last reset was caused by OPTION byte loader
//
// Cyclic Redundancy Check (CRC) functions available:
// --------------------------------------------------
// CRC_enable()             enable cyclic redundancy check (CRC) module
// CRC_disable()            disable CRC module
// CRC_write(w)             add new 32-bit word for CRC calculation
// CRC_write32(w)           add new 32-bit word for CRC calculation (alias)
// CRC_write16(h)           add new 16-bit half-word for CRC calculation
// CRC_write8(b)            add new  8-bit byte for CRC calculation
// CRC_read()               read last 32-bit CRC calculation
// CRC_read32()             read last 32-bit CRC calculation (alias)
// CRC_read16()             read last 16-bit CRC calculation
// CRC_read8()              read last  8-bit CRC calculation
// CRC_reset()              reset CRC calculation
//
// Interrupt (INT) functions available:
// ------------------------------------
// INT_enable()             global interrupt enable
// INT_disable()            global interrupt disable
// INT_ATOMIC_BLOCK { }     execute block without being interrupted
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "py32f0xx.h"

// ===================================================================================
// System Options (set "1" to activate)
// ===================================================================================
#define SYS_CLK_INIT      1         // 1: init system clock on startup
#define SYS_TICK_INIT     1         // 1: init and start SYSTICK on startup
#define SYS_GPIO_EN       1         // 1: enable GPIO ports on startup
#define SYS_CLEAR_BSS     1         // 1: clear uninitialized variables
#define SYS_USE_VECTORS   1         // 1: create interrupt vector table
#define SYS_USE_HSE       0         // 1: use external crystal for system clock

// ===================================================================================
// Sytem Clock Defines
// ===================================================================================
// Set system clock frequency
#ifndef F_CPU
  #define F_CPU           8000000   // 8 MHz if not otherwise defined
#endif

#if defined(py32f002bx5)\
   || defined(py32f002ax5)\
   || defined(py32f002ax6)\
   || defined(py32f002bx5)
#define PY32F002
#elif defined(py32f003x4)\
   || defined(py32f003x6)\
   || defined(py32f003x7)\
   || defined(py32f003x8)
#define PY32F003
#elif defined(py32f030x4)\
   || defined(py32f030x6)\
   || defined(py32f030x8)\
   || defined(py32f031x6)
#define PY32F030
#else
#error Unsupported PY32F0xx device, please define PY32F002, PY32F003, or PY32F030
#endif


// Calculate system clock settings
#if SYS_USE_HSE == 0
#if defined(PY32F002)
     #define CLK_MASK     ((0b100<<13) | *(uint32_t *)(0x1fff0100))
     #define CLK_DIV      (0)
#else
  #if F_CPU   == 48000000
     #define CLK_MASK     ((0b100<<13) | *(uint32_t *)(0x1fff0f10))
     #define CLK_DIV      (0b000<<11)
  #elif F_CPU == 32000000
     #define CLK_MASK     ((0b010<<13) | *(uint32_t *)(0x1fff0f08))
     #define CLK_DIV      (0b000<<11)
  #elif F_CPU == 24000000
     #define CLK_MASK     ((0b100<<13) | *(uint32_t *)(0x1fff0f10))
     #define CLK_DIV      (0b000<<11)
  #elif F_CPU == 16000000
     #define CLK_MASK     ((0b010<<13) | *(uint32_t *)(0x1fff0f08))
     #define CLK_DIV      (0b000<<11)
  #elif F_CPU == 12000000
     #define CLK_MASK     ((0b100<<13) | *(uint32_t *)(0x1fff0f10))
     #define CLK_DIV      (0b001<<11)
  #elif F_CPU ==  8000000
     #define CLK_MASK     ((0b001<<13) | *(uint32_t *)(0x1fff0f04))
     #define CLK_DIV      (0b000<<11)
  #elif F_CPU ==  6000000
     #define CLK_MASK     ((0b100<<13) | *(uint32_t *)(0x1fff0f10))
     #define CLK_DIV      (0b010<<11)
  #elif F_CPU ==  4000000
     #define CLK_MASK     ((0b000<<13) | *(uint32_t *)(0x1fff0f00))
     #define CLK_DIV      (0b000<<11)
  #elif F_CPU ==  2000000
     #define CLK_MASK     ((0b000<<13) | *(uint32_t *)(0x1fff0f00))
     #define CLK_DIV      (0b001<<11)
  #elif F_CPU ==  1000000
     #define CLK_MASK     ((0b000<<13) | *(uint32_t *)(0x1fff0f00))
     #define CLK_DIV      (0b010<<11)
  #else
     #warning Unsupported system clock frequency, using internal 8 MHz
     #define CLK_MASK ((0b001<<13) | *(uint32_t *)(0x1fff0f04))
     #define CLK_DIV  (0b000<<11)
     #undef  F_CPU
     #define F_CPU        8000000     
  #endif
#endif

  #if F_CPU > 24000000
    #define CLK_init      CLK_init_HSI_PLL
  #else
    #define CLK_init      CLK_init_HSI
  #endif

  #define CLK_HSE_FREQ    (0b00<<2)

#else
  #if F_CPU   >= 16000000
    #define CLK_HSE_FREQ  (0b11<<2)
  #elif F_CPU >=  8000000
    #define CLK_HSE_FREQ  (0b10<<2)
  #elif F_CPU >=  4000000
    #define CLK_HSE_FREQ  (0b01<<2)
  #else
    #error HSE must be at least 4MHz
  #endif

  #define CLK_init        CLK_init_HSE
  #define CLK_MASK        ((0b001<<13) | *(uint32_t *)(0x1fff0f04))
  #define CLK_DIV         (0b000<<11)
#endif

// ===================================================================================
// System Clock Functions
// ===================================================================================

// System clock init functions (called automatically on system start)
void CLK_init_HSI(void);      // init internal oscillator (non PLL) as system clock source
void CLK_init_HSI_PLL(void);  // init internal oscillator with PLL as system clock source
void CLK_init_HSE(void);      // init external crystal (non PLL) as system clock source
void CLK_init_HSE_PLL(void);  // init external crystal with PLL as system clock source
void CLK_reset(void);         // reset system clock to default state

// Internal high-speed clock (HSI) functions
#define HSI_enable()      RCC->CR |=  RCC_CR_HSION        // enable HSI
#define HSI_disable()     RCC->CR &= ~RCC_CR_HSION        // disable HSI
#define HSI_ready()       (RCC->CR & RCC_CR_HSIRDY)       // check if HSI is stable

// External high-speed clock (HSE) functions
#define HSE_enable()      RCC->CR |=  RCC_CR_HSEON        // enable HSE
#define HSE_disable()     RCC->CR &= ~RCC_CR_HSEON        // disable HSE
#define HSE_ready()       (RCC->CR & RCC_CR_HSERDY)       // check if HSE is stable
#define HSE_bypass_on()   RCC->CR |=  RCC_CR_HSEBYP       // enable HSE clock bypass
#define HSE_bypass_off()  RCC->CR &= ~RCC_CR_HSEBYP       // disable HSE clock bypass
#define HSE_CSS_on()      RCC->CR |=  RCC_CR_CSSON        // enable HSE clock security
#define HSE_CSS_off()     RCC->CR &= ~RCC_CR_CSSON        // enable HSE clock security

// Internal 32.768kHz low-speed clock (LSI) functions
#define LSI_enable()      RCC->CSR |=  RCC_CSR_LSION      // enable LSI
#define LSI_disable()     RCC->CSR &= ~RCC_CSR_LSION      // disable LSI
#define LSI_ready()       (RCC->CSR & RCC_CSR_LSIRDY)     // check if LSI is stable

// External 32.768kHz low-speed clock (LSE) functions
#define LSE_enable()      RCC->BDCR |=  RCC_BDCR_LSEON    // enable LSE
#define LSE_disable()     RCC->BDCR &= ~RCC_BDCR_LSEON    // disable LSE
#define LSE_ready()       (RCC->BDCR & RCC_BDCR_LSERDY)   // check if LSE is stable
#define LSE_bypass_on()   RCC->BDCR |=  RCC_BDCR_LSEBYP   // enable LSE clock bypass
#define LSE_bypass_off()  RCC->BDCR &= ~RCC_BDCR_LSEBYP   // disable LSE clock bypass
#define LSE_CSS_on()      RCC->BDCR |=  RCC_BDCR_LSECSSON // enable LSE clock security
#define LSE_CSS_off()     RCC->BDCR &= ~RCC_BDCR_LSECSSON // enable LSE clock security

// Phase-locked loop (PLL) functions
#define PLL_enable()      RCC->CR |=  RCC_CR_PLLON        // enable PLL
#define PLL_disable()     RCC->CR &= ~RCC_CR_PLLON        // disable PLL
#define PLL_ready()       (RCC->CR & RCC_CR_PLLRDY)       // check if PLL is stable
#define PLL_setHSI()      RCC->PLLCFGR = 0                // set HSI as PLL input
#define PLL_setHSE()      RCC->PLLCFGR = 1                // set HSE as PLL input

// Clock output functions
#define MCO_setSYS()      RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_MCOSEL) | (0b001 << 24)
#define MCO_setHSI()      RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_MCOSEL) | (0b011 << 24)
#define MCO_setHSE()      RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_MCOSEL) | (0b100 << 24)
#define MCO_setPLL()      RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_MCOSEL) | (0b101 << 24)
#define MCO_setLSI()      RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_MCOSEL) | (0b110 << 24)
#define MCO_setLSE()      RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_MCOSEL) | (0b111 << 24)
#define MCO_setPRE(n)     RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_MCOPRE) | (((n)&7) << 28)
#define MCO_stop()        RCC->CFGR &= ~RCC_CFGR_MCOSEL

// ===================================================================================
// Real-Time Clock (RTC) Functions
// ===================================================================================
void RTC_init(void);      // init RTC with LSI as clock source and 1s clock period
void RTC_init_LSE(void);  // init RTC with LSE as clock source and 1s clock period
void RTC_setPrescaler(uint32_t val);  // set RTC prescaler (default 32768)
void RTC_setCounter(uint32_t val);    // set RTC counter value
void RTC_setAlarm(uint32_t val);      // set RTC alarm value (default 0xffffffff)
#define RTC_getCounter()  (((uint32_t)RTC->CNTH<<16)|RTC->CNTL)
#define RTC_getAlarm()    (((uint32_t)RTC->ALRH<<16)|RTC->ALRL)

// ===================================================================================
// SYSTICK Timer Functions
// ===================================================================================
#define STK_enable()      SysTick->CTRL = SysTick_CTRL_ENABLE | SysTick_CTRL_CLKSOURCE
#define STK_disable()     SysTick->CTRL = 0

// ===================================================================================
// Delay (DLY) Functions (using SYSTICK)
// ===================================================================================
#define DLY_US_TIME       (F_CPU / 1000000)             // system ticks per us
#define DLY_MS_TIME       (F_CPU / 1000)                // system ticks per ms
#define DLY_us(n)         DLY_ticks((n)*DLY_US_TIME-1)  // delay n microseconds
void DLY_ticks(uint32_t n);                             // delay n+1 clock cycles
static inline void DLY_ms(uint32_t ms) {                // delay n milliseconds
  while(ms--)
  {
     DLY_us(499);
     DLY_us(499);
  }
}

// ===================================================================================
// Independent Watchdog Timer (IWDG) Functions
// ===================================================================================
void IWDG_start(uint16_t ms);                           // start IWDG with time in ms
void IWDG_reload(uint16_t ms);                          // reload IWDG with time in ms
#define IWDG_feed()       IWDG->KR = 0xAAAA             // feed the dog (reload time)

// ===================================================================================
// Low Power Timer (LPTIM) Functions
// ===================================================================================
void LPT_init(void);          // init and enable low-power timer
void LPT_shot(uint16_t ms);   // start LPT single shot with period in ms
void LPT_sleep(uint16_t ms);  // put device in to SLEEP for period in ms
void LPT_stop(uint16_t ms);   // put device in to STOP for period in ms
#define LPT_period(ms)    LPTIM->ARR = (ms)             // set period
#define LPT_restart()     LPTIM->CR |= LPTIM_CR_SNGSTRT // restart timer

// ===================================================================================
// Sleep Functions
// ===================================================================================
void SLEEP_WFI_now(void);   // put device into sleep, wake up by interrupt
void SLEEP_WFE_now(void);   // put device into sleep, wake up by event
void STOP_WFI_now(void);    // put device into stop (deep sleep), wake up interrupt
void STOP_WFE_now(void);    // put device into stop (deep sleep), wake up event
void STOP_lowPower(void);   // set reduced power in stop mode

#define SLEEP_ms(ms)      {LPT_init(); LPT_sleep(ms);}
#define STOP_ms(ms)       {LPT_init(); LPT_stop(ms);}

// ===================================================================================
// Reset (RST) Functions
// ===================================================================================
#define RST_now()         NVIC_SystemReset()
#define RST_clearFlags()  RCC->CSR |= RCC_CSR_RMVF
#define RST_wasWWDG()     (RCC->CSR & RCC_CSR_WWDGRSTF)
#define RST_wasIWDG()     (RCC->CSR & RCC_CSR_IWDGRSTF)
#define RST_wasSoftware() (RCC->CSR & RCC_CSR_SFTRSTF)
#define RST_wasPower()    (RCC->CSR & RCC_CSR_PWRRSTF)
#define RST_wasPin()      (RCC->CSR & RCC_CSR_PINRSTF)
#define RST_wasOption()   (RCC->CSR & RCC_CSR_OBLRSTF)

// ===================================================================================
// Bootloader (BOOT) Functions
// ===================================================================================
#define BOOT_ADDR         0x1fff0000
void BOOT_now(void);      // jump to bootloader

// ===================================================================================
// Cyclic Redundancy Check (CRC) Functions
// ===================================================================================
#define CRC_enable()      RCC->AHBENR |=  RCC_AHBENR_CRCEN
#define CRC_disable()     RCC->AHBENR &= ~RCC_AHBENR_CRCEN
#define CRC_write(w)      CRC->DR = (uint32_t)(w)   // add new word for calculation
#define CRC_read()        (CRC->DR)                 // read last calculation
#define CRC_reset()       CRC->CR = 1               // reset calculation
#define CRC_data32        (*(__IO uint32_t*)CRC_BASE)
#define CRC_data16        (*(__IO uint16_t*)CRC_BASE)
#define CRC_data8         (*(__IO uint8_t*)CRC_BASE)
#define CRC_write32(w)    CRC_data32 = (uint32_t)(w)
#define CRC_write16(h)    CRC_data16 = (uint16_t)(h)
#define CRC_write8(b)     CRC_data8  = (uint8_t)(b)
#define CRC_read32()      CRC_data32
#define CRC_read16()      CRC_data16
#define CRC_read8()       CRC_data8

// ===================================================================================
// Interrupt (INT) Functions
// ===================================================================================
#define INT_enable()          __enable_irq()
#define INT_disable()         __disable_irq()
#define INT_ATOMIC_BLOCK      for(INT_ATOMIC_RESTORE, __ToDo = 1; __ToDo; __ToDo = 0)
#define INT_ATOMIC_RESTORE    uint32_t __reg_save __attribute__((__cleanup__(__iRestore))) = __iSave()

// Save interrupt status and disable interrupts
static inline uint32_t __iSave(void) {
  uint32_t result;
  __ASM volatile("MRS %0, primask" : "=r" (result));
  __ASM volatile("cpsid i" ::: "memory");
  return result;
}

// Restore interrupt status
static inline void __iRestore(const uint32_t *__s) {
  __ASM volatile("MSR primask, %0" :: "r" (*__s) : "memory");
}

// ===================================================================================
// Imported System Functions from cmsis_gcc.h and core_cm0plus.h
// ===================================================================================

// Ignore some GCC warnings
#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#if defined (__thumb__) && !defined (__thumb2__)
#define __CMSIS_GCC_OUT_REG(r) "=l" (r)
#define __CMSIS_GCC_USE_REG(r) "l" (r)
#else
#define __CMSIS_GCC_OUT_REG(r) "=r" (r)
#define __CMSIS_GCC_USE_REG(r) "r" (r)
#endif

// Enable Interrupts
__attribute__( ( always_inline ) ) __STATIC_INLINE void __enable_irq(void) {
  __ASM volatile ("cpsie i" : : : "memory");
}

// Disable Interrupts
__attribute__( ( always_inline ) ) __STATIC_INLINE void __disable_irq(void) {
  __ASM volatile ("cpsid i" : : : "memory");
}

// Get Control Register
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_CONTROL(void) {
  uint32_t result;
  __ASM volatile ("MRS %0, control" : "=r" (result) );
  return(result);
}

// Set Control Register
__attribute__( ( always_inline ) ) __STATIC_INLINE void __set_CONTROL(uint32_t control) {
  __ASM volatile ("MSR control, %0" : : "r" (control) : "memory");
}

// Get IPSR Register
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_IPSR(void) {
  uint32_t result;
  __ASM volatile ("MRS %0, ipsr" : "=r" (result) );
  return(result);
}

// Get APSR Register
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_APSR(void) {
  uint32_t result;
  __ASM volatile ("MRS %0, apsr" : "=r" (result) );
  return(result);
}

// Get xPSR Register
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_xPSR(void) {
  uint32_t result;
  __ASM volatile ("MRS %0, xpsr" : "=r" (result) );
  return(result);
}

// Get Process Stack Pointer
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_PSP(void) {
  register uint32_t result;
  __ASM volatile ("MRS %0, psp\n"  : "=r" (result) );
  return(result);
}

// Set Process Stack Pointer
__attribute__( ( always_inline ) ) __STATIC_INLINE void __set_PSP(uint32_t topOfProcStack) {
  __ASM volatile ("MSR psp, %0\n" : : "r" (topOfProcStack) : "sp");
}

// Get Main Stack Pointer
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_MSP(void) {
  register uint32_t result;
  __ASM volatile ("MRS %0, msp\n" : "=r" (result) );
  return(result);
}

// Set Main Stack Pointer
__attribute__( ( always_inline ) ) __STATIC_INLINE void __set_MSP(uint32_t topOfMainStack) {
  __ASM volatile ("MSR msp, %0\n" : : "r" (topOfMainStack) : "sp");
}

// Get Priority Mask
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_PRIMASK(void) {
  uint32_t result;
  __ASM volatile ("MRS %0, primask" : "=r" (result) );
  return(result);
}

// Set Priority Mask
__attribute__( ( always_inline ) ) __STATIC_INLINE void __set_PRIMASK(uint32_t priMask) {
  __ASM volatile ("MSR primask, %0" : : "r" (priMask) : "memory");
}

// No Operation
__attribute__((always_inline)) __STATIC_INLINE void __NOP(void) {
  __ASM volatile ("nop");
}

// Wait For Interrupt
__attribute__((always_inline)) __STATIC_INLINE void __WFI(void) {
  __ASM volatile ("wfi");
}

// Wait For Event
__attribute__((always_inline)) __STATIC_INLINE void __WFE(void) {
  __ASM volatile ("wfe");
}

// Send Event
__attribute__((always_inline)) __STATIC_INLINE void __SEV(void) {
  __ASM volatile ("sev");
}

// Instruction Synchronization Barrier
__attribute__((always_inline)) __STATIC_INLINE void __ISB(void) {
  __ASM volatile ("isb 0xF":::"memory");
}

// Data Synchronization Barrier
__attribute__((always_inline)) __STATIC_INLINE void __DSB(void) {
  __ASM volatile ("dsb 0xF":::"memory");
}

// Data Memory Barrier
__attribute__((always_inline)) __STATIC_INLINE void __DMB(void) {
  __ASM volatile ("dmb 0xF":::"memory");
}

// Reverse byte order (32 bit)
__attribute__((always_inline)) __STATIC_INLINE uint32_t __REV(uint32_t value) {
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5)
  return __builtin_bswap32(value);
#else
  uint32_t result;
  __ASM volatile ("rev %0, %1" : __CMSIS_GCC_OUT_REG (result) : __CMSIS_GCC_USE_REG (value) );
  return(result);
#endif
}

// Reverse byte order (16 bit)
__attribute__((always_inline)) __STATIC_INLINE uint32_t __REV16(uint32_t value) {
  uint32_t result;
  __ASM volatile ("rev16 %0, %1" : __CMSIS_GCC_OUT_REG (result) : __CMSIS_GCC_USE_REG (value) );
  return(result);
}

// Reverse byte order in signed short value
__attribute__((always_inline)) __STATIC_INLINE int32_t __REVSH(int32_t value) {
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)
  return (short)__builtin_bswap16(value);
#else
  int32_t result;
  __ASM volatile ("revsh %0, %1" : __CMSIS_GCC_OUT_REG (result) : __CMSIS_GCC_USE_REG (value) );
  return(result);
#endif
}

// Rotate Right in unsigned value (32 bit)
__attribute__((always_inline)) __STATIC_INLINE uint32_t __ROR(uint32_t op1, uint32_t op2) {
  return (op1 >> op2) | (op1 << (32U - op2));
}

// Breakpoint
#define __BKPT(value)     __ASM volatile ("bkpt "#value)

// Reverse bit order of value
__attribute__((always_inline)) __STATIC_INLINE uint32_t __RBIT(uint32_t value) {
  uint32_t result;
  int32_t s = 4 /*sizeof(v)*/ * 8 - 1; // extra shift needed at end
  result = value;                      // r will be reversed bits of v; first get LSB of v
  for (value >>= 1U; value; value >>= 1U) {
    result <<= 1U;
    result |= value & 1U;
    s--;
  }
  result <<= s;                        // shift when v's highest bits are zero
  return(result);
}

// Count leading zeros
#define __CLZ             __builtin_clz

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif

// The following MACROS handle generation of the register offset and byte masks
#define _BIT_SHIFT(IRQn)         (  ((((uint32_t)(int32_t)(IRQn))         )      &  0x03UL) * 8UL)
#define _SHP_IDX(IRQn)           ( (((((uint32_t)(int32_t)(IRQn)) & 0x0FUL)-8UL) >>    2UL)      )
#define _IP_IDX(IRQn)            (   (((uint32_t)(int32_t)(IRQn))                >>    2UL)      )

// Enable External Interrupt
__STATIC_INLINE void NVIC_EnableIRQ(IRQn_Type IRQn) {
  NVIC->ISER[0U] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
}

// Disable External Interrupt
__STATIC_INLINE void NVIC_DisableIRQ(IRQn_Type IRQn) {
  NVIC->ICER[0U] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
}

// Get Pending Interrupt
__STATIC_INLINE uint32_t NVIC_GetPendingIRQ(IRQn_Type IRQn) {
  return((uint32_t)(((NVIC->ISPR[0U] & (1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
}

// Set Pending Interrupt
__STATIC_INLINE void NVIC_SetPendingIRQ(IRQn_Type IRQn) {
  NVIC->ISPR[0U] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
}

// Clear Pending Interrupt
__STATIC_INLINE void NVIC_ClearPendingIRQ(IRQn_Type IRQn) {
  NVIC->ICPR[0U] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
}

// Set Interrupt Priority
__STATIC_INLINE void NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority) {
  if ((int32_t)(IRQn) < 0) {
    SCB->SHP[_SHP_IDX(IRQn)] = ((uint32_t)(SCB->SHP[_SHP_IDX(IRQn)] & ~(0xFFUL << _BIT_SHIFT(IRQn))) |
       (((priority << (8U - __NVIC_PRIO_BITS)) & (uint32_t)0xFFUL) << _BIT_SHIFT(IRQn)));
  }
  else {
    NVIC->IP[_IP_IDX(IRQn)]  = ((uint32_t)(NVIC->IP[_IP_IDX(IRQn)]  & ~(0xFFUL << _BIT_SHIFT(IRQn))) |
       (((priority << (8U - __NVIC_PRIO_BITS)) & (uint32_t)0xFFUL) << _BIT_SHIFT(IRQn)));
  }
}

// Get Interrupt Priority
__STATIC_INLINE uint32_t NVIC_GetPriority(IRQn_Type IRQn) {
  if ((int32_t)(IRQn) < 0) {
    return((uint32_t)(((SCB->SHP[_SHP_IDX(IRQn)] >> _BIT_SHIFT(IRQn) ) & (uint32_t)0xFFUL) >> (8U - __NVIC_PRIO_BITS)));
  }
  else {
    return((uint32_t)(((NVIC->IP[ _IP_IDX(IRQn)] >> _BIT_SHIFT(IRQn) ) & (uint32_t)0xFFUL) >> (8U - __NVIC_PRIO_BITS)));
  }
}

// System Reset
__STATIC_INLINE void NVIC_SystemReset(void) {
  __DSB();  // Ensure all outstanding memory accesses included buffered write are completed before reset
  SCB->AIRCR  = ((0x5FAUL << SCB_AIRCR_VECTKEY_Pos) | SCB_AIRCR_SYSRESETREQ_Msk);
  __DSB();                                                          // Ensure completion of memory access
  for(;;) __NOP();                                                  // wait until reset
}

// System Tick Configuration
__STATIC_INLINE uint32_t SysTick_Config(uint32_t ticks) {
  if ((ticks - 1UL) > SysTick_LOAD_RELOAD_Msk) return (1UL);        // Reload value impossible
  SysTick->LOAD  = (uint32_t)(ticks - 1UL);                         // set reload register
  NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL); // set Priority for Systick Interrupt
  SysTick->VAL   = 0UL;                                             // Load the SysTick Counter Value
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                   SysTick_CTRL_TICKINT_Msk   |
                   SysTick_CTRL_ENABLE_Msk;                         // Enable SysTick IRQ and SysTick Timer
  return (0UL);                                                     // Function successful
}

#ifdef __cplusplus
};
#endif
