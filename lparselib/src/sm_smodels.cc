// Copyright 1998 by Patrik Simons
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston,
// MA 02111-1307, USA.
//
// Patrik.Simons@hut.fi
#include <string.h>
#include <float.h>
#include <assert.h>
// #include <iostream.h>
#include <limits.h>

#include "sm_smodels.h"

Api::list::list ()
{
  top = 0;
  size = 32;
  atoms = new Atom *[size];
  weights = new SM_Weight[size];
}

Api::list::~list ()
{
  delete[] atoms;
  delete[] weights;
}

void
Api::list::push (Atom *a, SM_Weight w)
{
  if (top == size)
    grow ();
  atoms[top] = a;
  weights[top] = w;
  top++;
}

void
Api::list::reset ()
{
  top = 0;
}

void
Api::list::grow ()
{
  long sz = size*2;
  Atom **atom_array = new Atom *[sz];
  SM_Weight *weight_array = new SM_Weight[sz];
  for (int i = 0; i < size; i++)
    {
      atom_array[i] = atoms[i];
      weight_array[i] = weights[i];
    }
  size = sz;
  delete[] atoms;
  atoms = atom_array;
  delete[] weights;
  weights = weight_array;
}

Api::Api (Program *p)
  : program (p)
{
  tree = 0;
  pointer_to_tree = 0;
  init = new Init;
}

Api::~Api ()
{
  delete init;
  delete pointer_to_tree;
}

inline long
Api::size (list &l)
{
  return l.top;
}

Atom *
Api::new_atom ()
{
  Atom *a = new Atom (program);
  program->atoms.push (a);
  program->number_of_atoms++;
  return a;
}

void
Api::set_compute (Atom *a, bool pos)
{
  assert (a);
  if (pos)
    a->computeTrue = true;
  else
    a->computeFalse = true;
}

void
Api::reset_compute (Atom *a, bool pos)
{
  assert (a);
  if (pos)
    a->computeTrue = false;
  else
    a->computeFalse = false;
}

void
Api::set_name (Atom *a, const char *s)
{
  assert (a);
  if (a->name && tree)
    tree->remove (a);
  delete[] a->name;
  if (s)
    {
      a->name = strcpy (new char[strlen (s)+1], s);
      if (tree)
	tree->insert (a);
    }
  else
    a->name = 0;
}

void
Api::remember ()
{
  if (pointer_to_tree == 0)
    pointer_to_tree = new Tree;
  tree = pointer_to_tree;
}

void
Api::forget ()
{
  tree = 0;
}

Atom *
Api::get_atom (const char *name)
{
  if (pointer_to_tree)
    return pointer_to_tree->find (name);
  else
    return 0;
}

void
Api::begin_rule (SM_RuleType t)
{
  type = t;
  atleast_weight = WEIGHT_MIN;
  atleast_body = 0;
  atleast_head = 1;
}

void
Api::set_init ()
{
  init->head = head.atoms;
  init->hsize = size (head);
  init->pbody = pbody.atoms;
  init->psize = size (pbody);
  init->pweight = pbody.weights;
  init->nbody = nbody.atoms;
  init->nweight = nbody.weights;
  init->nsize = size (nbody);
  init->atleast_weight = atleast_weight;
  init->atleast_body = atleast_body;
  init->atleast_head = atleast_head;
  init->maximize = maximize;
}

void
Api::end_rule ()
{
  set_init ();
  SM_Rule *r = 0;
  r = new SM_Rule;
  
  if (r)
    {
      program->rules.push (r);
      program->number_of_rules++;
      r->init (init);
    }
  pbody.reset ();
  nbody.reset ();
  head.reset ();
}

void
Api::add_head (Atom *a)
{
  assert (a);
  head.push (a);
}

void
Api::add_body (Atom *a)
{
  bool pos = true;
  assert (a);
  if (pos)
    pbody.push (a);
  else
    nbody.push (a);
}


void
Api::change_body (long i, bool pos, SM_Weight w)
{
#ifdef USEDOUBLE
  assert (w >= 0);
#endif
  if (pos)
    {
      assert (0 <= i && i < size (pbody));
      pbody.weights[i] = w;
    }
  else
    {
      assert (0 <= i && i < size (nbody));
      nbody.weights[i] = w;
    }
}

void
Api::set_atleast_weight (SM_Weight w)
{
  atleast_weight = w;
}

void
Api::set_atleast_body (long n)
{
  atleast_body = n;
}

void
Api::set_atleast_head (long n)
{
  atleast_head = n;
}


void
Api::done ()
{
  // Set up atoms
  for (Node *n = program->atoms.head (); n; n = n->next)
    {
      Atom *a = n->atom;
      a->source = 0;     // Not null if copied
      if (a->headof + a->posScore + a->negScore)
	{
	  a->head = new Follows[a->headof + a->posScore + a->negScore];
	  a->endHead = a->head + a->headof;
	  a->pos = a->endHead;
	  a->endPos = a->pos + a->posScore;
	  a->endUpper = a->endPos;
	  a->neg = a->endPos;
	  a->endNeg = a->neg + a->negScore;
	  a->end = a->endNeg;
	}
    }
  
  for (Node *n = program->rules.head (); n; n = n->next)
    n->rule->setup ();
  for (Node *n = program->atoms.head (); n; n = n->next)
    n->atom->head -= n->atom->headof;
}


