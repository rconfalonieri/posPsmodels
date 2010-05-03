// term.cc -- implementation for terms
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
#include "config.h"
#include <string.h>
#ifndef TERM_H
#include "term.h"
#endif
#ifndef SYMBOL_H
#include "symbol.h"
#endif
#ifndef PARSETREE_H
#include "parsetree.h"
#endif
#ifndef DEBUG_H
#include "debug.h"
#endif
#ifndef ERROR_H
#include "error.h"
#endif
#include "graph.h"
#include "predicate.h"

char *function_strings[] = { "***null***", "abs", "eq", "le", "ge",
			     "lt", "gt", "neq", "plus", "minus",
			     "times", "div",  "mod", "assign", "and",
			     "or", "not", "xor",  "weight", "norm",
			     "exp", "***error***" }; 

char *operators[] = { "``error''", "---na---", "==", "<=", ">=", "<", ">",
		      "!=", "+", "-", "*", "/", "%", "=", "&", "|",
		      "~", "^", "---na---", "---na---"  }; 


// returns the internal function name corresponding to an operator */
char *get_function_name(char *fun)
{
  int i = 0;
  for (i = 2; i < NUM_MAX_FUN; i++) {
    if (!strcmp(fun, operators[i]))
      return function_strings[i];
  }
  return NULL;
}

Term::Term(TermType tt, long v, long ln)
{
  val = v;
  type = tt;
  if (tt == T_VARIABLE)
    ground = 0;
  else
    ground = 1;
  lineno = ln;
  has_range = 0;
  has_atom = 0;
  has_function = 0;
}

void Term::Print()
{
  switch (type) {
  case T_CONSTANT:
    if (IS_NUMBER(val)) {
      output(cerr, "%ld", val);
    } else {
      output(cerr, "%s", constant_table->LookupByValue(val));
    }
    break;
  case T_VARIABLE:
    output(cerr, "%s", variable_table->symbols[val]);
    break;
  case T_FUNCTION:
    ((Function*)this)->Print();
    break;
  case T_RANGE:
    ((Range*)this)->Print();
    break;
  case T_ATOM:
    ((LiteralTerm*)this)->Print();
    break;
  default:
    break;
  }
}

long Term::Eval()
{
  long result = ERROR_NUMBER;
  switch (type) {
  case T_CONSTANT:
    result = val;
    break;
  case T_VARIABLE:
    result = variables[val];
    break;
  case T_FUNCTION:
    result = ((Function*)this)->Call();
    break;
  default:
    int_error("evaluating an invalid term type", "");
  }
  return result;
}

Term *Term::Duplicate()
{
  if (type == T_FUNCTION)
    return ( (Term*) ((Function*)this)->DuplicateFunction());

  if (type == T_RANGE)
    return ( (Term*) ((Range*)this)->DuplicateRange());

  if (type == T_ATOM)
    return ( (Term*) ((LiteralTerm*)this)->DuplicateAtom());

  Term *res = new Term(type, val, lineno);
  if (!res)
    error(SYS_ERR, "malloc error");
  res->ground = ground;
  res->has_range = has_range;
  res->has_function = has_function;
  return res;
}

int Term::AddVars(Variable *vs)
{
  
  switch (type) {
  case T_VARIABLE:
    if (!vs[val]) {
      vs[val] = 1;
      return 1;
    }
    break;
  case T_FUNCTION:
    return ((Function*)this)->AddVars(vs);
  default:
    break;
  }
  return 0;
}

