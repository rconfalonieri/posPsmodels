// array.h -- definitions of flexible arrays for lparse
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

#ifndef ARRAY_H
#define ARRAY_H

#include "config.h"
#ifndef GLOBAL_H
#include "global.h"
#endif

class ResizableArray
{
public:
  ResizableArray(long sz = 10);
  ~ResizableArray();

  long *operator[](long idx);
private:
  long *arr;
  long size;
};

class Predicate;

class PredicateArray 
{
public:
  PredicateArray(long sz = 10);
  ~PredicateArray();

  Predicate *operator[](long idx);
private:
  Predicate **arr;
  long size;
};

class GraphNode;

class GraphNodeArray 
{
public:
  GraphNodeArray();
  ~GraphNodeArray();

  GraphNode *operator[](long idx);

  void ClearEdges();
private:
  GraphNode **arr;
  long size;
};

class LongList;

class LongListArray
{
public:
  LongListArray(long sz = 10);
  ~LongListArray();

  LongList *operator[](long idx);
private:
  LongList **arr;
  long size;
};

#endif
