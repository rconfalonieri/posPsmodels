// literal.cc -- implementation of literals
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
#include <string.h>

#include "config.h"
#ifndef DEBUG_H
#include "debug.h"
#endif
#ifndef LITERAL_H
#include "literal.h"
#endif
#ifndef GLOBAL_H
#include "global.h"
#endif
#ifndef PARSETREE_H
#include "parsetree.h"
#endif
#ifndef INSTANCE_H
#include "instance.h"
#endif
#ifndef SYMBOL_H
#include "symbol.h"
#endif
#ifndef ARRAY_H
#include "array.h"
#endif
#ifndef PREDICATE_H
#include "predicate.h"
#endif
#ifndef ERROR_H
#include "error.h"
#endif

Literal::Literal(ParseNode *nd, long p, int ar)
{
  ParseNode *tmp = NULL;
  int pos = ar -1; // for reverse travelsal
  Term *tm = NULL;
  ground = 1;
  has_range = 0;
  has_function = 0;
  has_atom = 0;
  negative = nd->l2.negative;
  arity = ar;
  numvars = 0;
  inverted = 0;
  pred = p;
  deleted = 0;
  lineno = nd->lineno;
  weight = NULL;
  dynamic_weight = NULL;
  conditions = NULL;
  condition_func = NULL;
  has_global_condition = 0;
  ignore_global_condition = 0;
  empty_condition = 0;
  special = 0;
  is_warned = 0;
  is_symbolic_function = 0;
  
  if (arity > 0) {
    args = new TermType[arity];
    vars = new long[arity];
    cons = new Instance[arity];
    terms = new Term*[arity];
    if (!args || !vars || !cons || !terms)
      error(SYS_ERR, "malloc error");

    tmp = nd->right->left;
    // check the arguments
    while (tmp && (pos >= 0)) {
      if (!tmp->left)
        int_error("missing argument", "");
      
      tm = Term::ConstructArgument(tmp->left);
      if (!tm)
        int_error("null term", "");
      switch(tmp->left->type) {
      case PT_ATOM:
        numvars += tmp->left->ext.lit->numvars;
        if (tmp->left->ext.lit->numvars > 0)
          ground = 0;
        args[pos] = T_ATOM;
        vars[pos] = ERROR_NUMBER;
        cons[pos] = ERROR_NUMBER;
        terms[pos] = tm;
        has_atom = 1;
        break;
      case PT_VARIABLE: 
        numvars++;
        ground = 0;
        args[pos] = T_VARIABLE;
        vars[pos] = tm->val;
        cons[pos] = ERROR_NUMBER;
        terms[pos] = NULL;
        delete tm;
        break;
      case PT_NUMBER:
      case PT_CONSTANT:
        args[pos] = T_CONSTANT;
        vars[pos] = ERROR_NUMBER;
        cons[pos] = tm->val;
        terms[pos] = NULL;
        delete tm;
        break;
      case PT_FUNCTION:
      case PT_EXPR:
        args[pos] = T_FUNCTION;
        has_function = 1;
        vars[pos] = ERROR_NUMBER;
        cons[pos] = ERROR_NUMBER;
        terms[pos] = tm;
        ((Function*)tm)->AddDependencies(pred);
        if (!tm->ground)
          ground = 0;
        break;
      case PT_RANGE:
        has_range = 1;
        args[pos] = T_RANGE;
        vars[pos] = ERROR_NUMBER;
        cons[pos] = ERROR_NUMBER;
        terms[pos] = tm;
        break;
      default:
        int_error("misformed parse tree: invalid literal argument type"
                  "'%s'", parse_strings[tmp->left->type]);
      }
      tmp = tmp->right;
      pos--;
    }
  } else {
    args = NULL;
    vars =  NULL;
    cons =  NULL;
    terms = NULL;
  }
}


Literal::Literal(Variable v, long p )
{
  arity = 1;
  numvars = 1;
  ground = 0;
  negative = 0;
  has_global_condition = 0;
  ignore_global_condition = 0;
  empty_condition = 0;
  has_atom = 0;
  deleted = 0;
  has_range = 0;
  has_function = 0;
  pred = p;
  lineno = -1;
  special = 0;
  inverted = 0;
  is_warned = 0;
  is_symbolic_function = 0;
  args = new TermType[1];
  vars = new Variable[1];
  cons = new Instance[1];
  terms = new Term*[1];

  weight = NULL;
  dynamic_weight = NULL;
  conditions = NULL;
  condition_func = NULL;
 
  if (!args || !vars || !cons  || !terms)
    error(SYS_ERR, "malloc error");

  *args = T_VARIABLE;
  *vars = v;
  *cons = ERROR_NUMBER;
  *terms = NULL;
}

Literal::Literal(Variable *v, long p, int ar )
{
  int i = 0;
  special = 0;
  arity = ar;
  numvars = ar;
  deleted = 0;
  ground = 0;
  negative = 0;
  has_range = 0;
  has_function = 0;
  inverted = 0;
  pred = p;
  has_global_condition = 0;
  ignore_global_condition = 0;
  empty_condition = 0;
  lineno = -1;
  weight = NULL;
  dynamic_weight = NULL;
  conditions = NULL;
  has_atom = 0;
  condition_func = NULL;
  
  args = new TermType[ar];
  vars = new Variable[ar];
  cons = new Instance[ar];
  terms = new Term*[ar];

  if (!args || !vars || !cons  || !terms)
    error(SYS_ERR, "malloc error");

  for (i = 0; i < arity; i++) {
    args[i] = T_VARIABLE;
    vars[i] = v[i];
    cons[i] = ERROR_NUMBER;
    terms[i] = NULL;
  }
}

Literal::Literal(long p, Instance *it, int ar, int neg)
{
  int i;
  special = 0;
  ground = 1;
  has_range = 0;
  has_function = 0;
  has_global_condition = 0;
  ignore_global_condition = 0;
  empty_condition = 0;
  negative = neg;
  arity = ar;
  numvars = 0;
  pred = p;
  lineno = -1;
  deleted = 0;
  is_warned = 0;
  is_symbolic_function = 0;
  weight = NULL;
  dynamic_weight = NULL;
  inverted = 0;
  conditions = NULL;
  has_atom = 0;
  condition_func = NULL;
  
  if (arity > 0) {
    args = new TermType[arity];
    vars = new long[arity];
    cons = new Instance[arity];
    terms = new Term*[arity];
    if (!args || !vars || !cons || !terms)
      error(SYS_ERR, "malloc error");
  } else {
    args = NULL;
    vars =  NULL;
    cons =  NULL;
    terms = NULL;
  }
  for (i = 0; i < arity; i++) {
    args[i] = T_CONSTANT;
    cons[i] = it[i];
    vars[i] = ERROR_NUMBER;
    terms[i] = NULL;
  }
}
  
Literal::Literal()
{
  arity = 0;
  numvars = 0;
  special = 0;
  ground = 1;
  negative = 0;
  has_range = 0;
  has_function = 0;
  has_atom = 0;
  has_global_condition = 0;
  ignore_global_condition = 0;
  empty_condition = 0;
  deleted = 0;
  pred = 0;
  lineno = 0;
  is_warned = 0;
  is_symbolic_function = 0;
  inverted = 0;
  has_atom = 0;
  args = NULL;
  vars = NULL;
  cons = NULL;
  terms = NULL;
  weight = NULL;
  dynamic_weight = NULL;
  conditions = NULL;
  condition_func = NULL;
}

