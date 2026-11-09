/*
 * Author   : Vladislav Khudash
 * Source   : https://github.com/vk-candpython/linbot/blob/main/agent.h
 * Compiler : GCC (GNU)
 * Platform : Linux (x86_64)
 * Summary  : Agent configuration and definitions.
*/


#ifndef LINBOT_AGENT_H
#define LINBOT_AGENT_H


/* https://github.com/vk-candpython/linbot/blob/main/lindef.h
INTERNAL LINUX (x86_64) DECLARATIONS */
#include "lindef.h"


#define LB_SERVER_IPV4 IPv4(127, 0, 0, 1)
#define LB_SERVER_PORT 12701


#define LB_NONCE_SIZE 12


typedef UINT64 idsz_t;
typedef UINT8  cmdsz_t;


DEC_STRUCT_PACKED (handshake_t) {
    idsz_t serverID;
    idsz_t sessionID;
    BYTE   nonce[LB_NONCE_SIZE];
};

DEC_STRUCT_PACKED (server_cmd_t) {
    idsz_t  id;
    cmdsz_t command;
    UINT32  len;
};

DEC_STRUCT_PACKED (agent_cmd_t) {
    idsz_t id;
    UINT32 len;
};


#define LB_CMD_REG     404
#define LB_CMD_SYSINFO 201





#endif /* LINBOT_AGENT_H */
