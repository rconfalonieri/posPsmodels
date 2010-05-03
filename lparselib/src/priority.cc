// priority.cc -- implementation of priority definitions
// Copyright (C) 2003-2007 Tommi Syrjänen <Tommi.Syrjanen@hut.fi>
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

#include "priority.h"
#include "rule.h"
#include "predicate.h"
#include "symbol.h"
#include "error.h"
#include <string.h>
#include "list.h"
#include "literal.h"
#include <stdlib.h>

PriorityData priority_data;

Choice *PriorityData::CreateChoice(Rule *r)
{
  // first put the head atoms to correct order:
  int s = 0, i;
  int e = r->head_number -1;
  Literal *tmp;
  Instance item[2];
  SpecialLiteral *slt;
  Term *t;

  if (r->head_number > max_degree)
    max_degree = r->head_number;
  
  while (s < e) {
    tmp = r->head_array[s];
    r->head_array[s] = r->head_array[e];
    r->head_array[e] = tmp;
    s++;
    e--;
  }

  Choice *choice = new Choice;

  choice->original = r;
  r->choice = choice;
  
  // create the split program:
  
  // a x b x c :- foo
  // ===>  { choose(1,1), choose(1,2), choose(1,3) } :- foo

  Rule *rl = new Rule(CHOICERULE, r->head_number);
  item[0] = -1;
  for (i = 0; i < r->head_number; i++) {
    item[1] = i+1;
    rl->head_array[i] = new Literal(choice_pred, item, 2, 0);
  }
  rl->head_number = r->head_number;
  // body
  Literal *l;
  while ((l = r->positive.Iterate())) {
	tmp = l->Duplicate();
	rl->AddLiteral(tmp);
  }
  while ((l = r->negative.Iterate())) {
    tmp = l->Duplicate();
    rl->AddLiteral(tmp);
  }
  choice->split.Insert(rl);

  // choice is activated if body is true
  l = new Literal(activate_pred, item, 1, 0);
  Rule *sp = new Rule(l, BASICRULE);
  while ((l = r->positive.Iterate())) {
    sp->AddLiteral(l->Duplicate());
  }
  
  while ((l = r->negative.Iterate())) {
    sp->AddLiteral(l->Duplicate());
  }
  choice->split.Insert(sp);
  
  // at least one choice has to be true
  if (!false_lit)
    false_lit = Predicate::DefineFalseLiteral();
  
  sp = new Rule(false_lit, BASICRULE, 0);
  while ((l = r->positive.Iterate())) {
    sp->AddLiteral(l);
  }
  
  while ((l = r->negative.Iterate())) {
    sp->AddLiteral(l);
  }
  for (i = 0; i < r->head_number; i++) {
    tmp = r->head_array[i]->Duplicate(1);
    sp->AddLiteral(tmp);
  }
  choice->split.Insert(sp);

  // at most one choice true
  sp = new Rule(false_lit, BASICRULE, 0);

  t = new Term(T_CONSTANT, 2, 0);
  slt = new SpecialLiteral(CONSTRAINTRULE, t, 0, -1);
  
  while ((l = r->positive.Iterate())) {
    sp->AddLiteral(l);
  }
  
  while ((l = r->negative.Iterate())) {
    sp->AddLiteral(l);
  }
  for (i = 0; i < r->head_number; i++) {
    tmp = rl->head_array[i]->Duplicate(0);
    slt->tail.Insert(tmp);
  }
  sp->AddLiteral(slt);
  choice->split.Insert(sp);

  // create actual split program
  for (i = 0; i < r->head_number; i++) {
    tmp = r->head_array[i]->Duplicate();
    sp = new Rule(tmp, BASICRULE);
    tmp = rl->head_array[i]->Duplicate();
    sp->AddLiteral(tmp);
    for (int j = 0; j < i; j++) {
      tmp = r->head_array[j]->Duplicate(1);
      sp->AddLiteral(tmp);
    }
    while ((l = r->positive.Iterate())) {
      sp->AddLiteral(l);
    }
    
    while ((l = r->negative.Iterate())) {
      sp->AddLiteral(l);
    }
    
    choice->split.Insert(sp);
  }

  for (i = 0; i < r->head_number; i++) {
    tmp = rl->head_array[i]->Duplicate();
    sp = new Rule(tmp, BASICRULE);
    tmp = r->head_array[i]->Duplicate();
    sp->AddLiteral(tmp);
    for (int j = 0; j < i; j++) {
      tmp = r->head_array[j]->Duplicate(1);
      sp->AddLiteral(tmp);
    }
    while ((l = r->positive.Iterate())) {
      sp->AddLiteral(l);
    }
    
    while ((l = r->negative.Iterate())) {
      sp->AddLiteral(l);
    } 
    choice->split.Insert(sp);
  }
  
  
  return choice;
}


