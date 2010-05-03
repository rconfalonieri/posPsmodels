// list.cc -- implementation of lparse's list classes
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
#include <limits.h>

#include "config.h"
#ifndef LIST_H
#include "list.h"
#endif
#ifndef ERROR_H
#include "error.h"
#endif
#ifndef DEBUG_H
#include "debug.h"
#endif
#ifndef RULE_H
#include "rule.h"
#endif
#ifndef LITERAL_H
#include "literal.h"
#endif
#ifndef PREDICATE_H
#include "predicate.h"
#endif
#ifndef TERM_H
#include "term.h"
#endif
#ifndef PARSETREE_H
#include "parsetree.h"
#endif
#ifndef LIBRARY_H
#include "library.h"
#endif

List::List(ListType tp)
{
  type = tp;
  last = items = CreateEmptyNode();
  current = NULL;
  if (!items)
    error(SYS_ERR, "malloc error");
  size = 0;
  last_weigth = last->weight;
}

List::~List()
{
  ListNode *tmp = NULL;
  
  while (items) {
    tmp = items;
    items = items->next;
    delete tmp;
  }
}

ListNode *List::CreateEmptyNode()
{
  ListNode *new_node = NULL;

  new_node = new ListNode;

  if (!new_node)
    error(SYS_ERR, "malloc error");

  new_node->item = NULL;
  new_node->next = NULL;
  if (type == ORDERED_BIG)
    new_node->weight = LONG_MAX;
  else
    new_node->weight = LONG_MIN;
  return new_node;
}

ListNode *List::DuplicateNode(ListNode *nd)
{
  assert(nd != NULL);
  ListNode *new_node = NULL;
  new_node = new ListNode;

  if (! new_node)
    error(SYS_ERR, "malloc error");

  new_node->item = nd->item;
  new_node->weight = nd->weight;
  new_node->next = NULL;
  return new_node;
}

void List::Insert(void *item, long weight)
{
  ListNode *new_node = NULL;
  ListNode *tmp = items;
  
  new_node = new ListNode;
  if (! new_node)
    error(SYS_ERR, "malloc error");

  new_node->weight = weight;
  new_node->item = item;
  new_node->next = NULL;
  size++;

  debug(DBG_LIST, 3, "List::Insert(), weight: %d, size: %d",
	weight, size);
  
  switch (type) {
  case UNORDERED:
    new_node->next = items->next;
    items->next = new_node;
    break;
  case ORDERED_BIG:
    if (weight < last->weight) {
      last->next = new_node;
      last = new_node;
      return;
    }
    while (tmp->next && (tmp->next->weight > weight))
      tmp = tmp->next;
    new_node->next = tmp->next;
    tmp->next = new_node;
    break;
  case ORDERED_SMALL:
    if (weight > last->weight) {
      last->next = new_node;
      last = new_node;
      current = items->next;
      return;
    }
    while (tmp->next && (tmp->next->weight < weight))
      tmp = tmp->next;
    new_node->next = tmp->next;
    tmp->next = new_node;
    break;
  }
  current = items->next;
  if (new_node->next == NULL) {
    last = new_node;
    last_weigth = new_node->weight;
  }
  return;
}

void List::Remove(void *item)
{
  ListNode *tmp1 = NULL;
  ListNode *tmp2 = NULL;

  
  tmp1 = items->next;
  tmp2 = items;

  while (tmp1 && (tmp1->item != item)) {
    tmp2 = tmp1;
    tmp1 = tmp1->next;
  }
  if (tmp1) {
    tmp2->next = tmp1->next;
    if (current == tmp1)
      current = tmp2->next;
    delete tmp1;
    size--;
  }
}
    

// Ei pida jarjestysta kunnossa
void List::Append(void *item, long wght)
{
  ListNode *new_node = NULL;
  new_node = new ListNode;

  if (!new_node)
    error(SYS_ERR, "malloc error");
  new_node->weight = wght;
  new_node->item = item;
  new_node->next = NULL;

  last->next = new_node;
  last = new_node;
  last_weigth = wght;
  current = items->next;
  size++;
}