Term *Term::ConstructArgument(ParseNode *item)
{
  Term *new_term = NULL;
  long nval = 0;

  switch (item->type) {
  case PT_EXPR:
  case PT_FUNCTION:
    return item->ext.fun; // since postorder this can be done
    break;
  case PT_VARIABLE:
    nval = variable_table->Lookup(item->sval);
    if (nval < 0)
      int_error("unknown variable '%s'", item->sval);
    new_term = new Term(T_VARIABLE, nval, item->lineno);
    if (!new_term)
      error(SYS_ERR, "malloc error");
    return new_term;
  case PT_NUMBER:
    new_term = new Term(T_CONSTANT, item->l1.val, item->lineno);
    if (!new_term)
      error(SYS_ERR, "malloc error");
    return new_term;
  case PT_CONSTANT:
    nval = constant_table->Lookup(item->sval);
    if (nval < 0)
      int_error("unknown constant '%s'", item->sval);
    new_term = new Term(T_CONSTANT, nval, item->lineno);
    if (!new_term)
      error(SYS_ERR, "malloc error");
    return new_term;
  case PT_RANGE:
    new_term = ConstructRange(item);
    return new_term;
  case PT_ATOM:
    new_term = new LiteralTerm(item->ext.lit);
    if (!new_term)
      error(SYS_ERR, "malloc error");
    return new_term;
  default:
    int_error("invalid term type %s", parse_strings[item->type]);
  }
  return NULL;
}



Range *Term::ConstructRange(ParseNode *pn)
{
  Term *left = ConstructArgument(pn->left);
  Term *right = ConstructArgument(pn->right);
  Range *new_range = 0;

  if (!left->ground || !right->ground ||
      ((left->type == T_CONSTANT &&
	IS_CONSTANT(left->val))) ||
      ((right->type == T_CONSTANT &&
	IS_CONSTANT(right->val)))) {
    error(USR_ERR, "%s: invalid range definition: ", 
	  error_file_and_line(pn->lineno));
    output(cerr, "\t");
    left->Print();
    output(cerr, " .. ");
    right->Print();
    output(cerr, "\n");
  }
  new_range = new Range(left, right, pn->lineno, 0);
  return new_range;
}
//      if (!term->(lval1)) {
//        output(cerr, "   expression '");
//        $1->PrintExpression();
//        output(cerr, "' is not a constant valued expression.\n");
//      } 
//  	    if (!IS_NUMBER(lval2)) {
//                output(cerr, "   expression '");
//                $3->PrintExpression();
//                output(cerr, "' is not a constant valued expression.\n");
//              } 
//  	} if (!IS_NUMBER(lval1) && !IS_NUMBER(lval2)) {
//            $$ = new ParseNode(PT_RANGE, NULL, $1, $3, lineno);
//          } else if (!IS_NUMBER(lval1) && IS_NUMBER(lval2)) {
//            $$ = new ParseNode(PT_RANGE, NULL, $1, NULL, lineno, 0,
//                               lval2);
//            delete $3;
//          } else if (IS_NUMBER(lval1) && !IS_NUMBER(lval2)) {
//            $$ = new ParseNode(PT_RANGE, NULL, NULL, $3, lineno, lval1);
//            delete $1;
//          } else {
//            delete $1;
//            delete $3;
//        	  $$ = new ParseNode(PT_RANGE, NULL, NULL, NULL, lineno, lval1,
//                             lval2); } }

char *Term::GetPrintString()
{
  ostringstream ss;
  //  int pos = 0, i = 0;
  
  switch (type) {
  case T_FUNCTION:
    return ((Function*)this)->GetFunPrintString();
  case T_VARIABLE:
    return variable_table->LookupByValue(val);
  case T_CONSTANT:
    if (IS_NUMBER(val)) {
      output(ss, "%ld", MAKE_NUMBER(val));
      return clone_string(ss.str().c_str());
    } else {
      return constant_table->LookupByValue(GET_CONSTANT(val));
    }
  default:
    int_error("unimplemented", "");
  }
}