static Auxiliary pos_aux(true);

Atom::Atom (Program *p0)
  : p(p0)
{
  endHead = 0;
  endPos = 0;
  endNeg = 0;
  endUpper = 0;
  end = 0;
  headof = 0;
  head = 0;
  pos = 0;
  neg = 0;
  source = 0;
  posScore = 0;
  negScore = 0;
  name = 0;
  closure = false;
  Bpos = false;
  Bneg = false;
  visited = false;
  guess = false;
  isnant = false;
  dependsonTrue = false;
  dependsonFalse = false;
  computeTrue = false;
  computeFalse = false;
  backtracked = false;
  forced = false;
  in_etrue_queue = false;
  in_efalse_queue = false;
  in_queue = false;
  pos_tested = false;
  neg_tested = false;
}

Atom::~Atom ()
{
  delete[] head;
  delete[] name;
}

void
Atom::etrue_push ()
{
  if (!(in_etrue_queue | in_efalse_queue))
    p->equeue.push (this);
  if (Bneg | in_efalse_queue)
    p->conflict = true;
  in_etrue_queue = true;
}

void
Atom::efalse_push ()
{
  if (!(in_etrue_queue | in_efalse_queue))
    p->equeue.push (this);
  if (Bpos | in_etrue_queue)
    p->conflict = true;
  in_efalse_queue = true;
}

void
Atom::queue_push ()
{
  if (!in_queue)
    p->queue.push (this);
  in_queue = true;
}

void 
Atom::backchainTrue ()
{
  // Find the only active rule.
  for (Follows *f = head; f != endHead; f++)
    if (!f->r->isInactive ())
      {
	f->r->backChainTrue ();
	break;
      }
}

void 
Atom::backchainFalse ()
{
  for (Follows *f = head; f != endHead; f++)
    f->r->backChainFalse ();
}

//
// Note that since backchain depends on the truth values of the
// atoms in the body of the rules, we must set Bpos first.
// To keep everything safe we also handle inactivation before 
// we fire any rules.
//
void
Atom::setBTrue ()
{
  Bpos = true;
  Follows *f;
  for (f = neg; f != endNeg; f++)
    f->r->inactivate (f);
  for (f = pos; f != endPos; f++)
    f->r->mightFire (f);
  // This atom could have been forced.
  if (headof == 1)
    backchainTrue ();
}
 
void
Atom::setBFalse ()
{
  Bneg = true;
  Follows *f;
  for (f = pos; f != endPos; f++)
    f->r->inactivate (f);
  for (f = neg; f != endNeg; f++)
    f->r->mightFire (f);
  closure = false;
  source = 0;
  // This atom could have been forced.
  if (headof)          // There are active rules.
    backchainFalse (); // Might backchain already backchained rules
		       // in mightFire.
}

void
Atom::setTrue ()
{
  for (Follows *f = pos; f != endPos; f++)
    f->r->mightFireInit (f);
  closure = true;
}

void
Atom::reduce_head ()
{
  Follows t;
  Follows *g = head;
  for (Follows *f = head; f != endHead; f++)
    {
      f->r->swapping (f, g);
      g->r->swapping (g, f);
      t = *g;
      *g = *f;
      *f = t;
      if (!g->r->isInactive ())
	g++;
    }
  endHead = g;
}

void
Atom::reduce_pbody (bool strong)
{
  Follows t;
  Follows *g = pos;
  for (Follows *f = pos; f != endPos; f++)
    {
      f->r->swapping (f, g);
      g->r->swapping (g, f);
      t = *g;
      *g = *f;
      *f = t;
      if ((strong && Bpos) || g->r->isInactive ())
	g->r->not_in_loop (g);
      else if (Bneg == false)
	g++;
    }
  endPos = g;
  endUpper = g;
}

void
Atom::reduce_nbody (bool)
{
  Follows t;
  Follows *g = neg;
  for (Follows *f = neg; f != endNeg; f++)
    {
      f->r->swapping (f, g);
      g->r->swapping (g, f);
      t = *g;
      *g = *f;
      *f = t;
      if (Bpos == false && Bneg == false && !g->r->isInactive ())
	g++;
    }
  endNeg = g;
}

void
Atom::reduce (bool strongly)
{
  reduce_head ();
  reduce_pbody (strongly);
  reduce_nbody (strongly);
}

void
Atom::unreduce ()
{
  endHead = pos;
  endPos = neg;
  endUpper = neg;
  endNeg = end;
  for (Follows *f = pos; f != endPos; f++)
    f->r->in_loop (f);
}

const char *
Atom::atom_name ()
{
  if (name)
    return name;
  else
    return "#noname#";
}

void
Atom::visit ()
{
  queue_push ();
  visited = true;
}


SM_Rule::SM_Rule ()
{
  head = 0;
  lit = 0;
  upper = 0;
  nbody = 0;
  pbody = 0;
  nend = 0;
  pend = 0;
  end = 0;
  inactive = 0;
  visited = false;
  type = basicrule;
}

