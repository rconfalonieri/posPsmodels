// term.h -- definitions for possible user data types for lparse 
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

#ifndef TERM_H
#define TERM_H
#include "config.h"
#ifndef GLOBAL_H
#include "global.h"
#endif

enum InternalFunction { FUN_NULL, FUN_ABS, FUN_EQ, FUN_LE, FUN_GE,
			FUN_LT, FUN_GT, FUN_NEQ, FUN_PLUS, FUN_MINUS,
			FUN_TIMES, FUN_DIV, FUN_MOD, FUN_ASSIGN,
			FUN_AND, FUN_OR, FUN_NOT, FUN_XOR, FUN_WEIGHT,
			FUN_NORM, FUN_EXP, NUM_MAX_FUN};

enum TermType { T_FUNCTION, T_RANGE, T_VARIABLE, T_CONSTANT, T_ATOM, 
		T_UNKNOWN };


/* ComparisonDirection tells which way numbers are changing from left
   to right */
enum ComparisonDirection { DIR_EMPTY, DIR_NONE, DIR_BIGGER, DIR_LOWER, DIR_BOTH };

// finds the internal function name corresponding to an operator.
// eg. "+" -> "plus"
char *get_function_name(char *);

class ParseNode;
class Range;

class Term
{
public:
  Term(TermType, long, long);
  Term() {type = T_UNKNOWN, val = -1; }
  static Term *ConstructArgument(ParseNode *);  
  Term *Duplicate();

  // return the value of the term
  long Eval();
  void Print();


  
  // check what variables are present in the term and mark them true
  // in 'vs'. Returns the number of new variables found
  int AddVars(Variable *vs);

  void Restrict(int);
  
  // creates and simplifies a bound expression of special rules 
  static Term *CreateBound(ParseNode *);
  static Range *ConstructRange(ParseNode *);
  long val;
  TermType type;
  int ground : 1;
  int has_range : 1;
  int has_function : 1;
  int has_atom : 1;
  long lineno;
  char *GetPrintString();
};

class Rule;

class Function : public Term
{
public:
  Function(InstFunc, ParseNode*, char *, long, int);
  Function(InstFunc, char *, int, long);

  Function(char *, Term *, Term*, long);
  
  ~Function();
  int AddArgument(Term *);
  Instance Call();
  Function *DuplicateFunction();

  // returns the maximum of variable positions in function
  int GetPos();
  Instance Test(int);
  void AddVars(int);
  int AddVars(Variable *);
  void AddVarsByType(Variable *vars, int in_condition = 0);
  void AddGraphEdges(Graph *, long, int initial = 0);
  void SetVars(int);
  void Restrict(int);
  void Print();
  void RemoveAssign();
  int Occurs(Variable v);
  int CheckGlobal(Variable *);
  void RemoveNestedAtoms(Rule *);


  char *GetFunPrintString();
  void AddDependencies(long dep);
  
  Function *Instantiate(Literal *l, Literal *p);
  char *name;
  InstFunc fun;
  Term **args;
  int arity;
  int assign :1;
  int is_weight_function : 1;
  int accept_constants:1;
  int negative:1;
  int allocated;
};

class Range : public Term
{
public:
  Range(Term *, Term *, long, int);
  Range *DuplicateRange();
  void Print();
  Term *start;
  Term *end;
  long current;
  int conjunctive : 1;

  void AddDependencies(long dep);
};

class LiteralTerm : public Term
{
public:
  LiteralTerm(Literal *l);
  ~LiteralTerm(); 
  Literal *lt;
  LiteralTerm *DuplicateAtom();

  int GetPos();
  Instance Test(int);
  void AddVars(int);
  int AddVars(Variable *);
  void  AddVarsByType(Variable *, int);
  void AddGraphEdges(Graph *, long);
  void SetVars(int);
  void Restrict(int);
  Instance ConstructAtom();
  int CheckGlobal(Variable *);
  void Print();
  char *GetLTPrintString();
};

#endif
