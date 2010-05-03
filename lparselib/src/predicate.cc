// predicate.cc -- implementation of predicate data type
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
#include <stdlib.h>
#include <ctype.h>

#ifndef PREDICATE_H
#include "predicate.h"
#endif
#ifndef SYMBOL_H
#include "symbol.h"
#endif
#ifndef ITERATE_H
#include "iterate.h"
#endif
#ifndef ERROR_H
#include "error.h"
#endif

lp_smodels *Predicate::transitive_program;

Predicate::Predicate()
  : indices(NULL), atoms(NULL), existing_indexes(0), arity(-1),
    rules(UNORDERED), special_rules(UNORDERED), ranges(UNORDERED),
    it(NULL), name(NULL), status(DM_UNKNOWN) 
{
  pred = -1;
  complement = -1;
  negation = -1;
  emitted = NULL;
  follows = 0;
  external_atom = 0;
  depends_on_external_atoms = 0;
  transitive_domain = 0;
  hidden = 0;
  shown = 0;
  special = 0;
  has_rule = 0;
  is_warned = 0;
  is_internal = 0;
  is_priority_predicate = 0;
  is_priority_meta_predicate = 0;
  is_only_symbolic_function = 1;
  normalized_complement = -1;
  predicate_weight_list = NULL;
  weight_list = NULL;
  first_line = 0;
  name = 0;
  num_components = 0;
  signature = 0;
}

int is_negation_name(char *st)
{
  while (*st == '_')
    st++;

  return *st == '-';
}



Predicate::~Predicate()
{
  int i;
  if (indices) {
    for (i = 0; i < arity; i++) {
      if (indices[i])
  	delete indices[i];
    } 
    delete [] indices;
  }
  if (it)
    delete it;
  if (atoms)
    delete atoms;
  if (name && strcmp(name, FALSE_NAME))
    delete [] name;
  rules.ClearIterator();
  //  Rule *rl;
  //while ((rl = rules.Iterate()))
  //
  //delete rl;
//   special_rules.ClearIterator();
//   while ((rl = special_rules.Iterate()))
//     delete rl;
  // cerr << "Rules" << endl;
  ranges.Clear();
  //  cerr << "Ranges" << endl;
  if (predicate_weight_list) {
    delete predicate_weight_list;
  }
  //  cerr << "Predicate Weight" << endl;
  if (weight_list) {
    delete weight_list;
  }
    
  //  cerr << "Weight" << endl;
  if (emitted) {
    delete emitted;
  }
     
  
  //  cerr << "Hei hoo" << endl;
}



// returns NULL if the predicate name is already complemented 
char *complement_name(char *nm)
{
  int len = strlen(nm);
  static char comp_name[BUFFER_LENGTH] = { 0 };

  if (len > BUFFER_LENGTH -2) {
    error(FATAL_ERR, "too long predicate name '%s'", nm);
  }

  if (nm[len-1] == '\'') // || nm[0] == '_' )
    return NULL;
  
  strcpy(comp_name, nm);
  strcpy(&comp_name[len], "'");
  return comp_name;
}

char *complement_atom_name(char *nm)
{
  static char comp_name[BUFFER_LENGTH] = { 0 };
  int pos = 0;
  
  while (*nm && *nm != '\'' && *nm != '(') {
    comp_name[pos++] = *nm++;
  }
  if (*nm == '\'')
    return NULL;
  
  comp_name[pos++] = '\'';
  while (*nm) {
    comp_name[pos++] = *nm++;
  }
  comp_name[pos++] = '\0';
  return comp_name;
}

  

// negate the atom name: foo -> -foo && -foo -> foo
const char *negation_name(char *nm)
{
  static string str;
  //  int len = strlen(nm);
  int underscores = 0;
  //long pos = 0;
  ostringstream ss;
  ss.str("");

  while (*nm == '_') {
    underscores++;
    nm++;
  }
  
  if (*nm == '-') {
    for (int i = 0; i < underscores; i++) {
      ss << "_";
      //      pos += sprintf(&neg_name[pos], "_");
    }
    output(ss, "%s", nm+1);
    //    neg_name[len-1] = '\0';
  } else {
    for (int i = 0; i < underscores; i++) {
      ss << "_";
    }
    output(ss, "-%s", nm);
  }
  str = ss.str();
  return str.c_str();
}