SM_Rule::~SM_Rule ()
{
  delete[] nbody;  // pbody is allocated after nbody
}

bool
SM_Rule::isInactive ()
{
  return inactive;
}

bool
SM_Rule::isUpperActive ()
{
  return upper == 0 && !inactive;
}

bool
SM_Rule::isFired ()
{
  return lit == 0;
}

void
SM_Rule::inactivate (const Follows *f)
{
  if (f->aux)
    upper++;
  inactive++;
  if (inactive == 1)
    {
      Atom *b = head;
      b->headof--;
      if (b->Bneg == false)
	{
	  if (b->headof && (b->source == 0 || b->source == this))
	    b->queue_push ();
	  if (b->headof == 0)
	    b->efalse_push ();
	  else if (b->headof == 1 && b->Bpos)
	    b->backchainTrue ();
	}
    }
}

void
SM_Rule::fireInit ()
{
  if (lit == 0)
    {
      head->etrue_push ();
      head->queue_push ();
      head->source = this;
    }
  else if (upper == 0)
    {
      head->queue_push ();
      if (head->source == 0)
	head->source = this;
    }
}

void
SM_Rule::mightFireInit (const Follows *)
{
  upper--;
  if (upper == 0 && !inactive)
    {
      head->queue_push ();
      if (head->source == 0)
	head->source = this;
    }
}

void
SM_Rule::unInit ()
{
  upper = pend-pbody;
  lit = end-nbody;
  inactive = 0;
}

void
SM_Rule::mightFire (const Follows *)
{
  lit--;
  if (lit == 0 && head->Bpos == false)
    head->etrue_push ();
  else if (lit == 1 && !inactive && head->Bneg == true)
    backChainFalse ();
}

void
SM_Rule::backChainTrue ()
{
  if (lit)
    {
      for (Atom **b = nbody; b != nend; b++)
	if ((*b)->Bneg == false)
	  (*b)->efalse_push ();
      for (Atom **b = pbody; b != pend; b++)
	if ((*b)->Bpos == false)
	  (*b)->etrue_push ();
    }
}

void
SM_Rule::backChainFalse ()
{
  if (lit == 1 && !inactive)
    {
      for (Atom **b = nbody; b != nend; b++)
	if ((*b)->Bneg == false)
	  {
	    (*b)->etrue_push ();
	    return;
	  }
      for (Atom **b = pbody; b != pend; b++)
	if ((*b)->Bpos == false)
	  {
	    (*b)->efalse_push ();
	    return;
	  }
    }
}

void
SM_Rule::backtrackFromActive (const Follows *)
{
  lit++;
}

void
SM_Rule::backtrackFromInactive (const Follows *f)
{
  if (f->aux)
    upper--;
  inactive--;
  if (inactive == 0)
    head->headof++;
}

void
SM_Rule::propagateFalse (const Follows *)
{
  upper++;
  if (upper == 1 && !inactive && head->closure != false &&
      (head->source == 0 || head->source == this))
    head->queue_push ();
}

void
SM_Rule::propagateTrue (const Follows *)
{
  upper--;
  if (upper == 0 && !inactive && head->closure == false)
    {
      if (head->source == 0)
	head->source = this;
      head->queue_push ();
    }
}

void
SM_Rule::backtrackUpper (const Follows *)
{
  upper--;
}

void
SM_Rule::search (Atom *)
{
  if (!head->visited && head->Bneg == false)
    head->visit ();
  for (Atom **a = nbody; a != nend; a++)
    if (!(*a)->visited && (*a)->Bneg == false)
      (*a)->visit ();
  for (Atom **a = pbody; a != pend; a++)
    if (!(*a)->visited && (*a)->Bneg == false)
      (*a)->visit ();
  visited = true;
}

void
SM_Rule::reduce (bool strongly)
{
  Atom **a;
  Atom **b;
  Atom *t;
  b = nbody;
  for (a = nbody; a != nend; a++)
    {
      t = *b;
      *b = *a;
      *a = t;
      if ((*b)->Bpos == false && (*b)->Bneg == false)
	b++;
    }
  nend = b;
  b = pbody;
  for (a = pbody; a != pend; a++)
    {
      t = *b;
      *b = *a;
      *a = t;
      if ((strongly == false || (*b)->Bpos == false)
	  && (*b)->Bneg == false)
	b++;
    }
  pend = b;
}

void
SM_Rule::unreduce ()
{
  nend = pbody;
  pend = end;
}

void
SM_Rule::setup ()
{
  head->head->r = this;
  head->head->aux = 0;
  head->head++;
  Atom **a;
  for (a = nbody; a != nend; a++)
    {
      (*a)->negScore--;
      (*a)->neg[(*a)->negScore].r = this;
      (*a)->neg[(*a)->negScore].aux = 0;
    }
  for (a = pbody; a != pend; a++)
    {
      (*a)->posScore--;
      (*a)->pos[(*a)->posScore].r = this;
      (*a)->pos[(*a)->posScore].aux = &pos_aux;
    }
}

