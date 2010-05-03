// parsetree.h -- definitions for parse tree structure for lparse
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

#ifndef PARSETREE_H
#define PARSETREE_H

#include "config.h"
#ifndef DEBUG_H
#include "debug.h"
#endif
#ifndef GLOBAL_H
#include "global.h"
#endif
#ifndef TERM_H
#include "term.h"
#endif
#ifndef LITERAL_H
#include "literal.h"
#endif
#ifndef RULE_H
#include "rule.h"
#endif
#ifndef LIST_H
#include "list.h"
#endif

enum ParseType { PT_RULES, PT_COMPUTE, PT_DECLARATION,
		 PT_RULE, PT_HEAD, PT_TAIL, PT_SPECIAL_TAIL,
		 PT_BASIC_TAIL, PT_ATOM, PT_FUNCTION,
		 PT_ARGLIST, PT_TERM, PT_CONSTANT, PT_RANGE, PT_EXPR,
		 PT_LITERAL_LIST, PT_COMPUTE_LIST,
		 PT_ARGUMENTS, PT_SPECIAL_HEAD, PT_HEADLIST,
		 PT_VARIABLE, PT_NUMBER, PT_WEIGHT, PT_CONDITION,
		 PT_MINIMIZE, PT_MAXIMIZE, PT_HIDE, PT_EXTERNAL,
		 PT_SHOW, PT_BOUND, PT_DIRECTIVE,
		 PT_ORDERED_DISJUNCTION, PT_UNKNOWN };  

class ParseNode
{
public:
  ParseType type;
  char *sval;
  struct ParseNode *left, *right, *parent;
  union  {
    Function *fun;
    Literal *lit;
    char *st;
  } ext;

  union {
    int conj;
    int weight;
    int paren;
  } i1;

  union {
    RuleType rule_type;
    InternalFunction op;
    long start;
    long val;
  } l1;
  
  union {
    int negative;
	int is_disjunction;
    long end;
  } l2;

  long lineno;
#ifdef DEBUG
  void Print(int ind = 0);
#endif
  ParseNode(ParseType, char *, ParseNode *, ParseNode *,
	    long, long = 0, long = 0, int = 0); 
  ParseNode();
  ~ParseNode();


  int ProcessTree();
  int ProcessCompute(int, int);
  int ProcessOptimize();
  int ProcessCondition();
  int DelayProcessing();
  static int ProcessDelayed();
  static int ProcessWeightDeclarations();
  int ProcessDeclaration();
  int ProcessSpecialRule();
  int ProcessRule(int, int);
  int ProcessAtom(int, int);
  int ProcessTerm(int, int);
  int ProcessWeight();
  int ProcessRange();
  int ProcessFunction(int, int);
  int ProcessDirective();
  int PrintOptimizations(ParseNodeList *);

  int ProcessPriority();
  
  void PrintExpression();
  int HidePredicate();
  int ShowPredicate(); 
  int DefineExternalPredicate();

  // checks whether a bound expression is legal
  int CheckBound();
  
  RuleType IdentifySpecialHead(int *);  
  RuleType IdentifySpecialTail();  
  Literal *ProcessHeadLiteral(ParseNode *hd);
  int AddNewRight(ParseNode *);
  
  void FindDependencies();

  void MergeFunctions();
  
  // evaluates a constant expression
  long Eval();
  int CountArity();
};


#endif
