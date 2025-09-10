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

//------------------------------------------------------------------------------
// Module constant defines
//------------------------------------------------------------------------------
#define TAG "main"

#define SYSTICK_ONE_MILLISECOND ((uint32_t)F_CPU / 1000)
#define SYSTICK_ONE_MICROSECOND ((uint32_t)F_CPU / 1000000)
#define millis()                (SysTick->VAL / SYSTICK_ONE_MILLISECOND)
#define ticks()                 (SysTick->VAL)

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

/**
 * @brief  Read data from a file descriptor using semihosting.
 * @param  fd - file descriptor (SEMIHOST_STDIN)
 * @param  buf - pointer to the buffer to store read data
 * @param  count - number of bytes to read
 * @return The number of bytes read, or 0 if no data is available.
 */
static inline int read(int fd, void *buf, int count)
{
    int32_t args[3] = {fd, (int32_t)buf, count};
    int ret = SEMIHOST_SysCall(SYS_READ, &args[0]);
    if (ret < 0) return 0;      // nothing
    if (ret == 0) return count; // all bytes read
    return count - ret;         // return number of bytes read
}

void uip_log(char *msg)
{
    UNUSED(msg);
    // LOGI("uip", msg);
}

int main(void)
{

#if 0 // enable for testing semihosting i/o
    char c;
    while (1)
    {
        slipdev_char_put('.');
        if (slipdev_char_poll((uint8_t *)&c))
        {
            printf("You pressed: %c\n", c);
        }
        DLY_ms(500);
    }
#elif 0 // test buffered read
    char rx_buffer[10];
    while (1)
    {
        int ret = read(SEMIHOST_STDIN, rx_buffer, sizeof(rx_buffer));
        if (ret != 0)
        {
            printf("Read %d bytes: \r\n", ret);
        }
        DLY_ms(100);
    }

#endif

    slipdev_init();
    uip_init();
    httpd_init();

    uint32_t lastMillis = millis();

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

        const uint32_t now = millis();
        if (now - lastMillis >= 1000)
        {
            lastMillis = now;
            for (uint8_t i = 0; i < UIP_CONNS; i++)
            {
                uip_periodic(i);
                if (uip_len > 0)
                {
                    slipdev_send();
                }
            }
        }
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

extern int _write(int file, char *ptr, int len);
void slipdev_char_put(uint8_t c)
{
    (void)_write(SEMIHOST_STDOUT, (char *)&c, 1);
}

void slipdev_write(uint8_t *buf, uint16_t len)
{
    (void)_write(SEMIHOST_STDOUT, (char *)buf, len);
}

#if 0 // read 1 char at a time
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
#else // buffered read
 
// This can be as big as the SLIP_BUFFER_SIZE for 
// the lowest latency or smaller for lower memory usage
#define RX_BUFFER_SIZE 32
uint8_t slipdev_char_poll(uint8_t *c)
{
    static uint8_t rx_buffer[RX_BUFFER_SIZE];
    static int stored = 0;
    static int sent = 0;

    // Fill the buffer if empty
    if (stored == 0)
    {
        stored = read(SEMIHOST_STDIN, rx_buffer, RX_BUFFER_SIZE);
    }

    // Return a character if available
    if (stored != sent)
    {
        *c = rx_buffer[sent++];
        if (sent == stored)
        {
            sent = 0;
            stored = 0;
        }
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
