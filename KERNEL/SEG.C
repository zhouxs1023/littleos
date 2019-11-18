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
 * seg.c - routines for GDT, IDT, TSS, LDT...
 */

#include <kernel.h>
#include <seg.h>

extern struct i386_descriptor gdt[GDT_SZ];      /* in start.asm */
extern struct i386_gate idt[256];

void undef_int(), undef_trap();

struct i386_descriptor *get_descriptor(int sel)
{
	if (sel & 2) return 0;	/* descriptor is in the LDT, return an error */

	if ( (sel>8) && (sel/8<GDT_SZ) && (gdt[sel/8].access & ACC_P) )
        	return (struct i386_descriptor*) &gdt[sel/8];
	else return 0;
}

ulong get_segment_base(int sel)
{
	struct i386_descriptor *d = get_descriptor(sel);
        if (d) return (d->base_low + (d->base_med << 16) + (d->base_high << 24));
	 else return 0;
}

int set_segment_base(int sel, ulong base)
{
	struct i386_descriptor *d = get_descriptor(sel);
        if (d) {
        	fill_descriptor_base(d, base);
                return 1;
	} else return 0;
}

ulong get_segment_limit(int sel)
{
	struct i386_descriptor *d = get_descriptor(sel);
	if (d) {
		ulong limit = d->limit_low + (d->limit_high << 16);
        	if (d->granularity & SZ_G) limit <<= 12;
         	return limit;
        } else return 0;
}

int set_segment_limit(int sel, ulong limit)
{
	struct i386_descriptor *d = get_descriptor(sel);
        if (d) {
        	fill_descriptor_limit(d, limit);
                return 1;
	} else return 0;
}

void init_gdt(void)
{
	/* first part of the GDT is setup in start.asm */
        /* nothing yet */
        /* example: fill_gdt_descriptor(KERNEL_CS, ...); */
}

/***** Interruptions *****/

/* int.asm */
extern void irq0(), irq1(), irq2(), irq3(), irq4(), irq5(),
	irq6(), irq7(), irq8(), irq9(), irq10(), irq11(), irq12(),
        irq13(), irq14(), irq15();

void init_idt(void)
{
	int i;
        struct pseudo_descriptor idtr;
        for (i = 0; i < 32; i++)
        	fill_gate(&idt[i], (long)&undef_trap, KERNEL_CS, ACC_TRAP_GATE, 0);

        fill_gate(&idt[IRQ(0)], (long)&irq0, KERNEL_CS, ACC_TRAP_GATE, 0);
        fill_gate(&idt[IRQ(1)], (long)&irq1, KERNEL_CS, ACC_TRAP_GATE, 0);
        fill_gate(&idt[IRQ(2)], (long)&irq2, KERNEL_CS, ACC_TRAP_GATE, 0);
        fill_gate(&idt[IRQ(3)], (long)&irq3, KERNEL_CS, ACC_TRAP_GATE, 0);
        fill_gate(&idt[IRQ(4)], (long)&irq4, KERNEL_CS, ACC_TRAP_GATE, 0);
        fill_gate(&idt[IRQ(5)], (long)&irq5, KERNEL_CS, ACC_TRAP_GATE, 0);
        fill_gate(&idt[IRQ(6)], (long)&irq6, KERNEL_CS, ACC_TRAP_GATE, 0);
        fill_gate(&idt[IRQ(7)], (long)&irq7, KERNEL_CS, ACC_TRAP_GATE, 0);
        fill_gate(&idt[IRQ(8)], (long)&irq8, KERNEL_CS, ACC_TRAP_GATE, 0);
        fill_gate(&idt[IRQ(9)], (long)&irq9, KERNEL_CS, ACC_TRAP_GATE, 0);
        fill_gate(&idt[IRQ(10)], (long)&irq10, KERNEL_CS, ACC_TRAP_GATE, 0);
        fill_gate(&idt[IRQ(11)], (long)&irq11, KERNEL_CS, ACC_TRAP_GATE, 0);
        fill_gate(&idt[IRQ(12)], (long)&irq12, KERNEL_CS, ACC_TRAP_GATE, 0);
        fill_gate(&idt[IRQ(13)], (long)&irq13, KERNEL_CS, ACC_TRAP_GATE, 0);
        fill_gate(&idt[IRQ(14)], (long)&irq14, KERNEL_CS, ACC_TRAP_GATE, 0);
        fill_gate(&idt[IRQ(15)], (long)&irq15, KERNEL_CS, ACC_TRAP_GATE, 0);

	for (i = IRQ(16); i < 256; i++)
        	fill_gate(&idt[i], (long)&undef_int, KERNEL_CS, ACC_INTR_GATE, 0);


        /* now load IDT */
	idtr.limit = 256*8-1;
        idtr.linear_base = (long)&idt + virtual_addr;
	lidt(&idtr);
}

void undef_trap(void)
{
	panic("Undefined trap");
}

void undef_int(void)
{
	panic("Undefined interrupt");
}

