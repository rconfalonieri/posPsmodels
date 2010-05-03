// rule.h -- declarations for rules for lparse
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

#ifndef RULE_H
#define RULE_H
#include "config.h"
#ifndef GLOBAL_H
#include "global.h"
#endif
#ifndef LIST_H
#include "list.h"
#endif
#ifndef LITERAL_H
#include "literal.h"
#endif
#ifndef TERM_H
#include "term.h"
#endif
#ifndef ARRAY_H
#include "array.h"
#endif
#include "priority.h"

class Rule
{
public:
  Rule(Literal *, RuleType, int del = 1);
  Rule(RuleType, int num_heads);
  ~Rule();
  
  void AddLiteral(Literal *lt);
  void AddFunction(Function *, int start = 0);
  void AddHead(Literal *lt);
  
  // Writes the ranges and function calls  out by adding variables and
  // literals
  void SimplifyLiteral(Literal *lt);
  void SimplifySpecialRule();
  void SimplifyAtomsFromFunctions(Function *f);
  void RestrictFunction(Function *tm);
  void GroundRule(int);
  void GroundSpecialRule(int);
  
  void EmitGround(int, int, int);
  void EmitSpecialRule(int, int, int);
  void EmitSmodels1(int gpos, int gneg, int);
  void EmitSmodels2(int gpos, int gneg, int);
  void EmitText(int gpos, int gneg, int);
  void EmitNormalText();
  void EmitNormalTextBody(Literal *);
  void EmitNormalGround(int gpos, int gneg, int);
  void EmitNormalGroundBody(Literal *, int gpos, int gneg, int);
  void EmitFalse();
  void EmitOrHead();
  void PR();
  void PrintRule(ostream& s = cerr);
  void EmitPriority(int gpos, int gneg);
  void EmitMetaPriority(int gpos, int gneg);
  void PrintSpecialHead();
  int TrueGround();
  void RemoveLocalVars();

  int SortFunctions();
  
  void GroundCondition(Literal *, LiteralList *);

  void GroundPriorityRule();
  

  void DefineImplicitDomain();
  void CalculateOptimize();
  void CalculateCompute();
  int ExpandCondition(Literal *, LiteralList *, FunctionList*, int);
  void ExpandChoiceRule();

  // expands all local conditions
  void ExpandAllConditions();

  // expands all global conditions. Returns 1 if the resulting rule is
  // still satisfiable, and 0 if not. 
  int ExpandGlobalConditions();

  // searches correct weight definitions for all literals that need them
  void FindWeights();

  // creates a complementary rule to be used with regular model
  // semantics. Also modifes the current rule by inverting negated
  // atoms. 
  Rule *CreateComplement();

  RestrictType CheckRestrict(int);
  RestrictType CheckSpecialRestrict();
  void EmitNormalGroundSmodels2(int gpos, int gneg, int);
  int CheckNegativeLiterals(); // should negative literals be checked

  void GroundHeads(); // create the list of all heads of the rule
  
  void CreateInstance(int);

  const char *GetPrintString();
  const char *GetInstanceString();

  int CheckTyping();

  void PrintDebugInformation();
  
  Literal *head;
  Literal **head_array;
  int head_number, max_head;
  int h_pos;

  Term *atleast;
  Term *atmost;

  int weight_head : 1;
  int ground : 1;
  int delete_all : 1;
  int sorted : 1;

  int transitive_domain : 1;
  int fact : 1;
  
  int deleted; //number of literals deleted from rule head
  
  int num_weights; // how many weight rules there are in the body
  int num_assigns; // how many assign functions are there in the
		   // rule body
  int num_conditions; // how many conditions are in the rule body, not
  		   //  counting those in special rules.
  int num_head_conditions; // how many global conditions are in rule head
  int num_positive_conditions; //how many global conditions are in rule body
  int num_negative_conditions;

  int num_internal_literals;
  int num_negative_internal_literals;
  
  // how many positive external atoms are in the rule tail
  int num_positive_externals;
  int num_negative_externals;

  LiteralList grounded_heads;

  LiteralList positive, negative;
  LiteralList *special_lits;
  FunctionList functions;
  LiteralList positive_conditions, negative_conditions, head_conditions;

  RestrictType restrict_status;
  long line_start, line_end;
  RuleType type;
  static int max_tail;
  int special; 
  Variable *vars; // keep status of variables for special rules
  Choice *choice;
  Literal *label;
  long first_ground;
  long num_ground;
  char *print_str;
};


#endif
