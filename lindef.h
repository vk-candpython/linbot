/*
 * Author   : Vladislav Khudash
 * Source   : https://github.com/vk-candpython/linbot/blob/main/lindef.h
 * Compiler : GCC (GNU)
 * Platform : Linux (x86_64)
 * Summary  : Standalone declarations and basic utilities.
*/


#ifndef LINBOT_LINDEF_H
#define LINBOT_LINDEF_H


/************************************
 *     COMPILATION REQUIREMENTS     *
 ************************************/


#if !defined(__GNUC__) || defined(__clang__)
    #error "lindef.h: GCC compiler is required."
#endif

#if defined(__STRICT_ANSI__)
    #error "lindef.h: GNU extensions required."
#endif

#if !defined(__STDC_VERSION__) || (__STDC_VERSION__ < 201112L)
    #error "lindef.h: C11 standard or higher is required."
#endif

#if !defined(__linux__)
    #error "lindef.h: Linux OS is required."
#endif

#if !defined(__x86_64__)
    #error "lindef.h: x86_64 architecture is required."
#endif

#if !defined(__LP64__)
    #error "lindef.h: LP64 ABI is required."
#endif




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

typedef long int             LONG;
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
#define GCC_ATTR_(...) __attribute__((__VA_ARGS__))

/* Compiler builtin shortcut */
#define GCC_FUNC_(name) __builtin_##name

/* Compiler assembly block shorthand */
#define GCC_ASM_ __asm__ volatile


#define CONSTANT_P_(x) GCC_FUNC_(constant_p)((x))  // Compile-time constant check
#define UNLIKELY_(x)   GCC_FUNC_(expect)(!!(x), 0) // Branch prediction (cold)
#define LIKELY_(x)     GCC_FUNC_(expect)(!!(x), 1) // Branch prediction (hot)

/* Inform compiler that
   this code path is never reached */
#define UNREACHABLE_() GCC_FUNC_(unreachable)()

/* Load integer TYPE from memory at pointer (unaligned-safe) */
#define LOAD_INT_(ptr, TYPE) ({                     \
    TYPE _val;                                      \
    GCC_FUNC_(memcpy)(&_val, ptr, sizeof(TYPE));    \
    _val;                                           \
})


/* Structure declaration helper */
#define DEC_STRUCT(name) \
    typedef struct name name; struct name

/* Packed structure declaration helper (no alignment gaps) */
#define DEC_STRUCT_PACKED(name) \
    typedef struct name name; struct GCC_ATTR_(packed) name

/* Function declaration helper */
#define DEC_FUNC(TYPE) \
    static inline GCC_ATTR_(always_inline) TYPE

/* Register declaration helper */
#define DEC_REG(TYPE, reg, val) \
    register TYPE reg __asm__(#reg) = (TYPE)(val)

/* Const-qualified pointer helper
   (const data and const pointer) */
#define CONST_PTR(TYPE) const TYPE *const

/* Restrict-qualified pointer helper
   (guarantees no memory aliasing) */
#define RESTR_PTR(TYPE) TYPE *restrict


/* Calculate static array element count */
#define ARRAY_SIZE(arr) (sizeof((arr)) / sizeof(*(arr)))



/* Check for Linux syscall (failure) */
#define SYSFAIL(status) UNLIKELY_((LONG)(status) < 0L)

/* Check for Linux syscall (success) */
#define SYSOK(status) LIKELY_((LONG)(status) >= 0L)




/********************************
 *     GENERAL-DECLARATIONS     *
 ********************************/


#define NULL ((VOID*)0)

#define FALSE ((BOOLEAN)0)
#define TRUE  ((BOOLEAN)1)


/* System x64 call numbers */

#define SYSNUM_read         0
#define SYSNUM_write        1
#define SYSNUM_openat       257
#define SYSNUM_close        3
#define SYSNUM_poll         7
#define SYSNUM_mmap         9
#define SYSNUM_munmap       11
#define SYSNUM_rt_sigaction 13
#define SYSNUM_socket       41
#define SYSNUM_connect      42
#define SYSNUM_getrandom    318

#define EINTR 4


/* Signal definitions */

#define SIGHUP  1
#define SIGPIPE 13
#define SIGCHLD 17

#define SIG_IGN ((VOID*)1)

DEC_STRUCT (kernel_sigaction) {
    VOID* sa_handler;
    ULONG sa_flags;
    VOID* sa_restorer;
    ULONG sa_mask;
};


/* Mman mapping flags */

#define PROT_READ  0x1
#define PROT_WRITE 0x2

#define MAP_PRIVATE   0x02
#define MAP_ANONYMOUS 0x20

#define MAP_FAILED ((VOID*)-1)


/* Fcntl flags */

#define O_RDONLY 00

#define AT_FDCWD -100


/* Do not block if entropy pool is empty (getrandom) */
#define GRND_NONBLOCK 0x0001


/* Socket definitions */

/* Construct IPv4 address (little-endian)
   Do not wrap in htonl() */
#define IPv4(a, b, c, d) (            \
    ((UINT32)((UINT8)(a)) << 0 ) |    \
    ((UINT32)((UINT8)(b)) << 8 ) |    \
    ((UINT32)((UINT8)(c)) << 16) |    \
    ((UINT32)((UINT8)(d)) << 24)      \
)

