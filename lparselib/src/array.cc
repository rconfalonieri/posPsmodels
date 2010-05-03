// array.cc -- implementation of flexible arrays for lparse
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
#ifndef GLOBAL_H
#include "global.h"
#endif
#ifndef ARRAY_H
#include "array.h"
#endif
#ifndef DEBUG_H
#include "debug.h"
#endif
#ifndef ERROR_H
#include "error.h"
#endif
#ifndef PREDICATE_H
#include "predicate.h"
#endif
#include <string.h>

#include "list.h"

ResizableArray::ResizableArray(long sz)
  : arr(NULL)
{
  size = sz;
  arr = new long[size];

  if (!arr)
    error(SYS_ERR,"malloc error");

  memset(arr, 0, sizeof(long) * size);
}

ResizableArray::~ResizableArray()
{
  delete [] arr;
}


long *ResizableArray::operator[](long idx)
{
  long *tmp = NULL, ns = size, i;

  if (idx < size) 
    return &arr[idx];

  // resize the array.
  while (ns <= idx) 
    ns *= 2;

  tmp = new long[ns];
  if (!tmp)
    error(SYS_ERR,"malloc error");

  for (i= 0; i < size; i++) 
    tmp[i] = arr[i];

  while (i < ns) {
    tmp[i] = 0;
    i++;
  }

  size = ns;
  delete []arr;
  arr = tmp;

  return &arr[idx];
}
  
PredicateArray::PredicateArray(long sz)
{
  long i = 0;
  size = sz;
  arr = new Predicate*[size];

  if (!arr)
    error(SYS_ERR,"malloc error");

  for (i = 0; i < sz; i++)
    arr[i] = new Predicate;
}

PredicateArray::~PredicateArray()
{
  long i = 0;
  //  cerr << "Deleting " << size << " predicates" << endl;
  for (i=0; i < size; i++) {
    if (arr[i]) {
      //      cerr << "Deleting: " << i << " " << arr[i]->Name() << endl;
      delete arr[i];
    } else {
      //      cerr << "Not exists " << i << endl;
    }
  }
    
  delete [] arr;
}

Predicate *PredicateArray::operator[](long idx)
{
  long ns;
  long i;
  Predicate **tmp;

  ns = this->size;
  if (idx < this->size) 
    return this->arr[idx];

  while (ns <= idx) 
    ns *= 2;

  tmp = new Predicate*[ns];
  if (!tmp)
    error(SYS_ERR,"malloc error");

  for (i= 0; i < size; i++) 
    tmp[i] = arr[i];
  while (i < ns) {
    tmp[i] = new Predicate;
    i++;
  }
  
  size = ns;
  
  delete [] arr;
  arr = tmp;
  return this->arr[idx];
}





GraphNodeArray::GraphNodeArray()
{
  int i;
  size = 10;
  arr = new GraphNode*[size];

  if (!arr)
    error(SYS_ERR,"malloc error");

  for (i = 0; i < size; i++) 
    arr[i] = new GraphNode;
}

GraphNodeArray::~GraphNodeArray()
{
  int i;
  for (i = 0; i < size; i++) {
    delete arr[i];
  }
  delete [] arr;
}

void GraphNodeArray::ClearEdges()
{
  for (int i = 0; i < size; i++) {
    arr[i]->ClearEdges();
  }
}

GraphNode *GraphNodeArray::operator[](long idx)
{
  long ns;
  long i;
  GraphNode **tmp;

  ns = this->size;
  if (idx < this->size) 
    return this->arr[idx];
				
  while (ns <= idx) 
    ns *= 2;

  tmp = new GraphNode*[ns];
  if (!tmp)
    error(SYS_ERR,"malloc error");

  for (i= 0; i < size; i++) 
    tmp[i] = arr[i];
  while ( i < ns)
    tmp[i++] = new GraphNode;
  
  size = ns;

  delete [] arr;
  arr = tmp;

  
  return this->arr[idx];
}




PredicateArray predicates;


  
LongListArray::LongListArray(long sz)
{
  long i = 0;
  size = sz;
  arr = new LongList*[size];

  if (!arr)
    error(SYS_ERR,"malloc error");

  for (i = 0; i < sz; i++)
    arr[i] = new LongList;
}

LongListArray::~LongListArray()
{
  long i = 0;
  for (i=0; i < size; i++)
    delete arr[i];
    
  delete [] arr;
}

LongList *LongListArray::operator[](long idx)
{
  long ns;
  long i;
  LongList **tmp;

  ns = this->size;
  if (idx < this->size) 
    return this->arr[idx];

  while (ns <= idx) 
    ns *= 2;

  tmp = new LongList*[ns];
  if (!tmp)
    error(SYS_ERR,"malloc error");

  for (i= 0; i < size; i++) 
    tmp[i] = arr[i];
  while (i < ns) {
    tmp[i] = new LongList;
    i++;
  }
  
  size = ns;
  
  delete [] arr;
  arr = tmp;
  return this->arr[idx];
}

