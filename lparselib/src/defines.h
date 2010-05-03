// defines.h -- global definitions
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

#ifndef DEFINE_H
#define DEFINE_H

#include <limits.h>
#include <sstream>
using namespace std;
// Configurable parameters

//initial size of symbol tables 
#define SYMBOL_DEFAULT_SIZE	11
#define ATOM_DEFAULT_SIZE	10

// Domain sizes
#define DOMAIN_DEFAULT_SIZE	11
#define SMALL_DOMAIN_SIZE	3
#define DOMAIN_MIN_SIZE		20
#define DOMAIN_MAX_SIZE		9000000


#define RC_FILE			".lparserc"
#define LPARSE_LIBRARIES	"LPARSE_LIBRARIES"
#define LPARSE_LIBRARY_PATH	"LPARSE_LIBRARY_PATH"

// Maximum number of ranges
#define RANGE_MAX_NUMBER	INT_MAX
// the length of string to hold the names of the ranges
#define RANGE_MAX_NAME		14


#define MAX_PROBE		50 // for debugging hash tables


#define MAX_NUMBER_OF_FUNCTION_ARGUMENTS	64


#define BUFFER_LENGTH	4096

// Unconfigurable constants. These shouldn't be modified without
// checking results beforehand
#define MAX_NUMBER		(1 << 30)
#define ERROR_NUMBER		-2147483647L

// System definitions. Don't mess with these unless you know what you
// are doing. 
typedef long Instance;
typedef long Variable;

// Since there are two types of instances, constants and numbers, the
// numbers all have the 30th bit set as 1. (Yes, this effectively
// restricts the range of possible numbers to 0- (2^30-1). However, I 
// don't think this limit is reached in practise very often... 
#define NUMBER_BIT	(1 << 30)
#define IS_NUMBER(x)	( (x) < NUMBER_BIT && (x) > -NUMBER_BIT)
#define IS_CONSTANT(x)	(  ! IS_NUMBER((x)) )
#define GET_CONSTANT(x) ( (x) ^ NUMBER_BIT )
#define MAKE_CONSTANT(x) ( (x) | NUMBER_BIT)
#define MAKE_NUMBER(x)	( (x) ) // | NUMBER_BIT )
#define GET_VALUE(x)	( (x) ) //^ NUMBER_BIT )
#define max(x, y) ( ( (x) < (y)) ? (y) : (x))
typedef Instance iFunc(int, long *);
typedef iFunc *InstFunc;
extern InstFunc internal_functions[];


#define FALSE_NAME	"_false"
#define INCONSISTENT_NAME "INCONSISTENT"
#define PRIORITY_PREDICATE "prefer"
#define PRIORITY_TCL	"prefer'"

// possible exit values
#define E_OK	0
#define E_RANGE	1
#define E_ERROR	2

enum PrintDomain { PR_ALL, PR_HEADS, PR_NONE, PR_POSITIVE };
enum RestrictType { RT_UNKNOWN, RT_NONE, RT_WEAK, RT_STRONG };
enum DomainType { DM_UNKNOWN = -1, DM_FALSE, DM_TRUE, DM_DOMAIN,
		  DM_INTERNAL, DM_SPECIAL };

enum RuleType { EMPTYRULE, BASICRULE, CONSTRAINTRULE, CHOICERULE,
				 GENERATERULE, WEIGHTRULE, OPTIMIZERULE, ORDERED_DISJUNCTION,
				 DISJUNCTIVERULE };

enum RegularLevel { REGULAR_NONE,
		    REGULAR_NO_CONSTRAINTS,
		    REGULAR_NORMAL,
		    REGULAR_ALL_CONSTRAINTS,
		    REGULAR_ERROR };


// different possibilities of grounding

// ground a non-domain predicate, and emit the result
#define GRD_RULE	1
// ground a domain predicate, and add the result as instances of it
#define GRD_DOMAIN	2
// ground a condition, and add all insances of it to global condition
// instanceset. 
#define GRD_CONDITION	3

// ground a transitive domain predicate
#define GRD_TRANSITIVE_DOMAIN	4

