// iterate.cc -- implementation of iterator classes
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

#include <stdlib.h>

#include "config.h"
#ifndef  GLOBAL_H
#include "global.h"
#endif
#include "instance.h"
#ifndef ITERATE_H
#include "iterate.h"
#endif
#ifndef ERROR_H
#include "error.h"
#endif
HashSetIterator::HashSetIterator(HashSet *hs)
  : current(0), returned(0), set(hs)
{
}

HashSetIterator::~HashSetIterator()
{
}

void HashSetIterator::Reset(HashSet *hs)
{
  current = 0;
  returned = 0;
  set = hs;
}

void HashSetIterator::Clear()
{
  current = 0;
  returned = 0;
}
    

Instance *HashSetIterator::Iterate(Instance )
{
  if (!set)
    int_error("trying to iterate an empty iterator", "");
  
  Instance *result = NULL;
  if (current == set->position) {
    current = 0;
    returned = 0;
    return NULL;
  }
  if (returned == set->size) {
    returned = current = 0;
    return NULL;
  }
  
  if (current == set->position) {
    current = 0;
    returned = 0;
    return NULL;
  }
  returned++;
  result = &(set->storage[current*set->arity]);
  current++;
  return result;
} 


RBTreeIterator::RBTreeIterator(RBTree *tr)
{
  tree = tr;
  current = tr->head;
}

RBTreeIterator::~RBTreeIterator()
{
}

void RBTreeIterator::Reset(RBTree *tr)
{
  tree = tr;
  current = tr->head;
}

void RBTreeIterator::Clear()
{
  current = tree->head;
}
  
Instance *RBTreeIterator::Iterate(Instance value)
{
  TreeNode *tmp = NULL;
  
  if (current == NULL) {	
    current = tree->head->right;
    return NULL;
  }

  while (current != NULL) {
    if (value < current->item[tree->index])
      current = current->left;
    else if (value > current->item[tree->index])
      current = current->right;
    else {
      tmp = current;
      current = current->next;	
      return tmp->item;
    }
  }
  current = tree->head->right;
  return NULL;
}

