// rule.cc -- implementation for rules
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
#include <limits.h>

#include <stdlib.h>

#ifndef GLOBAL_H
#include "global.h"
#endif
#ifndef RULE_H
#include "rule.h"
#endif
#ifndef SYMBOL_H
#include "symbol.h"
#endif
#ifndef LITERAL_H
#include "literal.h"
#endif
#ifndef LIST_H
#include "list.h"
#endif
#ifndef GRAPH_H
#include "graph.h"
#endif
#ifndef SYMBOL_H
#include "symbol.h"
#endif
#ifndef PREDICATE_H
#include "predicate.h"
#endif
#ifndef ITERATE_H
#include "iterate.h"
#endif
#ifndef ERROR_H
#include "error.h"
#endif
#include "term.h"

void clear_pos(int pos)
{
  int i;
  for (i = 0; i < sys_data.total_vars; i++) {
    if (var_pos[i] >= pos) {
      variables[i] = ERROR_NUMBER;
      var_pos[i] = ERROR_NUMBER;
    }
  }
}

int Rule::max_tail = 1;

Rule::Rule(Literal *lt, RuleType rt, int dlt)
  : positive(UNORDERED), negative(UNORDERED),
    functions(UNORDERED), positive_conditions(UNORDERED),
    negative_conditions(UNORDERED), head_conditions(UNORDERED)   
{
  Literal *tmp = NULL;
  Function *fun = NULL;
  special_lits = NULL;
  special = 0;
  choice = 0;
  restrict_status  = RT_UNKNOWN;
  head = lt;
  line_end = lt->lineno;
  line_start = lt->lineno;
  type = rt;
  h_pos = 0;
  fact = 0;
  transitive_domain = 0;
  weight_head = 0;
  deleted = 0;
  vars = NULL;
  atleast = atmost = NULL;
  head_number = 1;
  max_head = 1;
  head_array = NULL;
  num_weights = 0;
  num_assigns = 0;
  delete_all = dlt;
  num_conditions = 0;
  first_ground = 0;
  num_negative_conditions = 0;
  num_positive_conditions = 0;
  num_head_conditions = 0;
  num_positive_externals = 0;
  num_negative_externals = 0;
  num_internal_literals = 0;
  num_negative_internal_literals = 0;
  print_str = 0;
  sorted = 0;
  if (lt->ground)
    ground = 1;
  else
    ground = 0;
  
  if (lt->NeedsSimplifying())
    SimplifyLiteral(lt);

  if (type == OPTIMIZERULE)
    return;
  
  // if there are conditions for head atom, transfer them to the tail:
  //   bar(X):foo(X) :- baz(X)  ===> bar(X) :- foo(X), baz(X).
  if (lt->conditions) {
    while ((tmp = lt->conditions->Iterate())) {
      AddLiteral(tmp);

    }
    // clear the information so it won't interfere with range
    // restriction checking
    delete lt->conditions;
    lt->conditions = NULL;
  }
  if (lt->condition_func) {
    while ((fun = lt->condition_func->Iterate())) {
      AddFunction(fun);
    }
    delete lt->condition_func;
    lt->condition_func = NULL;
  }
  choice = 0;
  label = 0;

}

Rule::Rule(RuleType rt, int initial_head) // for multiple heads
  : positive(UNORDERED), negative(UNORDERED), 
    functions(UNORDERED), positive_conditions(UNORDERED),
    negative_conditions(UNORDERED), head_conditions(UNORDERED)   
{
  assert((rt == CHOICERULE) || (rt == ORDERED_DISJUNCTION)
         || (rt == DISJUNCTIVERULE)); 
  restrict_status = RT_UNKNOWN;
  choice = 0;
  head = NULL;
  special_lits = NULL;
  special = 1;
  weight_head = 0;
  num_weights = 0;
  fact = 0;
  num_conditions = 0;
  num_head_conditions = 0;
  num_negative_conditions = 0;
  num_positive_conditions = 0;
  num_internal_literals = 0;
  num_negative_internal_literals = 0;
  sorted = 0;
  first_ground = 0;
  num_ground = 0;
  vars = NULL;
  transitive_domain = 0;
  if (initial_head <= 0)
    initial_head = 2;
  head_array = new Literal*[initial_head];
  for (int i = 0; i < initial_head; i++) {
    head_array[i] = 0;
  }
  line_end = 0;
  h_pos = 0;
  line_start = 0;
  type = rt;
  atleast = atmost = NULL;
  deleted = 0;
  delete_all = 1;
  head_number = 0;
  max_head = initial_head;
  ground = 1;
  num_assigns = 0;
  choice = 0;
  label = 0;
}

Rule::~Rule()
{
  Literal *lt = NULL;
  Function *fun = NULL;
  int i;

  //  cerr << "FOO" << endl;
  if (delete_all) {
    if (head && head != false_lit)
      delete head;
    while (( lt = positive.Iterate())) {
      if (!lt->special) {
        if (lt != false_lit) {
          delete lt;
        }
      } else {
        delete (SpecialLiteral*) lt;
      }
    }
    while (( lt = negative.Iterate())) {
      if (lt != false_lit)
        delete lt;
    }
    while (( fun = functions.Iterate())) {
      delete fun;
    }
  }
  if (head_array) {
    for (i = 0; i <head_number; i++) {
      if (head_array[i] && head_array[i] != false_lit)
        delete head_array[i];
    }
    delete [] head_array;
  }

  if (special_lits)
    delete special_lits;
  
  if (vars)
    delete [] vars;

  if (choice)
    delete choice;


}
  

void Rule::SimplifySpecialRule()
{
  Literal *lt = NULL;
  SpecialLiteral *sp = NULL;
  
  ExpandAllConditions();
  //   FindWeights();
  
  while ((lt = positive.Iterate())) {
    if (lt->special) {
      sp = (SpecialLiteral*) lt;
      if (sp->type == WEIGHTRULE)
        num_weights++;
      sp->AddVariables();
    }
  }

  while (special_lits && (sp = (SpecialLiteral*) special_lits->Iterate())) {
    if (sp->pred2 >= 0)
      AddLiteral(sp->atmost_lit);
  }
  
  // make all variables global by constructing new domain
  // predicates for each variable
  RemoveLocalVars();
  
  if (type == CHOICERULE) {
    ExpandChoiceRule();
  }
}

void Rule::ExpandChoiceRule()
{
  Rule *rl = NULL;
  Literal *lt = NULL;
  SpecialLiteral *slt = NULL;
  Function *fun = NULL;
  int i = 0;
  int numvars = 0;
  RuleType tp = CONSTRAINTRULE;
  Term *bound = NULL;
  
  if (weight_head) {
    tp = WEIGHTRULE;
  }

  SortFunctions();


  
  // we must check if any condition expansion made the rule a ground one
  if (atmost && ((atmost->type != T_CONSTANT) ||
                 (atmost->val != ERROR_NUMBER))) {
    // if there is an upper bound to the number of true heads
    // construct the rule like this:
    //  { a1, ..., an } m :- body.
    // ===> :- m +1 { a1, ..., an }, body.

    if (!false_lit) {
      false_lit = Predicate::DefineFalseLiteral();
    }
    
    rl = new Rule(false_lit, BASICRULE);

    if (!rl)
      error(SYS_ERR, "malloc error");

    if (sys_data.print_debug) {
      int pos;
      ostringstream ss;

      rl->print_str = new char[strlen(print_str)+6];
      pos = output(ss, "%s %%[ub]", print_str);
      rl->print_str = clone_string(ss.str().c_str());
    }
    
    // count the new bound expression
    if (atmost->type == T_CONSTANT) {
      bound = new Term(T_CONSTANT, atmost->val+1, atmost->lineno);
    } else {
      Term *tmp = new Term(T_CONSTANT, 1, line_start);
      bound = new Function("plus", atmost->Duplicate(), tmp,
                           line_start);
    }
    slt = new SpecialLiteral(tp, bound, NULL, line_start);
    if (!slt)
      error(SYS_ERR, "malloc error");

    for (i = 0; i < head_number; i++) {
      if (!head_array[i]->deleted) {
        lt = head_array[i];
        numvars += lt->numvars;
        slt->tail.Insert( lt->Duplicate(0));
        if (lt->conditions || lt->condition_func) {
          slt->num_conditions++;
          rl->num_conditions++;
          if (lt ->negative)
            slt->num_negative_conditions++;
          else
            slt->num_positive_conditions++;
        }
      }
    }
    slt->AddVariables();
    rl->AddLiteral(slt);
    while ((lt = positive.Iterate())) {
      rl->AddLiteral(lt);
      numvars += lt->numvars;
    }
    while ((lt = negative.Iterate())) {
      rl->AddLiteral(lt);
      numvars += lt->numvars;
    }
    while ((fun = functions.Iterate())) {
      rl->AddFunction(fun);
      //      numvars += fun->numvars;
    }
    if (!ground && !numvars)
      ground = 1;

    rl->SortFunctions();
    predicates[false_lit->pred]->AddSpecialRule(rl);
  }

  if (atleast) {
    // the rules of form:
    // m { a1, ..., an } :- body
    //    ====> :- n-m +1{ not a1, ..., not an }

    if (!false_lit) {
      false_lit = Predicate::DefineFalseLiteral();
    }
    
    rl = new Rule(false_lit, BASICRULE);
    if (!rl)
      error(SYS_ERR, "malloc error");

    if (sys_data.print_debug) {
      //      int pos;
      ostringstream ss;
      output(ss,  "%s %%[lb]", print_str);
      rl->print_str = clone_string(ss.str().c_str());
    }
    
    if (num_head_conditions == 0) {
      if (!weight_head) {
        long val = head_number + 1 - deleted;
	
	
        // count the new bound expression
        if (atleast->type == T_CONSTANT) {
          bound = new Term(T_CONSTANT, val - atleast->val, atleast->lineno);
        } else {
          Term *tmp = new Term(T_CONSTANT, val, line_start);
          bound = new Function("minus", tmp, atleast->Duplicate(), 
                               line_start);
        }
      } else { 	// weight head
        bound = atleast->Duplicate();
      }
    } else {
      bound = atleast->Duplicate();
    }
    
  
    slt = new SpecialLiteral(tp, bound, NULL, line_start);
    //			     head_number - atleast +1 -deleted,
    //		     ERROR_NUMBER);
    if (!slt)
      error(SYS_ERR, "malloc error");

    if (num_head_conditions > 0) {
      slt->dynamic_atleast = 1;
    }
    if (weight_head) {
      slt->weight_atleast = 1;
    }
    
    for (i = 0; i < head_number; i++) {
      if (!head_array[i]->deleted) {
        Literal *tmp_literal =  head_array[i];
        lt = tmp_literal->Duplicate(0);
        if (lt->negative) {
          lt->negative = 0;
        } else {
          lt->negative = 1;
        }
        slt->tail.Insert(lt);
        if (lt->conditions || lt->condition_func) {
          slt->num_conditions++;
          rl->num_conditions++;
          if (lt->negative)
            slt->num_negative_conditions++;
          else
            slt->num_positive_conditions++;
        }
      }
    }


    //      if (weight_head) {
    //        // create a dummy literal that ensures that there is at least one
    //        // existing atom with a weight in the weight rule tail.
    //        if (!dummy_weight) {
    //  	long p = Predicate::DefineSystemPredicate();
    //  	dummy_weight = new Literal(p, NULL, 0, 0);
    //  	dummy_weight->weight = new Weight();
    //  	dummy_weight->weight->v.val = sys_data.default_weight;
    //  	predicates[p]->SetStatus(DM_FALSE);
    //        }
    //        slt->tail.Insert(dummy_weight->Duplicate());
    //    }
    

    slt->AddVariables();

    rl->AddLiteral(slt);
    while ((lt = positive.Iterate()))
      rl->AddLiteral(lt);
    while ((lt = negative.Iterate()))
      rl->AddLiteral(lt);
    while ((fun = functions.Iterate()))
      rl->AddFunction(fun);

    // finally, transform all negative literals in a choice rule head
    // to positive literals.
    for (i = 0; i < head_number; i++) {
      if (head_array[i]->negative)
        head_array[i]->negative = 0;
    }


    rl->SortFunctions();    
    predicates[false_lit->pred]->AddSpecialRule(rl);
    
    
  }
}
  
    
void Rule::SimplifyLiteral(Literal *lt)
{
  // go through all arguments, generate a new variable and a new
  // predicate for each disjunctive range found and generate a new
  // variable and a function term for every function found. The new
  // term will be added as a condition to literal in question 

  //  int pos = 0;
  long i = 0;
  long var = ERROR_NUMBER, pred = -1;
  Literal *new_lt = NULL;
  Literal *lt2 = NULL;
  LiteralList to_be_added;
  Term *nt1 = NULL;
  Function *nt2 = NULL;
  Function *fun = NULL;
  InstFunc fp;
  Range * r;
  //  Function *fun;

  static char *varname = NULL;

  //   if (!varname) {
  //     varname = new char[RANGE_MAX_NAME];
  //     if (!varname)
  //       error(SYS_ERR, "malloc error");
  //   }

  // recursively simplify conditions, if they exist
  if (lt->conditions) {
    while ((lt2 = lt->conditions->Iterate())) {
      if (lt2->NeedsSimplifying()) {
        SimplifyLiteral(lt2);
        Literal *tmp = 0;
        while (lt2->conditions && (tmp = lt2->conditions->Iterate())) {
          to_be_added.Insert(tmp);
        }
        while (lt2->condition_func &&
               (fun = lt2->condition_func->Iterate())) {
          lt->condition_func->Insert(fun);
        }
        lt2->conditions = 0;
        lt2->condition_func = 0;
      }
    }
    while ((lt2 = to_be_added.Iterate())) {
      lt->conditions->Insert(lt2);
    }
  }

  ostringstream ss;
  for (i = 0; i < lt->arity; i++) {
    switch(lt->args[i]) {
    case T_RANGE:
      if (sys_data.num_ranges >= RANGE_MAX_NUMBER)
        int_error("maximum number of ranges exceeded", "");
      ss.str("");
      // get the right range number
      output(ss, "I_Range&%d", sys_data.num_ranges);
      var = variable_table->Insert(ss.str().c_str());
      if (var < 0)
        int_error("cannot generate range variable '%s'", varname);
      lt->args[i] = T_VARIABLE;
      lt->vars[i] = var;
      lt->ground = 0;
      pred = Predicate::DefineRangePredicate();
      predicates[pred]->SetArity(1);

      r = (Range*) lt->terms[i];
      predicates[pred]->AddRange(r);
      r->AddDependencies(pred);

      // and finally construct a matching literal and add it
      new_lt = new Literal(var, pred);
      if (!new_lt)
        error(SYS_ERR, "malloc error");
      new_lt->lineno = line_start;
      if (!lt->conditions) {
      	lt->conditions = new LiteralList;
      	lt->condition_func = new FunctionList;
      }
      lt->conditions->Append(new_lt);
      dependency_graph->AddEdge(lt->pred, pred, false);
      //      AddLiteral(new_lt);
      // if the rule was ground rule, it is not it anymore. 
      if (ground)
        ground = 0;
      break;
    case T_FUNCTION:
      var = DefineSystemVariable();
      if (var < 0)
        int_error("cannot generate function argument '%s'",
                  varname);
      lt->args[i] = T_VARIABLE;
      lt->vars[i] = var;
      lt->ground = 0;
      // construct the new function
      fp = function_table->GetFunction("assign");
      nt1 = new Term(T_VARIABLE, var, line_start);
      nt2 = new Function(fp, "assign", 2, line_start);
      nt2->AddArgument(nt1);
      nt2->AddArgument(lt->terms[i]);
      lt->terms[i] = NULL; // don't delete the function here, it is
      // needed later.
      if (nt2->has_range)
        RestrictFunction(nt2);

      if (!lt->conditions) {
      	lt->conditions = new LiteralList;
      	lt->condition_func = new FunctionList;
      }
      // remove nested assigns 
      if (nt2->args[1]->has_function) {
        Function *f = (Function*) nt2->args[1];
        if (f->assign) {
          Function *assign_fun = f->DuplicateFunction();
          // AddFunction(nt2);
          lt->condition_func->Append(assign_fun);
        }
      }
	
      lt->condition_func->Append(nt2);
      
      //      AddFunction(nt2);
      
      // if the rule was ground it will be anymore... 
      if (ground)
        ground = 0;
      break;
    case T_ATOM:
      var = DefineSystemVariable();
      if (var < 0)
        int_error("cannot generate literal argument '%s'",
                  varname);
      lt->args[i] = T_VARIABLE;
      lt->vars[i] = var;
      lt->ground = 0;
      lt2 = ((LiteralTerm*)lt->terms[i])->lt;
      if (lt2->NeedsSimplifying()) {
        SimplifyLiteral(lt2);
      }
      
      // construct the new assign for the atom
      fp = function_table->GetFunction("assign");
      nt1 = new Term(T_VARIABLE, var, line_start);
      nt2 = new Function(fp, "assign", 2, line_start);
      nt2->AddArgument(nt1);
      nt2->AddArgument(lt->terms[i]);
      lt->terms[i] = NULL;
      
      if (!lt->condition_func) {
      	lt->condition_func = new FunctionList();
      	lt->conditions = new LiteralList();
      }
      if (lt2->condition_func) {
        while ((fun = lt2->condition_func->Iterate())) {
          lt->condition_func->Append(fun);
        }
        lt2->condition_func = NULL;
      	lt2->conditions = NULL;
      }

      // remove nested assigns 
      //      if (nt2->args[1]->has_function) {
      //	Function *f = (Function*) nt2->args[1];
      //	if (f->assign) {
      //	  Function *assign_fun = f->DuplicateFunction();
      //	  // AddFunction(nt2);
      //	  lt->condition_func->Append(assign_fun);
      //	}
      //      }
	

      
      lt->condition_func->Append(nt2);
      // AddFunction((Function*)nt2, 1);
      if (ground)
        ground = 0;
      break;
    default:
      break;
    }
  }
}
    
    

