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

#ifndef _TRAPS_H
#define _TRAPS_H

struct trap_frame {
	unsigned long 	gs,
                        fs,
                        es,
                        ds,

                        edi,
                        esi,
                        ebp,
                        cr2, /* cr2 instead of esp */
                        ebx,
                        edx,
                        ecx,
                        eax,

                        trapno,
                        err,

                        eip,
                        cs,
                        eflags,
                        esp,
                        ss;
};

#define TRAP_STOP_EXECUTION 1
#define TRAP_CONTINUE_EXECUTION 0

void dump_trap_frame(const struct trap_frame*);
long trap_handler(struct trap_frame*);

#endif