void PriorityData::AddChoice(Rule *r)
{
  choices->Insert(CreateChoice(r));
}

void Choice::UpdateConstants()
{
  long current = ++priority_data.num_choices;
  Literal *l, *tmp;
  int i;
  SpecialLiteral *slt;
  char *st;
  
  Rule *r;

  // new rule instance 
  predicates[priority_data.rule_pred]->AddInstance(&current);

  // store also label
  if (original->label) {
    long idx;
    const char *buf;
    if (!priority_data.label_table) {
      priority_data.label_table = new SymbolTable;
    }
    buf = original->label->GetGroundInstance();
    idx = priority_data.label_table->Insert(buf);
    
    priority_data.labels[idx]->Insert(current);
    
  }
  
  while ((r = split.Iterate())) {
    if (r->type == BASICRULE) {
      l = r->head;
      // change first argument to `current'
      if ((l->pred == priority_data.choice_pred) 
	  || (l->pred == priority_data.satisfied_pred)
	  || (l->pred == priority_data.activate_pred)) {
	l->cons[0] = current;
      }
    } else {
      for (i = 0; i < r->head_number; i++) {
	// all should be choice_preds
	l = r->head_array[i];
	if (!(l->pred == priority_data.choice_pred))
	  int_error("strange priority rule", "");
	l->cons[0] = current;
      }
      // number of choices same as number of heads
      *priority_data.choice_degrees[current] = r->head_number;
      priority_data.total_alternatives += r->head_number;
      st = clone_string(original->GetInstanceString());
      priority_data.original_rules.Append(st,0);


    }
    while ((l = r->positive.Iterate())) {
      if (l->pred == priority_data.choice_pred)
	l->cons[0] = current;
    }
    while ((l = r->negative.Iterate())) {
      if (l->pred == priority_data.choice_pred)
	l->cons[0] = current;
    }
    while (r->special_lits && (tmp = r->special_lits->Iterate())) {
      slt = (SpecialLiteral*) tmp;
      while ((l = slt->tail.Iterate())) {
	if (l->pred == priority_data.choice_pred)
	  l->cons[0] = current;
      }
    }
  }
}

void PriorityData::GroundChoices()
{
  Choice *c;

  if (!choices)
    return;
  
  choices->ClearIterator();
  while ((c = choices->Iterate())) {
    c->original->GroundRule(GRD_RULE);
  }
  predicates[rule_pred]->Emit();
  predicates[satisfied_pred]->Emit();

  predicates[meta_pred]->Emit();
  
  // create positive loops for old_satisfied predicate to force their
  // inclusion
  //    {
  //      Instance it[2];
  //      Literal *l = new Literal(old_satisfied_pred, it, 2, 0);
  //      Rule *r = new Rule(l, BASICRULE, 0);
  //      r->AddLiteral(l);

  //      for (size_t i = 1; i <= num_choices; i++) {
  //        l->cons[0] = i;
  //        for (size_t d = 1; d <= *choice_degrees[i]; d++) {
  //  	l->cons[1] = d;
  //  	r->EmitGround(0,0,0);
  //        }
  //      }
  //    }
  
  // create atoms for metapriority definitions, if necessary
  if (sys_data.use_interactive_priorities) {
    Instance it[2];
    const char *st;
    long a;
    //      Literal *l  = new Literal(meta_pred, it, 2, 0);
    //      Rule *r = new Rule(l, BASICRULE, 0);

    //      predicates[meta_pred]->is_priority_meta_predicate = 0;
    //      r->AddLiteral(l);
    
    for (int i = 1; i <= num_choices; i++) {
      it[0] = i;
      //      l->cons[0] = i;
      for (int j = 1; j <= num_choices; j++) {
	if (i == j) {
	  continue;
	}
	it[1] = j;
	//	l->cons[1] = i;
	//	r->EmitGround(0, 0, 0);
	st = get_ground_instance(it, control_pred, 2);
	a = atom_table->Insert(st) +1;
    
	mp->atom.Insert(a);
	mp->first.Insert(i);
	mp->second.Insert(j);
      }
    }
    predicates[meta_pred]->is_priority_meta_predicate = 1;
  }

}

