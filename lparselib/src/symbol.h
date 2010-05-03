// symbol.h -- definition of symbol table structures for lparse
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

#ifndef SYMBOL_H
#define SYMBOL_H
#include "config.h"
#ifndef DEBUG_H
#include "debug.h"
#endif
#ifndef DEFINES_H
#include "defines.h"
#endif

// Normal identifier, only value is needed. 
// arity should be coded in predicate definition: like foo(X,Y) -> foo/2 
struct SymbolNode
{
  const char *symbol;
  long value;
  SymbolNode(const char *);
  ~SymbolNode();
};

/* Class Symboltable is a closed automatically resizing hash table. It
   is used for storing predicates, variables and nonnumeric
   constants. 

   Attributes:
   	SymbolNode **items	-- the actual table
	SymbolNode **tmp_items	-- temporary table used in resizing
	long size		-- how many items are stored in table
	long max_size		-- How big the table is.
	***NOTE: max_size is actually an index to primes array. If you
	   try to use it directly you will be badly hosed. (Been
	   there, done that, trust me) ***
        long max_arity		-- What is the greatest arity seen
			           this far. This is meaninful only
				   for predicates and it is used by
				   functions that want to know how
				   much to reserve memory for each
				   instance.
	char **symbols		-- Array of all symbols in program
				   indexed by their values. 
   Methods:

   	long Insert(char *key, int ar = -1)
		-- Inserts symbol 'key' with arity 'ar' to
		   table. Returns the value of symbol. Removes
		   duplicates.
        long Lookup(char *key)	-- returns value of 'key' or -1 if not
				   found
	char *LookupByValue(long val)
		-- Returns symbol corresponding to 'val' You shouldn't
		   modify the returned string since it points to
		   actual entry in the table. 
	void CreateSymbolArray() -- Initializes 'symbols' array.
	long Size()		-- returns number of items in table
	int MaxArity()		-- returns biggest arity seen
	long Hash(char *key, int sz = 0)
		-- Finds hash value for 'key'. Sz should be '1' when
		   rehashing and '0' otherwise.
	void ReHash()		-- approximately doubles the size of
				   the hash table
	void PrintItems()	-- Prints out all the items in hash table
   */
class SymbolTable
{
public:
  SymbolTable(long sz = SYMBOL_DEFAULT_SIZE, int constants = 0);  
  ~SymbolTable();
  
  long Insert(const char *key, int arity = -1);
  long Lookup(const char *key, int arity = -1);

  long LookIndex(const char *key, int arity = -1);
  char *LookupByValue(long val);
  long Size() { return size; }
  int MaxArity() { return max_arity; }
  void CreateSymbolArray();
  void SetValue(char *key, long val, int arity = -1);

  // check if there is an item 'key' with different arity than 'arity' 
  long CheckName(char *key, int arity);

  void PrintItems();

#if DEBUG & DBG_SYMBOL
  long conflicts;
  int insert;
#endif
  char **symbols;

private:
  unsigned long Hash(const char *key, unsigned long lenght,
		     unsigned long initval);
  void ReHash();
  unsigned long FindIndex(const char *, long);
  SymbolNode **items;
  SymbolNode **tmp_nodes;
  
  long size, max_size, max_arity, new_size;
  int constants; // is 1 if we store constants, otherwise 0
};


  
  

// For declaring functions the function to be called must be specified.
struct FunctionNode
{
  // pointer to function with at least one long-argument.
  InstFunc func;
  char *symbol;
  int valid; // has the function been inserted
};


class FunctionTable
{
public:
  FunctionTable(long sz = SYMBOL_DEFAULT_SIZE);  
  ~FunctionTable();
  
  long Insert(char *key);
  long Register(char *key, InstFunc p, int val = 0); 
  long Lookup(char *key);
  long Define(char *symbol, char *value, int val = 0);

  InstFunc GetFunction(char *key);
  long Size() { return size; }
  void PrintRegistered();
#ifdef DEBUG
  void PrintItems();
#endif

private:

  void ReHash();
  unsigned long FindIndex(char *, long);
  FunctionNode **items, **tmp_nodes;

  long size, max_size, new_size;
};

#endif
