/*
 *  LittleOS
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
 * lpt.c - parallel port handling (printer)
 */

#include <kernel.h>
#include <pio.h>

#define LPT_EXISTS	1		// lp exists
#define LPT_OPEN 	2		// lp is in use

// bit defines for 8255 status port (base + 1)
#define LP_PBUSY	0x80 	// printer is busy, active low
#define LP_PACK		0x40 	// printer acknowledge, active low
#define LP_POUTPA	0x20 	// out of paper
#define LP_PSELECD	0x10
#define LP_PERRORP	0x08 	// general printer error, active low

//* defines for 8255 control port (base + 2)
#define LP_PINTEN	0x10	// printer enabled
#define LP_PSELECP	0x08
#define LP_PINITP	0x04  // printer reset, active low
#define LP_PAUTOLF	0x02
#define LP_PSTROBE	0x01	// strobe bit

typedef struct LPTInfos
{
	ushort ioport;
	ushort irq;
	byte   flags;
} LPTInfos;

static LPTInfos LPTPorts[]=
{
	{ 0x3BC, 0, 0},
	{ 0x378, 0, 0},
	{ 0x278, 0, 0}
};

static int LPTProbePort (LPTInfos *LPT)
{
	int Counter = 0;

	outw (LPT->ioport, 0);

	// TODO: the kernel needs a microdelay function
	while(Counter ++ < 50)
		inportb (0x80);

	if(inportb(LPT->ioport) == 0)
	{
		LPT->flags = LPT_EXISTS;
		return 0;
	}

	return -1;
}

static void ResetLPT	(LPTInfos *LPT)
{
	int Counter;

	outportb(LPT->ioport +2, 0);
	for (Counter = 0 ; Counter < 10000 ; Counter++)
		; // TODO: the kernel needs a microdelay function

	outportb(LPT->ioport +2, LP_PSELECP | LP_PINITP);
}

static int WritePolledCharToLPT (LPTInfos *LPT, char c)
{
 	LPT = LPT;
	c   = c;

	// TODO: send the character to the printer

	return (0);
}

static int WritePolledLPT (int Nr, const char *s, int Count)
{
	LPTInfos *LPT	 =	LPTPorts + Nr;
 	int       i, status;

	s     = s;

	for (i = 0; i < Count;)
	{
	 	status = WritePolledCharToLPT (LPT, s[i]);

	 	if (status == 0)
		  i++;
		else
		{
		 	// TODO: print error message and retry
		 	;
		}
	}

	return (i);
}

///////////////////////////////////////////////////////////////////////////
// public functions

void InitLPT(void)
{	
	LPTInfos *LPT = LPTPorts;
	int i;
	printk("\eRPrinter Port(s): ");
	for(i=0; i < (sizeof (LPTPorts) / sizeof (*LPTPorts)); i++)
	{
	 	// is this interface available ?
		if(LPTProbePort(LPT)==0)
		{
			// yes, the reset it to come in a known state
			ResetLPT (LPT);

			// and print a message
			printk ("\eNLPT%d at 0x%x, using polling driver\r\n", i, LPT->ioport);
		}
		LPT++;
	}
}

int OpenLPT (int Nr)
{
	if (Nr < 0 || Nr > (sizeof (LPTPorts) / sizeof (*LPTPorts)))
	   return (0);

   if (! (LPTPorts[Nr].flags & LPT_EXISTS))
	  	return (0);

   if (LPTPorts[Nr].flags & LPT_OPEN)
	  	return (0);

	LPTPorts[Nr].flags |= LPT_OPEN;
  	return (1);
}

int CloseLPT (int Nr)
{
	if (Nr < 0 || Nr > (sizeof (LPTPorts) / sizeof (*LPTPorts)))
	   return (0);

   if (! (LPTPorts[Nr].flags & LPT_EXISTS))
	  	return (0);

   if (! (LPTPorts[Nr].flags & LPT_OPEN))
	  	return (0);

	LPTPorts[Nr].flags &= ~LPT_OPEN;
  	return (1);
}

int WriteLPT (int Nr, const char *s, int Count)
{
	if (Nr < 0 || Nr > (sizeof (LPTPorts) / sizeof (*LPTPorts)))
	   return (-1);

   if (! (LPTPorts[Nr].flags & LPT_EXISTS))
	  	return (-1);

   if (! (LPTPorts[Nr].flags & LPT_OPEN))
	  	return (-1);

   if (Count <= 0)
	  return (Count);

 	return (WritePolledLPT(Nr, s, Count));
}

