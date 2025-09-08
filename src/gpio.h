// ===================================================================================
// Basic GPIO Functions for PY32F002, PY32F003, and PY32F030                  * v0.3 *
// ===================================================================================
//
// Pins must be defined as PA0, PA1, .., PF14, PF15 - e.g.:
// #define PIN_LED PC0      // LED on pin PC0
//
// PIN functions available:
// ------------------------
// PIN_input(PIN)           Set PIN as INPUT (floating, no pullup/pulldown)
// PIN_input_PU(PIN)        Set PIN as INPUT with internal PULLUP resistor
// PIN_input_PD(PIN)        Set PIN as INPUT with internal PULLDOWN resistor
// PIN_input_AN(PIN)        Set PIN as INPUT for analog peripherals (e.g. ADC) (*)
// PIN_output(PIN)          Set PIN as OUTPUT (push-pull)
// PIN_output_OD(PIN)       Set PIN as OUTPUT (open-drain)
// PIN_output_OD_PU(PIN)    Set PIN as OUTPUT (open-drain, pullup)
//
// PIN_pullup(PIN)          Enable PULLUP resistor on PIN
// PIN_pulldown(PIN)        Enable PULLDOWN resistor on PIN
// PIN_pulloff(PIN)         Disable PULLUP/PULLDOWN resistor on PIN (*)
// PIN_pushpull(PIN)        Set PIN output type to push-pull (*)
// PIN_opendrain(PIN)       Set PIN output type to open-drain
// PIN_speed(PIN, s)        Set PIN output SPEED (0: very low (*) .. 3: very high)
// PIN_alternate(PIN, AF)   Set alternate function AF (0..15) for PIN
//
// PIN_low(PIN)             Set PIN output value to LOW (*)
// PIN_high(PIN)            Set PIN output value to HIGH
// PIN_toggle(PIN)          TOGGLE PIN output value
// PIN_read(PIN)            Read PIN input value
// PIN_write(PIN, val)      Write PIN output value (0 = LOW / 1 = HIGH)
//
// PORT functions available:
// -------------------------
// PORT_enable(PIN)         Enable GPIO PORT of PIN
// PORTA_enable()           Enable GPIO PORT A
// PORTB_enable()           Enable GPIO PORT B
// PORTF_enable()           Enable GPIO PORT F
// PORTS_enable()           Enable all GPIO PORTS
//
// PORT_disable(PIN)        Disable GPIO PORT of PIN
// PORTA_disable()          Disable GPIO PORT A
// PORTB_disable()          Disable GPIO PORT B
// PORTF_disable()          Disable GPIO PORT F
// PORTS_disable()          Disable all GPIO PORTS
//
// Analog-to-Digital Converter (ADC) functions available:
// ------------------------------------------------------
// ADC_init()               Init, enable and calibrate ADC (must be called first)
// ADC_enable()             Enable ADC (power-up)
// ADC_disable()            Disable ADC (power-down)
// ADC_calibrate()          Calibrate ADC (ADC must be disabled)
//
// ADC_fast()               Set fast mode   (  3.5 ADC clock cycles, least accurate) (*)
// ADC_medium()             Set medium mode ( 71.5 ADC clock cycles, medium accurate)
// ADC_slow()               Set slow mode   (239.5 ADC clock cycles, most accurate)
//
// ADC_input(PIN)           Set PIN as ADC input
// ADC_input_VREF()         Set internal voltage referece (Vref) as ADC input
// ADC_input_TEMP()         Set internal temperature sensor as ADC input
//
// ADC_read()               Sample and read 12-bit ADC value (0..4095)
// ADC_read_VDD()           Sample and read supply voltage (VDD) in millivolts (mV)
// ADC_read_TEMP()          Sample and read temperature sensor in °C
//
// Notes:
// ------
// - (*) reset state
// - Pins used for ADC must be set with PIN_input_AN beforehand. ADC input pins are:
//   PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7, PB0, PB1.
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// ===================================================================================
// Enumerate PIN Designators (use these designators to define pins)
// ===================================================================================
enum{
  PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7, PA8, PA9, PA10, PA11, PA12, PA13, PA14, PA15,
  PB0, PB1, PB2, PB3, PB4, PB5, PB6, PB7, PB8, PB9, PB10, PB11, PB12, PB13, PB14, PB15,
  PF0, PF1, PF2, PF3, PF4, PF5, PF6, PF7, PF8, PF9, PF10, PF11, PF12, PF13, PF14, PF15
};

