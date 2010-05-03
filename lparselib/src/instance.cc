// instance.cc -- Implementation of instances
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
#include "global.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#ifndef ERROR_H
#include "error.h"
#endif
#ifndef INSTANCE_H
#include "instance.h"
#endif
#ifndef DEBUG_H
#include "debug.h"
#endif
#ifndef GLOBAL_H
#include "global.h"
#endif
#ifndef SYMBOL_H
#include "symbol.h"
#endif
#ifndef PREDICATE_H
#include "predicate.h"
#endif

#define MAX_SIZE (1 << max_size)
#define NEW_SIZE (1 << new_size)
#define NORMAL_MASK  ((1 << max_size) -1UL)
#define REHASH_MASK  ((1 << new_size) -1UL)
#define FILL_LEVEL	2

/* returns true if there are some non-alphanumeric characters in the
   constant or if it starts with a number. */
int must_quote(char *st)
{
  if (!sys_data.drop_quotes) {
    return 0;
  }
  int nesting_depth = 0;
  int inside_quotes = 0;

  if (isdigit(*st))
    return 1;
  
  for (char *p = st; *p; p++) {
    if (*p == '\"')
      inside_quotes = !inside_quotes;
    else if (inside_quotes)
      continue;
    else if (*p == '(')
      nesting_depth++;
    else if (*p == ')')
      nesting_depth--;
    else if ((nesting_depth > 0) && (*p == ',' || *p == '-'))
      //don't quote commas within parenthesis.   
      continue;
    else if (!isalnum(*p) && *p != '_' && *p != '\'')
      return 1;
  }
  return 0;
}


const char *get_ground_instance(Instance *it, long pred, int ar)
{
  ostringstream ss;
  static string str;
  //  static 
  int i;
  long pos = 0;
  if ((predicates[pred]->Status() == DM_INTERNAL) ||
      (predicates[pred]->Status() == DM_SPECIAL)) {
    output(ss, "_%s", predicates[pred]->Name());
  } else {  
    if (!(predicates[pred]->hidden) || sys_data.emit_text) {
      output(ss, "%s", predicates[pred]->Name());
    } else {
      output(ss, "___%s",
		     predicates[pred]->Name());
    }
  }
  if (ar > 0) {
    if (it[0] == ERROR_NUMBER) {
      pos += output(ss, "(_");
    } else if (IS_NUMBER(it[0])) {
      pos += output(ss, "(%ld", GET_VALUE(it[0]));
    } else {
      if (must_quote(constant_table->symbols[GET_CONSTANT(it[0])]))
	pos += output(ss, "(\"%s\"",
		       constant_table->symbols[GET_CONSTANT(it[0])]);
      else
	pos += output(ss, "(%s",
		       constant_table->symbols[GET_CONSTANT(it[0])]);
    }
    for ( i = 1; i < ar; i++)
      if (it[i] == ERROR_NUMBER) {
	pos += output(ss, ",_");
      } else if (IS_NUMBER(it[i])) {
	pos += output(ss, ",%ld", GET_VALUE(it[i]));
      } else if (must_quote(constant_table->symbols[GET_CONSTANT(it[i])])) {
	pos += output(ss, ",\"%s\"",
		       constant_table->symbols[GET_CONSTANT(it[i])]);
      } else {
	pos += output(ss, ",%s",
		       constant_table->symbols[GET_CONSTANT(it[i])]);
      }
    pos += output(ss, ")");
  }
  str = ss.str();
  return str.c_str();
}
  

static long print_text_instance(Instance *it, long pred, int ar) 
{
  const char *buf = NULL;
  long atom = -1;

  buf = get_ground_instance(it, pred, ar);
  if (!buf)
    int_error("NULL ground instance", "");

  //insert the atom to the table
  atom = atom_table->Insert(buf);
  
  output(sys_data.output_file, buf);
  return atom;
}

static long print_smodels_instance(Instance *it, long pred, int ar)
{
  const char *buf = NULL;
  long atom = -1;
  
  buf = get_ground_instance(it, pred, ar);

  atom = atom_table->Insert(buf);
  
  if (atom < 0)
    int_error("cannot create ground atom '%s'", buf);

  output(sys_data.output_file, "%ld ", atom+1);
  return atom;
}

long print_instance(Instance *it, long pred, int ar)
{
  if (sys_data.emit_text)
    return print_text_instance(it, pred, ar);
  else
    return print_smodels_instance(it, pred, ar);
}



