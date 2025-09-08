//------------------------------------------------------------------------------
//       Filename: log.h
//------------------------------------------------------------------------------
//       Bogdan Ionescu (c) 2024
//------------------------------------------------------------------------------
//       Purpose : Defines the logging API
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
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

//------------------------------------------------------------------------------
// Module exported defines
//------------------------------------------------------------------------------
#ifndef CONFIG_DEBUG_ENABLE_LOGS
#define CONFIG_DEBUG_ENABLE_LOGS (1)
#endif

#if (CONFIG_DEBUG_ENABLE_LOGS)
#define LOGD(f_, ...) LOG(eLOG_LEVEL_DEBUG, (f_), ##__VA_ARGS__)
#define LOGI(f_, ...) LOG(eLOG_LEVEL_INFO, (f_), ##__VA_ARGS__)
#define LOGW(f_, ...) LOG(eLOG_LEVEL_WARNING, (f_), ##__VA_ARGS__)
#define LOGE(f_, ...) LOG(eLOG_LEVEL_ERROR, (f_), ##__VA_ARGS__)
#else
#define LOGD(f_, ...) ((void)f_)
#define LOGI(f_, ...) ((void)f_)
#define LOGW(f_, ...) ((void)f_)
#define LOGE(f_, ...) ((void)f_)
#endif

//------------------------------------------------------------------------------
// Module exported type definitions
//------------------------------------------------------------------------------
typedef enum
{
    eLOG_LEVEL_DEBUG = 0,
    eLOG_LEVEL_INFO,
    eLOG_LEVEL_WARNING,
    eLOG_LEVEL_ERROR,
    eLOG_LEVEL_NONE
} LogLevel_e;

//------------------------------------------------------------------------------
// Module exported functions
//------------------------------------------------------------------------------
void LOG_Init(const LogLevel_e level, const uint32_t *const systick);

//------------------------------------------------------------------------------
// Module exported variables
//------------------------------------------------------------------------------
#if (CONFIG_DEBUG_ENABLE_LOGS)
void LOG(const LogLevel_e type, const char *tag, char *format, ...);
#endif

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif
