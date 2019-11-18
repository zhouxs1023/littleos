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

#ifndef _STDARG_H_
#define _STDARG_H_

typedef char *va_list;

#define __va_rounded_size(TYPE)  \
  (((sizeof (TYPE) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))

#define va_start(VAR, LASTARG) 						\
 (VAR = ((char *) &(LASTARG) + __va_rounded_size (LASTARG)))

#define va_end(x)

#define va_arg(VAR, TYPE)						\
 (VAR += __va_rounded_size (TYPE),					\
  *((TYPE *) (VAR - __va_rounded_size (TYPE))))

#endif /* _STDARG_H_ */