Function::Function(InstFunc f, ParseNode *pn, char *n, long ln, int
		   ar) 
{
  ParseNode *tmp = NULL, *parg = NULL;
  Term *arg = NULL;
  int pos = ar-1, i = 0;
  Literal *lt;
  negative = 0;
  type = T_FUNCTION;
  val = 0;
  fun = f;
  arity = ar;

  name = n;
  ground = 1;
  is_weight_function = 0;
  has_range = 0;
  has_atom = 0;
  has_function = 1;
  lineno = ln;
  allocated = 0;
  args = new Term*[arity];


  if (!args)
    error(SYS_ERR, "malloc error");
  for(i=0;i<arity;i++)
    args[i] = NULL;

  if (!strcmp(name, "assign"))
    assign = 1;
  else
    assign = 0;
  if (!strcmp(name, "eq") || !strcmp(name, "neq"))
    accept_constants = 1;
  else
    accept_constants = 0;

  // process arguments
  tmp = pn->right;
  while(tmp && (pos >= 0)) {
    parg = tmp->left;
    if (!parg)
      int_error("Internal error: misformed parse tree."
		"missing function argument", "");
    switch(parg->type) {
    case PT_ATOM:
    case PT_CONSTANT:
    case PT_FUNCTION:
    case PT_EXPR:
    case PT_VARIABLE:
    case PT_NUMBER:
    case PT_RANGE:
      arg = Term::ConstructArgument(parg);
      break;
    default:
      int_error("misformed parse tree: "
		"invalid function argument type %s",
		parse_strings[parg->type]); 
    }
    if (!arg->ground)
      ground = 0;
    args[pos--] = arg;
    allocated++;
    tmp = tmp->right;
  }
  if (pos != -1) {
    int_error("misformed function call tree", "");
  }

  if (!strcmp(name, "norm")) {
    long pred;
    switch (args[0]->type) {
    case T_CONSTANT:
      if (!IS_CONSTANT(args[0]->val)) {
	error(USR_ERR, "%s: invalid argument for `norm' function", 
	      error_file_and_line(pn->lineno));
      } else {
	pred =
	  Predicate::DefinePredicate(constant_table->LookupByValue(args[0]->val),
				     0, lineno);
	args[0]->val = pred;
      }
      break;
    case T_ATOM:
      lt = ((LiteralTerm*) args[0])->lt;
      pred = lt->pred;
      delete args[0];
      args[0] = new Term(T_CONSTANT, pred, lineno);
      break;
    default:
      error(USR_ERR, "%s: invalid argument for `norm' function", 
	    error_file_and_line(pn->lineno));
      break;
    }
  }
}

Function::Function(InstFunc f, char *n, int ar, long ln) 
{
  type = T_FUNCTION;

  fun = f;
  arity = ar;
  name = n;
  negative = 0;
  ground = 1;
  has_range = 0;
  has_function = 1;
  is_weight_function = 0;
  has_atom = 0;
  lineno = ln;
  val = 0;
  allocated = 0;
  args = new Term*[arity];
  if (!args)
    error(SYS_ERR, "malloc error");
  for (int i = 0; i < arity; i++)
    args[i] = NULL;

  if (!strcmp(name, "assign"))
    assign = 1;
  else
    assign = 0;
  if (!strcmp(name, "eq") || !strcmp(name, "neq"))
    accept_constants = 1;
  else
    accept_constants = 0;
}

Function::Function(char *n, Term *left, Term *right, long ln)
{
  fun = function_table->GetFunction(n);

  if (!fun) {
    int_error("unknown function %s", n);
  }
  negative = 0;
  type = T_FUNCTION;
  val = 0;
  arity = 2;
  name = n;
  ground = 1;
  has_range = 0;
  has_atom = 0;

  has_function = 1;
  is_weight_function = 0;
  lineno = ln;
  allocated = 0;
  args = new Term*[arity];
  args[0] = left;
  args[1] = right;

  if (!strcmp(name, "assign"))
    assign = 1;
  else
    assign = 0;

  if (!strcmp(name, "eq") || !strcmp(name, "neq"))
    accept_constants = 1;
  else
    accept_constants = 0;

  if (!left->ground || !right->ground)
    ground = 0;
}


Function::~Function()
{
  int i = 0;
  for (i = 0; i < arity; i++) {
    switch (args[i]->type) {
    case T_FUNCTION:
      delete (Function*) args[i];
      break;
    case T_RANGE:
      delete (Range*) args[i];
      break;
    default:
      delete args[i];
      break;
    }
  }
  delete [] args;
}

Function *Function::DuplicateFunction()
{
  int i = 0;
  Function *new_fun = new Function(fun, name, arity, lineno);
  if (!new_fun)
    error(SYS_ERR, "malloc error");
  new_fun->ground = ground;
  new_fun->has_range = has_range;
  new_fun->has_function = has_function;
  new_fun->negative = negative;
  new_fun->allocated = allocated;
  new_fun->is_weight_function = is_weight_function;
  for (i = 0; i < arity; i++) 
    new_fun->args[i] = args[i]->Duplicate();
  return new_fun;
}

