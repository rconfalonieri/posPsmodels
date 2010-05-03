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
#include <iostream.h>
#include <iomanip.h>
#include <unistd.h>
#include <limits.h>
#include <float.h>
#include <stdlib.h>
#include "print.h"
#include "atomrule.h"
#include "smodels.h"

#include <ctype.h>

/*** TSS 4.1.2001 ***/
#include <fstream>
#include <strstream>
/*** TSS 4.1.2001 ***/



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

  /*** TSS 4.1.2001 ***/
  status_restored = 1;
  status_data = 0;
  /*** TSS 4.1.2001 ***/
  /*** TSS 4.6.2001 ***/
  print_conservative = false;
  print_credulous = false;
  print_unsat = false;
  /*** TSS 4.6.2001 ***/
}

Smodels::~Smodels ()
{
  delete[] atom;
}

/*** TSS 2.1.2001 ***/
const size_t BUFSIZE_LOCAL = 4096;

long
Smodels::remaining_searchspace()
{
  long size = size_of_searchspace;
  long i = 0;

  for (i = 0; i < stack.top; i++) {
    Atom *a = stack.stack[i];
    if (a->guess)
      break;
    if (a->backtracked)
      size--;
  }
  return size;
}

long
Smodels::searchtree_depth()
{
  long depth = 0;
  long i;
  for (i = 0; i < stack.top; i++) {
    Atom *a = stack.stack[i];
    if (a->guess || a->backtracked)
      depth++;
  }
  return depth;
}

void
Smodels::read_continuation()
{
  char buf[BUFSIZE_LOCAL];
  char *p;
  long a;
  bool forced = false;
  bool backtracked = false;
  bool guess = false;
  bool positive = true;

  
  status_restored = false;
  status_data = new continuation_data[program.number_of_atoms];

  while (cin.getline(buf, BUFSIZE_LOCAL)) {
    forced = false;
    backtracked = false;
    guess = false;
    positive = true;
  
    p = buf;

    if (*p) {
      if (*p == '-') {
	positive = false;
	p++;
      }

      if (isdigit(*p)) {
	a = strtol(p, 0, 10);
      } else {
	switch (*p) {
	case 'b':
	  backtracked = true;
	  break;
	case 'f':
	  forced = true;
	  break;
	case 'g':
	  guess = true;
	  break;
	default:
	  break;
	}
	p++; p++;
	if (*p == '-') {
	  positive = false;
	  p++;
	}
	a = strtol(p, 0, 10);
      }
      status_data[a-1].negative = !positive;
      status_data[a-1].forced = forced;
      status_data[a-1].backtracked = backtracked;
      status_data[a-1].guess = forced;
      status_data[a-1].active = true;
      
    }
  }
  return;  
}

