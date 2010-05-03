// list.h -- list classes for lparse
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

#ifndef LIST_H
#define LIST_H

#include "config.h"

enum ListType { UNORDERED, ORDERED_BIG, ORDERED_SMALL };

struct ListNode {
  long weight;
  void *item;
  ListNode *next;
};

/*
 * List is an abstract class to be used as a base class and actual
 * lists for each type will be inherited from it. It would be more
 * natural to use templates, but there seems to be some problems with
 * template instantiation. Yup. Pretty ugly. 
 *
 * List stores (void *) pointers, and the derived classes should act
 * as wrappers converting items to and from void pointers.
 *
 * The type of the list (ordered or unordered) is chosen by argument
 * to constructor.
 *
 * List uses sentinel node at the start of the list.
 *
 */
class List
{
public:
  // Constructor for List. 
  List(ListType type = UNORDERED);
  ~List();
  
  /**
   * Inserts an item to list. Inserting an item resets iteration
   * counter. Weight defaults as 0 so the same function can be used
   * for both ordered and unordered lists.
   */
  void Insert(void *item, long weight = 0);
  void Append(void *item, long weight = 0);
  void Remove(void *item);
  
  /// Returns the items one at time. Returns NULL at the end. 
  void *Iterate();

  /** Merges another with this list. Same weighting duplicates are
   * removed for ordered lists, but not for unordered.
   */
  void Merge(List *lst);

  void Clear();

  int Find(void *item);
  void ClearIterator() { current = items->next; }
  
  long Length() {return size; }
  long Size() { return size; }
  ListType Type() { return type; }
protected:
  void MergeUnordered(List *lst);
  void MergeOrdered(List *lst);

  ListNode *CreateEmptyNode();
  ListNode *DuplicateNode(ListNode *);

  ListNode *items, *current, *last;
  long size, last_weigth;
  ListType type;
};


class Rule;

/**
 * RuleList implements a list of rules.
 */
class RuleList : public List
{
public:
  RuleList(ListType type = UNORDERED);
  ~RuleList();

  void Insert(Rule *item, long weight = 0);
  void Append(Rule *item, long weight = 0);
  Rule *Iterate();
};

class Literal;

class LiteralList : public List
{
public:
  LiteralList(ListType type = UNORDERED);
  ~LiteralList();

  void Insert(Literal *item, long weight = 0);
  void Remove(Literal *item);
  void Clear(int del = 0);
  Literal *Iterate();
};

class LongList : public List
{
public:
  LongList(ListType type = UNORDERED);
  ~LongList();

  void Insert(long item, long weight = 0);
  long Iterate();
  void Remove(long item);
};


class Function;
class Graph;
class FunctionList : public List
{
public:
  FunctionList(ListType type = UNORDERED);
  ~FunctionList();

  void Insert(Function *item, long weight = 0);
  void Append(Function *item, long weight = 0);

  void Sort(Graph *);
  Function *Iterate();
};

class ParseNode;

class ParseNodeList : public List
{
public:
  ParseNodeList(ListType type = UNORDERED);
  ~ParseNodeList();
  
  void Insert(ParseNode *item, long weight = 0);
  void Append(ParseNode *item, long weight = 0);
  ParseNode *Iterate();
};

class WeightNode;

class WeightList : public List
{
public:
  WeightList(ListType type = UNORDERED);
  ~WeightList();
  
  void Insert(WeightNode *item, long weight = 0);
  void Append(WeightNode *item, long weight = 0);
  WeightNode *Iterate();
};

class StringList : public List
{
public:
  StringList(ListType type = UNORDERED);
  ~StringList();
  
  void Insert(char *item, long weight = 0);
  void Append(char *item, long weight = 0);
  void Clear();
  char *Iterate();
};

struct LibraryNode;

class LibraryList : public List
{
public:
  LibraryList(ListType type = UNORDERED);
  ~LibraryList();
  
  void Insert(LibraryNode *item, long weight = 0);
  void Append(LibraryNode *item, long weight = 0);
  LibraryNode *Iterate();
};


class Range;

class RangeList : public List
{
public:
  RangeList(ListType type = UNORDERED);
  ~RangeList();
  
  void Insert(Range *item, long weight = 0);
  void Append(Range *item, long weight = 0);
  Range *Iterate();
};



struct Choice;

class ChoiceList : public List
{
public:
  ChoiceList(ListType type = UNORDERED);
  ~ChoiceList();
  
  void Insert(Choice *item, long weight = 0);
  void Append(Choice *item, long weight = 0);
  Choice *Iterate();
};

#endif