void Function::AddDependencies(long dep)
{
  if (!strcmp(name, function_strings[FUN_NORM])) {
    dependency_graph->AddEdge(dep, args[0]->val, false);
    return;
  }

  for (int i = 0; i < arity; i++) {
    if (args[i]->type == T_FUNCTION)
      ((Function*)args[i])->AddDependencies(dep);
  }
}

int Function::AddArgument(Term *t)
{
  debug(DBG_TERM, 2, "Adding argument %ld, old arity %d", t, arity);
  assert (t);


  if (t->has_atom)
    has_atom = 1;
  
  args[allocated++] = t;

  if (!t->ground) 
    ground = 0;
  if (t->has_range) 
    has_range = 1;
  return 1;
}

Instance Function::Call()
{
  int i = 0 ;
  long parms[MAX_NUMBER_OF_FUNCTION_ARGUMENTS];

  long res = 0;

  if (is_weight_function) {
    res = ((LiteralTerm*)args[0])->lt->GetWeight();
    return res;
  }
  
  for(i = 0; i < arity; i++) {
    switch (args[i]->type) {
      // non numerical constants are weeded out before this
    case T_ATOM:
      //      if (!accept_constants) 
      //	error(FATAL_ERR, "Runtime error: an atom was passed as an "
      //	      "argument to function '%s' at line %ld", name, lineno);
      parms[i] = GET_VALUE( ((LiteralTerm*) args[i])->ConstructAtom());
      break;
    case T_CONSTANT: 
      //      assert (IS_NUMBER(args[i]->val));
      parms[i] = GET_VALUE(args[i]->val);
      break;
    case T_VARIABLE:
      //      if (!IS_NUMBER(variables[args[i]->val]) && !accept_constants)
      //error(FATAL_ERR, "Runtime error: non-numerical constant '%s'"
      //      " passed as an argument to function '%s' at line %ld",
      //      constant_table->symbols[GET_CONSTANT(variables[args[i]->val])],
      //      name, lineno); 
      parms[i] = GET_VALUE(variables[args[i]->val]);
      break;
    case T_FUNCTION:
      parms[i] = ((Function *)args[i])->Call();
      break;
    default:
      int_error("misformed function call tree", "");
    }
  }
#ifdef CXX_HAS_NO_EXCEPTIONS
  res = fun( arity, parms);
#else
  try {
    res = fun( arity, parms);
  }

  catch (RuntimeError &r) {
    output(cerr, "Runtime error at line %ld:", lineno );
    r.Print();
    output(cerr, "\nError happened while calling function: ");
    Print();
    output(cerr, "\n");
    exit(2);
  }
#endif
  return res;
}

int Function::GetPos()
{
  int res = 0, i = 0, pos = 0;
  
  for (i=0; i < arity; i++) {
    switch (args[i]->type) {
    case T_FUNCTION:
      pos = ((Function*)args[i])->GetPos();
      if (pos < 0) {
	return -1;
      }
      res = max(res, pos);
      break;
    case T_VARIABLE:
      pos = variables[args[i]->val];
      if (pos < 0)  // there are some undefined variables
	return -1;
      res = max(res, pos);
      break;
    case T_CONSTANT:
      break;
    case T_ATOM:
      pos = ((LiteralTerm*)args[i])->GetPos();
      if (pos < 0)
	return -1;
      res = max(res, pos);
      break;
    default:
      int_error("misformed function call tree", "");
    }
  }
  return res;
}

Instance Function::Test(int pos)
{
  Instance res = ERROR_NUMBER;
  
  // If function call has an assignment store the value
  if (assign) {
    if (arity != 2) 
      int_error("misformed assign statement", "");

    switch(args[1]->type) {
    case T_FUNCTION:
      res = MAKE_NUMBER(((Function*)args[1])->Call());
      break;
    case T_VARIABLE:
      res = variables[args[1]->val];
      break;
    case T_CONSTANT:
      res = args[1]->val;
      break;
    case T_ATOM:
      res = ((LiteralTerm*) args[1])->ConstructAtom();
      break;
    default:
      int_error("misformed assign statement", "");
    }
    variables[args[0]->val] = res;
    var_pos[args[0]->val] = pos;
    return 1;
  }
      
  else
    res = Call();

  if (negative) 
    return ! res;
  else 
    return res;
}