Literal *Literal::Duplicate(int force_negative, int include_conditions)
{
  int i = 0;
  Literal *new_lit = new Literal(), *tmp = NULL;
  Function *fun = NULL;
  if (!new_lit)
    error(SYS_ERR, "malloc error");
  
  new_lit->arity = arity;
  new_lit->numvars = numvars;
  new_lit->ground = ground;
  if (force_negative)
    new_lit->negative = force_negative;
  else
    new_lit->negative = negative;
  new_lit->lineno = lineno;
  new_lit->has_range = has_range;
  new_lit->has_function = has_function;
  new_lit->pred = pred;
  new_lit->has_atom = has_atom;
  new_lit->weight = weight;
  new_lit->dynamic_weight = dynamic_weight;
  new_lit->inverted = inverted;
  new_lit->deleted = deleted;
  new_lit->has_global_condition = has_global_condition;
  new_lit->ignore_global_condition = ignore_global_condition;
  new_lit->is_symbolic_function = is_symbolic_function;
  new_lit->empty_condition = 0;
  if (include_conditions && conditions) {
    new_lit->conditions = new LiteralList(UNORDERED);
    while ((tmp = conditions->Iterate())) {
      new_lit->conditions->Insert(tmp->Duplicate(0));
    }
  } else
    new_lit->conditions = NULL;

  if (include_conditions &&condition_func) {
    new_lit->condition_func = new FunctionList(UNORDERED);
    while ((fun = condition_func->Iterate()))
      new_lit->condition_func->Insert(fun->DuplicateFunction());
  } else
    new_lit->condition_func = NULL;
    
  new_lit->special = special;
  
  if (arity > 0) {
    new_lit->args = new TermType[arity];
    new_lit->vars = new long[arity];
    new_lit->cons = new Instance[arity];
    new_lit->terms = new Term*[arity];
    if (!new_lit->args || !new_lit->vars || !new_lit->cons ||
        !new_lit->terms) 
      error(SYS_ERR, "malloc error");
  } else {
    new_lit->args = NULL;
    new_lit->vars =  NULL;
    new_lit->cons =  NULL;
    new_lit->terms = NULL;
  } 
  for (i = 0; i < arity; i++) {
    new_lit->args[i] = args[i];
    new_lit->vars[i] = vars[i];
    new_lit->cons[i] = cons[i];
    if (terms[i])
      new_lit->terms[i] = terms[i]->Duplicate();
    else
      new_lit->terms[i] = NULL;
  }
  return new_lit;
}


// returns 1 if the literal is known to be true, 0 otherwise
int Literal::Test()
{
  int i = 0;
  static Instance *item = NULL;

  if (!item) {
    item = new Instance[predicate_table->MaxArity()];
    if (!item)
      error(SYS_ERR, "malloc error");
  }

  if (arity == 0) {
    if (predicates[pred]->follows)
      return 1;
    else
      return 0;
  }
  
  for (i = 0; i < arity; i++) {
    switch (args[i]) {
    case T_VARIABLE:
      item[i] = variables[vars[i]];
      break;
    case T_CONSTANT:
      item[i] = cons[i];
      break;

    default:
      int_error("misformed literal: invalid argument type '%d'",
                args[i]);
    }
  }
  if (predicates[pred]->atoms->Lookup(item) >= 0)
    return 1;
  else
    return 0;
}
    
int Literal::GetPos()
{
  int res = 0, i = 0;

  for (i = 0 ; i < arity; i++) {
    if (args[i] == T_VARIABLE)
      res = max(res, variables[vars[i]]);
  }

  return res;
}

const char *Literal::GetGroundInstance()
{
  //  static char buf[BUFFER_LENGTH] = { 0 };
  static string str;
  ostringstream ss;
  static Instance *item = NULL;
  static int saved_arity = 0;
  int i;
  long pos = 0;
  ss.str("");
  if (!item || sys_data.term_max_arity > saved_arity) {
    if (item)
      delete [] item;
    item = new Instance[sys_data.term_max_arity];
    if (!item)
      error(SYS_ERR, "malloc error");
    saved_arity = sys_data.term_max_arity;
  }
  for (i = 0; i < arity; i++) {
    switch (args[i]) {
    case T_VARIABLE:
      item[i] = variables[vars[i]];
      break;
    case T_CONSTANT:
      item[i] = cons[i];
      break;
    default:
      int_error("invalid term type '%d' in Literal::EmitGround()",
                args[i]);
    }
  }
  pos += output(ss, "%s", get_ground_instance(item, pred, arity));

  str = ss.str();
  return str.c_str();
}

Instance Literal::EmitGround(int print_weight)
{
  static Instance *item = NULL;
  static int saved_arity = 0;
  
  int i = 0;
  Instance it = ERROR_NUMBER;

  if (!item || sys_data.term_max_arity > saved_arity) {
    if (item)
      delete [] item;
    item = new Instance[sys_data.term_max_arity];
    if (!item)
      error(SYS_ERR, "malloc error");
    saved_arity = sys_data.term_max_arity;
  }

  sys_data.ground_sigma++;

  for (i = 0; i < arity; i++) {
    switch (args[i]) {
    case T_VARIABLE:
      item[i] = variables[vars[i]];
      break;
    case T_CONSTANT:
      item[i] = cons[i];
      break;
    default:
      int_error("invalid term type '%d' in Literal::EmitGround()",
                args[i]);
    }
  }
  it = print_instance(item, pred, arity);
  if (print_weight && sys_data.emit_text) {
    output(sys_data.output_file, "=");
    //    FindWeight();
    EmitWeight(inverted);
    //   weight->Emit(inverted);
  }
  
  return it;
}

void Literal::EmitWeight(int inv) {
  FindWeight();
  if (weight) {
    weight->Emit(inv);
  } else if (dynamic_weight) {
    dynamic_weight->Emit(inv);
  } else {
    int_error("a literal without weight", "");
  }
}

void Literal::Restrict(int level)
{
  Literal *lt = NULL;
  Function *fun = NULL;
  int i = 0;
  
  // process conditions first, to find out possible free variables. 
  if (conditions) {
	conditions->ClearIterator();
    while (( lt = conditions->Iterate())) {
      lt->Restrict(R_CONDITIONED);
    }
  }
  if (condition_func) {
	condition_func->ClearIterator();
    while (( fun = condition_func->Iterate())) {
      if (!fun->assign)
		fun->Restrict(R_NEGATIVE_LOCAL); 
      else
		fun->Restrict(R_CONDITIONED);
    }
  }
  
  if (weight){
    switch (weight->type) {
    case WT_NUMBER:
      break;
    case WT_VARIABLE:
      if (MAKE_GLOBAL(variables[weight->v.var]) < R_NEGATIVE) {
        if (GLOBAL_VARIABLE(variables[weight->v.var])) {
          variables[weight->v.var] = R_WEIGHT;
        } else {
          variables[weight->v.var] = R_WEIGHT_LOCAL;
        }
      }
      break;
    case WT_EXPR:
      weight->v.fun->Restrict(R_WEIGHT_LOCAL);
      break;
    case WT_ATOM:
      weight->v.lit->Restrict(R_WEIGHT_LOCAL);
      break;
    }
  }

  for (i = 0; i < arity; i++) {
    if (vars[i] >= 0) {
      if (variables[vars[i]] == R_NONE) {
        variables[vars[i]] = level;
      } else {
        if ((variables[vars[i]] == R_CONDITION_LOCAL) &&
            ((level == R_NEGATIVE_LOCAL) || (level == R_NEGATIVE))) {
          if (level == R_NEGATIVE)
            variables[vars[i]] = R_NC;
          else
            variables[vars[i]] = R_NC_LOCAL;
        } else { 	  
          if (level != R_CONDITIONED) {
            if ((GLOBAL_VARIABLE(variables[vars[i]]) || 
                 !GLOBAL_VARIABLE(level)) &&
                (variables[vars[i]] < level)) {
              variables[vars[i]] = level;
            } else { // it may be a local variable with global level
              if (GLOBAL_VARIABLE(level) &&
                  (MAKE_GLOBAL(variables[vars[i]]) < level))
                variables[vars[i]] = level;
            }
          }
        }
      }
    }
  }

  if (level == R_CONDITIONED)
    return;

  // finally go through all variables and clear condition variables
  for (i = 0 ; i < sys_data.total_vars; i++) {
    if (variables[i] == R_CONDITIONED)
      variables[i] = R_CONDITION_LOCAL;
  }
}

void SpecialLiteral::RestrictSpecial()
{
  Literal *lt = NULL;
  Function *fun = NULL;

  if (atleast) {
    atleast->Restrict(R_NEGATIVE);
  }
  if (atmost) {
    atmost->Restrict(R_NEGATIVE);
  }
  
  while ((lt = tail.Iterate())) {
    if (lt->special) // sanity check
      int_error("nested extended tail sections", "");

    if (lt->negative)
      lt->Restrict(R_NEGATIVE_LOCAL);
    else if (lt->DomainLiteral()) {
      lt->Restrict(R_DOMAIN_LOCAL);
    } else
      lt->Restrict(R_POSITIVE_LOCAL);
  }
  while ((fun = funs.Iterate())) {
    fun->Restrict(R_NEGATIVE_LOCAL );
  }
}


