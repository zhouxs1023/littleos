/*
 *  LittleOS
 *  Copyright (C) 1998 Lacroix Pascal (placr@mygale.org)
 *  Copyright (C) 1998 Eran Rundstein
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
 * keyboard.c - keyboard handling
 */

#include <kernel.h>
#include <string.h>

#define USERKEY 300
#define BS USERKEY+1
#define EN USERKEY+2
#define F1 USERKEY+3
#define F2 USERKEY+4
#define F3 USERKEY+5
#define F4 USERKEY+6
#define F5 USERKEY+7
#define F6 USERKEY+8
#define F7 USERKEY+9
#define F8 USERKEY+10
#define F9 USERKEY+11
#define F10 USERKEY+12
#define TAB USERKEY+13

extern struct i386_gate idt[256];
extern void kb_wait(); /* in asm.asm */

extern int c_x;
extern int c_y;

byte leds;
void keyboard_handler();

static int ControlKeys;

int writechar(int sc) {
	int ch = 0;
	if ( (ControlKeys & KBD_SHIFT) == 0)
	switch (sc) { // No SHIFT
               case 16: ch='q'; break;
               case 17: ch='w'; break;
               case 18: ch='e'; break;
               case 19: ch='r'; break;
               case 20: ch='t'; break;
               case 21: ch='y'; break;
               case 22: ch='u'; break;
               case 23: ch='i'; break;
               case 24: ch='o'; break;
               case 25: ch='p'; break;
               case 30: ch='a'; break;
               case 31: ch='s'; break;
               case 32: ch='d'; break;
               case 33: ch='f'; break;
               case 34: ch='g'; break;
               case 35: ch='h'; break;
               case 36: ch='j'; break;
               case 37: ch='k'; break;
	       case 38: ch='l'; break;	
               case 44: ch='z'; break;
               case 45: ch='x'; break;
               case 46: ch='c'; break;
               case 47: ch='v'; break;
               case 48: ch='b'; break;
               case 49: ch='n'; break;
               case 50: ch='m'; break;

               case 2: ch='1'; break;
               case 3: ch='2'; break;
               case 4: ch='3'; break; 
               case 5: ch='4'; break;
               case 6: ch='5'; break;
               case 7: ch='6'; break;
               case 8: ch='7'; break;
               case 9: ch='8'; break;
               case 10: ch='9'; break;
               case 11: ch='0'; break;
		case 57: ch=' '; break;
		case 14: ch=BS; break;
		case 28: ch=EN; break;

		case 185: ch=' '; break;
		case 142: ch=BS; break;
		case 156: ch=EN; break;
				
		case 59: ch=F1;	break;	
		case 60: ch=F2;	break; 		
		case 61: ch=F3;	break;
		case 62: ch=F4;	break;		
		case 63: ch=F5;	break;		
		case 64: ch=F6;	break;		
		case 65: ch=F7;	break;		
		case 66: ch=F8;	break;		
		case 67: ch=F9;	break;		
		case 68: ch=F10; break;		

		
		case 13: ch='='; break;
		case 26: ch='['; break;
		case 27: ch=']'; break;
		case 15: ch=TAB; break;
		case 122: ch='/'; break;
		case 43: ch='\\'; break;
		case 53: ch='/'; break;
	
               default: break;
	}
	else
	switch (sc) { // We got SHIFT
               case 16: ch='Q'; break;
               case 17: ch='W'; break;
               case 18: ch='E'; break;
               case 19: ch='R'; break;
               case 20: ch='T'; break;
               case 21: ch='Y'; break;
               case 22: ch='U'; break;
               case 23: ch='I'; break;
               case 24: ch='O'; break;
               case 25: ch='P'; break;
               case 30: ch='A'; break;
               case 31: ch='S'; break;
               case 32: ch='D'; break;
               case 33: ch='F'; break;
               case 34: ch='G'; break;
               case 35: ch='H'; break;
               case 36: ch='J'; break;
               case 37: ch='K'; break;
	       case 38: ch='L'; break;	
               case 44: ch='Z'; break;
               case 45: ch='X'; break;
               case 46: ch='C'; break;
               case 47: ch='V'; break;
               case 48: ch='B'; break;
               case 49: ch='N'; break;
               case 50: ch='M'; break;

               case 2: ch='!'; break;
               case 3: ch='@'; break;
               case 4: ch='#'; break; 
               case 5: ch='$'; break;
               case 6: ch='%'; break;
               case 7: ch='^'; break;
               case 8: ch='&'; break;
               case 9: ch='*'; break;
               case 10: ch='('; break;
               case 11: ch=')'; break;
		case 57: ch=' '; break;
		case 14: ch=BS; break;
		case 28: ch=EN; break;

		case 185: ch=' '; break;
		case 142: ch=BS; break;
		case 156: ch=EN; break;
				
		case 59: ch=F1;	break;	
		case 60: ch=F2;	break; 		
		case 61: ch=F3;	break;
		case 62: ch=F4;	break;		
		case 63: ch=F5;	break;		
		case 64: ch=F6;	break;		
		case 65: ch=F7;	break;		
		case 66: ch=F8;	break;		
		case 67: ch=F9;	break;		
		case 68: ch=F10; break;		

		
		case 13: ch='+'; break;
		case 26: ch='{'; break;
		case 27: ch='}'; break;
		case 15: ch=TAB; break;
		case 122: ch='/'; break;
		case 43: ch='|'; break;
		case 53: ch='?'; break;
	
               default: ch=0; break;
	}

        return ch;
	
}