// ===================================================================================
// Set PIN as INPUT (high impedance, no pullup/pulldown)
// ===================================================================================
#define PIN_input(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOA->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1))) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOB->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1))) : \
  ((PIN>=PF0)&&(PIN<=PF15) ? ( GPIOF->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOF->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1))) : \
(0))))
#define PIN_input_HI PIN_input
#define PIN_input_FL PIN_input

// ===================================================================================
// Set PIN as INPUT with internal PULLUP resistor
// ===================================================================================
#define PIN_input_PU(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOA->PUPDR  =  (GPIOA->PUPDR                        \
                                             & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                             |  ((uint32_t)0b01<<(((PIN)&15)<<1))) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOB->PUPDR  =  (GPIOB->PUPDR                        \
                                             & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                             |  ((uint32_t)0b01<<(((PIN)&15)<<1))) : \
  ((PIN>=PF0)&&(PIN<=PF15) ? ( GPIOF->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOF->PUPDR  =  (GPIOF->PUPDR                        \
                                             & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                             |  ((uint32_t)0b01<<(((PIN)&15)<<1))) : \
(0))))

// ===================================================================================
// Set PIN as INPUT with internal PULLDOWN resistor
// ===================================================================================
#define PIN_input_PD(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOA->PUPDR  =  (GPIOA->PUPDR                        \
                                             & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                             |  ((uint32_t)0b10<<(((PIN)&15)<<1))) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOB->PUPDR  =  (GPIOB->PUPDR                        \
                                             & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                             |  ((uint32_t)0b10<<(((PIN)&15)<<1))) : \
  ((PIN>=PF0)&&(PIN<=PF15) ? ( GPIOF->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOF->PUPDR  =  (GPIOF->PUPDR                        \
                                             & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                             |  ((uint32_t)0b10<<(((PIN)&15)<<1))) : \
(0))))

// ===================================================================================
// Set PIN as INPUT for analog peripherals (e.g. ADC)
// ===================================================================================
#define PIN_input_AN(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->MODER |=  ((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOA->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1))) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->MODER |=  ((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOB->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1))) : \
  ((PIN>=PF0)&&(PIN<=PF15) ? ( GPIOF->MODER |=  ((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOF->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1))) : \
(0))))
#define PIN_input_AD  PIN_input_AN
#define PIN_input_ADC PIN_input_AN

// ===================================================================================
// Set PIN as OUTPUT PUSH-PULL
// ===================================================================================
#define PIN_output(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->MODER  =   (GPIOA->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOA->PUPDR  &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),  \
                               GPIOA->OTYPER &= ~((uint32_t)1<<((PIN)&15)))        : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->MODER  =   (GPIOB->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOB->PUPDR  &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),  \
                               GPIOB->OTYPER &= ~((uint32_t)1<<((PIN)&15)))        : \
  ((PIN>=PF0)&&(PIN<=PF15) ? ( GPIOF->MODER  =   (GPIOF->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOF->PUPDR  &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),  \
                               GPIOF->OTYPER &= ~((uint32_t)1<<((PIN)&15)))        : \
(0))))
#define PIN_output_PP PIN_output

// ===================================================================================
// Set PIN as OUTPUT OPEN-DRAIN
// ===================================================================================
#define PIN_output_OD(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->MODER  =   (GPIOA->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOA->PUPDR  &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),  \
                               GPIOA->OTYPER |=  ((uint32_t)1<<((PIN)&15)))        : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->MODER  =   (GPIOB->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOB->PUPDR  &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),  \
                               GPIOB->OTYPER |=  ((uint32_t)1<<((PIN)&15)))        : \
  ((PIN>=PF0)&&(PIN<=PF15) ? ( GPIOF->MODER  =   (GPIOF->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOF->PUPDR  &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),  \
                               GPIOF->OTYPER |=  ((uint32_t)1<<((PIN)&15)))        : \
(0))))

