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
 * physmem.c - physical memory (page allocation)
 */

#include <kernel.h>
#include <paging.h>
#include <string.h>
#include <bitops.h>

extern end;
extern void die();

ulong total_memory, total_pages;

/* support 512M of RAM (you can change this to reduce size) */
#define MAX_MEMORY 512*1024*1024
#define MAX_PAGES (MAX_MEMORY/PAGE_SIZE/32)
static ulong mem_map[MAX_PAGES];	/* 4096*32= 128kb --> supports 512M */
					/* one bit for each page (4096 bytes) */

#define ALIGN4(x) (x) = ( ((x) + 3) & ~3 )

static inline void __free_page(ulong page_no)
{
#ifdef DEBUG
	if (!test_bit(page_no, &mem_map)) {
        	ulong l;
        	D("warning: page %d is already free", page_no);
                for (l = 0; l < 100000000; l++) ;
	}
#endif

        clear_bit(page_no, &mem_map);
}

static inline void __alloc_page(ulong page_no)
{
#ifdef DEBUG
	if (test_bit(page_no, &mem_map)) {
        	ulong l;
		D("warning: page %d is already allocated", page_no);
                for (l = 0; l < 100000000; l++) ;
	}
#endif
	set_bit(page_no, &mem_map);
}

static inline char __is_page_free(ulong page_no)
{
        return !test_bit(page_no, &mem_map);
}

static inline ulong __find_free_page(ulong start)
{
	long l;
        for (l = lin2ptenum(start); l < total_pages; l++)
        	if (__is_page_free(l)) {
                	return l*PAGE_SIZE;
                }
	return (ulong)-1;
}

static ulong start_alloc = 0;

/* allocate a page (4096 bytes) */
ulong phys_alloc_page(int where)
{
        ulong addr = __find_free_page(start_alloc);

        if (start_alloc > total_pages || addr == (ulong)(-1)) {
        	start_alloc = 0;
                addr = __find_free_page(start_alloc);
        }
        if (addr==(ulong)(-1)) return addr;	/* error */
        start_alloc = addr;
        __alloc_page(lin2ptenum(addr));
        return addr;
}

void phys_free_page(ulong addr)
{
	ulong index = PAGE_ROUND(addr) / PAGE_SIZE;
	if (index < total_pages && !__is_page_free(index)) {
        	__free_page(index);
                start_alloc = index * PAGE_SIZE;
	}
}

void phys_mark_page(ulong addr, int what)
{
	if (what == 0) __free_page(PAGE_ROUND(addr) / PAGE_SIZE);
        else
        if (what == 1) __alloc_page(PAGE_ROUND(addr) / PAGE_SIZE);
}

ulong get_free_pages(void)
{
	ulong i, j;
        j = 0;
        for (i = 0; i < total_pages; i++)
        	if (__is_page_free(i)) j++;
	return j;
}

void init_memory(void)
{
	ulong i, j, used = 0;
        ulong kernel_end, tmp;

        if (total_memory>MAX_MEMORY) total_memory = MAX_MEMORY;

        /* number of pages of PAGE_SIZE (=4096) bytes */
        total_pages = (total_memory/PAGE_SIZE);

        j = (long)&end;

        printk("\eRMemory:\eN %d Kb total, %d Kb free (kernel takes %d Kb)\n", total_memory/1024, (total_memory-j)/1024,
        	j/1024);

	/* first set all pages free */
	for (j = 0; j < MAX_PAGES; j++) mem_map[j] = 0;

        /* now we mark some reserved pages */

        /* now BIOS and video area */
        for (j = 0xA0000; j < 0x100000; j+=PAGE_SIZE)
        	__alloc_page(j/PAGE_SIZE);

        /* now kernel */
        for (j = 0; j < PAGE_ROUND_UP((long)&end); j+= PAGE_SIZE)
        	__alloc_page((j+phys_base)/PAGE_SIZE);

	printk("%d total, %d free pages\n", total_pages, get_free_pages());
}