void
Smodels::init (bool cont)
{
  program.init ();
  dcl.init ();
  atom = new Atom *[program.number_of_atoms];
  /*** TSS 2.1.2001
  for (Node *n = program.atoms.head (); n; n = n->next) 
    atom[atomtop++] = n->atom;
  ***/
  for (Node *n = program.atoms.head (); n; n = n->next) {
    atom[atomtop] = n->atom;
    n->atom->index = atomtop++;
  }
  /*** TSS 2.1.2001 ***/
  stack.Init (program.number_of_atoms);
  depends.Init (program.number_of_atoms);
  lasti = 0;

  /*** TSS 2.1.2001 ***/
  if (cont) {
      read_continuation();
      // read the computation state
  }
  /*** TSS 2.1.2001 ***/
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

  //  atom[atomtop]->index = atomtop;
  //  atom[n]->index = n;
  /*** TSS 4.1.2001 ***/
  //  continuation_data tmp = status_data[atomtop];
  //  status_data[atomtop] = status_data[n];
  //  status_data[n] = tmp;
  /*** TSS 4.1.2001 ***/
  
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

  
  /*** TSS 29.5.01 ***/
  //  find_equalities(1, false);
  //  expand (); // and again
  /*** TSS 29.5.01 ***/

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
      /** TSS 2.1.2001 **/
      a->backtracked = false;
      a->forced = false;
      /** TSS 2.1.2001 **/
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
      /*** TSS 2.1.2001 **/
      a->backtracked = false;
      a->forced = false;
      /*** TSS 2.1.2001 **/
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
  PRINT_BACKTRACK (cout << "Backtracking: " << a->atom_name () <<
		   endl);
  /*** TSS 2.1.2001 **/
  a->backtracked = true;
  /*** TSS 2.1.2001 **/
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
      /** TSS 2.1.2001 **/
      a->backtracked = false;
      a->forced = false;
      /** TSS 2.1.2001 **/

      if (cnflct == 0 || guesses < level || dcl.path (cnflct,a))
	{
	  if (guesses < level)
	    level = guesses;
	  /*** TSS 2.1.2001 **/
	  a->backtracked = true;
	  /*** TSS 2.1.2001 **/
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

/*** TSS 4.1.2001 ***/
void
Smodels::dump_status()
{
  static int dump_no;

  char dump_file[100];

  ostrstream filename(dump_file, 100);
  filename << "smodels_dump." << dump_no++ << '\0';
  filename << close;
  

  cerr << "Dumping computation status to file `"
       << dump_file << "'" << endl;

  ofstream os(dump_file);
  
  if (!os) {
    cerr << "Cannot open dump file, using stderr instead " << endl;
    program.print_internal(max_models, cerr);
    dump_stack(cerr);
    return;
  }
  program.print_internal(max_models, os);
  dump_stack(os);
  os.close();
  cerr << "done" << endl;
}

void
Smodels::dump_stack(ostream& os)
{
  long i;
  for (i = 0; i < stack.top-1; i++)
    {
      Atom *a = stack.stack[i];
      if (a->forced)
	os << "f ";
      else if (a->backtracked)
	os << "b ";
      else if (a->guess)
	os << "g ";
      if (a->Bneg) {
	os << "-";
      }
      os << a->index +1;
      if (a->name) {
	os << " (" << a->name << ")";
      }
      os  << endl;
    }
}
/*** TSS 4.1.2001 ***/



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
      /*** TSS 2.1.2001 **/
      // a->forced = true;
      /*** TSS 2.1.2001 **/
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
      /*** TSS 2.1.2001 **/
      // a->forced = true;
      /*** TSS 2.1.2001 **/
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
  /*** TSS 4.1.2001 ***/
  if (!status_restored) {
    if (status_data[a->index].active) {
      if (status_data[a->index].backtracked) {
	a->backtracked = true;
  	if (status_data[a->index].negative) {
	  a->setBFalse();
  	} else {
	  a->setBTrue();
  	}
	//	removeAtom(hi_index);
	return;
      }
    } else {
      status_restored = true;
    }
  }
  /*** TSS 4.1.2001 ***/
  
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
Smodels::smodels (bool look, bool jump, bool cont)
{
  /*** TSS 2.1.2001 ***/
  //  if (!cont) {
  if (look)
    setup_with_lookahead ();
  else
    setup ();
  //  }
  /*** TSS 2.1.2001 ***/
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
Smodels::smodels (bool look, bool jump, long max_conflicts, bool cont)
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
  for (Node *n = program.atoms.head (); n; n = n->next) {
    if (n->atom->Bpos && n->atom->name) {
      cout << n->atom->name << ' ';
    }
    /*** TSS 4.6.2001 ***/
    if (n->atom->Bpos) {
      n->atom->sometimes_true = 1;
    } else {
      n->atom->sometimes_false = 1;
    }
    /*** TSS 4.6.2001 ***/
  }
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
#if 0
  /*** TSS 2.1.2001 ***/
  long i;
  cout << "Stack:" << endl;
  for (i = 0; i < stack.top; i++)
    {
      Atom *a = stack.stack[i];
      if (a->forced)
	cout << "f ";
      else if (a->backtracked)
	cout << "b ";
      else if (a->guess)
	cout << "g ";
      if (a->Bneg) {
	cout << "-";
      }
      cout << a->index +1;
      if (a->name) {
	cout << " (" << a->name << ")";
      }
      cout  << endl;
    }
  /*** TSS 2.1.2001 ***/
#else
  long i;
  //  cerr << "\x1b[1;1fStack: ";
  cerr << "Stack: " << endl;
  for (i = 0; i < stack.top; i++)
    {
      Atom *a = stack.stack[i];

      if (!a->name)
	continue;
//        if (a->forced)
//  	cerr << "\x1b[31m";
//        else if (a->backtracked)
//  	cerr << "\x1b[32m";
//        else if (a->guess)
//  	cerr << "\x1b[34m";
      if (a->Bneg)
	cerr << " not " << a->atom_name ();
      else
	cerr << " " << a->atom_name ();
      //      cerr << "\x1b[0m ";
    }
  //  cerr << "\x1b[0J" << endl;
  cerr << endl;
  //  sleep(1);
#endif
}


#if 0
/*** TSS 28.5.2001 ***/
inline long atom_to_index(long a)
{
  return a + 2;
}

inline long index_to_atom(long a)
{
  return a - 2;
}

inline long representative(long *eq_table, long item)
{
  return eq_table[item];
}


void Smodels::find_equalities(int depth)
{
  if (depth == 0)
    return;

  bool possibly_true = true;
  bool possibly_false = true;
  
  long *equivalences = new long[program.number_of_atoms+2];
  signed char *pos_branch = new signed char [program.number_of_atoms+2];
  signed char *neg_branch = new signed char [program.number_of_atoms+2];

  for (long i = 0; i < program.number_of_atoms+2; i++) {
    equivalences[i] = i;
    pos_branch[i] = -1;
    neg_branch[i] = -1;
  }

  for (Node *n = program.atoms.head(); n; n = n->next) {
    Atom *at = n->atom;

    if (at->Bpos || at->Bneg) // skip atoms with values
      continue;

    possibly_true = branch(n->atom, true, pos_branch, depth -1);
    possibly_false = branch(n->atom, false, neg_branch, depth -1);

    if (possibly_true) {
      if (possibly_false) {
	merge(equivalences, pos_branch, neg_branch);
      } else {
	merge(equivalences, pos_branch, 0);
      }
    } else {
      if (possibly_false) {
	merge(equivalences, 0, neg_branch);
      } else {
	// always a contradiction:
	make_equal(at->index, true);
	make_equal(at->index, false);
      }
    }
  }
  apply_equalities(equivalences);
  return;
}

bool Smodels::branch(Atom *atom, bool value, signed char *result, int
		     max_depth)
{
  Atom *at = 0;
  set_branch_truth_value(atom, value);
  expand();
  if (conflict()) {
    clear_branch_truth_value(atom, value);
    return false;
  }
  if (max_depth == 0) { // leaf
    for (Node *n = program.atoms.head (); n; n = n->next) {
      at = n->atom;
      if (at->Bpos) {
	result[at->index] = true;
      } else if (at->Bneg) {
	result[at->index] = false;
      }
    }
  } else { // branch deeper
    atom->branch_merge_tested = true;
    bool possibly_true = true;
    bool possibly_false = true;
    signed char *pos_branch = new signed char[program.number_of_atoms+2];
    signed char *neg_branch = new signed char[program.number_of_atoms+2];
    for (long i = 0; i < program.number_of_atoms+2; i++) {
      pos_branch[i] = -1;
      neg_branch[i] = -1;
    }
    possibly_true = branch(
    
    atom->branch_merge_tested = false;
  }
  clear_branch_truth_value(atom, value);
  return true;
}


void Smodels::branch(Atom *atom, long *pos_result,
		     long *neg_result, int max_depth)
{
  if (max_depth == 1) { // leaf
    set_branch_truth_value(atom, true);
    expand();
    if (!conflict()) {
      Atom *at;
      for (Node *n = program.atoms.head (); n; n = n->next) {
	at = n->atom;
	if (at->Bpos) {
	  pos_result[at->index] = true;
	} else if (at->Bneg) {
	  pos_result[at->index] = false;
	}
      }
    } // return nogood if conflict
    clear_branch_truth_value(atom, true);
    
    set_branch_truth_value(atom, false);
    expand();
    if (!conflict()) {
      Atom *at;
      for (Node *n = program.atoms.head (); n; n = n->next) {
	at = n->atom;
	if (at->Bpos) {
	  neg_result[at->index] = true;
	} else if (at->Bneg) {
	  neg_result[at->index] = false;
	}
      }
    } // return nogood if conflict
    clear_branch_truth_value(atom, false);
  } else { // branch deeper 
    atom->branch_merge_tested = true;
    long *pos_branch = new long[program.number_of_atoms+2];
    long *neg_branch = new long[program.number_of_atoms+2];

    for (long i = 0; i < program.number_of_atoms+2; i++) {
      pos_branch[i] = -1;
      neg_branch[i] = -1;
    }
    
    set_branch_truth_value(atom, true);
    expand();

    if (!conflict()) {
      for (Node *n = program.atoms.head (); n; n = n->next) {
	Atom *at = n->atom;
	if (!at->branch_merge_tested && !at->Bpos && !at->Bneg) {
	  branch(at, pos_branch, neg_branch, max_depth-1);
	  merge(pos_result, pos_branch, neg_branch);
	}
      }
    } // should return a no-good when in conflict
    
    clear_branch_truth_value(atom, true);
    
    set_branch_truth_value(atom, false);
    expand();

    if (!conflict()) {
      for (Node *n = program.atoms.head (); n; n = n->next) {
	Atom *at = n->atom;
	if (!at->branch_merge_tested && !at->Bpos && !at->Bneg) {
	  branch(at, pos_branch, neg_branch, max_depth -1);
	  merge(neg_result, pos_branch, neg_branch);
	}
      }
    } // should return a no-good when in conflict
    
    clear_branch_truth_value(atom, false);    

    delete [] pos_branch;
    delete [] neg_branch;
    atom->branch_merge_tested = false;
  }
}


void Smodels::find_equalities(int depth, bool greedy_apply)
{
  if (depth == 0) {
    return;
  }

  // allocate three arrays to hold equivalences
  // entry 0 is for false and 1 for true
  long *equivalences  = new long[program.number_of_atoms+2];
  long *pos_branch = new long[program.number_of_atoms+2];
  long *neg_branch = new long[program.number_of_atoms+2];

  for (long i = 0; i < program.number_of_atoms+2; i++) {
    equivalences[i] = i;
    pos_branch[i] = -1;
    neg_branch[i] = -1;
  }
  
  Node *n = 0;
  for (n = program.atoms.head() ; n ; n = n->next) {
    Atom *at = n->atom;

    if (at->Bpos || at->Bneg) // skip atoms with values
      continue;
    
    branch(n->atom, pos_branch, neg_branch, depth);
    merge(equivalences, pos_branch, neg_branch);

  }
  apply_equalities(equivalences);

  delete [] equivalences;
  delete [] pos_branch;
  delete [] neg_branch;
}




void Smodels::set_branch_truth_value(Atom *a, bool value)
{
  if (value) {  // true
    stack.push (a);
    add_dependency (a, true);
    a->guess = true;
    guesses++;
    a->setBTrue();
  } else { // false
    stack.push (a);
    add_dependency(a, false);
    a->guess = true;
    guesses++;
    a->setBFalse ();
  }
}

void Smodels::clear_branch_truth_value(Atom *a, bool value)
{
  unwind();
  if (value) { //true
    a->backtrackFromBTrue();
  } else {
    a->backtrackFromBFalse();
  }
}


void Smodels::merge(long *result, long *pos_data, long *neg_data)
{
  for (Node *n = program.atoms.head (); n; n = n->next) {
    Atom *at = n->atom;
    long id = at->index;

    if (pos_data[id] < 0) {
      continue;
    }
    
    // always true / false 
    if (pos_data[id] == neg_data[id]) {
      if (pos_data[id] == true) {
	result[atom_to_index(id)] = true;
      }
      if (pos_data[id] == false) {
	result[atom_to_index(id)] = false;
      }
    }

    for (Node *p = program.atoms.head(); p; p = p->next) {
      long id2 = p->atom->index;
      if ((id == id2) || (pos_data[id2] < 0)) {
	continue;
      }
      
      if ((pos_data[id] == pos_data[id2]) &&
	  (neg_data[id] == neg_data[id2])) {
	// make equal with path compression
	if (id < id2) {
	  result[atom_to_index(id2)] = result[atom_to_index(id)];
	} else {
	  result[atom_to_index(id)] = result[atom_to_index(id2)];
	}
      }

      // also inequivalence should be here
    }
  }
}

void Smodels::apply_equalities(long *eq)
{
  print_equivalences(eq);

  // set all members of the equivalence class `false' untrue
  // set all members of the equivalence class `true' true
  // replace all atoms of other equivalence classes by their
  // representative
  long a_index;
  long canon;
  Atom *a;
  for (long i = program.number_of_atoms +1; i > 1; --i) {
    a_index = index_to_atom(i);
    a = atom[a_index];
    // value already, don't mess with it
    if (a->Bpos || a->Bneg) {
      continue;
    }

    canon = representative(eq, i);
    
    if (canon == 0) {     // false
      number_of_forced_atoms++;
      stack.push(a);
      a->setBFalse();
    } else if (canon == 1) { // true
      number_of_forced_atoms++;
      stack.push(a);
      a->setBTrue();
    } else if (canon != i) {
      // *** EI TOIMI *** 
    }
  }
  
  
}

void Smodels::print_equivalences(long *eq)
{
  static bool *printed;
  long class_no = 0;
  
  if (!printed) {
    printed = new bool[program.number_of_atoms +2];
  }

  for (long i = 0; i < program.number_of_atoms +2 ; ++i) {
    printed[i] = false;
  }

  cout << "Equivalence classes: " << endl;

  long at, new_at, a_index;

  for (long i = 0 ; i < program.number_of_atoms +2; ++i) {
    if (!printed[i]) { // a new equivalence class found
      cout << ++class_no << ": " ;
      printed[i] = true;
      a_index = index_to_atom(i);
      switch (a_index) {
      case -2:
	cout << "false ";
	break;
      case -1:
	cout << "true ";
	break;
      default:
	cout << atom[a_index]->atom_name() << " ";
      }
      for (long j = i+1; j < program.number_of_atoms +2; ++j) {
	if (representative(eq, j) == i) {
	  a_index = index_to_atom(j);
	  switch (a_index) {
	  case -2:
	    cout << "false ";
	    break;
	  case -1:
	    cout << "true ";
	    break;
	  default:
	    cout << atom[a_index]->atom_name() << " ";
	  }
	  printed[j] = true;
	}
      }
      cout << endl;
    }
  }
}
/*** TSS 28.5.2001 ***/
#endif

/*** TSS 4.6.2001 **/
void Smodels::print_conservative_model(bool surely_all)
{
  cout << "Conservative Model";

  if (!surely_all) {
    cout << " (possibly too large)";
  }
  cout << ": ";

  for (Node *n = program.atoms.head (); n ; n = n->next) {
    if (n->atom->name && n->atom->sometimes_true &&
	!n->atom->sometimes_false) {
      cout << n->atom->name << ' ';
    }
  }
  cout << endl;
}

void Smodels::print_credulous_model(bool surely_all)
{
  cout << "Credulous Model";
  if (!surely_all) {
    cout << " (possibly incomplete)";
  }
  cout << ": ";

  for (Node *n = program.atoms.head (); n ; n = n->next) {
    if (n->atom->name && n->atom->sometimes_true) {
      cout << n->atom->name << ' ';
    }
  }
  cout << endl;
}

void Smodels::print_unsat_atoms(bool surely_all)
{
  cout << "Unsatisfiable Atoms:";
  if (!surely_all) {
    cout << " (possibly too large)";
  }
  cout << ": ";
  
  for (Node *n = program.atoms.head (); n ; n = n->next) {
    if (n->atom->name && !n->atom->sometimes_true) {
      cout << n->atom->name << ' ';
    }
  }
  cout << endl;
}
/*** TSS 4.6.2001 **/