#define GLOBAL_BIT		(1 << 6)
#define R_NONE		     	-1
#define R_WEIGHT_LOCAL	     	0
#define R_CONDITION_LOCAL    	1
#define R_NEGATIVE_LOCAL     	2
#define R_NC_LOCAL	 	3
#define R_POSITIVE_LOCAL 	4
#define R_ASSIGNED_LOCAL 	5
#define R_DOMAIN_LOCAL	 	6
#define R_CONDITIONED		7

#define R_WEIGHT		(R_WEIGHT_LOCAL | GLOBAL_BIT)
#define R_NEGATIVE		(R_NEGATIVE_LOCAL | GLOBAL_BIT)
#define R_NC			(R_NC_LOCAL | GLOBAL_BIT)
#define R_POSITIVE		(R_POSITIVE_LOCAL | GLOBAL_BIT)
#define R_DOMAIN		(R_DOMAIN_LOCAL | GLOBAL_BIT)
#define R_CONDITION		(R_CONDITION_LOCAL | GLOBAL_BIT)

#define R_FUNCTION_FAIL		(7 | GLOBAL_BIT)
#define R_ASSIGNED		(R_ASSIGNED_LOCAL | GLOBAL_BIT)


#define GLOBAL_VARIABLE(x) 	( ((x) >= 0) && ((x) & GLOBAL_BIT))
#define MAKE_LOCAL(x)		( (x) & ~GLOBAL_BIT)
#define MAKE_GLOBAL(x)		( (x) | GLOBAL_BIT)

#define VAR_NONE	ERROR_NUMBER
#define VAR_WEIGHT	0
#define VAR_LOCAL	1
#define VAR_GLOBAL	2
#define VAR_LITERAL	3
#define VAR_CONDITION	4
#define VAR_FREE	5
#define VAR_SEMI	6


#define COND_ERROR	-1
#define COND_LOCAL	0
#define COND_GLOBAL	1

// system data structure
struct SysData
{
  ostream *output_file; 
  int emit_text; //is output plain text
  int output_version;
  int abort_when_warn; // treat warnings as errors
  int warnings; // mask for warnings
  int num_errors; // how many errors are found
  int internal_functions; // are internal functions enabled
  int dlp_semantics; // controls interpretation of 'a | b.'
  PrintDomain print_domains; // which domains are printed
  long domain_size;
  long max_number; // biggest integer used in the program
  long ground_atoms; // how many different grounded atoms
  long ground_rules; // how many grounded rules
  long ground_sigma; // how many grounded atoms in total
  long internal_atoms; // how many internal atoms (special rule heads)
  			// have been generated
  long internal_variables; // same for variables
  int num_ranges; // how many ranges are defined
  int num_functions; // how many functions are called
  int unknown_vars; // how many variables will be generated by special
                    // rules
  int total_vars; // how many variables there will be in the program 
  int has_negs;
  int print_registered;
  int has_implicit_domains;
  int print_rules;
  int force_small_domains;
  int term_max_arity;
  long default_weight;
  int use_regular_models; // true if regular model expansion is in
		          // use, see lparse.texi
  int allow_only_disjunctive_rules;
  int num_input_files; // how many input files there are
  int true_negation; // use classical negation
  int inconsistent_answers; // are inconsistent answer sets reported?
  int totalize_negations; // force either -a or a to be in the model.
  int drop_quotes; // should quotation marks be left out
  int hide_all;
  int default_negative_weight_to_positive; // should 'not a' use 'a''s weight
  int use_priorities;
  int use_interactive_priorities;
  int normal_output;
  int print_whole_symbol_table;
  int check_tightness;
  int is_tight;
  int strong_typing;
  int output_graph;
  int only_facts;
  int print_debug;
  int print_empty_choices;
  char **input_files; // names of the files
  long *file_start_lines; // what lines do the lines start from

  char *atom_output_file;
  
  RegularLevel regular_level; // for regular model semantics

  ~SysData();
};

// these control variable substitutions during unification
#define SUB_NONE	ERROR_NUMBER
#define SUB_CONSTANT	0
#define SUB_VARIABLE	1

// if there is no weight defined for a literal, use this
#define DEFAULT_WEIGHT	1

// find out the correct type for 32-bit integers
#if SIZEOF_LONG == 4
typedef unsigned long u4;
#else
typedef unsigned int u4;
#endif

#endif
