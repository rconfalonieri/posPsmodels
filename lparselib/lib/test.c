/* simple file to test Makefile */

#include <stdarg.h>

long test_plus(int nargs, long *args)
{
  long result = 0;
  int i = 0;

  for (i =0; i < nargs; i++) {
    result += args[i];
  }

  return result;
}

