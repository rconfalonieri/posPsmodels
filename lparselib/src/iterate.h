// iterate.h -- definitions of iterator classes
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

#ifndef ITERATE_H
#define ITERATE_H

#include "config.h"

#ifndef GLOBAL_H
#include "global.h"
#endif

#ifndef INSTANCE_H
#include "instance.h"
#endif


class Iterator
{
public:
  virtual ~Iterator() { }
  virtual Instance *Iterate(Instance) = 0;
  virtual void Clear() = 0;
};
  

// HashSetIterator 
class HashSetIterator : public Iterator
{
public:
  HashSetIterator(HashSet *hs);
  virtual ~HashSetIterator() ;

  virtual void Reset(HashSet *);
  virtual void Clear();
  virtual Instance *Iterate(Instance it = -1);
private:
  long current, returned;
  HashSet *set;
};

// An iterator for RBTrees. Constructor takes as its only argument the
// tree to iterate. The Iterate(Instance value) returns all items with
// index value 'value' from tree one by one and NULL when they run
// out.
class RBTree;
class TreeNode;
class RBTreeIterator : public Iterator
{
public:
  RBTreeIterator(RBTree *);
  virtual ~RBTreeIterator();

  virtual void Reset(RBTree *);
  virtual  void Clear();
  virtual Instance *Iterate(Instance value);
private:
  TreeNode *current;
  RBTree *tree;
};


#endif