Instance *Literal::GetInstance()
{
  static Instance *it = NULL;
  int i = 0;
  
  if (! it) {
    it = new Instance[predicate_table->MaxArity()];
    if (! it)
      error(SYS_ERR, "malloc error");
  }

  for (i = 0; i < arity; i++) {
    if (args[i] == T_VARIABLE) 
      it[i] = variables[vars[i]];
    else if (args[i] == T_CONSTANT)
      it[i] = cons[i];
    else 
      int_error("misformed literal", "");
  }
  return it;
}

void Literal::CreateInstance(int dtype)
{
  Instance *it = NULL;

  it = GetInstance();
  switch (dtype) {
  case GRD_DOMAIN:
    predicates[pred]->AddInstance(it);
    break;
  case GRD_CONDITION:
    condition_set->Insert(it);
    break;
  default:
    int_error("unknown grounding type", "");
    break;
  }
}

Weight *Literal::WeightDefined()
{
  if (weight)
    return weight;
  else 
    return NULL;
}


void Literal::FindWeight()
{
  WeightNode *wn = predicates[pred]->weight_list;
  
  // don't override local definitions
  if (weight) 
    return;

  if (dynamic_weight) {
    if (dynamic_weight != default_weight) {
      delete dynamic_weight;
    }
    dynamic_weight = NULL;
  }
  
  // first try to find a weight definition from the predicate's list
  // of weight definitions. 
  while (wn) {
    if ((wn->lt->lineno <= lineno) && Matches(this, wn->lt)) {
      if (! wn->wt)
        int_error("missing weight definition", "");

      if (wn->wt->type == WT_ATOM) {
        if (ground) {
          weight = wn->wt->TieWeight(this, wn->wt->v.lit);
        } else {
          dynamic_weight = wn->wt->TieWeight(this, wn->wt->v.lit);
        }
      } else {
        if (ground) {
          weight = wn->wt->Instantiate(this, wn->lt);
        } else {
          dynamic_weight = wn->wt->Instantiate(this, wn->lt);
        }
      }
      return;
    }
    wn = wn->prev;
  }

  // if the predicate is negative and doesn't have a definition by
  // now, try to find a definition for a positive literal, instead
  if (negative && sys_data.default_negative_weight_to_positive) {
    negative = 0;
    wn =  predicates[pred]->weight_list;
    while (wn) {
      if ((wn->lt->lineno <= lineno) && Matches(this, wn->lt)) {
        if (! wn->wt)
          int_error("missing weight definition", "");
        if (wn->wt->type == WT_ATOM) {
          if (ground) {
            weight = wn->wt->TieWeight(this, wn->wt->v.lit);
          } else {
            dynamic_weight = wn->wt->TieWeight(this, wn->wt->v.lit);
          }
        } else {
          if (ground) {
            weight = wn->wt->Instantiate(this, wn->lt);
          } else {
            dynamic_weight = wn->wt->Instantiate(this, wn->lt);
          }
        }
        negative = 1;
        return;
      }
      wn = wn->prev;
    }
    negative = 1;
  }

  if (ground) 
    weight = default_weight;
  else
    dynamic_weight = default_weight;
  return;
}

Variable *m_variables = 0;
Variable *m_var_pos = 0;

int Literal::Matches(Literal *lt1, Literal *lt2)
{
  int i = 0, fail = 0;

  if (lt1->pred != lt2->pred) {
    int_error("trying to unify different predicates", "");
  }

  // check if both are positive or both are negative
  if (lt1->negative != lt2->negative) {
    return 0;
  }
  
  if (!m_var_pos) {
    m_variables = new Variable[sys_data.total_vars];
    m_var_pos = new Variable[sys_data.total_vars];
  }
  /* the idea is to make an unification. The 'm_variables' and 'm_var-pos'
     arrays will be used to hold the substitution information.
     'm_var_pos' holds the type of the substituted term, and 'm_variables'
     the actual value.
  */

  for (i = 0; i < sys_data.total_vars; i++) {
    m_variables[i] = VAR_NONE;
    m_var_pos[i] = SUB_NONE;
  }
  for (i=0; i < lt1->arity && !fail; i++) {
    switch (lt1->args[i]) {
    case T_CONSTANT:
      switch (lt2->args[i]) {
      case T_CONSTANT:
        if (lt1->cons[i] != lt2->cons[i])
          fail = 1;
        break;
      case T_VARIABLE:
        if (!Substitute(lt2->vars[i], lt1->cons[i], SUB_CONSTANT))
          fail = 1;
        break;
	
      default:
        int_error("I don't like ranges of functions in unification", "");
        break;
      }
      break; 
    case T_VARIABLE:
      switch (lt2->args[i]) {
      case T_CONSTANT:
        if (!Substitute(lt1->vars[i], lt2->cons[i], SUB_CONSTANT))
          fail = 1;
        break;
      case T_VARIABLE:
        if (!Substitute(lt1->vars[i], lt2->vars[i], SUB_VARIABLE))
          fail = 1;
        break;
      default:
        int_error("I don't like ranges of functions in unification", "");
        break;
      }
      break;
    default:
      int_error("no, not this", "");
      break;
    }
  }
  if (fail)
    return 0;
  else
    return 1;
}

// this assumes that variables and constants are identical. Ugly
int Literal::Substitute(Variable var, Variable val, int val_type)
{
  int res = 0;

  if (val_type == SUB_CONSTANT) {
    // first, check that if var has a ground value already that it is
    // equivalent to 'val'
    if (variables[var] > ERROR_NUMBER) {
      if (variables[var] != val) {
        return 0;
      }
    }
    
    // If var has an old value check whether it can be unified with
    // the new value
    switch (m_var_pos[var]) {
    case SUB_VARIABLE:
    case SUB_NONE:
      m_var_pos[var] = SUB_CONSTANT;
      m_variables[var] = val;
      res = 1;
      break;
    case SUB_CONSTANT:
      if (m_variables[var] != val)
        res = 0;
      break;
    default:
      int_error("unknown substitution type", "");
    }
  } else { // SUB_VARIABLE
    m_variables[var] = m_variables[val];
    m_var_pos[var] = m_var_pos[val];
    res = 1;
  }

  return res;
}
    
  

void Literal::Print(int print_weight)
{
  //  int first = 1;
  // Literal *lt = NULL;
  //Function *fun = NULL;

  if (special) {
    ( (SpecialLiteral*) this)->PrintSpecial();
    return;
  }
    
  const char *buf = GetPrintString();
  output(cerr, "%s", buf);

  //   if (conditions || condition_func) {
  //     fprintf(cerr, ": ");
  //     while (conditions && (lt = conditions->Iterate())) {
  //       if (!first)
  // 	fprintf(cerr, ": ");
  //       else
  // 	first = 0;
      
  //       lt->Print();
  //     }

  //     while (condition_func && (fun = condition_func->Iterate())) {
  //       if (!first)
  // 	fprintf(cerr, ": ");
  //       else
  // 	first = 0;
  //       fun->Print();
  //     }
  //   }


  if (print_weight) {
    if (!weight)
      weight  = WeightDefined();
    output(cerr, "=");
    if (weight) {
      switch (weight->type) {
      case WT_NUMBER:
        output(cerr, "%ld", inverted ? -weight->v.val : weight->v.val);
        break;
      case WT_VARIABLE:
        output(cerr, "%s", variable_table->symbols[weight->v.var]);
        break;
      case WT_EXPR:
        weight->v.fun->Print();
        break;
      case WT_ATOM:
        weight->v.lit->Print();
        break;
      }
    } else {
      output(cerr, "1");
    }
  }
}

const char *Literal::GetInstanceString()
{
  Instance *item = GetInstance();
  return get_ground_instance(item, pred, arity);
}

long Weight::GetValue()
{
  switch (type) {
  case WT_NUMBER:
    return v.val;
    break;
  case WT_VARIABLE:
    if (IS_NUMBER(variables[v.var])) {
      return variables[v.var];
    } else {
      error(FATAL_ERR, "%s: Runtime Error: weight expression %s's "
            "value is not a number, but a constant '%s'",
            error_file_and_line(lineno),
            variable_table->symbols[v.var],
            constant_table->symbols[MAKE_NUMBER(variables[v.var])]);
    }
    break;
  case WT_EXPR:
    return v.fun->Call();
    break;
  case WT_ATOM:
    int_error("tied weight not resolved correctly", "");
  }
  return 0;
}
  

