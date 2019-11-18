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
 ** Mouse driver 0.0.2
 ** Written by Eran Rundstein (talrun@actcom.co.il)
 ** For LittleOS

 ** Known Limitations:
 ** Only supports mice on COM port 1

*/

#include <kernel.h>

#define MOUSE_IRQ_COM1  4
#define MOUSE_IRQ_COM2  3

#define COM1_PORT       0x3f8
#define COM2_PORT       0x2f8

#define max_screen_x    79
#define max_screen_y    24


extern int c_x,c_y;


static unsigned int MOUSE_IRQ=MOUSE_IRQ_COM1;
static unsigned int MOUSE_COM=COM1_PORT;

static unsigned int     bytepos=0, coordinate;
static unsigned char    mpacket[3];
static signed int       mouse_x=40, mouse_y=12,mo_x=40,mo_y=12;
static unsigned char    mouse_button1, mouse_button2;
static signed int       horiz_sensitivity, vert_sensitivity;
static int    mo_c=0;


unsigned int microsoft_mouse_handler(unsigned long irq)
{
	unsigned int mbyte=inb(MOUSE_COM);
	int i;
	ushort *vid = (ushort *)VIDEO_ADDR;
	int scx,scy;

	scx=c_x;
	scy=c_y;
	
	vid[mo_x+mo_y*80]=mo_c;
	mo_c = vid[mouse_x+mouse_y*80];
	vid[mouse_x+mouse_y*80]=((int)'Û' + (7<<8));
	mo_x=mouse_x;
	mo_y=mouse_y;
	
	// Synchronize
	if((mbyte&64)==64) { bytepos=0; }

	mpacket[bytepos]=mbyte;
	bytepos++;
	// Process packet
	if(bytepos==3) {
		// Retrieve change in x and y from packet
		int change_x=((mpacket[0] & 3) << 6) + mpacket[1];
		int change_y=((mpacket[0] & 12) << 4) + mpacket[2];

		// Some mice need this
		if(coordinate==1) {
		  change_x-=128;
		  change_y-=128;
		}

		// Change to signed
		if(change_x>=128) { change_x=change_x-256; }
		if(change_y>=128) { change_y=change_y-256; }

		// Adjust mouse position according to sensitivity
		mouse_x+=change_x/horiz_sensitivity;
		mouse_y+=change_y/vert_sensitivity;

		// Check that mouse is still in screen
		if(mouse_x<0) { mouse_x=0; }
		if(mouse_x>max_screen_x) { mouse_x=max_screen_x; }
		if(mouse_y<0) { mouse_y=0; }
		if(mouse_y>max_screen_y) { mouse_y=max_screen_y; }

		// Retrieve mouse button status from packet
		mouse_button1=mpacket[0] & 32;
		mouse_button2=mpacket[0] & 16;

		bytepos=0;
	}
}

void InitializeMouseHardware(unsigned int mtype)
{
	char clear_error_bits;

	outb(MOUSE_COM+3, 0x80); // set DLAB on
	outb(MOUSE_COM, 0x60); // speed LO byte
	outb(MOUSE_COM+1, 0); // speed HI byte
	outb(MOUSE_COM+3, mtype); // 2=MS Mouse; 3=Mouse systems mouse
	outb(MOUSE_COM+1, 0); // set comm and DLAB to 0
	outb(MOUSE_COM+4, 1); // DR int enable
 
	clear_error_bits=inb(MOUSE_COM+5); // clear error bits
}

int DetMicrosoft(void)
{
	char tmp;
	int buttons;
	char ind;
	int i;
	outb(MOUSE_COM+4, 0x0b);
	buttons=0;
	tmp = inb(MOUSE_COM);
	// Check the first for bytes for signs that this is an MS mouse
	for(i=0; i<4; i++) {
		while( (inb(MOUSE_COM+5) & 1)==0) ;
		ind=inb(MOUSE_COM);
		if(ind==0x33) buttons=3;
		if(ind==0x4d) buttons=2;
	}

	return buttons;
}

int CheckMouseType(unsigned int mtype)
{
	unsigned int retval=0;

	InitializeMouseHardware(mtype);
	if(mtype==2) retval=DetMicrosoft();
	if(mtype==3) {
		outb(MOUSE_COM+4, 11);
		retval=3;
	}
	outb(MOUSE_COM+1, 1);

	return retval;
}

void ClearMouse(void)
{
	unsigned temp;
	unsigned int i;
	// Waits until the mouse calms down but also quits out after a while
	// in case some destructive user wants to keep moving the mouse
	// before we're done

	unsigned int restarts=0;
	for (i=0; i<60000; i++)
	{
	  temp=inb(MOUSE_COM);
	  if(temp!=0) {
	    restarts++;
	    if(restarts<300000) {
		    i=0;
	    } else
	    {
		    i=60000;
	    }
	  }
	}
}

void InitializeMouse(void)
{
	int mbuttons=0, gotmouse=0;
	horiz_sensitivity=2;
	vert_sensitivity=3;

	// Check for Microsoft mouse (2 buttons)
	if(CheckMouseType(2)!=0)
	{
		gotmouse=1;
		printk("Microsoft Mouse Detected\n");
		ClearMouse();
		coordinate=0;
		if (!alloc_irq(MOUSE_IRQ, (ulong) &microsoft_mouse_handler,"msmouse"))
			printk("\eRProblems with mouse...\eN\n");
		enable_irq(MOUSE_IRQ);
	}

	// Check for Microsoft Systems mouse (3 buttons)
	if(gotmouse==0) {
	  if(CheckMouseType(3)!=0)
	  {
		gotmouse=1;
		printk("Microsoft Mouse Detected\n");
		ClearMouse();
		coordinate=1;
		if (!alloc_irq(MOUSE_IRQ, (ulong) &microsoft_mouse_handler,"msmouse3"))
			printk("\eRProblems with mouse...\n");
		enable_irq(MOUSE_IRQ);

	  }
	}

	if(gotmouse==0) printk("No Mouse Detected!\n");
}

void osSetupMouse(void)
{
	printk("\eRMouse: \eNDriver version 0.0.2\n");
	InitializeMouse();
}