void Rule::AddLiteral(Literal *lt)
{
  predicates[lt->pred]->is_only_symbolic_function = 0;
  if (predicates[lt->pred]->Status() == DM_INTERNAL) {
    num_internal_literals++;
    if (lt->negative)
      num_negative_internal_literals++;
  }
  
  if (lt->special) {
    special++;
    if (!special_lits)
      special_lits = new LiteralList;
    if (!special_lits)
      error(SYS_ERR, "malloc error");
    special_lits->Insert(lt);
    if (((SpecialLiteral*)lt)->num_conditions > 0) {
      num_conditions++;
    }
  }

  if (!lt->ground) {
    ground = 0;
  }

  
  if (lt->NeedsSimplifying())
    SimplifyLiteral(lt);
  
  if (lt->negative)
    negative.Insert(lt);
  else {
    positive.Insert(lt);
    if (positive.Size() > max_tail) {
      max_tail = positive.Size();
    }
  }

  if (line_start < 1) {
    line_start = lt->lineno;
  }
  if (line_end < lt->lineno) {
    line_end = lt->lineno;
  }
  if (type == OPTIMIZERULE)
    return;

  
  // If the literal has functions in the condition and no literals,
  // flatten them into usual functions.
  if (lt->condition_func) {
    if ((lt->condition_func->Size() > 0) &&
        (lt->conditions->Size() == 0)) {
      Function *fun = 0;
      while ((fun = lt->condition_func->Iterate())) {
        AddFunction(fun);
      }
      delete lt->condition_func;
      delete lt->conditions;
      lt->condition_func = 0;
      lt->conditions = 0;
    }
  }


  
  if (lt->conditions) {
    num_conditions++;
    if (lt->negative)
      num_negative_conditions++;
    else
      num_positive_conditions++;
  }
  // check for conditions, and add them as normal literals if this is
  // a basic rule
  //  if (lt->conditions) {
  // while ((tmp = lt->conditions->Iterate())) {
  //   if (type == BASICRULE) 
  //	AddLiteral(tmp);
  // }
  // delete lt->conditions;
  // lt->conditions = NULL;
  // }
  //if (lt->condition_func) {
  //  while ((fun = lt->condition_func->Iterate())) {
  //    if (type == BASICRULE) 
  //	AddFunction(fun);
  //  }
  //  delete lt->condition_func;
  // lt->condition_func = NULL;
  // }
}
  
void Rule::SimplifyAtomsFromFunctions(Function *f)
{
  int i;
  Function *fun = NULL;
  Literal *lt = NULL;
  
  for (i = 0; i < f->arity; i++) {
    switch (f->args[i]->type) {
    case T_CONSTANT:
    case T_VARIABLE:
    case T_RANGE:
      break;
    case T_FUNCTION:
      SimplifyAtomsFromFunctions( (Function*)f->args[i]);
      break;
    case T_ATOM:
      if (!strcmp(f->name, "weight")) {
        lt = ((LiteralTerm*)f->args[0])->lt;
	
        if (lt->NeedsSimplifying()) {
          SimplifyLiteral(lt);
	  
          if (lt->conditions->Size() > 0) {
            error(USR_ERR, "%s: You may not use a condition inside a "
                  "weight function call", error_file_and_line(f->lineno));
          }
          while ((fun = lt->condition_func->Iterate())) {
            AddFunction(fun, 1);
          }
          lt->conditions = NULL;
          lt->condition_func = NULL;
        }
      }
      break;
    default:
      int_error("no, not this", "");
      break;
    }
  }
}

void Rule::AddFunction(Function *t, int start)
{
  if (!t->ground) {
    ground = 0;
  }

  if (t->has_atom) {
    SimplifyAtomsFromFunctions(t);
  }
  

  if (!start)
    functions.Append(t);
  else
    functions.Insert(t);
  if (!t->assign) {
    if (positive.Size() + functions.Size() > max_tail) {
      max_tail = positive.Size() + functions.Size();
    }
  } else {
    num_assigns++;
  }
  if (line_end < t->lineno)
    line_end = t->lineno;
}

void Rule::AddHead(Literal *lt)
{
  assert(lt);

  if (lt->NeedsSimplifying())
    SimplifyLiteral(lt);
  
  if ((line_start == 0) || (line_start > lt->lineno))
    line_start = lt->lineno;
  if (line_end < lt->lineno)
    line_end = lt->lineno;
  
  if (!lt->ground)
    ground = 0;

  if (lt->conditions || lt->condition_func) {
    num_conditions++;
    num_head_conditions++;
  }
  
  head_array[head_number++] = lt;

  if (head_number == max_head) {
    Literal **hds = new Literal*[2*max_head];
    int i;
    if (!hds)
      error(SYS_ERR, "malloc error");
    for (i = 0; i < head_number; i++) {
      hds[i] = head_array[i];
    }
    for ( ; i < 2*max_head; i++) {
      hds[i] = NULL;
    }
    delete [] head_array;
    head_array = hds;
    max_head *= 2;
  }
  
  return;
}

void Rule::ExpandAllConditions()
{
  static LiteralList conds(UNORDERED);
  static LiteralList to_add(UNORDERED);
  static LiteralList add_to_basic(UNORDERED); 
  static FunctionList cond_funcs(UNORDERED);
  Literal *lt = NULL, *lt2 = NULL;
  int i = 0;
  SpecialLiteral *sp = NULL;
  Function *fun = NULL;

  add_to_basic.Clear();
  // first check the heads if necessary
  if (type != BASICRULE) {
    for (i=0; i < head_number; i++) {
      conds.Clear();
      cond_funcs.Clear();
      lt = head_array[i];
      if (lt->condition_func || lt->conditions) {
        if (ExpandCondition(lt, &conds, &cond_funcs, weight_head) ==
            COND_LOCAL) { 
          while ((lt2 = conds.Iterate())) {
            AddHead(lt2);
          }
          while ((fun = cond_funcs.Iterate())) {
            AddFunction(fun);
          }
          lt->deleted = 1;
          deleted++;
          num_conditions--;
          num_head_conditions--;
        }
      }
    }
  } else {
    if (head->condition_func || head->conditions) {
      error(USR_ERR, "%s: conditions are not allowed in basic rule "
            "heads", error_file_and_line(line_start));
    }
  }
  
  // then the tails
  while ((lt = positive.Iterate())) {
    if (lt->special) {
      sp = (SpecialLiteral*) lt;
      if (sp->type == WEIGHTRULE)
        num_weights++;
      
      while ((lt2 = sp->tail.Iterate())) {
        if (lt2->conditions || lt2->condition_func) {
          conds.Clear();
          cond_funcs.Clear();
	  
          if (ExpandCondition(lt2, &conds, &cond_funcs,
                              (sp->type == WEIGHTRULE))  ==
              COND_LOCAL) {
            while ((lt = conds.Iterate())) {
              to_add.Insert(lt);
            }
            while ((fun = cond_funcs.Iterate())) {
              AddFunction(fun);
            }
            //	    if ((conds.Size() > 0) || (cond_funcs.Size() > 0)) {
            sp->tail.Remove(lt2);
	      
            sp->num_conditions--;
            if (lt2->negative) {
              sp->num_negative_conditions--;
            } else {
              sp->num_positive_conditions--;
            }
	      
            //	    } else {
            //	      lt2->empty_condition = 1;
            //	    }
          }
        }
        while ((lt = to_add.Iterate()))
          sp->tail.Insert(lt);
        to_add.Clear();
      }
    } else if (lt->conditions || lt->condition_func) {
      conds.Clear();
      cond_funcs.Clear();
      if (ExpandCondition(lt, &conds, &cond_funcs, 0) == COND_LOCAL) {
        while ((lt2 = conds.Iterate())) {
          add_to_basic.Insert(lt2);
        }
        while ((fun = cond_funcs.Iterate())) {
          AddFunction(fun);
        }
        //	if ((conds.Size() > 0) || (cond_funcs.Size() > 0)) {
        positive.Remove(lt);
        num_conditions--;
        num_positive_conditions--;
        //	} else {
        //	  lt->empty_condition = 1;
        //	}
      }
    }
  }
  // FIXME: Why this is here???
  while (special_lits && (sp = (SpecialLiteral*) special_lits->Iterate())) {
    if (sp->atmost_lit)
      AddLiteral(sp->atmost_lit);
  }
  
  while ((lt = negative.Iterate())) {
    if (lt->conditions || lt->condition_func) {
      conds.Clear();
      cond_funcs.Clear();
      if (ExpandCondition(lt, &conds, &cond_funcs, 0) == COND_LOCAL) {
        while ((lt2 = conds.Iterate())) {
          add_to_basic.Insert(lt2);
        }
        while ((fun = cond_funcs.Iterate())) {
          AddFunction(fun);
        }
        //	if ((conds.Size() > 0) || (cond_funcs.Size() > 0)) {
        negative.Remove(lt);
        num_conditions--;
        num_negative_conditions--;
        //	} else {
        lt->empty_condition = 1;
        //	}
      }
    }
  }
  
  if (add_to_basic.Size() > 0) {
    while ((lt = add_to_basic.Iterate())) {
      AddLiteral(lt);
    }
  }
}

