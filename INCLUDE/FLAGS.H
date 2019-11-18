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

#ifndef _FLAGS_H_
#define _FLAGS_H_

#define get_flags() ({ unsigned long val; \
	__asm__ ("pushfl; popl %0" : "=r" (val) : ); val; })

#define set_flags(x) __asm__ __volatile__("pushl %0; popfl " : : "r" (x) : "memory")

/* disable interrupts */
#define cli() __asm__ ("cli":::"memory")

/* enable interrupts */
#define sti() __asm__ ("sti":::"memory")

#endif
