// library.cc -- internally defined functions.
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

#include <ctype.h>
#include <stdlib.h>

#include "../lib/lparse.h"

#include "config.h"
#ifdef HAVE_LIBDL
#include <dlfcn.h>
#endif


#ifndef LIBRARY_H
#include "library.h"
#endif
#ifndef SYMBOL_H
#include "symbol.h"
#endif
#ifndef ERROR_H
#include "error.h"
#endif
#ifndef GLOBAL_H
#include "global.h"
#endif
#include "term.h"

#include "predicate.h"

#include <string.h>

// check whether symbol 'key' is defined in library. 
InstFunc LibraryNode::FindSymbol(char *key)
{
  InstFunc fun = NULL;

#if HAVE_LIBDL
  if (!loaded) { // load this first
    handle = dlopen(lib, RTLD_LAZY);
    if (!handle) {
      error(SYS_ERR, "cannot open library file '%s'", lib);
      return NULL;
    }
    loaded = 1;
  }

  fun = (InstFunc) dlsym(handle, key);

  return fun; // return it, null or no
#elif defined MS_WINDOWS
  if (!loaded) {
    handle = LoadLibrary("lparse.dll");
       if (!handle) {
      error(SYS_ERR, "cannot open library file '%s'", lib);
      return NULL;
    }
    loaded = 1;
  } 
  fun = (InstFunc) GetProcAddress(handle, key);
#else // no dynamic libraries
  warn(WARN_ALL, 0, "Current system architecture doesn't have dl-library which "
       "is needed for user-defined functions.");
  return NULL;
#endif
}

LibraryNode::~LibraryNode()
{
#if HAVE_LIBDL
  if (handle)
    dlclose(handle);
#elif  defined MS_WINDOWS
  if (handle)
    FreeLibrary(handle);
#endif
}

// the format of the declarations is :
//   long foo(int nargs, ...); /* bar.cc */
/* *** INTERNAL LIBRARY FUNCTION DECLARATIONS  *** */
long int_plus(int nargs, long *args);
long int_eq(int nargs, long *args);
long int_assign(int nargs, long *args);
long int_minus(int nargs, long *args);
long int_abs(int nargs, long *args);
long int_le(int nargs, long *args);
long int_ge(int nargs, long *args);
long int_gt(int nargs, long *args);
long int_lt(int nargs, long *args);
long int_neq(int nargs, long *args);
long int_times(int nargs, long *args);
long int_div(int nargs, long *args);
long int_mod(int nargs, long *args);
long int_weight(int nargs, long *args); // a dummy
long int_norm(int nargs, long *args); 
long int_exp(int nargs, long *args); 


long int_and(int nargs, long *args);
long int_or(int nargs, long *args);
long int_xor(int nargs, long *args);
long int_not(int nargs, long *args);

InstFunc internal_functions[] = {
  0, int_abs, int_eq, int_le, int_ge, int_lt,
  int_gt, int_neq, int_plus, int_minus,
  int_times, int_div, int_mod, int_assign,
  int_and, int_or, int_not, int_xor, int_weight, int_norm, int_exp
};

/* comparison functions for string constants */
long le_string(int nargs, long *args);
long ge_string(int nargs, long *args);
long lt_string(int nargs, long *args);
long gt_string(int nargs, long *args);


int register_functions()
{
  int i = sys_data.internal_functions;
  // Internal functions should be automatically be valid
  function_table->Register("plus", int_plus, i);
  function_table->Register("eq", int_eq, i);
  function_table->Register("assign", int_assign, i);
  function_table->Register("minus", int_minus, i);
  function_table->Register("abs", int_abs, i);
  function_table->Register("le", int_le, i);
  function_table->Register("lt", int_lt, i);
  function_table->Register("ge", int_ge, i);
  function_table->Register("gt", int_gt, i);
  function_table->Register("neq", int_neq, i);
  function_table->Register("times", int_times, i);
  function_table->Register("div", int_div, i);
  function_table->Register("mod", int_mod, i);
  function_table->Register("and", int_and, i);
  function_table->Register("or", int_or, i);
  function_table->Register("xor", int_xor, i);
  function_table->Register("not", int_not, i);
  function_table->Register("weight", int_weight, i);
  function_table->Register("norm", int_norm, i);
  function_table->Register("exp", int_exp, i);
    
  return 1;
}