RestrictType Predicate::CheckRestrict(int print)
{
  debug(DBG_RESTRICT, 2, "Checking restrict for %s", name);
  assert(arity >= 0);
  RestrictType result = RT_STRONG, tmp = RT_STRONG;
  Rule *rl = NULL;

  if ((status == DM_SPECIAL) ||
      (status == DM_INTERNAL))
    return RT_STRONG;
  
  while ((rl = rules.Iterate())) {
    tmp = rl->CheckRestrict(print);
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

  debug(DBG_RESTRICT, 2, "\tResult: $d", result);
  return result;
}

void Predicate::AddRule(Rule *rl, int)
{
  is_only_symbolic_function = 0;
  has_rule = 1;

  // check if the rule is a fact
  if ((rl->positive.Size() == 0) && (rl->negative.Size() == 0) &&
      (rl->functions.Size() == 0) && rl->ground) {
    AddInstance(rl->head->cons);
    if (sys_data.print_domains == PR_ALL) {
      rl->fact = 1;
      rules.Insert(rl);
      //    if (sys_data.use_regular_models && cmp) {
      //      predicates[complement]->AddInstance(rl->head->cons);
      //    }
    } else {
      delete rl;
    }
  } else {
    rules.Insert(rl);
    //    if (sys_data.use_regular_models && cmp) {
    //      Rule *r = rl->CreateComplement();
    //      predicates[complement]->rules.Insert(r);
    //    }
  }
}


void Predicate::CreateComplementRules()
{
  Rule *rl = NULL;

  if (pred == complement) {
    return;
  }
  
  while ((rl = rules.Iterate())) {
    Rule *r = rl->CreateComplement();
    predicates[complement]->rules.Insert(r);
  }
  
  while ((rl = special_rules.Iterate())) {
    Rule *r = rl->CreateComplement();
    predicates[complement]->special_rules.Insert(r);
  }
}

void Predicate::CreateAllComplementRules()
{
  long p;
  for (p = 0 ; p < predicate_table->Size(); p++) {
    if (predicates[p]->complement > p) {
      predicates[p]->CreateComplementRules();
    }
  }
}

void Predicate::AddSpecialRule(Rule *rl, int )
{
  has_rule = 1;
  special = 1;
  special_rules.Insert(rl);

  // check if this is the internal false predicate that is otherwise a
  // domain predicate and false.
  if (false_lit && (false_lit->pred == pred) && (status == DM_DOMAIN)) {
    status = DM_FALSE;
  }

  //  if (sys_data.use_regular_models && cmp) {
  //    Rule *r = rl->CreateComplement();
  //    predicates[complement]->special_rules.Insert(r);
  //  }

}


void Predicate::AddInstance(Instance *inst)
{
  if (arity == 0)
    follows = 1;
  else 
    atoms->Insert(inst);
}

void Predicate::AddWeight(Weight *nw, Literal *lt, int global)
{
  WeightNode *wn = NULL;

  wn = new WeightNode;
  wn->wt = nw;
  wn->lt = lt;
  if (global) {
    wn->prev = predicate_weight_list;
    predicate_weight_list = wn;
  } else {
    wn->prev = weight_list;
    weight_list = wn;
  }
}


void Predicate::SetArity(int ar)
{
  arity = ar;

  if (ar > 0) {
    if (!atoms) {
      atoms = new InstanceSet(sys_data.domain_size, ar);
      it = new InstanceIterator(atoms);
      if (!atoms || ! it)
	error(SYS_ERR, "malloc error");
    } else {
      atoms->Clear(ar);
      it->Clear();
    }
  }
}



void Predicate::SetStatus(DomainType r)
{
  status = r;

}
  

void Predicate::CreateIndex(int ind)
{
  Instance *item = NULL;
  assert((ind >= 0) && (ind < arity));

  if (!indices) {
    indices = new Index*[arity];
    if (!indices)
      error(SYS_ERR, "malloc error");
    for (long i = 0; i < arity; i++ )
      indices[i] = NULL;
  }
  if (existing_indexes & (1 << ind)) 
    int_error("trying to create same index two times in predicate"
	      " '%s'", name);

  indices[ind] = new Index(arity, ind);
  if (!indices[ind])
    error(SYS_ERR, "malloc error");

  existing_indexes |= (1 << ind);

  it->Clear();
  while ((item = it->Iterate())) 
    indices[ind]->Insert(item);
  return;
}



void Predicate::Emit()
{
  Instance *item = NULL;
  Rule *rl = NULL;
  Literal *head = NULL;
  static Instance *items = NULL;
  int i = 0;

  // check if this is a domain predicate that shouldn't be printed
  if (!external_atom && !depends_on_external_atoms &&
      (((sys_data.print_domains == PR_NONE) &&
	(status == DM_DOMAIN)) || (status == DM_INTERNAL) ||
       (status == DM_SPECIAL)))
    return;
  if (sys_data.emit_text && hidden &&
      ((status == DM_DOMAIN)) ||
      (status == DM_INTERNAL) ||(status == DM_SPECIAL))
    return;

  
  if (it)
    it->Clear();


  if (!items) {
    items = new Instance[predicate_table->MaxArity()+1];
    if (!items)
      error(SYS_ERR, "malloc error");
    memset(items, 0, sizeof(Instance) * predicate_table->MaxArity()+1); 
  }

  // don't print external atoms 
  if (!external_atom && !depends_on_external_atoms) {
    if (sys_data.print_domains != PR_ALL) {
      if (arity == 0) {
	if (follows) {
	  // predicate is a ground fact. Construct a rule and emit it.
	  head = new Literal(pred, NULL, 0, 0);
	  if (!head)
	    error(SYS_ERR, "malloc error");
	  rl = new Rule(head, BASICRULE);
	  if (!rl)
	    error(SYS_ERR, "malloc error");
	  rl->EmitGround(0,0,1);
	  delete rl;
	} else if (false_lit && (false_lit->pred == pred) &&
		   (sys_data.output_version < 2)) {
	  // construct the rule false :- false. (this is needed because 
	  // otherwise smodels 1 won't work when false is not true :-)
	  head = false_lit;
	  rl = new Rule(head, BASICRULE);
	  if (!rl)
	    error(SYS_ERR, "malloc error");
	  rl->AddLiteral(head); 
	  rl->EmitGround(0,0,0);
	  delete rl;
	}
      }
    }

    // print all the ground instances 
    if ((arity > 0) && atoms && (atoms->Size() > 0) &&
	(sys_data.print_domains != PR_ALL)) { 
      head = new Literal(pred, items, arity, 0);
      if (!head)
	error(SYS_ERR, "malloc error");
      
      rl = new Rule(head, BASICRULE);
      if (!rl)
	error(SYS_ERR, "malloc error");
      
      while ((item = it->Iterate())) {
	for (i = 0; i < arity; i++) {
	  head->cons[i] = item[i];
	}
	rl->EmitGround(0,0,1);
      }
      delete rl;
    }
  }

  if (!sys_data.only_facts) {
	rules.ClearIterator();
	special_rules.ClearIterator();
	// and the normal rules
	if (!external_atom &&
		(depends_on_external_atoms || (status != DM_DOMAIN))) {
	  while ((rl = rules.Iterate())) {
		if ((sys_data.print_domains == PR_ALL) && rl->ground)
		  rl->EmitGround(0,0,0);
		else
		  rl->GroundRule(GRD_RULE);
	  }
	  while ((rl = special_rules.Iterate()))
		rl->GroundSpecialRule(GRD_RULE);
	}
  }

}

void Predicate::CalculateDomain()
{
  debug(DBG_GROUND, 2, "Calculating domain for %s", name);
  Rule *rl = NULL;
  int type = GRD_DOMAIN;
  
  // don't change status of internal range predicates
  if (status != DM_INTERNAL)
    SetStatus(DM_DOMAIN);
  else if (is_internal) {
    status = DM_INTERNAL;
  }

  // expand ranges one at a time
  if (ranges.Size() > 0) {
    ranges.ClearIterator ();
    Range *r;
    long start;
    long end;
    long new_instance;
    while ((r = ranges.Iterate())) {
      start = r->start->Eval();
      end = r->end->Eval();

      if (start > end) {
	long tmp_long = start;
	start = end;
	end = tmp_long;
      }
      // add the values of range to the predicate
      for (long j = start; j <= end; j++) {
	new_instance = MAKE_NUMBER(j);
	AddInstance(&new_instance);
      }
    }
  }
  // ground all rules one at time
  rules.ClearIterator();
  while ((rl = rules.Iterate())) {
    rl->GroundRule(type);
  }
}

void Predicate::AddSCCFacts()
{
  Instance *i = 0;
  
  InstanceIterator *iter = 0;

  if (atoms) {
    iter = new InstanceIterator(atoms);
    while ((i = iter->Iterate())) {
      transitive_program->AddFact(pred, i);
    }
    delete iter;
  } else {
    if (follows)
      transitive_program->AddFact(pred, 0);
  }
}
//  transitive_program->model();
//
//  while ((i = transitive_program->Iterate())) {
//    AddInstance(i);
//  }
//  delete transitive_program;


void Predicate::AddRange(Range *r)
{
  ranges.Insert(r);
}

void Predicate::EmitAll()
{
  long i = 0;

  if (false_lit)
    atom_table->Insert(FALSE_NAME);

  for( i = 0; i < predicate_table->Size(); i++) {
    if (predicates[i]->external_atom)
      dependency_graph->PropagateExternalDefinitions(i);
  }
  
  for( i = 0; i < predicate_table->Size(); i++)
    if (!predicates[i]->is_priority_predicate)
      predicates[i]->Emit();

}

void Predicate::PrintAllRules()
{
  long i = 0;
  Rule *rl = NULL;
  
  for (i = 0; i < predicate_table->Size(); i++) {
    predicates[i]->rules.ClearIterator();
    while ((rl = predicates[i]->rules.Iterate())) {
      rl->PrintRule();
    }
  }
}


RestrictType Predicate::CheckAllRestricts()
{
  RestrictType rt = RT_STRONG, tmp = RT_STRONG;
  long i = 0;
  
  for( i = 0; i < predicate_table->Size(); i++){
    tmp = predicates[i]->CheckRestrict(1);
    switch (tmp) {
    case RT_NONE:
      rt = RT_NONE;
      break;
    case RT_WEAK:
      if (rt > RT_WEAK)
	rt = RT_WEAK;
      break;
    default:
      break;
    }
  }

  if (sys_data.use_priorities) {
    priority_data.CheckRestrict(1);
  }
  
  return rt;
}

void Predicate::PrintDebugData()
{
  long i = 0;
  Rule *rl = NULL;

  
  for (i = 0; i < predicate_table->Size(); i++) {
    predicates[i]->rules.ClearIterator();
    while ((rl = predicates[i]->rules.Iterate())) {
      rl->PrintDebugInformation();
    }
    predicates[i]->special_rules.ClearIterator();
    while ((rl = predicates[i]->special_rules.Iterate())) {
      rl->PrintDebugInformation();
    }
  }
  output(sys_data.output_file, "%% Total rules: %ld\n", sys_data.ground_rules);
}

void Predicate::DefineAllComplements()
{
  long i;
  DomainType tp;
  long p;
  long orig_size = predicate_table->Size();
  
  if (false_lit) {
    predicates[false_lit->pred]->complement =
      false_lit->pred;
  }
  
  for (i = 0; i < orig_size; i++) {
    tp = predicates[i]->Status();
    if (tp == DM_DOMAIN) {
      predicates[i]->complement = i; // SetStatus(DM_DOMAIN);
    } else {
    //    if (tp == DM_FALSE) {
      p = DefineComplement(i);
    //}
    }
  }
  CreateAllComplementRules();
}


long Predicate::DefinePredicate(char *nm, int ar, long ln)
{
  long p = -1;

  // check whether the predicate is classically negated
  if (is_negation_name(nm)) {
    if (!sys_data.true_negation) {
      error(USR_ERR, "%ld: The minus sign in the predicate "
	    "`%s/%d' was interpreted as a classical negation but the "
	    "command line argument `--true-negation' was not used.\n"
	    "If you want it to be interpreted as an arithmetic minus "
	    "operation, please add a space between it and the "
	    "following symbol.", ln, nm, ar);
    }
  }

  p = predicate_table->Insert(nm, ar);

  
  if (p < 0)
    int_error("unknown predicate '%s'", nm);
  if (predicates[p]->pred == p)
    return p;  // already defined

  if (!strcmp(nm, FALSE_NAME)) {
    predicates[p]->SetStatus(DM_FALSE);
  } else if (sys_data.hide_all)
    predicates[p]->hidden = 1;
  
  
  dependency_graph->AddNode(p);
  predicates[p]->SetName(nm);
  predicates[p]->SetPred(p);
  predicates[p]->SetArity(ar);
  predicates[p]->first_line = ln;

  if (sys_data.use_priorities) {
    if ((ar == 2) && !strcmp(nm, PRIORITY_PREDICATE)) {
      predicates[p]->is_priority_predicate = 1;
    }
  }

  
#if 0
  if (sys_data.use_regular_models && !strchr(nm, '\'')) {
    if (!strcmp(nm, FALSE_NAME))
      predicates[p]->complement = p;
    else
      DefineComplement(p);
  }
#endif
  
  if (sys_data.true_negation) {
    DefineNegated(p);
    false_lit = Predicate::DefineFalseLiteral();
  }
  
  // check if there is a predicate with same name but different arity 
  if (sys_data.warnings & WARN_ARITY) {
    long p2 = predicate_table->CheckName(nm, ar);
    if (p2 >= 0) {
      warn(WARN_ARITY, predicates[p2]->first_line,
	   "predicate '%s' is used with %d argument%s at line %ld, while " 
	   "it is also used with %d argument%s at line %ld.",
	   nm, predicates[p2]->Arity(),
	   (predicates[p2]->Arity() == 1) ? "" : "s",
	   predicates[p2]->first_line,
	   ar,
	   (ar == 1) ? "" : "s",
	   ln);
      predicates[p2]->is_warned = 1;
    }
  }
  return p;
}

// generate a dummy head that cannot be true in any case. 
long Predicate::ConstructDummy()
{
  static char *buf = 0;
  int first = 0;
  
  if (!buf) {
    buf = clone_string("_dummy");
    first = 1;
  }
  long p = DefinePredicate(buf, 0, 1);
  if (first) {
    Literal *lt = new Literal();
    lt->pred = p;
    lt->negative = 1;
    compute->rl->AddLiteral(lt);
  }
  return p;
}

long Predicate::DefineSystemPredicate(DomainType tp)
{
  //  static char buf[RANGE_MAX_NAME] = { 0 };
  static stringstream ss;
  ss.str("");
  //  const int pos = 4;
  int val = 0;
  long p = -1;

  val = output(ss, "_int%ld", sys_data.internal_atoms++);
  
  p =  DefinePredicate(clone_string(ss.str().c_str()), 0, 0);
  predicates[p]->SetStatus(tp);
  predicates[p]->is_internal =1;
  
  if (sys_data.true_negation) {
    predicates[predicates[p]->negation]->SetStatus(tp);
    predicates[predicates[p]->negation]->is_internal = 1;
  }

  
  //  if (sys_data.use_regular_models) {
  //    DefineComplement(p);
  //  }
  return p;
}

long Predicate::DefinePriorityPredicate()
{
  long user_priority = predicate_table->Lookup(PRIORITY_PREDICATE, 2);

  long help_predicate = DefineRangePredicate();
  predicates[help_predicate]->SetStatus(DM_UNKNOWN);
  predicates[help_predicate]->is_priority_predicate = 1;
  if (user_priority < 0) {
    int_error("missing priority predicate definition","");
  }
  
  long p = DefinePredicate(clone_string(PRIORITY_TCL), 2,
			   predicates[user_priority]->first_line);

  predicates[p]->is_priority_predicate = 1;
  predicates[p]->is_priority_tcl = 1;
  
  Variable v[2] = { 0 }, v1, v2, v3;
  v1 = variable_table->Insert("I_1");
  v2 = variable_table->Insert("I_2");
  v3 = variable_table->Insert("I_3");

  // help_predicate(I_1) :- prefer(I_1, I_2).
  v[0] = v1;
  v[1] = v2;

  Literal *hd = new Literal(v[0], help_predicate);
  hd->lineno = predicates[p]->first_line;

  Rule *rl = new Rule(hd, BASICRULE);

  Literal *lt = new Literal(v, user_priority, 2);
  lt->lineno = predicates[p]->first_line;
  rl->AddLiteral(lt);
  
  predicates[help_predicate]->AddRule(rl);

  hd = new Literal(v[1], help_predicate);
  hd->lineno = predicates[p]->first_line;

  rl = new Rule(hd, BASICRULE);

  lt = new Literal(v, user_priority, 2);
  lt->lineno = predicates[p]->first_line;
  rl->AddLiteral(lt);
  
  predicates[help_predicate]->AddRule(rl);

  // priority'(I_1, I_3) :- priority'(I_1, I_2), priority(I_2, I_3).
  v[0] = v1;
  v[1] = v3;

  hd = new Literal(v, p, 2);
  hd->lineno = predicates[p]->first_line;

  
  rl = new Rule(hd, BASICRULE);

  v[0] = v1;
  v[1] = v2;
  
  lt = new Literal(v, p, 2);
  lt->lineno = predicates[p]->first_line;
  rl->AddLiteral(lt);

  v[0] = v2;
  v[1] = v3;
  
  lt = new Literal(v, user_priority, 2);
  lt->lineno = predicates[p]->first_line;
  rl->AddLiteral(lt);

  lt = new Literal(v1, help_predicate);
  lt->lineno = predicates[p]->first_line;
  rl->AddLiteral(lt);

  Term *l = new Term(T_VARIABLE, v1, predicates[p]->first_line);
  Term *r = new Term(T_VARIABLE, v2, predicates[p]->first_line);
  Function *fun = new Function("neq", l, r,
			       predicates[p]->first_line);
  rl->AddFunction(fun);
  l = new Term(T_VARIABLE, v1, predicates[p]->first_line);
  r = new Term(T_VARIABLE, v3, predicates[p]->first_line);
  fun = new Function("neq", l, r,
		     predicates[p]->first_line); 
  rl->AddFunction(fun);
  
  predicates[p]->AddRule(rl);

  // priority'(I_1, I_2) :- priority(I_1, I_2);
  v[0] = v1;
  v[1] = v3;
  
  hd = new Literal(v, p, 2);
  rl = new Rule(hd, BASICRULE);

  lt = new Literal(v, user_priority, 2);
  lt->lineno = predicates[p]->first_line;
  rl->AddLiteral(lt);

  
  predicates[p]->AddRule(rl);

  dependency_graph->AddEdge(p, user_priority, false);
  dependency_graph->AddEdge(p, p, false);
  dependency_graph->AddEdge(p, help_predicate, false);
  return p;
}


long Predicate::DefineRangePredicate()
{
  ostringstream ss;
  //  const int pos = 4;
  int val = 0;
  long p = -1;
   
  val = output(ss,"_int%ld", sys_data.internal_atoms++);
  sys_data.num_ranges++;
 
  p = DefinePredicate(clone_string(ss.str().c_str()), 1, 0);
  predicates[p]->SetStatus(DM_INTERNAL);
  predicates[p]->is_internal = 1;

  //  if (sys_data.use_regular_models) {
  //    predicates[predicates[p]->complement]->is_internal = 1;
  //    predicates[predicates[p]->complement]->SetStatus(DM_INTERNAL);
  //  }
  return p;
}


long Predicate::DefineInconsistentPredicate()
{
  //  static char buf[RANGE_MAX_NAME] = { 0 };
  ostringstream ss;

  ss << INCONSISTENT_NAME;
  long p = -1;
  
  p =  DefinePredicate(clone_string(ss.str().c_str()), 0, 0);
  predicates[p]->SetStatus(DM_FALSE);
  predicates[p]->is_internal =1;
  
  return p;
}


Literal *Predicate::DefineFalseLiteral()
{
  if (false_lit)
    return false_lit;
  
  long p = DefinePredicate(FALSE_NAME, 0, 0);
  false_lit = new Literal(p, NULL, 0,0);
  atom_table->Insert(FALSE_NAME);
  dependency_graph->AddEdge(p, p, 1); // negative dep. 
  predicates[p]->is_internal = 1;
  return false_lit;
}

// initialize a complementary predicate to allow regular model
// semantics to work 
long Predicate::DefineComplement(long p)
{
  // sanity check
  if (predicates[p]->pred != p)
    int_error("trying to define a complement of unexisting predicate","");

  long comp = -1;
  Predicate *original = predicates[p];

  // already defined?
  if (original->complement >= 0)
    return original->complement;

  char *new_name = complement_name(original->Name());
  if (!new_name) // complemented already 
    return original->complement;
  
  comp = DefinePredicate(clone_string(new_name), original->Arity(), original->first_line);
  
  original->complement = comp;
  predicates[comp]->complement = p;

  if (original->Internal()) {
    predicates[comp]->is_internal = 1;
  }
  
  // dependency_graph->AddEdge(comp, p, true); <= not needed
  dependency_graph->AddEdge(p, comp, true);
  
  return comp;
}

long Predicate::DefineNormalRuleComplement (long pred)
{
  if (predicates[pred]->normalized_complement >= 0) {
    return predicates[pred]->normalized_complement;
  } else {
    predicates[pred]->normalized_complement = DefineSystemPredicate();
  }
  return predicates[pred]->normalized_complement;
}


void Predicate::DefineNegated(long p)
{
  // sanity check
  if (predicates[p]->pred != p)
    int_error("trying to define a negation of unexisting predicate","");


  long neg = -1;
  Predicate *original = predicates[p];

  // already defined?
  if (original->negation >= 0)
    return;

  const char *new_name = negation_name(original->Name());

  neg = DefinePredicate(clone_string(new_name), original->Arity(),
			original->first_line); 

  original->negation = neg;
  predicates[neg]->negation = p;

  if (original->Internal()) {
    predicates[neg]->is_internal = 1;
  }
}

long Predicate::Size()
{
  if (!atoms)
    return 0;
  else
    return atoms->Size();
}

long Predicate::ExtensionSize()
{
  if (atoms)
    return atoms->Size();
  else if (follows)
    return 1;
  else
    return 0;
}

void Predicate::EmitComplements()
{
  long at = 0;
  long cmpl = 0;
  char *at_st = NULL;
  char *complement_st = NULL;

  for (at = 0; at < atom_table->Size(); at++) {
    at_st = atom_table->symbols[at];
    complement_st = complement_atom_name(at_st);

    if (complement_st) {
      if (sys_data.emit_text) {
        output(sys_data.output_file, "%s :- %s.\n", complement_st, at_st);
        if (sys_data.regular_level == REGULAR_ALL_CONSTRAINTS) {
          output(sys_data.output_file, "%s :- %s.\n", at_st ,
                  complement_st);
        }
      } else {
        cmpl = atom_table->Insert(complement_st);
        //	if (cmpl < 0) {
        //	  int_error("missing complementement", "");
        //	}
        
        if (sys_data.output_version >= 2) {
          output(sys_data.output_file, "%d %ld 1 0 %ld\n",
                  BASICRULE, cmpl+1, at+1);
          if (sys_data.regular_level == REGULAR_ALL_CONSTRAINTS) {
            output(sys_data.output_file, "%d %ld 1 0 %ld\n",
                    BASICRULE, at+1, cmpl+1);
          }
        } else {
          output(sys_data.output_file, "%ld  1 %ld 0\n",
                  cmpl+1, at +1);
          if (sys_data.regular_level == REGULAR_ALL_CONSTRAINTS) {
            output(sys_data.output_file, "%ld  1 %ld 0\n",
                    at +1, cmpl+1);
          }
        }
      }
    }
  }
}




void Predicate::EmitNegations()
{
  long at = 0;
  long neg = 0;
  long false_at = atom_table->Lookup(FALSE_NAME) +1;
  long inconsistent_at = -1;
  char *at_st = NULL;
  const char *neg_st = NULL;

  if (sys_data.inconsistent_answers)
    inconsistent_at = atom_table->Insert(INCONSISTENT_NAME) + 1;
  
  for (at = 0; at < atom_table->Size(); at++) {
    at_st = atom_table->symbols[at];
    
    if (is_negation_name(at_st)) {
      neg_st = negation_name(at_st);

      if (sys_data.emit_text) {

	output(sys_data.output_file, "%s :- %s, %s.\n",
		sys_data.inconsistent_answers ? INCONSISTENT_NAME : FALSE_NAME ,
		neg_st, at_st);
      } else {
	neg = atom_table->Insert(neg_st);
	if (neg < 0) {
	  int_error("missing complementement", "");
	}
	
	if (sys_data.output_version >= 2) {
	  output(sys_data.output_file, "%d %ld 2 0 %ld %ld\n",
		  BASICRULE, sys_data.inconsistent_answers ?
		  inconsistent_at : false_at,  neg+1, at+1);

	} else {
	  output(sys_data.output_file, "%ld 2 %ld %ld 0\n",
		  sys_data.inconsistent_answers ? inconsistent_at :
		  false_at, neg+1, at+1);
	 
	}
      }
    } else if (sys_data.totalize_negations) {
      long p = decode_atom(at_st);
      if (!predicates[p]->is_internal) {
	neg_st = negation_name(at_st);
	
	if (sys_data.emit_text) {
	  output(sys_data.output_file, "%s :- not %s.\n",
		  neg_st, at_st);
	} else {
	  neg = atom_table->Insert(neg_st);
	  if (neg < 0) {
	    int_error("missing complementement", "");
	  }
	  
	  if (sys_data.output_version >= 2) {
	    output(sys_data.output_file, "%d %ld 1 1 %ld\n",
		    BASICRULE, neg+1, at+1);
	  } else {
	    output(sys_data.output_file, "%ld 1 %ld 0\n",
		    neg+1, at+1);
	    
	  }
	}
      }
    }
  }
}

void Predicate::CheckUnsatisfiable()
{
  for (long i = 0; i < predicate_table->Size(); i++) {
    if (is_negation_name(predicates[i]->Name()))
      continue;
    if (!predicates[i]->Internal() &&
	!predicates[i]->is_only_symbolic_function && !predicates[i]->has_rule) {
      warn(WARN_UNSAT, predicates[i]->first_line,
	   "predicate '%s/%d' doesn't occur in any rule head.", 
	   predicates[i]->Name(), predicates[i]->Arity());
    }
  }
}

int Predicate::NeedsToBePrinted()
{
  return external_atom || depends_on_external_atoms;
}



long Predicate::decode_atom(const char *st, Instance *i)
{
  long pr = -1;
  long pos = 0;
  int paren_depth = 0;
  int inside_quotes = 0;
  long a = 0;
  long val;
  const char *p;
  char nm[BUFFER_LENGTH] = { 0 };
  char arg[BUFFER_LENGTH] = { 0 };
  
  // find the name
  pos = 0;
  for (p = st; *p && (*p != '(') ; p++) {
    nm[pos++] = *p;
  }
  nm[pos] = '\0';
  
  pos = 0;
  if (*p) {
    p++;
    for ( ; *p; p++) {
      if ( (*p == ',') && !inside_quotes && (paren_depth == 0)) {
	if (isdigit(arg[0])) {
	  arg[pos] = '\0';
	  i[a] = strtol(arg, NULL, 10);
	  a++;
	  pos = 0;
	} else {
	  arg[pos] = '\0';
	  val = constant_table->Insert(arg);
	  i[a] = val;
	  a++;
	  pos = 0;
	}
      } else if (*p == '\"') {
	inside_quotes = !inside_quotes;
	arg[pos++] = *p;
      } else if (*p == '(' && !inside_quotes) {
	paren_depth++;
	arg[pos++] = *p;
      } else if (*p == ')' && !inside_quotes) {
	paren_depth--;

	if (paren_depth == -1) {
	  if (isdigit(arg[0])) {
	    arg[pos] = '\0';
	    i[a] = strtol(arg, NULL, 10);
	    a++;
	    pos = 0;
	  } else {
	    arg[pos] = '\0';
	    val = constant_table->Insert(arg);
	    i[a] = val;
	    a++;
	    pos = 0;
	  }
	} else {
	  arg[pos++] = *p;
	}
      } else {
	arg[pos++] = *p;
      }
    }
  }
  pr = DefinePredicate(nm, a, -1);
  return pr;
}

long Predicate::decode_atom(const char *st)
{
  long pos = 0;
  int paren_depth = 0;
  int inside_quotes = 0;
  int args = 0;

  char name_string[BUFFER_LENGTH];
  const char *p;

  // find out name
  for (p = st; *p && (*p != '(') ; p++) {
    name_string[pos++] = *p;
  }
  name_string[pos] = '\0';

  // find out arity
  if (*p) {
    args++;
    p++;
    for ( ; *p; p++) {
      if ( (*p == ',') && !inside_quotes && (paren_depth == 0)) {
	args++;
      } else if (*p == '\"') {
	inside_quotes = !inside_quotes;
      } else if (*p == '(' && !inside_quotes) {
	paren_depth++;
      } else if (*p == ')' && !inside_quotes) {
	paren_depth--;
      }
    }
  }
  return DefinePredicate(clone_string(name_string), args, -1);
}


// returns the ground instance that corresponds to the n:th argument
// of the predicate string st or -1 if none exists
Instance Predicate::GetArgumentInstanceFromString(char *st, int n)
{
  Instance result = -1;
  char buf[BUFFER_LENGTH] = { 0 };
  char *p = st;
  size_t pos = 0;
  int paren_depth = 0;
  int current_argument = 1;
  int end = 0, end_inner = 0;
  int inside_quotes = 0;
  
  // search the beginning of the first argument;
  while (*p && (*p != '(')) {
    p++;
  }

  if (!*p) {
    return -1;
  }
  p++;
  // find the beginning of the correct argument
  while(!end && (current_argument < n)) {
    end_inner = 0;
    
    while (!end_inner && *p) {
      switch (*p) {
      case '(':
	if (!inside_quotes)
	  paren_depth++;
	break;
      case ')':
	if (!inside_quotes) {
	  paren_depth--;
	  if (paren_depth < 0) {
	    end = 1;
	  }
	}
	break;
      case ',':
	if (!inside_quotes) {
	  if (paren_depth == 0) {
	    current_argument++;
	    end_inner = 1;
	  }
	}
	break;
      case '"':
	inside_quotes = !inside_quotes;
	break;
      default:
	break;
      }
      p++;
    }
  }

  if (current_argument < n) { // too few arguments
    return -1;
  }

  st = p;
  // find the end of the argument
  pos = 0;
  end = 0;

  while (!end && *p) {
    switch (*p) {
    case '(':
      if (!inside_quotes) {
	paren_depth++;
      }
      break;
    case ')':
      if (!inside_quotes) {
	paren_depth--;
	if (paren_depth < 0) {
	  end = 1;
	}
      }
      break;
    case ',':
      if (!inside_quotes) {
	end = 1;
      }
      break;
    case '"':
      inside_quotes = !inside_quotes;
      break;
    default:
      break;
    }
    pos++;
    p++;
  }
  pos--;
  strncpy(buf, st, pos);
  buf[pos] = '\0';
  
  result = atom_table->Insert(buf);
  return result +1;
}

void Predicate::DefineImplicitDomains()
{
  long i;

  for (i = 0; i < predicate_table->Size(); i++) {
    predicates[i]->DefineImplicitDomain();
  }
}

void Predicate::DefineImplicitDomain()
{
  Rule *rl;
  while ((rl = rules.Iterate())) {
    rl->DefineImplicitDomain();
  }
  while ((rl = special_rules.Iterate())) {
    rl->DefineImplicitDomain();
  }
}
    

void Predicate::CheckImplicitDomains()
{
  Literal *lt = NULL;
  LongList errors;
  ostringstream ss;
  long p = -1;
  bool first = 1;
  char *error_template1 =
    "%s%sthe predicate `%s' in the domain statement %s. There is "
    "a cycle: ";
  char *error_template2 = " in the dependency graph. (Did you use "
    "some variables from `%s' in rules defining some of the"
    " predicates of the cycle?)";
  long pos = 0;
  
  implicit_domain_list->ClearIterator();
  while ((lt = implicit_domain_list->Iterate())) {
    if (!lt->DomainLiteral()) {
      errors.Clear();

      dependency_graph->ExtractCycle(lt->pred, &errors);

      pos += output(ss, error_template1,
		     error_file_and_line(lt->lineno),
		     ": ",
		     lt->GetPrintString(),
		     "is not a domain predicate");

      while ((p = errors.Iterate())) {
        p--;
        if (!first) {
          pos += output(ss, " -> ");
	} else {
	  first = 0;
	}
        pos += output(ss, "%s/%d", predicates[p]->Name(), 
                      predicates[p]->Arity());
      }
      pos += output(ss, error_template2, lt->GetPrintString());
      error(USR_ERR, ss.str().c_str());
    } else if ((predicates[lt->pred]->num_components > 1) &&
	       enabled_warning(WARN_DOMAIN)) {
      pos = 0;
      errors.Clear();
      dependency_graph->ExtractCycle(lt->pred, &errors);
      pos = output(ss, error_template1, "", "",
		    lt->GetPrintString(),
		    " depends on recursively defined predicates");
      while ((p = errors.Iterate())) {
	if (!first) {
	  pos += output(ss, " -> ");
	} else {
	  first = 0;
	}
	p--;
	pos += output(ss, "%s/%d", predicates[p]->Name(),
		       predicates[p]->Arity());
      }
      pos += output(ss, error_template2,
		     lt->GetPrintString());
      warn(WARN_DOMAIN, lt->lineno, ss.str().c_str());
    }
  }
}

int Predicate::Internal()
{
  return is_internal;
}


int Predicate::HasSpecialRules(StronglyConnectedComponent *scc)
{
  long l;
  bool res = 0;
  while ((l = scc->nodes.Iterate())) {
    l--;
    if (predicates[l]->HasSpecial())
      res = 1;
  }
  return res;
}

void Predicate::CalculateSCCDomain(StronglyConnectedComponent *scc)
{
  long l;
  Rule *rl = NULL;

  scc->is_domain = 1;
  scc->nodes.ClearIterator();
  if ((scc->nodes.Size() == 1) && !scc->positive_edge) {
    l = scc->nodes.Iterate();
    l--;
    if (predicates[l]->CheckRestrict(true) != RT_STRONG) {
      return;
    }
    predicates[l]->CalculateDomain();
    return;
  }
  Predicate *p;
  transitive_program = new lp_smodels;
  while ((l = scc->nodes.Iterate())) {
    l--;
    p = predicates[l];

    //    predicates[l]->CheckRestrict(1);
    if (predicates[l]->CheckRestrict(true) != RT_STRONG) {
      return;
    }
    
    p->rules.ClearIterator();
    while ((rl = p->rules.Iterate())) {
      rl->GroundRule(GRD_TRANSITIVE_DOMAIN);
    }
    p->AddSCCFacts();
  }

  while ((l = scc->nodes.Iterate())) {
    // don't change status of internal range predicates
    l--;
    p = predicates[l];
    if (p->Status() != DM_INTERNAL)
      p->SetStatus(DM_DOMAIN);
    else if (p->is_internal) {
      p->status = DM_INTERNAL;
    }
  }
  
  transitive_program->Model();
  transitive_program->CreateInstances();
  
  delete transitive_program;
}

void Predicate::SetDomainFalse(StronglyConnectedComponent *scc)
{
  long l = 0;
  scc->is_domain = 0;
  while ((l = scc->nodes.Iterate())) {
    l--;
    predicates[l]->SetStatus(DM_FALSE);
  }
}
 
void Predicate::PrintDomainPredicates()
{
  long i, j;
  Instance *it;

  for (i = 0; i < predicate_table->Size(); i++) {
    if (predicates[i]->Status() == DM_DOMAIN) {
      output(cout, "%ld %s: ", i, predicates[i]->Name());
      if (predicates[i]->atoms) {
        predicates[i]->it->Clear();
        
        while ((it = predicates[i]->it->Iterate())) {
          output(cout, "(%ld", it[0]);
          for (j = 1; j < predicates[i]->Arity(); j++) {
            output(cout, ",%ld", it[j]);
          }
          output(cout, ") ");
        }
        output(cout, "\n");
      } else {
        if (predicates[i]->follows) {
          output(cout, "T\n");
        } else {
          output(cout, "F\n");
        }
      }
    }
  }
}

void Predicate::PrintDomainRules()
{
  long i;
  Rule *r;
  
  for (i = 0; i < predicate_table->Size(); i++) {
    if (predicates[i]->Status() == DM_DOMAIN) {
      while ((r = predicates[i]->rules.Iterate())) {
        output(cout, "%ld: ", i);
        r->PrintRule();
      }
    }
  }
}

void Predicate::DefineSignature(long *sig)
{
  //   if (signature) {
  // 	// if not same, user error
  // 	for (long i = 0 ; i < Arity(); i++) {
  // 	  if (sig[i] != signature[i]) {
  // 		user_error("Two conflicting definitions for predicate %s", Name());
  // 	  }
  // 	}
  // 	return;
  //   }
  //   signature = new long[Arity()];
  //   for (long i =0 ; i < Arity(); i++) {
  // 	signature[i] = sig[i];
  //   }
  //   return;
  
}

