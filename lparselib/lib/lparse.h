/* lparse.h -- definitions for a lparse API
 * Copyright (C) 1999-2000 Tommi Syrjänen <Tommi.Syrjanen@hut.fi>
 *  
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *  
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *  
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

/*
 * This file presents prototypes for a small set of functions that can
 * be called from lparse user-defined functions.
 */

#ifdef __cplusplus
extern "C" {
#endif


/* When a function is called during grounding the arguments to
   functions are already instantiated. There are two different types
   of constants, numbers and symbolic constants. Symbolic constants
   are represented as indices to a symbol table. If you try to perform
   arithmetic with those indices, the most probable result is a
   segmentation fault.

   The symbolic functions are internally treated as symbolic constants
   that happen to have a specific form. Currently, the library does
   not provide functions for manipulating symbolic functions, but the
   file api_example.c in the examples directory of lparse contains a
   short example how they can be detected. */

/* The enumeration lparse_constant_t holds the possible types of
   lparse constants. */
typedef enum { LP_NUMERIC, LP_SYMBOLIC } lparse_constant_t;

/* The constant ERROR_NUMBER is 
 */
#define LP_INVALID_CONSTANT -2147483647L


/* Given a lparse constant, the function lparse_constant_type(long)
   returns the type of the constant. */
lparse_constant_t lparse_constant_type(long constant);

/* Function lparse_is_numeric(long) returns true if its argument is a
   numeric constant */ 
int lparse_is_numeric(long constant);

/* Function lparse_is_symbolic(long) returns true if its argument is a
   symbolic constant. */
int lparse_is_symbolic(long constant);

/* Function lparse_get_symbolic_constant_name(long arg) fetches the
   character string of a constant from the symbol table. Returns NULL
   if the index is faulty. The pointer is to the actual symbol table
   entry, so don't mess with it */
char *lparse_get_symbolic_constant_name(long symbolic_constant);

/* Function lparse_get_symbolic_constant_index(char *) returns the
   symbol table index of its argument or value INVALID_CONSTANT if the
   constant doesn't exist */
long lparse_get_symbolic_constant_index(char *symbolic_constant);

/* Function lparse_symbol_exists(char*) returns true if its argument
   string is defined as a constant in the program. */
int lparse_symbol_exists(char *symbolic_constant);

/* Function lparse_create_new_symbolic_constant(char*) stores its
   argument into the symbol table and returns its index. It is safe to
   add same constant many times to the table and all calls will return
   the same index */
long lparse_create_new_symbolic_constant(char *new_string);

#ifdef __cplusplus
}
#endif




