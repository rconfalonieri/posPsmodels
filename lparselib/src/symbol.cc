//   symbol.cc -- implementation of symbol tables
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
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#ifndef LIBRARY_H
#include "library.h"
#endif
#ifndef DEBUG_H
#include "debug.h"
#endif
#ifndef SYMBOL_H
#include "symbol.h"
#endif
#ifndef TERM_H
#include "term.h"
#endif
#ifndef LIST_H
#include "list.h"
#endif
#ifndef ERROR_H
#include "error.h"
#endif

#define MAX_SIZE (1 << max_size)
#define NEW_SIZE (1 << new_size)
#define NORMAL_MASK  ((1 << max_size) - 1)
#define REHASH_MASK  ((1 << new_size) - 1)

void find_libraries();


SymbolNode::SymbolNode(const char *st)
{
  symbol = clone_string(st);
  value = 0;
}

SymbolNode::~SymbolNode()
{
  delete [] symbol;
}

SymbolTable::SymbolTable(long s, int con)
{
  debug(DBG_SYMBOL, 1, "Creating SymbolTable");

  if (sys_data.force_small_domains) {
    max_size = SMALL_DOMAIN_SIZE;
  } else {
    max_size = s;
  }
  new_size = 0;
  size = 0; 
  max_arity = -1;
  constants = con;

  debug(DBG_SYMBOL, 4, "\tAllocating items");
  items = new SymbolNode*[MAX_SIZE];
  symbols = new char*[MAX_SIZE];
  
  if (!items || !symbols)
    error(SYS_ERR, "malloc error");

  for (long i = 0; i < MAX_SIZE; i++) {
    items[i] = NULL;
    symbols[i] = NULL;
  }
#if DEBUG & DBG_SYMBOL
  conflicts = 0; 
#endif
}

SymbolTable::~SymbolTable()
{
  long i;
    
  debug(DBG_SYMBOL, 1, "destroying SymbolTable");
  debug(DBG_SYMBOL, 4, "\tdestroying items");
  for (i = 0; i < MAX_SIZE; i++) {
    if (symbols[i])
      delete [] symbols[i];
    if (items[i]) {
      delete items[i];
    }
  }
  
  debug(DBG_SYMBOL, 4, "\tdestroying array");
  delete [] items;
  delete [] symbols;
  
}


#define SEED_1	0xAAAAAAAA
#define SEED_2	0xF0F0F0F0


unsigned long SymbolTable::FindIndex(const char *key, long mask)
{
  long index = 0,ind1 = 0, ind2 = 0;
  long tries = 20;
  SymbolNode **table = NULL;

  if (mask == REHASH_MASK)
    table = tmp_nodes;
  else
    table = items;
  
  ind1 = hash(key, strlen(key), SEED_1) % mask;
  if (table[ind1] && strcmp(key, table[ind1]->symbol))
    ind2 = (hash(key, strlen(key), SEED_2) % mask) + 1;
  else 
    return ind1;

  index = ind1;
  do {
    tries--;
    index = (index + ind2) % mask;
  } while (table[index] && strcmp(key, table[index]->symbol) && tries);

  if (!tries) { //switch to linear probing
    do {
      index = (index+1) % mask;
    } while (table[index] && strcmp(key, table[index]->symbol));
  }
  
#if DEBUG & DBG_SYMBOL
  if (insert)
    conflicts += i;
#endif
  return index;
}

void SymbolTable::SetValue(char *key, long val, int)
{
  long index = 0;
  
  index = FindIndex(key, NORMAL_MASK);
  
  if (!items[index])
    int_error("cannot change unexisting symbol value for %s", "key");
  
  items[index]->value = val;
}