const char *get_priority_test_program_instance(char *st)
{
  static string str;
  ostringstream ss;
  ss.str("");
  output(ss, "+om(%s)", st);

  str = ss.str();
  return str.c_str();
}

inline int equal_item(Instance *i1, Instance *i2, int arity)
{
  int i;
  for (i = 0; i < arity; i++)
    if (i1[i] != i2[i])
      return 0;
  return 1;
}





HashSet::HashSet(long sz, int dim)
{
  long i = 0;

  if (sys_data.force_small_domains) {
    max_size = SMALL_DOMAIN_SIZE;
  } else {
    max_size = sz;
  }

  size = 0;
  arity = dim;
  if (arity <= 0)
    max_arity = 1;
  else
    max_arity = arity;

  tmp_items = NULL;
  new_size = 0;
  items = NULL;
  valid = NULL;
#if DEBUG & DBG_INSTANCE
  conflicts = 0;
  insert = 0;
  no_success = 0;
  total_tries = 0;
#endif
  debug(DBG_INSTANCE, 1, "Creating HashSet");
  items = new Instance*[MAX_SIZE];

  if (!items)
    error(SYS_ERR,"malloc error");

  for (i = 0; i < MAX_SIZE; i++) {
    items[i] = NULL;
  }
  storage = new Instance[(MAX_SIZE / FILL_LEVEL)*max_arity];
  //  hashed_items = new long[(MAX_SIZE / FILL_LEVEL)];
  hashed_items = NULL;
  if (!storage) // || !hashed_items)
    error(SYS_ERR,"malloc error");
  position = 0;

}
  
HashSet::~HashSet()
{
  debug(DBG_INSTANCE, 1, "Deleting HashSet");

  delete  [] items; 
  delete [] valid;
  if (storage)
    delete [] storage;
  if (hashed_items)
    delete [] hashed_items;
  
}




void HashSet::Print()
{
  long i = 0, j = 0;

  for (i = 0; i < MAX_SIZE; i++) {
    if(items[i]) {
      cout << i << " ";
      for (j = 0; j < arity; j++) {
        cout << items[i][j] << " ";
      }
      cout << endl;
    }
  }
}


void HashSet::Clear(int new_ar)
{
  debug(DBG_INSTANCE, 1, "Clearing HashSet");
  debug(DBG_INSTANCE, 4, "\tArity: %d -> %d", arity, new_ar);
  long i;

  for (i = 0; i < MAX_SIZE; i++) {
    delete [] items[i];
    items[i] = NULL;
  }
  max_arity = new_ar;
  arity = new_ar;
  size = 0;
}

/* the hash algorithm is from
   http://ourworld.compuserve.com/homepages/bob_jenkins/evahash.htm */


/* The mixing step */
#define mix(a,b,c) \
{ \
  a=a-b;  a=a-c;  a=a^(c>>13); \
  b=b-c;  b=b-a;  b=b^(a<<8);  \
  c=c-a;  c=c-b;  c=c^(b>>13); \
  a=a-b;  a=a-c;  a=a^(c>>12); \
  b=b-c;  b=b-a;  b=b^(a<<16); \
  c=c-a;  c=c-b;  c=c^(b>>5);  \
  a=a-b;  a=a-c;  a=a^(c>>3);  \
  b=b-c;  b=b-a;  b=b^(a<<10); \
  c=c-a;  c=c-b;  c=c^(b>>15); \
}


unsigned long hash(const char *key, unsigned long length,
		   unsigned long initval)
{
  u4 a, b, c; /* internal state */
  u4 len; /* how much work to do */

  /* Set up the internal state */
  len = length;
  a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
  c = initval;         /* variable initialization of internal state */

  /*---------------------------------------- handle most of the key */
  while (len >= 12) {
    a=a+(key[0]+((u4)key[1]<<8)+((u4)key[2]<<16) +((u4)key[3]<<24));
    b=b+(key[4]+((u4)key[5]<<8)+((u4)key[6]<<16) +((u4)key[7]<<24));
    c=c+(key[8]+((u4)key[9]<<8)+((u4)key[10]<<16)+((u4)key[11]<<24));
    mix(a,b,c);
    key = key+12; len = len-12;
  } 

  /*------------------------------------- handle the last 11 bytes */
  c = c+length;
  switch(len)              /* all the case statements fall through */
    {
    case 11: c=c+((u4)key[10]<<24);
    case 10: c=c+((u4)key[9]<<16);
    case 9 : c=c+((u4)key[8]<<8);
      /* the first byte of c is reserved for the length */
    case 8 : b=b+((u4)key[7]<<24);
    case 7 : b=b+((u4)key[6]<<16);
    case 6 : b=b+((u4)key[5]<<8);
    case 5 : b=b+key[4];
    case 4 : a=a+((u4)key[3]<<24);
    case 3 : a=a+((u4)key[2]<<16);
    case 2 : a=a+((u4)key[1]<<8);
    case 1 : a=a+key[0];
      /* case 0: nothing left to add */
    }
  mix(a,b,c);
  /*-------------------------------------------- report the result */
  return c;
}

