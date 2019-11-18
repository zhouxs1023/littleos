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
 * timer.c - timer functions (timer_handler, delay...)
 */

#include <kernel.h>
#include <timer.h>

extern void irq0();

ulong ticks = 0;
extern int sound_delay;

void timer_handler(void)
{
	ticks++;
        if (sound_delay>0) {
        	sound_delay--;
                if (sound_delay==0) sound(0);
        }
        // schedule();
}

void init_timer(void)
{
	if (alloc_irq(0, (ulong) &timer_handler, "timer") == 0) {
        	panic("irq0 can not be used for timer");
        }
        printk("\eRTimer: \eNOK\n");
        enable_irq(0);
}

void delay(unsigned int no_ticks)
{
	ulong start, i, l;
        start = l = ticks;
        while (l< start+no_ticks) {
        	delay1ms();
        	l = ticks;
                delay1ms();
        }
}

