/*
 * Author   : Vladislav Khudash
 * Source   : https://github.com/vk-candpython/linbot/blob/main/lindef.h
 * Compiler : GCC
 * Platform : Linux (x86_64)
 * Summary  : Standalone declarations and basic utilities.
*/


#pragma once


/*****************************
 *     TYPE-DECLARATIONS     *
 *****************************/


typedef void                 VOID;

typedef _Bool                BOOLEAN;

typedef char                 CHAR;
typedef unsigned char        BYTE;

typedef __INT8_TYPE__        INT8;
typedef __INT16_TYPE__       INT16;
typedef __INT32_TYPE__       INT32;
typedef __INT64_TYPE__       INT64;

typedef __UINT8_TYPE__       UINT8;
typedef __UINT16_TYPE__      UINT16;
typedef __UINT32_TYPE__      UINT32;
typedef __UINT64_TYPE__      UINT64;

typedef signed   long int    LONG;
typedef unsigned long int    ULONG;

typedef __PTRDIFF_TYPE__     SSIZE_T;
typedef __SIZE_TYPE__        SIZE_T;




/*****************************
 *     ALIASES & HELPERS     *
 *****************************/


/* Logical operator aliases */

#define not !
#define or  ||
#define and &&



/* Compiler attribute wrapper helper */
#define _GCC_ATTR(...) \
    __attribute__((__VA_ARGS__))

/* Compiler builtin shortcut */
#define _GCC_FUNC(name) \
    __builtin_##name

/* Compiler assembly block shorthand */
#define _GCC_ASM \
    __asm__ volatile


/* Compile-time constant check */
#define _CONSTANT_P(x) _GCC_FUNC(constant_p)((x))

#define _UNLIKELY(x) _GCC_FUNC(expect)(not not (x), 0) // Branch prediction (cold)
#define _LIKELY(x)   _GCC_FUNC(expect)(not not (x), 1) // Branch prediction (hot)


/* Inform compiler that
   this code path is never reached */
#define _UNREACHABLE() _GCC_FUNC(unreachable)()


/* Function declaration helper */
#define DEC_FUNC(TYPE) \
    static inline _GCC_ATTR(always_inline) TYPE

/* Register declaration helper */
#define DEC_REG(TYPE, reg, val) \
    register TYPE reg __asm__(#reg) = (TYPE)(val)

/* Const-qualified pointer helper
   (const data and const pointer) */
#define CONST_PTR(TYPE) \
    const TYPE *const

/* Restrict-qualified pointer helper
   (guarantees no memory aliasing) */
#define RESTR_PTR(TYPE) \
    TYPE *restrict


/* Calculate static array element count */
#define ARRAY_SIZE(arr) \
    (sizeof((arr)) / sizeof(*(arr)))



/* Check for Linux syscall failure */
#define SYSFAIL(status) \
    _UNLIKELY((ULONG)(status) >= (ULONG)-MAX_ERRNO)




/********************************
 *     GENERAL-DECLARATIONS     *
 ********************************/


#define NULL ((VOID*)0)

#define FALSE ((BOOLEAN)0)
#define TRUE  ((BOOLEAN)1)

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1


/* System x64 call numbers */

#define SYS_read          0
#define SYS_write         1
#define SYS_openat        257
#define SYS_close         3
#define SYS_poll          7
#define SYS_socket        41
#define SYS_connect       42
#define SYS_exit_group    231

#define MAX_ERRNO 4095


/* Fcntl flags */

#define O_RDONLY 00

#define AT_FDCWD -100


/* Socket definitions */

/* Construct IPv4 address (little-endian)
   Do not wrap in htonl() */
#define IPv4(a, b, c, d) (            \
    ((UINT32)((UINT8)(a)) << 0 ) |    \
    ((UINT32)((UINT8)(b)) << 8 ) |    \
    ((UINT32)((UINT8)(c)) << 16) |    \
    ((UINT32)((UINT8)(d)) << 24)      \
)

