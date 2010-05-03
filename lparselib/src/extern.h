// extern.h -- external variable declarations for lparse
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

#ifndef EXTERN_H
#define EXTERN_H


class SymbolTable;
class FunctionTable;
struct ComputeStmt;
struct ParseNode;
class PredicateArray;

class LongList;
class Graph;
class Literal;
class LiteralList;
class ParseNodeList;
class HashSet; // ugly
class RuleList;
class ResizableArray;
class LibraryList;
class StringList;
struct Weight; 
extern Literal *false_lit;
extern ParseNode *logic_program;
extern FunctionTable *function_table;
extern SymbolTable *predicate_table;
extern SymbolTable *variable_table;
extern SymbolTable *constant_table;
extern SymbolTable *numeric_constant_table;
extern SymbolTable *atom_table;
extern SymbolTable *warnings_table;

extern SymbolTable *priority_atoms;

//extern int errno;
extern long primes[];
extern SysData sys_data;
extern char *operators[];
extern char *function_strings[];
extern ComputeStmt *compute;
extern RuleList *optimize;

extern char *parse_strings[];
extern Graph *dependency_graph;
extern Graph *scc_graph;
extern PredicateArray predicates;
extern Variable *variables;
extern Variable *var_pos;
extern ParseNode *c_stmt;
extern ParseNodeList *special_rules;
extern ParseNodeList *weight_declarations;
extern HashSet *condition_set;
extern ResizableArray *condition_weights;
extern LibraryList *user_libraries;
extern StringList *warning_list;
extern Weight *default_weight;

extern LongList *command_line_constants;

extern LiteralList *implicit_domain_list;

unsigned long hash(const char *key, unsigned long length,
		   unsigned long initval);

// replacement of strdup that is not standard
char *clone_string(const char *st);

#if !HAVE_STRTOL
long int strtol(const char *nptr, char **endptr, int base);
#endif

int output(ostream& os, const char *str, ...);
int output(ostream* os, const char *str, ...);
int output_varlist(ostream& os, const char *str, va_list args);
int output_varlist(ostream* os, const char *str, va_list args);


#endif