#define SEED_1	0xAAAAAAAA
#define SEED_2	0xF0F0F0F0


unsigned long HashSet::FindIndex(Instance *key, unsigned long mask)
{
  long index = 0,ind1 = 0, ind2 = 0, tries = 15;
  Instance** table = NULL;

  if (mask == REHASH_MASK)
    table = tmp_items;
  else
    table = items;
  
  ind1 = hash((char *)key, arity*4, SEED_1) % mask;
  if (table[ind1] && !equal_item(key, table[ind1], arity))
    ind2 = (hash((char *)key, arity*4, SEED_2) % mask) + 1;
  else
    return ind1;

  index = ind1;
  do {
    tries--;
    index =  (index + ind2) % mask;
  } while (table[index] && !equal_item(key, table[index], arity) && tries);

#if DEBUG & DBG_INSTANCE
  total_tries += (15 - tries);
#endif
  
  if (!tries) { //switch to linear probing
#if DEBUG & DBG_INSTANCE
    if (insert)
      no_success++;
#endif
    do {
#if DEBUG & DBG_INSTANCE
      i++;
#endif
      index = (index+1) % mask;
    } while (table[index] && !equal_item(key, table[index], arity));
  }
#if DEBUG & DBG_INSTANCE
  if (insert)
    conflicts += i + (15 - tries);
#endif
  return index;
}


long HashSet::Insert(Instance *key)
{
  long index = 0, i = 0;
  Instance *new_node = NULL;
  //  debug(DBG_INSTANCE, 2, "Inserting value (%s, %d)", key, arity);

#if DEBUG & DBG_INSTANCE
  insert = 1;
#endif
  index = FindIndex(key, NORMAL_MASK);

#if DEBUG & DBG_INSTANCE
  insert = 0;
#endif

  assert(index < MAX_SIZE);
  // Is it in set already?
  if (items[index]) {
    return index;
  }
  
  new_node = &storage[arity*position];
  //  hashed_items[position] = index;
  position++;
  
  if (!new_node)
    error(SYS_ERR,"malloc error");
  
  for (i = 0; i < arity; i++)
    new_node[i] = key[i];
  
  items[index] = new_node;
  size++;
  if ( size > MAX_SIZE/FILL_LEVEL-1) {
    Rehash();
    // calculate new index
    index = FindIndex(key, NORMAL_MASK);
  }
  return index;
}

long HashSet::Lookup(Instance *key)
{
  long index = FindIndex(key,NORMAL_MASK);
  
  assert( index < MAX_SIZE);
  if (items[index])
    return index;
  else
    return -1;
}

void HashSet::Rehash()
{
  long i = 0, index = 0;
  Instance *new_storage = NULL;
  //  long *new_hashed_items = NULL;
  // debug(DBG_SYMBOL, 3, "Rehashing HashSet:\n\tOld max_size: %d\n\tNew"
  //    "max_size: %d\n", primes[max_size], primes[new_max_size]);
  
  new_size = max_size+1;
  tmp_items = new Instance *[NEW_SIZE];

  if (!tmp_items)
    error(SYS_ERR,"malloc error");

  for (i = 0; i < NEW_SIZE; i++) {
    tmp_items[i] = NULL;
  }

  new_storage = new Instance[(NEW_SIZE/FILL_LEVEL) * max_arity];
  //  new_hashed_items = new long[NEW_SIZE/FILL_LEVEL];
  memcpy(new_storage, storage, (MAX_SIZE/FILL_LEVEL)*arity*sizeof(Instance));
  //  memset(new_hashed_items, 0, (MAX_SIZE/FILL_LEVEL)*sizeof(long));
  
  for (i = 0; i < MAX_SIZE/FILL_LEVEL; i++) {
    index = FindIndex(&storage[i*arity], REHASH_MASK);
    assert( index < NEW_SIZE);
    //    new_hashed_items[i] = index;
    tmp_items[index] = &new_storage[arity*i];
  }
  max_size = new_size;
  // debug(DBG_INSTANCE, 4, "Rehash: Deleting old nodes");
  delete [] items;
  delete [] storage;
  //  delete [] hashed_items;
  items = tmp_items;
  storage = new_storage;
  //  hashed_items = new_hashed_items;
}



