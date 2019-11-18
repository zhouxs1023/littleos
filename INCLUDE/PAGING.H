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

#ifndef _PAGING_H
#define _PAGING_H

#include <kernel.h>

#define PAGE_SIZE	4096
#define PAGE_SZ 	PAGE_SIZE
#define PAGE_SHIFT 	12

#define PDE_SHIFT	22
#define PDE_MASK	0x3FF

#define PTE_SHIFT	PAGE_SHIFT
#define PTE_MASK        0x3FF

#define PAGE_ROUND_UP(x) ( (x&0xfff) ? ((x&(~0xfff))+0x1000) : x )
#define PAGE_ROUND(x) ((x) & 0xFFFFF000)

#define FLUSH_TLB()    set_cr3(get_cr3())

/* Intel constants */
#define PTE_VALID	0x00000001
#define PTE_WRITE	0x00000002
#define PTE_USER	0x00000004
#define PTE_WTHRU	0x00000008
#define PTE_NCACHE	0x00000010
#define PTE_REF		0x00000020
#define PTE_MOD		0x00000040
#define PTE_GLOBAL	0x00000100
#define PTE_AVAIL	0x00000E00
#define PTE_PFN		0xFFFFF000

#define PDE_VALID	0x00000001
#define PDE_WRITE	0x00000002
#define PDE_USER	0x00000004
#define PDE_WTHRU	0x00000008
#define PDE_NCACHE	0x00000010
#define PDE_REF		0x00000020
#define PDE_AVAIL	0x00000E00
#define PDE_PFN		0xFFFFF000

/* linear address to page number */
#define lin2pdenum(x)	(((x) >> PDE_SHIFT) & PDE_MASK)
#define lin2ptenum(x)	(((x) >> PTE_SHIFT) & PTE_MASK)

/* page number to address */
#define pdenum2lin(x)	((ulong)(x) << PDE_SHIFT)
#define ptenum2lin(x)	((ulong)(x) <> PTE_SHIFT)


extern ulong kernelbase;

extern ulong page_directory[], lowmem_page_table[],
	 system_page_table[];

void init_paging(void);
int map_page(ulong pa, ulong va);
ulong get_page_attr(ulong va);
ulong get_page_entry(ulong va);
int is_page_valid(ulong va);

ulong va_to_pa(ulong va);

#endif