void Weight::Emit(int invert)
{
  long wt = 0;

  switch (type) {
  case WT_NUMBER:
    wt = v.val;
    break;
  case WT_VARIABLE:
    if (IS_NUMBER(variables[v.var])) {
      wt = variables[v.var];
    } else {
      error(FATAL_ERR, "%s: Runtime Error: weight expression %s's "
            "value is not a number, but a constant '%s'",
            error_file_and_line(lineno),
            variable_table->symbols[v.var],
            constant_table->symbols[MAKE_NUMBER(variables[v.var])]);
    }
    break;
  case WT_EXPR:
    wt = v.fun->Call();
    break;
  case WT_ATOM:
    wt = v.lit->GetWeight();
    break;
  }
  if (invert)
    wt = -wt;
  
  output(sys_data.output_file, "%ld", wt);
}

void Literal::SetWeight(long value)
{
  if (weight) {
    weight->type = WT_NUMBER;
    weight->v.val = value;
  } else if (dynamic_weight) {
    dynamic_weight->type = WT_NUMBER;
    dynamic_weight->v.val = value;
    weight = dynamic_weight;
    dynamic_weight = NULL;
  } else {
    weight = new Weight;
    weight->type = WT_NUMBER;
    weight->v.val = value;
  }
  return;
}



long Literal::GetWeight()
{
  FindWeight();
  if (weight) {
    return weight->GetValue();
  } else if (dynamic_weight) {
    return dynamic_weight->GetValue();
  } else {
    int_error("missing weight", "");
  }
  return 0;
}

const char *Literal::GetPrintString(int first_pos)
{
  static string str;
  ostringstream ss;
  
  char *separ = "";
  int pos = first_pos, i = 0;
  int first = 1;
  Range *r = NULL;
  ss.str("");
  if (special) {
    SpecialLiteral *sl = (SpecialLiteral*) this;
    if (sl->atleast) {
      pos += output(ss, "%s ", sl->atleast->GetPrintString());
    }
    if (sl->type == WEIGHTRULE) {
      ss << "[ ";
    } else {
      ss << "{ ";
    }
    Literal *lt = 0;
    separ = "";
    while ((lt = sl->tail.Iterate())) {
      pos += output(ss, "%s%s", separ, lt->GetPrintString(pos));
      separ = ", ";
    }
    if (sl->type == WEIGHTRULE) {
      ss << " ]";
    } else {
      ss << " } ";
    }
    if (sl->atmost) {
      pos += output(ss, " %s ", sl->atmost->GetPrintString());
    }
  } else {
    if (negative) {
      pos += output(ss, "not ");
    }
    pos += output(ss, "%s", predicates[pred]->Name());
    if (arity > 0) {
      pos += output(ss, "(");
      for (i = 0; i < arity; i++) {
        if (!first)
          pos += output(ss, ",");
        first = 0;
	
        switch (args[i]) {
        case T_VARIABLE:
          pos += output(ss, "%s",
                         variable_table->symbols[vars[i]]); 
          break;
        case T_CONSTANT:
          if (IS_NUMBER(cons[i]))
            pos += output(ss, "%ld", GET_VALUE(cons[i]));
          else
            pos += output(ss, "%s",
                           constant_table->symbols[GET_CONSTANT(cons[i])]); 
          break;
        case T_FUNCTION:
          ((Function*)terms[i])->Print();
          break;
        case T_RANGE:
          r = (Range *) terms[i];
          pos += output(ss, "%ld..%ld", r->start->val, r->end->val);
          break;
        default:
          //	int_error("invalid term type '%d' in literal",
          // args[0]);
          break;
        }
      }
      pos += output(ss, ")");
    }
    Literal *lt = 0;
    Function *fun =0 ;
    if (conditions || condition_func) {
      while (conditions && (lt = conditions->Iterate())) {
        pos += output(ss, " : %s", lt->GetPrintString(pos));
      }
      while (condition_func && (fun = condition_func->Iterate())) {
        if (!first)
          output(cerr, ": ");
        else
          first = 0;
        pos += output(ss, "%s", fun->GetPrintString());
      }
    }
  }
  str = ss.str();
  return str.c_str();
}
  


SpecialLiteral::SpecialLiteral(RuleType rl, Term *lb, Term *ub, long ln)
  : tail(UNORDERED), funs(UNORDERED)
{
  special = 1;
  ground = 1;
  negative = 0;
  arity = 0;
  numvars = 0;
  has_range = 0;
  has_function = 0;
  lineno = ln;
  dynamic_atleast = 0;
  weight_atleast = 0;
  negatives_counted = 0;
  args = NULL;
  vars = NULL;
  cons = NULL;
  terms = NULL;
  conditions = NULL;
  num_conditions = 0;
  num_positive_conditions = 0;
  num_negative_conditions = 0;
  weight = NULL;
  atmost_lit = NULL;
  type = rl;
  pred = Predicate::DefineSystemPredicate(DM_SPECIAL);

  atleast = lb;
  atmost = ub;
  
  if (atmost) {
    pred2 = Predicate::DefineSystemPredicate(DM_SPECIAL);
    if (atmost->type == T_CONSTANT) {
      atmost->val++;
    } else {
      Term *tmp = new Term(T_CONSTANT, 1, lineno);
      atmost = new Function("plus", atmost, tmp, lineno);
    }
  }  else 
    pred2 = -1;
}

void SpecialLiteral::PrintSpecial()
{
  Literal *lt = NULL;
  Function *fun = NULL;
  int first = 1, print_weight = 0;

  if (type == WEIGHTRULE)
    print_weight = 1;

  if (atleast)
    atleast->Print();

  output(cerr, " %s ", print_weight ? "["  : "{");
  
  while ((lt = tail.Iterate())) {
    if (!first)
      output(cerr, ", ");
    else
      first = 0;
//     if (lt->negative)
//       output(cerr, "not ");
    lt->Print(print_weight);
  }

  while ((fun = funs.Iterate())) {
    if (!first)
      output(cerr, ", ");
    else
      first = 0;
    fun->Print();
  }

  output(cerr, " %s ", print_weight ? "]"  : "}");

  if (atmost)
    atmost->Print();

}

int Literal::NeedsSimplifying()
{
  if (conditions || condition_func || has_range || has_function || has_atom)
    return 1;
  else
    return 0;
}


void Literal::CountSpecialVars()
{
  int i = 0;
  Literal *lt = 0;
  
  for (i = 0; i < arity ;i++) {
    switch (args[i]) {
    case T_RANGE:
    case T_FUNCTION:
      sys_data.unknown_vars++;
      break;
    case T_ATOM:
      sys_data.unknown_vars++;
      ((LiteralTerm*)terms[i])->lt->CountSpecialVars();
      break;
    default:
      break;
    }
  }
  if (conditions)
    while ((lt = conditions->Iterate()))
      lt->CountSpecialVars();
}

