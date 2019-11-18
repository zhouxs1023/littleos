/*
 *  BuildIN Fast SHELL
 *  Copyright (C) 1998 Eran Rundstein (talrun@actcom.co.il)
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
 * Some commands added by Pascal Lacroix
 */

#include <kernel.h>
#include <stdarg.h>
#include <gdt.h>
#include <string.h>
#include <stdlib.h>

#include <timer.h>
#include <time.h>

#include "../version.h"

char *prompt = "#:/";

void newline() {
     printk("\n%s",prompt);
}


void init_shell()
{ 	/* Currently don't do alot... */
	printk("\eRLoading LittleOS Basic ShellProcessor: ");
	printk("\eNOK!\n");
        printk("Welcome to LittleOS, type `help' for help\n");
	newline();
}

void proccmd(char *cmd) {
	printk("\n");
	if ( (strcmp(cmd, "ver")) == 0) {
		printk("\eRLittle\eBOS\eN "VERSION"\n\eRBuildIN Command processor by: Eran Rundstein\eN\n");
	} else
	if ( (strcmp(cmd, "clear")) == 0) {
		clrscr();
		gotoxy(0,0);
	} else
        if ( (strcmp(cmd, "info")) == 0) {
        	printk("\e14System information:\n\eN");
                io_stats();
                delay(2*hz);
                irq_stats();
                delay(1*hz);
        } else
        if ( (strcmp(cmd, "reboot")) == 0) {
        	printk("\nRebooting... ");
                delay(100); printk("NOW!"); delay(5);
        	reboot();
        } else
        if ( (strncmp(cmd, "va2pa", 5)) == 0) {
                ulong addr = strtoul(cmd+6, NULL, 0);
                printk("va = 0x%08lX, pa = 0x%08lX\n", addr, va_to_pa(addr));
        } else
        if ( (strcmp(cmd, "time")) == 0) {
        	struct time t;
        	printk("Current time: ");
                gettime(&t);
                printk("%02d:%02d:%02d\n", t.hour, t.min, t.sec);
        } else
        if ( (strcmp(cmd, "date")) == 0) {
        	struct date d;
                printk("Current date: ");
                getdate(&d);
                printk("%02d %02d %04d\n", d.day, d.month, d.year);
        } else
        if ( (strcmp(cmd, "help")) == 0) {
        	printk("\eNShell help:\nknown commands:\n");
                printk("\tver           - display OS version\n");
                printk("\tclear         - clear the screen\n");
                printk("\tinfo          - print some info about irqs and io\n");
                printk("\thelp          - print this help\n");
                printk("\treboot        - reboot the PC\n");
                printk("\tva2pa addr    - will print the physical address of the virtual\n");
                printk("\ttime          - print the current time\n");
                printk("\tdate          - print the current date\n");
        } else
        	printk("unknown command\n");
}