RestrictType PriorityData::CheckRestrict(int print)
{
  RestrictType result = RT_STRONG, tmp = RT_STRONG;
  Choice *c;
  Rule *rl;
  if (!choices)
    return RT_STRONG;
  choices->ClearIterator();
  
  while ((c = choices->Iterate())) {
    while ((rl = c->split.Iterate())) {
      if (!rl->special)
	tmp = rl->CheckRestrict(print);
      else
	tmp = rl->CheckSpecialRestrict();
      switch (tmp) {
      case RT_NONE:
	result = RT_NONE;
	break;
      case RT_WEAK:
	if (result > RT_NONE)
	  result = RT_WEAK;
	break;
      default:
	break;
      }
    }
    c->original->CheckSpecialRestrict();
    switch (tmp) {
    case RT_NONE:
      result = RT_NONE;
      break;
    case RT_WEAK:
      if (result > RT_NONE)
	result = RT_WEAK;
      break;
    default:
      break;
    }
  }
  return result;
}

void PriorityData::CreateGeneratorRules()
{
  Rule *r;
  Literal *l;
  Variable D, R;

  if (!choices)
    return;

  //  predicates[meta_pred]->SetName(clone_string("*priority"));
  
  D = variable_table->Insert("D");
  R = variable_table->Insert("R");
  
  // define rules and degrees
  long i;
  Instance it[2];
  
  //  for (i = 1; i <= num_choices; i++) {
  //    predicates[rule_pred]->AddInstance(&i);
  //  }
  for (i = 1; i <= max_degree; i++) {
    predicates[degree_pred]->AddInstance(&i);
  }

  // rule is satisfied to degree 1 if not activated
  it[0] = -1;
  it[1] = 1;
  l = new Literal(satisfied_pred, it, 2, 0);
  l->cons[0] = -1;
  l->vars[0] = R;
  l->ground = 0;
  l->numvars = 1;
  l->args[0] = T_VARIABLE;
  
  r = new Rule(l, BASICRULE);
  l = new Literal(R, activate_pred);
  l->negative = 1;
  r->AddLiteral(l);
  l = new Literal(R, rule_pred);
  r->AddLiteral(l);
  predicates[satisfied_pred]->AddRule(r);
    
  
  // a rule is satisfied to degree D if choose(R, D) is true
  l = new Literal(satisfied_pred, it, 2, 0);
  for (i = 0; i < 2; i++) {
    l->cons[i] = -1;
    l->args[i] = T_VARIABLE;
  }
  l->vars[0] = R;
  l->vars[1] = D;
  l->ground = 0;
  l->numvars = 2;

  r = new Rule(l, BASICRULE);

  l = new Literal(R, rule_pred);
  r->AddLiteral(l);

  l = new Literal(D, degree_pred);
  r->AddLiteral(l);
  
  l = new Literal(choice_pred, it, 2, 0);
  for (i = 0; i < 2; i++) {
    l->cons[i] = -1;
    l->args[i] = T_VARIABLE;
  }
  l->vars[0] = R;
  l->vars[1] = D;
  l->ground = 0;
  l->numvars = 2;
  r->AddLiteral(l);
  predicates[satisfied_pred]->AddRule(r);
  
}

void PriorityData::CreateTesterRules()
{
  if (!choices)
    return;
  switch (semantics) {
  case PRI_INCLUSION:
    CreateInclusionTesterRules();
    break;
  case PRI_PARETO:
    CreateParetoTesterRules();
    break;
  case PRI_CARDINALITY:
    CreateCardinalityTesterRules();
    break;
  default:
    int_error("strange priority semantics", "");
  }
  // Create common rules
  Rule *r;
  Literal *l;
  long a1, a2;
  Variable R, D_n, D_o;
  R = variable_table->Insert("R");
  D_n = variable_table->Insert("Dn");
  D_o = variable_table->Insert("Do");
  Function *fun;
  Term *t_1, *t_2;
  Instance it[2];
  const char *st;

  // excused(X) :- ...
  l = new Literal(R, excused_pred);
  r = new Rule(l, BASICRULE);
    
  l = new Literal(meta_pred, it, 2, 0);
  l->args[0] = l->args[1] = T_VARIABLE;
  l->vars[0] = D_n;
  l->vars[1] = R;
  r->AddLiteral(l);
    
  l = new Literal(D_n, better_pred);
  r->AddLiteral(l);
    
  l = new Literal(D_n, rule_pred);
  r->AddLiteral(l);
    
  l = new Literal(R, rule_pred);
  r->AddLiteral(l);
    
  t_1 = new Term(T_VARIABLE, R, 0);
  t_2 = new Term(T_VARIABLE, D_n, 0);
  fun = new Function("neq", t_1, t_2, 0);
  r->AddFunction(fun);
    
  
  tester->Insert(r);
  
  // { old_sat(1,1), ..., old_sat(n, d) }
  r = new Rule(CHOICERULE, total_alternatives);
  long n = 1;
  for (int i = 1; i <= num_choices; i++) {
    it[0] = i;
    for (int d = 1; d <= *choice_degrees[i]; d++) {
      it[1] = d;
      l = new Literal(old_satisfied_pred, it, 2, 0);
      r->AddHead(l);

      // associate *sat and *old_sat
      st = get_ground_instance(it, satisfied_pred, 2);
      a1 = atom_table->Insert(st) + 1;
      *sat_atom[n] = a1;
      st = get_ground_instance(it, old_satisfied_pred, 2);
      a2 = atom_table->Insert(st) + 1;
      *old_sat_atom[n] = a2;
      n++;
    }
  }
  tester->Insert(r);
  // { enable(1,2), ..., enable() }.
  if (sys_data.use_interactive_priorities) {
    r = new Rule(CHOICERULE, num_choices * (num_choices -1));
    for (int i = 1; i <= num_choices; i++) {
      it[0] = i;
      for (int j = 1; j <= num_choices; j++) {
	if (i == j) {
	  continue;
	}
	it[1] = j;
	l = new Literal(control_pred, it, 2, 0);

	r->AddHead(l);
      }
    }
    tester->Insert(r);
  }
  
}