void
SM_Rule::init (Init *init)
{
  head = *init->head;
  head->headof++;
  long n = init->psize+init->nsize;
  if (n != 0)
    nbody = new Atom *[n];
  else
    nbody = 0;
  end = nbody+n;
  pend = end;
  lit = n;
  for (long i = 0; i < init->nsize; i++)
    {
      nbody[i] = init->nbody[i];
      nbody[i]->negScore++;
      nbody[i]->isnant = true;
    }
  nend = nbody + init->nsize;
  pbody = nend;
  upper = init->psize;
  for (long i = 0; i < init->psize; i++)
    {
      pbody[i] = init->pbody[i];
      pbody[i]->posScore++;
    }
}

void
SM_Rule::print ()
{
//   cout << head->atom_name ();
//   if (nbody)
//     cout << " :- ";
//   Atom **a;
//   int comma = 0;
//   for (a = pbody; a != pend; a++)
//     {
//       if (comma)
// 	cout << ", ";
//       cout << (*a)->atom_name ();
//       comma = 1;
//     }
//   for (a = nbody; a != nend; a++)
//     {
//       if (comma)
// 	cout << ", ";
//       cout << "not " << (*a)->atom_name ();
//       comma = 1;
//     }
//   cout << '.' << endl;
}

void
SM_Rule::print_internal ()
{
//   cout << (int)type << ' ' << head->posScore << ' '
//        << (nend-nbody)+(pend-pbody) << ' ' << nend-nbody;
//   head->negScore = 1;
//   Atom **a;
//   for (a = nbody; a != nend; a++)
//     {
//       cout << ' ' << (*a)->posScore;
//       (*a)->negScore = 1;  // Mark as needed in compute statement
//     }
//   for (a = pbody; a != pend; a++)
//     {
//       cout << ' ' << (*a)->posScore;
//       (*a)->negScore = 1;
//     }
//   cout << endl;
}

Dcl::Dcl (Smodels *s)
  : smodels (s), program (&s->program)
{
  tmpfalse = 0;
  tmpfalsetop = 0;
}

Dcl::~Dcl ()
{
  delete[] tmpfalse;
}

void
Dcl::init ()
{
  tmpfalse = new Atom *[program->number_of_atoms];
  tmpfalsetop = 0;
}

void
Dcl::setup ()
{
  for (Node *n = program->rules.head (); n; n = n->next)
    n->rule->fireInit ();
  // Find atoms that are true.
  while (!program->queue.empty ())
    {
      Atom *a = program->queue.pop ();
      if (a->closure == false)
	a->setTrue ();
      a->in_queue = false;
    }
  // Set not true atoms to explicitly false.
  for (Node *n = program->atoms.head (); n; n = n->next)
    {
      Atom *a = n->atom;
      if (a->closure == false)
	{
	  if (a->Bpos)
	    {
	      //	      smodels->set_conflict ();
	      return;
	    }
	  else
	    {
	      smodels->setToBFalse (a);
	      // SM_Rule::inactivate () changes upper
	      // and it shouldn't be changed here.
	      for (Follows *f = a->pos; f != a->endPos; f++)
		f->r->backtrackUpper (f);
	    }
	}
    }
}

void
Dcl::revert ()
{
  for (Node *n = program->rules.head (); n; n = n->next)
    n->rule->unInit ();
  for (Node *n = program->atoms.head (); n; n = n->next)
    {
      Atom *a = n->atom;
      a->source = 0;
      a->headof = a->endHead - a->head;
      a->closure = false;
      a->Bpos = false;
      a->Bneg = false;
      a->visited = false;
      a->guess = false;
      a->backtracked = false;
      a->forced = false;
      a->in_etrue_queue = false;
      a->in_efalse_queue = false;
      a->in_queue = false;
    }
}

void
Dcl::dcl ()
{
  Atom *a;
  for (;;)
    {
      // If an atom is explicitly set after it's on the queue then
      // this test misses the conflict
      if (program->conflict)
	break;
      // Propagate explicit truth. (Fitting Semantics)
      if (!program->equeue.empty ())
	{
	  a = program->equeue.pop ();
	  if ((a->Bneg | a->in_efalse_queue)
	      && (a->Bpos | a->in_etrue_queue))
	    {
	      a->in_etrue_queue = false;
	      a->in_efalse_queue = false;
	      break;
	    }
	  else if (a->in_efalse_queue)
	    {
	      if (a->Bneg == false)
		smodels->setToBFalse (a);
	    }
	  else if (a->Bpos == false)
	    smodels->setToBTrue (a);
	  a->in_etrue_queue = false;
	  a->in_efalse_queue = false;
	}
      else if (program->queue.empty ())
	return;
      // Find conflicts in order to guarantee soundness of smodels
      // This part prevents dcl from being incrementally linear.
      else if (propagateFalse ())
	break;
    }
  program->conflict = false;
  //  smodels->set_conflict ();
}

