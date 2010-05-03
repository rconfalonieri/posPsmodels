/*
** parse.y -- the bison parser for lparse
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
#include "parsetree.h"
#include "debug.h"
#include "term.h"
#include "symbol.h"
#include "global.h"
#ifndef ERROR_H
#include "error.h"
#endif
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef MS_WINDOWS
#include <malloc.h>
#define alloca _alloca
#endif

extern int yylex(void);
extern ParseNode *logic_program;

void read_option(char *);

ParseNode *tmp_node;


// Turn YYDEBUG on while debugging
#if DEBUG & DBG_PARSE
#define YYDEBUG 1
#else
#define YYDEBUG 0
#endif

//#define YYERROR_VERBOSE

long lineno = 1;
long lval1 = 0, lval2 = 0;
int return_symbol = 0;
int return_funcsymbol = 1;

extern char *yytext;

void yyerror(char *st)
{
  sys_data.num_errors++;
  fprintf(stderr, "%s: %s", error_file_and_line(lineno), st);
  if (strlen(yytext))
    fprintf(stderr, " before '%s'", yytext);
  fprintf(stderr, "\n");
}

%}

%union {
   char *s;
   long l;
   ParseNode *pn;
}

%token <l> NUMBER WEIGHT MINIMIZE MAXIMIZE HIDE EXP 
%token <s> FUNCTION FUNCSYMBOL IF NOT COMPUTE IDENTIFIER
%token <s> VARIABLE  CONST  EXTERNAL SHOW OPTION VARIABLEDEF
%token <s> LE GE EQ NEQ DOTS LT GT ASSIGN MOD COMMAS NORM
%token <s> METAPRIORITY TYPE PREDICATE NONDOMAIN
%type <pn> program rules statement declaration rule head tail 
%type <pn> basic_tail special_tail special_head literal atom
%type <pn> function arguments arglist term constant range expr
%type <pn> literal_list compute_list compute_stmt weight_def nc_expr
%type <pn> basic_literal headlist condition_list optimize_stmt bound
%type <pn> directive ordered_disjunction
%type <l> models


%left ASSIGN
%left '&' '|' '^'
%left EQ LE GE LT GT NEQ
%left '+' '-'
%left '*' '/', MOD
%right UMINUS '~'
%right EXP
%right '('
%left  ')'
%left ':'
%left ','


%%

program : rules
	{ logic_program = $1; }
	;

rules : rules rule '.'
      { $$ = new ParseNode(PT_RULES, NULL, $2, $1, lineno); }
      | rules statement '.'
      { $$ = new ParseNode(PT_RULES, NULL, $2, $1, lineno); }
      | rules error '.'
      { yyerrok; }
      | rules directive 
      { $$ = new ParseNode(PT_RULES, NULL, $2, $1, lineno); }
      |
      { $$ = NULL; }
      ;

statement : declaration
	  { $$ = $1; }
	  | compute_stmt
	  { $$ = $1; }
	  | optimize_stmt
	  { $$ = $1; }
	  ;

directive : OPTION
          {read_option($1); $$ = NULL; }
          ;

declaration : FUNCTION IDENTIFIER 
	    { function_table->Insert($2); $$ = NULL; delete $2;}
	    | WEIGHT literal_list
	    { $$ = new ParseNode(PT_DECLARATION, NULL, NULL, $2, 
	                         lineno); }
	    | HIDE literal_list 
            { $$ = new ParseNode(PT_HIDE, NULL, $2, NULL, lineno); }
	    | HIDE
	    { $$ = NULL; sys_data.hide_all = 1; }
	    | CONST IDENTIFIER ASSIGN expr 
	    { 
              lval1 = $4->Eval();
              if (lval1 == ERROR_NUMBER) {
                 error(USR_ERR, "%s: Invalid constant declaration: ",
                       error_file_and_line(lineno));
                 fprintf(stderr, "\t\tconst %s = ", $2);
                 $4->PrintExpression();
                 fprintf(stderr, ".\n   The expression is not a"
    	                 " valid constant expression\n");
              }	 
              long index;
              int needed = 0;
              int exists = 0;

              if ((index = numeric_constant_table->LookIndex($2)) >= 0) { 
	              exists = 1;
    		      needed = 1;
                  if (lval1 != numeric_constant_table->Lookup($2)) { 
                    long val;
                    if (command_line_constants) {
                      command_line_constants->ClearIterator();
                      while ((val = command_line_constants->Iterate())) {
		                --val;
                        if (val == index) {
                           needed = 0;
                           break;
                        }
                      }
                   }
                 } else { needed = 0; }
              }
              if (!exists) {
                  numeric_constant_table->Insert($2);
                  numeric_constant_table->SetValue($2, lval1);
              }
              if (needed) {
	          warn(WARN_SIMILAR, lineno, "the numeric constant '%s'"
	               " has two values defined for it, '%ld' and"
		       " '%ld'", $2, numeric_constant_table->Lookup($2), lval1);
              }
              if (enabled_warning(WARN_SIMILAR) && (constant_table->Lookup($2) >= 0)) { 
                 warn(WARN_SIMILAR, lineno, "the constant '%s' is used as "
		     "both numeric and symbolic constant", $2);
              }
	      delete [] $2;
              delete $4;
              $$ = NULL; }
	    | CONST constant expr 
	    { yyerror("numeric constant declarations now need a '='");
	      $$ = NULL;
	      delete [] $2;
              delete $3;
	    }
	    | CONST VARIABLE ASSIGN expr 
	    {
              error(USR_ERR, "%s: Invalid constant declaration: ",
                    error_file_and_line(lineno));
              fprintf(stderr, "\t\tconst %s = ", $2);
              $4->PrintExpression();
              fprintf(stderr, ".\n   The symbol '%s' is a variable, not"
	              " a constant\n", $2);
              $$ = NULL;
              delete $2;
              delete $4;
            }
	    | EXTERNAL literal_list
	    { $$ = new ParseNode(PT_EXTERNAL, NULL, $2, NULL, lineno);
	    }
	    | SHOW literal_list
	    { $$ = new ParseNode(PT_SHOW, NULL, $2, NULL, lineno); }
	    | VARIABLEDEF literal_list 
	    { sys_data.has_implicit_domains = 1; $$ = new
              ParseNode(PT_DIRECTIVE, NULL, NULL, $2, lineno) ; } 
	    | TYPE literal_list
		{ $$ = NULL; }
        | PREDICATE literal_list
		{ $$ = NULL; }
		| NONDOMAIN literal_list 
		{ $$ = new ParseNode(PT_DIRECTIVE, NULL, NULL, $2, lineno, 1); }
	    ;

rule : head 
     { $$ = new ParseNode(PT_RULE, NULL, $1, NULL, lineno); }
     | head IF tail 
     { $$ = new ParseNode(PT_RULE, NULL, $1, $3, lineno); }
     | IF tail 
     { $$ = new ParseNode(PT_RULE, NULL, NULL, $2, lineno); }
     ;

head : atom
     { $$ = $1; }
     | special_head
     { $$ = $1; }
     | 
     { $$ = NULL; }
     ;

tail : special_tail ',' tail
     { $$ = new ParseNode(PT_TAIL, NULL, $1, $3, lineno); }
     | basic_tail
     { $$ = new ParseNode(PT_TAIL, NULL, $1, NULL, lineno); }
     | special_tail
     { $$ = new ParseNode(PT_TAIL, NULL, $1, NULL, lineno); }
     ;

basic_tail : basic_tail ',' literal
	   { $$ = new ParseNode(PT_BASIC_TAIL, NULL, $3, $1, lineno);}
	   | literal
	   { $$ = new ParseNode(PT_BASIC_TAIL, NULL, $1, NULL, lineno);}
	   | basic_tail ',' nc_expr
	   { $$ = new ParseNode(PT_BASIC_TAIL, NULL, $3, $1, lineno);
             if ($3->type == PT_CONSTANT) $3->type = PT_ATOM; }
	   | nc_expr
	   { $$ = new ParseNode(PT_BASIC_TAIL, NULL, $1, NULL,
                                lineno); }
	   ;

special_tail : bound  '[' basic_tail ']' bound
	     { tmp_node = new ParseNode(PT_BOUND, NULL, $1, $5,
	                                lineno); 
               $$ = new ParseNode(PT_SPECIAL_TAIL, NULL, tmp_node, $3,
  	                          lineno, 0, 0, 1); }
             | bound  '{' basic_tail '}' bound
	     { tmp_node = new ParseNode(PT_BOUND, NULL, $1, $5,
	                                lineno);
               $$ = new ParseNode(PT_SPECIAL_TAIL, NULL, tmp_node, $3,
  	                          lineno); }
	     ;

special_head : bound '{' literal_list '}' bound
	     { tmp_node = new ParseNode(PT_BOUND, NULL, $1, $5, lineno);
               $$ = new ParseNode(PT_SPECIAL_HEAD, NULL, tmp_node, $3,
  	                          lineno); }
	     | bound '[' literal_list ']' bound
             { tmp_node = new ParseNode(PT_BOUND, NULL, $1, $5, lineno);
			 	       $$ = new ParseNode(PT_SPECIAL_HEAD, NULL, tmp_node, $3,
  	         		                    lineno, 0, 0,1); }
             | headlist
	     { $$ = new ParseNode(PT_SPECIAL_HEAD, NULL, NULL, $1, 
  	                          lineno, 0, 1);  }
	     | ordered_disjunction
	     { $$ = new ParseNode(PT_ORDERED_DISJUNCTION, NULL, NULL,
                         	  $1, lineno);
         }
	     | atom ':' ordered_disjunction
	     {  
           $$ = new ParseNode(PT_ORDERED_DISJUNCTION, NULL, $1,
               $3, lineno);
         }
		 | '|' literal_list '|'
         { $$ = new ParseNode(PT_SPECIAL_HEAD, NULL, NULL, $2, 0, 1,
  	            lineno); }
	     ;



headlist : headlist '|' atom
	 { $$ = new ParseNode(PT_LITERAL_LIST, NULL, $3, $1, lineno); }
	 | atom
	 { $$ = new ParseNode(PT_LITERAL_LIST, NULL, $1, NULL,
	   lineno);  }
	 ;

ordered_disjunction : ordered_disjunction IDENTIFIER atom
	 { if (strcmp($2, "x")) {
  	      yyerror("parse error");
          $$ = NULL;
       } else {
          $$ = new ParseNode(PT_LITERAL_LIST, NULL, $3, $1, lineno);
       }
     }      
	 | atom
	 { $$ = new ParseNode(PT_LITERAL_LIST, NULL, $1, NULL,
	   lineno);  }
	 ;


basic_literal : atom
	{ $$ = $1; }
	| NOT atom
	{ $2->l2.negative = 1; $$ = $2; }
	| atom ':' condition_list
	{ $$ = new ParseNode(PT_CONDITION, NULL, $1, $3, lineno); }
	| NOT atom ':' condition_list
	{ $$ = new ParseNode(PT_CONDITION, NULL, $2, $4, lineno);
	  $$->l2.negative = 1;}
	;

literal : basic_literal
	{ $$ = $1; }
	| basic_literal ASSIGN weight_def
	{ $$ = $1; if ($1->type != PT_CONDITION) $$->left = $3;
                   else $$->left->left = $3; }
	;

condition_list : condition_list ':' atom
	 { $$ = new ParseNode(PT_LITERAL_LIST, NULL, $3, $1, lineno); }
	 | atom
	 { $$ = new ParseNode(PT_LITERAL_LIST, NULL, $1, NULL,
	   lineno);  }
	 | nc_expr
	 { $$ = new ParseNode(PT_LITERAL_LIST, NULL, $1, NULL,
	   lineno);  }
	 | condition_list ':' nc_expr
	 { $$ = new ParseNode(PT_LITERAL_LIST, NULL, $3, $1, lineno); }
	 ;

weight_def : expr
	   { $$ =$1; }
	   | atom
	   { $$ = $1; }
	   ;



atom : IDENTIFIER '(' arguments ')'
     { $$ = new ParseNode(PT_ATOM, $1, NULL, $3, lineno); }
     | IDENTIFIER
     { $$ = new ParseNode(PT_ATOM, $1, NULL, NULL, lineno); } 
     ;

bound : expr
      { $$ = $1; }
      |
      { $$ = NULL; }
      ;


function : FUNCSYMBOL '(' arglist ')'
	 { $$ = new ParseNode(PT_FUNCTION, $1, NULL, $3, lineno);}
	 | NOT FUNCSYMBOL '(' arglist ')'
	 { $$ = new ParseNode(PT_FUNCTION, $2, NULL, $4, lineno);
           $$->l2.negative = 1; }
	 | WEIGHT '(' term ')'
	 { $$ = new ParseNode(PT_EXPR, NULL, NULL, $3, lineno,
	                      FUN_WEIGHT); }
	 | WEIGHT '(' NOT term ')'
	 { $4->l2.negative = 1;
           $$ = new ParseNode(PT_EXPR, NULL, NULL, $4, lineno,
	                      FUN_WEIGHT); }
	 ;

arguments : arglist
	  { $$ = new ParseNode(PT_ARGUMENTS, NULL, $1, NULL, lineno);}
	  | arguments ';' arglist
	  { $$ = new ParseNode(PT_ARGUMENTS, NULL, $3, $1, lineno); }
	  ;

arglist : arglist ','  term
	{ $$ = new ParseNode(PT_ARGLIST, NULL, $3, $1, lineno); }
	| term
	{ $$ = new ParseNode(PT_ARGLIST, NULL, $1, NULL , lineno); }
	;

term : range
     { $$ = $1; }
     | expr
     { lval1 = $1->Eval();
       if (lval1 != ERROR_NUMBER) {
           $$ = new ParseNode(PT_NUMBER, NULL, NULL, NULL,
                lineno,lval1);
           delete $1;
       } else {
         $$ = $1;
       }
     }
     | atom 
     { $$ = $1;
       if ($$->right == NULL) {
         lval1 = numeric_constant_table->Lookup($$->sval);
         if (lval1 != ERROR_NUMBER) {
            $$->type = PT_NUMBER;
            $$->l1.val = lval1;
         } else {
          $$->type = PT_CONSTANT;
         }
       }
     }
     | atom '|' expr
     {
      /* crude workaround for situations like a | 1, where 'a' is
	 actually a numeric constant */
       if ( $1->right ) {
	error(USR_ERR, "%s: parse error before '|'\n",
              error_file_and_line(lineno));
       }
       $$ = new ParseNode(PT_EXPR, NULL, $1, $3, lineno, FUN_OR);
       lval1 = numeric_constant_table->Lookup($1->sval);
       if (lval1 != ERROR_NUMBER) {
         delete $$->left;
         $$->left = new ParseNode(PT_NUMBER, NULL, NULL, NULL, lineno,
                                  lval1);
       } else {
         $1->type = PT_CONSTANT;
       }
     }
     ;

