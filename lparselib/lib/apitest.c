
/* apitest.c -- a small example how the function API can be used */

#include "lparse.h"
#include <string.h>

/* This function returns the symbolic constant 'true' if its first
   argument is a symbolic function, and 'false' otherwise */
long is_symbolic_function(int nargs, long *args)
{
  char *st = 0;
  
  if (lparse_is_symbolic(args[0])) {
    st = lparse_get_symbolic_constant_name(args[0]);

    /* supposes that there is a '(' in a constant only if it actually is a
       symbolic function */
    if (strstr(st, "(")) {    
      return lparse_create_new_symbolic_constant("true");
    }
  }
  return lparse_create_new_symbolic_constant("false");
}
