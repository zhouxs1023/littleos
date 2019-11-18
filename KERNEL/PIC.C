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
 * pic.c - Programmable Interrupt Controller
 */

#include <kernel.h>
#include <pio.h>

#define SLAVE_IRQ 8
#define MASTER_SLAVE 2

#define	ICU0 0x20
#define	ICU1 0xa0

#define	ICU_RESET 0x11

static ushort irq_mask = 0xFFFF;

/* Initialize the PIC */
void init_pic(ushort master, ushort slave)
{
	outb(ICU0, ICU_RESET);
        outb(ICU0+1, master);
        outb(ICU0+1, 0x04);
        outb(ICU0+1, 0x01);
        outb(ICU0+1, 0xFF);
        outb(ICU0, 0x02);

        outb(ICU1, ICU_RESET);
        outb(ICU1+1, slave);
        outb(ICU1+1, 0x02);
        outb(ICU1+1, 0x01);
        outb(ICU1+1, 0xFF);
        outb(ICU1, 0x02);

        printk("\eRPIC:\eN master at 0x%02x, slave at 0x%02x\n", master, slave);

        alloc_io(ICU0, ICU0 + 1, "pic0");
        alloc_io(ICU1, ICU1 + 1, "pic1");
}

/* enables irq irq_no */
void enable_irq(ushort irq_no)
{
	irq_mask &= ~(1 << irq_no);
	if(irq_no >= SLAVE_IRQ)
		irq_mask &= ~(1 << MASTER_SLAVE);
	
	outb(ICU0+1, irq_mask & 0xFF);
	outb(ICU1+1, (irq_mask >> 8) & 0xFF);
}

/* disables irq irq_no */
void disable_irq(ushort irq_no)
{
	irq_mask |= (1 << irq_no);
	if(irq_no >= SLAVE_IRQ)
		irq_mask |= (1 << MASTER_SLAVE);
	
	outb(ICU0+1, irq_mask & 0xFF);
	outb(ICU1+1, (irq_mask >> 8) & 0xFF);
}

