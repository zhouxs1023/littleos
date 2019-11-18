/*
 *  LittleOS
 *  Copyright (C) 1998 Lacroix Pascal (placr@mygale.org)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * Changes by Eran Rundstein
 */

#ifndef KERNEL_H
#define KERNEL_H

#ifndef i386
#define i386
#endif

#include <types.h>
#include <gdt.h>

#include <isa.h>
#include "../version.h"

#define ASM __asm__
#define VOLATILE __volatile__
#define INLINE extern __inline__

#define IRQ(x) (0x20 + x)

extern ulong id_map;
#define IDMAP (id_map)

#define KBD_NOMODIFIER   0
#define KBD_SHIFT        1
#define KBD_LEFTSHIFT    2
#define KBD_RIGHTSHIFT   4
#define KBD_CTRL         8
#define KBD_LEFTCTRL    16
#define KBD_RIGHTCTRL   32
#define KBD_ALT         64
#define KBD_ALTGR      128

#define NR_ROWS 	25
#define NR_LINES 	NR_ROWS
#define NR_COLUMNS 	80
#define VIDMEM_BASE	0xB8000
#define VIDEO_ADDR	(VIDMEM_BASE + IDMAP)

#ifdef DEBUG

#define DELAY { unsigned long l; for (l = 0; l < 10000*20; l++) ; }

#define D(x, y...) ({ printk("Debug(%s in %s:%d): ", __FUNCTION__, __FILE__, __LINE__); printk(x "\n", ## y); DELAY; })
#define assert(x) if (!(x)) { printk("Assertion `%s' failed at line %d in function %s in file %s\n", #x, __LINE__, __FUNCTION__, __FILE__); panic("halt"); }

#else

#define D(x, y...)
#define assert(x)

#endif

#include <pio.h>
#include <seg.h>
#include <flags.h>

extern ulong total_memory;
extern struct i386_gate idt[256];
extern struct i386_descriptor gdt[GDT_SZ];
extern int sound_delay, hz;

/* main.c */
void reboot() __attribute__ ((noreturn));
void panic(const char* msg, ...) __attribute__ ((noreturn));


/* console.c */
void clrscr(void);
void gotoxy(int, int);

void sound(int);
void beep(void);

void puts(const char *);
int printk(const char*, ...);
void init_console(void);


/* physmem.c */
void init_memory(void);

/* asm.asm */
unsigned char get_byte(unsigned short sel, unsigned long addr);
unsigned short get_word(unsigned short sel, unsigned long addr);
unsigned long get_long(unsigned short sel, unsigned long addr);
void put_byte(unsigned short sel, unsigned long addr, unsigned char val);
void put_word(unsigned short sel, unsigned long addr, unsigned short val);
void put_long(unsigned short sel, unsigned long addr, unsigned long val);


/* start.asm */
extern ulong virtual_base, phys_base;
#define virtual_addr virtual_base
#define phys_addr phys_base


/* pic.c */
void init_pic(ushort, ushort);
void enable_irq(ushort);
void disable_irq(ushort);


/* pit.c */
void init_pit(int);
ulong getticks(void);
#define get_ticks() getticks()

/* traps.c */
void init_traps(void);

/* timer.c */
void init_timer(void);

extern unsigned long ticks;

/* keyboard.c */
void init_keyboard(void);

/* irq.c */
int alloc_irq(int, ulong, char*);
int free_irq(int);
void irq_stats(void);

/* io.c */
int alloc_io(int, int, char *);

/* shell.c */
extern char *prompt;
void newline();
void init_shell();
void proccmd(char *);

/* fd.c */
unsigned char fd_in();
void fd_out(byte val);
void fd_reset();
void fd_start(int drive);
void fd_stop(int drive);
void fd_seek();
void fd_init();
#define init_fd fd_init

/* serial.c */
int uart_detect(unsigned base);
int irq_setup(unsigned base);
void uart_init(unsigned uart_base);
unsigned uart_getchar(unsigned uart_base);
void InitializeSerial(void);
void testserial();
void osSetupSer(void);
#define init_serial osSetupSer

/* lpt.c */
void InitLPT(void);
int OpenLPT(int);
int CloseLPT(int);
int WriteLPT(int, const char*, int);
#define init_lpt InitLPT
#define open_lpt OpenLPT
#define close_lpt CloseLPT
#define write_lpt WriteLPT

/* mouse.c */
void InitializeMouseHardware(unsigned int mtype);
int DetMicrosoft(void);
int CheckMouseType(unsigned int mtype);
void ClearMouse(void);
void InitializeMouse(void);
unsigned char get_text_char(int x, int y);
unsigned char get_text_color(int x, int y);
void put_text_char(int x, int y, unsigned char putchar[2]);
void osSetupMouse(void);
#define init_mouse osSetupMouse

#endif
