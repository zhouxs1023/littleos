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

#ifndef _ISA_H
#define _ISA_H

#define IO_KBD		0x60
#define IO_RTC		0x70
#define IO_WD2		0x170
#define IO_WD1		0x1F0
#define IO_LPT2		0x278
#define IO_LPT1		0x378
#define IO_COM1		0x3F8
#define IO_COM2		0x2F8
#define IO_COM3		0x3E8
#define	IO_COM4		0x2E8
#define IO_VGA		0x3C0
#define IO_FD1		0x3F0
#define IO_FD2		0x370

#define IO_COMSIZE	8
#define IO_DMASIZE	16
#define IO_LPTSIZE	8
#define IO_RTCSIZE	16
#define IO_WDCSIZE	8
#define IO_FDCSIZE	8
#define IO_VGASIZE	16
#define IO_KBDSIZE	16

#endif
