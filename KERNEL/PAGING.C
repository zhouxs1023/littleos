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

#include <kernel.h>
#include <paging.h>

extern end;

static ulong virtual_begin, virtual_end;
static ulong phys_begin, phys_end;

static ulong current_break = 0;

extern ulong phys_alloc_page();

void map_memory();

void init_paging(void)
{
        long i, l;

        printk("\eRPaging:\eN ");
        /* protect from user processes */
        for (i = 0; i < 1024; i++)
          lowmem_page_table[i] = (i*PAGE_SIZE) | PTE_WRITE | PTE_VALID;

        i = (long) &end;
        l = (PAGE_ROUND_UP(i))/PAGE_SIZE;

        for (i = 0; i < l; i++)
          system_page_table[i] = (i*PAGE_SIZE + phys_base) | PTE_WRITE | PTE_VALID;

        map_memory();

        FLUSH_TLB();

        /* to test virtual/physical translation */
        put_long(ZERO_DS, virtual_begin, 123456);
        printk("%s\n", get_long(ZERO_DS, phys_begin) == 123456 ? "OK" : "Error");
        /* should always be OK! */

}

/* map a page at physical address pa to virtual address va */
int map_page(ulong pa, ulong va)
{
	ulong pde_a, pde_val;
        ulong pte_a, pte_val, index;

        /* round to a page */
        pa = PAGE_ROUND(pa);
        va = PAGE_ROUND(va);

        pde_a = (ulong) &page_directory[lin2pdenum(va)] + phys_addr;
        pde_val = get_long(ZERO_DS, pde_a);

        if (!(pde_val & PDE_VALID)) {
        	/* creating a new entry */
                pte_a = phys_alloc_page();
                if (pte_a == (ulong) -1) return -1;
                pde_val = pte_a | PDE_VALID | PDE_WRITE;
                put_long(ZERO_DS, pde_a, pde_val);
        } else pte_a = (pde_val & 0xFFFFF000);

        index = (va / PAGE_SIZE) % PAGE_SIZE;
        pte_a+= index*sizeof(ulong);
        pte_val = pa | PTE_VALID | PTE_WRITE;

        put_long(ZERO_DS, pte_a, pte_val);

        return 1; /* good */
}

void map_memory(void)
{
	ulong starta, enda;
        ulong pa, va;

        starta = (ulong) &end;
        starta+= phys_addr;
        starta = PAGE_ROUND_UP(starta);

        enda = total_memory-4095;
        enda = PAGE_ROUND_UP(enda);

	/* first we map virtual == physical */
        /* in order to be able to use ZERO_DS with physical memory */
        for (pa = 0; pa < enda; pa+= PAGE_SIZE)
        	map_page(pa, pa);

        /* now we map only memory after the kernel at 2GB */
        va = 1024*1024*1024*2UL;

        virtual_begin = va;
        phys_begin = starta;

        for (pa = starta; pa < enda; pa+= PAGE_SIZE, va+= PAGE_SIZE)
        	map_page(pa, va);

        virtual_end = va;
        phys_end = enda;
}

/* return the attributs and address of a page */
ulong get_page_entry(ulong addr)
{
	ulong pde_a, pde_val;
        ulong pte_a, pte_val;

        addr = PAGE_ROUND(addr);

        pde_a = (ulong) &page_directory[lin2pdenum(addr)] + phys_addr;
        pde_val = get_long(ZERO_DS, pde_a);

        /* page directory is not valid, return 0 */
        if (!(pde_val & PDE_VALID)) return 0;

        pte_a = pde_val & 0xFFFFF000;
        pte_a+= ((addr / PAGE_SIZE) % PAGE_SIZE) * sizeof(ulong);
        pte_val = get_long(ZERO_DS, pte_a);

        return pte_val;
}

ulong get_page_attr(ulong addr)
{
	return get_page_entry(addr) & 0x0FFF;
}

int is_page_valid(ulong addr)
{
	return (get_page_attr(addr) & PTE_VALID);
}

ulong va_to_pa(ulong va)
{
	ulong addr = get_page_entry(va);
        if (addr & PTE_VALID) return (addr & 0xFFFFF000);
        else return (ulong)-1;
}

extern void phys_mark_page(ulong addr, int what);

/* note: you should not use this with (delta % PAGE_SIZE)!=0 */
void *ksbrk(ulong delta)
{
	ulong addr;
	if (current_break == 0) current_break = virtual_begin;

        if (delta == 0) return (void *) (current_break - virtual_base);

        D("ksbrk(%d K)", delta/1024);
	if (delta % PAGE_SIZE || delta < 0) return (void *) -1;

	addr = current_break;
        while (delta > 0) {
                /* reserve physical page (mark it allocated) */
		phys_mark_page(va_to_pa(current_break), 1);

       		current_break += PAGE_SIZE;
               	delta -= PAGE_SIZE;
        }
	if (current_break > virtual_end) return (void *) -1;

        return (void *) (addr - virtual_base);
}

