// priority.h -- definitions for priority semantics
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

#ifndef PRIORITY_H
#define PRIORITY_H

#include "list.h"
#include "defines.h"
#include "array.h"
#include "literal.h"

enum PrioritySemantics {
  PRI_NONE,
  PRI_INCLUSION,
  PRI_PARETO,
  PRI_CARDINALITY
};

#define CHOICE_PRED 		"_choose"
#define META_PRED 		"priority"
#define OLD_SATISFIED_PRED 	"*old_sat"
#define SATISFIED_PRED 		"*sat"
#define META_CONTROL_PRED	"*control"
#define RULE_PRED 		"_rule"
#define DEGREE_PRED 		"_degree"
#define ACTIVATE_PRED		"_active"
#define WORSE_PRED		"_worse"
#define BETTER_PRED		"_better"
#define EXCUSED_PRED		"_excused"
#define POSSIBLY_PRED		"_psat"

#define CARDINALITY_PRED	"card"
#define OLD_CARDINALITY_PRED	"old_card"


struct Choice
{
  Rule *original;
  RuleList split;
  void UpdateConstants();

};

struct MetaPriorities {
  LongList atom;
  LongList first;
  LongList second;
};


class PriorityData
{
public:
  PriorityData() : num_choices(0), max_degree(0),
		   semantics(PRI_INCLUSION), choices(0), tester(0),
		   choice_pred(-1), meta_pred(-1), control_pred(-1),
		   old_satisfied_pred(-1), satisfied_pred(-1),
		   total_alternatives(0), label_table(0)  { }

  Choice *CreateChoice(Rule *r);
  void AddChoice(Rule *r);
  void GroundChoices();

  void GroundTester();
  
  void CreateGeneratorRules();
  void CreateTesterRules();
  void CreateParetoTesterRules();
  void CreateInclusionTesterRules();
  void CreateCardinalityTesterRules();
  
  RestrictType CheckRestrict(int print);
  long num_choices;
  int max_degree;
  PrioritySemantics semantics;
  ChoiceList *choices;
  RuleList *tester;
  long choice_pred;
  long meta_pred;
  long control_pred;
  long old_satisfied_pred;
  long satisfied_pred;
  long rule_pred;
  long degree_pred;
  long activate_pred;
  long better_pred;
  long worse_pred;
  long excused_pred;
  long possibly_sat_pred;
  long sat_card_pred;
  long old_sat_card_pred;
  ResizableArray choice_degrees;
  ResizableArray sat_atom;
  ResizableArray old_sat_atom;
  long total_alternatives;
  StringList original_rules;
  LongListArray labels;
  SymbolTable *label_table;
  MetaPriorities *mp;
  
};

extern PriorityData priority_data;

#endif
