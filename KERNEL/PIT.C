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
 * pit.c - Programmable Interrupt Timer
 */

#include <kernel.h>
#include <pio.h>

int hz;

void init_pit(int H)
{
	ushort tmp;

	if (H>0) hz = H; else hz = 40;  /* minimum 40 Hz */
	tmp = 1193180 / hz;

        outb(0x43, 0x36);
        outb(0x40, (unsigned char) tmp);
        outb(0x40, (unsigned char) (tmp >> 8));
        printk("\eRPIT:\eN timer frequency is %d Hz\n", hz);
        alloc_io(0x40, 0x43, "pit");
}

ulong getticks()
{
	int tmp;
	for (tmp = 0; tmp < 1000; tmp++) ;
	return ticks;
}

