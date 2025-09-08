// ===================================================================================
// Basic System Functions for PY32F002, PY32F003, and PY32F030                * v1.2 *
// ===================================================================================
//
// This file must be included!!! The system configuration and the system clock are 
// set up automatically on system start.

#include "system.h"

// ===================================================================================
// Setup Microcontroller (this function is called automatically at startup)
// ===================================================================================
void SYS_init(void) {
  // Init system clock
  #if SYS_CLK_INIT > 0
  #if F_CPU > 24000000
  FLASH->ACR = FLASH_ACR_LATENCY;                         // increase FLASH latency
  #endif
  CLK_init();
  #endif

  // Init SYSTICK
  #if SYS_TICK_INIT > 0
  SysTick->LOAD = (F_CPU / 1000) - 1; // set SysTick to 1ms
  SysTick->CTRL = SysTick_CTRL_ENABLE | SysTick_CTRL_CLKSOURCE | SysTick_CTRL_TICKINT;
  #endif

  // Enable GPIO
  #if SYS_GPIO_EN > 0
  RCC->IOPENR = RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOBEN | RCC_IOPENR_GPIOFEN;
  #endif
}

// ===================================================================================
// System Clock Functions
// ===================================================================================

// Init internal oscillator (non PLL) as system clock source
void CLK_init_HSI(void) {
  #if CLK_DIV > 0
  RCC->CR = CLK_DIV | RCC_RC_HSION;                       // set HSI divisor
  #endif
  RCC->ICSCR = (RCC->ICSCR & 0xFFFF0000) | CLK_MASK;      // set HSI freq and calibration
}