void PriorityData::CreateInclusionTesterRules()
{
  if (!choices)
    return;
  Rule *r;
  Literal *l, *lt;
//  long a1, a2;
  Variable R, D_n, D_o;
  R = variable_table->Insert("R");
  D_n = variable_table->Insert("Dn");
  D_o = variable_table->Insert("Do");
  Function *fun;
  Term *t_1, *t_2;
  Instance it[2];
//  char *st;
  SpecialLiteral *slt;

  // better(D) :-  ...
  l = new Literal(D_n, better_pred);
  r = new Rule(l, BASICRULE);

  it[0] = -1;
  it[1] = -1;
  l = new Literal(satisfied_pred, it, 2, 0);
  l->args[0] = l->args[1] = T_VARIABLE;
  l->vars[0] = R;
  l->vars[1] = D_n;
  r->AddLiteral(l);

  l = new Literal(old_satisfied_pred, it, 2, 0);
  l->args[0] = l->args[1] = T_VARIABLE;
  l->vars[0] = R;
  l->vars[1] = D_o;
  r->AddLiteral(l);

  l = new Literal(R, rule_pred);
  r->AddLiteral(l);
  
  l = new Literal(D_n, degree_pred);
  r->AddLiteral(l);

  l = new Literal(D_o, degree_pred);
  r->AddLiteral(l);

  t_1 = new Term(T_VARIABLE, D_n, 0);
  t_2 = new Term(T_VARIABLE, D_o, 0);
  fun = new Function("gt", t_2, t_1, 0);
  r->AddFunction(fun);
  
  tester->Insert(r);

  // worse(D) :- ...
  l = new Literal(D_o, worse_pred);
  r = new Rule(l, BASICRULE);
  it[0] = -1;
  it[1] = -1;
  l = new Literal(satisfied_pred, it, 2, 0);
  l->args[0] = l->args[1] = T_VARIABLE;
  l->vars[0] = R;
  l->vars[1] = D_n;
  r->AddLiteral(l);

  l = new Literal(old_satisfied_pred, it, 2, 0);
  l->args[0] = l->args[1] = T_VARIABLE;
  l->vars[0] = R;
  l->vars[1] = D_o;
  r->AddLiteral(l);

  l = new Literal(R, rule_pred);
  r->AddLiteral(l);
  
  l = new Literal(D_n, degree_pred);
  r->AddLiteral(l);

  l = new Literal(D_o, degree_pred);
  r->AddLiteral(l);

  t_1 = new Term(T_VARIABLE, D_n, 0);
  t_2 = new Term(T_VARIABLE, D_o, 0);
  fun = new Function("gt", t_1, t_2, 0);
  r->AddFunction(fun);

  l = new Literal(R, excused_pred);
  l->negative = 1;
  r->AddLiteral(l);
  
  tester->Insert(r);
  
  // :- not better
  // better :- { worse(D_2) : degree(D_2) : D_1 < D_2 } 0,
  // better(D_1), degree(D_1).
  it[0] = 0;
  l = new Literal(better_pred, it, 1, -1);
  r = new Rule(false_lit, BASICRULE, 0);

  r->AddLiteral(l->Duplicate(1));
  tester->Insert(r);

  r = new Rule(l, BASICRULE, 1);
  
  t_1 = new Term(T_CONSTANT, 0, 0);
  slt = new SpecialLiteral(CONSTRAINTRULE, 0, t_1, -1);
  l = new Literal(D_n, worse_pred);
  l->conditions = new LiteralList;
  l->condition_func = new FunctionList;
  lt = new Literal(D_n, degree_pred);
  l->conditions->Insert(lt);
  t_1 = new Term(T_VARIABLE, D_n, 0);
  t_2 = new Term(T_VARIABLE, D_o, 0);
  fun = new Function("lt", t_1, t_2, 0);
  l->condition_func->Insert(fun);
  slt->tail.Insert(l);
  slt->num_conditions++;
  slt->num_positive_conditions++;
  
  r->AddLiteral(slt);
  l = new Literal(D_o, better_pred);
  r->AddLiteral(l);
  l = new Literal(D_o, degree_pred);
  r->AddLiteral(l);

  r->CheckSpecialRestrict();
  r->SimplifySpecialRule();

  
  tester->Insert(r);

  // :- worse.
  // :- { better(D_2) : degree(D_2) : D_1 < D_2 } 0, worse(D_1), degree(D_1)
  r = new Rule(false_lit, BASICRULE, 0);
  t_1 = new Term(T_CONSTANT, 0, 0);
  slt = new SpecialLiteral(CONSTRAINTRULE, 0, t_1, -1);
  l = new Literal(D_n, better_pred);
  l->conditions = new LiteralList;
  l->condition_func = new FunctionList;
  lt = new Literal(D_n, degree_pred);
  l->conditions->Insert(lt);
  t_1 = new Term(T_VARIABLE, D_n, 0);
  t_2 = new Term(T_VARIABLE, D_o, 0);
  fun = new Function("lt", t_1, t_2, 0);
  l->condition_func->Insert(fun);
  slt->tail.Insert(l);
  slt->num_conditions++;
  slt->num_positive_conditions++;  
  r->AddLiteral(slt);
  l = new Literal(D_o, worse_pred);
  r->AddLiteral(l);
  l = new Literal(D_o, degree_pred);
  r->AddLiteral(l);

  r->CheckSpecialRestrict();
  r->SimplifySpecialRule();
  tester->Insert(r);
}

