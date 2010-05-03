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
#include <iostream>
#include <unistd.h>
#include <limits.h>
#include <float.h>
#include <stdlib.h>
#include "print.h"
#include "atomrule.h"
#include "smodels.h"

using namespace std;

Smodels::Smodels ()
  : dcl (this)
{
  atom = 0;
  atomtop = 0;
  fail = false;
  guesses = 0;
  conflict_found = false;
  answer_number = 0;
  number_of_choice_points = 0;
  number_of_wrong_choices = 0;
  number_of_backjumps = 0;
  number_of_picked_atoms = 0;
  number_of_forced_atoms = 0;
  number_of_assignments = 0;
  size_of_searchspace = 0;
  number_of_denants = 0;
  max_models = 0;
  setup_top = 0;
  score = 0;
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
  lasti = 0;
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
Smodels::set_conflict ()
{
  conflict_found = true;
}

void
Smodels::setToBTrue (Atom *a)
{
  if (a->isnant)
    score++;
  add_dependency (a, true);
  a->setBTrue ();
  stack.push (a);
}

void
Smodels::setToBFalse (Atom *a)
{
  if (a->isnant)
    score++;
  add_dependency (a, false);
  a->setBFalse ();
  stack.push (a);
}

void
Smodels::pick ()
{
  hi_index = 0;
  for (long i = 0; i < atomtop; i++)
    {
      Atom *a = atom[i];
      if (a->Bpos || a->Bneg)
	{
	  removeAtom (i);
	  i--;
	}
      else if (a->isnant)
	{
	  hi_index = i;
	  hi_is_positive = false;
	  break;
	}
    }
  choose ();
}

inline void
Smodels::expand ()
{
  long top = stack.top;
  dcl.dcl ();
  number_of_assignments += stack.top - top + 1; // One for the choice
}

bool
Smodels::conflict ()
{
  if (conflict_found)
    {
      PRINT_CONFLICT (cout << "Conflict" << endl);
      reset_queues ();
      conflict_found = false;
      return true;
    }
  for (OptimizeRule *r = program.optimize; r; r = r->next)
    if (r->minweight > r->minoptimum)
      return true;
    else if (r->minweight < r->minoptimum)
      return false;
    else if (r->next == 0)
      return true;
  return false;
}

void
Smodels::setup ()
{
  dcl.setup ();
  if (conflict_found) // Can't use conflict() as this removes the conflict.
    return;
  expand ();  // Compute well-founded model
  number_of_assignments--;
  dcl.reduce (true);  // Reduce the program strongly
  // Initialize literals chosen to be in the stable model / full set.
  long top = stack.top;
  for (Node *n = program.atoms.head (); n; n = n->next)
    {
      Atom *a = n->atom;
      // Negative literal
      if (a->computeFalse && a->Bneg == false)
	{
	  if (a->Bpos)
	    {
	      set_conflict ();
	      return;
	    }
	  setToBFalse (a);
	}
      // Positive literal
      if (a->computeTrue && a->Bpos == false)
	{
	  if (a->Bneg)
	    {
	      set_conflict ();
	      return;
	    }
	  setToBTrue (a);
	}
    }
  number_of_assignments += stack.top - top;
  expand ();
  number_of_assignments--;
  if (conflict_found)
    return;
  dcl.reduce (false); // Reduce weakly
  dcl.improve ();
  clear_dependency ();
  setup_top = stack.top;
  number_of_assignments--;
  level = 0;
  cnflct = 0;
}

void
Smodels::setup_with_lookahead ()
{
  setup ();
  while (!conflict_found && lookahead_no_heuristic ())
    expand ();
  setup_top = stack.top;
}

void
Smodels::improve ()
{
  dcl.reduce (false); // Reduce weakly
  dcl.improve ();
  dcl.denant ();
  clear_dependency ();
}

void
Smodels::revert ()
{
  // Revert to before the setup call.
  unwind_to (setup_top);
  reset_queues ();
  dcl.undenant ();
  dcl.unreduce ();
  dcl.revert ();
  clear_dependency ();
  stack.reset ();
  guesses = 0;
  fail = false;
  conflict_found = false;
  answer_number = 0;
  number_of_choice_points = 0;
  number_of_wrong_choices = 0;
  number_of_backjumps = 0;
  number_of_picked_atoms = 0;
  number_of_forced_atoms = 0;
  number_of_assignments = 0;
}

inline bool 
Smodels::covered ()
{
  return stack.full ();
}

Atom *
Smodels::unwind ()
{
  Atom *a = stack.pop ();
  while (a->guess == false)
    {
      PRINT_STACK (a->backtracked = false);
      PRINT_STACK (a->forced = false);
      if (a->Bpos)
	{
	  if (a->posScore > score)
	    a->posScore = score;
	  a->backtrackFromBTrue ();
	}
      else if (a->Bneg)
	{
	  if (a->negScore > score)
	    a->negScore = score;
	  a->backtrackFromBFalse ();
	}
      a = stack.pop ();
    }
  a->guess = false;
  guesses--;
  return a;
}

void
Smodels::unwind_to (long top)
{
  while (stack.top > top)
    {
      Atom *a = stack.pop ();
      if (a->guess)
	{
	  a->guess = false;
	  guesses--;
	}
      PRINT_STACK (a->backtracked = false);
      PRINT_STACK (a->forced = false);
      if (a->Bpos)
	a->backtrackFromBTrue ();
      else if (a->Bneg)
	a->backtrackFromBFalse ();
    }
  atomtop = program.number_of_atoms;
}

Atom *
Smodels::backtrack ()
{
  if (guesses == 0)
    {
      fail = true;
      return 0;
    }
  Atom *a = unwind ();
  PRINT_BACKTRACK (cout << "Backtracking: " << a->atom_name () << endl);
  PRINT_STACK (a->backtracked = true);
  if (a->Bneg)
    {
      a->backtrackFromBFalse ();
      a->setBTrue ();
    }
  else
    {
      a->backtrackFromBTrue ();
      a->setBFalse ();
    }
  stack.push (a);
  return a;
}

Atom *
Smodels::backjump ()
{
  for (;;) 
    {
      if (guesses == 0)
	{
	  fail = true;
	  return 0;
	}
      Atom *a = unwind ();
      PRINT_BACKTRACK (cout << "Backtracking: " << a->atom_name () << endl);
      bool b = a->Bneg;
      if (a->Bneg)
	a->backtrackFromBFalse ();
      else
	a->backtrackFromBTrue ();
      PRINT_STACK (a->backtracked = false);
      PRINT_STACK (a->forced = false);
      if (cnflct == 0 || guesses < level || dcl.path (cnflct,a))
	{
	  if (guesses < level)
	    level = guesses;
	  PRINT_STACK (a->backtracked = true);
	  if (b)
	    a->setBTrue ();
	  else
	    a->setBFalse ();
	  stack.push (a);
	  cnflct = a;
	  return a;
	}
      number_of_backjumps++;
    }
}

bool
Smodels::testPos (Atom *a)
{
  if (a->isnant)
    score = 1;
  else
    score = 0;
  stack.push (a);
  add_dependency (a, true);
  a->guess = true;
  a->pos_tested = true;
  guesses++;
  a->setBTrue ();
  number_of_picked_atoms++;
  expand ();
  if (conflict ())
    {
      // Backtrack puts the atom onto the stack.
      number_of_forced_atoms++;
      backtrack ();
      cnflct = a;
      PRINT_STACK (a->forced = true);
      return true;
    }
  unwind ();
  a->posScore = score;
  a->backtrackFromBTrue ();
  return false;
}

bool
Smodels::testNeg (Atom *a)
{
  if (a->isnant)
    score = 1;
  else
    score = 0;
  stack.push (a);
  add_dependency (a, false);
  a->guess = true;
  a->neg_tested = true;
  guesses++;
  a->setBFalse ();
  number_of_picked_atoms++;
  expand ();
  if (conflict ())
    {
      // Backtrack puts the atom onto the stack.
      number_of_forced_atoms++;
      backtrack ();
      cnflct = a;
      PRINT_STACK (a->forced = true);
      return true;
    }
  unwind ();
  a->negScore = score;
  a->backtrackFromBFalse ();
  return false;
}

void
Smodels::testScore (Atom *a, long i)
{
  unsigned long mn, mx;
  bool positive;
  if (a->negScore < a->posScore)
    {
      mn = a->negScore;
      mx = a->posScore;
      positive = true;
    }
  else
    {
      mn = a->posScore;
      mx = a->negScore;
      positive = false;
    }
  if (mn > hiscore1 || (mn == hiscore1 && mx > hiscore2))
    {
      hiscore1 = mn;
      hiscore2 = mx;
      hi_index = i;
      hi_is_positive = positive;
    }
}

//
// Choose a literal that gives rise to a conflict. 
//
bool
Smodels::lookahead_no_heuristic ()
{
  hiscore1 = 0;
  hiscore2 = 0;
  hi_is_positive = false;
  hi_index = 0;
  reset_dependency ();
  long i;
  for (i = lasti; i < atomtop; i++)
    {
      Atom *a = atom[i];
      if (a->Bpos || a->Bneg)
	{
	  removeAtom (i);
	  i--;
	  continue;
	}
      if (a->dependsonFalse == false && testNeg (a))
	goto conflict;
      if (a->dependsonTrue == false && testPos (a))
	goto conflict;
      if (a->pos_tested && a->neg_tested)
	testScore (a, i);
    }
  for (i = 0; i < atomtop && i < lasti; i++)
    {
      Atom *a = atom[i];
      if (a->Bpos || a->Bneg)
	{
	  removeAtom (i);
	  if (hi_index == atomtop)
	    hi_index = i;
	  if (atomtop < lasti)
	    i--;
	  continue;
	}
      if (a->dependsonFalse == false && testNeg (a))
	goto conflict;
      if (a->dependsonTrue == false && testPos (a))
	goto conflict;
      if (a->pos_tested && a->neg_tested)
	testScore (a, i);
    }
  return false;
 conflict:
  lasti = i;
  removeAtom (i);
  return true;
}

//
// Find the literal that brings the most atoms into the closures.
//
void
Smodels::heuristic ()
{
  for (long i = 0; i < atomtop; i++)
    {
      Atom *a = atom[i];
      if (a->neg_tested && a->pos_tested)
	continue;
      if (a->negScore < hiscore1 ||
	  (a->negScore == hiscore1 && a->posScore <= hiscore2))
	  continue;
      if (a->posScore < hiscore1 ||
	  (a->posScore == hiscore1 && a->negScore <= hiscore2))
      	continue;
      if (a->neg_tested == false)
	{
	  testNeg (a);
	  if (a->negScore < hiscore1 ||
	      (a->negScore == hiscore1 && a->posScore <= hiscore2))
	      continue;
	}
      if (a->pos_tested == false)
	testPos (a);
      testScore (a, i);
    }
  if (program.optimize)
    optimize_heuristic ();
}

void
Smodels::optimize_heuristic ()
{
  Atom *a = atom[hi_index];
  // Test positive
  stack.push (a);
  add_dependency (a, true);
  a->guess = true;
  a->pos_tested = true;
  guesses++;
  a->setBTrue ();
  number_of_picked_atoms++;
  expand ();
  for (OptimizeRule *r = program.optimize; r; r = r->next)
    r->heuristic_best = r->minweight;
  unwind ();
  a->backtrackFromBTrue ();
  // Test negative
  stack.push (a);
  add_dependency (a, false);
  a->guess = true;
  a->neg_tested = true;
  guesses++;
  a->setBFalse ();
  number_of_picked_atoms++;
  expand ();
  for (OptimizeRule *r = program.optimize; r; r = r->next)
    if (r->minweight < r->heuristic_best)
      {
	hi_is_positive = false;
	break;
      }
    else if (r->minweight > r->heuristic_best)
      {
	hi_is_positive = true;
	break;
      }
  unwind ();
  a->backtrackFromBFalse ();
}

//
// Choose the literal that brings the most atoms into the closures.
//
void
Smodels::choose ()
{
  Atom *a = atom[hi_index];
  stack.push (a);
  a->guess = true;
  guesses++;
  removeAtom (hi_index);
  cnflct = 0;
  if (hi_is_positive)
    a->setBTrue ();
  else
    a->setBFalse ();
  number_of_picked_atoms++;
  number_of_choice_points++;
  PRINT_PICK (if (hi_is_positive)
	      cout << "Chose " << a->atom_name () << endl;
	      else
	      cout << "Chose not " << a->atom_name () << endl);
}

//
// Choose a literal that gives rise to a conflict. 
// If no such literal exists we choose the literal 
// that brings the most atoms into the closures.
//
void
Smodels::lookahead ()
{
  if (lookahead_no_heuristic ())
    return;
  heuristic ();
  choose ();
}

void
Smodels::backtrack (bool jump)
{
  Atom *a;
  if (jump)
    a = backjump ();
  else
    a = backtrack ();
  if (a)
    addAtom (a);
  number_of_wrong_choices++;
}

int
Smodels::smodels (bool look, bool jump)
{
  if (look)
    setup_with_lookahead ();
  else
    setup ();
  if (conflict ())
    return 0;
  if (look && !covered ())
    {
      heuristic ();
      improve ();
      choose ();
    }
  while (!fail)
    {
      PRINT_DCL (cout << "Smodels call" << endl;
		 dcl.print ());
      PRINT_BF (print ());
      expand ();
      PRINT_BF (cout << "Expand" << endl);
      PRINT_DCL (dcl.print ());
      PRINT_BF (print ());
      PRINT_PROGRAM(printProgram ());
      PRINT_STACK (printStack ());
      if (conflict ())
	backtrack (jump);
      else if (covered ())
	{
	  answer_number++;
	  level = guesses;
	  cout << "Answer: " << answer_number << endl;
	  printAnswer ();
	  program.set_optimum ();
	  if (max_models && answer_number >= max_models)
	    return 1;
	  else
	    backtrack (jump);
	}
      else if (look)
	lookahead ();
      else
	pick ();
    }
  number_of_wrong_choices--;
  return 0;
}

int
Smodels::model (bool look, bool jump)
{
  if (answer_number)
    backtrack (jump);
  else
    {
      if (look)
	setup_with_lookahead ();
      else
	setup ();
      if (conflict ())
	return 0;
      if (look && !covered ())
	{
	  heuristic ();
	  improve ();
	  choose ();
	}
    }
  while (!fail)
    {
      expand ();
      if (conflict ())
	backtrack (jump);
      else if (covered ())
	{
	  answer_number++;
	  level = guesses;
	  program.set_optimum ();
	  return 1;
	}
      else if (look)
	lookahead ();
      else
	pick ();
    }
  number_of_wrong_choices--;
  return 0;
}

int
Smodels::smodels (bool look, bool jump, long max_conflicts)
{
  long conflicts = 0;
  while (!fail)
    {
      expand ();
      if (conflict ())
	{
	  backtrack (jump);
	  conflicts++;
	}
      else if (covered ())
	{
	  answer_number++;
	  level = guesses;
	  cout << "Answer: " << answer_number << endl;
	  printAnswer ();
	  program.set_optimum ();
	  if (max_models && answer_number >= max_models)
	    {
	      unwind_to (setup_top);
	      return 1;
	    }
	  else
	    {
	      backtrack (jump);
	      conflicts++;
	    }
	}
      else if (conflicts >= max_conflicts)
	break;  // Must do expand after backtrack to retain
		// consistency
      else if (look)
	lookahead ();
      else
	pick ();
    }
  unwind_to (setup_top);
  if (fail)
    {
      number_of_wrong_choices--;
      return -1;
    }
  return 0;
}

int
Smodels::wellfounded ()
{
  setup ();
  if (conflict ())
    return 0;
  cout << "Well-founded model: " << endl;
  cout << "Positive part: ";
  for (Node *n = program.atoms.head (); n; n = n->next)
    if (n->atom->Bpos && n->atom->name)
      cout << n->atom->name << ' ';
  cout << endl << "Negative part: ";
  for (Node *n = program.atoms.head (); n; n = n->next)
    if (n->atom->Bneg && n->atom->name)
      cout << n->atom->name << ' ';
  cout << endl;
  return 1;
}

void
Smodels::shuffle ()
{
  Atom *t;
  long i, r;
  for (i = 0; i < atomtop; i++) {
    t = atom[i];
    // If the low order bits aren't as random as the high order bits,
    // your random number generator is broken.
    r = rand ()%(atomtop-i)+i;
    atom[i] = atom[r];
    atom[r] = t;
  }
}

void
Smodels::print ()
{
  cout << "Body: ";
  for (Node *n = program.atoms.head (); n; n = n->next)
    if (n->atom->Bpos)
      cout << n->atom->atom_name () << ' ';
  cout << endl << "NBody: ";
  for (Node *n = program.atoms.head (); n; n = n->next)
    if (n->atom->Bneg)
      cout << n->atom->atom_name () << ' ';
  cout << endl << "Pick: ";
  Atom **a;
  for (a = stack.stack; a != stack.stack+stack.top; a++)
    if ((*a)->guess)
      if((*a)->Bneg)
	cout << "not " << (*a)->atom_name () << ' ';
      else
	cout << (*a)->atom_name () << ' ';
  cout << endl;
}

void
Smodels::printAnswer ()
{
  // Prints the stable model.
  cout << "Stable Model: ";
  for (Node *n = program.atoms.head (); n; n = n->next)
    if (n->atom->Bpos && n->atom->name)
      cout << n->atom->name << ' ';
  cout << endl;
  for (OptimizeRule *r = program.optimize; r; r = r->next)
    {
      cout << "{ ";
      int comma = 0;
      for (Atom **a = r->pbody; a != r->end; a++)
	if ((*a)->name)
	  {
	    if (comma)
	      cout << ", ";
	    cout << (*a)->name;
	    comma = 1;
	  }
      for (Atom **a = r->nbody; a != r->pbody; a++)
	if ((*a)->name)
	  {
	    if (comma)
	      cout << ", ";
	    cout << "not " << (*a)->name;
	    comma = 1;
	  }
      cout << " } ";
      cout.precision (DBL_DIG);
      cout << "min = " << r->minweight << endl;
    }
}

void
Smodels::printStack ()
{
  long i;
  cout << "\x1b[1;1fStack: ";
  for (i = 0; i < stack.top; i++)
    {
      Atom *a = stack.stack[i];
      if (a->forced)
	cout << "\x1b[31m";
      else if (a->backtracked)
	cout << "\x1b[32m";
      else if (a->guess)
	cout << "\x1b[34m";
      if (a->Bneg)
	cout << "not " << a->atom_name ();
      else
	cout << a->atom_name ();
      cout << "\x1b[0m ";
    }
  cout << "\x1b[0J" << endl;
  //  sleep(1);
}
