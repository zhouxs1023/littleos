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
 * traps.c - handled CPU exceptions (page fault, GPF...)
 */

#include <kernel.h>
#include <traps.h>

extern void do_divide(), do_debug(), do_nmi(), do_int3(),
        do_overflow(), do_bounds(), do_invalid_op(), do_fpu_not_avail(),
        do_double_fault(), do_hardware_error(), do_invalid_TSS(),
        do_segment_not_present(), do_stack_segment(),
        do_general_protection(), do_page_fault(), do_fpu_error(), do_reserved();

int reboot_time = 50;

const char* trap_msg[] = {
	"Divide error",			// 0
        "Debug exception",		// 1
        "NMI Interrupt",		// 2
        "Breakpoint",			// 3
        "INTO Detected Overflow",	// 4
        "BOUND Range Exceeded",		// 5
        "Invalid Opcode",		// 6
        "Coprocessor not available",	// 7
        "Double exception",		// 8
        "Coprocessor segment overrun",	// 9
        "Invalid Task State Segment",	// 10
        "Segment not present",		// 11
        "Stack fault",			// 12
        "General protection fault",	// 13
        "Page fault",			// 14
        "reserved",			// 15
        "Coprocessor error",		// 16
        "reserved"			// 17-32
        };

void init_traps(void)
{
	fill_gate(&idt[0], (long)&do_divide, KERNEL_CS, ACC_TRAP_GATE, 0);
	fill_gate(&idt[1], (long)&do_debug, KERNEL_CS, ACC_TRAP_GATE, 0);
	fill_gate(&idt[2], (long)&do_nmi, KERNEL_CS, ACC_TRAP_GATE, 0);
	fill_gate(&idt[3], (long)&do_int3, KERNEL_CS, ACC_TRAP_GATE|ACC_PL_U, 0);
	fill_gate(&idt[4], (long)&do_overflow, KERNEL_CS, ACC_TRAP_GATE|ACC_PL_U, 0);
	fill_gate(&idt[5], (long)&do_bounds, KERNEL_CS, ACC_TRAP_GATE|ACC_PL_U, 0);
	fill_gate(&idt[6], (long)&do_invalid_op, KERNEL_CS, ACC_TRAP_GATE, 0);
	fill_gate(&idt[7], (long)&do_fpu_not_avail, KERNEL_CS, ACC_TRAP_GATE, 0);
	fill_gate(&idt[8], (long)&do_double_fault, KERNEL_CS, ACC_TRAP_GATE, 0);
	fill_gate(&idt[9], (long)&do_hardware_error, KERNEL_CS, ACC_TRAP_GATE, 0);
	fill_gate(&idt[10], (long)&do_invalid_TSS, KERNEL_CS, ACC_TRAP_GATE, 0);
	fill_gate(&idt[11], (long)&do_segment_not_present, KERNEL_CS, ACC_TRAP_GATE, 0);
	fill_gate(&idt[12], (long)&do_stack_segment, KERNEL_CS, ACC_TRAP_GATE, 0);
	fill_gate(&idt[13], (long)&do_general_protection, KERNEL_CS, ACC_TRAP_GATE, 0);
	fill_gate(&idt[14], (long)&do_page_fault, KERNEL_CS, ACC_TRAP_GATE, 0);
	fill_gate(&idt[15], (long)&do_reserved, KERNEL_CS, ACC_TRAP_GATE, 0);
	fill_gate(&idt[16], (long)&do_fpu_error, KERNEL_CS, ACC_TRAP_GATE, 0);
	fill_gate(&idt[17], (long)&do_reserved, KERNEL_CS, ACC_TRAP_GATE, 0);
	fill_gate(&idt[18], (long)&do_reserved, KERNEL_CS, ACC_TRAP_GATE, 0);
	fill_gate(&idt[19], (long)&do_reserved, KERNEL_CS, ACC_TRAP_GATE, 0);
	fill_gate(&idt[20], (long)&do_reserved, KERNEL_CS, ACC_TRAP_GATE, 0);
	fill_gate(&idt[21], (long)&do_reserved, KERNEL_CS, ACC_TRAP_GATE, 0);
	fill_gate(&idt[22], (long)&do_reserved, KERNEL_CS, ACC_TRAP_GATE, 0);
	fill_gate(&idt[23], (long)&do_reserved, KERNEL_CS, ACC_TRAP_GATE, 0);
	fill_gate(&idt[24], (long)&do_reserved, KERNEL_CS, ACC_TRAP_GATE, 0);
	fill_gate(&idt[25], (long)&do_reserved, KERNEL_CS, ACC_TRAP_GATE, 0);
	fill_gate(&idt[26], (long)&do_reserved, KERNEL_CS, ACC_TRAP_GATE, 0);
	fill_gate(&idt[27], (long)&do_reserved, KERNEL_CS, ACC_TRAP_GATE, 0);
	fill_gate(&idt[28], (long)&do_reserved, KERNEL_CS, ACC_TRAP_GATE, 0);
	fill_gate(&idt[29], (long)&do_reserved, KERNEL_CS, ACC_TRAP_GATE, 0);
	fill_gate(&idt[30], (long)&do_reserved, KERNEL_CS, ACC_TRAP_GATE, 0);
	fill_gate(&idt[31], (long)&do_reserved, KERNEL_CS, ACC_TRAP_GATE, 0);

}

