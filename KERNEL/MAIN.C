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
 * main.c - kernel entry point (in C)
 */

#include <kernel.h>
#include <stdarg.h>
#include <paging.h>
#include <kmalloc.h>

#include "../version.h"

void reboot(void)
{
	int k;
	outb(0x3f2,0x00);
	outb(0x64,0xfe);
	for (k=0; k<4000; k++);
	outb(0x64,0xff);
	asm("cli;hlt");
        while (1) ;
}

static char panic_buf[250];

void panic(const char *msg, ...)
{
        va_list args;

        va_start(args, msg);
        vsprintf(panic_buf, msg, args);
        va_end(args);

        puts("\n\e15*** Kernel Panic ***\n\eN");
        puts(panic_buf);
        puts("\n");

        cli();
        while (1) ;
}

extern char btext, etext, bdata, edata, bbss, end;

extern short *video;

#include <seg.h>

extern byte read_cmos(int);

void kernel_main(void)
{
        init_console(); /* this must be first */

        printk("\eBLittle\eROS\eN version "VERSION" by Lacroix Pascal\n");
        printk("%lu bytes code, %lu bytes data and %lu bytes BSS\n",
                &etext - &btext, &edata - &bdata, &end - &bbss);
        init_cpu();	/* cpu detection */
        init_gdt();
        init_pic(0x20, 0x28);
        init_pit(200);
        init_idt();
        init_traps();

        init_memory();  /* memory allocation routines (phys_alloc_*) */
        init_paging();  /* maps all memory */
	/* now, we can use kmalloc and kfree */

        /* should be init_drivers() instead of all init_* */
	init_timer();
	init_keyboard();
	init_lpt();
	init_serial();
	init_mouse();
	init_fd();

	sti();

	init_shell();

        while (1)  ;

}