bool
Dcl::propagateFalse ()
{
  Atom *a;
  Follows *f;
  long i;
  tmpfalsetop = 0;
  // Propagate falsehood.
  while (!program->queue.empty ())
    {
      a = program->queue.pop ();
      a->source = 0;
      // If a->closure is false, then the descendants of a are
      // in the queue.
      if (a->closure == true && a->Bneg == false)
	{
	  for (f = a->pos; f != a->endUpper; f++)
	    f->r->propagateFalse (f);
	  a->closure = false;
	  tmpfalse[tmpfalsetop] = a;
	  tmpfalsetop++;
	}
      a->in_queue = false;
    }
  // Some atoms now false should be true.
  for (i = 0; i < tmpfalsetop; i++)
    if (tmpfalse[i]->headof)
      for (f = tmpfalse[i]->head; f != tmpfalse[i]->endHead; f++)
	if (f->r->isUpperActive ())
	  {
	    tmpfalse[i]->source = f->r;
	    tmpfalse[i]->queue_push ();
	    break;
	  }
  // Propagate truth to find them all.
  while (!program->queue.empty ())
    {
      a = program->queue.pop ();
      if (a->closure == false && a->Bneg == false)
	{
	  for (f = a->pos; f != a->endUpper; f++)
	    f->r->propagateTrue (f);
	  a->closure = true;
	}
      a->in_queue = false;
    }
  // Those atoms now false can be set to explicitly false.
  for (i = 0; i < tmpfalsetop; i++)
    {
      a = tmpfalse[i];
      if (a->closure == false)
	if (a->Bpos)
	  {
	    // Conflict found, we reset the f->upper variables that are
	    // not reset while backtracking.
	    for (; i < tmpfalsetop; i++)
	      {
		a = tmpfalse[i];
		if (a->closure == false)
		  {
		    for (f = a->pos; f != a->endUpper; f++)
		      f->r->backtrackUpper (f);
		    a->closure = true;
		  }
	      }
	    return true;
	  }
	else
	  {
	    smodels->setToBFalse (a);
	    // SM_Rule::inactivate () changes upper and it shouldn't be
	    // changed for the atoms below endUpper.
	    for (f = a->pos; f != a->endUpper; f++)
	      f->r->backtrackUpper (f);
	  }
    }
  return false;
}

int
Dcl::path (Atom *a, Atom *b)
{
  if (a == b)
    return 1;
  Follows *f;
  Atom *c;
  for (Node *n = program->rules.head (); n; n = n->next)
    n->rule->visited = false;
  for (Node *n = program->atoms.head (); n; n = n->next)
    n->atom->visited = false;
  a->queue_push ();
  a->visited = true;
  while (!program->queue.empty ())
    {
      c = program->queue.pop ();
      c->in_queue = false;
      if (c == b)
	{
	  while (!program->queue.empty ())
	    {
	      c = program->queue.pop ();
	      c->in_queue = false;
	    }
	  return 1;
	}
      for (f = c->head; f != c->endHead; f++)
	if (!f->r->visited && !f->r->isInactive ())
	  f->r->search (c);
      for (f = c->pos; f != c->endPos; f++)
	if (!f->r->visited && !f->r->isInactive ())
	  f->r->search (c);
      for (f = c->neg; f != c->endNeg; f++)
	if (!f->r->visited && !f->r->isInactive ())
	  f->r->search (c);
    }
  return 0;
}

void
Dcl::reduce (bool strongly)
{
  for (Node *n = program->atoms.head (); n; n = n->next)
    {
      Atom *a = n->atom;
      a->reduce (strongly);
    }
  for (Node *n = program->rules.head (); n; n = n->next)
    {
      if (n->rule->isInactive ())
	continue;   // Nothing points at this rule now
      n->rule->reduce (strongly);
    }
}
 
void
Dcl::unreduce ()
{
  for (Node *n = program->atoms.head (); n; n = n->next)
    n->atom->unreduce ();
  for (Node *n = program->rules.head (); n; n = n->next)
    n->rule->unreduce ();
}

void
Dcl::print ()
{
//   cout << "bdcl:" << endl << "True:";
//   for (Node *n = program->atoms.head (); n; n = n->next)
//     if (n->atom->closure == true)
//       cout << ' ' << n->atom->atom_name ();
//   cout << endl << "BTrue:";
//   for (Node *n = program->atoms.head (); n; n = n->next)
//     if (n->atom->Bpos)
//       cout << ' ' << n->atom->atom_name ();
//   cout << endl << "BFalse:";
//   for (Node *n = program->atoms.head (); n; n = n->next)
//     if (n->atom->Bneg)
//       cout << ' ' << n->atom->atom_name ();
//   cout << endl;
}

#define lowlink posScore
#define dfnumber negScore
#define marked dependsonTrue
#define realnant dependsonFalse


class Denant
{
public:
  Denant (long size);
  ~Denant () {};
  void strong (Atom *);
  void do_strong (Atom *a, Atom *b, bool negatively);
  void do_strong (Atom *a, SM_Rule *r, bool negatively);

  long count;
  long size_of_searchspace;
  long number_of_denants;
  Stack stack;
};

Denant::Denant (long size)
{
  stack.Init (size);
  count = 0;
  size_of_searchspace = 0;
  number_of_denants = 0;
}

inline void
Denant::do_strong (Atom *a, Atom *b, bool negatively)
{
  if (!b->visited)
    {
      strong (b);
      if (a->lowlink >= b->lowlink)
	{
	  a->lowlink = b->lowlink;
	  a->realnant |= negatively;
	}
    }
  else if (!b->marked && a->lowlink >= b->dfnumber)
    {
      a->lowlink = b->dfnumber;
      a->realnant |= negatively;
    }
}