constant : IDENTIFIER
	{ lval1 = numeric_constant_table->Lookup($1);
	  if (lval1 == ERROR_NUMBER)  
		$$ = new ParseNode(PT_CONSTANT, $1, NULL, NULL,
                                   lineno);
	  else {
	     $$ = new ParseNode(PT_NUMBER, NULL, NULL, NULL, lineno,lval1);
             delete [] $1; }
	}
	| NUMBER
	{ $$ = new ParseNode(PT_NUMBER, NULL, NULL, NULL, lineno,
	 $1);}
	;

range : expr DOTS expr
      { 
        $$ = new ParseNode(PT_RANGE, NULL, $1, $3, lineno);
      }
      | expr COMMAS expr
      {
        $$ = NULL;
	yyerror("conjunctive ranges are no longer supported, "
	        "please use conditions instead");
        /* lval1 = $1->Eval();
        lval2 = $3->Eval();
  	$$ = new ParseNode(PT_RANGE, NULL, NULL, NULL, lineno, lval1,
                           lval2, 1); */} 
      ;

expr : '(' expr ')'
     { $$ = $2; $$->i1.paren =1; }
     | '|' expr '|'     
     { $$ = new ParseNode(PT_EXPR, NULL, NULL, $2, lineno,
	                  FUN_ABS); $$->i1.paren = 1;}
     | NORM atom NORM
     { $$ = new ParseNode(PT_EXPR, NULL, NULL, $2, lineno,
                          FUN_NORM); }
     | expr EXP expr
     { $$ = new ParseNode(PT_EXPR, NULL, $1, $3, lineno, FUN_EXP); }
     | '-' expr %prec UMINUS
     { $$ = new ParseNode(PT_EXPR, NULL, NULL, $2 , lineno,
                          FUN_MINUS);
       $$->left = new ParseNode(PT_NUMBER, NULL, NULL, NULL, lineno, 0);}
     | expr '+' expr
     { $$ = new ParseNode(PT_EXPR, NULL, $1, $3, lineno, FUN_PLUS); }
     | expr '-' expr
     { $$ = new ParseNode(PT_EXPR, NULL, $1, $3, lineno, FUN_MINUS); }
     | expr '*' expr
     { $$ = new ParseNode(PT_EXPR, NULL, $1, $3, lineno, FUN_TIMES); }
     | expr '/' expr
     { $$ = new ParseNode(PT_EXPR, NULL, $1, $3, lineno, FUN_DIV); }
     | expr MOD expr
     { $$ = new ParseNode(PT_EXPR, NULL, $1, $3, lineno, FUN_MOD); }
     | expr EQ expr
     { $$ = new ParseNode(PT_EXPR, NULL, $1, $3, lineno, FUN_EQ); }
     | expr NEQ expr
     { $$ = new ParseNode(PT_EXPR, NULL, $1, $3, lineno, FUN_NEQ); }
     | expr LT expr
     { $$ = new ParseNode(PT_EXPR, NULL, $1, $3, lineno, FUN_LT); }
     | expr LE expr
     { $$ = new ParseNode(PT_EXPR, NULL, $1, $3, lineno, FUN_LE); }
     | expr GT expr
     { $$ = new ParseNode(PT_EXPR, NULL, $1, $3, lineno, FUN_GT); }
     | expr GE expr
     { $$ = new ParseNode(PT_EXPR, NULL, $1, $3, lineno, FUN_GE); }
     | '~' expr
     { $$ = new ParseNode(PT_EXPR, NULL, $2 ,NULL , lineno,
                          FUN_NOT);
       $$->right = new ParseNode(PT_NUMBER, NULL, NULL, NULL, lineno,
                                 0);}
     | expr '&' expr
     { $$ = new ParseNode(PT_EXPR, NULL, $1, $3, lineno, FUN_AND); }
     | expr '|' expr
     { $$ = new ParseNode(PT_EXPR, NULL, $1, $3, lineno, FUN_OR); }
     | expr '^' expr
     { $$ = new ParseNode(PT_EXPR, NULL, $1, $3, lineno, FUN_XOR); }
     | expr ASSIGN expr
     { $$ = new ParseNode(PT_EXPR, NULL, $1, $3, lineno,
                         FUN_ASSIGN); }
     | function
     { $$ = $1; }
     | constant
     { $$ = $1; }
     | VARIABLE
     { $$ = new ParseNode(PT_VARIABLE, $1, NULL, NULL, lineno); }
     ;

