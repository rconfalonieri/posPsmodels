// instance.h -- data structures for instances for lparse
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

#ifndef INSTANCE_H
#define INSTANCE_H

#include "config.h"
#ifndef DEBUG_H
#include "debug.h"
#endif
#ifndef GLOBAL_H
#include "global.h"
#endif

#include "iterate.h"
// prints ground instance 'it' of arity 'ar' and predicate 'pred'
long print_instance(Instance *it, long pred, int ar);
const char *get_ground_instance(Instance *it, long pred, int ar);
long DefineSystemVariable();
int internal_variable(Variable v);

const char *get_priority_test_program_instance(char *sr);

/* HashSet implements a storage of ground instances as a closed 
   automatically resized hash table.
   Attributes:
   	Instance *items	-- the actual table
	int max_size	-- index to primes array, like with
			   SymbolTable.
        long size	-- number of instances in table			   
        char *valid	-- valid[i] is 1 if there is a valid instance
			   at items[i], 0 otherwise
        long base	-- the base for hash value calculation
	Instance *tmp_items 	-- used while rehashing
	char *new_valid		-- ditto
	int arity	-- arity of the instance
	int max_arity	-- maximum arity used with this set

   Methods
   	HashSet(long sz, int dim, long b) -- constructs a hash set
				with a initial size of 'sz', arity of
				'dim' and base of 'b'. 
	long Insert(Instance *it) -- inserts it to set and returns its
				     index. Removes duplicates
        long Lookup(Instance *it) -- returns 1 if there is instance
       				     'it' in the table, 0 otherwise
	void Clear(int a, long b) -- clears all items from HashSet and
				     sets new arity of 'a' and new
				     base 'b'.
	void SetBase(long bs)	  -- sets new base and rehashes
	long Hash(Instance *it)   -- calculates hash value. The
				     instance is interpreted as a
				     number of base 'base' mod size
				     with quadratic probing to resolve
				     duplicate hash values.
*/
class HashSet
{
public:
  HashSet(long sz, int dim);
  ~HashSet();

  long Insert(Instance *);
  long Lookup(Instance *);

  Instance *Iterate();
  long Size() { return size; }
  void Clear(int);

  friend class HashSetIterator;
  void Print();  
private:
  unsigned long Hash(char *key, unsigned long, unsigned long);
  void Rehash();
  unsigned long FindIndex(Instance *key, unsigned long);
  
  Instance **items, **tmp_items;
  Instance *storage;
  long position;
  long *hashed_items;
  int max_size, new_size;
#if DEBUG & DBG_INSTANCE
  long conflicts;
  int insert;
  long no_success;
  long total_tries;
#endif
  long size;
  char *valid, *new_valid;
  long base;
  int arity, max_arity;
};





struct TreeNode
{
  char red;
  Instance *item;
  struct TreeNode *left, *right, *next;
  ~TreeNode();
};

/* RBTree is a Reb-Black tree implementation that works as an index to
   InstanceSets. The instances stored are ordered by values of values
   of place 'index'. Since finding all duplicate indices efficiently
   is essential to iteration through all values the duplicates are
   stored in a linked list starting at a node. The implementation is
   quite straight from Sedgewick

   Attributes:
	TreeNode *head		-- root of the tree
	int arity		-- arity of the instances stored
	int index		-- the place of index

	The rest TreeNodes are used for RB-tree algorithms.
   
   Methods:
	RBTree(int ar, int in)	-- create a RB-tree with arity 'ar'
				   and index 'in'
   	void Insert(Instance *it)-- insert an item to tree discarding
				    duplicates
	int Lookup(Instance *it)-- check if 'it' is in tree
   */
class RBTree  
{
public:
  RBTree(int ar, int in);
  ~RBTree();
  void Insert( Instance *item);
  int Lookup( Instance*);
  friend class RBTreeIterator;
private:
  void Split(Instance);
  void Print(TreeNode *nd, int id = 0);
  TreeNode *Rotate(Instance, TreeNode*);
  TreeNode *x, *head, *p, *g, *gg, *current;
  int arity;
  int index;
};


// Choose the index and set representation
typedef RBTree Index;
typedef HashSet InstanceSet;
typedef RBTreeIterator IndexIterator;
typedef HashSetIterator InstanceIterator;

#endif