void Function::AddGraphEdges(Graph *sg, long start, int initial)
{
  int i = initial;

  for (; i < arity; i++) {
    switch(args[i]->type) {
    case T_VARIABLE:
      sg->AddEdge(start, args[i]->val, false);
      break;
    case T_FUNCTION:
      ((Function*)args[i])->AddGraphEdges(sg, start);
      break;
    case T_ATOM:
      ((LiteralTerm*)args[i])->AddGraphEdges(sg, start);
      break;
    default:
      break;
    }
  }
}

int Function::CheckGlobal(Variable *vars)
{
  int i = 0;
  for (i=0; i < arity; i++) {
    switch (args[i]->type) {
    case T_VARIABLE:
      if (vars[args[i]->val] != VAR_CONDITION)
	return 1;
      break;
    case T_FUNCTION:
      if (( (Function*)args[i])->CheckGlobal(vars))
	return 1;
      break;
    case T_ATOM:
      if (( (LiteralTerm*)args[i])->CheckGlobal(vars))
	return 1;
      break;
    default:
      break;
    }
  }
  return 0;
}

void Function::AddVarsByType(Variable *vars, int in_condition)
{
  int i = 0;
  for (i=0; i < arity; i++) {
    switch (args[i]->type) {
    case T_VARIABLE:
      if (in_condition && vars[args[i]->val] != R_POSITIVE) {
        vars[args[i]->val] = R_CONDITION;
      } else if (!in_condition) {
        vars[args[i]->val] = R_POSITIVE;
      }
      break;
    case T_FUNCTION:
      ( (Function*)args[i])->AddVarsByType(vars, in_condition);
      break;
    case T_ATOM:
      ((LiteralTerm*)args[i])->AddVarsByType(vars, in_condition);
      break;
    default:
      break;
    }
  }
}

void Function::AddVars(int pos)
{
  if (!strcmp(name, "assign")) {
    variables[args[0]->val] = pos;
    return;
  }
  return;
}


void Function::SetVars(int t)
{
  int i =0;

  for (i = 0; i < arity; i++) {
    switch(args[i]->type) {
    case T_VARIABLE:
      if (variables[args[i]->val] < t)
	variables[args[i]->val] = t;
      break;
    case T_FUNCTION:
      ((Function*)args[i])->SetVars(t);
      break;
    case T_ATOM:
      ((LiteralTerm*)args[i])->SetVars(t);
      break;
    default:
      break; // do nothing
    }
  }
}


int Function::AddVars(Variable *vs)
{
  int i = 0;
  int result = 0;
  
  for (i = 0; i < arity; i++) {
    switch(args[i]->type) {
    case T_VARIABLE:
      if (!internal_variable(args[i]->val) && !vs[args[i]->val]) { 
	vs[args[i]->val] = 1;
	result++;
      }
      break;
    case T_FUNCTION:
      result += ((Function*)args[i])->AddVars(vs);
      break;
    case T_ATOM:
      result += ((LiteralTerm*)args[i])->AddVars(vs);
      break;
    default:
      break; // do nothing
    }
  }
  return result;
}

void Term::Restrict(int t)
{
  long v;
  switch (type) {
  case T_VARIABLE:
    v = variables[val];
    if (MAKE_GLOBAL(v) == R_ASSIGNED) {
      variables[val] = R_FUNCTION_FAIL;
    } else if (GLOBAL_VARIABLE(v) && v < MAKE_GLOBAL(t)) {
      variables[val] = MAKE_GLOBAL(t);
    } else if (MAKE_GLOBAL(v) < MAKE_GLOBAL(t)) {
      variables[val] = t;
    }
    break;
  case T_FUNCTION:
    ((Function*)this)->Restrict(t);
  default:
    break;
  }
}
  