long SymbolTable::Insert(const char *key, int arity)
{
  long index = 0;
  //  long length = strlen(key);
  ostringstream ss;

  ss << key;

#if DEBUG & DBG_SYMBOL
  insert = 1;
#endif
  
  SymbolNode *new_node = NULL;
  debug(DBG_SYMBOL, 2, "Inserting symbol (%s, %d)", key, arity);

  if (arity > 0) {
    output(ss, "/%d", arity);
  }
  // try to find the correct index
  index = FindIndex(ss.str().c_str(), NORMAL_MASK);

#if DEBUG & DBG_SYMBOL
  insert = 0;
#endif

  assert(index < MAX_SIZE);
  
  if (items[index]) { // old item
    return items[index]->value;
  }
    
  if (arity > max_arity)
    max_arity = arity;
  
  new_node =  new SymbolNode(ss.str().c_str());
  if (!new_node)
    error(SYS_ERR, "malloc error");
  
  if (constants)
    new_node->value = MAKE_CONSTANT(size);
  else
    new_node->value = size;
  items[index] = new_node;
  symbols[size] = clone_string(ss.str().c_str());
  size++;
  
  // If the table is half full, rehash it
  if ( size >= MAX_SIZE/2-1) {
    ReHash();
  }
  return new_node->value;
}

char *SymbolTable::LookupByValue(long value)
{
  if (!IS_CONSTANT(value))
    return NULL;

  long val = GET_CONSTANT(value);

  return symbols[val];
}
  
  

long SymbolTable::Lookup(const char *key, int arity)
{
  debug(DBG_SYMBOL, 2, "Looking for symbol (%s)", key);
  long index = 0;
  //  int pos = 0;
  ostringstream ss;
  
  if (arity < 0) {
    ss << key;
  } else {
    output(ss, "%s/%d", key, arity);
  }
  index = FindIndex(ss.str().c_str(), NORMAL_MASK);
  
  assert( index < MAX_SIZE);
  
  if (items[index] != NULL)
    return items[index]->value;
  else
    return ERROR_NUMBER;
}

long SymbolTable::LookIndex(const char *key, int arity)
{
  debug(DBG_SYMBOL, 2, "Looking for symbol (%s)", key);
  long index = 0;
  ostringstream ss;
  //  int pos = 0;
  
  if (arity < 0) {
    ss << key;
  } else {
    output(ss, "%s/%d", key, arity);
  }
  
  index = FindIndex(ss.str().c_str(), NORMAL_MASK);
  
  assert( index < MAX_SIZE);
  
  if (items[index] != NULL)
    return index;
  else
    return ERROR_NUMBER;
}



void SymbolTable::ReHash()
{
  long i, index = 0;
  char **tmp_symbols = NULL;
  debug(DBG_SYMBOL, 3, "Rehashing SymbolTable:\n\tOld MAX_SIZE:"
	"%ld\n\tNew MAX_SIZE: %ld\n", MAX_SIZE, NEW_SIZE); 

  debug(DBG_SYMBOL, 4, "\tAllocating new table");
  new_size = max_size +1;
  tmp_nodes = new SymbolNode *[NEW_SIZE];
  if (!tmp_nodes)
    error(SYS_ERR, "malloc error");
  for (i = 0; i < NEW_SIZE; i++)
    tmp_nodes[i] = NULL;
  
  // enlarge symbols-array also.
  tmp_symbols = new char*[NEW_SIZE];
  for (i = 0; i < MAX_SIZE; i++) 
    tmp_symbols[i] = symbols[i];
  // clear the rest of the 
  for ( ; i < NEW_SIZE; i++)
    tmp_symbols[i] = NULL;
  delete [] symbols;
  symbols = tmp_symbols;
  
  debug(DBG_SYMBOL, 4, "\tRehashing:");
  for (i = 0; i < MAX_SIZE; i++) {
    if (items[i] != NULL) {
      debug(DBG_SYMBOL, 5, "\t\t%ld: symbol: %s, val %ld", i,
	    items[i]->symbol, items[i]->value);      
      index = FindIndex(items[i]->symbol, REHASH_MASK);

      assert( index < NEW_SIZE);
      tmp_nodes[index] = items[i];
    } 
  }
  max_size = new_size;

  debug(DBG_SYMBOL, 4, "Rehash: Deleting old nodes");
  delete [] items;
  items = tmp_nodes;
}

