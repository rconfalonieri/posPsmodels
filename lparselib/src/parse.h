/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     NUMBER = 258,
     WEIGHT = 259,
     MINIMIZE = 260,
     MAXIMIZE = 261,
     HIDE = 262,
     EXP = 263,
     FUNCTION = 264,
     FUNCSYMBOL = 265,
     IF = 266,
     NOT = 267,
     COMPUTE = 268,
     IDENTIFIER = 269,
     VARIABLE = 270,
     CONST = 271,
     EXTERNAL = 272,
     SHOW = 273,
     OPTION = 274,
     VARIABLEDEF = 275,
     LE = 276,
     GE = 277,
     EQ = 278,
     NEQ = 279,
     DOTS = 280,
     LT = 281,
     GT = 282,
     ASSIGN = 283,
     MOD = 284,
     COMMAS = 285,
     NORM = 286,
     METAPRIORITY = 287,
     TYPE = 288,
     PREDICATE = 289,
     NONDOMAIN = 290,
     UMINUS = 291
   };
#endif
/* Tokens.  */
#define NUMBER 258
#define WEIGHT 259
#define MINIMIZE 260
#define MAXIMIZE 261
#define HIDE 262
#define EXP 263
#define FUNCTION 264
#define FUNCSYMBOL 265
#define IF 266
#define NOT 267
#define COMPUTE 268
#define IDENTIFIER 269
#define VARIABLE 270
#define CONST 271
#define EXTERNAL 272
#define SHOW 273
#define OPTION 274
#define VARIABLEDEF 275
#define LE 276
#define GE 277
#define EQ 278
#define NEQ 279
#define DOTS 280
#define LT 281
#define GT 282
#define ASSIGN 283
#define MOD 284
#define COMMAS 285
#define NORM 286
#define METAPRIORITY 287
#define TYPE 288
#define PREDICATE 289
#define NONDOMAIN 290
#define UMINUS 291




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 74 "parse.yy"
{
   char *s;
   long l;
   ParseNode *pn;
}
/* Line 1529 of yacc.c.  */
#line 127 "parse.cc.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