void Function::Restrict(int t)
{
  int i = 0;
  long v = 0;

  // treat assignments as a domain predicate
  if (assign) {
    if (t == R_CONDITIONED) {
      if (variables[args[0]->val] == R_NONE)
	variables[args[0]->val] = R_CONDITIONED;
    } else {
      if (GLOBAL_VARIABLE(t) || GLOBAL_VARIABLE(variables[args[0]->val]))
	variables[args[0]->val] = R_ASSIGNED;
      else
	variables[args[0]->val] = R_ASSIGNED_LOCAL;
      i++;
    }
  }
  if (t == R_CONDITIONED)
    t = R_NEGATIVE_LOCAL;
  
  
  for (; i < arity; i++) {
    if (args[i]->type == T_VARIABLE) {
      v = variables[args[i]->val];
      if (MAKE_GLOBAL(v) == R_ASSIGNED) {
	variables[args[i]->val] = R_FUNCTION_FAIL;
      } else if (GLOBAL_VARIABLE(v) && v < MAKE_GLOBAL(t)) {
	variables[args[i]->val] = MAKE_GLOBAL(t);
      } else if (MAKE_GLOBAL(v) < MAKE_GLOBAL(t)) {
	variables[args[i]->val] = t;
      }
    } else if (args[i]->type == T_FUNCTION)
      ((Function *)args[i])->Restrict(t);
    else if (args[i]->type == T_ATOM)
      ((LiteralTerm *)args[i])->Restrict(t);
  }
  if (assign)
    if (variables[args[0]->val] == R_ASSIGNED_LOCAL)
      variables[args[0]->val] = R_DOMAIN_LOCAL;
    else if (variables[args[0]->val] == R_ASSIGNED)
      variables[args[0]->val] = R_DOMAIN;

}

void Function::RemoveAssign()
{
  assign = 0;
  name = "eq";
  fun = function_table->GetFunction("eq");
  accept_constants = 1;
}


void Function::Print()
{
  int first = 1;
  int i;
  Range *r = NULL;
  output(cerr, "%s(", name);
  for (i = 0; i < arity; i++) {
    if (!first)
      output(cerr, ",");
    first = 0;
    switch (args[i]->type) {
    case T_VARIABLE:
      output(cerr, "%s",
	      variable_table->symbols[args[i]->val]); 
      break;
    case T_CONSTANT:
      if (IS_NUMBER(args[i]->val))
	output(cerr, "%ld", GET_VALUE(args[i]->val));
      else
	output(cerr, "%s",
		constant_table->symbols[GET_CONSTANT(args[i]->val)]); 
      break;
    case T_FUNCTION:
      ((Function*) args[i])->Print();
      break;
    case T_RANGE:
      r = (Range *) args[i];
      output(cerr, "%ld..%ld", r->start->val, r->end->val);
      break;
    case T_ATOM:
      ((LiteralTerm*)args[i])->lt->Print(0);
      break;
    default:
      output(cerr, "** error **");
    }
  }
  output(cerr, ")");
}
Range::Range(Term *s, Term *e, long ln, int con)
{
  type = T_RANGE;
  start = s;
  end = e;
  ground = s->ground && e->ground;
  has_range = 1;
  has_atom = 0;
  current = s->val;
  has_function = 0;
  lineno = ln;
  conjunctive = con;
}

void Range::Print()
{
  //  output(cerr, "%ld .. %ld", start, end);
}

void Range::AddDependencies(long dep)
{
  if (start->type == T_FUNCTION) {
    ((Function*)start)->AddDependencies(dep);
  }
  if (end->type == T_FUNCTION) {
    ((Function*)end)->AddDependencies(dep);
  }
}

Range *Range::DuplicateRange()
{
  Range *rn = new Range(start, end, lineno, conjunctive);
  return rn;
}
 
LiteralTerm::LiteralTerm(Literal *l)
{
  type = T_ATOM;

  has_atom = 1;
  has_range = 0;
  
  lt = l;
  lt->is_symbolic_function = 1;
  ground = lt->ground;
}

