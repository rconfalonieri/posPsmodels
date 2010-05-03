// error.cc -- debugging and error macros
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

#include "config.h"
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <string>

#include "error.h"
#include "global.h"
#include "debug.h"
#include "list.h"
#include "symbol.h"
#include "term.h"
#include "predicate.h"

int indent(int amount, ostream &os)
{
  int i;
  for (i = 0; i < amount; i++) {
    os << " ";
  }
  return amount;
}

int get_word(char *source, char* target)
{
  int res = 0;

  while (*source && !isspace(*source)) {
    *target++ = *source++;
    res++;
  }
  *target = '\0';
  return res;
}

void format_message(char *msg, int indent_amount, int indent_first_line,
		    int width)
{
  int pos = 0;
  int column = 0;
  int word_length = 0;
  char *p;
  stringstream ss;
  static char word_buf[BUFFER_LENGTH] = { 0 };

  if (indent_amount > 8) {
    indent_amount = 8;
  }
  
  if (indent_first_line) {
    pos += indent(indent_amount, ss);
    column += indent_amount;
  } 
  p = msg;

  while (*p) {
    word_length = get_word(p, word_buf);

    if (word_length > 0) {
      if (column + word_length <= width) {
        column += word_length +1;
      } else { // newline
        ss << endl;
        pos += indent(indent_amount, ss);
        column = indent_amount + word_length +1;
      }
      ss << word_buf;
      p += word_length;
    } else {
      p++;
    }
  }
  ss << endl;
  strcpy(msg, ss.str().c_str());
}


void error(int level, const char *fmt, ...)  
{
  //  int pos = 0;
  //  char *p = 0;
  //int indent_level = 0;
  va_list(ap);
  va_start(ap, fmt);
  output_varlist(cerr, fmt, ap);
  va_end(ap);

//   // if the error string is really long, suppose that it is
//   // preformatted
//   if (pos > 1024) {
//     fprintf(stderr, buf);
//   } else {
//     int found_colons = 0;
//     p = buf;
    
//     while (*p && found_colons < 2) {
//       indent_level++;
//       if (*p == ':')
// 	found_colons++;
//       p++;
//     }
//     if (found_colons != 2) {
//       indent_level = 4;
//     }
    
//     if (level >= SYS_ERR) {
//       perror(buf);
//     } else {
//       format_message(buf, indent_level+1, false, 76);
//       fprintf(stderr, buf);
//     }

//   }
  if (level > USR_ERR) {
    print_warnings();
    exit(E_ERROR);
  } else
    sys_data.num_errors++;
}

struct warn_label {
  char *label;
  int value;
};
  
static warn_label labels[] =
{
  { "a", WARN_ALL },
  { "all", WARN_ALL },
  { "ar", WARN_ARITY },
  { "arity", WARN_ARITY },
  { "error", WARN_ERROR },
  { "er", WARN_ERROR },
  { "extended", WARN_EXTENDED },
  { "ex", WARN_EXTENDED } ,
  { "l" ,WARN_LIBRARY },
  { "library", WARN_LIBRARY },
  { "si", WARN_SIMILAR },
  { "similar", WARN_SIMILAR },
  { "sy", WARN_SYNTAX },
  { "syntax", WARN_SYNTAX },
  { "typo", WARN_TYPO },
  { "t", WARN_TYPO },
  { "u", WARN_UNSAT },
  { "unsat", WARN_UNSAT },
  { "domain", WARN_DOMAIN },
  { "d", WARN_DOMAIN },
  { "w", WARN_WEIGHT },
  { "weight", WARN_WEIGHT },
  { "n", WARN_NONE },
  { "none", WARN_NONE },
  { "*", WARN_INVALID_WARNING }
};


int get_warn_from_string(char *st)
{
  int i = 0;

  while (strcmp(labels[i].label,st) && (*labels[i].label != '*')) { 
    i++;
  }
  return labels[i].value;
}

int enabled_warning(int cause)
{
  if (cause & sys_data.warnings)
    return 1;
  else
    return 0;
}