int Rule::ExpandCondition(Literal *lt, LiteralList *conds,
                          FunctionList *, int) 
{
  // go through each condition of the literal, construct a rule out of
  // them, ground the rule, and add the resulting literals to the rule
  // body. 

  //  { foo(Local):bar(Local) } :- ... ====> { foo(a), ... } :- ...
  
  int global_vars = 0;
  int sets_variables = 0; // true, if a condition gives values to
  // non-internal variables. 
  Literal *tmp = NULL;
  Function *fun = NULL;
  int i = 0;
  int errors = 0;

  while (lt->conditions && (tmp = lt->conditions->Iterate())) { 
    if (!tmp->DomainLiteral()) {
      error(USR_ERR, "%s: A non-domain condition predicate '%s'",
            error_file_and_line(lt->lineno), predicates[tmp->pred]->Name());
      errors++;
      continue;
    }
    for (i = 0; i < tmp->arity; i++) {
      if (tmp->args[i] == T_VARIABLE) {
        if (vars[tmp->vars[i]] != VAR_CONDITION) 
          global_vars++;
        else // if ( !internal_variable(tmp->vars[i]))
          sets_variables = 1;
      }
    }
  }
  while (lt->condition_func && (fun = lt->condition_func->Iterate())) {
    for (i=0; i < fun->arity; i++) {
      if (fun->args[i]->type == T_VARIABLE) {
        if (vars[fun->args[i]->val] != VAR_CONDITION) {
          global_vars++;
        } else { //if (!internal_variable(fun->args[i]->val)) {
          sets_variables = 1;
        }
      } else if (fun->args[i]->type == T_FUNCTION &&
                 ((Function*)fun->args[i])->CheckGlobal(vars)) {
        global_vars++;
      }
      else if (fun->args[i]->type == T_ATOM &&
               ((LiteralTerm*)fun->args[i])->CheckGlobal(vars)) {
        global_vars++; // *** korjaa!!!
      }
    }
  }
  for (i = 0 ; i < lt->arity; i++) {
    if ((lt->args[i] == T_VARIABLE) &&
        vars[lt->vars[i]] != VAR_CONDITION) {
      global_vars++;
    }
  }
  
  
  // don't ground yet if there are global variables 
  if (global_vars > 0) {
    lt->has_global_condition = 1;
    if (!sets_variables) {
      lt->ignore_global_condition = 1;
    }
    return COND_GLOBAL;
  }
  /*  error(USR_ERR, "%ld--%ld: a global variable is used within a condition",
      line_start, line_end); 
      output(cerr, "\tglobal variables: ");
      for (i = 0; i < global_vars; i++) {
      output(cerr, "%s ", variable_table->symbols[global[i]]);
      }
      output(cerr, "\n");
      errors++;
      }

   
      if (errors)
      return COND_ERROR;
  */

  for (i = 0; i < sys_data.total_vars; i++) {
    variables[i] = VAR_NONE;
    var_pos[i] = VAR_NONE;
  }

  GroundCondition(lt, conds);
  return COND_LOCAL;
}

int Rule::CheckNegativeLiterals()
{
  if (sys_data.print_domains != PR_ALL)
    return 1;
  else
    return 0;
}



void Rule::RestrictFunction(Function *tm)
{
  //  int pos = 0;
  long i;
  long var = ERROR_NUMBER, pred = -1;
  Literal *lt = NULL;

  static char *varname = NULL;
  static char *predname = NULL;

  if (!varname) {
    varname = new char[RANGE_MAX_NAME];
    predname = new char[RANGE_MAX_NAME];
    if (!varname || !predname)
      error(SYS_ERR, "malloc error");
  }
    
  for (i = 0; i < tm->arity; i++) {
    if (tm->args[i]->type == T_RANGE) {
      ostringstream ss;
      pred = Predicate::DefineRangePredicate();
      output(ss, "I_Range&%d", sys_data.num_ranges);
      
      var = variable_table->Insert(ss.str().c_str());
      if (var < 0)
        int_error("cannot generate range variable '%s'", varname);
      tm->args[i]->type = T_VARIABLE;
      tm->val = var;
      
      predicates[pred]->SetArity(1);

      Range *r = ((Range *) tm->args[i]);
      predicates[pred]->AddRange(r);

      if (head) 
        r->AddDependencies(head->pred);
      else {
        for (i = 0; i < head_number; i++) {
          if (!head_array[i]->deleted)
            r->AddDependencies(head_array[i]->pred);
        }
      }
      
      lt = new Literal(var, pred);
      if (!lt)
        error(SYS_ERR, "malloc error");
      lt->lineno = line_start;
      AddLiteral(lt);
    } else if (tm->args[i]->has_range) {
      RestrictFunction((Function*) tm->args[i]);
    }
  }
}

int Rule::TrueGround()
{
  Literal *lt = NULL;
  Predicate *pr = NULL;
  Function *fun = NULL;
  
  while ((lt = positive.Iterate())) {
    pr = predicates[lt->pred];
    if (!lt->DomainLiteral()) {
      positive.ClearIterator();
      return 1;
    }
    if (lt->arity == 0) {
      if (!pr->follows) {
        positive.ClearIterator();
        return 0;
      }
    } else {
      if (pr->atoms->Lookup(lt->cons) < 0) {
        positive.ClearIterator();
        return 0;
      }
    }
  }
  
  while ((lt = negative.Iterate())) {
    if (lt->arity == 0) {
      if (predicates[lt->pred]->follows) {
        negative.ClearIterator();
        return 0;
      }
    } else {
      if (predicates[lt->pred]->atoms->Lookup(lt->cons) >= 0) {
        negative.ClearIterator();
        return 0;
      }
    }
  }

  while ((fun = functions.Iterate())) {
    if (!fun->Test(0)) {
      functions.ClearIterator();
      return 0;
    }
  }
  
  return 1;
}


void Rule::GroundSpecialRule(int domain)
{
  GroundRule(domain);
}


// FIXME: GroundRule is not reentrant. 
void Rule::GroundRule(int domain)
{
  debug(DBG_GROUND, 3, "Grounding rule. domain: %d", domain);
  Literal *lt = NULL;
  Function *fun = NULL;
  static FunctionList **funs = NULL;
  static int old_max = 0;
  Iterator **indices = NULL;
  Predicate *pr = NULL;
  static LiteralList **negs = NULL;
  Literal **lts = NULL;
  Instance *item = NULL;
  static Instance *null_item = NULL;
  Instance **values = NULL;
  int *inds = NULL;
  DomainType tp = DM_UNKNOWN;
  int alloc_max = 0, max = 0, i = 0, existing = 0, ind = -1, end = 0;
  int constant_index = 0, constant_index_pos = 0;
  static Instance *constant_indices = NULL;
  int check_negatives = CheckNegativeLiterals();
  
  LiteralList posg(ORDERED_SMALL), posn(UNORDERED);
  // respectively number of domain positive, nondomain positive and
  // domain negative
  int gpos = 0, npos = 0, gneg = 0, pos = 0, discard = 0; 

  //  if (fact)
  //    return;
  
  if (!null_item) {
    null_item = new Instance;
    if (!null_item)
      error(SYS_ERR, "malloc error");
    *null_item = -1;
  }

  
  if (old_max < max_tail) {
    for (i = 0; i < old_max; i++) {
      delete funs[i];
    }
    delete [] funs;
    delete [] constant_indices;
    funs = new FunctionList*[max_tail];
    constant_indices = new Instance[max_tail];
    if (!funs || !constant_indices)
      error(SYS_ERR, "malloc error");
    for (i = 0; i < max_tail; i++) {
      funs[i] = new FunctionList;
      constant_indices[i] = -1;
	  
    }
  }
  
  if (old_max < max_tail) {
    if (negs) {
      delete [] negs;
    }
    negs = new LiteralList*[max_tail];
    if (!negs)
      error(SYS_ERR, "malloc error");
    for (i=0; i < max_tail; i++)
      negs[i] = new LiteralList;
    old_max = max_tail;
  }
  
  for (i = 0; i < max_tail; i++) {
    funs[i]->Clear();
    negs[i]->Clear();
  }
  
  // first we divide positive literals to domain and non-domain
  // predicates and sort domain predicates by the size of their
  // domains.
  positive.ClearIterator();
  while ((lt = positive.Iterate())) {
    if (lt->special)
      continue;
    if (lt->DomainLiteral() &&
        (!predicates[lt->pred]->transitive_domain || lt->pred !=
         head->pred)) {
      if (predicates[lt->pred]->NeedsToBePrinted())
        gpos--;
      
      if (lt->has_global_condition && !lt->ignore_global_condition) {
        if (!lt->DomainLiteral())
          gpos++;
        continue;
      }
      if (lt->arity > 0 && !lt->ground) { 
        posg.Insert(lt, predicates[lt->pred]->Size());
        gpos++;
      } else {
        if (!lt->Test()) { // is there a unsatisfiable literal
          end = 1;
        }
        gpos++;
      }
    } else {
      posn.Insert(lt);
      npos++;
    }
  }
  
  // allocate and initialize the data structures
  max = posg.Size();
  
  if (!max)
    alloc_max = 1;
  else
    alloc_max = max;
  
  indices = new Iterator*[alloc_max];
  
  inds = new int[alloc_max];
  lts = new Literal*[alloc_max];
  values = new Instance*[alloc_max];
  if (!inds || !indices || !lts || !values)
    error(SYS_ERR, "malloc error");
  
  for (i = 0; i < sys_data.total_vars; i++)
    variables[i] = ERROR_NUMBER;
  
  memset(inds, -1, alloc_max * sizeof(int));
  for (i = 0; i < alloc_max; i++ ) {
    indices[i] = NULL;
    lts[i] = NULL;
  }
  for (i = 0; i < sys_data.total_vars; i++)
    var_pos[i] = ERROR_NUMBER;
  for (i = 0; i < max; i++)
    values[i] = null_item;
  
  // set value of variables[i] to the first literal in positive
  // position where it appears in the domain predicates. Confusing,
  // huh. At same time choose an index for it. Use a constant index if
  // possible. 
  pos = -1;
  while ((lt = posg.Iterate())) {
    pos++; existing = 0; ind = -1;
    lts[pos] = lt;
    pr = predicates[lt->pred];
    if (lt->ground && !lt->Test()) { // return if unsatisfiable body 
      end = 1;
    }
    constant_index = 0;
    for (i = 0; i < lt->arity; i++) {
      if (lt->vars[i] >= 0) {
        if (variables[lt->vars[i]] < 0) {
          variables[lt->vars[i]] = pos;
        } else { // choose as index if needed
          if ((variables[lt->vars[i]] != pos) && !existing) {
            if (pr->existing_indexes & (1 << i)) 
              existing = 1;
            ind = i;
            constant_index = 0;
          }
        }
      } else {
        // check for possibility of using a constant as an index
        if (lt->cons[i] >= 0 && !existing) {
          if (pr->existing_indexes & (1 << i))
            existing = 1;
          ind = i;
          constant_index = 1;
        }
      }
    }
    // allocate the iterator
    if (ind < 0) {
      indices[pos] = new InstanceIterator(pr->atoms);
    } else {
      if (!existing)
        pr->CreateIndex(ind);
      indices[pos] = new IndexIterator(pr->indices[ind]);
    }
    if (!indices[pos])
      error(SYS_ERR, "malloc error");
    inds[pos] = ind; 
    // and set the values array to point to right variable
    if (ind >= 0) {
      if (!constant_index)
        values[pos] = &variables[lt->vars[ind]];
      else {
        values[pos] = &constant_indices[constant_index_pos];
        constant_indices[constant_index_pos++] = lt->cons[ind];
      }
    }
  }
  
  // then go through all functions to seek when the function can be
  // computed and add it to right list
  if (functions.Size() > 0) {
    functions.ClearIterator();
    while ((fun = functions.Iterate())) {
      if (fun->assign && variables[fun->args[0]->val] >=0)
        fun->RemoveAssign();
	  
      pos = fun->GetPos();
      if (pos < 0)
        pos = max -1;
      if ((pos < 0) || (pos >= max))
        pos = 0;
      fun->AddVars(pos);
      funs[pos]->Append(fun);
    }
  }
  
  // same to negative literals
  if (negative.Size() > 0)
    negative.ClearIterator();
  while ((lt = negative.Iterate())) {
    tp = predicates[lt->pred]->Status();
    if (tp == DM_SPECIAL)
      continue;
    if (!lt->ground) {
      pos = lt->GetPos();
      if ((pos != 0) && ((pos < 0) || (pos >= max)))
        int_error("invalid negative literal position '%d'", pos);
      negs[pos]->Insert(lt);
      
    } else {
      if (check_negatives && lt->Test()) { // check if a negative ground literal is true
        end = 1;        // if it is, the rule body is always
      }                 // unsatisfied 
    }
    // at the same time calculate the number of negative domains
    if (tp == DM_DOMAIN && !predicates[lt->pred]->NeedsToBePrinted())
      gneg++;
  }
  
  // and then do the actual job
  for (i = 0; i < sys_data.total_vars+1; i++) {
    variables[i] = ERROR_NUMBER;
  }
  pos = 0;
  
  if (ground) {
    if (!end && TrueGround()) {
      if (domain == GRD_RULE || sys_data.print_domains == PR_ALL)
        EmitGround(gpos, gneg,1);
      
      if (domain != GRD_RULE) {
        CreateInstance(domain);
      }	
    } else if ((sys_data.print_domains == PR_ALL) &&
               (positive.Size() + negative.Size() > 0))  {
      EmitGround(gpos, gneg, 1);
    }
    end = 1;
  }
  
  //  if (sys_data.print_domains == PR_ALL) {
  //	end = 0;
  //  }
  
  if (!end) {
    while (1) {
      if (pos < 0)
        break; // all possible bindings have been checked
      
      discard = 0;
      if (max != 0) 
        item = indices[pos]->Iterate(*values[pos]);

      // if run out of items in this level go back up one level
      if (!item && max != 0) {
        pos--;
        clear_pos(pos);
      } else {
        // bind the new variables
        if (Literal::BindLiteral(lts[pos], item, pos)) {
          // values were ok, then check all applicable functions and 
          // negative literals. These have to be in this order since
          // fun->Test() may set values of some variables
	  
          while ((functions.Size() > 0) && (fun = funs[pos]->Iterate()))
            if (!discard && !fun->Test(pos))
              discard = 1;


          if (check_negatives) {
            while ((negative.Size() > 0)&& (lt = negs[pos]->Iterate()))
              if (!discard && lt->Test())
                discard = 1;
          }
        } else
          discard = 1; // binding failed
	
        if (!discard) {
          if ((pos == max - 1) || (max == 0)) { // deepest level
            if (domain == GRD_RULE || sys_data.print_domains == PR_ALL)
              EmitGround(gpos, gneg,1);

            if (domain != GRD_RULE)
              CreateInstance(domain);
	  
            if (max != 0)
              clear_pos(pos);
            else
              break;
          } else {
            // not yet at deepest level, so go deeper
            pos++;
          }
        }else {
          // clear the variables and stay at this level
          clear_pos(pos);
        }
      }
    }
  }
  delete [] inds;
  for (i = 0; i < alloc_max; i++) {
    delete indices[i];
  }
  delete [] indices; 
  delete [] lts;
  delete [] values;
}


