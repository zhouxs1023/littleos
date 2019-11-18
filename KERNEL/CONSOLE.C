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
 * console.c - console (screen and keyboard) handling
 */

/*
 * There are some special codes for printing:
 * \eXX   	sets color to XX
 * \eN or \en 	sets color to white (7)
 * \eR or \er	sets color to red (12 or 4)
 * \eB or \eb	sets color to blue (9 or 1)
 * \eG or \eg	sets color to green (10 or 2)
 */

/* keyboard is already not done */

#include <kernel.h>
#include <stdarg.h>
#include <gdt.h>
#include <string.h>	/* for memcpy */

static ushort *video; /* video memory */
static byte attr = 7;		/* attribute of the text */
long pos;

int c_x=0, c_y=0;

#define CRTC_COMMAND	0x3D4
#define CRTC_DATA	0x3D5
#define CRTC_CURLO	0x0F
#define CRTC_CURHI	0x0E

void clrscr()
{
	int i;
        for (i = 0; i < NR_LINES*NR_COLUMNS; i++)
        	video[i] = ' ' + (attr << 8);
}

void gotoxy(int x, int y)
{
        c_x = x;
        c_y = y;

        pos = c_y * NR_COLUMNS + c_x;

        outb(CRTC_COMMAND, CRTC_CURLO);
        outb(CRTC_DATA, (unsigned char) pos);

        outb(CRTC_COMMAND, CRTC_CURHI);
        pos >>= 8;
        outb(CRTC_DATA, (unsigned char) pos);
}

int sound_delay = 0;

void sound(int freq)
{
	if (freq>0) {	/* activate speaker */
                outb(0x42, (unsigned char) freq);
                outb(0x42, (unsigned char) (freq >> 8));
                outb(0x61, inb(0x61) | 3);
        }
        else
        {	/* stop speaker */
        	outb(0x61, inb(0x61) & !3);
        }
}

void beep(void)
{
        sound_delay = 10;
        sound(500);
}

void puts(const char *s)
{
        int c, color;
        ulong pos;
        ulong flags = get_flags();	/* save flags and disable int */
        cli();

        while ((c=(unsigned char)(*s++)) !=0) {
                switch (c) {
                	case '\e':
                        	c = *s++;
                                switch (c) {
                                	case 'N':
                                        case 'n':
                                        	attr = 7;
                                                break;
                                        case 'r': attr = 4; break;
                                        case 'R': attr = 4 + 8; break;
                                        case 'b': attr = 1; break;
                                        case 'B': attr = 1 + 8; break;
                                        case 'g': attr = 2; break;
                                        case 'G': attr = 2 + 8; break;
                                        default:
	                                	color = (c - '0')*10;
        	                                c = *s++;
                	                        color += c - '0';
                        	                if (color>0 && color < 16) attr = color;
                                                break;
                                }
                                break;
                	case 7: /* beep */
                                sound_delay = 10;
                                sound(1000);
                                break;
                	case 10:
                        case 11:
                        case 12:
                                c_y++;
                                c_x = 0;
                                break;

			case 13:
                        	c_x = 0;
                                break;

                        case 8: /* \b */
                        	if (c_x) c_x--;
                                break;

			case 9: /* \t */
                                c = 8 - (c_x&7);
                                c_x += c;
                                if (c_x >= NR_COLUMNS) {
                                	c_x = 0;
                                        c_y++;
                                }
                                c = 9;
                                break;

                        default:
                        	video[ c_x + c_y*NR_COLUMNS ] = c + (attr << 8);
                                c_x++;
                                if (c_x >= NR_COLUMNS) {
                                	c_x = 0;
                                        c_y++;
                                }
                                break;
                } /* end of switch */
                if (c_y >= NR_LINES) {
                	memcpy( (void*)VIDEO_ADDR, (void*)(VIDEO_ADDR+160), NR_COLUMNS*NR_LINES*2);
                        for (c = 0; c < 80; c++)
                        	video[NR_COLUMNS*(NR_LINES-1) + c] = ' ' + (attr << 8);
                        c_y = NR_LINES-1;
                }

        }

        /* update cursor */
        pos = c_y * NR_COLUMNS + c_x;

        outb(CRTC_COMMAND, CRTC_CURLO);
        outb(CRTC_DATA, (unsigned char) pos);

        outb(CRTC_COMMAND, CRTC_CURHI);
        pos >>= 8;
        outb(CRTC_DATA, (unsigned char) pos);


        /* restore flags */
        set_flags(flags);
}

static char buf[512];

int printk(const char *fmt, ...)
{
	va_list args;
        int i;

        va_start(args, fmt);
        i = vsprintf(buf, fmt, args);
        va_end(args);

        puts(buf);
        return i;
}

#include "logo.h"

void init_console(void)
{
	static char *vid;
	int i;

        /* we have to init this first */
        video = (ushort *) VIDEO_ADDR;

	clrscr();
        i = 0;
        vid = (char *) VIDEO_ADDR;

        while (IMAGEDATA[i]!=0) vid[i+160] = IMAGEDATA[i++];

        printk("\n\n\n\n\n\n\n\n");
        alloc_io(IO_VGA, IO_VGA + IO_VGASIZE, "vga");
}