// ===================================================================================
// Set PIN as OUTPUT OPEN-DRAIN PULLUP
// ===================================================================================
#define PIN_output_OD_PU(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->MODER  =   (GPIOA->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOA->PUPDR  =   (GPIOA->PUPDR                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOA->OTYPER |=  ((uint32_t)1<<((PIN)&15)))        : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->MODER  =   (GPIOB->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOB->PUPDR  =   (GPIOB->PUPDR                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOB->OTYPER |=  ((uint32_t)1<<((PIN)&15)))        : \
  ((PIN>=PF0)&&(PIN<=PF15) ? ( GPIOF->MODER  =   (GPIOF->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOF->PUPDR  =   (GPIOF->PUPDR                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOF->OTYPER |=  ((uint32_t)1<<((PIN)&15)))        : \
(0))))

// ===================================================================================
// Enable PULLUP resistor on PIN
// ===================================================================================
#define PIN_pullup(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->PUPDR =  (GPIOA->PUPDR                        \
                                            & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                            |  ((uint32_t)0b01<<(((PIN)&15)<<1))) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->PUPDR =  (GPIOB->PUPDR                        \
                                            & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                            |  ((uint32_t)0b01<<(((PIN)&15)<<1))) : \
  ((PIN>=PF0)&&(PIN<=PF15) ? ( GPIOF->PUPDR =  (GPIOF->PUPDR                        \
                                            & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                            |  ((uint32_t)0b01<<(((PIN)&15)<<1))) : \
(0))))

// ===================================================================================
// Enable PULLDOWN resistor on PIN
// ===================================================================================
#define PIN_pulldown(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->PUPDR =  (GPIOA->PUPDR                        \
                                            & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                            |  ((uint32_t)0b10<<(((PIN)&15)<<1))) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->PUPDR =  (GPIOB->PUPDR                        \
                                            & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                            |  ((uint32_t)0b10<<(((PIN)&15)<<1))) : \
  ((PIN>=PF0)&&(PIN<=PF15) ? ( GPIOF->PUPDR =  (GPIOF->PUPDR                        \
                                            & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                            |  ((uint32_t)0b10<<(((PIN)&15)<<1))) : \
(0))))

// ===================================================================================
// Disable PULLUP/PULLDOWN resistor on PIN
// ===================================================================================
#define PIN_pulloff(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1)) ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1)) ) : \
  ((PIN>=PF0)&&(PIN<=PF15) ? ( GPIOF->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1)) ) : \
(0))))

// ===================================================================================
// Set PIN output type to push-pull
// ===================================================================================
#define PIN_pushpull(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->OTYPER &= ~((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->OTYPER &= ~((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PF0)&&(PIN<=PF15) ? ( GPIOF->OTYPER &= ~((uint32_t)1<<((PIN)&15)) ) : \
(0))))

// ===================================================================================
// Set PIN output type to open-drain
// ===================================================================================
#define PIN_opendrain(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->OTYPER |=  ((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->OTYPER |=  ((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PF0)&&(PIN<=PF15) ? ( GPIOF->OTYPER |=  ((uint32_t)1<<((PIN)&15)) ) : \
(0))))

// ===================================================================================
// Set PIN output SPEED (0: very low ... 3: very high)
// ===================================================================================
#define PIN_speed(PIN, s) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->OSPEEDR =  (GPIOA->OSPEEDR                         \
                                              & ~((uint32_t)   0b11<<(((PIN)&15)<<1)))   \
                                              |  ((uint32_t)((s)&3)<<(((PIN)&15)<<1))) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->OSPEEDR =  (GPIOB->OSPEEDR                         \
                                              & ~((uint32_t)   0b11<<(((PIN)&15)<<1)))   \
                                              |  ((uint32_t)((s)&3)<<(((PIN)&15)<<1))) : \
  ((PIN>=PF0)&&(PIN<=PF15) ? ( GPIOF->OSPEEDR =  (GPIOF->OSPEEDR                         \
                                              & ~((uint32_t)   0b11<<(((PIN)&15)<<1)))   \
                                              |  ((uint32_t)((s)&3)<<(((PIN)&15)<<1))) : \
(0))))

