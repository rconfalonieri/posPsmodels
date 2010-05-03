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
#include <stdlib.h>
#if defined _BSD_SOURCE || defined _SVID_SOURCE
#include <sys/time.h>
#include <sys/resource.h>
#endif
#include "stable.h"

/*** TSS 25.10.2000 ***/
#include <signal.h>
#include <unistd.h>
typedef void Sigfunc(int);

/*** TSS 25.10.2000 ***/


Stable::Stable ()
  : api (&smodels.program),
    reader (&smodels.program, &api)
{
  wellfounded = false;
  lookahead = true;
  backjumping = false;
  randomized = false;
  /*** TSS 2.1.2001 ***/
  cont = false;
  /*** TSS 2.1.2001 ***/
  /*** TSS 27.10.2000 ***/
  max_seconds = 0;
  /*** TSS 27.10.2000 ***/
  max_conflicts = 0;
  max_tries = 1;
  seed = 1;
}

Stable::~Stable ()
{
}

int
Stable::read (istream &f)
{
  timer.start ();
  int r = reader.read (f);
  // Set up atoms
  api.done ();

  /*** TSS 27.10.2000 ***/
  if (max_seconds > 0) {
    timer.set_alarm(max_seconds);
  }
  /*** TSS 27.10.2000 ***/
  
  if (r == 0)
    {
      /*** TSS 2.1.2001 ***/
      smodels.init (cont);
      /*** TSS 2.1.2001 ***/
      smodels.max_models = reader.models;
    }
  return r;
}

int
Stable::tries ()
{
  srand (1);   // Reset
  srand (seed);
  smodels.setup_with_lookahead ();
  int r = smodels.conflict ();
  if (!r)
    smodels.improve ();
  long tries;
  for (tries = 0; tries < max_tries && !r; tries++)
    {
      smodels.shuffle ();
      r = smodels.smodels (lookahead, backjumping, max_conflicts, cont);
    }
  if (tries == 0 || r == -1)
    return 0;
  else
    return 1;
}

// Randomized search
int
Stable::randomize ()
{
  srand (1);   // Reset
  srand (seed);
  smodels.setup_with_lookahead ();
  int r = smodels.conflict ();
  if (!r)
    smodels.improve ();
  long tries;
  max_conflicts = smodels.program.number_of_atoms - smodels.stack.top;
  for (;;)
    {
      for (tries = 0; tries < max_tries && !r; tries++)
	{
	  smodels.shuffle ();
	  r = smodels.smodels (lookahead, backjumping, max_conflicts, cont);
	}
      if (tries == 0 || r == -1)
	return 0;
      else  if (r)
	return 1;
      max_conflicts *= 2;
    }
}

void
Stable::print_internal ()
{
  if (wellfounded || !lookahead)
    {
      smodels.setup ();
      if (!smodels.conflict ())
	smodels.program.print_internal (smodels.max_models);
    }
  else
    {
      smodels.setup_with_lookahead ();
      if (!smodels.conflict ())
	{
	  smodels.improve ();
	  smodels.program.print_internal (smodels.max_models);
	}
    }
}

void
Stable::print_time ()
{
#if defined _BSD_SOURCE || defined _SVID_SOURCE
  struct rusage rusage;
  getrusage (RUSAGE_SELF, &rusage);
  cout << "Duration: " << rusage.ru_utime.tv_sec << '.'
       << setw(3) << setfill('0') << rusage.ru_utime.tv_usec/1000
       << endl;
#else
  cout << "Duration: " << timer.print () << endl;
#endif
}

void
Stable::calculate ()
{
  int r = 0;
  if (wellfounded)
    r = smodels.wellfounded ();
  else if (randomized)
    r = randomize ();
  else if (max_conflicts == 0)
    {
      if (seed != 1)
	{
	  srand (1);   // Reset
	  srand (seed);
	  smodels.shuffle ();
	}
      r = smodels.smodels (lookahead, backjumping, cont);
    }
  else
    r = tries ();
  if (r)
    cout << "True" << endl;
  else
    cout << "False" << endl;
  timer.stop ();

  /*** TSS 4.6.2001 ***/
  if (smodels.print_conservative && (smodels.answer_number > 1)) {
    smodels.print_conservative_model(!r);
  }
  if (smodels.print_credulous && (smodels.answer_number > 1)) {
    smodels.print_credulous_model(!r);
  }
  if (smodels.print_unsat &&  (smodels.answer_number > 1)) {
    smodels.print_unsat_atoms(!r);
  }
  /*** TSS 4.6.2001 ***/

  
  print_time ();
  cout << "Number of choice points: " <<
    smodels.number_of_choice_points << endl;
  cout << "Number of wrong choices: " <<
    smodels.number_of_wrong_choices << endl;
  cout << "Number of atoms: " << smodels.program.number_of_atoms <<
    endl;
  cout << "Number of rules: " << smodels.program.number_of_rules <<
    endl;
  if (lookahead)
    {
      cout << "Number of picked atoms: " <<
	smodels.number_of_picked_atoms << endl;
      cout << "Number of forced atoms: " <<
	smodels.number_of_forced_atoms << endl;
      cout << "Number of truth assignments: " <<
	smodels.number_of_assignments << endl;
      cout << "Size of searchspace (removed): " 
	   << smodels.size_of_searchspace << " ("
	   << smodels.number_of_denants << ')' << endl;
    }
  if (backjumping)
    cout << "Number of backjumps: " << smodels.number_of_backjumps <<
      endl;
}