// takes a condition as its argument, and expands it. This is intended
// to work in the innermost loop and be called by EmitRule, so messing
// up with variables and var_pos is out. Returns the expanded literals
// as a list and clears the changes to global variables that it does. 

// NRGH. I should rm -rf this part of code and start over...
void Rule::GroundCondition(Literal *lt, LiteralList *lst)
{
  assert (lt->conditions || lt->condition_func) ;
  static Literal **arr = NULL;
  static InstanceIterator **iter = NULL;
  static int max_arr = 0;
  static InstanceSet *dummy = NULL;
  static int *exists_value = 0;
  int i;
    
  if (!exists_value) {
    exists_value = new int[sys_data.total_vars];
  }
  
  Weight *wt = 0;
  Literal *l = NULL;
  Instance *item = NULL;
  int discard = 0;
  int end = 0;
  Function  *fun = NULL;
  int treshold =max_tail + 1; // this ensures that we can distinguish
  //  the variables set by this function
  // from those set in GroundRule
  int max = 0;
  if (lt->conditions) {
    max = lt->conditions->Length();
  }
  for (i = 0; i < sys_data.total_vars; i++) {
    if (var_pos[i] >= 0)
      exists_value[i] = 1;
    else
      exists_value[i] = 0;
  }
  // allocate the literal and iterator arrays, if needed 
  if (!arr || max > max_arr) {
    max_arr = max;
    if (arr) {
      delete [] arr;
      delete [] iter;
    }
    arr = new Literal*[max_arr+1];
    iter = new InstanceIterator*[max_arr+1];
    if (!arr || !iter)
      error(SYS_ERR, "malloc error");
    for (i = 0; i < max_arr+1; i++) {
      iter[i] = new InstanceIterator(NULL);
    }
  }

  if (lt->conditions) {
    lt->conditions->ClearIterator();
    lt->condition_func->ClearIterator();
  }
  
  lt->empty_condition = 1;

  // initialize the arrays
  int pos = 0;  
  while (lt->conditions && (l = lt->conditions->Iterate())) {
    if ((l->arity > 0) && !l->ground) {
      arr[pos] = l;
      // for now, don't try to optimize
      iter[pos]->Reset(predicates[l->pred]->atoms);
      pos++;
      for (i = 0; i < l->arity; i++) {
        if (l->args[i] == T_VARIABLE) {
          exists_value[l->vars[i]] = 1;
        }
      }
    } else if (!l->Test()) { // unsatisfiable literal
      end = 1;
    }
  }
  if (end)
    return;
  
  max = pos;

  while (lt->condition_func && (fun = lt->condition_func->Iterate()))
    {
      if (fun->assign && exists_value[fun->args[0]->val])
        fun->RemoveAssign();
    }
  
  // add a dummy iterator if there are only condition functions 
  if (lt->conditions->Size() == 0) {
    if (!dummy) {
      dummy = new HashSet(1,1);
    }
    iter[0]->Reset(dummy);
  }
  // now do the actual grounding. Construct a new ground literal for
  // each grounded condition, and add it to 'lst'

  pos = 0; 
  while (pos >= 0) {
    if (pos < 0)
      break; // everything that needs to be done is done. 

    discard = 0;
    if (max != 0) {
      item = iter[pos]->Iterate();
    }

    if (!item && max != 0) {
      pos--;
      clear_pos(pos+treshold);
    } else {
      // bind the new variables, if any
      if (Literal::BindLiteral(arr[pos], item, pos + treshold)) {
        // all's well, so go through functions
        if ((pos == max-1) || (max == 0)) {
          while (lt->condition_func &&
                 (fun = lt->condition_func->Iterate())) {
            if (!discard && !fun->Test(pos+treshold))
              discard = 1;
          }
        }
      } else { // binding failed
        discard = 1;
      }

      if (!discard) {
        if ((pos == max-1) || (max == 0)) {
          // deepest level, construct a literal
          lt->empty_condition = 0;
          l = lt->Instantiate();
          bool g = true;
          for (int j = 0; j < l->arity; j++) {
            if (l->args[j] == T_VARIABLE) {
              g = false;
            } else if ((l->args[j] == T_FUNCTION) && (!l->terms[j]->ground)) {
              g = false;
            }
          }
          lt->ground = g;
          if (lt->weight) {
            wt = lt->weight->Instantiate();
            l->weight = wt;
          } if (lt->dynamic_weight) {
            wt = lt->dynamic_weight->Instantiate();
            l->weight = wt;
          }
          if (lst)
            lst->Insert(l);
          clear_pos(pos + treshold);
          if (max == 0) // only one try if no literals
            pos--;
        } else {
          // go deeper
          pos++;
        }
      } else {
        // clear and stay at same level
        clear_pos(pos + treshold);
        if (max == 0) // only one try if no literals
          pos--;
      } 
    }
  }
}

int Rule::SortFunctions()
{
  static Graph *sort_graph = 0;
  Literal *lt = NULL;
  Function *fun = NULL;
  Variable start = -1;
  long i = 0;

  if (sorted) {
    return 1;
  } else {
    sorted = 1;
  }

  // do we have to do anything?
  if (!num_assigns && !num_conditions)
    return 1;
  
  if (!sort_graph) {
    sort_graph = new Graph;
    for (i =0; i < sys_data.total_vars; i++) {
      sort_graph->AddNode(i);
    }
  } else {
    sort_graph->ClearEdges();
  }
  // first check all functions for assigns
  while ((fun = functions.Iterate())) {
    if (fun->assign) {
      start = fun->args[0]->val;
      fun->AddGraphEdges(sort_graph, start, 1);
    }
  }
  // process the literals
  while ((lt = positive.Iterate())) {
    if (lt->special) {
      SpecialLiteral *slt = (SpecialLiteral*) lt;
      while ((fun = slt->funs.Iterate())) {
        if (fun->assign) {
          start = fun->args[0]->val;
          fun->AddGraphEdges(sort_graph, start, 1);
        }
      }
      while ((lt = slt->tail.Iterate())) {
        if (lt->condition_func) {
          while ((fun = lt->condition_func->Iterate())) {
            if (fun->assign) {
              start = fun->args[0]->val;
              fun->AddGraphEdges(sort_graph, start, 1);
            }
          }
        }
      }
    } else {
      if (lt->condition_func) {
        while ((fun = lt->condition_func->Iterate())) {
          if (fun->assign) {
            start = fun->args[0]->val;
            fun->AddGraphEdges(sort_graph, start, 1);
          }
        }
      }
    }
  }
  while ((lt = negative.Iterate())) {
    if (lt->condition_func) {
      while ((fun = lt->condition_func->Iterate())) {
        if (fun->assign) {
          start = fun->args[0]->val;
          fun->AddGraphEdges(sort_graph, start, 1);
        }
      }
    }
  }
  
  // Then sort the graph
  if (!sort_graph->TopologicalSort()) {
    error(USR_ERR, "%ld: The variable assigments in a rule", line_start);
    PrintRule();
    output(cerr, "   form a cycle.\n");
    return 0;
  }

  // arrange all the functions
  functions.Sort(sort_graph);

  //and in the literals...
  while ((lt = negative.Iterate())) {
    if (lt->condition_func) {
      lt->condition_func->Sort(sort_graph);
    }
  }
  while ((lt = positive.Iterate())) {
    if (lt->special) {
      SpecialLiteral *slt = (SpecialLiteral*) lt;
      slt->funs.Sort(sort_graph);
      while ((lt = slt->tail.Iterate())) {
        if (lt->condition_func)
          lt->condition_func->Sort(sort_graph);
      }
    } else if (lt->condition_func) {
      lt->condition_func->Sort(sort_graph);
    }
  }
  return 1;
}

// This checks _only_ basicrules, not any special constructions. 
RestrictType Rule::CheckRestrict(int print)
{
  long i = 0;
  Literal *lt = NULL;
  Function *fun = NULL;
  RestrictType res = RT_STRONG;
  static long *weak = NULL, *none = NULL;
  long numweak = 0, numnone = 0;

  if (restrict_status != RT_UNKNOWN) {
    return restrict_status;
  } 
  
  if (!SortFunctions()) {
    restrict_status = RT_NONE;
    return RT_NONE;
  }

  if (!weak || !none) {
    weak = new long[sys_data.total_vars+1];
    none = new long[sys_data.total_vars+1];
    
    if (! weak || !none)
      error(SYS_ERR, "malloc error");
  }

  if (!vars) {
    vars = new Variable[sys_data.total_vars+1];
    if (!vars)
      error(SYS_ERR, "malloc error");
  }
  memset(variables, R_NONE, sizeof(Variable) * sys_data.total_vars); 
  
  head->Restrict(R_NEGATIVE_LOCAL);

  negative.ClearIterator();
  while (( lt = negative.Iterate()))
    lt->Restrict(R_NEGATIVE_LOCAL);

  functions.ClearIterator();
  while (( fun = functions.Iterate()))
    fun->Restrict(R_NEGATIVE_LOCAL);
  
  positive.ClearIterator();
  while ((lt = positive.Iterate()))
    // Miksi?
    if (lt->DomainLiteral() && (lt->pred != head->pred)) // &&
      // !lt->conditions && !lt->condition_func) 
      lt->Restrict(R_DOMAIN_LOCAL);
    else
      lt->Restrict(R_POSITIVE_LOCAL);

  
  for (i = 0; i < sys_data.total_vars; i++) {
    switch (variables[i]) {
    case R_CONDITION_LOCAL:
      vars[i] = VAR_CONDITION;
      break;
    case R_FUNCTION_FAIL:
    case R_NEGATIVE_LOCAL:
    case R_NEGATIVE:
      res = RT_NONE;
      none[numnone++] = i;
      break;
    case R_POSITIVE:
    case R_POSITIVE_LOCAL:
      if (res > RT_NONE)
        res = RT_WEAK;
      weak[numweak++] = i;
      break;
    default:
      vars[i] = VAR_GLOBAL;
      break;
    }
    
  }
  restrict_status = res;
  if (res < RT_STRONG && print) {
    output(cerr, "%s: %srestricted rule: ",
           error_file_and_line(line_start), 
           ( (res == RT_NONE) ? "non" : "weakly "));
    PrintRule();
    if (numnone > 0) {
      output(cerr, "\tunrestricted variables: ");
      for (i = 0; i < numnone; i++)
        output(cerr, "%s ", variable_table->symbols[none[i]]);
      output(cerr, "\n");
    }
    if (numweak > 0) {
      output(cerr, "\tweakly restricted variables: ");
      for (i = 0; i < numweak; i++)
        output(cerr, "%s ", variable_table->symbols[weak[i]]);
      output(cerr, "\n");
    }
  }

  // expand conditions with no globality at this point
  if (num_conditions) {
    ExpandAllConditions();
    /** NEW ***/
    // check if the rule is now ground.
    bool g = 1;
    if (!head->ground) {
      g = false;
    }
    positive.ClearIterator();
    negative.ClearIterator();
	
    while ((lt = positive.Iterate())) {
      if (!lt->ground) {
        g = false;
      } 		
    }
    while ((lt = negative.Iterate())) {
      if (!lt->ground) {
        g = false;
      } 		
    }
    if (g == true) {
      ground = true;
    } else {
      ground = false;
    }
  }
  //  FindWeights();
  
  return res;
}