void SymbolTable::PrintItems()
{
  long i;

  output(cerr, "Table items:\n");
  for (i = 0; i < MAX_SIZE; i++) {
    if (items[i] != NULL)
      output(cerr, "\t%s: %ld\n", items[i]->symbol,
	      items[i]->value);
  }
}

long SymbolTable::CheckName(char *key, int ar1)
{
  int len = strlen(key);
  int ar2 = 0;
  
  // find all same keys and check if they have different arity 
  for ( long i = 0; i < MAX_SIZE; i++) {
    if (!items[i])
      continue;
    
    if (!strncmp(items[i]->symbol, key, len) && items[i]->symbol[len] == '/') {
      sscanf(&(items[i]->symbol[len+1]), "%d", &ar2);
      if (ar1 != ar2)
	return items[i]->value;
    }
  }
  return ERROR_NUMBER;
}


FunctionTable::FunctionTable(long)
{
  debug(DBG_SYMBOL, 1, "Creating FunctionTable");

  if (sys_data.force_small_domains) {
    max_size = SMALL_DOMAIN_SIZE;
  } else {
    max_size = SYMBOL_DEFAULT_SIZE;
  }

  size = 0;
  new_size = 0;
  
  debug(DBG_SYMBOL, 4, "\tAllocating items");
  items = new FunctionNode*[MAX_SIZE];
  if (!items)
    error(SYS_ERR, "malloc error");

  for (long i = 0; i < MAX_SIZE; i++)
    items[i] = NULL;
}

FunctionTable::~FunctionTable()
{
  debug(DBG_SYMBOL, 1, "destroying FunctionTable");
  long i;
  debug(DBG_SYMBOL, 4, "\tdestroying items");
  for (i = 0; i < MAX_SIZE; i++) {
    if (items[i]) {
      delete [] items[i]->symbol;
      delete items[i];
    }
  }
  
  debug(DBG_SYMBOL, 4, "\tdestroying array");
  delete [] items;
}


unsigned long FunctionTable::FindIndex(char *key, long mask)
{
  long index = 0,ind1 = 0, ind2 = 0;
  long tries = 20;
  FunctionNode **table = NULL;

  if (mask == REHASH_MASK)
    table = tmp_nodes;
  else
    table = items;
  
  ind1 = hash(key, strlen(key), SEED_1) % mask;
  if (table[ind1] && strcmp(key, table[ind1]->symbol))
    ind2 = (hash(key, strlen(key), SEED_2) % mask) + 1;
  else
    return ind1;

  index = ind1;
  do {
    tries--;
    index = (index + ind2) % mask;
  } while (table[index] && strcmp(key, table[index]->symbol) && tries);

  if (!tries) { //switch to linear probing
    do {
      index = (index+1) % mask;
    } while (table[index] && strcmp(key, table[index]->symbol));
  }
  
#if DEBUG & DBG_SYMBOL
  if (insert)
    conflicts += i;
#endif
  return index;
}


long FunctionTable::Insert(char *key)
{
  long index = 0;
  InstFunc fun = NULL;
  LibraryNode *ln = NULL;
  debug(DBG_SYMBOL, 2, "Inserting function (%s, %d)", key);

  // try first to find the symbol among registered functions:
  index = FindIndex(key, NORMAL_MASK);
  
  assert(index < MAX_SIZE);
  
  if (items[index]) {
    items[index]->valid = 1;
    return index;
  }

  // find the user libraries, if necessary
#if HAVE_DLFCN_H
  find_libraries();
#endif
  
  // then go through all user defined libraries loading them if
  // neccessary to see if the function is found somewhere.
  user_libraries->ClearIterator();
  while ((ln = user_libraries->Iterate())) {
    fun = ln->FindSymbol(key);
    if (fun) {
      return Register(key, fun, 1);
    }
  }
  
  // the function was not found
  extern long lineno;
  error(USR_ERR, "%s: can't find definition for function '%s'",
	error_file_and_line(lineno), key);
  return -1;
}