void
Denant::do_strong (Atom *a, SM_Rule *r, bool negatively)
{
  if (r->isInactive ())
    return;

  Atom *b = r->head;
  do_strong (a, b, negatively);

}

void
Denant::strong (Atom *a)
{
  a->visited = true;
  a->dfnumber = count;
  a->lowlink = count;
  count++;
  if (a->Bneg || a->Bpos)
    {
      a->marked = true;
      return;
    }
  stack.push (a);
  for (Follows *f = a->pos; f != a->endPos; f++)
    do_strong (a, f->r, false);
  for (Follows *f = a->neg; f != a->endNeg; f++)
    do_strong (a, f->r, true);
  for (Follows *f = a->head; !a->realnant && f != a->endHead; f++)
    if ((f->r->type == choicerule || f->r->type == generaterule) &&
	!f->r->isInactive ())
      a->realnant = true;
  if (a->lowlink == a->dfnumber)
    {
      Atom *b;
      do {
	b = stack.pop ();
	b->marked = true;
	if (b->isnant)
	  {
	    if (b->realnant == false)
	      {
		b->isnant = false;
		number_of_denants++;
	      }
	    else
	      size_of_searchspace++;
	  }
      } while (a != b);
    }
}

// Only the atoms that appear negatively in a loop can influence the
// stable models.
void
Dcl::denant ()
{
  for (Node *n = program->atoms.head (); n; n = n->next)
    {
      n->atom->visited = false;
      n->atom->marked = false;
      n->atom->realnant = false;
    }
  Denant i(program->number_of_atoms);
  for (Node *n = program->atoms.head (); n; n = n->next)
    if (!n->atom->visited)
      i.strong (n->atom);
}

void
Dcl::undenant ()
{
  for (Node *n = program->atoms.head (); n; n = n->next)
    {
      Atom *a = n->atom;
      for (Follows *f = a->neg; !a->isnant && f != a->endNeg; f++)
	if (f->r->type != optimizerule)
	  a->isnant = true;
      for (Follows *f = a->head; !a->isnant && f != a->endHead; f++)
	if (f->r->type == choicerule || f->r->type == generaterule)
	  a->isnant = true;
    }
}

#define lowlink posScore
#define dfnumber negScore
#define marked dependsonTrue

class Improve
{
public:
  Improve (long size);
  ~Improve () {};
  void strong (Atom *);
  void removeFrom (Atom *a, Follows *f);
  void doStrong (Atom *a, Atom *b);

  long count;
  Stack stack;
};

Improve::Improve (long size)
{
  stack.Init (size);
  count = 0;
}

inline void
Improve::removeFrom (Atom *a, Follows *f)
{
  f->r->not_in_loop (f);
  a->endUpper--;
  Follows *g = a->endUpper;
  f->r->swapping (f, g);
  g->r->swapping (g, f);
  Follows t = *f;
  *f = *g;
  *g = t;
}

inline void
Improve::doStrong (Atom *a, Atom *b)
{
  if (!b->visited)
    {
      strong (b);
      if (a->lowlink > b->lowlink)
	a->lowlink = b->lowlink;
    }
  else if (!b->marked && a->lowlink > b->dfnumber)
    a->lowlink = b->dfnumber;
}

void
Improve::strong (Atom *a)
{
  Atom *b;
  a->visited = true;
  a->dfnumber = count;
  a->lowlink = count;
  count++;
  stack.push (a);
  for (Follows *f = a->pos; f != a->endUpper; f++)
    {
      bool remove = false;
      if (a->Bneg || f->r->isInactive ())
	continue;
      b = f->r->head;
      doStrong (a, b);
      remove = b->marked;
      if (remove)
	{
	  removeFrom (a, f);
	  f--;
	}
    }
  if (a->lowlink == a->dfnumber)
    do {
      b = stack.pop ();
      b->marked = true;
    } while (a != b);
}

void
Dcl::improve ()
{
  for (Node *n = program->atoms.head (); n; n = n->next)
    {
      n->atom->visited = false;
      n->atom->marked = false;
    }
  Improve i(program->number_of_atoms);
  for (Node *n = program->atoms.head (); n; n = n->next)
    if (!n->atom->visited)
      i.strong(n->atom);
}

SM_List::SM_List ()
{
  tail = &list;
}

AtomSM_List::~AtomSM_List ()
{
  Node *n = head ();
  Node *t;
  while (n)
    {
      t = n->next;
      delete n->atom;
      delete n;
      n = t;
    }
}

SM_RuleSM_List::~SM_RuleSM_List ()
{
  Node *n = head ();
  Node *t;
  while (n)
    {
      t = n->next;
      delete n->rule;
      delete n;
      n = t;
    }
}

Program::Program ()
{
  number_of_atoms = 0;
  number_of_rules = 0;
  conflict = false;
}

Program::~Program ()
{
}

void
Program::init ()
{
  equeue.Init (number_of_atoms);
  queue.Init (number_of_atoms);
}


