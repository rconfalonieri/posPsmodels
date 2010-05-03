// library.h -- external library definition for lparse
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

#ifndef LIBRARY_H
#define LIBRARY_H

#include "config.h"

#ifndef DEFINES_H
#include "defines.h"
#endif

#ifdef MS_WINDOWS
#include <windows.h>
#endif

struct LibraryNode
{
  LibraryNode(char *st) { handle = NULL; lib = st; loaded = 0; }
  ~LibraryNode(); 
  InstFunc FindSymbol(char *st);
#ifdef MS_WINDOWS
  HINSTANCE handle;
#else
  void *handle; // pointer to the loaded library
#endif
  char *lib; // library name
  int loaded; // is it loaded 
};


int register_functions();

#endif
