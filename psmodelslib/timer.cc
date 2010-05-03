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
#include <sstream>
#include <iomanip>
#include <limits.h>
#include "timer.h"

/*** TSS 27.10.2000 ***/
#include <csignal>
// #include <unistd>
#include <sys/time.h>
#include <sys/resource.h>
/*** TSS 27.10.2000 ***/



Timer::Timer ()
{
  sec = 0;
  msec = 0;
}

void
Timer::start ()
{
  timer = clock ();
}

void
Timer::stop ()
{
  clock_t ticks = clock () - timer;
  long s = ticks / CLOCKS_PER_SEC;
  sec += s;
  msec += (ticks - s*CLOCKS_PER_SEC)*1000/CLOCKS_PER_SEC;
  if (msec >= 1000)
    {
      msec -= 1000;
      sec++;
    }
}

void 
Timer::reset ()
{
  sec = 0;
  msec = 0;
}

char *
Timer::print ()
{
  static char s[20];

// fixme:
//  ostrstream str (s, sizeof (s));
//  str << sec << '.' << setw(3) << setfill('0') << msec << ends;
//  return str.str ();
return s;
}

/*** TSS 27.10.2000 ***/
static clock_t start_time;
static unsigned int max_seconds;


ofstream Timer::os;

#include "stable.h"

static void
sig_alrm(int)
{
  // check whether too much time has been used
  struct rusage rusage;

  //  extern Stable* computation_program;

  //  computation_program->smodels.dump_status();
  alarm(max_seconds);
  
  //  getrusage (RUSAGE_SELF, &rusage);

  //  cerr << "Alarm at " << rusage.ru_utime.tv_sec << endl;
  
  //  if (rusage.ru_utime.tv_sec >= max_seconds) {
  //    cout << "Maximum search time exceeded. Stopping..." 
  //	 << endl;
  //    cout << "Total time used: " << rusage.ru_utime.tv_sec << "." <<
  //      setw(3) << setfill('0') 
  //	 << rusage.ru_utime.tv_usec/1000  << endl;
  //    exit(0);
  //  } else {
  //    alarm(max_seconds - rusage.ru_utime.tv_sec);
  //  }
  
}

void
Timer::set_alarm(unsigned int s)
{
  start_time = timer;
  max_seconds = s;
  signal(SIGALRM, sig_alrm);
  //  os.open("/tmp/timer");

  // do this in 60 min slots
  //  cerr << "alarm " << s << endl;
  alarm(s);
}


/*** TSS 27.10.2000 ***/