#define htons(x) GCC_FUNC_(bswap16)((UINT16)(x))
#define htonl(x) GCC_FUNC_(bswap32)((UINT32)(x))

#define AF_INET     2
#define SOCK_STREAM 1
#define IPPROTO_TCP 6

/* Retry delay: 5000ms base, 4095 mask (~3-7s range) */

#define SOCK_RETRY_DELAY 5000UL
#define SOCK_RETRY_MASK  4095UL


DEC_STRUCT (sockaddr_in) {
    UINT16 sin_family;
    UINT16 sin_port;
    UINT32 sin_addr;
    BYTE   sin_zero[8];
};




/************************************
 *     SYSTEM-CALL-DECLARATIONS     *
 ************************************/


#define SYS_CALL_ENTRY_(nm, ...) ({       \
    DEC_REG(LONG, rax, nm);               \
                                          \
    GCC_ASM_ ("syscall\n\t"               \
        : "+a"(rax)                       \
        : __VA_ARGS__                     \
        : "memory", "cc", "rcx", "r11"    \
    );                                    \
                                          \
    rax;                                  \
})

#define SYS_CALL_0_(nm, ...) SYS_CALL_ENTRY_(nm, ##__VA_ARGS__)


#define SYS_CALL_1_(nm, a1, ...) ({              \
    DEC_REG(LONG, rdi, a1);                      \
    SYS_CALL_0_(nm, "r"(rdi), ##__VA_ARGS__);    \
})

#define SYS_CALL_2_(nm, a1, a2, ...) ({              \
    DEC_REG(LONG, rsi, a2);                          \
    SYS_CALL_1_(nm, a1, "r"(rsi), ##__VA_ARGS__);    \
})

#define SYS_CALL_3_(nm, a1, a2, a3, ...) ({              \
    DEC_REG(LONG, rdx, a3);                              \
    SYS_CALL_2_(nm, a1, a2, "r"(rdx), ##__VA_ARGS__);    \
})

#define SYS_CALL_4_(nm, a1, a2, a3, a4, ...) ({              \
    DEC_REG(LONG, r10, a4);                                  \
    SYS_CALL_3_(nm, a1, a2, a3, "r"(r10), ##__VA_ARGS__);    \
})

#define SYS_CALL_5_(nm, a1, a2, a3, a4, a5, ...) ({             \
    DEC_REG(LONG, r8, a5);                                      \
    SYS_CALL_4_(nm, a1, a2, a3, a4, "r"(r8), ##__VA_ARGS__);    \
})

#define SYS_CALL_6_(nm, a1, a2, a3, a4, a5, a6, ...) ({             \
    DEC_REG(LONG, r9, a6);                                          \
    SYS_CALL_5_(nm, a1, a2, a3, a4, a5, "r"(r9), ##__VA_ARGS__);    \
})


#define SYS_CALL_SELECTOR_(_0, _1, _2, _3, _4, _5, _6, N, ...) SYS_CALL_##N##_




/**************************************
 *     UNIX-FUNCTION-DECLARATIONS     *
 **************************************/


#define sys_call(num, ...)                          \
    SYS_CALL_SELECTOR_(num, ##__VA_ARGS__,          \
        6, 5, 4, 3, 2, 1, 0)(num, ##__VA_ARGS__)


#define sys_read(fd, buf, count) \
    sys_call(SYSNUM_read, (fd), (buf), (count))

#define sys_write(fd, buf, count) \
    sys_call(SYSNUM_write, (fd), (buf), (count))

#define sys_openat(dfd, filename, flags, ...) \
    sys_call(SYSNUM_openat, (dfd), (filename), (flags), ##__VA_ARGS__)

#define sys_open(filename, flags, ...) \
    sys_openat(AT_FDCWD, (filename), (flags), ##__VA_ARGS__)

#define sys_close(fd) \
    sys_call(SYSNUM_close, (fd))

#define sys_sleep_ms(ms) \
    sys_call(SYSNUM_poll, 0, 0, (ms))

#define sys_mmap(addr, len, prot, flags, fd, offset) \
    sys_call(SYSNUM_mmap, (addr), (len), (prot), (flags), (fd), (offset))

#define sys_munmap(addr, len) \
    sys_call(SYSNUM_munmap, (addr), (len))

#define sys_rt_sigaction(signum, act, oldact, sigsetsize) \
    sys_call(SYSNUM_rt_sigaction, (signum), (act), (oldact), (sigsetsize))

#define sys_socket(domain, type, protocol) \
    sys_call(SYSNUM_socket, (domain), (type), (protocol))

#define sys_connect(sockfd, addr, addrlen) \
    sys_call(SYSNUM_connect, (sockfd), (addr), (addrlen))

#define sys_urandom(buf, count) \
    sys_call(SYSNUM_getrandom, (buf), (count), GRND_NONBLOCK)


#endif /* LINBOT_LINDEF_H */
