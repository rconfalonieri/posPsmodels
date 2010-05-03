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

#include "smodels.h"
#include "instance.h"

#include "predicate.h"
#include "sm_smodels.h"

lp_smodels::lp_smodels()
{
  sm = new Smodels;
  api = new Api(&sm->program);

  api->remember();
}


lp_smodels::~lp_smodels()
{
  //  printf("FOO\n");
  delete api;
  delete sm;
}

void lp_smodels::AddFact(long pred, Instance *it)
{
  const char *st = get_ground_instance(it, pred, predicates[pred]->arity);
  Atom *a = 0;

  if (predicates[pred]->hidden && !sys_data.emit_text)
    st += 3;
  
  a = api->get_atom(st);
  if (!a) {
    a = api->new_atom();
    api->set_name(a, st);
  }

  api->begin_rule(basicrule);
  api->add_head(a);
  api->end_rule();
  
}

void lp_smodels::AddRuleInstance(Rule *rl)
{
  Literal *lt = 0;
  const char *st = rl->head->GetInstanceString();
  
  if (predicates[rl->head->pred]->hidden && !sys_data.emit_text)
    st += 3;
  
  Atom *h = api->get_atom(st);

  if (!h) {
    h = api->new_atom();

    api->set_name(h, st);
  }
  Atom *a = 0;
  
  api->begin_rule(basicrule);

  api->add_head(h);

  rl->positive.ClearIterator();
  while ((lt = rl->positive.Iterate())) {
    if (!lt->DomainLiteral() && (!lt->has_global_condition ||
				 lt->ignore_global_condition)) { 
      st = lt->GetInstanceString();
      
      if (predicates[lt->pred]->hidden && !sys_data.emit_text)
	st += 3;

      a = api->get_atom(st);
      if (!a) {
	a = api->new_atom();
	api->set_name(a, st);
      }
      api->add_body(a);
    }
  }
  while ((lt = rl->positive_conditions.Iterate())) {
    if (!lt->DomainLiteral() && (!lt->has_global_condition ||
				 lt->ignore_global_condition)) { 
      st = lt->GetInstanceString();
      
      if (predicates[lt->pred]->hidden && !sys_data.emit_text)
	st += 3;

      a = api->get_atom(st);
      if (!a) {
	a = api->new_atom();
	api->set_name(a, st);
      }
      api->add_body(a);
    }
  }
    
  api->end_rule();
  
}


int lp_smodels::Model()
{
  api->done();

  //  printf("--------------\n");
  //  sm->program.print();
  //  printf("--------------\n");
  sm->init();

  if (sm->model()) {
    //    sm->printAnswer();
    return 1;
  } else {
    return 0;
  }
  return 0;
  
}

void lp_smodels::CreateInstances()
{
  static Node *pos = 0;
  static Instance *it = 0;
  long pred;
  if (!it) {
    it = new Instance[sys_data.term_max_arity+1];
  }
  
  if (!pos) {
    pos = sm->program.atoms.head();
  } else {
    pos = pos->next;
  }

  while (pos) {
    if (pos && pos->atom->Bpos) {
      pred = Predicate::decode_atom(pos->atom->name, it);
      predicates[pred]->AddInstance(it);
    } 
    pos = pos->next;
  }
  return;
}



