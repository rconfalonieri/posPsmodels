// extern.cc -- global variable instantiation
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


#ifndef GLOBAL_H
#include "global.h"
#endif

ParseNode *logic_program;
ParseNode *c_stmt = NULL;
FunctionTable *function_table;
SymbolTable *predicate_table;
SymbolTable *variable_table;
SymbolTable *constant_table;
SymbolTable *numeric_constant_table;
SymbolTable *atom_table;
SymbolTable *warnings_table; // stores warnings about similar identifiers

SymbolTable *priority_atoms;

SysData sys_data;
ComputeStmt *compute;
Graph *dependency_graph;
Graph *scc_graph;

Variable *variables;
ParseNodeList *special_rules;
ParseNodeList *weight_declarations;
RuleList *optimize;
ResizableArray *condition_weights;
LongList *command_line_constants;

class LiteralList;
LiteralList *implicit_domain_list;

#include "instance.h"
InstanceSet *condition_set;
class LibraryList;
LibraryList *user_libraries;
StringList *warning_list;

Weight *default_weight;

#include "error.h"
#include <string.h>
#include <stdlib.h>

char *clone_string(const char *st)
{
  char *new_st = new char[strlen(st)+1];
  if (!new_st)
    error(SYS_ERR, "malloc error");
  strcpy(new_st, st);
  return new_st;
}

// #if !HAVE_STRTOL
// #include <stdlib.h>
// #include <string.h>
// #include <ctype.h>
// #include "error.h"
// // this works only for base 10 and NULL endptr 
// long int strtol(const char *nptr, char **endptr, int base)
// {
//   int i = 0, pos = 0;
//   long result = 0;

//   if ((base != 10) || endptr)
//     int_error("incomplete strtol implementation");

//   while (nptr[pos] != '\0') {
//     if (isdigit(nptr[pos])) {
//       result *= 10;
//       result += nptr[pos] - '0';
//     } else {
//       result = 0;
//       break;
//     }
//     pos++;
//   }
//   return result;
// }

// #endif

SysData::~SysData()
{
  int i;
  if (input_files) {
    for (i = 0; i < num_input_files; i++) {
      delete [] input_files[i];
    }
    delete [] input_files;
  }
  if (file_start_lines) {
    delete [] file_start_lines;
  }

  if (atom_output_file)
    delete [] atom_output_file;
  
}