void
Program::print ()
{
//   Node *n;
//   for (n = rules.head (); n; n = n->next)
//     {
//       if (n->rule->isInactive ())
// 	continue;
//       n->rule->print ();
//     }
//   for (n = atoms.head (); n; n = n->next)
//     if (n->atom->computeTrue || n->atom->computeFalse)
//       break;
//   if (n == 0)
//     return;
//   cout << "compute { ";
//   bool comma = false;
//   for (n = atoms.head (); n; n = n->next)
//     {
//       if (n->atom->computeTrue)
// 	{
// 	  if (comma)
// 	    cout << ", ";
// 	  cout << n->atom->atom_name ();
// 	  comma = true;
// 	}
//       if (n->atom->computeFalse)
// 	{
// 	  if (comma)
// 	    cout << ", ";
// 	  cout << "not " << n->atom->atom_name ();
// 	  comma = true;
// 	}
//     }
//   cout << " }" << endl;
}

void
Program::print_internal (long models)
{
//   Node *n;
//   long i = 1;
//   for (n = atoms.head (); n; n = n->next)
//     {
//       n->atom->posScore = i;
//       n->atom->negScore = 0;
//       i++;
//     }
//   for (n = rules.head (); n; n = n->next)
//     {
//       if (n->rule->isInactive ())
// 	continue;
//       n->rule->print_internal ();
//     }
//   cout << 0 << endl;
//   for (n = atoms.head (); n; n = n->next)
//     if (n->atom->negScore && n->atom->name)
//       cout << n->atom->posScore << ' ' << n->atom->name << endl;
//   cout << 0 << endl;
//   cout << "B+" << endl;
//   for (n = atoms.head (); n; n = n->next)
//     if (n->atom->negScore && n->atom->computeTrue)
//       cout << n->atom->posScore << endl;
//   cout << 0 << endl;
//   cout << "B-" << endl;
//   for (n = atoms.head (); n; n = n->next)
//     if (n->atom->negScore && n->atom->computeFalse)
//       cout << n->atom->posScore << endl;
//   cout << 0 << endl;
//   cout << models << endl;
}

Queue::Queue (long size)
{
  if (size == 0)
    {
      queue = 0;
      return;
    }
  queue = new Atom *[size+1];
  last = size;
  reset ();
}

Queue::~Queue ()
{
  delete[] queue;
}

void
Queue::Init (long size)
{
  delete[] queue;
  queue = new Atom *[size+1];
  last = size;
  reset ();
}

Smodels::Smodels ()
  : dcl (this)
{
  atom = 0;
  atomtop = 0;
  setup_top = 0;
}

Smodels::~Smodels ()
{
  delete[] atom;
}

void
Smodels::init ()
{
  program.init ();
  dcl.init ();
  atom = new Atom *[program.number_of_atoms];
  for (Node *n = program.atoms.head (); n; n = n->next)
    atom[atomtop++] = n->atom;
  stack.Init (program.number_of_atoms);
  depends.Init (program.number_of_atoms);
}

inline void
Smodels::reset_queues ()
{
  while (!program.equeue.empty ())
    {
      Atom *a = program.equeue.pop ();
      a->in_etrue_queue = false;
      a->in_efalse_queue = false;
    }
  while (!program.queue.empty ())
    {
      Atom *a = program.queue.pop ();
      a->in_queue = false;
    }
}

inline void
Smodels::removeAtom (long n)
{
  assert (atomtop);
  atomtop--;
  Atom *a = atom[atomtop];
  atom[atomtop] = atom[n];
  atom[n] = a;
}

inline void
Smodels::addAtom (Atom *a)
{
  while (atom[atomtop] != a)
    atomtop++;
  atomtop++;
}

void
Smodels::reset_dependency ()
{
  while (!depends.empty ())
    {
      Atom *a = depends.pop ();
      a->dependsonTrue = false;
      a->dependsonFalse = false;
      a->pos_tested = false;
      a->neg_tested = false;
      a->posScore = ULONG_MAX;
      a->negScore = ULONG_MAX;
    }
}

void
Smodels::clear_dependency ()
{
  for (Node *n = program.atoms.head (); n; n = n->next)
    {
      Atom *a = n->atom;
      a->dependsonTrue = false;
      a->dependsonFalse = false;
      a->pos_tested = false;
      a->neg_tested = false;
      a->posScore = ULONG_MAX;
      a->negScore = ULONG_MAX;
    }
  depends.reset ();
}

void
Smodels::add_dependency (Atom *a, bool pos)
{
  if (!(a->dependsonTrue | a->dependsonFalse))
    depends.push (a);
  if (pos)
    a->dependsonTrue = true;
  else
    a->dependsonFalse = true;
}


void
Smodels::setToBTrue (Atom *a)
{
  add_dependency (a, true);
  a->setBTrue ();
  stack.push (a);
}

void
Smodels::setToBFalse (Atom *a)
{
  add_dependency (a, false);
  a->setBFalse ();
  stack.push (a);
}


inline void
Smodels::expand ()
{
  dcl.dcl ();
}


