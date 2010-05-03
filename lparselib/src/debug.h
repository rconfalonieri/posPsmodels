// debug.h -- debugging and error macros for lparse
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

#ifndef DEBUG_H
#define DEBUG_H

#include "config.h"

#define DBG_PARSE	(1 << 0)
#define DBG_SYMBOL	(1 << 1)
#define DBG_PARSETREE	(1 << 2)
#define DBG_MAIN	(1 << 3)
#define DBG_INSTANCE	(1 << 4)
#define DBG_TERM	(1 << 5)
#define DBG_GRAPH	(1 << 6)
#define DBG_LIST	(1 << 7)
#define DBG_GROUND	(1 << 8)
#define DBG_RESTRICT	(1 << 9)
#define DBG_NONE	0
#define DBG_ALL		~DBG_NONE

// Set DEBUG to bitwise or of wanted parts. And use parenthesis!!
#define DEBUG	       DBG_NONE

/* debug levels:
   1 -> most important notes (creating, destroying)
   2 -> 1st. level function calls
   3 -> 2nd. level function calls
   4 -> information internal to function
   5 -> minor and annoying data
   */
#define DEBUG_LEVEL 	1

void debug(int type, int level,char *,  ... );

#if 0
#ifdef DEBUG 
#define assert(x)   \
if (! (x)) {   \
  fprintf(stderr, "Assertion failed in %s at line %d.\n",\
	  __FILE__, __LINE__);\
  abort();\
}
#else
#define assert(x)
#endif
#endif

#endif
