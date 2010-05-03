/*
** lex.l -- lexer for lparse
**
** Copyright (C) 1999-2007 Tommi Syrjänen <Tommi.Syrjanen@hut.fi>
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**  
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**  
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**  
*/

%{
#include "config.h"
#include "parsetree.h"
#include "symbol.h"
#include "parse.h"
#include "error.h"
#include <string.h>

extern int lineno;
extern int return_symbol;
extern int return_funcsymbol;


int par_depth = 0;
int inside_definition = 0;
int newline_read = 0;

int next_input_file()
{
  static FILE *yyin = 0;
  static int current_file = 0;

  if (!newline_read)
    lineno++;
  
  if ( current_file < sys_data.num_input_files) {
    if ( yyin ) { fclose(yyin); }
    yyin = fopen(sys_data.input_files[current_file], "r");
    if (!yyin) {
      error(SYS_ERR, "cannot open input file %s",
	    sys_data.input_files[current_file++]); 
    }
    yy_switch_to_buffer( yy_create_buffer( yyin, YY_BUF_SIZE));
    sys_data.file_start_lines[current_file++] = lineno;
  } else {
    return 0;
  }
  return 1;
}

%}


%s PAREN
%s DEFINITION
%option noyywrap
%option never-interactive

IF		:-
NOT		[Nn][Oo][Tt]
WEIGHT		#?[Ww][Ee][Ii][Gg][Hh][Tt]
EXTERNAL	#?[Ee][Xx][Tt][Ee][Rr][Nn][Aa][Ll]
SHOW		#?[Ss][Hh][Oo][Ww]
COMPUTE		#?[Cc][Oo][Mm][Pp][Uu][Tt][Ee]
MINIMIZE	#?[Mm][Ii][Nn][Ii][Mm][Ii][Zz][Ee]
MAXIMIZE	#?[Mm][Aa][Xx][Ii][Mm][Ii][Zz][Ee]
FUNCTION	#?[Ff][Uu][Nn][Cc][Tt][Ii][Oo][Nn]
CONST		#?[Cc][Oo][Nn][Ss][Tt]
FUNCSYMBOL	[a-z_][a-zA-Z0-9_]*
IDENTIFIER	-?[a-z_][a-zA-Z0-9_']*
VARIABLE	[A-Z][a-zA-Z0-9_]*
STRING		\"[^\"\n]*\"
NUMBER		[0-9][0-9]*
COMMENT		%.*\n
COMMENT2	%.*
WS		[\t\r ]*
NL		\n
OPTION		#[Oo][Pp][Tt][Ii][Oo][Nn].*
VARIABLEDEF	#[Dd][Oo][Mm][Aa][Ii][Nn]
TYPE    	#[Tt][Yy][Pp][Ee]
PREDICATE	#[Pp][Rr][Ee][Dd][Ii][Cc][Aa][Tt][Ee]
NONDOMAIN   #[Nn][Oo][Nn][Dd][Oo][Mm][Aa][Ii][Nn]
/* operators */
EQ		== 
NEQ		!= 
LT		<
GT		>
LE		<=
GE		>=
ASSIGN		=
DOTS		\.\.
COMMAS		,,
MOD		[Mm][Oo][Dd]
HIDE		#?[Hh][Ii][Dd][Ee]
NORM		\|\|
EXP		\*\*

%%
{NL}		{ lineno++; newline_read = 1; }
{COMMENT}	{ newline_read = 1; lineno++;}
{WS}		{ newline_read = 0; }
{OPTION}	{ yylval.s = clone_string(yytext); return OPTION; }
{VARIABLEDEF}	{ return VARIABLEDEF; }	
{IF}		{ newline_read = 0;return IF; }
{COMPUTE}	{ newline_read = 0;return COMPUTE; }
{MINIMIZE}	{ newline_read = 0;return MINIMIZE; }
{MAXIMIZE}	{ newline_read = 0;return MAXIMIZE; }
{SHOW}		{ newline_read = 0;return SHOW; }
{NOT}		{ newline_read = 0;return NOT; }
{EXTERNAL}	{ newline_read = 0;return EXTERNAL; }
{FUNCTION}	{ newline_read = 0;inside_definition = 1; return FUNCTION; }
{STRING}	{ newline_read = 0; if (!sys_data.drop_quotes) {
                                     yylval.s = clone_string(yytext);
                                    } else {
                                     yylval.s = clone_string(yytext+1);
                                     yylval.s[strlen(yylval.s)-1] =
				     '\0';
                                    }
                  return IDENTIFIER; }
{MOD}		{ newline_read = 0; return MOD; }
{CONST}		{ newline_read = 0; inside_definition = 1; return CONST; }
{HIDE}		{ newline_read = 0; return HIDE; }
{WEIGHT}		{ return WEIGHT;}
{FUNCSYMBOL}    { newline_read = 0; if (function_table->Lookup(yytext) >= 0) { 
			yylval.s = clone_string(yytext);
			return FUNCSYMBOL;
		  } else REJECT; }
{IDENTIFIER}	{ newline_read = 0; yylval.s = clone_string(yytext); return IDENTIFIER; }
{VARIABLE}	{ newline_read = 0; yylval.s = clone_string(yytext);
				    return VARIABLE;} 
{TYPE}	{ newline_read = 0; yylval.s = clone_string(yytext);
				    return TYPE;} 
{PREDICATE}	{ newline_read = 0; yylval.s = clone_string(yytext);
				    return PREDICATE;} 
{NONDOMAIN}	{ newline_read = 0; yylval.s = clone_string(yytext);
				    return NONDOMAIN;} 
{NUMBER}	{ newline_read = 0; yylval.l = strtol(yytext,NULL,10);
		  return NUMBER;} 
{EQ}		{ newline_read = 0; return EQ; }
{NEQ}		{ newline_read = 0; return NEQ; }
{LT}		{ newline_read = 0; return LT; }
{GT}		{ newline_read = 0; return GT; }
{GE}		{ newline_read = 0; return GE; }
{LE}		{ newline_read = 0; return LE; }
{ASSIGN}	{ newline_read = 0; return ASSIGN; }
{DOTS}		{ newline_read = 0; return DOTS; }
{COMMAS}	{ newline_read = 0; return COMMAS; } 
{COMMENT2}	{ newline_read = 0; }
[(]		{ newline_read = 0; par_depth++; return '('; }
[)]		{ newline_read = 0; par_depth--; return ')';  }
{NORM}		{ newline_read = 0; return NORM; }
{EXP}		{ newline_read = 0; return EXP; }
<<EOF>>		{ if ((sys_data.num_input_files == 0) ||
                      (!next_input_file())) {
                    yyterminate();
                  }
                }
. 		{ newline_read = 0; return *yytext; }