void
Smodels::setup ()
{
  dcl.setup ();
  expand ();  // Compute well-founded model
  dcl.reduce (true);  // Reduce the program strongly
  // Initialize literals chosen to be in the stable model / full set.
  for (Node *n = program.atoms.head (); n; n = n->next)
    {
      Atom *a = n->atom;
      // Negative literal 
      if (a->computeFalse && a->Bneg == false)
	{
	  setToBFalse (a);
	}
      // Positive literal
      if (a->computeTrue && a->Bpos == false)
	{
	  setToBTrue (a);
	}
    }
  expand ();

  dcl.reduce (false); // Reduce weakly
  dcl.improve ();
  clear_dependency ();
  setup_top = stack.top;
}


int
Smodels::model ()
{
  setup ();
  expand();
  
  return 0;
}


void
Smodels::print ()
{
//   cout << "Body: ";
//   for (Node *n = program.atoms.head (); n; n = n->next)
//     if (n->atom->Bpos)
//       cout << n->atom->atom_name () << ' ';
//   cout << endl << "NBody: ";
//   for (Node *n = program.atoms.head (); n; n = n->next)
//     if (n->atom->Bneg)
//       cout << n->atom->atom_name () << ' ';
//   cout << endl << "Pick: ";
//   Atom **a;
//   for (a = stack.stack; a != stack.stack+stack.top; a++)
//     if ((*a)->guess)
//       if((*a)->Bneg)
// 	cout << "not " << (*a)->atom_name () << ' ';
//       else
// 	cout << (*a)->atom_name () << ' ';
//   cout << endl;
}

void
Smodels::printAnswer ()
{
//   // Prints the stable model.
//   cout << "Stable Model: ";
//   for (Node *n = program.atoms.head (); n; n = n->next)
//     if (n->atom->Bpos && n->atom->name)
//       cout << n->atom->name << ' ';
//   cout << endl;
}

Stack::Stack (long size)
{
  if (size == 0)
    {
      stack = 0;
      return;
    }
  stack = new Atom *[size];
  top = 0;
  last = size;
}

Stack::~Stack ()
{
  delete[] stack;
}

void
Stack::Init (long size)
{
  delete[] stack;
  stack = new Atom *[size];
  top = 0;
  last = size;
}

Tree::Node::Node (Atom *k, Node *l, Node *r)
{
  key = k;
  left = l;
  right = r;
}

Tree::Node::~Node ()
{
}

Tree::Tree ()
{
  root = 0;
}

Tree::~Tree ()
{
  flush (root);
}

void
Tree::flush (Node *n)
{
  if (n == 0)
    return;
  flush (n->left);
  flush (n->right);
  delete n;
}

Tree::Node *
Tree::splay (const char *key, Node *root)
{
  Node n;
  Node *l = &n;
  Node *r = &n;
  Node *t;
  for (;;)
    {
      int c = compare (key, root->key);
      if (c < 0)
	{
	  if (root->left == 0)
	    break;
	  if (compare (key, root->left->key) < 0)
	    {
	      t = root->left;
	      root->left = t->right;
	      t->right = root;
	      root = t;
	      if (root->left == 0)
		break;
	    }
	  r->left = root;
	  r = root;
	  root = root->left;
        }
      else if (c > 0)
	{
	  if (root->right == 0)
	    break;
	  if (compare (key, root->right->key) > 0)
	    {
	      t = root->right;
	      root->right = t->left;
	      t->left = root;
	      root = t;
	      if (root->right == 0)
		break;
	    }
	  l->right = root;
	  l = root;
	  root = root->right;
        }
      else
	break;
    }
  l->right = root->left;
  r->left = root->right;
  root->left = n.right;
  root->right = n.left;
  return root;
}

Atom *
Tree::find (const char *key)
{
  if (root)
    {
      root = splay (key, root);
      if (compare (key, root->key) == 0)
	return root->key;
    }
  return 0;
}

void
Tree::insert (Atom *key)
{
  if (root == 0)
    {
      root = new Node (key);
      return;
    }
  root = splay (key->name, root);
  int c = compare (key->name, root->key);
  if (c < 0)
    {
      Node *t = root;
      root = new Node (key, root->left, root);
      t->left = 0;
    }
  else if (c > 0)
    {
      Node *t = root;
      root = new Node (key, root, root->right);
      t->right = 0;
    }
}

void
Tree::remove (Atom *key)
{
  if (root == 0)
    return;
  Node *t = splay (key->name, root);
  if (compare (key->name, t->key) == 0)
    {
      if (t->left == 0)
	root = t->right;
      else
	{
	  root = splay (key->name, t->left);
	  root->right = t->right;
	}
      delete t;
    }
  else
    root = t;
}

void
Tree::check_consistency ()
{
  check_consistency (root);
}

void
Tree::check_consistency (Node *n)
{
//   if (n == 0)
//     return;
//   if (n->left && compare (n->left->key->name, n->key) > 0)
//     {
//       cerr << "error: key " << n->key << " smaller than key "
// 	   << n->left->key << endl;
//       return;
//     }
//   if (n->right && compare (n->right->key->name, n->key) < 0)
//     {
//       cerr << "error: key " << n->key << " larger than key "
// 	   << n->right->key << endl;
//       return;
//     }
//   check_consistency (n->left);
//   check_consistency (n->right);
}

int
Tree::compare (const char *a, Atom *b)
{
  return strcmp (a, b->name);
}