// this checks whether special rules are strongly range restricted
RestrictType Rule::CheckSpecialRestrict()
{
  int i = 0;
  Literal *lt = NULL;
  SpecialLiteral *spl = NULL;
  RestrictType res = RT_STRONG;
  Function *fun = NULL;
  static long *weak = NULL, *none = NULL, *in_conditions = NULL;
  int numnone=0, numweak=0, numcond = 0;

  if (restrict_status != RT_UNKNOWN) {
    return restrict_status;
  }

  
  if (!SortFunctions()) {
    restrict_status = RT_NONE;
    return RT_NONE;
  }
  
  if (!weak){
    weak = new long[sys_data.total_vars+1];
    none = new long[sys_data.total_vars+1];
    in_conditions = new long[sys_data.total_vars+1];
    
    if ( !weak || !none || !in_conditions)
      error(SYS_ERR, "malloc error");
  }

  memset(variables, R_NONE, sizeof(Variable) * sys_data.total_vars);
  
  // Restrict the bodies. All variables in the local bodies are treated as
  // global variables automatically.
  while ((lt = positive.Iterate())) {
    if (!lt->special) {// restrict oridinary literal
      if (lt->DomainLiteral())
        lt->Restrict(R_DOMAIN);
      else
        lt->Restrict(R_POSITIVE);
    } else { // a special literal
      spl = (SpecialLiteral*)lt;
      spl->RestrictSpecial();
    }
  }

  while ((lt = negative.Iterate())) {
    if (!lt->special) {
      lt->Restrict(R_NEGATIVE);
    } else {
      int_error("a special rule somehow ended as negative literal",
                ""); 
    }
  }

  while ((fun = functions.Iterate())) {
    fun->Restrict(R_NEGATIVE);
  }

  if (type == BASICRULE) {
    head->Restrict(R_NEGATIVE_LOCAL);
  } else {
    for (i=0; i < head_number; i++) {
      head_array[i]->Restrict(R_NEGATIVE_LOCAL);


    }
  }
  
  // go through all variables and see that they are restricted
  for (i = 0; i < sys_data.total_vars; i++) {
    switch (variables[i]) {
    case R_WEIGHT_LOCAL:
    case R_WEIGHT:
      //      res = RT_NONE;
      //free[numfree++] = i;
      //break; // fall through, at leas for a while 
    case R_FUNCTION_FAIL:
    case R_NEGATIVE:
    case R_NEGATIVE_LOCAL:
      res = RT_NONE;
      none[numnone++] = i;
      break;
    case R_POSITIVE:
    case R_POSITIVE_LOCAL:
      if (res > RT_NONE)
        res = RT_WEAK;
      weak[numweak++] = i;
      break;
      /*      if (res > RT_NONE)
              res = RT_WEAK;
              in_conditions[numcond++] = i; */
      break;
    default:
      break;
    }
  }
  
  // save the variable data so that it is available when conditions are
  // expanded
  vars = new Variable[sys_data.total_vars+1];
  if (!vars)
    error(SYS_ERR, "malloc error");
  for (i = 0; i < sys_data.total_vars; i++) {
    switch (variables[i]) {
    case R_NONE:
      vars[i] = VAR_NONE;
      break;
    case R_WEIGHT_LOCAL:
    case R_NEGATIVE_LOCAL:
    case R_POSITIVE_LOCAL:
    case R_ASSIGNED_LOCAL:
    case R_DOMAIN_LOCAL:
      vars[i] = VAR_LOCAL;
      break;
    case R_CONDITION_LOCAL:
      vars[i] = VAR_CONDITION;
      break;
    default:
      vars[i] = VAR_GLOBAL;
      break;
    }
  }
  restrict_status = res;
  if (res < RT_STRONG) {
    sys_data.num_errors++;
    output(cerr, "%s: %srestricted rule: ",
           error_file_and_line(line_start) ,
           ( (res == RT_NONE) ? "non" : "weakly ")); 
    PrintRule();
    if (numnone > 0) {
      output(cerr, "\tunrestricted variables: ");
      for (i = 0; i < numnone; i++)
        output(cerr, "%s ", variable_table->symbols[none[i]]);
      output(cerr, "\n");
    }
    if (numweak > 0) {
      output(cerr, "\tweakly restricted variables: ");
      for (i = 0; i < numweak; i++)
        output(cerr, "%s ", variable_table->symbols[weak[i]]);
      output(cerr, "\n");
    }
    if (numcond > 0) {
      output(cerr, "\there were variables that : ");
      for (i = 0; i < numcond; i++)
        output(cerr, "%s ", variable_table->symbols[in_conditions[i]]);
      output(cerr, "\n");
    }
  }
  
  return res;
}
  

void Rule::PrintSpecialHead()
{
  int first = 1, i = 0;

  if (type == ORDERED_DISJUNCTION) {
    for (i = 0; i < head_number; i++) {
      if (head_array[i]->deleted)
        continue;
      if (i > 0) {
        output(cerr, " x ");
      }
      head_array[i]->Print();
    }
  } else {
    if (atleast) {
      atleast->Print();
    }
    output(cerr, "%s ", weight_head ? "[" : "{");

    for (i = 0; i < head_number; i++) {
      if (head_array[i]->deleted)
        continue;
      
      if (!first)
        output(cerr, ", ");
      else
        first = 0;
      head_array[i]->Print();
    }
    output(cerr, " %s ", weight_head ? "]" : "}");
    
    if (atmost)
      atmost->Print();
  }
}

void rule_print(Rule* r )
{
  r->PrintRule();
}

void Rule::PR()
{
  PrintRule();
}

void Rule::PrintRule(ostream &s)
{
  int first = 1;
  Literal *lt = NULL;
  Function *ft = NULL;
  
  if (type == BASICRULE)
    head->Print();
  else 
    PrintSpecialHead();

  output(s, " :- ");

  // print first special literals 
  while (special_lits && (lt = special_lits->Iterate())) {
    if (!first)
      output(s, ", ");
    first = 0;
    lt->Print();
  } 
  
  while (( lt = positive.Iterate())) {
    if (lt->special)
      continue;
    if (!first)
      output(s, ", ");
    first = 0;
    lt->Print();
  }
  while (( lt = negative.Iterate())) {
    if (!first)
      output(s, ", ");
    //    output(s, "not ");
    first = 0;
    lt->Print();
  }
  while ((ft = functions.Iterate())) {
    if (!first)
      output(s, ", ");
    ft->Print();
  }
  output(s, ".\n");
}

Variable *var_pos = NULL;

int Rule::ExpandGlobalConditions()
{
  Literal *lt = NULL;
  SpecialLiteral *slt = NULL;
  int i;
  bool empty = false;
  bool failed = false;
  
  head_conditions.Clear(1);
  negative_conditions.Clear(1);
  positive_conditions.Clear(1);
  
  // first check the heads
  if (type != BASICRULE) {
    for (i = 0; i < head_number; i++) {
      lt = head_array[i];
      if (!lt->deleted && (lt->conditions || lt->condition_func)) {
        GroundCondition(lt, &head_conditions);
      }
    }
  }

  positive.ClearIterator();
  // then the tails
  while ((lt = positive.Iterate())) {
    if (lt->special) {
      slt = (SpecialLiteral *) lt;
      if (slt->num_conditions) {
        slt->negative_conditions.Clear();
        slt->positive_conditions.Clear();
        while ((lt = slt->tail.Iterate())) {
          if (lt->conditions || lt->condition_func) {
            if (lt->negative)
              GroundCondition(lt, &slt->negative_conditions);
            else 
              GroundCondition(lt, &slt->positive_conditions);
          }
        }
      }
    } else {
      if (lt->conditions || lt->condition_func) {
        GroundCondition(lt, &positive_conditions);
		
        // is the condition empty?
        if (lt->empty_condition)
          empty = true;
      }
    }
  }
  
  // check if there are unsatisfied domain literals in
  // positive_conditions
  while ((lt = positive_conditions.Iterate())) {
    if (lt->DomainLiteral() && !lt->Test()) {
      positive_conditions.ClearIterator();
      failed = true;
      break;
    }
  }

  negative.ClearIterator();
  while ((lt = negative.Iterate())) {
    if (lt->conditions || lt->condition_func) {
      GroundCondition(lt, &negative_conditions);
    }
  }
  
  // check for satisfiability
  while ((lt = negative_conditions.Iterate())) {
    if (lt->Test())
      failed = true;
  }
  if (failed)
    return 0;
  else
    return 1;
}

void Rule::CreateInstance(int dtype)
{
  if (num_conditions) {
    if (!ExpandGlobalConditions()) {
      return; // some unsatisfiable conditions
    }
  }
  if (dtype != GRD_TRANSITIVE_DOMAIN) {
    head->CreateInstance(dtype);
  } else {
    predicates[head->pred]->transitive_program->AddRuleInstance(this);
  }

}

/* use this only for choice rules */
void Rule::EmitNormalGround(int gpos, int gneg, int comp )
{
  Literal *lt;
  //  Literal *dummy = new Literal();
  int i;
  type = BASICRULE;
  /* go through the head array, emiting the rule once for all of them
     and also the complement atoms */
  for (i  = 0; i < head_number; i++) {
    if (head_array[i]->deleted ||
        head_array[i]->has_global_condition)
      continue;
    head = head_array[i];
    EmitSpecialRule( gpos,  gneg,  comp);
  }
  while ((lt = head_conditions.Iterate())) {
    head = lt;
    EmitSpecialRule( gpos,  gneg,  comp);
  }
  type = CHOICERULE;
}

void Rule::EmitGround(int gpos, int gneg, int comp )
{
  if (num_conditions) {
    if (!ExpandGlobalConditions()) {
      if (sys_data.print_domains != PR_ALL)
        return; // conditions contain unsatisfiable literals
      else if (!head_conditions.Size() && !positive_conditions.Size()
               && !negative_conditions.Size()) // empty conditions
        return;
    }
  }

  if (!special) {
    if (predicates[head->pred]->is_priority_meta_predicate) {
      EmitMetaPriority(gpos, gneg);
      return;
    }
    if (sys_data.emit_text) {
      EmitText(gpos, gneg,comp);
    } else {
      switch(sys_data.output_version) {
      case 1:
        EmitSmodels1(gpos, gneg,comp);
        break;
      case 2:
        EmitSmodels2(gpos, gneg,comp);
        break;
      default:
        int_error("unknown smodels version %d", sys_data.output_version);
        break;
      }
    }
  } else {
    EmitSpecialRule( gpos,  gneg,  comp);
  }
  return;
}

void Rule::EmitSpecialRule(int gpos, int gneg, int comp)
{
  SpecialLiteral *spl = NULL;
  
  if (type == ORDERED_DISJUNCTION) {
    EmitPriority(gpos, gneg);
    return;
  }
  
  if (sys_data.emit_text)
    EmitText(gpos, gneg, comp);
  else 
    EmitSmodels2(gpos,gneg, comp);
  
  while (special_lits && (spl = (SpecialLiteral *)
                          special_lits->Iterate())) { 
    spl->EmitRule();
  }

}   

void Rule::EmitNormalTextBody(Literal *current_head)
{
  Literal *lt = NULL;
  //  int i = 0;
  //  Instance head_atom = ERROR_NUMBER;
  DomainType lit_status = DM_DOMAIN;
  long complement;
  complement = Predicate::DefineNormalRuleComplement(current_head->pred);
  Literal *compl_head = current_head->Duplicate(1);
  compl_head->pred = complement;
  current_head->EmitGround();
  output(sys_data.output_file, " :- not ");
  compl_head->EmitGround();
  while (special_lits && (lt = special_lits->Iterate())) {
    output(sys_data.output_file, ", ");
    lt->EmitGround();
  }
  while ((lt = positive.Iterate())) {
    if (lt->special || lt->has_global_condition ||
        lt->empty_condition) 
      continue;
    lit_status = predicates[lt->pred]->Status();

    if ((lit_status != DM_INTERNAL) && 
        ((lit_status != DM_DOMAIN) || (sys_data.print_domains == PR_ALL)
         || (sys_data.print_domains == PR_POSITIVE)
         || predicates[lt->pred]->NeedsToBePrinted())) {
      output(sys_data.output_file, ", ");
      lt->EmitGround();
    }
  }

  while ((lt = positive_conditions.Iterate())) {
    lit_status = predicates[lt->pred]->Status();

    if ((lit_status != DM_INTERNAL) &&
        ((lit_status != DM_DOMAIN) || (sys_data.print_domains == PR_ALL)
         || (sys_data.print_domains == PR_POSITIVE)
         || predicates[lt->pred]->NeedsToBePrinted())){
      output(sys_data.output_file, ", ");
      lt->EmitGround();
    }
  }

  while ((lt = negative.Iterate())) {
    if (lt->has_global_condition || lt->empty_condition) 
      continue;
    lit_status = predicates[lt->pred]->Status();
    if ((lit_status != DM_INTERNAL) &&
        ((lit_status != DM_DOMAIN) || (sys_data.print_domains == PR_ALL)
         || predicates[lt->pred]->NeedsToBePrinted())){ 
      output(sys_data.output_file, ", not ");
      lt->EmitGround();
    }
  } 
  while ((lt = negative_conditions.Iterate())) {
    lit_status = predicates[lt->pred]->Status();
    if ((lit_status != DM_INTERNAL) &&
        ((lit_status != DM_DOMAIN) || (sys_data.print_domains == PR_ALL)
         || predicates[lt->pred]->NeedsToBePrinted())) {
      output(sys_data.output_file, ", not ");
      lt->EmitGround();
    }
  }
  output(sys_data.output_file, ".\n");

  compl_head->negative = 0;
  compl_head->EmitGround();
  output(sys_data.output_file, " :- not ");
  current_head->EmitGround();
  output(sys_data.output_file, ".\n");
}

void Rule::EmitNormalText()
{
  Literal *lt;
  int i;
  /* go through the head array, emiting the rule once for all of them
     and also the complement atoms */
  for (i  = 0; i < head_number; i++) {
    if (head_array[i]->deleted ||
        head_array[i]->has_global_condition)
      continue;
    EmitNormalTextBody(head_array[i]);
  }
  while ((lt = head_conditions.Iterate())) {
    EmitNormalTextBody(lt);
  }
}