/* *** INTERNAL LIBRARY FUNCTIONS START HERE *** */
long int_plus(int nargs, long *args )
{
  int i = 0;
  long result = 0;

  for (i=0; i < nargs; i++) {
    if (IS_CONSTANT(args[i])) 
      runtime_error(FUN_PLUS, ERR_ARGUMENT, args[i]);

    result += args[i];
  }

  return result;
}

long int_exp(int nargs, long *args )
{
  if (nargs != 2) {
    runtime_error(FUN_EXP, ERR_NUMARGS, 2);
  }
  long base = args[0];
  long power = args[1];
  long result = 1;
  
  if (IS_CONSTANT(args[0])) 
    runtime_error(FUN_EXP, ERR_ARGUMENT, args[0]);

  if (IS_CONSTANT(args[1])) 
    runtime_error(FUN_EXP, ERR_ARGUMENT, args[1]);

  if (power < 0)
    runtime_error(FUN_EXP, ERR_ARGUMENT, args[1]);
  
  while (power--) {
    result *= base;
    if (IS_CONSTANT(result))
      runtime_error(FUN_EXP, ERR_OVERFLOW, 0);
  }
  return result;
}

long int_norm(int nargs, long *args)
{
  if (nargs != 1) {
    runtime_error(FUN_NORM, ERR_NUMARGS, 1);
  }
  
  if (args[0] < 0 || args[0] > predicate_table->Size())
    runtime_error(FUN_NORM, ERR_RANGE, args[0]);

  long p = args[0];

  if (!predicates[p]->DomainPredicate())
    runtime_error(FUN_NORM, ERR_NONFIXED_EXTENSION, args[0]);
  
  if (predicates[p]->atoms)
    return predicates[p]->atoms->Size();

  if (predicates[p]->follows)
    return 1;
  else
    return 0;
}


long int_eq(int nargs, long *args)
{
  long result = 1;
  int i = 0;
  long first = 0;
  
  first = args[0];

  for (i = 1; i < nargs; i++) { // '1' intentional
    if (args[i] != first) {
      result = 0;
    }
  }
  return result;
}

long int_assign(int, long *)
{
  // dummy function, won't be called
  return 1;
}

long int_weight(int, long *)
{
  // dummy function, won't be called
  return 1;
}

long int_minus(int nargs, long *args)
{
  long result = 0;
  int i = 0;
  
  result = args[0];

  if (IS_CONSTANT(result)) 
    runtime_error(FUN_MINUS, ERR_ARGUMENT, args[i]);
 
  for (i = 1; i < nargs; i++) { // '1' intentional
    if (IS_CONSTANT(args[i])) 
      runtime_error(FUN_MINUS, ERR_ARGUMENT, args[i]);

    result -= args[i];
  }

  return result;
}
  
long int_abs(int, long *args)
{
  long result = 0;

  result = args[0];

  if (IS_CONSTANT(result)) 
    runtime_error(FUN_ABS, ERR_ARGUMENT, result);
      
  
  if (result < 0)
    result = -result;
  
  return result;
}

long int_le(int nargs, long *args)
{
  long result = 1;
  long prior_value = 0;
  int i = 0;

  if (IS_CONSTANT(args[0])) {
    return le_string(nargs, args);
  }
  
  prior_value = args[0];

  for (i = 1 ; i < nargs; i++ ) { // '1' intentional
    if (IS_CONSTANT(args[i]))
      runtime_error(FUN_LE, ERR_INVALID_COMPARE, args[0], args[i]);
    
    if (args[i] < prior_value) {
      result = 0;
    } else {
      prior_value = args[i];
    }
  }  
  
  return result;
}

long int_ge(int nargs, long *args)
{
  long result = 1;
  long prior_value = args[0];
  int i = 0;

  if (IS_CONSTANT(args[0])) {
    return ge_string(nargs, args);
  }
  
  for (i = 1 ; i < nargs; i++ ) { // '1' intentional
    if (IS_CONSTANT(args[i]))
      runtime_error(FUN_GE, ERR_INVALID_COMPARE, args[0],args[i]);

    
    if (args[i] > prior_value) {
      result = 0;
    } else {
      prior_value = args[i];
    }
  }  
  
  return result;
}

