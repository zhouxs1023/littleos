 /* this must be the same in start.asm */
#define GDT_SZ 256

/* Flat 4Gb descriptor - base = 0x00000000 limit = 0xffffffff (4096.0 Mb) */
#define ZERO_DS 0x08
/* Kernel code - base = 0xc0000000 limit = 0x3fffffff (1024.0 Mb) */
#define KERNEL_CS 0x10
/* Kernel data - base = 0xc0000000 limit = 0x3fffffff (1024.0 Mb) */
#define KERNEL_DS 0x18