void warn(int cause, long lineno, const char *fmt, ...)
{
  ostringstream ss;
  int pos = 0;
  int indent_level = 0;
  if (!(cause & sys_data.warnings))
    return;

  
  if (lineno) {
    output(ss, "%s: %s: ", error_file_and_line(lineno),
		   (sys_data.abort_when_warn) ? "Error" : "Warning");
  } else {
    output(ss, "%s: ", (sys_data.abort_when_warn) ? 
		   "Error" : "Warning");
  }
  indent_level = pos;
  
  va_list(ap);
  va_start(ap, fmt);
  output_varlist(ss, fmt, ap);
  //  pos += vsprintf(&warn_buf[pos], fmt, ap);
  va_end(ap);

//   if (pos >= BUFFER_LENGTH) {
//     output(stderr, "buffer overflow");
//     abort();
//   }
//   warn_buf[pos] = '\0';

//  format_message(ss.str().c_str(), indent_level, false, 76);
  
  warning_list->Insert(clone_string(ss.str().c_str()), lineno);
  
  if (sys_data.abort_when_warn)
    sys_data.num_errors++;
}

// for debugging, prints out all enabled warnings 
void decode_warnings(int w)
{
  output(cerr, "enabled warnings: ");
  if ( w & WARN_WEIGHT) {
    output(cerr, "weight, ");
  }

  if( w & WARN_ARITY ) {
    output(cerr, "arity, " );
  }

  if( w & WARN_UNSAT ) {
    output(cerr, "unsat, " );
  }

  if( w & WARN_SIMILAR ) {
    output(cerr, "similar, " );
  }

  //  if( w & WARN_TYPO ) {
  //    output(cerr, "typo, " );
  //  }

  if (w & WARN_LIBRARY) {
    output(cerr, "library, " );
  }

  output(cerr, "\n");
}

void print_warnings()
{
  char *st = NULL;

  while ((st = warning_list->Iterate())) {
    output(cerr, "%s\n", st);
  }
  warning_list->Clear();
}

  
RuntimeError::RuntimeError(ErrorType t, long a1, long a2)
{
  type = t;
  arg1 = a1;
  arg2 = a2;
}

static const char *error_messages[] = {
  "invalid argument type",
  "comparison of incompatible types:",
  "divide by zero",
  "incorrect number of arguments",
  "argument out of range",
  "result overflow",
  "trying to find the size of the extension of a non-domain predicate"
};

const char *RuntimeError::GetErrorMessage()
{
  return error_messages[type];
}

void print_argument(long x)
{
  if (IS_NUMBER(x)) {
    output(cerr, "%ld", GET_VALUE(x));
  } else {
    char *st = constant_table->LookupByValue(x);

    if (!st)
      int_error("missing constant", "");

    output(cerr, "%s", st);
  }
}

void RuntimeError::Print()
{
  output(cerr, "%s ", GetErrorMessage());

  switch (type) {
  case ERR_ARGUMENT:
    output(cerr, "for argument '");
    print_argument(arg1);
    output(cerr, "'");
    break;
  case ERR_INVALID_COMPARE:
    output(cerr, "'");
    print_argument(arg1);
    output(cerr, "' and '");
    print_argument(arg2);
    output(cerr, "' can't be compared");
    break;
  case ERR_DIVIDE_BY_ZERO:
  case ERR_OVERFLOW:
    break;
  case ERR_NUMARGS:
    output(cerr, "- excepting %ld arguments.", arg1);
    break;
  case ERR_RANGE:
    output(cerr, "- for argument `%ld'", arg1);
    break;
  case ERR_NONFIXED_EXTENSION:
    output(cerr, " of the predicate '%s'",
	    predicates[arg1]->Name());
    break;
  default:
    int_error("invalid error type", "");
  }
  output(cerr, "\n");
}

void runtime_error(InternalFunction fun, ErrorType t, long a1,
		   long a2)  
{
  RuntimeError rt(t, a1, a2);

#ifdef CXX_HAS_NO_EXCEPTIONS
  rt.Print();
  output(cerr, " in function '%s'\n", function_strings[fun]);
  
  exit(1);
#else
  throw rt;
#endif
}

char *error_file_and_line(long lineno)
{
  ostringstream os;
  if (lineno == 0) {
    return "";
  }
  int file_number = sys_data.num_input_files -1;
  
  if (sys_data.num_input_files <= 1) {
    os << lineno;
  } else {
    while (sys_data.file_start_lines[file_number] > lineno)
      file_number--;
    
    output(os, "%s:%ld", sys_data.input_files[file_number],
	    lineno-sys_data.file_start_lines[file_number]+1);
  }
  return clone_string(os.str().c_str());
}