RBTree::RBTree(int ar, int in)
{
  head = x =  new TreeNode;
  arity = ar;
  index = in;
  if (!head)
    error(SYS_ERR,"malloc error");
  /*   z = new TreeNode;
  z->left = z->right = z->next = z;
  z->red = 0;
  z->item = new Instance[ar];
  z->item[index] = 0; */
  head->right = head->left = head->next = NULL;
  head->item = new Instance[ar];
  head->red = 0;
  if (!head->item)
    error(SYS_ERR,"malloc error");
  for (int i = 0; i < arity; i++)
    head->item[i] = ERROR_NUMBER;

  p = g = gg = current = NULL;
}



RBTree::~RBTree()
{
  delete [] head->item;
  delete head;
}

TreeNode::~TreeNode()
{
  if (left)
    delete left;
  if (right)
    delete right;
  if (next)
    delete next;
}

int RBTree::Lookup(Instance *item)
{
  Instance key = item[index];
  TreeNode *tmp = head;

  while (tmp != NULL) {
    if (tmp->item[index] < key)
      tmp = tmp->right;
    else if (tmp->item[index] > key)
      tmp = tmp->left;
    else if (equal_item(item, tmp->item, arity))
      return 1;
    else // kaikki samanarvoiset talla tasolla
      tmp = tmp->next;
  }
  return 0;
}


void RBTree::Insert(Instance *item)
{
  TreeNode *new_node;
  Instance v = item[index];
  x = head, p = head, g = head, gg = head;
  
  while (x != NULL) {
    gg = g; g = p; p = x;

    if (v < x->item[index])
      x = x->left;
    else if (v > x->item[index])
      x = x->right;
    else { // loytyi samanarvoinen, lisataan siihen
      new_node = new TreeNode;
      new_node->item = item;
      new_node->red = 0;
      new_node->left = new_node->right = NULL;
      if (x != NULL) {
	new_node->next = x->next;
	x->next = new_node;
      }
      return;
    }
    // onko kyseessa 4-node
    if (x && x->left && x->left->red && x->right && x->right->red)
      Split(v);
  }
  x = new TreeNode;
  x->item = item;
  x->red = 0;
  x->left = x->right = x->next = NULL;

  if (v < p->item[index])
    p->left = x;
  else
    p->right = x;
  Split(v);
  return;
}


void RBTree::Split(Instance v)
{
  x->red = 1;
  if (x->left)
    x->left->red = 0;
  if (x->right)
    x->right->red = 0;
  if (p->red) {
    g->red = 1;
    if ((v < g->item[index]) != (v < p->item[index]))
      p = Rotate(v,g);
    x = Rotate(v,gg);
    x->red = 0;
  }
  head->right->red = 0;
}

TreeNode *RBTree::Rotate(Instance v, TreeNode *y)
{
  TreeNode *c, *gc;
  c = (v < y->item[index]) ? y->left : y->right;
  if (v < c->item[index]) {
    gc = c->left;
    c->left = gc->right;
    gc->right = c;
  } else {
    gc = c->right;
    c->right = gc->left;
    gc->left = c;
  }
  if ( v < y->item[index])
    y->left = gc;
  else
    y->right = gc;
  return gc;
}

#define IND(x) {for(i=0;i < x;i++) cout << " "; }

void RBTree::Print(TreeNode *tn, int id)
{
  int i;
  if (!tn)
    return;
  IND(id);
  output(cout,"Color: %s\n", (tn->red ? "red" : "black"));
  IND(id);
  output(cout,"Key: %ld\n", tn->item[index]);
  IND(id);
  output(cout,"Left:\n");
  Print(tn->left, id +2);
  IND(id);
  output(cout,"Right:\n");
  Print(tn->right, id +2);
  IND(id);
  output(cout,"Next:\n");
  Print(tn->next, id +2);
}

long DefineSystemVariable()
{
  ostringstream ss;
  // const int pos = 2;
  int val = 0;
  output(ss, "I_");

  //  sys_data.total_vars++;
  val = output(ss, "%ld", sys_data.internal_variables++);
  return variable_table->Insert(ss.str().c_str());
}
  
int internal_variable(Variable v)
{
  if ((variable_table->symbols[v][0] == 'I') &&
      (variable_table->symbols[v][1] == '_'))
    return 1;
  else
    return 0;
}