long int_gt(int nargs, long *args)
{
  long result = 1;
  long prior_value = args[0];
  int i = 0;
  
  if (IS_CONSTANT(args[0])) {
    return gt_string(nargs, args);
  }
  
  for (i = 1 ; i < nargs; i++ ) { // '1' intentional
    if (IS_CONSTANT(args[i]))
      runtime_error(FUN_GT, ERR_INVALID_COMPARE, args[0],args[i]);

    if (args[i] >= prior_value) {
      result = 0;
    } else {
      prior_value = args[i];
    }
  }  
  
  return result;
}

long int_lt(int nargs, long *args)
{
  long result = 1;
  long prior_value = args[0];
  int i = 0;

  if (IS_CONSTANT(args[0])) {
    return lt_string(nargs, args);
  }
  
  for (i = 1 ; i < nargs; i++ ) { // '1' intentional
    if (IS_CONSTANT(args[i]))
      runtime_error(FUN_LT, ERR_INVALID_COMPARE, args[0], args[i]);

    if (args[i] <= prior_value) {
      result = 0;
    } else {
      prior_value = args[i];
    }
  }  
  
  return result;
}

long int_neq(int nargs, long *args)
{
  long result = 1;
  int i = 0;
  int prior_value = args[0];

  for (i = 1; i < nargs; i++) { // '1' intentional
    if (args[i] == prior_value) {
      result = 0;
    } else {
      prior_value = args[i];
    }
  }

  return result;
}

long int_times(int nargs, long *args)
{
  long result = 1;
  int i = 0;

  if (IS_CONSTANT(result)) 
    runtime_error(FUN_TIMES, ERR_ARGUMENT, args[i]);

  for (i = 0; i < nargs; i++) {
    if (IS_CONSTANT(args[i])) 
      runtime_error(FUN_TIMES, ERR_ARGUMENT, args[i]);

    result *= args[i];
  }
  return result;
}

long int_div(int nargs, long *args)
{
  long result = args[0];
  int i = 0;

  if (IS_CONSTANT(result)) 
    runtime_error(FUN_DIV, ERR_ARGUMENT, args[i]);

  for (i = 1; i < nargs; i++) { // '1' intentional
    if (IS_CONSTANT(args[i])) 
      runtime_error(FUN_DIV,ERR_ARGUMENT, args[i]);
    
    if (args[i] == 0) {
      runtime_error(FUN_DIV,ERR_DIVIDE_BY_ZERO, 0); 
    } else {
      result /= args[i];
    }
  }
  return result;
}

long int_mod(int nargs, long *args)
{
  long result = args[0];
  int i = 0;

  if (IS_CONSTANT(result)) 
    runtime_error(FUN_MOD, ERR_ARGUMENT, args[i]);

  for (i = 1; i < nargs; i++) { // '1' intentional
    if (IS_CONSTANT(args[i])) 
      runtime_error(FUN_MOD, ERR_ARGUMENT, args[i]);

    if (args[i] == 0) {
      runtime_error(FUN_MOD, ERR_DIVIDE_BY_ZERO, 0); 
    } else {
      result %= args[i];
      if (result < 0)
	result += args[i];
    }
  }
  return result;
}

long int_and(int nargs, long *args)
{
  long result = args[0];
  int i = 0;

  if (IS_CONSTANT(result)) 
    runtime_error(FUN_AND, ERR_ARGUMENT, args[i]);
  
  for (i = 1; i < nargs; i++) { // '1' intentional
    if (IS_CONSTANT(args[i])) 
      runtime_error(FUN_AND, ERR_ARGUMENT, args[i]);
    
    result &= args[i];
  }
  return result;
}

long int_or(int nargs, long *args)
{
  long result = args[0];
  int i = 0;

  if (IS_CONSTANT(result)) 
    runtime_error(FUN_OR, ERR_ARGUMENT, args[i]);

  for (i = 1; i < nargs; i++) { // '1' intentional
    if (IS_CONSTANT(args[i])) 
      runtime_error(FUN_OR, ERR_ARGUMENT, args[i]);

    result |= args[i];
  }
  return result;
}