void Rule::EmitText(int, int, int)
{
  Literal *lt = NULL;
  int first = 1, i = 0;
  Instance head_atom = ERROR_NUMBER;
  DomainType lit_status = DM_DOMAIN;

  /// check for empty generaterule
  if (type != BASICRULE) {
    GroundHeads();
    if (grounded_heads.Size() < 1 && !sys_data.print_empty_choices)  {
      // empty choice rule, don't print
      return;
    }
  }

  if (sys_data.normal_output && type != BASICRULE) {
    EmitNormalText();
    return;
  }
  char *head_separator = ", ";
  char *head_start = "{ ";
  char *head_end = " }";
  switch (type) {
  case BASICRULE:
    head_atom = head->EmitGround();
    break;
  case DISJUNCTIVERULE:
    head_separator = " | ";
    head_start = "";
    head_end = ""; 
    /* fall through */
  default:
    output(sys_data.output_file, "%s", head_start);
    for (i = 0; i < head_number; i++) {
      if (head_array[i]->deleted ||
          head_array[i]->has_global_condition)
        continue;
      
      if (!first)
        output(sys_data.output_file, "%s", head_separator);
      else {
        first = 0;
      }
      head_array[i]->EmitGround();
    }
    while ((lt = head_conditions.Iterate())) {
      if (!first)
        output(sys_data.output_file, "%s", head_separator);
      else {
        first = 0;
      }
      lt->EmitGround();
    }
    if (grounded_heads.Size() < 1) {
      output(sys_data.output_file, "_false ");
    }
    output(sys_data.output_file, "%s", head_end);
  }
  first = 1;
  while (special_lits && (lt = special_lits->Iterate())) {
    if (first) {
      output(sys_data.output_file, " :- ");
      lt->EmitGround();
      first = 0;
    } else {
      output(sys_data.output_file, ", ");
      lt->EmitGround();
    }
  }

  
  while ((lt = positive.Iterate())) {
    if (lt->special || lt->has_global_condition ||
        lt->empty_condition) 
      continue;
    lit_status = predicates[lt->pred]->Status();
    
    if ((lit_status != DM_INTERNAL) && 
        ((lit_status != DM_DOMAIN) || (sys_data.print_domains == PR_ALL)
         || (sys_data.print_domains == PR_POSITIVE)
         || predicates[lt->pred]->NeedsToBePrinted())) {
      if (first) {
        output(sys_data.output_file, " :- ");
        lt->EmitGround();
        first = 0;
      } else {
        output(sys_data.output_file, ", ");
        lt->EmitGround();
      }
    }
  }

  while ((lt = positive_conditions.Iterate())) {
    lit_status = predicates[lt->pred]->Status();

    if ((lit_status != DM_INTERNAL) &&
        ((lit_status != DM_DOMAIN) || (sys_data.print_domains == PR_ALL)
         || (sys_data.print_domains == PR_POSITIVE)
         || predicates[lt->pred]->NeedsToBePrinted())){
      if (first) {
        output(sys_data.output_file, " :- ");
        lt->EmitGround();
        first = 0;
      } else {
        output(sys_data.output_file, ", ");
        lt->EmitGround();
      }
    }
  }

  while ((lt = negative.Iterate())) {
    if (lt->has_global_condition || lt->empty_condition) 
      continue;
    lit_status = predicates[lt->pred]->Status();
    if ((lit_status != DM_INTERNAL) &&
        ((lit_status != DM_DOMAIN) || (sys_data.print_domains == PR_ALL)
         || predicates[lt->pred]->NeedsToBePrinted())){ 
      if (first) {
        output(sys_data.output_file, " :- not ");
        lt->EmitGround();
        first = 0;
      } else {
        output(sys_data.output_file, ", not ");
        lt->EmitGround();
      }
    }
  } 
  while ((lt = negative_conditions.Iterate())) {
    lit_status = predicates[lt->pred]->Status();
    if ((lit_status != DM_INTERNAL) &&
        ((lit_status != DM_DOMAIN) || (sys_data.print_domains == PR_ALL)
         || predicates[lt->pred]->NeedsToBePrinted())) {
      if (first) {
        output(sys_data.output_file, " :- not ");
        lt->EmitGround();
        first = 0;
      } else {
        output(sys_data.output_file, ", not ");
        lt->EmitGround();
      }
    }
  }
  sys_data.ground_rules++;
  if (!first_ground) {
    first_ground = sys_data.ground_rules;
  }
  num_ground++;
  output(sys_data.output_file, ".\n");
}

void Rule::GroundHeads()
{
  Literal *lt;
  grounded_heads.Clear();
  for (int i = 0; i < head_number; i++) {
    if (head_array[i]->deleted ||
        head_array[i]->has_global_condition ||
        head_array[i]->empty_condition) 
      continue;
    grounded_heads.Insert(head_array[i]);
  }
  while ((lt = head_conditions.Iterate())) {
    grounded_heads.Insert(lt);
  }
}



void Rule::EmitNormalGroundSmodels2(int gpos, int gneg, int)
{
  Literal *lt = NULL, *head = NULL;
  Literal *complement_head = NULL;
  int numlit = 0, negs = 0; //, i =0 ;
  DomainType status = DM_DOMAIN;

  GroundHeads();
  
  // count the total number of literals in the body
  switch (sys_data.print_domains) {
  case PR_ALL:
    numlit = positive.Size() + negative.Size();
    numlit += positive_conditions.Size() - num_positive_conditions;
    numlit += negative_conditions.Size() - num_negative_conditions;
    numlit -= num_internal_literals;
    negs = negative.Size() + negative_conditions.Size() -
      num_negative_conditions - num_negative_internal_literals;
    break;
  case PR_POSITIVE:
    numlit = positive.Size() + negative.Size() - gneg;
    // domain predicates are already dropped from the condition lists
    numlit += positive_conditions.Size() - num_positive_conditions;
    numlit += negative_conditions.Size() - num_negative_conditions;
    negs = negative.Size() - gneg + negative_conditions.Size() -
      num_negative_conditions;
    break;
  case PR_HEADS:
  case PR_NONE:
    negs = negative.Size() - gneg + negative_conditions.Size() -
      num_negative_conditions; 
    numlit = positive.Size() -gpos + negative.Size() - gneg;
    numlit += positive_conditions.Size() - num_positive_conditions;
    numlit += negative_conditions.Size() - num_negative_conditions;
    break;
  default:
    int_error("really strange value '%d' in print_domains",
              sys_data.print_domains);
  }

  // Add the new negated atom
  negs++;
  numlit++;
  while ((head = grounded_heads.Iterate())) {
    complement_head = head->Duplicate();
    complement_head->pred = Predicate::DefineNormalRuleComplement(head->pred);
		  
    output(sys_data.output_file, "%d ", BASICRULE);
    head->EmitGround();
    // print the domain sizes 
    output(sys_data.output_file, "%d ", numlit);
    if (!numlit && type == GENERATERULE) {
      output(sys_data.output_file, "\n");
      return;
    }
    output(sys_data.output_file, "%d ", negs);
    // print first negative literals, then positive
    complement_head->EmitGround();
    negative.ClearIterator();
    while ((lt = negative.Iterate())) {
      if (lt->has_global_condition || lt->empty_condition)
        // skip expanded literals  
        continue;
      status = predicates[lt->pred]->Status();
      if ((status != DM_INTERNAL) &&
          ((status != DM_DOMAIN) || (sys_data.print_domains == PR_ALL)
           || predicates[lt->pred]->NeedsToBePrinted()))
        {
          lt->EmitGround();
          output(sys_data.output_file, " ");
        }
    }
    while ((lt = negative_conditions.Iterate())) {
      status = predicates[lt->pred]->Status();
      if ((status != DM_INTERNAL) &&
          ((status != DM_DOMAIN) || (sys_data.print_domains == PR_ALL)
           || predicates[lt->pred]->NeedsToBePrinted()))
        {
          lt->EmitGround();
          output(sys_data.output_file, " ");
        }
    }
	
    // and positive
    positive.ClearIterator();
    while ((lt = positive.Iterate())) {
      if (lt->has_global_condition || lt->empty_condition) 
        continue;
      if (!lt->special) {
        status = predicates[lt->pred]->Status();
      } else {
        status = DM_FALSE;
      }    
      if ((status != DM_INTERNAL) &&
          ((status != DM_DOMAIN) || (sys_data.print_domains == PR_ALL)
           || (sys_data.print_domains == PR_POSITIVE)
           || predicates[lt->pred]->NeedsToBePrinted()))
        {
          lt->EmitGround();
          output(sys_data.output_file, " ");
        }
    }
    while ((lt = positive_conditions.Iterate())) {
      status = predicates[lt->pred]->Status();
      if ((status != DM_INTERNAL) &&
          ((status != DM_DOMAIN) || (sys_data.print_domains == PR_ALL)
           || (sys_data.print_domains == PR_POSITIVE)
           || predicates[lt->pred]->NeedsToBePrinted()))
        {
          lt->EmitGround();
          output(sys_data.output_file, " ");
        }
    }
	
    output(sys_data.output_file, "\n");
    // increase the number of outputted atoms
    sys_data.ground_rules++;
    if (!first_ground) {
      first_ground = sys_data.ground_rules;
    }
    num_ground++;
    sys_data.ground_sigma += numlit + 1;

    // print the complement rule
    sys_data.ground_rules++;
    if (!first_ground) {
      first_ground = sys_data.ground_rules;
    }
    num_ground++;
    sys_data.ground_sigma += 2;
    output(sys_data.output_file, "1 ");
    complement_head->EmitGround();
    output(sys_data.output_file, " 1 1 ");
    head->EmitGround();
    output(sys_data.output_file, "\n");
  }
}

void Rule::EmitSmodels2(int gpos, int gneg, int cmp)
{
  Literal *lt = NULL;
  int numlit = 0, negs = 0; //, i =0 ;
  DomainType status = DM_DOMAIN;
  //  int heads = 0;

  if (type != BASICRULE) {
    GroundHeads();
    if (grounded_heads.Size() < 1 && !sys_data.print_empty_choices)  {
      // empty choice rule, don't print
      return;
    }
  }
  
  if (sys_data.normal_output && type != BASICRULE) {
    EmitNormalGroundSmodels2(gpos, gneg, cmp);
    return;
  }
  
  // output rule type
  output(sys_data.output_file, "%d ", type);

  // count the total number of literals in the body
  switch (sys_data.print_domains) {
  case PR_ALL:
    numlit = positive.Size() + negative.Size();
    numlit += positive_conditions.Size() - num_positive_conditions;
    numlit += negative_conditions.Size() - num_negative_conditions;
    numlit -= num_internal_literals;
    negs = negative.Size() + negative_conditions.Size() -
      num_negative_conditions - num_negative_internal_literals;
    break;
  case PR_POSITIVE:
    numlit = positive.Size() + negative.Size() - gneg;
    // domain predicates are already dropped from the condition lists
    numlit += positive_conditions.Size() - num_positive_conditions;
    numlit += negative_conditions.Size() - num_negative_conditions;
    negs = negative.Size() - gneg + negative_conditions.Size() -
      num_negative_conditions;
    break;
  case PR_HEADS:
  case PR_NONE:
    negs = negative.Size() - gneg + negative_conditions.Size() -
      num_negative_conditions; 
    numlit = positive.Size() -gpos + negative.Size() - gneg;
    numlit += positive_conditions.Size() - num_positive_conditions;
    numlit += negative_conditions.Size() - num_negative_conditions;
    break;
  default:
    int_error("really strange value '%d' in print_domains",
              sys_data.print_domains);
  }
  // print the head
  if (type == BASICRULE)
    head->EmitGround();
  else {
    if (grounded_heads.Size() < 1) {
      // trick, emit _false
      output(sys_data.output_file, "1 %ld ", atom_table->Lookup(FALSE_NAME)+1);
    } else {
      output(sys_data.output_file, "%d ", grounded_heads.Size());
      while ((lt = grounded_heads.Iterate())) {
        lt->EmitGround();
      }
    }
  }
  
  // print the domain sizes 
  output(sys_data.output_file, "%d ", numlit);
  if (!numlit && type == GENERATERULE) {
    output(sys_data.output_file, "\n");
    return;
  }
  output(sys_data.output_file, "%d ", negs);
  
  // print first negative literals, then positive
  negative.ClearIterator();
  while ((lt = negative.Iterate())) {
    if (lt->has_global_condition || lt->empty_condition)
      // skip expanded literals  
      continue;
    status = predicates[lt->pred]->Status();
    if ((status != DM_INTERNAL) &&
        ((status != DM_DOMAIN) || (sys_data.print_domains == PR_ALL)
         || predicates[lt->pred]->NeedsToBePrinted()))
      {
        lt->EmitGround();
        output(sys_data.output_file, " ");
      }
  }
  while ((lt = negative_conditions.Iterate())) {
    status = predicates[lt->pred]->Status();
    if ((status != DM_INTERNAL) &&
        ((status != DM_DOMAIN) || (sys_data.print_domains == PR_ALL)
         || predicates[lt->pred]->NeedsToBePrinted()))
      {
        lt->EmitGround();
        output(sys_data.output_file, " ");
      }
  }
  
  // and positive
  positive.ClearIterator();
  while ((lt = positive.Iterate())) {
    if (lt->has_global_condition || lt->empty_condition) 
      continue;
    if (!lt->special) {
      status = predicates[lt->pred]->Status();
    } else {
      status = DM_FALSE;
    }    
    if ((status != DM_INTERNAL) &&
        ((status != DM_DOMAIN) || (sys_data.print_domains == PR_ALL)
         || (sys_data.print_domains == PR_POSITIVE)
         || predicates[lt->pred]->NeedsToBePrinted()))
      {
        lt->EmitGround();
        output(sys_data.output_file, " ");
      }
  }
  while ((lt = positive_conditions.Iterate())) {
    status = predicates[lt->pred]->Status();
    if ((status != DM_INTERNAL) &&
        ((status != DM_DOMAIN) || (sys_data.print_domains == PR_ALL)
         || (sys_data.print_domains == PR_POSITIVE)
         || predicates[lt->pred]->NeedsToBePrinted()))
      {
        lt->EmitGround();
        output(sys_data.output_file, " ");
      }
  }
  
  output(sys_data.output_file, "\n");
  // increase the number of outputted atoms
  sys_data.ground_rules++;
  if (!first_ground) {
    first_ground = sys_data.ground_rules;
  }
  num_ground++;
  sys_data.ground_sigma += numlit + 1;
  
}