void PriorityData::CreateCardinalityTesterRules()
{
  if (!choices)
    return;
  Rule *r;
  Literal *l, *l2;
  SpecialLiteral *slt;
//  long a1, a2;
  Variable R, D, D_n, D_o, N, N_1, N_2;
  R = variable_table->Insert("R");
  D_n = variable_table->Insert("Dn");
  D = variable_table->Insert("D");
  D_o = variable_table->Insert("Do");
  N = variable_table->Insert("N");
  N_1 = variable_table->Insert("N_1");
  N_2 = variable_table->Insert("N_2");
  Function *fun;
  Term *t_1, *t_2;
  Instance it[2];
//  char *st;

  /* possibly_sat(0 .. ||rule(R)||) */
  for (int i = 0; i < choices->Size(); i++) {
    it[0] = i;
    predicates[possibly_sat_pred]->AddInstance(it);
  }

  /* sat_cardinality(D, N) :- N { satisfied( ) : } N */
  it[0] = it[1] = -1;
  l = new Literal(sat_card_pred, it, 2, 0);
  l->args[0] = l->args[1] = T_VARIABLE;
  l->vars[0] = D; l->vars[1] = N;

  r = new Rule(l, BASICRULE);

  t_1 = new Term(T_VARIABLE, N, 0);
  slt = new SpecialLiteral(CONSTRAINTRULE, t_1, t_1->Duplicate(), 0);

  l = new Literal(satisfied_pred, it, 2, 0);
  l->args[0] = l->args[1] = T_VARIABLE;
  l->vars[0] = R; l->vars[1] = D;
  l2 = new Literal(R, rule_pred);
  l->conditions = new LiteralList;
  l->condition_func = new FunctionList;
  l->conditions->Insert(l2);
  l->has_global_condition = 1;
  slt->tail.Insert(l);
  slt->AddVariables();
  slt->num_conditions++;
  slt->num_positive_conditions++;


  r->AddLiteral(slt);
  //  r->num_conditions++;

  l = new Literal(N, possibly_sat_pred);
  r->AddLiteral(l);

  l = new Literal(D, degree_pred);
  r->AddLiteral(l);
  r->CheckSpecialRestrict();
  r->SimplifySpecialRule();  
  tester->Insert(r);

  /* old_sat_cardinality(D, N) :- ... */
  it[0] = it[1] = -1;
  l = new Literal(old_sat_card_pred, it, 2, 0);
  l->args[0] = l->args[1] = T_VARIABLE;
  l->vars[0] = D; l->vars[1] = N;

  r = new Rule(l, BASICRULE);

  t_1 = new Term(T_VARIABLE, N, 0);
  slt = new SpecialLiteral(CONSTRAINTRULE, t_1, t_1->Duplicate(), 0);

  l = new Literal(old_satisfied_pred, it, 2, 0);
  l->args[0] = l->args[1] = T_VARIABLE;
  l->vars[0] = R; l->vars[1] = D;
  l2 = new Literal(R, rule_pred);
  l->conditions = new LiteralList;
  l->condition_func = new FunctionList;
  l->conditions->Insert(l2);
  slt->num_conditions++;
  slt->num_positive_conditions++;

  l->has_global_condition = 1;
  //  r->num_conditions++;
  slt->tail.Insert(l);
  r->AddLiteral(slt);
  l = new Literal(N, possibly_sat_pred);
  r->AddLiteral(l);

  l = new Literal(D, degree_pred);
  r->AddLiteral(l);

  r->CheckSpecialRestrict();
  r->SimplifySpecialRule();
  
  tester->Insert(r);

  /* better(D_1) :-  ... */
  l = new Literal(D, better_pred);
  r = new Rule(l, BASICRULE);

  it[0] = it[1] = -1;
  l = new Literal(sat_card_pred, it, 2, 0);
  l->args[0] = l->args[1] = T_VARIABLE;
  l->vars[0] = D;
  l->vars[1] = N_1;
  r->AddLiteral(l);

  l = new Literal(old_sat_card_pred, it, 2, 0);
  l->args[0] = l->args[1] = T_VARIABLE;
  l->vars[0] = D;
  l->vars[1] = N_2;
  r->AddLiteral(l);

  l = new Literal(N_1, possibly_sat_pred);
  r->AddLiteral(l);

  l = new Literal(N_2, possibly_sat_pred);
  r->AddLiteral(l);
  
  l = new Literal(D, degree_pred);
  r->AddLiteral(l);
  
  t_1 = new Term(T_VARIABLE, N_1, 0);
  t_2 = new Term(T_VARIABLE, N_2, 0);
  fun = new Function("gt", t_1, t_2, 0);
  r->AddFunction(fun);
  tester->Insert(r);
  /* worse(D_2) :- ... */

  l = new Literal(D, worse_pred);
  r = new Rule(l, BASICRULE);

  it[0] = it[1] = -1;
  l = new Literal(sat_card_pred, it, 2, 0);
  l->args[0] = l->args[1] = T_VARIABLE;
  l->vars[0] = D;
  l->vars[1] = N_1;
  r->AddLiteral(l);

  l = new Literal(old_sat_card_pred, it, 2, 0);
  l->args[0] = l->args[1] = T_VARIABLE;
  l->vars[0] = D;
  l->vars[1] = N_2;
  r->AddLiteral(l);

  l = new Literal(N_1, possibly_sat_pred);
  r->AddLiteral(l);

  l = new Literal(N_2, possibly_sat_pred);
  r->AddLiteral(l);
  
  l = new Literal(D, degree_pred);
  r->AddLiteral(l);
  
  t_1 = new Term(T_VARIABLE, N_1, 0);
  t_2 = new Term(T_VARIABLE, N_2, 0);
  fun = new Function("lt", t_1, t_2, 0);
  r->AddFunction(fun);
  tester->Insert(r);

  it[0] = 0;
  l = new Literal(better_pred, it, 1, -1);
  r = new Rule(false_lit, BASICRULE, 0);

  r->AddLiteral(l->Duplicate(1));
  tester->Insert(r);

  r = new Rule(l, BASICRULE, 1);
  
  t_1 = new Term(T_CONSTANT, 0, 0);
  slt = new SpecialLiteral(CONSTRAINTRULE, 0, t_1, -1);
  l = new Literal(D_n, worse_pred);
  l->conditions = new LiteralList;
  l->condition_func = new FunctionList;
  Literal *lt = new Literal(D_n, degree_pred);
  l->conditions->Insert(lt);
  t_1 = new Term(T_VARIABLE, D_n, 0);
  t_2 = new Term(T_VARIABLE, D_o, 0);
  fun = new Function("lt", t_1, t_2, 0);
  l->condition_func->Insert(fun);
  slt->tail.Insert(l);
  slt->num_conditions++;
  slt->num_positive_conditions++;
  r->AddLiteral(slt);
  l = new Literal(D_o, better_pred);
  r->AddLiteral(l);
  l = new Literal(D_o, degree_pred);
  r->AddLiteral(l);

  r->CheckSpecialRestrict();
  r->SimplifySpecialRule();

  
  tester->Insert(r);

  // :- worse.
  // :- { better(D_2) : degree(D_2) : D_1 < D_2 } 0, worse(D_1), degree(D_1)
  r = new Rule(false_lit, BASICRULE, 0);
  t_1 = new Term(T_CONSTANT, 0, 0);
  slt = new SpecialLiteral(CONSTRAINTRULE, 0, t_1, -1);
  l = new Literal(D_n, better_pred);
  l->conditions = new LiteralList;
  l->condition_func = new FunctionList;
  lt = new Literal(D_n, degree_pred);
  l->conditions->Insert(lt);
  t_1 = new Term(T_VARIABLE, D_n, 0);
  t_2 = new Term(T_VARIABLE, D_o, 0);
  fun = new Function("lt", t_1, t_2, 0);
  l->condition_func->Insert(fun);
  slt->tail.Insert(l);
  slt->num_conditions++;
  slt->num_positive_conditions++;
  r->AddLiteral(slt);
  l = new Literal(D_o, worse_pred);
  r->AddLiteral(l);
  l = new Literal(D_o, degree_pred);
  r->AddLiteral(l);
  
  r->CheckSpecialRestrict();
  r->SimplifySpecialRule();
  tester->Insert(r);
    
}

