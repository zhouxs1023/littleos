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
 * fd.c - floppy disk handling
 */

#include <kernel.h>
#include <types.h>

static uchar version;
static uchar cmos_type;
static uchar type;

static uchar motor_mask = 0;

static uint cur_op = 0;
static uint cur_drv;
static uint cur_head;
static uint cur_trk;

#define FD_SEEK   0x02

/*
 * max no of floppies supported
 */
#define     NFD      (2)

#define     FD_BASEIO   0x3F0

#define     F_MASTER 	0x80
#define     F_DIR    	0x40
#define     F_CMDBUSY   0x10

#define     FD_DATA     0x05
#define     FD_STATUS   0x04

/* 
 * fdc commands 
 */
#define     FDC_VERSION    0x10
#define     FDC_INTS       0x08  /* SENSEI */
#define     FDC_RESET      0x04  /* SENSE */
#define     FDC_SEEK       0x0F

#define     FDC_VER_765A   0x80
#define     FDC_VER_765B   0x90

#define     FDTYPE_MASK 0x0F

#define     FDTYPE_NONE 0x00
#define     FDTYPE_360  0x01
#define     FDTYPE_1200 0x02
#define     FDTYPE_720  0x03
#define     FDTYPE_1440 0x04
#define     FDTYPE_2880 0x05

#define     ENABLE_INTS 0x0C

#define     FD_MOTOR 	0x02

#define     MOTOR_0     0x10
#define     MOTOR_1     0x20

#define     RTCSEL      0x70
#define     RTCDATA     0x71

#define     NVRAM_FD 	0x10


uchar
fd_in()
{
   uint c;
   int r;

   c = 1000000;

   do {
      r = (inb(FD_BASEIO+FD_STATUS) 
         & (F_MASTER | F_DIR | F_CMDBUSY));
      if( r == (F_MASTER | F_DIR | F_CMDBUSY ))
         break;
      c--;
   } while(c);
   
   if( c == 0 )
      return 0;

   return inb(FD_BASEIO+FD_DATA);
}

void
fd_out(byte val)
{
   uint c;
   int r;

   c = 1000000;

   do {
      r = (inb(FD_BASEIO+FD_STATUS) & (F_MASTER | F_DIR));
      if( r == F_MASTER)
         break;
      c--;
   } while(c);

   if( c == 0 ) {
      printk("fd_out: timeout\n");
      return;
   }
   
   outb(FD_BASEIO+FD_DATA, val);
}

void 
fd_reset()
{
   /*
   outb(0x03f2, 0x08);
   for(c = 0; c < 10000; c++);
   outb(0x03f2, 0x0C);
   */

   int c;

   cli();

   outb(FD_BASEIO+FD_MOTOR, 0);
   outb(FD_BASEIO+FD_MOTOR, ENABLE_INTS);

   sti();

   for(c = 0; c < 10000; c++);
}

void 
fd_start(int drive)
{
   motor_mask |= drive;
   if( drive == 0x00 ) {
      motor_mask |= MOTOR_0;
   } else {
      motor_mask |= MOTOR_1;
   }
   outb(FD_BASEIO+FD_MOTOR, motor_mask);
}

void 
fd_stop(int drive)
{
   if( drive == 0x00 ) {
      motor_mask &= ~MOTOR_0;
   } else {
      motor_mask &= ~MOTOR_1;
   }
   outb(FD_BASEIO+FD_MOTOR, motor_mask);
}

void
fd_seek()
{
   cur_op = FD_SEEK;
   cur_head = 0;
   cur_trk = 0;
   cur_drv = 0;

   fd_out(FDC_SEEK);
   fd_out((cur_head << 2) | cur_drv);
   fd_out(cur_trk);
}


void fd_init()
{
   int c;
   uint l;
   printk("\eRInitializing FDC: \eN");	
   fd_reset();

   fd_out(FDC_VERSION);
   version = fd_in();
   printk("Version %d found: ", version);

   if( version == FDC_VER_765A )
      printk("stardard FDC found\n");
   else
   if( version == FDC_VER_765B )
      printk("enhanced FDC found\n");
   else
   if( version == 0 )
      printk("timeout\n");
   else
      printk("unknown controller\n");

   for(c = 0; c < NFD; ++c) {
      outb(RTCSEL, NVRAM_FD);
      cmos_type = inb(RTCDATA);

      type = (cmos_type >> (4 * (NFD - 1 - c ))) & FDTYPE_MASK;

      switch(type) {
         case FDTYPE_1200:
            printk("\tfd_init -> %d : 1.2MB floppy drive found\n",c);
            break;
         case FDTYPE_1440:
            printk("\tfd_init -> %d : 1.44MB floppy drive found\n",c);
            break;
         default:
            printk("\tfd_init -> %d : no drive\n",c);
            break;
      }
   }

   motor_mask |= (FDC_INTS | FDC_RESET);

   fd_start(0);
   for(l = 0; l < 1000000; l++);
   fd_stop(0);

   /*
   printk("trying to seek...\n");
   fd_start(0);
   fd_seek();
   for(l = 0; l < 10000000; l++);
   fd_stop(0);
   */
}

void fd_isr(unsigned long irq)
{
}