void Rule::EmitSmodels1(int gpos, int gneg, int)
{
  Literal *lt = NULL;
  int negs = 0, poss = 0;
  DomainType status = DM_DOMAIN;
  //   Instance head_atom = -1, cmp = -1;

  // count the total number of literals in the body
  switch (sys_data.print_domains) {
  case PR_ALL:
    poss = positive.Size();
    negs = negative.Size();
    break;
  case PR_POSITIVE:
    poss = positive.Size(); 
    negs = negative.Size() - gneg;
    break;
  case PR_HEADS:
  case PR_NONE:
    poss = positive.Size() - gpos;
    negs = negative.Size() - gneg;
    break;
  default:
    int_error("really strange value '%d' in print_domains",
              sys_data.print_domains);
  }
  
  // print the head
  head->EmitGround();
  
  // print the negative domain size
  output(sys_data.output_file, "%d ", negs);

  // print first negative literals, then positive
  while ((lt = negative.Iterate())) {
    status = predicates[lt->pred]->Status();
    if ((status != DM_INTERNAL) &&
        ((status != DM_DOMAIN) || (sys_data.print_domains == PR_ALL)))
      {
        lt->EmitGround();
        output(sys_data.output_file, " ");
      }
  }

  // and positive
  output(sys_data.output_file, "%d ", poss);
    
  while ((lt = positive.Iterate())) {
    status = predicates[lt->pred]->Status();
    if ((status != DM_INTERNAL) &&
        ((status != DM_DOMAIN) || (sys_data.print_domains == PR_ALL)
         || (sys_data.print_domains == PR_POSITIVE)))
      {
        lt->EmitGround();
        output(sys_data.output_file, " ");
      }
  }
  output(sys_data.output_file, "\n");

  // increase the number of outputted atoms
  sys_data.ground_rules++;
  if (!first_ground) {
    first_ground = sys_data.ground_rules;
  }
  num_ground++;
  sys_data.ground_sigma += poss + negs + 1;
}



int count_bits(long n)
{
  int res = 0;
  unsigned i = 0;
  
  for(i=0; i < sizeof(long)*CHAR_BIT; i++) {
    if (n & (1 << i))
      res++;
  }
  return res;
}

long get_subset(int pos, int k)
{
  static int subset = 0;
  int bits = (1 << pos);
  
  while(1) {
    if (++subset > bits){
      subset = 0;
      return 0;
    }
    if (count_bits(subset) == k)
      return subset;
  }
}


void Rule::EmitOrHead()
{
  int i = 0, first = 1;
  long set = 0;
  Rule *rl = NULL;
  Literal *lt = NULL;
  Literal *lt1 = NULL, *lt2 = NULL;
  int gpos = 0, gneg = 0;
  
  if (!false_lit) {
    false_lit = Predicate::DefineFalseLiteral();
  }
  // find all 2-subsets of the head atoms
  rl = new Rule(false_lit, BASICRULE);
  
  // first add the tail to the rule
  while ((lt = positive.Iterate())) {
    rl->AddLiteral(lt);
    if (!lt->special && lt->DomainLiteral())
      gpos++;
  }
  while ((lt = negative.Iterate())) {
    rl->AddLiteral(lt);
    if (!lt->special && lt->DomainLiteral())
      gneg++;
  }
  
  while ((set = get_subset(head_number, 2))) {
    for (i = 0; i < head_number; i++) {
      if (set & (1 << i)) {
        if (first) {
          lt1 = head_array[i];
          rl->AddLiteral(lt1);
          first = 0;
        } else {
          lt2 = head_array[i];
          rl->AddLiteral(lt2);
          first = 1;
        }
      }
    }
    
    rl->EmitGround(gpos,gneg,0);
    rl->positive.Remove(lt1);
    rl->positive.Remove(lt2);
  }

  // then demand that at least one atom should be true
  for (i = 0; i < head_number; i++) 
    rl->AddLiteral(head_array[i]->Duplicate(1));
  
  rl->EmitGround(gpos,gneg,0);

  delete rl;
}
  
void Rule::CalculateOptimize()
{
  Literal *lt = NULL;
  static LiteralList conds;
  static FunctionList funs;
  static Variable *free_vars = NULL;
  int num_free = 0;
  int i = 0;
  long weight = 0;
  const char *buf  = NULL;

  
  vars =  new Variable[sys_data.total_vars+1];
  if (!vars)
    error(SYS_ERR, "malloc error");
  for (i = 0; i < sys_data.total_vars ; i++) {
    vars[i] = VAR_LOCAL;
  }

  if (!free_vars) {
    free_vars = new Variable[sys_data.total_vars+1];
    if (!free_vars)
      error(SYS_ERR, "malloc error");
  }
  
  conds.Clear();
  funs.Clear();

  while ((lt = positive.Iterate())) {
    if (lt->conditions || lt->condition_func) {
      memset(variables, R_NONE, sizeof(Variable) *
             sys_data.total_vars);
      lt->Restrict(R_NEGATIVE_LOCAL);
      for (i=0; i < sys_data.total_vars; i++) {
        switch (variables[i]) {
        case R_NEGATIVE_LOCAL:
        case R_WEIGHT_LOCAL:
          free_vars[num_free++] = i;
          break;
        case R_CONDITION_LOCAL:
          vars[i] = VAR_CONDITION;
          break;
        default:
          break;
        }
      }
      ExpandCondition(lt, &conds, &funs, 1);
      positive.Remove(lt);
    } else {
      if (!lt->ground) {
        error(USR_ERR, "%s: a non-ground literal in optimize statement:",
              error_file_and_line(lt->lineno));
        output(cerr, "\t");
        lt->Print();
        output(cerr, "\n");
      }
    }
  }
  while ((lt = negative.Iterate())) {
    if (lt->conditions || lt->condition_func) {
      memset(variables, R_NONE, sizeof(Variable) *
             sys_data.total_vars);
      lt->Restrict(R_NEGATIVE_LOCAL);
      for (i=0; i < sys_data.total_vars; i++) {
        switch (variables[i]) {
        case R_NEGATIVE_LOCAL:
        case R_WEIGHT_LOCAL:
          free_vars[num_free++] = i;
          break;
        case R_CONDITION_LOCAL:
          vars[i] = VAR_CONDITION;
          break;
        default:
          break;
        }
      }
      ExpandCondition(lt, &conds, &funs, 1);
      negative.Remove(lt);
    } else {
      if (!lt->ground) {
        error(USR_ERR, "%s: a non-ground literal in optimize statement:",
              error_file_and_line(lt->lineno));
        output(cerr, "\tnot ");
        lt->Print();
        output(cerr, "\n");
      }
    }
  }
  if (num_free > 0) {
    error(USR_ERR, "%s: There are free variables in the %s-statement",
          error_file_and_line(line_start), (head->pred) ? "maximize" :
          "minimize"); 
    output(cerr, "\tFree variables: ");
    for (i = 0; i < num_free; i++) {
      output(cerr, "%s ", variable_table->symbols[free_vars[i]]);
    }
    output(cerr, "\n");
    return ;
  }
  
  // add all generated literals to the rule tail
  while ((lt = conds.Iterate())) {
    if (lt->negative)
      negative.Insert(lt);
    else
      positive.Insert(lt);
  }
  
  // then print the rule
  if (sys_data.emit_text) {
    int first = 1;
    output(sys_data.output_file, "%s %s ", (head->pred) ? "maximize"
           : "minimize", (weight_head) ? "[" : "{");
    while ((lt = positive.Iterate())) {
      if (!first)
        output(sys_data.output_file, ", ");
      else 
        first = 0;
      if (weight_head)
        lt->EmitGround(1);
      else
        lt->EmitGround(0);
    }
    while ((lt = negative.Iterate())) {
      if (!first)
        output(sys_data.output_file, ", not ");
      else {
        output(sys_data.output_file, "not ");
        first = 0;
      }
      if (weight_head)
        lt->EmitGround(1);
      else
        lt->EmitGround(0);
    }
    output(sys_data.output_file, " %s.\n", weight_head ? "]" : "}");
  } else { // smodels 2 output
    int inverted_negs = 0, inverted_pos = 0;

    // check for negative weights
    while ((lt = negative.Iterate())) {
      if (weight_head) {
        weight = lt->GetWeight();
      } else {
        weight = 1;
      }
      if (head->pred) { // handle maximizing by inverting the sign. 
        weight = -weight;
      }
      
      if (weight < 0) {
        lt->SetWeight(-weight);
        lt->inverted = 1;
        inverted_negs++;
      } else {
        lt->SetWeight(weight);
      }
    }
    while ((lt = positive.Iterate())) {
      if (weight_head) {
        weight = lt->GetWeight();
      } else {
        weight = 1;
      }
      if (head->pred) { // maximize
        weight = -weight;
      }
      if (weight < 0) {
        lt->SetWeight(-weight);
        lt->inverted = 1;
        inverted_pos++;
      } else {
        lt->SetWeight(weight);
      }
    }
    output(sys_data.output_file, "%d 0 %ld %ld ", type, 
           positive.Size() + negative.Size(),
           negative.Size() + inverted_pos - inverted_negs );

    // FIXME: get some sense to this code. 
    
    // the actual atoms
    while ((lt = negative.Iterate())) {
      if (lt->inverted)
        continue;
      buf = get_ground_instance(lt->cons, lt->pred, lt->arity);
      output(sys_data.output_file,"%ld ", atom_table->Insert(buf)+1);
    }
    while ((lt = positive.Iterate())) {
      if (lt->inverted) {
        buf = get_ground_instance(lt->cons, lt->pred, lt->arity);
        output(sys_data.output_file,"%ld ",
               atom_table->Insert(buf)+1);
      }
    }
    while ((lt = positive.Iterate())) {
      if (!lt->inverted) {
        buf = get_ground_instance(lt->cons, lt->pred, lt->arity);
        output(sys_data.output_file,"%ld ",
               atom_table->Insert(buf)+1);
      }
    }
    while ((lt = negative.Iterate())) {
      if (!lt->inverted)
        continue;
      buf = lt->GetPrintString();
      output(sys_data.output_file,"%ld ", atom_table->Insert(buf)+1);
    }

    // then weights
    while ((lt = negative.Iterate())) {
      if (lt->inverted)
        continue;
      weight = lt->GetWeight();
      
      output(sys_data.output_file,"%ld ", weight);
    }
    while ((lt = positive.Iterate())) {
      if (!lt->inverted)
        continue;
      weight = lt->GetWeight();
      output(sys_data.output_file,"%ld ", weight);
    }
    while ((lt = positive.Iterate())) {
      if (lt->inverted)
        continue;
      weight = lt->GetWeight();
      output(sys_data.output_file,"%ld ", weight);
    }
    while ((lt = negative.Iterate())) {
      if (!lt->inverted)
        continue;
      weight = lt->GetWeight();
      output(sys_data.output_file,"%ld ", weight);
    }    
    output(sys_data.output_file, "\n");
  }
}


void Rule::CalculateCompute()
{
  Literal *lt = NULL;
  static LiteralList conds;
  static FunctionList funs;
  static Variable *free_vars = NULL;
  int num_free = 0;
  int i = 0;
  
  vars =  new Variable[sys_data.total_vars+1];
  if (!vars)
    error(SYS_ERR, "malloc error");
  for (i = 0; i < sys_data.total_vars ; i++) {
    vars[i] = VAR_LOCAL;
  }

  if (!free_vars) {
    free_vars = new Variable[sys_data.total_vars+1];
    if (!free_vars)
      error(SYS_ERR, "malloc error");
  }
  
  conds.Clear();
  funs.Clear();

  while ((lt = positive.Iterate())) {
    if (lt->conditions || lt->condition_func) {
      memset(variables, R_NONE, sizeof(Variable) *
             sys_data.total_vars);
      lt->Restrict(R_NEGATIVE_LOCAL);
      for (i=0; i < sys_data.total_vars; i++) {
        switch (variables[i]) {
        case R_NEGATIVE_LOCAL:
        case R_WEIGHT_LOCAL:
          free_vars[num_free++] = i;
          break;
        case R_CONDITION_LOCAL:
          vars[i] = VAR_CONDITION;
          break;
        default:
          break;
        }
      }
      ExpandCondition(lt, &conds, &funs, 0);
      positive.Remove(lt);
    } else {
      if (!lt->ground) {
        error(USR_ERR, "%s: a non-ground literal in compute statement:",
              error_file_and_line(lt->lineno));
        output(cerr, "\t");
        lt->Print();
        output(cerr, "\n");
      }
    }
  }
  while ((lt = negative.Iterate())) {
    if (lt->conditions || lt->condition_func) {
      memset(variables, R_NONE, sizeof(Variable) *
             sys_data.total_vars);
      lt->Restrict(R_NEGATIVE_LOCAL);
      for (i=0; i < sys_data.total_vars; i++) {
        switch (variables[i]) {
        case R_NEGATIVE_LOCAL:
        case R_WEIGHT_LOCAL:
          free_vars[num_free++] = i;
          break;
        case R_CONDITION_LOCAL:
          vars[i] = VAR_CONDITION;
          break;
        default:
          break;
        }
      }
      ExpandCondition(lt, &conds, &funs, 0);
      negative.Remove(lt);
    } else {
      if (!lt->ground) {
        error(USR_ERR, "%s: a non-ground literal in compute statement:",
              error_file_and_line(lt->lineno));
        output(sys_data.output_file, "\tnot ");
        lt->Print();
      }
    }
  }
  if (num_free > 0) {
    error(USR_ERR, "%s: There are free variables in the compute "
          "statement", error_file_and_line(line_start));
    output(cerr, "\tFree variables: ");
    for (i = 0; i < num_free; i++) {
      output(cerr, "%s ", variable_table->symbols[free_vars[i]]);
    }
    output(cerr, "\n");
    return ;
  }
  
  // add all generated literals to the rule tail
  while ((lt = conds.Iterate())) {
    if (lt->negative)
      negative.Insert(lt);
    else
      positive.Insert(lt);
  }

}