LiteralTerm * LiteralTerm::DuplicateAtom()
{
  LiteralTerm *at = new LiteralTerm(lt->Duplicate(0));
  if (!at)
    error(SYS_ERR, "malloc error");
  at->has_atom = 1;
  at->ground = ground;
  return at;
}
LiteralTerm::~LiteralTerm()
{
  if (lt)
    delete lt;
}


Instance LiteralTerm::ConstructAtom()
{
  const char *st = NULL;
  Instance *it = NULL;
  Instance res = ERROR_NUMBER;
  
  it = lt->GetInstance();

  st = get_ground_instance(it, lt->pred, lt->arity);

  if (*st == '_') {
    st += 3;
  }
  
  res = constant_table->Insert(st);
  return res;
}

int LiteralTerm::GetPos()
{
  int res= 0, i = 0, pos = 0;

  for (i = 0; i < lt->arity; i++) {
    switch (lt->args[i]) {
    case T_FUNCTION:
      res = max(res, ((Function*)lt->terms[i])->GetPos());
      break;
    case T_VARIABLE:
      pos = variables[lt->vars[i]];
      if (pos < 0)
	return -1;
      res = max(res, pos);
      break;
    case T_CONSTANT:
      break;
    case T_ATOM:
      pos = max(res, ((LiteralTerm*)lt->terms[i])->GetPos());
      break;
    default:
      int_error("misformed atom", "");
      break;
    }
  }
  return res;
}

void LiteralTerm::AddVarsByType(Variable *vs, int in_cond)
{
  lt->AddVarsByType(vs, in_cond);
}

int LiteralTerm::AddVars(Variable *vs)
{
  int i = 0;
  int result = 0;

  for (i = 0; i < lt->arity; i++) {
    switch (lt->args[i]) {
    case T_VARIABLE:
      if (vs[lt->vars[i]] == 0) {
	vs[lt->vars[i]] = 1;
	result++;
      }
      break;
    case T_FUNCTION:
      result += ((Function*)lt->terms[i])->AddVars(vs);
      break;
    case T_ATOM:
      result += ((LiteralTerm*)lt->terms[i])->AddVars(vs);
      break;
    default:
      break; // do nothing
    }
  }
  return result;
}

void LiteralTerm::SetVars(int t)
{
  int i =0;
 
  for (i = 0; i < lt->arity; i++) {
    switch(lt->args[i]) {
    case T_VARIABLE:
      if (variables[lt->vars[i]] < t)
	variables[lt->vars[i]] = t;
      break;
    case T_FUNCTION:
      ((Function*)lt->terms[i])->SetVars(t);
      break;
    case T_ATOM:
      ((LiteralTerm*)lt->terms[i])->SetVars(t);
      break;
    default:
      break; // do nothing
    }
  }
}

void LiteralTerm::Restrict(int t)
{
  int i = 0;
  Variable v;
  
  for (i = 0; i < lt->arity; i++) {
    if (lt->args[i] == T_VARIABLE) {
      v = variables[lt->vars[i]];
      if (MAKE_GLOBAL(v) == R_ASSIGNED) {
	variables[lt->vars[i]] = R_FUNCTION_FAIL;
      } else if (GLOBAL_VARIABLE(v) && v < MAKE_GLOBAL(t)) {
	variables[lt->vars[i]] = MAKE_GLOBAL(t);
      } else if (MAKE_GLOBAL(v) < MAKE_GLOBAL(t)) {
	variables[lt->vars[i]] = t;
      }
    } else if (lt->args[i] == T_FUNCTION) {
      ((Function *)lt->terms[i])->Restrict(t);
    } else if (lt->args[i] == T_ATOM)
      ((LiteralTerm *)lt->terms[i])->Restrict(t);
  }
  
}

int LiteralTerm::CheckGlobal(Variable *vars)
{
  int i = 0;
  for (i=0; i < lt->arity; i++) {
    switch (lt->args[i]) {
    case T_VARIABLE:
      if (vars[lt->vars[i]] != VAR_CONDITION)
	return 1;
      break;
    case T_FUNCTION:
      if (( (Function*)lt->terms[i])->CheckGlobal(vars))
	return 1;
      break;
    case T_ATOM:
      if (( (LiteralTerm*)lt->terms[i])->CheckGlobal(vars))
	return 1;
      break;
    default:
      break;
    }
  }
  return 0;
}

