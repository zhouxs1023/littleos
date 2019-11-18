/*
 *  LittleOS
 *  Copyright (C) 1998 Eran Rundstein (talrun@actcom.co.il)
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
 ** Serial driver
 ** Written by Eran Rundstein (talrun@actcom.co.il)
 ** For LittleOS
*/

#include <kernel.h>

#define COM1    0x3F8
#define COM2    0x2F8
#define COM3    0x3E8
#define COM4    0x2E8

#define UART_BAUDRATE   96      // 1200 BPS
#define UART_LCRVAL     0x1b    // 0x1b for 8e1
#define UARY_FCRVAL     0x7

int uart_detect(unsigned base)
{
        // Returns 0 if no UART detected

        int olddata=inb(base+4);
        outb(base+4, 0x10);
        if ((inb(base+6) & 0xf0)) return 0;
        return 1;
};

int irq_setup(unsigned base)
{
        // Returns -1 if not found -- otherwise returns interrupt level

        char ier, mcr, imrm, imrs, maskm, masks, irqm, irqs;

        __asm("cli");        // disable all CPU interrupts
        ier = inb(base+1);   // read IER
        outb(base+1,0);      // disable all UART ints
        while (!(inb(base+5)&0x20));  // wait for the THR to be empty
        mcr = inb(base+4);   // read MCR
        outb(base+4,0x0F);   // connect UART to irq line
        imrm = inb(0x21);    // read contents of master ICU mask register
        imrs = inb(0xA1);    // read contents of slave ICU mask register
        outb(0xA0,0x0A);     // next read access to 0xA0 reads out IRR
        outb(0x20,0x0A);     // next read access to 0x20 reads out IRR
        outb(base+1,2);      // let's generate interrupts...
        maskm = inb(0x20);   // this clears all bits except for the one
        masks = inb(0xA0);   // that corresponds to the int
        outb(base+1,0);      // drop the int line
        maskm &= ~inb(0x20); // this clears all bits except for the one
        masks &= ~inb(0xA0); // that corresponds to the int
        outb(base+1,2);      // and raise it again just to be sure...
        maskm &= inb(0x20);  // this clears all bits except for the one
        masks &= inb(0xA0);  // that corresponds to the int
        outb(0xA1,~masks);   // now let us unmask this interrupt only
        outb(0x21,~maskm);
        outb(0xA0,0x0C);     // enter polled mode
        outb(0x20,0x0C);     // that order is important with Pentium/PCI systems
        irqs = inb(0xA0);    // and accept the interrupt
        irqm = inb(0x20);
        inb(base+2);         // reset transmitter interrupt in UART
        outb(base+4,mcr);    // restore old value of MCR
        outb(base+1,ier);    // restore old value of IER
        if (masks) outb(0xA0,0x20);  // send an EOI to slave
        if (maskm) outb(0x20,0x20);  // send an EOI to master
        outb(0x21,imrm);     // restore old mask register contents
        outb(0xA1,imrs);
        __asm("sti");
        if (irqs&0x80)       // slave interrupt occured
          return (irqs&0x07)+8;
        if (irqm&0x80)       // master interrupt occured
          return irqm&0x07;
        return -1;
};

void uart_init(unsigned uart_base)
{
        // Initialize the UART
        outb(uart_base+3, 0x80);
        outw(uart_base, UART_BAUDRATE);
        outb(uart_base+3, UART_LCRVAL);
        outb(uart_base+4, 0);
};

unsigned uart_getchar(unsigned uart_base)
{
        unsigned x;

        x=(inb(uart_base+5) & 0x9f) << 8;
        if(x & 0x100) x|=((unsigned)inb(uart_base)) & 0xff;
        return x;
};

unsigned int serial_handler(unsigned long irq)
{
	/* nothing yet */
};

void InitializeSerial(void)
{
	int i,irq_level;
        unsigned comports[4] = { COM1, COM2, COM3, COM4 };
        char *comname[4] = { "COM1", "COM2", "COM3", "COM4" };
        for (i=0; i<4; i++)
        {
                if(uart_detect(comports[i])==0)
                {
                          printk("\t%s not detected\n", comname[i]);
                } else {
                          uart_init(comports[i]);
                          irq_level=irq_setup(comports[i]);
                          if(irq_level==-1)
                          {
                                    printk("\tWarning: IRQ not detected!\n");
                          } else {
                                    printk("\t%s hooked to interrupt level %d\n", comname[i], irq_level);
                          };
                };
        };
};

// For testing purposes
void testserial()
{
        int i=0;
	char testc;
        union {
          unsigned val;
          char character;
        } x;

        printk("Testing serial input...\n");

        while(i==0) {
          x.val=uart_getchar(COM1);
//    if(!x.val) continue;
//    if(x.val & 0x100)

                testc=inb(COM1);

//    printk("(%x-%c)  %c\n", x.val, x.character, testc);
        };
};

void osSetupSer(void)
{
        printk("\eRSerial: \eNDriver version 0.1\n");
        InitializeSerial();
	printk("\eN\tOK\n");
};