void SpecialLiteral::CheckWeights()
{
  // FIXME: remove the repetition. 
  Literal *lt = NULL;
  int tr = 0;
  long wt = 0;

  weight_info.Clear();
  inverted_pos.Clear();
  inverted_neg.Clear();

  while ((lt = tail.Iterate())){
    if (lt->inverted)
      lt->inverted = 0;
  }

  long evaluated_atmost = ERROR_NUMBER;
  long evaluated_atleast = 0;

  if (atleast) {
    evaluated_atleast = atleast->Eval();
  }
  if (atmost) {
    evaluated_atmost = atmost->Eval();
  }

  
  weight_info.modified_atmost = evaluated_atmost;
  weight_info.modified_atleast = evaluated_atleast;
  
  while ((lt = tail.Iterate())) {
    if (lt->has_global_condition || lt->empty_condition)
      continue;

    if (type != CONSTRAINTRULE) {
      wt = lt->GetWeight();
    } else {
      wt = 1;
    }
    // invert the negative weights
    if (wt < 0) {
      lt->inverted = 1;
      weight_info.has_negative_weights = 1;
      wt = -wt;
      weight_info.modified_atmost += wt;
      weight_info.modified_atleast += wt;
      if (lt->negative){
        if (!lt->DomainLiteral() || (sys_data.print_domains ==
                                     PR_ALL)) {
          inverted_pos.Insert(lt);
          weight_info.inverted_pos++;
        }
      } else {
        if (!lt->DomainLiteral() || (sys_data.print_domains ==
                                     PR_ALL)) {
          inverted_neg.Insert(lt);
          weight_info.inverted_neg++;
        }
      }
    } else {
      lt->inverted = 0;
    }
    // check whether the literal is a domain literal that can be
    // dropped or if its condition is empty 
    if (lt->empty_condition || ((sys_data.print_domains != PR_ALL) &&
        lt->DomainLiteral() &&
                                !predicates[lt->pred]->NeedsToBePrinted()))  {
      if (lt->DomainLiteral())
        if(lt->negative) 
          weight_info.removed_negative_domains++;
        else
          weight_info.removed_positive_domains++;
      
      // adjust bounds accordingly
      if (lt->empty_condition)
        tr = !lt->negative;
      else
        tr = lt->Test();

      bool neg = 0;
      if ((lt->negative && !lt->inverted) ||
          (!lt->negative && lt->inverted))
        neg = 1;
      if ((!neg && tr) || (neg && !tr)) {
        if (type == CONSTRAINTRULE) {
          weight_info.total_weight++;
          weight_info.removed_weight++;
        } else {
          weight_info.total_weight += wt;
          weight_info.removed_weight += wt;
        }
      }
    } else {
      weight_info.total_weight += wt;
    }
  }

  while ((lt = positive_conditions.Iterate())) {

    if (type != CONSTRAINTRULE) {
      wt = lt->GetWeight(); // weight is set already
    } else {
      wt = 1;
    }
    
    // invert the negative weights
    if (wt < 0) {
      lt->inverted = 1;
      weight_info.has_negative_weights = 1;
      wt = -wt;
      weight_info.modified_atmost += wt;
      weight_info.modified_atleast += wt;
      if (!lt->DomainLiteral() || (sys_data.print_domains ==
                                   PR_ALL)) {
        inverted_neg.Insert(lt);
        weight_info.inverted_neg++;
      }
    } else {
      lt->inverted = 0;
    }
    // check whether the literal is a domain literal that can be
    // dropped
    if ((sys_data.print_domains != PR_ALL) && lt->DomainLiteral()
        && !predicates[lt->pred]->NeedsToBePrinted()) {
      num_positive_conditions--;

      // adjust bounds accordingly
      tr = lt->Test();
      bool neg = 0;
      if ((lt->negative && !lt->inverted) ||
          (!lt->negative && lt->inverted))
        neg = 1;
      
      if ((!neg && tr) || (neg && !tr)) {
        if (type == CONSTRAINTRULE) {
          weight_info.total_weight++;
          weight_info.removed_weight++;
        } else {
          weight_info.total_weight += wt;
          weight_info.removed_weight += wt;
        }
      }
    } else {
      weight_info.total_weight += wt;
    }
  }

  while ((lt = negative_conditions.Iterate())) {
    if (type != CONSTRAINTRULE) {
      wt = lt->GetWeight(); // weight is set already
    } else {
      wt = 1;
    }
    
    // invert the negative weights
    if (wt < 0) {
      lt->inverted = 1;
      weight_info.has_negative_weights = 1;
      wt = -wt;
      weight_info.modified_atmost += wt;
      weight_info.modified_atleast += wt;
      if (!lt->DomainLiteral() ||
          (sys_data.print_domains == PR_ALL)
          || predicates[lt->pred]->NeedsToBePrinted()){
        inverted_pos.Insert(lt);
        weight_info.inverted_pos++;
      }
    } else {
      lt->inverted = 0;
    }

    
    // check whether the literal is a domain literal that can be
    // dropped
    if ((sys_data.print_domains != PR_ALL) && lt->DomainLiteral()
        && !predicates[lt->pred]->NeedsToBePrinted()) {
      num_negative_conditions--;

      // check whether the literal is true and adjust the bounds if
      // so. 
      tr = lt->Test();
      bool neg = 0;
      if ((lt->negative && !lt->inverted) ||
          (!lt->negative && lt->inverted))
        neg = 1;
	
      if ((!neg && tr) || (neg && !tr)) {
	
        if (type == CONSTRAINTRULE) {
          weight_info.total_weight++;
          weight_info.removed_weight++;
        } else {
          weight_info.total_weight += wt;
          weight_info.removed_weight += wt;
        }
      }
    } else {
      weight_info.total_weight += wt;
    }
  }

  // check functions
  Function *fun;
  while ((fun = funs.Iterate())) {
    if (fun->Call()) {
      weight_info.modified_atleast--;
      weight_info.modified_atmost--;
    }
  }

  
  weight_info.weight_left = weight_info.total_weight -
    weight_info.removed_weight;

  if (weight_atleast) {
    weight_info.modified_atleast = weight_info.total_weight -
      weight_info.modified_atleast + 1;
  } else if (dynamic_atleast) {
    weight_info.modified_atleast = weight_info.weight_left -
      weight_info.modified_atleast + 1;
  }
  
  // calculate the new bounds:
  weight_info.modified_atleast -= weight_info.removed_weight;
  weight_info.modified_atmost -= weight_info.removed_weight;
  
  is_warned = 1;
  return ;
}
  
void SpecialLiteral::EmitText(int lower)
{
  int insert_negs = 1;
  int print_weights = (type == WEIGHTRULE);
  int first = 1;

  //  sys_data.ground_rules++;
  
  Literal *lt = NULL;
  
  if (negs.Size() > 0)
    insert_negs = 0;
  
  // check if the domain literals are enough to satisfy the whole rule
  if (lower) {
    if (weight_info.weight_left <= 0) {
      EmitGround();
      output(sys_data.output_file, ".\n");
      return;
    }
  } else {
    if (weight_info.weight_left <= 0) {
      atmost_lit->EmitGround();
      output(sys_data.output_file, ".\n");
      return;
    }
  }
  
  if (lower) {
    EmitGround();
    output(sys_data.output_file, " :- %ld %s ",
            weight_info.modified_atleast,
            print_weights ? "[": "{");
  } else {
    atmost_lit->EmitGround();
    output(sys_data.output_file, " :- %ld %s ",
            weight_info.modified_atmost,
            print_weights ? "[": "{");
  }

  while ((lt = tail.Iterate())) {
    if ((!predicates[lt->pred]->NeedsToBePrinted() &&
         (sys_data.print_domains != PR_ALL) && lt->DomainLiteral())
        || lt->has_global_condition)
      continue;
    if (lt->negative) {
      if (insert_negs)
        negs.Insert(lt);
    } else {
      if (!first)
        output(sys_data.output_file, ", ");
      else
        first = 0;
      if (lt->inverted)
        output(sys_data.output_file, "not ");
      lt->EmitGround(print_weights);
    }
  }
  while ((lt = negs.Iterate())) {
    if (lt->inverted ||
        (!predicates[lt->pred]->NeedsToBePrinted() &&
         (sys_data.print_domains != PR_ALL) && 
         lt->DomainLiteral()))
      continue;
    if (!first)
      output(sys_data.output_file, ", not ");
    else {
      output(sys_data.output_file, "not ");
      first = 0;
    }
    lt->EmitGround(print_weights);
  }
  while ((lt = inverted_pos.Iterate())) {
    if (!first)
      output(sys_data.output_file, ", ");
    else {
      output(sys_data.output_file, " ");
      first = 0;
    }
    lt->EmitGround(print_weights);
  }
  while ((lt = negative_conditions.Iterate())) {
    if (!predicates[lt->pred]->NeedsToBePrinted() &&
        (sys_data.print_domains != PR_ALL) && lt->DomainLiteral()) 
      continue;
    if (lt->inverted)
      continue;
    if (!first)
      output(sys_data.output_file, ", %s ", lt->inverted ? "" : "not");
    else {
      output(sys_data.output_file, "%s ", lt->inverted ? "" : "not");
      first = 0;
    }
    lt->EmitGround(print_weights);
  }
  // FIXME: PR_ALL???
  while ((lt = positive_conditions.Iterate())) {
    if (!predicates[lt->pred]->NeedsToBePrinted() &&
        (sys_data.print_domains != PR_ALL) && lt->DomainLiteral())  
      continue;
    if (!first)
      output(sys_data.output_file, ",  ");
    else {
      output(sys_data.output_file, " ");
      first = 0;
    }
    if (lt->inverted)
      output(sys_data.output_file, "not ");
    lt->EmitGround(print_weights);
  }
  
  output(sys_data.output_file, " %s.\n", print_weights ? "]": "}");
}