long int_xor(int nargs, long *args)
{
  long result = args[0];
  int i = 0;

  if (IS_CONSTANT(result)) 
    runtime_error(FUN_XOR, ERR_ARGUMENT, args[i]);

  for (i = 1; i < nargs; i++) { // '1' intentional
    if (IS_CONSTANT(args[i])) 
      runtime_error(FUN_XOR, ERR_ARGUMENT, args[i]);

    result ^= args[i];
  }
  return result;
}


long int_not(int, long *args)
{
  long result = args[0];

  if (IS_CONSTANT(result)) 
    runtime_error(FUN_NOT, ERR_ARGUMENT, result);
  
  result = ~result;
  return result;
}

long lt_string(int nargs, long *args)
{
  char *first = NULL;
  char *second = NULL;
  long result = 1;
  
  first = constant_table->LookupByValue(args[0]);

  if (!first)
    int_error("expecting a string", "");

  for (int i = 1; i < nargs; i++) {
    second = constant_table->LookupByValue(args[i]);
    if (!second)
      runtime_error(FUN_LT, ERR_INVALID_COMPARE, args[0], args[i]);

    if (strcmp(first, second) >= 0) {
      result = 0;
    }
    first = second;
  }
  return result;
}

long le_string(int nargs, long *args)
{
  char *first = NULL;
  char *second = NULL;
  long result = 1;
  
  first = constant_table->LookupByValue(args[0]);

  if (!first)
    int_error("expecting a string", "");

  for (int i = 1; i < nargs; i++) {
    second = constant_table->LookupByValue(args[i]);
    if (!second)
      runtime_error(FUN_LE, ERR_INVALID_COMPARE,args[0], args[i]);

    if (strcmp(first, second) > 0) {
      result = 0;
    }
    first = second;
  }
  return result;
}


long gt_string(int nargs, long *args)
{
  char *first = NULL;
  char *second = NULL;
  long result = 1;
  
  first = constant_table->LookupByValue(args[0]);

  if (!first)
    int_error("expecting a string", "");

  for (int i = 1; i < nargs; i++) {
    second = constant_table->LookupByValue(args[i]);
    if (!second)
      runtime_error(FUN_GT, ERR_INVALID_COMPARE, args[0],args[i]);

    if (strcmp(first, second) <= 0) {
      result = 0;
    }
    first = second;
  }
  return result;
}


long ge_string(int nargs, long *args)
{
  char *first = NULL;
  char *second = NULL;
  long result = 1;
  
  first = constant_table->LookupByValue(args[0]);

  if (!first)
    int_error("expecting a string", "");

  for (int i = 1; i < nargs; i++) {
    second = constant_table->LookupByValue(args[i]);
    if (!second)
      runtime_error(FUN_GE, ERR_INVALID_COMPARE,args[0], args[i]);

    if (strcmp(first, second) < 0) {
      result = 0;
    }
    first = second;
  }
  return result;
}


/* *** INTERNAL LIBRARY FUNCTIONS END HERE *** */


/** LPARSE API-functions are here */

extern "C" {

lparse_constant_t lparse_constant_type(long arg)
{
  if (IS_CONSTANT(arg)) 
    return LP_SYMBOLIC;
  else
    return LP_NUMERIC;
}

int lparse_is_numeric(long arg)
{
  return !IS_CONSTANT(arg);
}

int lparse_is_symbolic(long arg)
{
  return IS_CONSTANT(arg);
}

char *lparse_get_symbolic_constant_name(long arg)
{
  char *result = 0;
  if (!IS_CONSTANT(arg)) {
    error(FATAL_ERR, "function 'lparse_get_symbolic_constant(long arg)' was "
	  "called with an argument '%ld' that doesn't correspond to a "
	  "symbolic constant.\n");
  }
  result = constant_table->LookupByValue(arg);
  return result;
}

long lparse_get_symbolic_constant_index(char *str)
{
  return constant_table->Lookup(str);
}

int lparse_symbol_exists(char *str)
{
  if (constant_table->Lookup(str) > ERROR_NUMBER)
    return 1;
  else
    return 0;
}
  

long lparse_create_new_symbolic_constant(char *str)
{
  return constant_table->Insert(str);
}




} /* end extern "C" */
