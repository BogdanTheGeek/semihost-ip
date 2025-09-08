//------------------------------------------------------------------------------
//       Filename: main.c
//------------------------------------------------------------------------------
//       Bogdan Ionescu (c) 2025
//------------------------------------------------------------------------------
//       Purpose : Application entry point
//------------------------------------------------------------------------------
//       Notes : None
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Module includes
//------------------------------------------------------------------------------
#include "gpio.h"
#include "log.h"
#include "semihost.h"
#include "system.h"

#include "slipdev.h"
#include "uip.h"

#include "SEGGER_RTT.h"

//------------------------------------------------------------------------------
// Module constant defines
//------------------------------------------------------------------------------
#define TAG "main"

#define SYSTICK_ONE_MILLISECOND ((uint32_t)F_CPU / 1000)
#define SYSTICK_ONE_MICROSECOND ((uint32_t)F_CPU / 1000000)
#define millis()                (SysTick->VAL / SYSTICK_ONE_MILLISECOND)
#define ticks()                 (SysTick->VAL)
#define BUF                     ((struct uip_eth_hdr *)&uip_buf[0])

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif /* UNUSED */

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
static uint32_t s_systick = 0;

//------------------------------------------------------------------------------
// Module static function prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Module externally exported functions
//------------------------------------------------------------------------------

/**
 * @brief  Read a character from standard input (stdin).
 * @param  None
 * @return The character read from stdin, or -1 if no character is available.
 * @note   This function blocks until a character is available.
 */
int getchar(void)
{
    return SEMIHOST_SysCall(SYS_READC, NULL);
}

void uip_log(char *msg)
{
    UNUSED(msg);
    // LOGI("uip", msg);
}

int main(void)
{

#if 0
   while(1)
   {
      slipdev_char_put('.');
      int c = getchar();
      if (c != -1)
      {
         printf("You pressed: %c\n", c);
      }
      DLY_ms(1000);
   }
#endif

    // SEGGER_RTT_Init();

    slipdev_init();
    uip_init();
    httpd_init();

    uint32_t last_periodic = millis();

    for (;;)
    {
        uip_len = slipdev_poll();
        if (uip_len > 0)
        {
            uip_input();
            if (uip_len > 0)
            {
                slipdev_send();
            }
        }

#if 1
        for (uint8_t i = 0; i < UIP_CONNS; i++)
        {
            uip_periodic(i);
            if (uip_len > 0)
            {
                slipdev_send();
            }
        }

#else
        const uint32_t now = millis();
        if ((now - last_periodic) >= 100)
        {
            last_periodic = now;

            for (uint8_t i = 0; i < UIP_CONNS; i++)
            {
                uip_periodic(i);
                if (uip_len > 0)
                {
                    slipdev_send();
                }
            }
        }
#endif
    }
}

/**
 * @brief  SysTick interrupt handler.
 * @param  None
 * @return None
 */
void SysTick_Handler(void)
{
    s_systick++;
}

#if 1
void slipdev_char_put(uint8_t c)
{
    (void)_write(SEMIHOST_STDOUT, (char *)&c, 1);
}

void slipdev_write(uint8_t *buf, uint16_t len)
{
    (void)_write(SEMIHOST_STDOUT, (char *)buf, len);
}

uint8_t slipdev_char_poll(uint8_t *c)
{
    int data = getchar();
    if (data != -1)
    {
        *c = data;
        return 1;
    }
    else
    {
        return 0;
    }
}
#else
void slipdev_char_put(uint8_t c)
{
    SEGGER_RTT_Write(0, (const char *)&c, 1);
}

void slipdev_write(uint8_t *buf, uint16_t len)
{
    SEGGER_RTT_Write(0, (const char *)buf, len);
}

uint8_t slipdev_char_poll(uint8_t *c)
{
    int data = SEGGER_RTT_GetKey();
    if (data != -1)
    {
        *c = data;
        return 1;
    }
    else
    {
        return 0;
    }
}
#endif

//------------------------------------------------------------------------------
// Module static functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