void SpecialLiteral::EmitRule()
{
  CheckWeights();

  if (sys_data.emit_text) {
    if (!atleast ||
        (weight_info.modified_atleast <= weight_info.weight_left)) 
      EmitText(1);
    if (atmost &&
        (weight_info.modified_atmost <= weight_info.weight_left))
      EmitText(0);
  } else {
    if (!atleast ||
        (weight_info.modified_atleast <= weight_info.weight_left))
      EmitSmodels2(1);
    if (atmost &&
        (weight_info.modified_atmost <= weight_info.weight_left))
      EmitSmodels2(0);
  }
  return;
}

void SpecialLiteral::EmitSmodels2(int lower)
{
  long num_atoms = 0;
  long num_pos = 0;
  long num_neg = 0;
  Literal *lt = NULL;
  
  if (!negatives_counted) {
    negatives_counted = 1; 
    // count positive and negative literals of the tail
    while ((lt = tail.Iterate())) {
      if (lt->has_global_condition)
        continue;
      if (lt->negative)
        negs.Insert(lt);
    }
  }
  num_atoms = tail.Size() - weight_info.removed_positive_domains -
    weight_info.removed_negative_domains;
  num_atoms += positive_conditions.Size() - num_positive_conditions;
  num_atoms += negative_conditions.Size() - num_negative_conditions;


  // Compute the number of positive atoms. Start by removing the
  // number of negative atoms from the total size:
  num_pos = tail.Size() - negs.Size();
  // Then remove the positive domain literals:
  num_pos -= weight_info.removed_positive_domains;
  // Remove the number of negative conditions
  num_pos -=  num_negative_conditions;
  // Remove the number of positive literals with negative weights that
  // got turned into negative literals 
  num_pos -= inverted_neg.Size(); 

  // Ad the number of negative literals inverted to positive as well
  // as the size of expansions of positive conditions
  num_pos += weight_info.inverted_pos + positive_conditions.Size() -
    num_positive_conditions;

  
  //  num_pos = num_atoms - negs.Size() +
  //    weight_info.removed_negative_domains + weight_info.inverted_pos -
  //    weight_info.inverted_neg - num_negative_conditions; 

  //  num_pos -= negative_conditions.Size();
  
  num_neg = num_atoms - num_pos;
  
  // check if the domain literals are enough to make the rule true
  if (lower) {
    if (weight_info.modified_atleast <= 0) {
      output(sys_data.output_file, "%d ", BASICRULE);
      EmitGround();
      output(sys_data.output_file, "0 0\n");
      return;
    }
  } else {
    if (weight_info.modified_atmost <= 0) {
      output(sys_data.output_file, "%d ", BASICRULE);
      atmost_lit->EmitGround();
      output(sys_data.output_file, "0 0\n");
      return;
    }
  }
  
  output(sys_data.output_file, "%d ", type);
  
  // emit head 
  if (lower) {
    EmitGround();
  } else {
    atmost_lit->EmitGround();
  }
  
  if (type == CONSTRAINTRULE) {
    output(sys_data.output_file, "%ld %ld %ld ",
            num_atoms,
            num_neg,
            lower ? weight_info.modified_atleast :
            weight_info.modified_atmost);
  } else {
    output(sys_data.output_file, "%ld %ld %ld ",
            lower ? weight_info.modified_atleast :
            weight_info.modified_atmost,
            num_atoms,
            num_neg);
  }
  while ((lt = negs.Iterate())) {
    if (lt->inverted || lt->has_global_condition ||
        (!predicates[lt->pred]->NeedsToBePrinted() &&
         lt->DomainLiteral() &&	(sys_data.print_domains != PR_ALL)))
      continue;
    lt->EmitGround();
  }
  while ((lt = inverted_neg.Iterate())) {
    if (lt->has_global_condition)
      continue;
    lt->EmitGround();
  }
  while ((lt = negative_conditions.Iterate())) {
    if (lt->inverted)
      continue;
    lt->EmitGround();
  }
      
  while ((lt = tail.Iterate())) {
    if (!lt->has_global_condition &&
        !lt->inverted && !lt->negative &&  
        !(!predicates[lt->pred]->NeedsToBePrinted() &&
          lt->DomainLiteral() && (sys_data.print_domains != PR_ALL)))
      { 
        lt->EmitGround();
      }
  }
  while ((lt = inverted_pos.Iterate())) {
    if (lt->has_global_condition)
      continue;
    lt->EmitGround();
  }
  while ((lt = positive_conditions.Iterate())) {
    if (lt->inverted)
      continue;
    lt->EmitGround();
  }
  
  // if the rule is a weight rule, print also weights
  if (type == WEIGHTRULE) {
    while ((lt = negs.Iterate())) {
      if (lt->has_global_condition
          || lt->inverted ||
          (!predicates[lt->pred]->NeedsToBePrinted() &&
           lt->DomainLiteral() && (sys_data.print_domains != PR_ALL)))
        continue;
      lt->EmitWeight(lt->inverted);
      output(sys_data.output_file, " ");
    }
    while ((lt = inverted_neg.Iterate())) {
      if (lt->has_global_condition)
        continue;
      lt->EmitWeight(lt->inverted);
      output(sys_data.output_file, " ");
    }
    while ((lt = negative_conditions.Iterate())) {
      if (lt->inverted)
        continue;
      lt->EmitWeight(0);
      output(sys_data.output_file, " ");
    }

    while ((lt = tail.Iterate())) {
      if (!lt->has_global_condition
          && !lt->inverted && !lt->negative 
          && !(!predicates[lt->pred]->NeedsToBePrinted()
               &&lt->DomainLiteral() && 
               (sys_data.print_domains != PR_ALL))) { 
        lt->EmitWeight(lt->inverted);
        output(sys_data.output_file, " ");
      }
    }
  
    while ((lt = inverted_pos.Iterate())) {
      if (lt->has_global_condition)
        continue;
      lt->EmitWeight(lt->inverted);
      output(sys_data.output_file, " ");
    }
    while ((lt = positive_conditions.Iterate())) {
      if (lt->inverted)
        continue;
      lt->EmitWeight(0);
      output(sys_data.output_file, " ");
    }
  }
  output(sys_data.output_file, "\n");
}

// don't count the internal variables
int Literal::AddVars(Variable *vs)
{
  int result = 0, i = 0;

  if (special) {
    Literal *lt;
    Function *fun;
    SpecialLiteral *l = (SpecialLiteral*)this;
    while ((lt = l->tail.Iterate())) {
      result += lt->AddVars(vs);
    }
    while ((fun = l->funs.Iterate())) {
      result += fun->AddVars(vs);
    }
  } else {
    for (i = 0; i < arity; i++) {
      switch (args[i]) {
      case T_VARIABLE:
		if (!internal_variable(vars[i]) && !vs[vars[i]]) { 
		  vs[vars[i]] = 1;
		  result++;
		}
		break;
      case T_FUNCTION:
		result = ((Function*)terms[i])->AddVars(vs);
		break;
      case T_ATOM:
		result = ((LiteralTerm*)terms[i])->lt->AddVars(vs);
		break;
      default:
		break;
      }
    }
    if (conditions) {
      Literal *lt = NULL;
      while ((lt = conditions->Iterate())) {
		result += lt->AddVars(vs);
      }
    }
    if (condition_func) {
      Function *fun = NULL;
      while ((fun = condition_func->Iterate())) {
		result += fun->AddVars(vs);
      }
    }
  }
  return result;
}

void Literal::AddVarsByType(Variable *vs, int in_condition)
{
  // Add variable types to the variables array.
  // How this works:
  //  - Internal variables are not marked in any way
  //  - local variables are marked with 
  //      R_CONDITION
  //  - variables occurring outside conditions are marked with
  //      R_POSITIVE
  if (special) {
    Literal *lt;
    Function *fun;
    SpecialLiteral *l = (SpecialLiteral*)this;
    while ((lt = l->tail.Iterate())) {
      lt->AddVarsByType(vs);
    }
    while ((fun = l->funs.Iterate())) {
      fun->AddVarsByType(vs);
    }
  } else {
    int i;
    for (i = 0; i < arity; i++) {
      switch (args[i]) {
      case T_VARIABLE:
        if (!internal_variable(vars[i])) {
          if (in_condition && (vs[vars[i]] != R_POSITIVE)) {
            vs[vars[i]] = R_CONDITION;
          } else if (!in_condition) {
            vs[vars[i]] = R_POSITIVE;
          }
        }
        break;
      case T_FUNCTION:
        ((Function*)terms[i])->AddVars(vs);
		break;
      case T_ATOM:
        ((LiteralTerm*)terms[i])->lt->AddVars(vs);
		break;
      default:
        break;
      }
    }
    if (conditions) {
      Literal *lt = NULL;
      while ((lt = conditions->Iterate())) {
		lt->AddVarsByType(vs,1);
      }
    }
    if (condition_func) {
      Function *fun = NULL;
      while ((fun = condition_func->Iterate())) {
		fun->AddVarsByType(vs, 1);
      }
    }
  }
}