// makes all variables global variables by adding new literal to
// normal body for each 
void Rule::RemoveLocalVars()
{
  Predicate *pr = NULL;
  long pred = -1;
  Rule *rl = NULL;
  Literal *lt = NULL, *new_head = NULL, *lt2 = NULL;
  SpecialLiteral *slt = NULL;
  Literal **new_heads = NULL;
  int i = 0, pos = 0, hd = 0;

  if (ground || !special_lits)
    return;
  
  new_heads = new Literal*[sys_data.total_vars];
  memset(variables, R_NONE, sizeof(Variable) * sys_data.total_vars);
  for (i = 0; i < sys_data.total_vars; i++)
    new_heads[i] = NULL;

  while ((slt = (SpecialLiteral*) special_lits->Iterate())) {
    if (slt->arity > 0) {
      while ((lt = slt->tail.Iterate())) {
        if (lt->DomainLiteral()) {
          for (i = 0; i < lt->arity; i++) {
            if ((lt->args[i] == T_VARIABLE) &&
                vars[lt->vars[i]] == VAR_LOCAL) {
              // a local variable found. construct a new literal for
              // it if necessary, and make a rule for it with this
              // literal.
              if (variables[lt->vars[i]] < 0) {
                variables[lt->vars[i]] = pos;
                pred = Predicate::DefineSystemPredicate();
                pr = predicates[pred];
                pr->SetArity(1);
                new_head = new Literal(lt->vars[i], pred);
                new_heads[pos] = new_head;
                rl = new Rule(new_heads[pos], BASICRULE);
                pos++;
              } else {
                hd = variables[lt->vars[i]];
                pr = predicates[new_heads[hd]->pred];
                rl = new Rule(new_heads[hd], BASICRULE);
              }
              lt2 = lt->Duplicate(0);
              lt2->negative = 0;
              lt2->weight = NULL;
              rl->AddLiteral(lt2);
              pr->AddRule(rl);
            }
          }
        }
      }
    }
  }

  for (i = 0; i < pos; i++) {
    if (new_heads[i]) {
      predicates[new_heads[i]->pred]->CalculateDomain();
      AddLiteral(new_heads[i]);
    }
  }
  delete [] new_heads;
}

const char *Rule::GetInstanceString()
{
  static string str;
  ostringstream ss;
  int pos = 0, i = 0;
  int first = 1;
  Literal *lt;
  ss.str("");
  
  if (label) {
    pos += output(ss, "%s : ", label->GetInstanceString());
  }

  switch (type) {
  case BASICRULE:
    pos += output(ss, "%s", head->GetInstanceString());
    break;
  case CHOICERULE:
    pos += output(ss, "{ ");    
    for (i = 0; i < head_number; i++) {
      if (!head_array[i]->deleted) {
        pos += output(ss, "%s%s",
                      head_array[i]->GetInstanceString(),
                      (i < head_number-1) ? ", " : "");
      }
    }
    pos += output(ss, "} ");    

    break;
  case ORDERED_DISJUNCTION:
    for (i = 0; i < head_number; i++) {
      if (!head_array[i]->deleted) {
        pos += output(ss, "%s%s",
                      head_array[i]->GetInstanceString(),
                      (i < head_number-1) ? " x " : "");
      }
    }
    break;
  default:
    int_error("trying to stringize a strange rule\n", "");
  }
  if (positive.Size() + negative.Size() > 0) {
    pos += output(ss, " :- ");
  }
  
  while (special_lits && (lt = special_lits->Iterate())) {
    if (first) {
      first = 0;
    } else {
      pos += output(ss, ", ");
    }
    pos += output(ss, "%s", lt->GetInstanceString());
  }
  while ((lt = positive.Iterate())) {
    if (first) {
      first = 0;
    } else {
      pos += output(ss, ", ");
    }
    pos += output(ss, "%s", lt->GetInstanceString());
  }
  while ((lt = negative.Iterate())) {
    if (first) {
      first = 0;
    } else {
      pos += output(ss, ", ");
    }
    pos += output(ss, "%s", lt->GetInstanceString());
  }
  pos += output(ss, ".");
  str = ss.str();
  return str.c_str();
}

const char *Rule::GetPrintString()
{
  ostringstream ss;
  if (print_str) {
    return print_str;
  }
  int pos = 0, i = 0;
  int first = 1;
  Literal *lt;
  Function *fun;

  if (label) {
    pos += output(ss, "%s : ", label->GetPrintString());
  }

  
  switch (type) {
  case BASICRULE:
    pos += output(ss, "%s", head->GetPrintString());
    break;
  case CHOICERULE:
    if (atleast) {
      pos += output(ss, "%s ", atleast->GetPrintString());
    }
    pos += output(ss, "{ ");    
    for (i = 0; i < head_number; i++) {
      if (!head_array[i]->deleted) {
        pos += output(ss, "%s%s",
                      head_array[i]->GetPrintString(),
                      (i < head_number-1) ? ", " : "");
      }
    }
    pos += output(ss, "} ");
    if (atmost) {
      pos += output(ss, "%s ", atmost->GetPrintString());
    }

    break;
  case ORDERED_DISJUNCTION:
    for (i = 0; i < head_number; i++) {
      if (!head_array[i]->deleted) {
        pos += output(ss, "%s%s",
                      head_array[i]->GetPrintString(),
                      (i < head_number-1) ? " x " : "");
      }
    }
    break;
  case DISJUNCTIVERULE:
    for (i = 0; i < head_number; i++) {
      if (!head_array[i]->deleted) {
        pos += output(ss, "%s%s",
                      head_array[i]->GetPrintString(),
                      (i < head_number-1) ? " | " : "");
      }
    }
    break;
  default:
    int_error("trying to stringize a strange rule\n", "");
  }

  if (positive.Size() + negative.Size() > 0) {
    pos += output(ss, " :- ");
  }
  
  while (special_lits && (lt = special_lits->Iterate())) {
    if (first) {
      first = 0;
    } else {
      pos += output(ss, ", ");
    }
    pos += output(ss, "%s", lt->GetPrintString());
  }
  while ((lt = positive.Iterate())) {
    if (!predicates[lt->pred]->Internal()) {
      if (first) {
        first = 0;
      } else {
        pos += output(ss, ", ");
      }
      pos += output(ss, "%s", lt->GetPrintString());
    }
  }
  while ((lt = negative.Iterate())) {
    if (first) {
      first = 0;
    } else {
      pos += output(ss, ", ");
    }
    pos += output(ss, "%s", lt->GetPrintString());
  }

  while ((fun = functions.Iterate())) {
    if (first) {
      first = 0;
    } else {
      pos += output(ss, ", ");
    }
    pos += output(ss, "%s", fun->GetPrintString());
  }
  
  print_str = clone_string(ss.str().c_str());
  return print_str;
}

	  
void Rule::FindWeights()
{
#if 0
  if (weight_head) {
    for (int i = 0; i < head_number; i++) {
      // define weights
      head_array[i]->FindWeight();
      if (weight_head) {
        if (head_array[i]->weight == default_weight) {
          warn(WARN_WEIGHT, head_array[i]->lineno,
               "default weight used for atom '%s' in rule head",
               head_array[i]->GetPrintString());
        }
      }
    }
  }

  if (special_lits) {
    SpecialLiteral *slt = NULL;
    while ((slt = (SpecialLiteral*)special_lits->Iterate())) {
      if (slt->type == WEIGHTRULE) {
        slt->FindWeights();
      }
    }
  }
#endif
}

Rule *Rule::CreateComplement()
{
  Rule *new_rule = NULL;
  Literal *new_head = NULL;
  Literal *new_lit = NULL;
  

  if (type == BASICRULE) {
    new_head = head->CreateComplement();
    new_rule = new Rule(new_head, BASICRULE);
  } else {
    new_rule = new Rule(type, head_number);
  }
  if (!new_rule)
    error(SYS_ERR, "malloc error");
  
  new_rule->head_number = head_number;
  if (atleast)
    new_rule->atleast = atleast->Duplicate();
  else
    new_rule->atleast = NULL;

  if (atmost)
    new_rule->atmost = atmost->Duplicate();
  else
    new_rule->atleast = NULL;
  
  new_rule->line_start = line_start;
  new_rule->line_end = line_end;
  new_rule->weight_head = weight_head;
  new_rule->ground = ground;
  new_rule->delete_all = delete_all;
  new_rule->deleted = deleted;
  new_rule->num_weights = num_weights;
  new_rule->num_assigns = num_assigns;
  new_rule->num_conditions = num_conditions;
  new_rule->num_positive_conditions = num_positive_conditions;
  new_rule->restrict_status = restrict_status;
  new_rule->special = special;
  
  if(type == CHOICERULE) {
    for (int i = 0 ; i < head_number; i++) {
      new_rule->head_array[i] = head_array[i]->CreateComplement();
    }
  }
  
  Literal *lt = NULL;
  while ((lt = positive.Iterate())) {
    new_lit = lt->CreateComplement();
    new_rule->AddLiteral(new_lit);
  }

  while ((lt = negative.Iterate())) {
    new_lit = lt->Duplicate(0);
    lt->pred = predicates[lt->pred]->complement;
    new_rule->AddLiteral(new_lit);
  }

  Function *fun = NULL;
  while ((fun = functions.Iterate())) {
    new_rule->AddFunction(fun->DuplicateFunction());
  }
  return new_rule;
}

void Rule::DefineImplicitDomain()
{
  Literal *lt = NULL;
  Literal *l = NULL;
  Function *fun = NULL;
  bool all_present;
  int i;
  memset(variables, 0, sys_data.total_vars * sizeof(Variable));

  if (type == BASICRULE) {
    head->AddVarsByType(variables);
  } else {
    for (i = 0; i < head_number; i++) {
      head_array[i]->AddVarsByType(variables);
    }
  }

  positive.ClearIterator();
  while ((lt = positive.Iterate())) {
    lt->AddVarsByType(variables);
  }
  negative.ClearIterator();
  while ((lt = negative.Iterate())) {
    lt->AddVarsByType(variables);
  }
  functions.ClearIterator();
  while ((fun = functions.Iterate())) {
    fun->AddVarsByType(variables);
  }
  
  while ((lt = implicit_domain_list->Iterate())) {
    all_present = true;
    for (i = 0; i < lt->arity; i++) {
      if (variables[lt->vars[i]] == 0) {
        all_present = false;
      }
    }
    if (all_present) {
      l = lt->Duplicate();
      AddLiteral(l);
      if (type == BASICRULE) {
        dependency_graph->AddEdge(head->pred, l->pred, false);
      } else {
        for (i = 0; i < head_number; i++) {
          dependency_graph->AddEdge(head_array[i]->pred, l->pred, false);
        }
        
      }

      for (i = 0; i < lt->arity; i++) {
        if (variables[lt->vars[i]] == R_CONDITION) {
          warn(WARN_DOMAIN, line_start, "The variable '%s' occurs as a local "
               "variable in a condition even though it has an implicit "
               "domain defined for it with a #domain-statement. "
               "You probably should use a different name for the variable.",
               variable_table->symbols[i]);          
        }
      }
    }
  }
}

void Rule::EmitPriority(int gpos, int gneg)
{
  Rule *r;
  choice->split.ClearIterator();
  choice->UpdateConstants();
  while ((r = choice->split.Iterate())) {
    r->EmitGround(gpos, gneg, 0);
  }
}

void Rule::EmitMetaPriority(int gpos, int gneg)
{
  static bool has_error = false;
  const char *buf;
  char  *first, *second;
  long i1, i2;
  long a, b, i;
  Instance it[2];

  TermType args[2];
  long vs[2];
  long cons[2];

  if (has_error)
    return;
  
  buf = head->GetGroundInstance();
  Predicate::decode_atom(buf, it);

  first = constant_table->LookupByValue(it[0]);
  second = constant_table->LookupByValue(it[1]);

  if (!first || !second) {
    error(FATAL_ERR, "Undefined metapriority label in `%s'", buf);
    has_error = 1;
    return;
  }
  
  i1 = priority_data.label_table->Lookup(first);
  i2 = priority_data.label_table->Lookup(second);

  if ((i1 > ERROR_NUMBER) && (i2 > ERROR_NUMBER)) {
  
    for (i = 0; i < 2; i++) {
      args[i] = head->args[i];
      cons[i] = head->cons[i];
      vs[i] = head->vars[i];
      head->args[i] = T_CONSTANT;
      head->vars[i] = -1;
    }
    
    while ((a = priority_data.labels[i1]->Iterate())) {
      while ((b = priority_data.labels[i2]->Iterate())) {
        head->cons[0] = a;
        head->cons[1] = b;
        if (sys_data.emit_text)
          EmitText(gpos, gneg, 0);
        else
          EmitSmodels2(gpos,gneg, 0);
      }
    }
    for (i = 0; i < 2; i++) {
      head->args[i] = args[i];
      head->cons[i] = cons[i];
      head->vars[i] = vs[i];
    }
  }
}

int Rule::CheckTyping()
{
  // stub
  return 0;
}

void Rule::PrintDebugInformation()
{
  if (num_ground > 0) {
    output(sys_data.output_file, "%% %ld %ld: ", first_ground, num_ground);
    output(sys_data.output_file, "%s", GetPrintString());
    output(sys_data.output_file, "\n");
  }
}

  