void LiteralTerm::AddGraphEdges(Graph *sg, long start)
{
  for (int i=0; i < lt->arity; i++) {
    switch(lt->args[i]) {
    case T_VARIABLE:
      sg->AddEdge(start, lt->vars[i], false);
      break;
    case T_FUNCTION:
      ((Function*)lt->terms[i])->AddGraphEdges(sg, start);
      break;
    case T_ATOM:
      ((LiteralTerm*)lt->terms[i])->AddGraphEdges(sg, start);
      break;
    default:
      break;
    }
  }

}

// instantiates all variables in a function
// does it by scanning through arguments to find variables, then
// searces the position where the variable occurs in 'p', and then
// uses the value that is in corresponding position of 'lt'
Function* Function::Instantiate(Literal *lt, Literal *p)
{
  Function *new_fun = new Function(fun, name, arity, lt->lineno);
  Term *tm = NULL;
  Function *f = NULL;
  Variable v = ERROR_NUMBER;
  int found = 0;
  int i = 0, j = 0;
  
  if (!new_fun)
    error(SYS_ERR, "malloc error");

  for (i = 0 ; i < arity; i++) {
    switch (args[i]->type) {
    case T_CONSTANT:
      tm = args[i]->Duplicate();
      new_fun->AddArgument(tm);
      break;
      
    case T_VARIABLE:
      v = args[i]->val;

      // find the variable position
      for (j = 0; j < p->arity && !found; j++) {
	if ((p->args[j] == T_VARIABLE) && (p->vars[j] == v)) {
	  found = 1;
	  switch (lt->args[j]) {
	  case T_CONSTANT:
	    tm = args[j]->Duplicate();
	    tm->type = T_CONSTANT;
	    tm->val = lt->cons[j];
	    new_fun->AddArgument(tm);
	    break;
	  case T_VARIABLE:
	    tm = args[j]->Duplicate();
	    tm->val = lt->vars[j];
	    tm->type = T_VARIABLE;
	    new_fun->AddArgument(tm);
	    break;
	  default:
	    int_error("invalid function instantiation","");
	    break;
	  }
	}
      }
      if (!found)
	int_error("invalid instantiation", "");
      break;
      
    case T_FUNCTION:
      f = ((Function*)args[i])->Instantiate(lt, p);
      new_fun->AddArgument(f);
      break;
      
    default:
      int_error("invalid function to instantiate", "");
      break;

    }
  }
  return new_fun;
}

Term *Term::CreateBound(ParseNode *pn)
{
  long value = ERROR_NUMBER;
  if (!pn) {
    return NULL;
  }

  value = pn->Eval();

  if (value != ERROR_NUMBER) {
    return new Term(T_CONSTANT, value, pn->lineno);
  } else {
    return ConstructArgument(pn);
  }
}

void LiteralTerm::Print()
{
  lt->Print(0);
}


char *Function::GetFunPrintString()
{
  ostringstream ss;
  int pos = 0, i = 0;
  pos += output(ss, "%s(", name);

  for (i = 0; i < arity; i++) {
    if (i > 0 && i <= arity-1) {
      pos += output(ss, ", ");
    }
    switch (args[i]->type) {
    case T_CONSTANT:
      if (IS_NUMBER(T_CONSTANT)) {
        pos += output(ss, "%ld", args[i]->val);
      }	else {
        pos += output(ss, "%s",
                       constant_table->LookupByValue(args[i]->val));
      }
      break;
    case T_VARIABLE:
      pos += output(ss, "%s",
                     variable_table->LookupByValue(MAKE_CONSTANT(args[i]->val)));
      break;
    case T_FUNCTION:
      pos += output(ss, "%s",
		     ((Function*)args[i])->GetFunPrintString());
      break;
    case T_ATOM:
      pos += output(ss, "%s",
		     ((LiteralTerm*)args[i])->lt->GetPrintString());
      break;
    default:
      int_error("printing strange function", "");
    }
  }
  ss << ")";
  return clone_string(ss.str().c_str());
}
