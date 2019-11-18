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
 * io.c - functions only used for debugging io
 */

#include <kernel.h>
#include <string.h>

struct io_struct {
	int start, stop;
        char name[10];
} io_allocs[128];

int no_ios = 0;

int alloc_io(int start, int stop, char *name)
{
	/* first check if used */
        /* ... */
        io_allocs[no_ios].start = start;
        io_allocs[no_ios].stop = stop;
        strncpy(io_allocs[no_ios].name, name, 16);
        no_ios++;
        return 1;
}

void io_stats(void)
{
	int i;
        printk("\e15======= IO STATISTICS =======\n");
        for (i = 0; i < no_ios; i++) {
		if (strlen(io_allocs[i].name)>0)
			printk("\e150x%04x-0x%04x \eN- \e14%10s\n", io_allocs[i].start,
				io_allocs[i].stop, io_allocs[i].name);
        }
        printk("\e15===== END OF STATISTICS =====\n\eN");
}

void delay1ms(void)
{
	inb(0x84); inb(0x84); inb(0x84);
	inb(0x84); inb(0x84); inb(0x84);
}