void dump_trap_frame(const struct trap_frame *ft)
{
	int i, user_mode = (ft->cs & 3);

	printk("\n\e15=== EXCEPTION %d: \e14%s\e15 ===\n\eN", ft->trapno, trap_msg[ft->trapno]);

        if (ft->trapno == 14) { /* page fault */
        	if (!is_page_valid(ft->cr2)) printk("--> Page is not present\n");
	}

        printk("\e07EAX= \e150x%08lX  \e07EBX= \e150x%08lX  \e07ECX= \e150x%08lX  \e07EDX= \e150x%08lX\n",
        	ft->eax, ft->ebx, ft->ecx, ft->edx);
        printk("\e07ESI= \e150x%08lX  \e07EDI= \e150x%08lX  \e07EBP= \e150x%08lX  \e07ESP= \e150x%08lX\n",
        	ft->esi, ft->edi, ft->ebp, user_mode?ft->esp:(unsigned)&ft->esp);
	printk("\e07CR0= \e150x%08lX  \e07CR2= \e150x%08lX  \e07CR3= \e150x%08lX\n", get_cr0(), get_cr2(), get_cr3() );
        printk("\e07CS:EIP= \e140x%04X:0x%08lX  \e07Eflags= \e150x%08lX\n",
        	ft->cs, ft->eip, ft->eflags);
        printk("\e07DS= \e150x%04X \e07ES= \e150x%04X \e07FS= \e150x%04X \e07GS= \e150x%04X \e07SS= \e150x%04X\n",
        	ft->ds, ft->es, ft->fs, ft->gs, (user_mode?ft->ss:get_ss()) );
	printk("\e14Exception with error code 0x%lX (%ld) in %s mode\n\eN", ft->err, ft->err, user_mode? "user":"kernel");
        if (!user_mode) {
        	printk("Stack:\n");
                for (i = 0; i < 32; i++)
                	printk("%08X%c", (&ft->esp)[i], ((i&7)==7? '\n':' '));
        }
}

struct pseudo_descriptor null_idt = {0,0,0xD0000000};

static char get_seconds()
{
	int i;
        for (i = 0; i < 10000; i++) ;
        outb(0x70, 0);
        delay1ms();
        for (i = 0; i < 5000; i++) ;
	return inb(0x71);
}

void trap_panic(struct trap_frame *tf)
{
	int i;
        int a, b, c;
        if (!(tf->cs & 3)) {

        	dump_trap_frame(tf);

		printk("\n\e15Fatal error: exception in kernel mode\n\eN");

        	/* waiting `i' seconds */

                i = (reboot_time>=5)? reboot_time : 15;	/* min 5 seconds */

		while (i-- >0) {
                	printk("\rRebooting in \e15%d \eNseconds...", i+1);
                        if (i == 0) printk("\n\e15Rebooting now!");
                        /* waiting 1 second: read clock from CMOS */
                        a = b = get_seconds();

                        while (b==a) {
                    		for (c = -32000; c < 32000; c++) ;
                                b = get_seconds();
                        }

                }

                reboot();
        }

        dump_trap_frame(tf);
        panic("System halted.");
}

/* this is called when an exception happen */
long trap_handler(struct trap_frame *tf)
{
	return TRAP_STOP_EXECUTION;
}


