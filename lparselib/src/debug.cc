// debug.cc -- debugging and error macros
// Copyright (C) 1999-2007 Tommi Syrjänen <Tommi.Syrjanen@hut.fi>
//  
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//  
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//  
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//  

#include "config.h"
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>

#ifndef DEBUG_H
#include "debug.h"
#endif
#ifndef GLOBAL_H
#include "global.h"
#endif
#ifndef ERROR_H
#include "error.h"
#endif


#if DEBUG > DBG_NONE
void debug(int type, int level, char *fmt, ...)
{
  if ((DEBUG & type) &&
      (DEBUG_LEVEL >= level)) {
    va_list(ap);
    va_start(ap, fmt);
    output(cerr, fmt, ap);
    output(cerr, "\n");
    va_end(ap);
  }
}
#else
void debug(int, int, char*, ...)
{
}
#endif

  