#define htons(x) _GCC_FUNC(bswap16)((UINT16)(x))
#define htonl(x) _GCC_FUNC(bswap32)((UINT32)(x))

#define AF_INET     2
#define SOCK_STREAM 1
#define IPPROTO_TCP 6

#define SOCK_TIMEOUT 3000

struct sockaddr_in {
    UINT16    sin_family;
    UINT16    sin_port;
    UINT32    sin_addr;
    UINT8     sin_zero[8];
};




/************************************
 *     SYSTEM-CALL-DECLARATIONS     *
 ************************************/


#define _SYS_CALL_ENTRY(nm, ...) ({       \
    DEC_REG(LONG, rax, nm);               \
                                          \
    _GCC_ASM ("syscall\n\t"               \
        : "+a"(rax)                       \
        : __VA_ARGS__                     \
        : "memory", "cc", "rcx", "r11"    \
    );                                    \
                                          \
    rax;                                  \
})

#define _SYS_CALL_0(nm, ...) \
    _SYS_CALL_ENTRY(nm, ##__VA_ARGS__)


#define _SYS_CALL_1(nm, a1, ...) ({              \
    DEC_REG(LONG, rdi, a1);                      \
    _SYS_CALL_0(nm, "r"(rdi), ##__VA_ARGS__);    \
})

#define _SYS_CALL_2(nm, a1, a2, ...) ({              \
    DEC_REG(LONG, rsi, a2);                          \
    _SYS_CALL_1(nm, a1, "r"(rsi), ##__VA_ARGS__);    \
})

#define _SYS_CALL_3(nm, a1, a2, a3, ...) ({              \
    DEC_REG(LONG, rdx, a3);                              \
    _SYS_CALL_2(nm, a1, a2, "r"(rdx), ##__VA_ARGS__);    \
})

#define _SYS_CALL_4(nm, a1, a2, a3, a4, ...) ({              \
    DEC_REG(LONG, r10, a4);                                  \
    _SYS_CALL_3(nm, a1, a2, a3, "r"(r10), ##__VA_ARGS__);    \
})

#define _SYS_CALL_5(nm, a1, a2, a3, a4, a5, ...) ({             \
    DEC_REG(LONG, r8, a5);                                      \
    _SYS_CALL_4(nm, a1, a2, a3, a4, "r"(r8), ##__VA_ARGS__);    \
})

#define _SYS_CALL_6(nm, a1, a2, a3, a4, a5, a6, ...) ({             \
    DEC_REG(LONG, r9, a6);                                          \
    _SYS_CALL_5(nm, a1, a2, a3, a4, a5, "r"(r9), ##__VA_ARGS__);    \
})


#define _SYS_CALL_SELECTOR(_0, _1, _2, _3, _4, _5, _6, N, ...) \
    _SYS_CALL_##N




/**************************************
 *     UNIX-FUNCTION-DECLARATIONS     *
 **************************************/


#define syscall(nm, ...)                           \
    _SYS_CALL_SELECTOR(nm, ##__VA_ARGS__,          \
        6, 5, 4, 3, 2, 1, 0)(nm, ##__VA_ARGS__)


#define read(fd, buf, count) \
    syscall(SYS_read, (fd), (buf), (count))

#define write(fd, buf, count) \
    syscall(SYS_write, (fd), (buf), (count))

#define openat(dfd, filename, flags, ...) \
    syscall(SYS_openat, (dfd), (filename), (flags), ##__VA_ARGS__)

#define open(filename, flags, ...) \
    openat(AT_FDCWD, (filename), (flags), ##__VA_ARGS__)

#define close(fd) \
    syscall(SYS_close, (fd))

#define sys_sleep_ms(ms) \
    syscall(SYS_poll, 0, 0, (ms))

#define socket(domain, type, protocol) \
    syscall(SYS_socket, (domain), (type), (protocol))

#define connect(sockfd, addr, addrlen) \
    syscall(SYS_connect, (sockfd), (addr), (addrlen))

#define exit_group(status) do {           \
    syscall(SYS_exit_group, (status));    \
    _UNREACHABLE();                       \
} while (0)