// Init internal oscillator with PLL as system clock source
void CLK_init_HSI_PLL(void) {
  RCC->ICSCR = (RCC->ICSCR & 0xFFFF0000) | CLK_MASK;      // set HSI freq and calibration
  //RCC->PLLCFGR = RCC_PLLCFGR_PLLSRC_HSI;                // set HSI as source for PLL
  RCC->CR |= RCC_CR_PLLON;                                // Enable PLL
  while(!(RCC->CR & RCC_CR_PLLRDY));                      // Wait till PLL is ready      
  RCC->CFGR = 0b010;                                      // PLL as system clock source
  while((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_PLL); // Wait for PLL
}

// Init external crystal (non PLL) as system clock source
void CLK_init_HSE(void) {
  RCC->ECSCR = (RCC->ECSCR & ~RCC_ECSCR_HSE_FREQ) | CLK_HSE_FREQ; // set frequency
  RCC->CR |= RCC_CR_HSEON;                                // Turn on HSE
  while(!(RCC->CR & RCC_CR_HSERDY));                      // wait till HSE is ready
  RCC->CFGR = 0b001;                                      // HSE as system clock source
  while((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_HSE); // Wait for HSE
}

// Init external crystal with PLL as system clock source
void CLK_init_HSE_PLL(void) {
  RCC->ECSCR = (RCC->ECSCR & ~RCC_ECSCR_HSE_FREQ) | CLK_HSE_FREQ; // set frequency
  RCC->CR |= RCC_CR_HSEON;                                // Turn on HSE
  while(!(RCC->CR & RCC_CR_HSERDY));                      // wait till HSE is ready
  RCC->PLLCFGR = RCC_PLLCFGR_PLLSRC_HSE;                  // set HSE as source for PLL
  RCC->CR |= RCC_CR_PLLON;                                // Enable PLL
  while(!(RCC->CR & RCC_CR_PLLRDY));                      // Wait till PLL is ready      
  RCC->CFGR = 0b010;                                      // PLL as system clock source
  while((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_PLL); // Wait for PLL
}

// Reset system clock to default state
void CLK_reset(void) {
  RCC->CR |= RCC_CR_HSION;                                // enable HSI, no divider
  while(!(RCC->CR & RCC_CR_HSIRDY));                      // wait for HSI ready
  RCC->CFGR    = 0x00000000;                              // HSI as system clock source
  while(RCC->CFGR & RCC_CFGR_SWS_Msk);                    // wait until switched to HSI
  RCC->CR      = RCC_CR_HSION;                            // keep only HSI on
  RCC->ICSCR   = 0x00ff10ff;                              // reset ICSCR register
  RCC->PLLCFGR = 0x00000000;                              // reset PLLCFGR register
  RCC->CIER    = 0x00000000;                              // disable all interrupts
  RCC->CICR    = 0xffffffff;                              // clear all interrupt flags
  FLASH->ACR   = 0x00000000;                              // reset flash latency
}

// ===================================================================================
// Real-Time Clock (RTC) Functions
// ===================================================================================

// Init RTC with LSI as clock source and 1s clock period
void RTC_init(void) {
  LSI_enable();                         // enable internal low-speed clock (LSI)
  RCC->APBENR1 |= RCC_APBENR1_PWREN     // enable low power control block clock
                | RCC_APBENR1_RTCAPBEN; // enable RTC module clock
  PWR->CR1     |= PWR_CR1_DBP;          // enable access to RTC domain
  RCC->BDCR     = (0b10<<8)             // set LSI as clock source
                | RCC_BDCR_RTCEN;       // enable RTC
}

// Init RTC with LSE as clock source and 1s clock period
void RTC_init_LSE(void) {
  RCC->APBENR1 |= RCC_APBENR1_PWREN     // enable low power control block clock
                | RCC_APBENR1_RTCAPBEN; // enable RTC module clock
  PWR->CR1     |= PWR_CR1_DBP;          // enable access to RTC domain
  RCC->BDCR     = RCC_BDCR_LSEON        // turn on LSE
                | (0b01<<8)             // set LSE as clock source
                | RCC_BDCR_RTCEN;       // enable RTC
}

// Set RTC prescaler (default 32768)
void RTC_setPrescaler(uint32_t val) {
  while(!(RTC->CRL & RTC_CRL_RTOFF));   // wait for ready to write
  RTC->CRL  = RTC_CRL_CNF;              // enter configuration mode
  RTC->PRLH = val >> 16;                // set prescaler (high value)
  RTC->PRLL = val & 0xffff;             // set prescaler (low value)
  RTC->CRL  = 0;                        // exit configuration mode -> write changes
}

// Set RTC counter value
void RTC_setCounter(uint32_t val) {
  while(!(RTC->CRL & RTC_CRL_RTOFF));   // wait for ready to write
  RTC->CRL  = RTC_CRL_CNF;              // enter configuration mode
  RTC->CNTH = val >> 16;                // set prescaler (high value)
  RTC->CNTL = val & 0xffff;             // set prescaler (low value)
  RTC->CRL  = 0;                        // exit configuration mode -> write changes
}

// Set RTC alarm value (default 0xffffffff)
void RTC_setAlarm(uint32_t val) {
  while(!(RTC->CRL & RTC_CRL_RTOFF));   // wait for ready to write
  RTC->CRL  = RTC_CRL_CNF;              // enter configuration mode
  RTC->ALRH = val >> 16;                // set prescaler (high value)
  RTC->ALRL = val & 0xffff;             // set prescaler (low value)
  RTC->CRL  = 0;                        // exit configuration mode -> write changes
}

// ===================================================================================
// Delay Functions
// ===================================================================================

// Wait n+1 counts of SysTick
void DLY_ticks(uint32_t n) {
  const uint32_t start = SysTick->VAL;
  while(( start - SysTick->VAL) < n);
}

// ===================================================================================
// Independent Watchdog Timer (IWDG) Functions
// ===================================================================================

// Start independent watchdog timer (IWDG) with given time in milliseconds (max 8191).
// Once the IWDG has been started, it cannot be disabled, only reloaded (feed).
// It can be stopped by disabling the internal low-speed clock (LSI).
void IWDG_start(uint16_t ms) {
  LSI_enable();                         // enable internal low-speed clock (LSI)
  IWDG->KR   = 0x5555;                  // allow register modification
  while(IWDG->SR & IWDG_SR_PVU);        // wait for prescaler register to be ready
  IWDG->PR   = 0b100;                   // set LSI clock prescaler 64 (~0.5kHz)
  while(IWDG->SR & IWDG_SR_RVU);        // wait for reload register to be ready
  IWDG->RLR  = ms >> 1;                 // set watchdog counter reload value
  IWDG->KR   = 0xAAAA;                  // load reload value into watchdog counter
  IWDG->KR   = 0xCCCC;                  // enable IWDG
}

// Reload watchdog counter with n milliseconds, n<=8191
void IWDG_reload(uint16_t ms) {
  IWDG->KR   = 0x5555;                  // allow register modification
  while(IWDG->SR & IWDG_SR_RVU);        // wait for reload register to be ready
  IWDG->RLR = ms >> 1;                  // set watchdog counter reload value
  IWDG->KR   = 0xAAAA;                  // load reload value into watchdog counter
}

// ===================================================================================
// Low Power Timer (LPT) Functions
// ===================================================================================

// Init and enable low-power timer
void LPT_init(void) {
  LSI_enable();                         // enable internal low-speed clock (LSI)
  RCC->APBENR1 |= RCC_APBENR1_LPTIMEN;  // enable LPT module clock
  RCC->CCIPR   |= (uint32_t)0b01<<18;   // set LSI as LPT clock source
  LPTIM->CFGR   = (uint32_t)0b101<<9;   // set prescaler to 32 (~1kHz)
  LPTIM->IER    = LPTIM_IER_ARRMIE;     // set interrupt enable flag
  LPTIM->CR     = LPTIM_CR_ENABLE;      // enable timer
}

// Start low-power timer single shot with period in ms
void LPT_shot(uint16_t ms) {
  DLY_ms(2);                            // wait two LPT clock cycles
  LPTIM->ARR = ms;                      // set interval
  LPTIM->CR |= LPTIM_CR_SNGSTRT;        // start timer in single mode
}

// Put device in to SLEEP and wake-up after LPT period in ms
void LPT_sleep(uint16_t ms) {
  LPT_shot(ms);                         // start low-power timer (LPT) single shot
  SCB->SCR |=  SCB_SCR_SEVONPEND_Msk;   // enable wake-up on pending interrupt
  SLEEP_WFE_now();                      // put device into stop, wake up by LPT event
  SCB->SCR &= ~SCB_SCR_SEVONPEND_Msk;   // disable wake-up on pending interrupt
  LPTIM->ICR = LPTIM_ICR_ARRMCF;        // clear LPT interrupt flag
  NVIC_ClearPendingIRQ(LPTIM1_IRQn);    // clear NVIC pending interrupt flag
}

// Put device in to STOP (deep sleep) and wake-up after LPT period in ms
void LPT_stop(uint16_t ms) {
  LPT_shot(ms);                         // start low-power timer (LPT) single shot
  SCB->SCR |= SCB_SCR_SEVONPEND_Msk;    // enable wake-up on pending interrupt
  STOP_WFE_now();                       // put device into stop, wake up by LPT event
  SCB->SCR &= ~SCB_SCR_SEVONPEND_Msk;   // disable wake-up on pending interrupt
  LPTIM->ICR = LPTIM_ICR_ARRMCF;        // clear LPT interrupt flag
  NVIC_ClearPendingIRQ(LPTIM1_IRQn);    // clear NVIC pending interrupt flag
}

// ===================================================================================
// Sleep Functions
// ===================================================================================

// Put device into sleep, wake up by interrupt
void SLEEP_WFI_now(void) {
  __WFI();                              // wait for interrupt
}

// Put device into sleep, wake up by event
void SLEEP_WFE_now(void) {
  __WFE();                              // wait for event
}

// Put device into stop (deep sleep), wake up interrupt
void STOP_WFI_now(void) {
  NVIC->ICPR[0] = 0xffffffff;           // clear pending interrupts
  EXTI->PR = 0xffffffff;                // clear all EXTI pending flags
  RTC->CRL = 0;                         // clear RTC interrupt flags
  SCB->SCR |=  SCB_SCR_SLEEPDEEP_Msk;   // set deep sleep mode
  __WFI();                              // wait for interrupt
  SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;   // unset deep sleep mode
}

// Put device into stop (deep sleep), wake up event
void STOP_WFE_now(void) {
  NVIC->ICPR[0] = 0xffffffff;           // clear pending interrupts
  EXTI->PR = 0xffffffff;                // clear all EXTI pending flags
  RTC->CRL = 0;                         // clear RTC interrupt flags
  SCB->SCR |=  SCB_SCR_SLEEPDEEP_Msk;   // set deep sleep mode
  __WFE();                              // wait for event
  SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;   // unset deep sleep mode
}

// Reduce power in stop mode
void STOP_lowPower(void) {
  RCC->APBENR1 |= RCC_APBENR1_PWREN;    // enable low power control block clock
  PWR->CR1 = PWR_CR1_LPR                // use low power regulator in stop mode
           | PWR_CR1_VOS                // use VDD = 1.0V in stop mode
           | (0b011 << 16);             // supply 0.9V to SRAM in stop mode
}

// ===================================================================================
// Bootloader Functions
// ===================================================================================
void BOOT_now(void) {
  void (*SysMemBootJump)(void);
  __disable_irq();                      // disable all interrupts
  SysTick->CTRL = 0;                    // disable SysTick Timer
  CLK_reset();                          // reset system clock settings
  NVIC->ICER[0]=0xffffffff;             // clear interrupt enable register
  NVIC->ICPR[0]=0xffffffff;             // clear interrupt pending register
  __enable_irq();                       // re-enable all interrupts
  SysMemBootJump = (void (*)(void)) (*((uint32_t *) ((BOOT_ADDR + 4))));
  __set_MSP(*(uint32_t *)BOOT_ADDR);    // set main stack pointer for bootloader
  SysMemBootJump();                     // jump to bootloader
  while(1);                             // just be sure
}

// ===================================================================================
// C++ Support
// ===================================================================================
#ifdef __cplusplus
extern void __cxa_pure_virtual() { while (1); }
extern void (*__preinit_array_start[]) (void) __attribute__((weak));
extern void (*__preinit_array_end[]) (void) __attribute__((weak));
extern void (*__init_array_start[]) (void) __attribute__((weak));
extern void (*__init_array_end[]) (void) __attribute__((weak));

void __libc_init_array(void) {
  uint32_t count, i;
  count = __preinit_array_end - __preinit_array_start;
  for (i = 0; i < count; i++) __preinit_array_start[i]();
  count = __init_array_end - __init_array_start;
  for (i = 0; i < count; i++) __init_array_start[i]();
}
#endif

// ===================================================================================
// C version of PY32F030 Startup .s file
// ===================================================================================
extern uint32_t _sbss, _ebss, _sdata, _edata, _sidata;
extern void _estack(void);

// Prototypes
int main(void)                __attribute__((section(".text.main"), used));
void (*const vectors[])(void) __attribute__((section(".isr_vector"), used));
void Reset_Handler(void)      __attribute__((section(".text.irq_handler"), naked, used, noreturn));

#if SYS_USE_VECTORS > 0
// Unless a specific handler is overridden, it just spins forever
void Default_Handler(void)    __attribute__((section(".text.irq_handler"), naked, used));
void Default_Handler(void)    { while(1); }

// All interrupt handlers are aliased to default_handler unless overridden individually
#define DUMMY_HANDLER __attribute__((section(".text.irq_handler"), weak, alias("Default_Handler"), used))
DUMMY_HANDLER void NMI_Handler(void);
DUMMY_HANDLER void HardFault_Handler(void);
DUMMY_HANDLER void SVC_Handler(void);
DUMMY_HANDLER void PendSV_Handler(void);
DUMMY_HANDLER void SysTick_Handler(void);
DUMMY_HANDLER void WWDG_IRQHandler(void);
DUMMY_HANDLER void PVD_IRQHandler(void);
DUMMY_HANDLER void RTC_IRQHandler(void);
DUMMY_HANDLER void FLASH_IRQHandler(void);
DUMMY_HANDLER void RCC_IRQHandler(void);
DUMMY_HANDLER void EXTI0_1_IRQHandler(void);
DUMMY_HANDLER void EXTI2_3_IRQHandler(void);
DUMMY_HANDLER void EXTI4_15_IRQHandler(void);
DUMMY_HANDLER void LCD_IRQHandler(void);
DUMMY_HANDLER void DMA1_Channel1_IRQHandler(void);
DUMMY_HANDLER void DMA1_Channel2_3_IRQHandler(void);
DUMMY_HANDLER void DMA1_Channel4_5_6_7_IRQHandler(void);
DUMMY_HANDLER void ADC_COMP_IRQHandler(void);
DUMMY_HANDLER void TIM1_BRK_UP_TRG_COM_IRQHandler(void);
DUMMY_HANDLER void TIM1_CC_IRQHandler(void);
DUMMY_HANDLER void TIM2_IRQHandler(void);
DUMMY_HANDLER void TIM3_IRQHandler(void);
DUMMY_HANDLER void LPTIM1_IRQHandler(void);
DUMMY_HANDLER void TIM7_IRQHandler(void);
DUMMY_HANDLER void TIM14_IRQHandler(void);
DUMMY_HANDLER void TIM15_IRQHandler(void);
DUMMY_HANDLER void TIM16_IRQHandler(void);
DUMMY_HANDLER void TIM17_IRQHandler(void);
DUMMY_HANDLER void I2C1_IRQHandler(void);
DUMMY_HANDLER void I2C2_IRQHandler(void);
DUMMY_HANDLER void SPI1_IRQHandler(void);
DUMMY_HANDLER void SPI2_IRQHandler(void);
DUMMY_HANDLER void USART1_IRQHandler(void);
DUMMY_HANDLER void USART2_IRQHandler(void);
DUMMY_HANDLER void USART3_4_IRQHandler(void);
DUMMY_HANDLER void LED_IRQHandler(void);
DUMMY_HANDLER void USB_IRQHandler(void);
#endif  // SYS_USE_VECTORS > 0

// Interrupt vector table
void (*const vectors[])(void) = {
  &_estack,                       //  0 - Initial Stack Pointer Value

  // Cortex-M0+ handlers
  Reset_Handler,                  //  1 - Reset
  #if SYS_USE_VECTORS > 0
  NMI_Handler,                    //  2 - NMI
  HardFault_Handler,              //  3 - Hard Fault
  0,                              //  4 - Reserved
  0,                              //  5 - Reserved
  0,                              //  6 - Reserved
  0,                              //  7 - Reserved
  0,                              //  8 - Reserved
  0,                              //  9 - Reserved
  0,                              // 10 - Reserved
  SVC_Handler,                    // 11 - SVCall
  0,                              // 12 - Reserved
  0,                              // 13 - Reserved
  PendSV_Handler,                 // 14 - PendSV
  SysTick_Handler,                // 15 - SysTick

  // Peripheral handlers
  WWDG_IRQHandler,                //  0 - Window Watchdog
  PVD_IRQHandler,                 //  1 - PVD through EXTI Line detect
  RTC_IRQHandler,                 //  2 - RTC through EXTI Line
  FLASH_IRQHandler,               //  3 - FLASH
  RCC_IRQHandler,                 //  4 - RCC
  EXTI0_1_IRQHandler,             //  5 - EXTI Line 0 and 1
  EXTI2_3_IRQHandler,             //  6 - EXTI Line 2 and 3
  EXTI4_15_IRQHandler,            //  7 - EXTI Line 4 to 15
  LCD_IRQHandler,                 //  8 - LCD
  DMA1_Channel1_IRQHandler,       //  9 - DMA1 Channel 1
  DMA1_Channel2_3_IRQHandler,     // 10 - DMA1 Channel 2 and 3
  DMA1_Channel4_5_6_7_IRQHandler, // 11 - DMA1 Channel 4 to 7
  ADC_COMP_IRQHandler,            // 12 - ADC&COMP1
  TIM1_BRK_UP_TRG_COM_IRQHandler, // 13 - TIM1 Break, Update, Trigger and Commutation
  TIM1_CC_IRQHandler,             // 14 - TIM1 Capture Compare
  TIM2_IRQHandler,                // 15 - TIM2
  TIM3_IRQHandler,                // 16 - TIM3
  LPTIM1_IRQHandler,              // 17 - LPTIM1
  TIM7_IRQHandler,                // 18 - TIM7
  TIM14_IRQHandler,               // 19 - TIM14
  TIM15_IRQHandler,               // 20 - TIM15
  TIM16_IRQHandler,               // 21 - TIM16
  TIM17_IRQHandler,               // 22 - TIM17
  I2C1_IRQHandler,                // 23 - I2C1
  I2C2_IRQHandler,                // 24 - I2C2
  SPI1_IRQHandler,                // 25 - SPI1
  SPI2_IRQHandler,                // 26 - SPI2
  USART1_IRQHandler,              // 27 - USART1
  USART2_IRQHandler,              // 28 - USART2
  USART3_4_IRQHandler,            // 29 - USART3, USART4
  LED_IRQHandler,                 // 30 - LED
  USB_IRQHandler,                 // 31 - USB
  #endif  // SYS_USE_VECTORS > 0
};

// Reset handler
void Reset_Handler(void) {
  uint32_t *src, *dst;

  // Set stack pointer
  asm volatile(
  " ldr r0, =_estack  \n\
    mov sp, r0        \n"
    : : : "r0", "memory"
  );

  // Configure vector table location
  SCB->VTOR = (uint32_t)vectors;

  // Copy data from FLASH to RAM
  src = &_sidata;
  dst = &_sdata;
  while(dst < &_edata) *dst++ = *src++;

  // Clear uninitialized variables
  #if SYS_CLEAR_BSS > 0
  dst = &_sbss;
  while(dst < &_ebss) *dst++ = 0;
  #endif

  // Init system
  SYS_init();

  // C++ Support
  #ifdef __cplusplus
  __libc_init_array();
  #endif

  // Start main function
  main();
  while(1);
}
