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
 * irq.c
 */

#include <kernel.h>
#include <string.h>

extern struct i386_gate idt[256];

static struct irq_struct {
	ulong proc;
        char name[10];
        int used;
} irqs[16];

ulong 	proc_irq0=0,   	proc_irq1=0,   	proc_irq2=0,
      	proc_irq3=0,   	proc_irq4=0,   	proc_irq5=0,
      	proc_irq6=0,   	proc_irq7=0,   	proc_irq8=0,
      	proc_irq9=0,   	proc_irq10=0,  	proc_irq11=0,
      	proc_irq12=0,  	proc_irq13=0,  	proc_irq14=0,
      	proc_irq15=0;

ulong	num_irq0=0,	num_irq1=0,	num_irq2=0,
	num_irq3=0,	num_irq4=0,	num_irq5=0,
	num_irq6=0,	num_irq7=0,	num_irq8=0,
	num_irq9=0,	num_irq10=0,	num_irq11=0,
	num_irq12=0,	num_irq13=0,	num_irq14=0,
	num_irq15=0;


int alloc_irq(int no, ulong proc_addr, char* name)
{
	ulong flags = get_flags();

	if (no <= 16 && irqs[no].used == 0) {
                irqs[no].proc = proc_addr;
                irqs[no].used = 1;
                strncpy(irqs[no].name, name, 10);
                #define SET_I(x) case x: proc_irq##x = proc_addr; break;
                switch (no) {
                	SET_I(0)
                        SET_I(1)
                        SET_I(2)
                        SET_I(3)
                        SET_I(4)
                        SET_I(5)
                        SET_I(6)
                        SET_I(7)
                        SET_I(8)
                        SET_I(9)
                        SET_I(10)
                        SET_I(11)
                        SET_I(12)
                        SET_I(13)
                        SET_I(14)
                        SET_I(15)
                        default: panic("alloc_irq: Undefined no\n");
                }
                #undef SET_I
                set_flags(flags);
                return 1;
        }
        D("alloc_irq: irq %d is already used\n", no);
        set_flags(flags);
	return 0;
}

int free_irq(int no)
{
	ulong flags = get_flags();
        cli();

	if (irqs[no].used == 1) {
		irqs[no].used = 0;
                strcpy(irqs[no].name, "");
                #define CLEAR(x) case x: proc_irq##x = 0; num_irq##x = 0; break;
                switch (no) {
                        CLEAR(0)
                        CLEAR(1)
                        CLEAR(2)
                        CLEAR(3)
                        CLEAR(4)
                        CLEAR(5)
                        CLEAR(6)
                        CLEAR(7)
                        CLEAR(8)
                        CLEAR(9)
                        CLEAR(10)
                        CLEAR(11)
                        CLEAR(12)
                        CLEAR(13)
                        CLEAR(14)
                        CLEAR(15)
                        default: panic("free_irq: Undefined no\n");
                }
                #undef CLEAR
                set_flags(flags);
                return 1;
        }
        else {
        	D("free_irq: irq %d is not used\n", no);
                set_flags(flags);
        	return 0;
        }
}

void irq_stats(void)
{

	printk("\e15====== IRQ STATISTICS =======\n");
        #define DUMP_INFO(x) { \
                if (strlen(irqs[x].name)>0) \
                        printk("\e15irq%02d:\e14 %12s \eN(\eB%ld\eN ints)\n", x, irqs[x].name, num_irq##x); \
                        else      \
                        printk("\e15irq%02d:\eN %12s (%ld ints)\n", x, "undefined", num_irq##x); \
                }
        DUMP_INFO(0)
        DUMP_INFO(1)
        DUMP_INFO(2)
        DUMP_INFO(3)
        DUMP_INFO(4)
        DUMP_INFO(5)
        DUMP_INFO(6)
        DUMP_INFO(7)
        DUMP_INFO(8)
        DUMP_INFO(9)
        DUMP_INFO(10)
        DUMP_INFO(11)
        DUMP_INFO(12)
        DUMP_INFO(13)
        DUMP_INFO(14)
        DUMP_INFO(15)
        printk("\e15===== END OF STATISTICS =====\n\eN");
}

