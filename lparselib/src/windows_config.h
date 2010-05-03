
/* windows_config.h -- a lparse configuration file for Windows 
** Copyright (C) 1999-2000 Tommi Syrjänen <Tommi.Syrjanen@hut.fi>
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**  
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**  
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**  
*/

/* On Windows systems this file should be placed in lparse main
   distribution directory under the name `config.h' */

#define PACKAGE "lparse"
#define VERSION "1.0.18"

#define MS_WINDOWS
#define HAVE_VPRINTF 1
#define STDC_HEADERS 1
#define YYTEXT_POINTER 1
#define SIZEOF_INT 4
#define SIZEOF_LONG 4
#define HAVE_STRTOL 1
/* onko tämä oikein? */
#define HAVE_STRDUP 1

#define HAVE_LIMITS_H 1

#ifdef CXX_HAS_BUGGY_FOR_LOOPS
/* #undef for */
#define for if(1) for
#endif

#ifdef CXX_HAS_NO_BOOL
#define bool int
#define true 1
#define false 0
#endif
   
#include <assert.h>

