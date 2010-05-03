// error.h -- debugging and error macros for lparse
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

#ifndef ERROR_H
#define ERROR_H
#include "config.h"

// ERR_USR - error in user input, INT_ERR - internal error, bug in
// code, ERR_SYS - system error
#define USR_ERR		0
#define FATAL_ERR	1
#define SYS_ERR		2

// Warning levels

#define WARN_NONE	0
#define WARN_WEIGHT	(1 << 0)  // problems with weights
//#define WARN_TYPES	(1 << 1)  // check that there are no symbolic
//				  // arguments to functions
#define WARN_ARITY	(1 << 2)  // warn if same predicate symbol has
				  // different arities
#define WARN_EXTENDED	(1 << 3)
#define WARN_SYNTAX	(WARN_WEIGHT | WARN_ARITY | WARN_EXTENDED)


#define WARN_UNSAT	(1 << 4)  // warn if predicates without rules
#define WARN_SIMILAR	(1 << 5)  // warn if both 'Id' and 'id' are used
#define WARN_DOMAIN	(1 << 8)  // warn of invalid #domain statements
#define WARN_TYPO	(WARN_UNSAT | WARN_SIMILAR | WARN_DOMAIN)

#define WARN_LIBRARY	(1 << 6)  // problems with user-defined
				  //functions

#define WARN_ERROR	(1 << 7) // are warnings errors

#define WARN_ALL	~0
#define WARN_INVALID_WARNING (1 << 9)  // warn of invalid #domain statements

#define NUM_WARNINGS	10

void error(int sc, const char *, ...);

void warn(int cause, long lineno, const char *, ...);

int enabled_warning(int cause);

int get_warn_from_string(char *st);
void print_warnings();


enum ErrorType { ERR_ARGUMENT, ERR_INVALID_COMPARE,
		 ERR_DIVIDE_BY_ZERO, ERR_NUMARGS, ERR_RANGE,
		 ERR_OVERFLOW, ERR_NONFIXED_EXTENSION }; 

// Exception class for handling runtime-errors 
class RuntimeError {
public:
  RuntimeError(ErrorType t, long a1 = 0, long a2 = 0);

  ErrorType type;
  long arg1; // what argument was responsible for error
  long arg2; // what argument was responsible for error

  const char *GetErrorMessage();
  void Print();
};

// return string of the form file_name:lineno
char *error_file_and_line(long lineno);


// allows only one argument. There IS NOT and shouldn't be a comma
// before 'fmt'. 
#define int_error( fmt, a ) do { \
 error(USR_ERR, "lparse internal error at %s:%d: " fmt, __FILE__, \
	__LINE__, a); \
  fprintf(stderr, "Please send a bug report to <tommi.syrjanen@hut.fi>\n"); \
  exit(1); \
} while (0)

// wrapper for RuntimeError to use if there are no exceptions on the
// target system.

#include "term.h"
void runtime_error(InternalFunction f, ErrorType t, long a1 = 0, long a2 = 0);

#endif