nc_expr : '(' expr ')'
     { $$ = $2; }
     | '|' expr '|'     
     { $$ = new ParseNode(PT_EXPR, NULL, NULL, $2, lineno,
	                  FUN_ABS);} 
     | '-' expr %prec UMINUS
     { $$ = new ParseNode(PT_EXPR, NULL, NULL, $2 , lineno,
                          FUN_MINUS);
       $$->left = new ParseNode(PT_NUMBER, NULL, NULL, NULL, lineno, 0);}
     | expr '+' expr
     { $$ = new ParseNode(PT_EXPR, NULL, $1, $3, lineno, FUN_PLUS); }
     | expr '-' expr
     { $$ = new ParseNode(PT_EXPR, NULL, $1, $3, lineno, FUN_MINUS); }
     | expr '*' expr
     { $$ = new ParseNode(PT_EXPR, NULL, $1, $3, lineno, FUN_TIMES); }
     | expr '/' expr
     { $$ = new ParseNode(PT_EXPR, NULL, $1, $3, lineno, FUN_DIV); }
     | expr MOD expr
     { $$ = new ParseNode(PT_EXPR, NULL, $1, $3, lineno, FUN_MOD); }
     | expr EQ expr
     { $$ = new ParseNode(PT_EXPR, NULL, $1, $3, lineno, FUN_EQ); }
     | expr NEQ expr
     { $$ = new ParseNode(PT_EXPR, NULL, $1, $3, lineno, FUN_NEQ); }
     | expr LT expr
     { $$ = new ParseNode(PT_EXPR, NULL, $1, $3, lineno, FUN_LT); }
     | expr LE expr
     { $$ = new ParseNode(PT_EXPR, NULL, $1, $3, lineno, FUN_LE); }
     | expr GT expr
     { $$ = new ParseNode(PT_EXPR, NULL, $1, $3, lineno, FUN_GT); }
     | expr GE expr
     { $$ = new ParseNode(PT_EXPR, NULL, $1, $3, lineno, FUN_GE); }
     | '~' expr
     { $$ = new ParseNode(PT_EXPR, NULL, $2 ,NULL , lineno,
                          FUN_NOT);
       $$->right = new ParseNode(PT_NUMBER, NULL, NULL, NULL, lineno,
                                 0);}
     | expr '&' expr
     { $$ = new ParseNode(PT_EXPR, NULL, $1, $3, lineno, FUN_AND); }
     | expr '|' expr
     { $$ = new ParseNode(PT_EXPR, NULL, $1, $3, lineno, FUN_OR); }
     | expr '^' expr
     { $$ = new ParseNode(PT_EXPR, NULL, $1, $3, lineno, FUN_XOR); }
     | expr ASSIGN expr
     { $$ = new ParseNode(PT_EXPR, NULL,  $1, $3, lineno,
                          FUN_ASSIGN);}
     | function
     { $$ = $1; }
     ;


