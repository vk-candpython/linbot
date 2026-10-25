/*
 * Author   : Vladislav Khudash
 * Source   : https://github.com/vk-candpython/linbot/blob/main/agent.h
 * Compiler : GCC
 * Platform : Linux (x86_64)
 * Summary  : Agent configuration and definitions.
*/


#pragma once


#if !defined(__linux__)
    #error "linbot: Linux is required."
#endif

#if !defined(__x86_64__)
    #error "linbot: x86_64 is required."
#endif

#if !defined(__LP64__)
    #error "linbot: LP64 ABI is required."
#endif


/* https://github.com/vk-candpython/linbot/blob/main/lindef.h
INTERNAL LINUX (x86_64) DECLARATIONS */
#include "lindef.h"




#define BOT_SERVER_IPV4 IPv4(127, 0, 0, 1)
#define BOT_SERVER_PORT 2022