void PriorityData::CreateParetoTesterRules()
{
  if (!choices)
    return;
  Rule *r;
  Literal *l;
//  long a1, a2;
  Variable R, D_n, D_o;
  R = variable_table->Insert("R");
  D_n = variable_table->Insert("Dn");
  D_o = variable_table->Insert("Do");
  Function *fun;
  Term *t_1, *t_2;
  Instance it[2];
//  char *st;

  // better :- ... .
  l = new Literal(R, better_pred);
  r = new Rule(l, BASICRULE);

  it[0] = -1;
  it[1] = -1;
  l = new Literal(satisfied_pred, it, 2, 0);
  l->args[0] = l->args[1] = T_VARIABLE;
  l->vars[0] = R;
  l->vars[1] = D_n;
  r->AddLiteral(l);

  l = new Literal(old_satisfied_pred, it, 2, 0);
  l->args[0] = l->args[1] = T_VARIABLE;
  l->vars[0] = R;
  l->vars[1] = D_o;
  r->AddLiteral(l);

  l = new Literal(R, rule_pred);
  r->AddLiteral(l);
  
  l = new Literal(D_n, degree_pred);
  r->AddLiteral(l);

  l = new Literal(D_o, degree_pred);
  r->AddLiteral(l);

  t_1 = new Term(T_VARIABLE, D_n, 0);
  t_2 = new Term(T_VARIABLE, D_o, 0);
  fun = new Function("gt", t_2, t_1, 0);
  r->AddFunction(fun);
  
  tester->Insert(r);

  // worse :- ...
  l = new Literal(R, worse_pred);
  r = new Rule(l, BASICRULE);
  it[0] = -1;
  it[1] = -1;
  l = new Literal(satisfied_pred, it, 2, 0);
  l->args[0] = l->args[1] = T_VARIABLE;
  l->vars[0] = R;
  l->vars[1] = D_n;
  r->AddLiteral(l);

  l = new Literal(old_satisfied_pred, it, 2, 0);
  l->args[0] = l->args[1] = T_VARIABLE;
  l->vars[0] = R;
  l->vars[1] = D_o;
  r->AddLiteral(l);

  l = new Literal(R, rule_pred);
  r->AddLiteral(l);
  
  l = new Literal(D_n, degree_pred);
  r->AddLiteral(l);

  l = new Literal(D_o, degree_pred);
  r->AddLiteral(l);

  t_1 = new Term(T_VARIABLE, D_n, 0);
  t_2 = new Term(T_VARIABLE, D_o, 0);
  fun = new Function("gt", t_1, t_2, 0);
  r->AddFunction(fun);

  l = new Literal(R, excused_pred);
  l->negative = 1;
  r->AddLiteral(l);
  
  tester->Insert(r);
  
  
  // :- not better
  r = new Rule(false_lit, BASICRULE, 0);
  t_1 = new Term(T_CONSTANT, num_choices, 0);
  SpecialLiteral *slt = new SpecialLiteral(CONSTRAINTRULE, t_1, 0, -1);
  for (int i = 1; i <= num_choices; i++) {
    it[0] = i;
    l = new Literal(better_pred, it, 1, 1);
    slt->tail.Insert(l);
  }
  r->AddLiteral(slt);
  tester->Insert(r);

  // :- worse.
  r = new Rule(false_lit, BASICRULE, 0);
  t_1 = new Term(T_CONSTANT, 1, 0);
  slt = new SpecialLiteral(CONSTRAINTRULE, t_1, 0, -1);
  for (int i = 1; i <= num_choices; i++) {
    it[0] = i;
    l = new Literal(worse_pred, it, 1, 0);
    slt->tail.Insert(l);
  }
  r->AddLiteral(slt);
  tester->Insert(r);

}