static void set_leds(void)
{
	kb_wait();
        outb(0x60, 0xED);
        kb_wait();
        outb(0x60, leds);
}

int probe_keyboard()
{
        /* probe_keyboard does nothing yet! */
	return 1;
}

void init_keyboard(void)
{
	printk("\eRKeyboard:\eN ");
	if (alloc_irq(1, (ulong) &keyboard_handler, "keyboard") == 0) {
        	panic("irq1 can not be used for keyboard");
        }
        if (alloc_io(IO_KBD, IO_KBD + IO_KBDSIZE, "keyboard") == 0) {
        	panic("unable to allocate io range for keyboard");
        }

        leds = 2; /* just num lock (bit 2) */
        set_leds();
        enable_irq(1);
        if (!probe_keyboard()) {
        	printk("warning: not found\n");
                /* should we use com port ? */
        }
        else printk("OK\n");
}

#define CAPS 4
#define NUM 2
#define SCROLL 1

extern int pg_fault;

char buf[256];
int inbuf=0;

/* this is called by irq1 in irq.asm */
void keyboard_handler(unsigned long irq)
{
	int c = inb(0x60);
	int cc;
        static int show_all = 0;
	static int doit = 0;

        if (c == 1) { /* 1 = ESC */
        	show_all = !show_all;
                printk("Show all scancodes: \e15%s\n\eN", show_all?"Yes" : "No");
	}
	if (c == 60) { /* 60 = F2 */
		doit = !doit;
		printk("Show keyboard events: \e15%s\n\eN", doit?"Yes" : "No");
	}
	if (c == 67) { /* 67 - F9 */
		printk("\eRGoing for reboot: \e15NOW!\n");
                reboot();
	}
	if (doit)
	        if (show_all) printk("scancode: %d\n", c);
        if (c == 58) { leds = leds ^ CAPS; set_leds();/* caps */}
        else
        if (c == 69) { leds = leds ^ NUM; set_leds(); /* num lock */}
        else
        if (c == 70) { leds = leds ^ SCROLL; set_leds(); /* scroll lock */}
        else
        if (c < 0x80 && !show_all) {
		if (doit)
			printk("scancode: %d\n", c);
                if (c == 59) {    /* make a page fault */
                      printk("Make a page fault with: put_byte(ZERO_DS, 5, 123)\n");

                      put_byte(ZERO_DS, 0xA0000 + 5, 123);
                }
        }
        switch (c) {
		case 0x2a:
         		ControlKeys |= (KBD_SHIFT | KBD_LEFTSHIFT);
			break;

		case 0x36:
		         ControlKeys |= (KBD_SHIFT | KBD_RIGHTSHIFT);
			break;

		case 0xaa:
	         ControlKeys &= ~KBD_LEFTSHIFT;
        	 if (! (ControlKeys & KBD_RIGHTSHIFT))
            		ControlKeys &= ~KBD_SHIFT;
	         break;

		case 0xb6:
	        	 ControlKeys &= ~KBD_RIGHTSHIFT;
        		 if (! (ControlKeys & KBD_LEFTSHIFT))
	            		ControlKeys &= ~KBD_SHIFT;
			break;
        }
	if (c < 0x80) {
	   cc = writechar(c);
		switch (cc) {
		case BS: if (c_x<=strlen(prompt))
				beep();
			 else { 
			 	 gotoxy(c_x-1,c_y);
				 printk(" ");
				 gotoxy(c_x-1,c_y);
				 buf[--inbuf]=0;				 
			 } 
			break;
		case EN: proccmd(buf);
			 newline();
			 inbuf=0;
			 buf[inbuf]=0;
			 break;
		case TAB: printk("\t");
			  break;
		case 0: break;
		default: buf[inbuf++]=cc; buf[inbuf]=0; printk("%c",cc); break;
		}
        }
}