literal_list : literal_list ',' literal
	     { $$ = new ParseNode(PT_LITERAL_LIST, NULL, $3, $1, lineno);}
	     | literal
	     { $$ = new ParseNode(PT_LITERAL_LIST, NULL, $1, NULL, lineno);}
	     ;

compute_list : literal_list
	     { $$ = $1; }
	     |
	     { $$ = 0; }
	     ;


compute_stmt : COMPUTE models '{' compute_list '}'
	     { $$ = new ParseNode(PT_COMPUTE, NULL, NULL, $4,
	                          lineno, $2);}
             ;

optimize_stmt : MAXIMIZE '{' compute_list '}'
	      { $$ = new ParseNode(PT_MAXIMIZE, NULL, NULL, $3,
	                          lineno); }
	      | MAXIMIZE '[' compute_list ']'
	      { $$ = new ParseNode(PT_MAXIMIZE, NULL, NULL, $3,
	                          lineno, 0, 0, 1); }
	      | MINIMIZE '{' compute_list '}'
	      { $$ = new ParseNode(PT_MINIMIZE, NULL, NULL, $3,
	                          lineno); }
	      | MINIMIZE '[' compute_list ']'
	      { $$ = new ParseNode(PT_MINIMIZE, NULL, NULL, $3,
	                          lineno,0,0,1); }
	      ;

models : expr
       {
         if ($1->type == PT_CONSTANT) {
           if (!strcmp($1->sval, "all")) {
             $$ = 0;
           } else {
             error(USR_ERR, "%s: invalid number of models. Use a "
                   "positive integer, a constant valued expression, "
                   "or \"all\"", 
                   error_file_and_line(lineno));
                   $$ = 1;
           }
         } else {
           $$ = $1->Eval();
           if ($$ == ERROR_NUMBER || $$ < 0) {
             error(USR_ERR, "%s: invalid number of models. Use a "
                  "positive integer, a constant valued expression, or "
	          "\"all\"", error_file_and_line(lineno));
           }
         }
         delete $1;
       }
       |
       { $$ = 1; }
       ;