long FunctionTable::Register(char *key, InstFunc p, int val)
{
  debug(DBG_SYMBOL, 2, "Registering function (%s, %ld)", key,
	(long) p);
  long index = 0;
  FunctionNode *new_node  = NULL;
  index = FindIndex(key, NORMAL_MASK);
  
  assert(index < MAX_SIZE);

  if (items[index]) {
    warn(WARN_LIBRARY, 0, "Trying to declare function %s twice.", key);
    return index;
  }

  new_node = new FunctionNode;
  if (!new_node)
    error(SYS_ERR, "malloc error");

  size++;
  debug(DBG_SYMBOL, 4, "\tInitializing new_node");
  new_node->symbol = clone_string(key);
  new_node->valid = val;
  new_node->func = p;

  items[index] = new_node;
  if (size > MAX_SIZE/2 -1) {
    ReHash();
    index = FindIndex(key, NORMAL_MASK);
  }
  return index;
}

long FunctionTable::Lookup(char *key)
{
  debug(DBG_SYMBOL, 2, "Looking for symbol (%s)", key);
  long index = FindIndex(key, NORMAL_MASK);

  assert( index < MAX_SIZE);

  // check if exists and if valid
  if (!items[index] || !items[index]->valid)
    return ERROR_NUMBER;
  else
    return 1;
}

  
void FunctionTable::ReHash()
{
  long  i = 0, index = 0;
  new_size = max_size+1;
  debug(DBG_SYMBOL, 3, "Rehashing FunctionTable:\n\tOld max_size:"
	"%ld\n\tNew max_size: %ld\n", MAX_SIZE,
	NEW_SIZE); 

  debug(DBG_SYMBOL, 4, "\tAllocating new table");
  tmp_nodes = new FunctionNode *[NEW_SIZE];
  if (!tmp_nodes)
    error(SYS_ERR, "malloc error");

  for (i = 0; i < NEW_SIZE; i++) {
    tmp_nodes[i] = NULL;
  }

  debug(DBG_SYMBOL, 4, "\tRehashing:");
  for (i = 0; i < MAX_SIZE; i++) {
    if (items[i] != NULL) {
      debug(DBG_SYMBOL, 5, "\t\t%ld: symbol: %s, %s", i, 
	    items[i]->symbol, (items[i]->valid) ?
	    "valid" : "invalid");
      
      index = FindIndex(items[i]->symbol, REHASH_MASK);
      assert( index < NEW_SIZE);
      tmp_nodes[index] = items[i];
    } 
  }
  max_size = new_size;
  debug(DBG_SYMBOL, 4, "Rehash: Deleting old nodes");
  delete [] items;
  items = tmp_nodes;
}

InstFunc FunctionTable::GetFunction(char *key)
{
  long index = FindIndex(key, NORMAL_MASK);
  if (!items[index] || !items[index]->valid)
    return NULL;
  else
    return items[index]->func;
}

int compare_functions(const void *it1, const void *it2)
{
  FunctionNode const *i1 = *((FunctionNode *const*) it1); 
  FunctionNode const *i2 = *((FunctionNode *const*) it2);

  //  i1 = *((FunctionNode **) it1);
  //  i2 = *((FunctionNode **) it2);
  if (!i1) 
    return 1;
  if (!i2)
    return -1;

  return strcmp(i1->symbol, i2->symbol);
}

void FunctionTable::PrintRegistered()
{
  output(cerr, "This doesn't (at least currently) work\n");
}



#ifdef DEBUG
void FunctionTable::PrintItems()
{
  long i = 0;

  output(cerr, "Table items:\n");

  for (i = 0; i < MAX_SIZE; i++) {
    if (items[i] != NULL)
      output(cerr, "\t%s: %s\n", items[i]->symbol,
	      items[i]->valid ? "valid" : "invalid");
  }
}
#endif
      