// void Literal::AddVarsByType(Variable *vs, int in_condition)
// {
//   int result = 0, i = 0;

//   if (special) {
//     Literal *lt;
//     Function *fun;
//     SpecialLiteral *l = (SpecialLiteral*)this;
//     while ((lt = l->tail.Iterate())) {
//        lt->AddVarsByType(vs);
//     }
//     while ((fun = l->funs.Iterate())) {
//       result += fun->AddVars(vs);
//     }
//   } else {
//     for (i = 0; i < arity; i++) {
//       switch (args[i]) {
//       case T_VARIABLE:
// 		if (!internal_variable(vars[i])) {
// 		  if (in_condition) {
// 			if (!vs[vars[i]] || vs[vars[i]] == 3) {
// 			  vs[vars[i]] = 2; // magic
// 			  result++;
// 			} 
// 		  } else {
// 			switch (vs[vars[i]]) {
// 			case 0:
// 			  if (conditions) {
// 				vs[vars[i]] = 3;
// 			  } else {
// 				vs[vars[i]] = 1;
// 			  }
// 			  result++;
// 			  break;
// 			case 2:
// 			case 3:
// 			  vs[vars[i]] = 1;
// 			  break;
// 			}
// 		  }
// 		}
// 		break;
//       case T_FUNCTION:
// 		result = ((Function*)terms[i])->AddVars(vs);
// 		break;
//       case T_ATOM:
// 		result = ((LiteralTerm*)terms[i])->lt->AddVars(vs);
// 		break;
//       default:
// 		break;
//       }
//     }
//     if (conditions) {
//       Literal *lt = NULL;
//       while ((lt = conditions->Iterate())) {
// 		result += lt->AddVarsByType(vs,1);
//       }
//     }
//     if (condition_func) {
//       Function *fun = NULL;
//       while ((fun = condition_func->Iterate())) {
// 		result += fun->AddVars(vs);
//       }
//     }
    
//   }
//   return result;
// }
  
  
void SpecialLiteral::AddVariables()
{
  Literal *lt = NULL;
  Function *fun = NULL;
  int num = 0, i = 0;
  
  static Variable *existing_vars = NULL;
  
  if (!existing_vars) {
    existing_vars = new Variable[sys_data.total_vars+1];
    if (!existing_vars)
      error(SYS_ERR, "malloc error");
  }
  memset(existing_vars, 0, sizeof(Variable) * sys_data.total_vars+1);
  
  // conditions have been cleaned away before this, so don't have to
  // worry about them 
  while ((lt = tail.Iterate())) {
    num += lt->AddVars(existing_vars);
  }
  while ((fun = funs.Iterate())) {
    num += fun->AddVars(existing_vars);
  }

  // add also bounds
  if (atleast) {
    num += atleast->AddVars(existing_vars);
  }
  
  if (atmost) {
    num += atmost->AddVars(existing_vars);
  }
  
  if (!num) {
    if (pred2 >= 0) {
      atmost_lit = Literal::Duplicate(0);
      atmost_lit->special = 0;
      atmost_lit->negative = 1;
      atmost_lit->pred = pred2;
    }
    return;
  }
  arity = numvars = num;
  if (arity > sys_data.term_max_arity)
    sys_data.term_max_arity = arity;
  
  args = new TermType[num];
  vars = new Variable[num];
  cons = new Instance[num];
  terms = new Term*[num];
  if (!args || !vars || !terms || !cons)
    error(SYS_ERR, "malloc error");

  memset(terms, 0, sizeof(*terms) * num);
  
  num = 0;
  for (i = 0; i < sys_data.total_vars; i++) {
    if (existing_vars[i]) {
      args[num] = T_VARIABLE;
      vars[num++] = i;
    }
  }
  if (pred2 >= 0) {
    atmost_lit = Literal::Duplicate(0);
    atmost_lit->special = 0;
    atmost_lit->pred = pred2;
    atmost_lit->negative = 1;
  }

  // sanity check
  if (num != arity)
    int_error("missing variables", "");
}


int Literal::BindLiteral(Literal *lt, Instance *item, int pos)
{
  int i;

  if (!item || !lt)
    return 1;
  
  if (!var_pos){
    var_pos = new Variable[sys_data.total_vars * sizeof(Instance)];
    if (!var_pos)
      error(SYS_ERR, "malloc error");
  }

  for (i = 0; i < lt->arity; i++) {
    if (lt->vars[i] >= 0) {
      if (variables[lt->vars[i]] > ERROR_NUMBER) { // variable has an old value
        if (item[i] != variables[lt->vars[i]]) 
          return 0; // mismatch
        else 
          continue;
      } else { // set the variable value and position
        variables[lt->vars[i]] = item[i];
        var_pos[lt->vars[i]] = pos;
      }
    } else if (lt->cons[i] != item[i])
      return 0;
  }

  return 1;
}

  
int Literal::DomainLiteral()
{
  if (special)
    return 0;

  return ((predicates[pred]->Status() == DM_DOMAIN) ||
          (predicates[pred]->Status() == DM_INTERNAL));
}
  
void Literal::Simplify(FunctionList *save_funs)
{
  long new_pred = -1;
  Literal *lt = NULL;
  Variable new_var = ERROR_NUMBER, var = ERROR_NUMBER;
  static char *varname = NULL;
  static char *predname = NULL;
  Range *rn = NULL;
  Term *nt1 = NULL;
  Function *nt2 = NULL;
  FunctionList *s_funs = NULL;
  InstFunc fp;
  int i = 0;

  LiteralList to_be_added;

  if (!varname) {
    varname = new char[RANGE_MAX_NAME];
    predname = new char[RANGE_MAX_NAME];
    if (!varname || !predname)
      error(SYS_ERR, "malloc error");
  }
  if (!save_funs && !conditions) {
    conditions = new LiteralList();
    condition_func = new FunctionList();
  }
  if (save_funs) {
    s_funs = save_funs;
  } else {
    s_funs = condition_func;
  }
  
  // recursively simplify conditions, if they exist
  if (conditions) {
    while ((lt = conditions->Iterate())) {
      if (lt->NeedsSimplifying()) {
        lt->Simplify(s_funs);
      }
      if (!lt->conditions)
        continue;
      
      Literal *tmp = 0;
      while (lt->conditions && (tmp = lt->conditions->Iterate())) {
        to_be_added.Insert(tmp);
      }
      //      while (lt->condition_func && (fun = lt->condition_func->Iterate())) {
      //	condition_func->Insert(fun);
      //      }
      lt->conditions = 0;
      lt->condition_func = 0;
    }
    while ((lt = to_be_added.Iterate())) {
      conditions->Insert(lt);
      predicates[lt->pred]->is_only_symbolic_function = 0;
    }
  }
  for (i = 0; i < arity; i++) {
    switch (args[i]) {
    case T_RANGE:
      new_pred = Predicate::DefineRangePredicate();
      new_var = DefineSystemVariable();
      args[i] = T_VARIABLE;
      vars[i] = new_var;
      rn = (Range*) terms[i];
      predicates[new_pred]->AddRange(rn);
      rn->AddDependencies(pred);

      
      // construct matching literal and add it to conditions
      lt = new Literal(new_var, new_pred);
      if (!lt)
        error(SYS_ERR, "malloc error");
      lt->lineno = lineno;
      if (!conditions) {
        conditions = new LiteralList(UNORDERED);
        condition_func = new FunctionList(UNORDERED);
      }
      conditions->Insert(lt);
      if (ground)
        ground = 0;
      dependency_graph->AddEdge(pred, new_pred, false);
      
      break;
    case T_FUNCTION:
      sys_data.num_functions++;
      new_var = DefineSystemVariable();
      args[i] = T_VARIABLE;
      vars[i] = new_var;

      fp = function_table->GetFunction("assign");
      nt1 = new Term(T_VARIABLE, new_var, lineno);
      nt2 = new Function(fp, "assign", 2, lineno);
      nt2->AddArgument(nt1);
      nt2->AddArgument(terms[i]);

      s_funs->Insert(nt2);
      break;
    case T_ATOM:
      var = DefineSystemVariable();
      if (var < 0)
        int_error("cannot generate literal argument '%s'",
                  varname);
      args[i] = T_VARIABLE;
      vars[i] = var;

      lt = ((LiteralTerm*)terms[i])->lt;
      if (lt->has_range || lt->has_function || lt->has_atom) {
        lt->Simplify(s_funs);
      }
      
      // construct the new assign for the atom
      fp = function_table->GetFunction("assign");
      nt1 = new Term(T_VARIABLE, var, lineno);
      nt2 = new Function(fp, "assign", 2, lineno);
      nt2->AddArgument(nt1);
      nt2->AddArgument(terms[i]);
      terms[i] = NULL;
      

      s_funs->Insert(nt2);
      
      if (ground)
        ground = 0;
      break;
    default:
      break;
    }
  }
}
  