void *List::Iterate()
{
  ListNode *tmp = current;

  if (!current) {
    current = items->next;
    return NULL;
  }
  current = current->next;
  return tmp->item;
}

void List::Merge(List *lst)
{
  debug(DBG_LIST, 3, "List::Merge");

  if (type == UNORDERED)
    MergeUnordered(lst);
  else
    MergeOrdered(lst);
}

void List::MergeUnordered(List *lst)
{
  debug(DBG_LIST, 3, "List::MergeUnordered");
  last->next = lst->items->next;
  last = lst->last;
  
  size += lst->size;
}

// Should x be before y in list
#define BEFORE(x, y) ((type == ORDERED_SMALL) ? \
		      ((x) < (y)) : ((x) > (y))) 

void List::MergeOrdered(List *lst)
{
  debug(DBG_LIST, 3, "List::MergeOrdered()");
  ListNode *new_list = NULL, *tmpn = NULL, *tmp1 = NULL, *tmp2 = NULL;
  
  new_list = CreateEmptyNode();
  if (! new_list)
    error(SYS_ERR, "malloc error");
  tmpn = new_list;

  tmp1 = items->next;
  tmp2 = lst->items->next;

  // Size is initialized to total lengths of both lists and then
  // subrtacted by one for each duplicate.
  size += lst->size;
  
  for (;;) {
    if (!tmp1) {  
      debug(DBG_LIST, 3, "List 1 finished");
      while (tmp2) {
	tmpn->next = DuplicateNode(tmp2);
	tmp2 = tmp2->next;
	tmpn = tmpn->next;
	last = tmpn;
      }
      break;
    } else if (!tmp2){  
      debug(DBG_LIST, 3, "List 2 finished");
      while (tmp2) {
	tmpn->next = DuplicateNode(tmp2);
	tmp2 = tmp2->next;
	tmpn = tmpn->next;
	last = tmpn;
      }
      break;
    } else if (BEFORE(tmp1->weight, tmp2->weight)) {
      debug(DBG_LIST, 3, "\t %d / %d chose 1", tmp1->weight, tmp2->weight);
      tmpn->next = DuplicateNode(tmp1);
      tmpn = tmpn->next;
      tmp1 = tmp1->next;
      last = tmpn;
    } else if (BEFORE(tmp2->weight, tmp1->weight)) {
      debug(DBG_LIST, 3, "\t %d / %d chose 2", tmp1->weight, tmp2->weight);
      tmpn->next = DuplicateNode(tmp2);
      tmpn = tmpn->next;
      tmp2 = tmp2->next;
      last = tmpn;
    } else {
      debug(DBG_LIST, 3, "\t %d / %d ...", tmp1->weight, tmp2->weight);
      if (tmp1->item == tmp2->item) {  // is duplicate?
	debug(DBG_LIST, 3, "\t equal");
	size--;
	tmp2 = tmp2->next;
      }
      tmpn->next = DuplicateNode(tmp1);
      tmpn = tmpn->next;
      tmp1 = tmp1->next;
      last = tmpn;
    }
  }

  // Delete the old list
  tmp1 = items;
  while (tmp1) {
    tmp2 = tmp1;
    tmp1 = tmp1->next;
    delete tmp2;
  }

  items = new_list;
  current = 0;
  last_weigth = last->weight;
}
    
void List::Clear()
{
  ListNode *tmp = NULL;

  current= items->next;
  
  while(current) {
    tmp = current->next;
    delete current;
    current = tmp;
  }

  size = 0;
  last_weigth = 0;
  items->next = NULL;
  current = items->next;
  last = items;
}

	  
RuleList::RuleList(ListType tp)
{
  type = tp;
}