void PriorityData::GroundTester()
{
  Rule *r;

  if (!tester)
    return;
  if (sys_data.emit_text) {
    output(sys_data.output_file, "%% Tester rules begin here\n");
  }

  
  tester->ClearIterator();
  while ((r = tester->Iterate())) {
    r->GroundRule(GRD_RULE);
  }
  predicates[meta_pred]->Emit();
  if (sys_data.use_interactive_priorities) {
    predicates[meta_pred]->is_priority_meta_predicate = 0;
    Instance it[2];
    Literal *h = new Literal(meta_pred, it, 2, 0);
    Literal *l = new Literal(control_pred, it, 2, 0);
    Rule *r = new Rule(h, BASICRULE);
    r->AddLiteral(l);
    for (int i = 1; i <= num_choices; i++) {
      h->cons[0] = i;
      l->cons[0] = i;
      for (int j = 1; j <= num_choices; j++) {
	if (i == j) {
	  continue;
	}
	h->cons[1] = j;
	l->cons[1] = j;
	r->EmitGround(0, 0,0);
      }
    }
    predicates[meta_pred]->is_priority_meta_predicate = 1;
  }

  if (sys_data.emit_text) {
    output(sys_data.output_file, "%% Total number of "
	    "choices: %ld\n", num_choices);
  } else {
    output(sys_data.output_file, "0\n");
    output(sys_data.output_file, "%ld\n", num_choices);
  }

  if (sys_data.emit_text) {
    output(sys_data.output_file, "%% Total number of "
	    "alternatives: %ld\n", total_alternatives);
  } else {
    output(sys_data.output_file, "%ld\n", total_alternatives);
  }
  if (sys_data.emit_text) {
    output(sys_data.output_file, "%% Interactive metapriorities "
	    "in use: %s\n", sys_data.use_interactive_priorities ? "true" :
	    "false"); 
  }  else {
    output(sys_data.output_file, "%ld\n",
	    sys_data.use_interactive_priorities);
  }
  if (sys_data.emit_text) {
    output(sys_data.output_file, "%% Rules corresponding to original choices\n");
  }
  long n = 1;
  char *st;

  original_rules.ClearIterator();
  while ((st = original_rules.Iterate())) {
    output(sys_data.output_file, "%ld %ld %s\n",
	    n,
	    *choice_degrees[n],
	    st);
    n++;
  }
  output(sys_data.output_file, "0\n");
  // Connections between *sat and *old_sat
  if (!sys_data.emit_text) {
    for (n = 1; n <= total_alternatives; n++) {
      output(sys_data.output_file, "%s %s\n",
	      atom_table->symbols[*sat_atom[n]-1],
	      atom_table->symbols[*old_sat_atom[n]-1]);
    }
  }
  output(sys_data.output_file, "0\n");
  // output connections between priorities and atoms
  if (sys_data.use_interactive_priorities && !sys_data.emit_text) {
    long f, s;
    while ((n = mp->atom.Iterate())) {
      f = mp->first.Iterate();
      s = mp->second.Iterate();
      output(sys_data.output_file, "%s %ld %ld\n",
	      atom_table->symbols[n-1], f, s);
    }
  }
  output(sys_data.output_file, "0\n");  
  //    if (label_table) {
  //      //    label_table->CreateSymbolArray();
  //      long ch;
  //      for (n = 0; n < label_table->Size(); n++) {
  //        output(sys_data.output_file, "%s: ", label_table->symbols[n]);
  //        while ((ch = labels[n]->Iterate())) {
  //  	output(sys_data.output_file, "%ld ", ch);
  //        }
  //        output(sys_data.output_file, "\n");
  //      }
  //    }
  
}