// ===================================================================================
// Set alternate function AF (0..15) for PIN
// ===================================================================================
#define PIN_alternate(PIN, AF) \
  ((PIN>=PA0)&&(PIN<=PA7)  ? ( GPIOA->AFR[0] =  (GPIOA->AFR[0]                           \
                                             & ~((uint32_t)   0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)((AF)&15)<<(((PIN)&7)<<2))) : \
  ((PIN>=PA8)&&(PIN<=PA15) ? ( GPIOA->AFR[1] =  (GPIOA->AFR[1]                           \
                                             & ~((uint32_t)   0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)((AF)&15)<<(((PIN)&7)<<2))) : \
  ((PIN>=PB0)&&(PIN<=PB7)  ? ( GPIOB->AFR[0] =  (GPIOB->AFR[0]                           \
                                             & ~((uint32_t)   0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)((AF)&15)<<(((PIN)&7)<<2))) : \
  ((PIN>=PB8)&&(PIN<=PB15) ? ( GPIOB->AFR[1] =  (GPIOB->AFR[1]                           \
                                             & ~((uint32_t)   0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)((AF)&15)<<(((PIN)&7)<<2))) : \
  ((PIN>=PF0)&&(PIN<=PF7)  ? ( GPIOF->AFR[0] =  (GPIOF->AFR[0]                           \
                                             & ~((uint32_t)   0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)((AF)&15)<<(((PIN)&7)<<2))) : \
  ((PIN>=PF8)&&(PIN<=PF15) ? ( GPIOF->AFR[1] =  (GPIOF->AFR[1]                           \
                                             & ~((uint32_t)   0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)((AF)&15)<<(((PIN)&7)<<2))) : \
(0)))))))

// ===================================================================================
// Set PIN output value to LOW
// ===================================================================================
#define PIN_low(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->BRR = 1<<((PIN)&15) ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->BRR = 1<<((PIN)&15) ) : \
  ((PIN>=PF0)&&(PIN<=PF15) ? ( GPIOF->BRR = 1<<((PIN)&15) ) : \
(0))))

// ===================================================================================
// Set PIN output value to HIGH
// ===================================================================================
#define PIN_high(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->BSRR = 1<<((PIN)&15) ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->BSRR = 1<<((PIN)&15) ) : \
  ((PIN>=PF0)&&(PIN<=PF15) ? ( GPIOF->BSRR = 1<<((PIN)&15) ) : \
(0))))

// ===================================================================================
// Toggle PIN output value
// ===================================================================================
#define PIN_toggle(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->ODR ^= 1<<((PIN)&15) ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->ODR ^= 1<<((PIN)&15) ) : \
  ((PIN>=PF0)&&(PIN<=PF15) ? ( GPIOF->ODR ^= 1<<((PIN)&15) ) : \
(0))))

// ===================================================================================
// Read PIN input value (returns 0 for LOW, 1 for HIGH)
// ===================================================================================
#define PIN_read(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( (GPIOA->IDR>>((PIN)&15))&1 ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( (GPIOB->IDR>>((PIN)&15))&1 ) : \
  ((PIN>=PF0)&&(PIN<=PF15) ? ( (GPIOF->IDR>>((PIN)&15))&1 ) : \
(0))))

// ===================================================================================
// Write PIN output value (0 = LOW / 1 = HIGH)
// ===================================================================================
#define PIN_write(PIN, val) (val)?(PIN_high(PIN)):(PIN_low(PIN))

// ===================================================================================
// Enable GPIO PORTS
// ===================================================================================
#define PORTA_enable()    RCC->IOPENR |= RCC_IOPENR_GPIOAEN
#define PORTB_enable()    RCC->IOPENR |= RCC_IOPENR_GPIOBEN
#define PORTF_enable()    RCC->IOPENR |= RCC_IOPENR_GPIOFEN
#define PORTS_enable()    RCC->IOPENR  = RCC_IOPENR_GPIOAEN \
                                       | RCC_IOPENR_GPIOBEN \
                                       | RCC_IOPENR_GPIOFEN

#define PORT_enable(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( RCC->IOPENR |= RCC_IOPENR_GPIOAEN ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( RCC->IOPENR |= RCC_IOPENR_GPIOBEN ) : \
  ((PIN>=PF0)&&(PIN<=PF15) ? ( RCC->IOPENR |= RCC_IOPENR_GPIOFEN ) : \
(0))))

// ===================================================================================
// Disable GPIO PORTS
// ===================================================================================
#define PORTA_disable()   RCC->IOPENR &= ~RCC_IOPENR_GPIOAEN
#define PORTB_disable()   RCC->IOPENR &= ~RCC_IOPENR_GPIOBEN
#define PORTF_disable()   RCC->IOPENR &= ~RCC_IOPENR_GPIOFEN
#define PORTS_disable()   RCC->IOPENR  = 0