RuleList::~RuleList()
{
  /*  ListNode *tmp = NULL;

  while (items) {
    tmp = items;
    items = items->next;
    delete ((Rule*)tmp->item);
    delete tmp;
    }
  items = NULL;
   */
}


void RuleList::Insert(Rule *item, long weight)
{
  List::Insert((void *) item, weight);
}

void RuleList::Append(Rule *item, long weight)
{
  List::Append((void *) item, weight);
}

Rule *RuleList::Iterate()
{
  return (Rule *) List::Iterate();
}


LiteralList::LiteralList(ListType tp)
{
  type = tp;
}


LiteralList::~LiteralList()
{
  // literals have to be kept intact while deleting.
#if 0
  ListNode *tmp;


  while (items) {
    tmp = items;
    items = items->next;
    delete ( (Literal*) tmp->item);
    delete tmp;
  }
  items = NULL;
#endif
}


void LiteralList::Insert(Literal *item, long weight)
{
  List::Insert((void *) item, weight);
}


void LiteralList::Remove(Literal *item)
{
  List::Remove((void *) item);
}



Literal *LiteralList::Iterate()
{
  return (Literal*) List::Iterate();
}

void LiteralList::Clear(int del)
{

  if (!del) {
    List::Clear();
  } else {
    ListNode *tmp1 = items->next;
    ListNode *tmp2 = NULL;
    
    while (tmp1) {
      delete ( (Literal*) tmp1->item);
      tmp2 = tmp1->next;
      delete tmp1;
      tmp1 = tmp2;
    }
    size = 0;
    last_weigth = 0;
    items->next = NULL;
    last = items;
    current = items->next;
  }
}


LongList::LongList(ListType tp)
{
  type = tp;

}


LongList::~LongList()
{
  ListNode *tmp;

  while (items) {
    tmp = items;
    items = items->next;
    delete tmp;
  }
}


void LongList::Insert(long item, long weight)
{
  List::Insert((void *) item, weight);
}


long LongList::Iterate()
{
  return (long) List::Iterate();
}


FunctionList::FunctionList(ListType tp)
{
  type = tp;
}

// The graph 'gr' is used to compare the variables in the graph
void FunctionList::Sort(Graph *gr)
{
  FunctionList *new_list = new FunctionList(ORDERED_SMALL);
  Function *fun = 0;
  long weight = 0;

  if (size < 2) // nothing to be done
    return;
  
  while ((fun = Iterate())) {
    if (fun->assign)
      weight = gr->GetPosition(fun->args[0]->val);
    else
      weight = sys_data.total_vars;
    new_list->Insert(fun, weight);
  }
  items = new_list->items;
  last = new_list->last;
  last_weigth = new_list->last_weigth;
  ClearIterator();
}


FunctionList::~FunctionList()
{
  // delete items elsewhere
}


void FunctionList::Insert(Function *item, long weight)
{
  Function *fun = NULL;
  // removes duplicates
  while ((fun = Iterate())) {
    if (fun == item)
      return;
  }
  List::Insert((void *) item, weight);
}

void FunctionList::Append(Function *item, long weight)
{
  List::Append((void *) item, weight);
}


Function *FunctionList::Iterate()
{
  return (Function*) List::Iterate();
}

ParseNodeList::ParseNodeList(ListType tp)
{
  type = tp;

}

ParseNodeList::~ParseNodeList()
{
  ListNode *tmp;
  
  while (items) {
    tmp = items;
    items = items->next;
    delete ((ParseNode*) tmp->item);
    delete tmp;
  }
  items = NULL;
}

void ParseNodeList::Insert(ParseNode *item, long weight)
{
  ParseNode *fun = NULL;
  // removes duplicates
  while ((fun = Iterate())) {
    if (fun == item)
      return;
  }
  List::Insert((void *) item, weight);
}

void ParseNodeList::Append(ParseNode *item, long weight)
{
  List::Append((void *) item, weight);
}