void AtomData::Clear()
{
  modified_atmost = 0;
  modified_atleast = 0;
  inverted_pos = 0;
  inverted_neg = 0;
  has_negative_weights = 0;
  removed_positive_domains = 0;
  removed_negative_domains = 0;
  total_weight = 0;
  removed_weight = 0;
  weight_left = 0;
}

AtomData::AtomData()
{
  modified_atleast = 0;
  modified_atmost = 0;
  has_negative_weights=0;
  removed_positive_domains=0;
  total_weight = 0;
  removed_weight = 0;
  weight_left = 0;
  inverted_pos = 0;
  inverted_neg = 0;
}

Literal::~Literal()
{
  Literal *lt = NULL;
  Function *fun = NULL;
  int i;
  if (args)
    delete [] args;
  if (vars)
    delete [] vars;
  if (cons)
    delete [] cons;
  if (terms) {
    for (i =0 ; i< arity; i++)
      if (terms[i])
        delete terms[i];
    delete [] terms;
  }
  if (conditions) {
    while ((lt = conditions->Iterate())) {
      delete lt;
    }
    delete conditions;
  }
  if (condition_func) {
    while ((fun = condition_func->Iterate())) {
      delete fun;
    }
    delete condition_func;
  }
  if (weight && weight != default_weight)
    delete weight;
  
}

SpecialLiteral::~SpecialLiteral()
{
  Literal *lt = NULL;
  Function *fun = NULL;

  while ((lt = tail.Iterate())) {
    delete lt;
  }
  while ((fun = funs.Iterate())) {
    delete fun;
  }
}

ComputeStmt::~ComputeStmt()
{
  if (rl)
    delete rl;
}

Literal *Literal::Instantiate()
{
  Literal *lt = Duplicate(0, 0);
  lt->has_global_condition = 0;
  lt->ignore_global_condition = 0;
  
  for (int i = 0; i < arity; i++) {
    if (args[i] == T_VARIABLE) {
      lt->vars[i] = ERROR_NUMBER;
      lt->cons[i] = variables[vars[i]];
      lt->args[i] = T_CONSTANT;
    }
  }
  return lt;
}

void SpecialLiteral::FindWeights()
{
  Literal *lt = NULL;
  const char *s = NULL;
  
  while ((lt = tail.Iterate())) {
    lt->FindWeight();
    if (lt->weight == default_weight) {
      s = lt->GetPrintString();
      warn(WARN_WEIGHT, lt->lineno, "default weight used for literal %s%s'"
           " in a weightrule", (lt->negative) ? "'not " : "'", s);
    }
  }
}

// Before calling this all variables should have a value defined.
Weight *Weight::Instantiate()
{
  Weight *new_weight = new Weight();
  switch (type) {
  case WT_NUMBER:
    new_weight->v.val = v.val;
    break;
  case WT_VARIABLE:
    new_weight->v.val = variables[v.var];
    break;
  case WT_EXPR:
    new_weight->v.val = v.fun->Call();
    break;
  default:
    int_error("invalid weight type", "");
  }
  return new_weight;
}

Weight *Weight::Instantiate(Literal *lt, Literal *p)
{
  Weight *new_weight = new Weight();
  int found = 0;
  switch(type) {
  case WT_NUMBER:
    new_weight->v.val = v.val;
    break;
  case WT_VARIABLE:
    found = 0;
    // find the occurence of the variable from 'p', find the
    // corresponding position from 'lt' and set v.val to that value.
    for (int i = 0; i < p->arity && !found; i++) {
      if ((p->args[i] == T_VARIABLE) && (p->vars[i] == v.var)) {
        found = 1;
        switch (lt->args[i]) {
        case T_CONSTANT:
          new_weight->type = WT_NUMBER;
          new_weight->v.val = lt->cons[i];
          if (!IS_NUMBER(lt->cons[i])) {
            error(FATAL_ERR, "%s: runtime error: non-numeric weight for "
                  "literal '%s'", error_file_and_line(lt->lineno),
                  lt->GetPrintString()); 
          }
          break;
        case T_VARIABLE:
          new_weight->type = WT_VARIABLE;
          new_weight->v.var = lt->vars[i];
          break;
        default:
          int_error("invalid weight type for '%s'",
                    lt->GetPrintString());
          break;
        }
      }
    }
    if (!found)
      int_error("invalid weight type for '%s'", lt->GetPrintString());
    break;
  case WT_EXPR:
    new_weight->type = WT_EXPR;
    new_weight->v.fun = v.fun->Instantiate(lt, p);
    break;
  case WT_ATOM:
    int_error("a tied weight was not resolved correctly", "");
  }
  return new_weight;
}
	
	
Literal *Literal::CreateComplement()
{
  Literal *new_lit = Duplicate(0);
  if (!new_lit)
    error(SYS_ERR, "malloc error");

  if (predicates[pred]->complement >= 0) {
    new_lit->pred = predicates[pred]->complement;
  } 
  return new_lit;
}

Literal *SpecialLiteral::Duplicate(int force_negative, int include_conditions)
{
  Term *least = 0;
  Term *most = 0;
  if (atleast) {
	least = atleast->Duplicate();
  }
  if (atmost) {
	most = atmost->Duplicate();
  }
  SpecialLiteral *new_lit = new SpecialLiteral(type, least, most, lineno);
  Literal *lt;
  tail.ClearIterator();
  while ((lt = tail.Iterate())) {
	new_lit->tail.Insert(lt->Duplicate(0,1));
  }
  return new_lit;
}

SpecialLiteral *SpecialLiteral::CreateComplement()
{
  SpecialLiteral *new_lit = new SpecialLiteral(type, atleast, atmost, lineno);

  if (!new_lit)
    error(SYS_ERR, "malloc error");

  new_lit->arity = arity;
  new_lit->numvars = numvars;
  new_lit->has_range = has_range;
  new_lit->has_function = has_function;
  new_lit->lineno = lineno;
  new_lit->dynamic_atleast = dynamic_atleast;
  new_lit->negatives_counted = negatives_counted;
  new_lit->num_conditions = num_conditions;
  new_lit->num_positive_conditions = num_positive_conditions;
  new_lit->num_negative_conditions = num_negative_conditions;

  Literal *lt = NULL;
  Literal *tmp = NULL;
  
  while ((lt = tail.Iterate())) {
    if (lt->negative) {
      tmp  = lt->Duplicate();
      lt->pred = predicates[lt->pred]->complement;
    } else {
      tmp = lt->CreateComplement();
    }
    new_lit->tail.Insert(tmp);
  }

  Function *fun = NULL;
  while ((fun = funs.Iterate())) {
    new_lit->funs.Insert(fun->DuplicateFunction());
  }

  return new_lit;
}

Weight* Weight::TieWeight(Literal *lt, Literal *source)
{
  Weight *wt = NULL;
  long tmp_lineno = source->lineno;
  source->lineno = lt->lineno;
  source->FindWeight();
  if (source->weight) {
    wt = source->weight->Instantiate(lt, source);
  } else {
    wt = source->dynamic_weight->Instantiate(lt, source);
  }
  source->lineno = tmp_lineno;
  
  source->weight = NULL;
  return wt;
}