#define PORT_disable(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( RCC->IOPENR &= ~RCC_IOPENR_GPIOAEN ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( RCC->IOPENR &= ~RCC_IOPENR_GPIOBEN ) : \
  ((PIN>=PF0)&&(PIN<=PF15) ? ( RCC->IOPENR &= ~RCC_IOPENR_GPIOFEN ) : \
(0))))

// ===================================================================================
// ADC Functions
// ===================================================================================

// ADC calibration registers
#define ADC_TSCAL1          (*(__I uint16_t*)(0x1FFF0F14))    // at 30°C
#define ADC_TSCAL2          (*(__I uint16_t*)(0x1FFF0F18))    // at 85°C

// ADC calibration values
#define ADC_CTEMP1          30    // calibration temperature of TSCAL1
#define ADC_CTEMP2          85    // calibration temperature of TSCAL2

// Set ADC sampling rate
#define ADC_fast()          ADC1->SMPR = 0b000
#define ADC_medium()        ADC1->SMPR = 0b110
#define ADC_slow()          ADC1->SMPR = 0b111

// Set ADC input
#define ADC_input_TEMP()    ADC1->CHSELR = (uint16_t)1<<11
#define ADC_input_VREF()    ADC1->CHSELR = (uint16_t)1<<12
#define ADC_input(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7) ? ( ADC1->CHSELR = (uint16_t)1<<( (PIN)&7)    ) : \
  ((PIN>=PB0)&&(PIN<=PB1) ? ( ADC1->CHSELR = (uint16_t)1<<(((PIN)&7)+8) ) : \
(0)))

// Enable ADC
static inline void ADC_enable(void) {
  ADC->CCR  = ADC_CCR_TSEN | ADC_CCR_VREFEN;    // enable VREF and TEMP
  ADC1->ISR = 0xff;                             // clear ADC flags
  ADC1->CR |= ADC_CR_ADEN;                      // enable ADC
  DLY_us(20);                                   // wait until stable
}

// Disable ADC
static inline void ADC_disable(void) {
  ADC->CCR = 0;                                 // disable VREF and TEMP
  if(ADC1->CR & ADC_CR_ADSTART) {               // ADC still converting?
    ADC1->CR |= ADC_CR_ADSTP;                   // stop ongoing conversion
    while(ADC1->CR & ADC_CR_ADSTP);             // wait for conversion stopped
  }
  ADC1->CR &= ~ADC_CR_ADEN;                     // disable ADC
}

// Calibrate ADC (ADC must be disabled)
static inline void ADC_calibrate(void) {
  ADC1->CR |= ADC_CR_ADCAL;                     // start calibration
  while(ADC1->CR & ADC_CR_ADCAL);               // wait until finished
}

// Setup, calibrate and enable ADC
static inline void ADC_init(void) {
  RCC->APBENR2 |= RCC_APBENR2_ADCEN;            // power on ADC
  ADC1->CFGR2 = (uint32_t)0b0100 << 28;         // set PCLK/16 as ADC clock source
  DLY_us(20);                                   // wait until stable
  ADC_enable();                                 // turn on ADC
}

// Sample and read ADC value of current input
static inline uint16_t ADC_read(void) {
  DLY_us(20);                                   // wait a bit
  ADC1->CR |= ADC_CR_ADSTART;                   // start conversion
  while(!(ADC1->ISR & ADC_ISR_EOC));             // wait until finished
  return ADC1->DR;                              // return result
}

// Sample and read supply voltage (VDD) in millivolts (mV)
static inline uint16_t ADC_read_VDD(void) {
  ADC_input_VREF();                             // set VREF as ADC input
  return((uint32_t)1200 * 4095 / ADC_read());   // return VDD in mV
}

// Sample and read temperature sensor in °C
static inline int8_t ADC_read_TEMP(void) {
  ADC_input_TEMP();                             // set temp sensor as ADC input
#if defined(PY32F002)
  return(ADC_read());
#else
  return((ADC_read()-ADC_TSCAL1)*(ADC_CTEMP2-ADC_CTEMP1)/(ADC_TSCAL2-ADC_TSCAL1)+ADC_CTEMP1);
#endif
}

#ifdef __cplusplus
};
#endif