ParseNode *ParseNodeList::Iterate()
{
  return (ParseNode*) List::Iterate();
}


void WeightList::Insert(WeightNode *item, long weight)
{
  WeightNode *fun = NULL;
  // removes duplicates
  while ((fun = Iterate())) {
    if (fun == item)
      return;
  }
  List::Insert((void *) item, weight);
}

void WeightList::Append(WeightNode *item, long weight)
{
  List::Append((void *) item, weight);
}


WeightNode *WeightList::Iterate()
{
  return (WeightNode*) List::Iterate();
}


WeightList::WeightList(ListType tp)
{
  type = tp;

}


WeightList::~WeightList()
{
  ListNode *tmp;

  while (items) {
    tmp = items;
    items = items->next;
    delete ((WeightNode*) tmp->item);
    delete tmp;
  }
  items = NULL;
}

void StringList::Insert(char *item, long weight)
{
  List::Insert((void *) item, weight);
}

void StringList::Append(char *item, long weight)
{
  List::Append((void *) item, weight);
}


void StringList::Clear()
{
  ListNode *it = NULL;
  ListNode *tmp = NULL;

  it = items->next;
  items->next = NULL;
  while (it) {
    delete [] (char*)it->item;
    tmp = it->next;
    delete it;
    it = tmp;
  }
  current = NULL;
  last = items;
}

char *StringList::Iterate()
{
  return (char*) List::Iterate();
}


StringList::StringList(ListType tp)
{
  type = tp;
}


StringList::~StringList()
{
  ListNode *tmp;

  while (items) {
    tmp = items;
    items = items->next;
    free ((char*) tmp->item);
    delete tmp;
  }
  items = NULL;
}

void LibraryList::Insert(LibraryNode *item, long weight)
{
  LibraryNode *fun = NULL;
  // removes duplicates
  while ((fun = Iterate())) {
    if (fun == item)
      return;
  }
  List::Insert((void *) item, weight);
}

void LibraryList::Append(LibraryNode *item, long weight)
{
  List::Append((void *) item, weight);
}


LibraryNode *LibraryList::Iterate()
{
  return (LibraryNode*) List::Iterate();
}


LibraryList::LibraryList(ListType tp)
{
  type = tp;

}


LibraryList::~LibraryList()
{
  ListNode *tmp;

  while (items) {
    tmp = items;
    items = items->next;
    delete ((LibraryNode*) tmp->item);
    delete tmp;
  }
  items = NULL;
}



RangeList::RangeList(ListType tp)
{
  type = tp;

}

RangeList::~RangeList()
{
  ListNode *tmp;
  
  while (items) {
    tmp = items;
    items = items->next;
    delete ((Range*) tmp->item);
    delete tmp;
  }
  items = NULL;
}

void RangeList::Insert(Range *item, long weight)
{
  Range *fun = NULL;
  // removes duplicates
  while ((fun = Iterate())) {
    if (fun == item)
      return;
  }
  List::Insert((void *) item, weight);
}

void RangeList::Append(Range *item, long weight)
{
  List::Append((void *) item, weight);
}


Range *RangeList::Iterate()
{
  return (Range*) List::Iterate();
}


ChoiceList::ChoiceList(ListType tp)
{
  type = tp;

}

ChoiceList::~ChoiceList()
{
  ListNode *tmp;
  
  while (items) {
    tmp = items;
    items = items->next;
    delete ((Choice*) tmp->item);
    delete tmp;
  }
  items = NULL;
}

void ChoiceList::Insert(Choice *item, long weight)
{
  Choice *fun = NULL;
  // removes duplicates
  while ((fun = Iterate())) {
    if (fun == item)
      return;
  }
  List::Insert((void *) item, weight);
}

void ChoiceList::Append(Choice *item, long weight)
{
  List::Append((void *) item, weight);
}


Choice *ChoiceList::Iterate()
{
  return (Choice*) List::Iterate();
}

