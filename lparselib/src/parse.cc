/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



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




/* Copy the first part of user declarations.  */
#line 21 "parse.yy"

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



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 74 "parse.yy"
{
   char *s;
   long l;
   ParseNode *pn;
}
/* Line 193 of yacc.c.  */
#line 227 "parse.cc"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 240 "parse.cc"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   990

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  55
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  33
/* YYNRULES -- Number of rules.  */
#define YYNRULES  136
/* YYNRULES -- Number of states.  */
#define YYNSTATES  264

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   291

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    36,     2,
      45,    46,    41,    39,    48,    40,    49,    42,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    47,    54,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    50,     2,    51,    38,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    52,    37,    53,    43,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    44
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     9,    13,    17,    20,    21,    23,
      25,    27,    29,    32,    35,    38,    40,    45,    49,    54,
      57,    60,    63,    66,    69,    72,    74,    78,    81,    83,
      85,    86,    90,    92,    94,    98,   100,   104,   106,   112,
     118,   124,   130,   132,   134,   138,   142,   146,   148,   152,
     154,   156,   159,   163,   168,   170,   174,   178,   180,   182,
     186,   188,   190,   195,   197,   199,   200,   205,   211,   216,
     222,   224,   228,   232,   234,   236,   238,   240,   244,   246,
     248,   252,   256,   260,   264,   268,   272,   275,   279,   283,
     287,   291,   295,   299,   303,   307,   311,   315,   319,   322,
     326,   330,   334,   338,   340,   342,   344,   348,   352,   355,
     359,   363,   367,   371,   375,   379,   383,   387,   391,   395,
     399,   402,   406,   410,   414,   418,   420,   424,   426,   428,
     429,   435,   440,   445,   450,   455,   457
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      56,     0,    -1,    57,    -1,    57,    61,    49,    -1,    57,
      58,    49,    -1,    57,     1,    49,    -1,    57,    59,    -1,
      -1,    60,    -1,    85,    -1,    86,    -1,    19,    -1,     9,
      14,    -1,     4,    83,    -1,     7,    83,    -1,     7,    -1,
      16,    14,    28,    81,    -1,    16,    79,    81,    -1,    16,
      15,    28,    81,    -1,    17,    83,    -1,    18,    83,    -1,
      20,    83,    -1,    33,    83,    -1,    34,    83,    -1,    35,
      83,    -1,    62,    -1,    62,    11,    63,    -1,    11,    63,
      -1,    73,    -1,    66,    -1,    -1,    65,    48,    63,    -1,
      64,    -1,    65,    -1,    64,    48,    70,    -1,    70,    -1,
      64,    48,    82,    -1,    82,    -1,    74,    50,    64,    51,
      74,    -1,    74,    52,    64,    53,    74,    -1,    74,    52,
      83,    53,    74,    -1,    74,    50,    83,    51,    74,    -1,
      67,    -1,    68,    -1,    73,    47,    68,    -1,    37,    83,
      37,    -1,    67,    37,    73,    -1,    73,    -1,    68,    14,
      73,    -1,    73,    -1,    73,    -1,    12,    73,    -1,    73,
      47,    71,    -1,    12,    73,    47,    71,    -1,    69,    -1,
      69,    28,    72,    -1,    71,    47,    73,    -1,    73,    -1,
      82,    -1,    71,    47,    82,    -1,    81,    -1,    73,    -1,
      14,    45,    76,    46,    -1,    14,    -1,    81,    -1,    -1,
      10,    45,    77,    46,    -1,    12,    10,    45,    77,    46,
      -1,     4,    45,    78,    46,    -1,     4,    45,    12,    78,
      46,    -1,    77,    -1,    76,    54,    77,    -1,    77,    48,
      78,    -1,    78,    -1,    80,    -1,    81,    -1,    73,    -1,
      73,    37,    81,    -1,    14,    -1,     3,    -1,    81,    25,
      81,    -1,    81,    30,    81,    -1,    45,    81,    46,    -1,
      37,    81,    37,    -1,    31,    73,    31,    -1,    81,     8,
      81,    -1,    40,    81,    -1,    81,    39,    81,    -1,    81,
      40,    81,    -1,    81,    41,    81,    -1,    81,    42,    81,
      -1,    81,    29,    81,    -1,    81,    23,    81,    -1,    81,
      24,    81,    -1,    81,    26,    81,    -1,    81,    21,    81,
      -1,    81,    27,    81,    -1,    81,    22,    81,    -1,    43,
      81,    -1,    81,    36,    81,    -1,    81,    37,    81,    -1,
      81,    38,    81,    -1,    81,    28,    81,    -1,    75,    -1,
      79,    -1,    15,    -1,    45,    81,    46,    -1,    37,    81,
      37,    -1,    40,    81,    -1,    81,    39,    81,    -1,    81,
      40,    81,    -1,    81,    41,    81,    -1,    81,    42,    81,
      -1,    81,    29,    81,    -1,    81,    23,    81,    -1,    81,
      24,    81,    -1,    81,    26,    81,    -1,    81,    21,    81,
      -1,    81,    27,    81,    -1,    81,    22,    81,    -1,    43,
      81,    -1,    81,    36,    81,    -1,    81,    37,    81,    -1,
      81,    38,    81,    -1,    81,    28,    81,    -1,    75,    -1,
      83,    48,    70,    -1,    70,    -1,    83,    -1,    -1,    13,
      87,    52,    84,    53,    -1,     6,    52,    84,    53,    -1,
       6,    50,    84,    51,    -1,     5,    52,    84,    53,    -1,
       5,    50,    84,    51,    -1,    81,    -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   109,   109,   113,   115,   117,   119,   122,   125,   127,
     129,   133,   137,   139,   142,   144,   146,   194,   200,   212,
     215,   217,   220,   222,   224,   228,   230,   232,   236,   238,
     241,   244,   246,   248,   252,   254,   256,   259,   264,   269,
     276,   280,   284,   287,   291,   296,   303,   305,   310,   318,
     324,   326,   328,   330,   335,   337,   342,   344,   347,   350,
     354,   356,   362,   364,   368,   371,   375,   377,   380,   383,
     389,   391,   395,   397,   401,   403,   413,   425,   445,   454,
     459,   463,   474,   476,   479,   482,   484,   488,   490,   492,
     494,   496,   498,   500,   502,   504,   506,   508,   510,   515,
     517,   519,   521,   524,   526,   528,   532,   534,   537,   541,
     543,   545,   547,   549,   551,   553,   555,   557,   559,   561,
     563,   568,   570,   572,   574,   577,   582,   584,   588,   591,
     595,   600,   603,   606,   609,   614,   637
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "NUMBER", "WEIGHT", "MINIMIZE",
  "MAXIMIZE", "HIDE", "EXP", "FUNCTION", "FUNCSYMBOL", "IF", "NOT",
  "COMPUTE", "IDENTIFIER", "VARIABLE", "CONST", "EXTERNAL", "SHOW",
  "OPTION", "VARIABLEDEF", "LE", "GE", "EQ", "NEQ", "DOTS", "LT", "GT",
  "ASSIGN", "MOD", "COMMAS", "NORM", "METAPRIORITY", "TYPE", "PREDICATE",
  "NONDOMAIN", "'&'", "'|'", "'^'", "'+'", "'-'", "'*'", "'/'", "'~'",
  "UMINUS", "'('", "')'", "':'", "','", "'.'", "'['", "']'", "'{'", "'}'",
  "';'", "$accept", "program", "rules", "statement", "directive",
  "declaration", "rule", "head", "tail", "basic_tail", "special_tail",
  "special_head", "headlist", "ordered_disjunction", "basic_literal",
  "literal", "condition_list", "weight_def", "atom", "bound", "function",
  "arguments", "arglist", "term", "constant", "range", "expr", "nc_expr",
  "literal_list", "compute_list", "compute_stmt", "optimize_stmt",
  "models", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,    38,   124,    94,    43,
      45,    42,    47,   126,   291,    40,    41,    58,    44,    46,
      91,    93,   123,   125,    59
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    55,    56,    57,    57,    57,    57,    57,    58,    58,
      58,    59,    60,    60,    60,    60,    60,    60,    60,    60,
      60,    60,    60,    60,    60,    61,    61,    61,    62,    62,
      62,    63,    63,    63,    64,    64,    64,    64,    65,    65,
      66,    66,    66,    66,    66,    66,    67,    67,    68,    68,
      69,    69,    69,    69,    70,    70,    71,    71,    71,    71,
      72,    72,    73,    73,    74,    74,    75,    75,    75,    75,
      76,    76,    77,    77,    78,    78,    78,    78,    79,    79,
      80,    80,    81,    81,    81,    81,    81,    81,    81,    81,
      81,    81,    81,    81,    81,    81,    81,    81,    81,    81,
      81,    81,    81,    81,    81,    81,    82,    82,    82,    82,
      82,    82,    82,    82,    82,    82,    82,    82,    82,    82,
      82,    82,    82,    82,    82,    82,    83,    83,    84,    84,
      85,    86,    86,    86,    86,    87,    87
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     3,     3,     3,     2,     0,     1,     1,
       1,     1,     2,     2,     2,     1,     4,     3,     4,     2,
       2,     2,     2,     2,     2,     1,     3,     2,     1,     1,
       0,     3,     1,     1,     3,     1,     3,     1,     5,     5,
       5,     5,     1,     1,     3,     3,     3,     1,     3,     1,
       1,     2,     3,     4,     1,     3,     3,     1,     1,     3,
       1,     1,     4,     1,     1,     0,     4,     5,     4,     5,
       1,     3,     3,     1,     1,     1,     1,     3,     1,     1,
       3,     3,     3,     3,     3,     3,     2,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     2,     3,
       3,     3,     3,     1,     1,     1,     3,     3,     2,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       2,     3,     3,     3,     3,     1,     3,     1,     1,     0,
       5,     4,     4,     4,     4,     1,     0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       7,     0,     0,     1,     0,    79,     0,     0,     0,    15,
       0,     0,    65,     0,   136,    78,   105,     0,     0,     0,
      11,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     6,     8,     0,    25,    29,    42,    43,    28,     0,
     103,   104,    64,     9,    10,     5,     0,    63,     0,    54,
     127,    50,    13,   129,   129,   129,   129,    14,    12,     0,
       0,     0,     0,     0,     0,     0,    27,    32,    33,    35,
       0,   103,    64,    37,     0,    78,     0,   135,     0,     0,
      78,     0,     0,    19,    20,    21,     0,    22,    23,    24,
       0,     0,    86,    98,     0,     4,     3,    65,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    51,
       0,    76,     0,    74,    75,     0,     0,     0,   128,     0,
       0,     0,     0,     0,    73,     0,    86,    98,     0,     0,
      65,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   129,
       0,    70,     0,     0,    17,    84,    83,    45,    82,    26,
      46,    48,    44,    49,     0,     0,    85,    95,    97,    92,
      93,    94,    96,   102,    91,    99,   100,   101,    87,    88,
      89,    90,     0,     0,     0,     0,    68,     0,     0,    55,
      61,    60,    52,    57,     0,    58,   126,   134,   133,   132,
     131,    66,     0,    83,    82,    34,    36,    31,     0,     0,
      95,    97,    92,    93,    94,    96,   124,    91,    99,   100,
     101,    87,    88,    89,    90,     0,     0,    62,     0,    16,
      18,    65,    65,    53,     0,    69,    77,    80,    81,     0,
      72,    65,    65,    67,   130,    71,    41,    40,     0,    56,
      59,    38,    39,    66
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,    30,    31,    32,    33,    34,    66,    67,
      68,    35,    36,    37,    49,    50,   202,   199,    51,    70,
      40,   160,   133,   134,    41,   123,   124,    73,   128,   129,
      43,    44,    78
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -123
static const yytype_int16 yypact[] =
{
    -123,    21,   283,  -123,     1,  -123,    -3,     6,    29,    74,
      53,    23,   603,    62,   635,   577,  -123,     4,    74,    74,
    -123,    74,    78,    74,    74,    74,   648,   635,   635,   635,
      47,  -123,  -123,    50,    89,  -123,    80,   107,     8,   117,
    -123,  -123,   838,  -123,  -123,  -123,    78,    77,   680,    97,
    -123,    79,   108,    74,    74,    74,    74,   108,  -123,   693,
     118,    25,   635,   635,   635,   635,  -123,   127,   130,  -123,
     124,   184,   860,  -123,   121,  -123,   635,   838,   142,   693,
     160,   169,   635,   108,   108,   108,   148,   108,   108,   108,
     882,   -17,   198,   198,   763,  -123,  -123,   603,    78,    78,
      78,    74,    74,   635,   635,   635,   635,   635,   635,   635,
     635,   635,   635,   635,   635,   635,   635,   635,   635,   162,
     725,   173,   166,  -123,   815,   693,   738,    74,   108,   164,
     165,   168,   170,   136,  -123,   904,   151,   257,   789,   603,
     603,   603,   603,   635,   635,   635,   635,   635,   635,   635,
     635,   635,   635,   635,   635,   635,   635,   635,   693,    74,
     -38,   181,   635,   635,   838,  -123,    26,  -123,  -123,  -123,
    -123,  -123,   107,  -123,    42,   -25,   198,   628,   628,   628,
     628,   628,   628,   926,   198,   948,   948,   948,    90,    90,
     198,   198,   738,   176,   167,   635,  -123,   635,   635,  -123,
    -123,   838,   189,  -123,   860,  -123,  -123,  -123,  -123,  -123,
    -123,  -123,   693,   177,   422,  -123,  -123,  -123,    46,    11,
      36,   219,   414,   439,   456,   481,   558,   301,   317,   361,
     397,   498,   523,   326,   381,   137,   185,  -123,   693,   838,
     838,   635,   635,   189,   693,  -123,   838,   838,   838,   738,
    -123,   635,   635,  -123,  -123,   181,  -123,  -123,   159,  -123,
    -123,  -123,  -123,  -123
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -123,  -123,  -123,  -123,  -123,  -123,  -123,  -123,   -92,  -109,
    -123,  -123,  -123,   144,  -123,    31,    54,  -123,   155,     0,
      -6,  -123,   -76,   -47,   228,  -123,    -2,  -122,    28,   -41,
    -123,  -123,  -123
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -126
static const yytype_int16 yytable[] =
{
      42,   122,    39,   161,   205,   169,    71,     5,   237,    46,
      72,    47,    77,   130,   131,   132,   238,   216,    80,    81,
     167,     3,   -49,   127,    90,    92,    93,    94,   242,     5,
      60,   127,   218,   219,    52,    74,    11,    57,    13,    47,
      75,    16,    48,    69,   103,   -47,    83,    84,   217,    85,
      45,    87,    88,    89,    91,   100,    53,    22,    54,   139,
     135,   136,   137,   138,   252,   111,    27,    58,    59,    28,
     205,    29,    74,   194,    90,   115,   116,   117,   118,    55,
     164,    56,   235,  -117,  -117,  -117,    46,  -117,    47,  -117,
     127,    71,    47,   241,   139,    72,    95,   251,   103,    96,
      97,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,    98,   236,   111,
      71,    99,    79,   201,   204,   125,   126,   260,    69,   174,
     175,   117,   118,    71,    71,    71,    71,   204,    72,   204,
     204,   220,   221,   222,   223,   224,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   127,    38,   206,   103,
     239,   240,   255,    48,   186,   250,   158,   101,   258,   102,
     215,    69,    69,    69,   141,   139,   142,    86,   140,   165,
       5,    60,   211,   253,   212,   212,    71,    11,   162,    13,
     204,    75,    16,   246,   159,   247,   248,   163,  -108,  -108,
    -108,   119,  -108,   121,  -108,   263,   103,   212,    22,   192,
     195,   186,   196,   245,   121,   207,   119,    27,   208,   209,
      28,   244,    29,   210,  -107,  -107,  -107,   103,  -107,   212,
    -107,  -125,  -125,  -125,   121,  -125,   249,  -125,   254,    42,
      42,   256,   257,    71,   172,    82,   243,   204,   111,    42,
      42,   261,   262,   170,   171,   173,     0,     0,   115,   116,
     117,   118,     0,     0,     0,   103,  -119,  -119,  -119,     0,
    -119,     0,  -119,     0,     0,   121,     0,     0,     0,     0,
     200,   203,     0,    -2,     4,     0,     5,     6,     7,     8,
       9,     0,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,  -120,  -120,  -120,     0,  -120,   103,
    -120,     0,     0,   121,    22,     0,    23,    24,    25,     0,
      26,     0,     0,    27,     0,   103,    28,     0,    29,     0,
       0,     0,   -30,   -65,   103,   -65,     0,     0,   104,   105,
     106,   107,     0,   108,   109,     0,   111,   203,  -113,  -113,
    -113,     0,  -113,     0,  -113,     0,   115,   116,   117,   118,
       0,     0,     0,     0,  -121,  -121,  -121,   121,  -121,   103,
    -121,     0,     0,  -111,  -111,  -111,     0,  -111,     0,  -111,
       0,     0,   104,   105,   106,   107,     0,   108,   109,   103,
     111,     0,     0,   121,     0,     0,     0,     0,     0,   121,
     115,   116,   117,   118,   259,   103,     0,     0,  -122,  -122,
    -122,     0,  -122,     0,  -122,     0,     0,     0,   104,   105,
     106,   107,   103,   108,   109,     0,   111,     0,  -112,  -112,
    -112,     0,  -112,     0,  -112,     0,   115,   116,   117,   118,
       0,     0,     0,   111,  -123,  -123,  -123,   103,  -123,     0,
    -123,     0,     0,   115,   116,   117,   118,     0,     0,     0,
       0,  -114,  -114,  -114,   103,  -114,     0,  -114,   111,  -106,
    -106,  -106,     0,  -106,     0,  -106,     0,     0,   115,   116,
     117,   118,     0,     0,     0,   111,  -115,  -115,  -115,   103,
    -115,     0,  -115,     0,     0,   115,   116,   117,   118,     0,
       0,     0,     0,  -116,  -116,  -116,   103,  -116,     0,  -116,
     111,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     115,   116,   117,   118,     0,     0,     0,   111,  -118,  -118,
    -118,   103,  -118,     0,  -118,     0,     0,     0,     0,   117,
     118,     0,     0,     0,     0,  -109,  -109,  -109,     0,  -109,
       0,  -109,   111,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   117,   118,   103,     0,     0,     0,
    -110,  -110,  -110,     0,  -110,     0,  -110,     0,     0,   104,
     105,   106,   107,     0,   108,   109,  -102,   111,   -63,     0,
       0,   -63,     0,     0,   112,   113,   114,   115,   116,   117,
     118,     0,     0,     0,     0,   -63,     5,    60,  -102,     0,
    -102,     0,     0,    11,   -63,    61,     0,    15,    16,     0,
       0,     0,    79,   -63,   -63,   -63,   -63,     0,   -63,     0,
     -63,   -63,     0,     0,    22,     0,   103,     0,     5,    60,
      62,     0,     0,    63,     0,    11,    64,    13,    65,    75,
      16,     5,    60,     0,     0,     0,     0,   111,    11,     0,
      61,     0,    15,    16,     0,     0,    22,   115,   116,   117,
     118,     0,    76,     0,     0,    27,     0,     0,    28,    22,
      29,     0,     0,     5,    60,    76,     0,     0,    27,     0,
      11,    28,   120,    29,    15,    16,     5,    60,     0,     0,
       0,     0,     0,    11,     0,    13,     0,    15,    16,     0,
       0,    22,     0,     0,     0,     0,     0,    76,     0,     0,
      27,     0,     0,    28,    22,    29,     0,     0,     5,    60,
      76,     0,     0,    27,     0,   193,    28,    13,    29,    15,
      16,     5,    60,     0,     0,     0,     0,     0,    11,     0,
      13,     0,    15,    16,     0,     0,    22,     0,     0,     0,
       0,     0,    76,     0,     0,    27,     0,     0,    28,    22,
      29,   103,     0,     0,     0,    62,     0,     0,    63,     0,
       0,    64,     0,    65,   104,   105,   106,   107,     0,   108,
     109,   110,   111,     0,     0,     0,     0,   103,     0,   112,
     113,   114,   115,   116,   117,   118,     0,     0,     0,   168,
     104,   105,   106,   107,     0,   108,   109,   110,   111,     0,
       0,     0,     0,   103,     0,   112,   113,   114,   115,   116,
     117,   118,     0,     0,     0,   214,   104,   105,   106,   107,
     197,   108,   109,   110,   111,   198,   103,     0,     0,     0,
       0,   112,   113,   114,   115,   116,   117,   118,     0,   104,
     105,   106,   107,     0,   108,   109,   110,   111,   103,     0,
       0,     0,     0,     0,   112,   113,   114,   115,   116,   117,
     118,   143,   144,   145,   146,     0,   147,   148,   149,   150,
     103,     0,     0,     0,     0,     0,   151,   152,   153,   154,
     155,   156,   157,   104,   105,   106,   107,     0,   108,   109,
     110,   111,   103,     0,     0,     0,     0,     0,   112,   166,
     114,   115,   116,   117,   118,   104,   105,   106,   107,     0,
     108,   109,   110,   111,   103,     0,     0,     0,     0,     0,
     112,   213,   114,   115,   116,   117,   118,   104,   105,   106,
     107,     0,   108,   109,     0,   111,   103,     0,     0,     0,
       0,     0,   112,   113,   114,   115,   116,   117,   118,   104,
     105,   106,   107,     0,   108,   109,     0,   111,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   115,   116,   117,
     118
};

static const yytype_int16 yycheck[] =
{
       2,    48,     2,    79,   126,    97,    12,     3,    46,    12,
      12,    14,    14,    54,    55,    56,    54,   139,    14,    15,
      37,     0,    14,    48,    26,    27,    28,    29,    53,     3,
       4,    48,   141,   142,     6,    10,    10,     9,    12,    14,
      14,    15,    45,    12,     8,    37,    18,    19,   140,    21,
      49,    23,    24,    25,    26,    47,    50,    31,    52,    48,
      62,    63,    64,    65,    53,    29,    40,    14,    45,    43,
     192,    45,    10,   120,    76,    39,    40,    41,    42,    50,
      82,    52,   158,    47,    48,    49,    12,    51,    14,    53,
      48,    97,    14,    51,    48,    97,    49,    51,     8,    49,
      11,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,    37,   159,    29,
     126,    14,    45,   125,   126,    28,    47,   249,    97,   101,
     102,    41,    42,   139,   140,   141,   142,   139,   140,   141,
     142,   143,   144,   145,   146,   147,   148,   149,   150,   151,
     152,   153,   154,   155,   156,   157,    48,     2,   127,     8,
     162,   163,   238,    45,   166,   212,    45,    50,   244,    52,
     139,   140,   141,   142,    50,    48,    52,    22,    48,    31,
       3,     4,    46,    46,    48,    48,   192,    10,    28,    12,
     192,    14,    15,   195,    52,   197,   198,    28,    47,    48,
      49,    46,    51,    48,    53,    46,     8,    48,    31,    47,
      37,   213,    46,    46,    59,    51,    61,    40,    53,    51,
      43,    45,    45,    53,    47,    48,    49,     8,    51,    48,
      53,    47,    48,    49,    79,    51,    47,    53,    53,   241,
     242,   241,   242,   249,   100,    17,   192,   249,    29,   251,
     252,   251,   252,    98,    99,   100,    -1,    -1,    39,    40,
      41,    42,    -1,    -1,    -1,     8,    47,    48,    49,    -1,
      51,    -1,    53,    -1,    -1,   120,    -1,    -1,    -1,    -1,
     125,   126,    -1,     0,     1,    -1,     3,     4,     5,     6,
       7,    -1,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    47,    48,    49,    -1,    51,     8,
      53,    -1,    -1,   158,    31,    -1,    33,    34,    35,    -1,
      37,    -1,    -1,    40,    -1,     8,    43,    -1,    45,    -1,
      -1,    -1,    49,    50,     8,    52,    -1,    -1,    21,    22,
      23,    24,    -1,    26,    27,    -1,    29,   192,    47,    48,
      49,    -1,    51,    -1,    53,    -1,    39,    40,    41,    42,
      -1,    -1,    -1,    -1,    47,    48,    49,   212,    51,     8,
      53,    -1,    -1,    47,    48,    49,    -1,    51,    -1,    53,
      -1,    -1,    21,    22,    23,    24,    -1,    26,    27,     8,
      29,    -1,    -1,   238,    -1,    -1,    -1,    -1,    -1,   244,
      39,    40,    41,    42,   249,     8,    -1,    -1,    47,    48,
      49,    -1,    51,    -1,    53,    -1,    -1,    -1,    21,    22,
      23,    24,     8,    26,    27,    -1,    29,    -1,    47,    48,
      49,    -1,    51,    -1,    53,    -1,    39,    40,    41,    42,
      -1,    -1,    -1,    29,    47,    48,    49,     8,    51,    -1,
      53,    -1,    -1,    39,    40,    41,    42,    -1,    -1,    -1,
      -1,    47,    48,    49,     8,    51,    -1,    53,    29,    47,
      48,    49,    -1,    51,    -1,    53,    -1,    -1,    39,    40,
      41,    42,    -1,    -1,    -1,    29,    47,    48,    49,     8,
      51,    -1,    53,    -1,    -1,    39,    40,    41,    42,    -1,
      -1,    -1,    -1,    47,    48,    49,     8,    51,    -1,    53,
      29,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      39,    40,    41,    42,    -1,    -1,    -1,    29,    47,    48,
      49,     8,    51,    -1,    53,    -1,    -1,    -1,    -1,    41,
      42,    -1,    -1,    -1,    -1,    47,    48,    49,    -1,    51,
      -1,    53,    29,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    41,    42,     8,    -1,    -1,    -1,
      47,    48,    49,    -1,    51,    -1,    53,    -1,    -1,    21,
      22,    23,    24,    -1,    26,    27,    28,    29,    11,    -1,
      -1,    14,    -1,    -1,    36,    37,    38,    39,    40,    41,
      42,    -1,    -1,    -1,    -1,    28,     3,     4,    50,    -1,
      52,    -1,    -1,    10,    37,    12,    -1,    14,    15,    -1,
      -1,    -1,    45,    46,    47,    48,    49,    -1,    51,    -1,
      53,    54,    -1,    -1,    31,    -1,     8,    -1,     3,     4,
      37,    -1,    -1,    40,    -1,    10,    43,    12,    45,    14,
      15,     3,     4,    -1,    -1,    -1,    -1,    29,    10,    -1,
      12,    -1,    14,    15,    -1,    -1,    31,    39,    40,    41,
      42,    -1,    37,    -1,    -1,    40,    -1,    -1,    43,    31,
      45,    -1,    -1,     3,     4,    37,    -1,    -1,    40,    -1,
      10,    43,    12,    45,    14,    15,     3,     4,    -1,    -1,
      -1,    -1,    -1,    10,    -1,    12,    -1,    14,    15,    -1,
      -1,    31,    -1,    -1,    -1,    -1,    -1,    37,    -1,    -1,
      40,    -1,    -1,    43,    31,    45,    -1,    -1,     3,     4,
      37,    -1,    -1,    40,    -1,    10,    43,    12,    45,    14,
      15,     3,     4,    -1,    -1,    -1,    -1,    -1,    10,    -1,
      12,    -1,    14,    15,    -1,    -1,    31,    -1,    -1,    -1,
      -1,    -1,    37,    -1,    -1,    40,    -1,    -1,    43,    31,
      45,     8,    -1,    -1,    -1,    37,    -1,    -1,    40,    -1,
      -1,    43,    -1,    45,    21,    22,    23,    24,    -1,    26,
      27,    28,    29,    -1,    -1,    -1,    -1,     8,    -1,    36,
      37,    38,    39,    40,    41,    42,    -1,    -1,    -1,    46,
      21,    22,    23,    24,    -1,    26,    27,    28,    29,    -1,
      -1,    -1,    -1,     8,    -1,    36,    37,    38,    39,    40,
      41,    42,    -1,    -1,    -1,    46,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,     8,    -1,    -1,    -1,
      -1,    36,    37,    38,    39,    40,    41,    42,    -1,    21,
      22,    23,    24,    -1,    26,    27,    28,    29,     8,    -1,
      -1,    -1,    -1,    -1,    36,    37,    38,    39,    40,    41,
      42,    21,    22,    23,    24,    -1,    26,    27,    28,    29,
       8,    -1,    -1,    -1,    -1,    -1,    36,    37,    38,    39,
      40,    41,    42,    21,    22,    23,    24,    -1,    26,    27,
      28,    29,     8,    -1,    -1,    -1,    -1,    -1,    36,    37,
      38,    39,    40,    41,    42,    21,    22,    23,    24,    -1,
      26,    27,    28,    29,     8,    -1,    -1,    -1,    -1,    -1,
      36,    37,    38,    39,    40,    41,    42,    21,    22,    23,
      24,    -1,    26,    27,    -1,    29,     8,    -1,    -1,    -1,
      -1,    -1,    36,    37,    38,    39,    40,    41,    42,    21,
      22,    23,    24,    -1,    26,    27,    -1,    29,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    39,    40,    41,
      42
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    56,    57,     0,     1,     3,     4,     5,     6,     7,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    31,    33,    34,    35,    37,    40,    43,    45,
      58,    59,    60,    61,    62,    66,    67,    68,    73,    74,
      75,    79,    81,    85,    86,    49,    12,    14,    45,    69,
      70,    73,    83,    50,    52,    50,    52,    83,    14,    45,
       4,    12,    37,    40,    43,    45,    63,    64,    65,    70,
      74,    75,    81,    82,    10,    14,    37,    81,    87,    45,
      14,    15,    79,    83,    83,    83,    73,    83,    83,    83,
      81,    83,    81,    81,    81,    49,    49,    11,    37,    14,
      47,    50,    52,     8,    21,    22,    23,    24,    26,    27,
      28,    29,    36,    37,    38,    39,    40,    41,    42,    73,
      12,    73,    78,    80,    81,    28,    47,    48,    83,    84,
      84,    84,    84,    77,    78,    81,    81,    81,    81,    48,
      48,    50,    52,    21,    22,    23,    24,    26,    27,    28,
      29,    36,    37,    38,    39,    40,    41,    42,    45,    52,
      76,    77,    28,    28,    81,    31,    37,    37,    46,    63,
      73,    73,    68,    73,    83,    83,    81,    81,    81,    81,
      81,    81,    81,    81,    81,    81,    81,    81,    81,    81,
      81,    81,    47,    10,    78,    37,    46,    25,    30,    72,
      73,    81,    71,    73,    81,    82,    70,    51,    53,    51,
      53,    46,    48,    37,    46,    70,    82,    63,    64,    64,
      81,    81,    81,    81,    81,    81,    81,    81,    81,    81,
      81,    81,    81,    81,    81,    77,    84,    46,    54,    81,
      81,    51,    53,    71,    45,    46,    81,    81,    81,    47,
      78,    51,    53,    46,    53,    77,    74,    74,    77,    73,
      82,    74,    74,    46
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 110 "parse.yy"
    { logic_program = (yyvsp[(1) - (1)].pn); ;}
    break;

  case 3:
#line 114 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_RULES, NULL, (yyvsp[(2) - (3)].pn), (yyvsp[(1) - (3)].pn), lineno); ;}
    break;

  case 4:
#line 116 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_RULES, NULL, (yyvsp[(2) - (3)].pn), (yyvsp[(1) - (3)].pn), lineno); ;}
    break;

  case 5:
#line 118 "parse.yy"
    { yyerrok; ;}
    break;

  case 6:
#line 120 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_RULES, NULL, (yyvsp[(2) - (2)].pn), (yyvsp[(1) - (2)].pn), lineno); ;}
    break;

  case 7:
#line 122 "parse.yy"
    { (yyval.pn) = NULL; ;}
    break;

  case 8:
#line 126 "parse.yy"
    { (yyval.pn) = (yyvsp[(1) - (1)].pn); ;}
    break;

  case 9:
#line 128 "parse.yy"
    { (yyval.pn) = (yyvsp[(1) - (1)].pn); ;}
    break;

  case 10:
#line 130 "parse.yy"
    { (yyval.pn) = (yyvsp[(1) - (1)].pn); ;}
    break;

  case 11:
#line 134 "parse.yy"
    {read_option((yyvsp[(1) - (1)].s)); (yyval.pn) = NULL; ;}
    break;

  case 12:
#line 138 "parse.yy"
    { function_table->Insert((yyvsp[(2) - (2)].s)); (yyval.pn) = NULL; delete (yyvsp[(2) - (2)].s);;}
    break;

  case 13:
#line 140 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_DECLARATION, NULL, NULL, (yyvsp[(2) - (2)].pn), 
	                         lineno); ;}
    break;

  case 14:
#line 143 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_HIDE, NULL, (yyvsp[(2) - (2)].pn), NULL, lineno); ;}
    break;

  case 15:
#line 145 "parse.yy"
    { (yyval.pn) = NULL; sys_data.hide_all = 1; ;}
    break;

  case 16:
#line 147 "parse.yy"
    { 
              lval1 = (yyvsp[(4) - (4)].pn)->Eval();
              if (lval1 == ERROR_NUMBER) {
                 error(USR_ERR, "%s: Invalid constant declaration: ",
                       error_file_and_line(lineno));
                 fprintf(stderr, "\t\tconst %s = ", (yyvsp[(2) - (4)].s));
                 (yyvsp[(4) - (4)].pn)->PrintExpression();
                 fprintf(stderr, ".\n   The expression is not a"
    	                 " valid constant expression\n");
              }	 
              long index;
              int needed = 0;
              int exists = 0;

              if ((index = numeric_constant_table->LookIndex((yyvsp[(2) - (4)].s))) >= 0) { 
	              exists = 1;
    		      needed = 1;
                  if (lval1 != numeric_constant_table->Lookup((yyvsp[(2) - (4)].s))) { 
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
                  numeric_constant_table->Insert((yyvsp[(2) - (4)].s));
                  numeric_constant_table->SetValue((yyvsp[(2) - (4)].s), lval1);
              }
              if (needed) {
	          warn(WARN_SIMILAR, lineno, "the numeric constant '%s'"
	               " has two values defined for it, '%ld' and"
		       " '%ld'", (yyvsp[(2) - (4)].s), numeric_constant_table->Lookup((yyvsp[(2) - (4)].s)), lval1);
              }
              if (enabled_warning(WARN_SIMILAR) && (constant_table->Lookup((yyvsp[(2) - (4)].s)) >= 0)) { 
                 warn(WARN_SIMILAR, lineno, "the constant '%s' is used as "
		     "both numeric and symbolic constant", (yyvsp[(2) - (4)].s));
              }
	      delete [] (yyvsp[(2) - (4)].s);
              delete (yyvsp[(4) - (4)].pn);
              (yyval.pn) = NULL; ;}
    break;

  case 17:
#line 195 "parse.yy"
    { yyerror("numeric constant declarations now need a '='");
	      (yyval.pn) = NULL;
	      delete [] (yyvsp[(2) - (3)].pn);
              delete (yyvsp[(3) - (3)].pn);
	    ;}
    break;

  case 18:
#line 201 "parse.yy"
    {
              error(USR_ERR, "%s: Invalid constant declaration: ",
                    error_file_and_line(lineno));
              fprintf(stderr, "\t\tconst %s = ", (yyvsp[(2) - (4)].s));
              (yyvsp[(4) - (4)].pn)->PrintExpression();
              fprintf(stderr, ".\n   The symbol '%s' is a variable, not"
	              " a constant\n", (yyvsp[(2) - (4)].s));
              (yyval.pn) = NULL;
              delete (yyvsp[(2) - (4)].s);
              delete (yyvsp[(4) - (4)].pn);
            ;}
    break;

  case 19:
#line 213 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXTERNAL, NULL, (yyvsp[(2) - (2)].pn), NULL, lineno);
	    ;}
    break;

  case 20:
#line 216 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_SHOW, NULL, (yyvsp[(2) - (2)].pn), NULL, lineno); ;}
    break;

  case 21:
#line 218 "parse.yy"
    { sys_data.has_implicit_domains = 1; (yyval.pn) = new
              ParseNode(PT_DIRECTIVE, NULL, NULL, (yyvsp[(2) - (2)].pn), lineno) ; ;}
    break;

  case 22:
#line 221 "parse.yy"
    { (yyval.pn) = NULL; ;}
    break;

  case 23:
#line 223 "parse.yy"
    { (yyval.pn) = NULL; ;}
    break;

  case 24:
#line 225 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_DIRECTIVE, NULL, NULL, (yyvsp[(2) - (2)].pn), lineno, 1); ;}
    break;

  case 25:
#line 229 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_RULE, NULL, (yyvsp[(1) - (1)].pn), NULL, lineno); ;}
    break;

  case 26:
#line 231 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_RULE, NULL, (yyvsp[(1) - (3)].pn), (yyvsp[(3) - (3)].pn), lineno); ;}
    break;

  case 27:
#line 233 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_RULE, NULL, NULL, (yyvsp[(2) - (2)].pn), lineno); ;}
    break;

  case 28:
#line 237 "parse.yy"
    { (yyval.pn) = (yyvsp[(1) - (1)].pn); ;}
    break;

  case 29:
#line 239 "parse.yy"
    { (yyval.pn) = (yyvsp[(1) - (1)].pn); ;}
    break;

  case 30:
#line 241 "parse.yy"
    { (yyval.pn) = NULL; ;}
    break;

  case 31:
#line 245 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_TAIL, NULL, (yyvsp[(1) - (3)].pn), (yyvsp[(3) - (3)].pn), lineno); ;}
    break;

  case 32:
#line 247 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_TAIL, NULL, (yyvsp[(1) - (1)].pn), NULL, lineno); ;}
    break;

  case 33:
#line 249 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_TAIL, NULL, (yyvsp[(1) - (1)].pn), NULL, lineno); ;}
    break;

  case 34:
#line 253 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_BASIC_TAIL, NULL, (yyvsp[(3) - (3)].pn), (yyvsp[(1) - (3)].pn), lineno);;}
    break;

  case 35:
#line 255 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_BASIC_TAIL, NULL, (yyvsp[(1) - (1)].pn), NULL, lineno);;}
    break;

  case 36:
#line 257 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_BASIC_TAIL, NULL, (yyvsp[(3) - (3)].pn), (yyvsp[(1) - (3)].pn), lineno);
             if ((yyvsp[(3) - (3)].pn)->type == PT_CONSTANT) (yyvsp[(3) - (3)].pn)->type = PT_ATOM; ;}
    break;

  case 37:
#line 260 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_BASIC_TAIL, NULL, (yyvsp[(1) - (1)].pn), NULL,
                                lineno); ;}
    break;

  case 38:
#line 265 "parse.yy"
    { tmp_node = new ParseNode(PT_BOUND, NULL, (yyvsp[(1) - (5)].pn), (yyvsp[(5) - (5)].pn),
	                                lineno); 
               (yyval.pn) = new ParseNode(PT_SPECIAL_TAIL, NULL, tmp_node, (yyvsp[(3) - (5)].pn),
  	                          lineno, 0, 0, 1); ;}
    break;

  case 39:
#line 270 "parse.yy"
    { tmp_node = new ParseNode(PT_BOUND, NULL, (yyvsp[(1) - (5)].pn), (yyvsp[(5) - (5)].pn),
	                                lineno);
               (yyval.pn) = new ParseNode(PT_SPECIAL_TAIL, NULL, tmp_node, (yyvsp[(3) - (5)].pn),
  	                          lineno); ;}
    break;

  case 40:
#line 277 "parse.yy"
    { tmp_node = new ParseNode(PT_BOUND, NULL, (yyvsp[(1) - (5)].pn), (yyvsp[(5) - (5)].pn), lineno);
               (yyval.pn) = new ParseNode(PT_SPECIAL_HEAD, NULL, tmp_node, (yyvsp[(3) - (5)].pn),
  	                          lineno); ;}
    break;

  case 41:
#line 281 "parse.yy"
    { tmp_node = new ParseNode(PT_BOUND, NULL, (yyvsp[(1) - (5)].pn), (yyvsp[(5) - (5)].pn), lineno);
			 	       (yyval.pn) = new ParseNode(PT_SPECIAL_HEAD, NULL, tmp_node, (yyvsp[(3) - (5)].pn),
  	         		                    lineno, 0, 0,1); ;}
    break;

  case 42:
#line 285 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_SPECIAL_HEAD, NULL, NULL, (yyvsp[(1) - (1)].pn), 
  	                          lineno, 0, 1);  ;}
    break;

  case 43:
#line 288 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_ORDERED_DISJUNCTION, NULL, NULL,
                         	  (yyvsp[(1) - (1)].pn), lineno);
         ;}
    break;

  case 44:
#line 292 "parse.yy"
    {  
           (yyval.pn) = new ParseNode(PT_ORDERED_DISJUNCTION, NULL, (yyvsp[(1) - (3)].pn),
               (yyvsp[(3) - (3)].pn), lineno);
         ;}
    break;

  case 45:
#line 297 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_SPECIAL_HEAD, NULL, NULL, (yyvsp[(2) - (3)].pn), 0, 1,
  	            lineno); ;}
    break;

  case 46:
#line 304 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_LITERAL_LIST, NULL, (yyvsp[(3) - (3)].pn), (yyvsp[(1) - (3)].pn), lineno); ;}
    break;

  case 47:
#line 306 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_LITERAL_LIST, NULL, (yyvsp[(1) - (1)].pn), NULL,
	   lineno);  ;}
    break;

  case 48:
#line 311 "parse.yy"
    { if (strcmp((yyvsp[(2) - (3)].s), "x")) {
  	      yyerror("parse error");
          (yyval.pn) = NULL;
       } else {
          (yyval.pn) = new ParseNode(PT_LITERAL_LIST, NULL, (yyvsp[(3) - (3)].pn), (yyvsp[(1) - (3)].pn), lineno);
       }
     ;}
    break;

  case 49:
#line 319 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_LITERAL_LIST, NULL, (yyvsp[(1) - (1)].pn), NULL,
	   lineno);  ;}
    break;

  case 50:
#line 325 "parse.yy"
    { (yyval.pn) = (yyvsp[(1) - (1)].pn); ;}
    break;

  case 51:
#line 327 "parse.yy"
    { (yyvsp[(2) - (2)].pn)->l2.negative = 1; (yyval.pn) = (yyvsp[(2) - (2)].pn); ;}
    break;

  case 52:
#line 329 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_CONDITION, NULL, (yyvsp[(1) - (3)].pn), (yyvsp[(3) - (3)].pn), lineno); ;}
    break;

  case 53:
#line 331 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_CONDITION, NULL, (yyvsp[(2) - (4)].pn), (yyvsp[(4) - (4)].pn), lineno);
	  (yyval.pn)->l2.negative = 1;;}
    break;

  case 54:
#line 336 "parse.yy"
    { (yyval.pn) = (yyvsp[(1) - (1)].pn); ;}
    break;

  case 55:
#line 338 "parse.yy"
    { (yyval.pn) = (yyvsp[(1) - (3)].pn); if ((yyvsp[(1) - (3)].pn)->type != PT_CONDITION) (yyval.pn)->left = (yyvsp[(3) - (3)].pn);
                   else (yyval.pn)->left->left = (yyvsp[(3) - (3)].pn); ;}
    break;

  case 56:
#line 343 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_LITERAL_LIST, NULL, (yyvsp[(3) - (3)].pn), (yyvsp[(1) - (3)].pn), lineno); ;}
    break;

  case 57:
#line 345 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_LITERAL_LIST, NULL, (yyvsp[(1) - (1)].pn), NULL,
	   lineno);  ;}
    break;

  case 58:
#line 348 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_LITERAL_LIST, NULL, (yyvsp[(1) - (1)].pn), NULL,
	   lineno);  ;}
    break;

  case 59:
#line 351 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_LITERAL_LIST, NULL, (yyvsp[(3) - (3)].pn), (yyvsp[(1) - (3)].pn), lineno); ;}
    break;

  case 60:
#line 355 "parse.yy"
    { (yyval.pn) =(yyvsp[(1) - (1)].pn); ;}
    break;

  case 61:
#line 357 "parse.yy"
    { (yyval.pn) = (yyvsp[(1) - (1)].pn); ;}
    break;

  case 62:
#line 363 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_ATOM, (yyvsp[(1) - (4)].s), NULL, (yyvsp[(3) - (4)].pn), lineno); ;}
    break;

  case 63:
#line 365 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_ATOM, (yyvsp[(1) - (1)].s), NULL, NULL, lineno); ;}
    break;

  case 64:
#line 369 "parse.yy"
    { (yyval.pn) = (yyvsp[(1) - (1)].pn); ;}
    break;

  case 65:
#line 371 "parse.yy"
    { (yyval.pn) = NULL; ;}
    break;

  case 66:
#line 376 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_FUNCTION, (yyvsp[(1) - (4)].s), NULL, (yyvsp[(3) - (4)].pn), lineno);;}
    break;

  case 67:
#line 378 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_FUNCTION, (yyvsp[(2) - (5)].s), NULL, (yyvsp[(4) - (5)].pn), lineno);
           (yyval.pn)->l2.negative = 1; ;}
    break;

  case 68:
#line 381 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, NULL, (yyvsp[(3) - (4)].pn), lineno,
	                      FUN_WEIGHT); ;}
    break;

  case 69:
#line 384 "parse.yy"
    { (yyvsp[(4) - (5)].pn)->l2.negative = 1;
           (yyval.pn) = new ParseNode(PT_EXPR, NULL, NULL, (yyvsp[(4) - (5)].pn), lineno,
	                      FUN_WEIGHT); ;}
    break;

  case 70:
#line 390 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_ARGUMENTS, NULL, (yyvsp[(1) - (1)].pn), NULL, lineno);;}
    break;

  case 71:
#line 392 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_ARGUMENTS, NULL, (yyvsp[(3) - (3)].pn), (yyvsp[(1) - (3)].pn), lineno); ;}
    break;

  case 72:
#line 396 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_ARGLIST, NULL, (yyvsp[(3) - (3)].pn), (yyvsp[(1) - (3)].pn), lineno); ;}
    break;

  case 73:
#line 398 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_ARGLIST, NULL, (yyvsp[(1) - (1)].pn), NULL , lineno); ;}
    break;

  case 74:
#line 402 "parse.yy"
    { (yyval.pn) = (yyvsp[(1) - (1)].pn); ;}
    break;

  case 75:
#line 404 "parse.yy"
    { lval1 = (yyvsp[(1) - (1)].pn)->Eval();
       if (lval1 != ERROR_NUMBER) {
           (yyval.pn) = new ParseNode(PT_NUMBER, NULL, NULL, NULL,
                lineno,lval1);
           delete (yyvsp[(1) - (1)].pn);
       } else {
         (yyval.pn) = (yyvsp[(1) - (1)].pn);
       }
     ;}
    break;

  case 76:
#line 414 "parse.yy"
    { (yyval.pn) = (yyvsp[(1) - (1)].pn);
       if ((yyval.pn)->right == NULL) {
         lval1 = numeric_constant_table->Lookup((yyval.pn)->sval);
         if (lval1 != ERROR_NUMBER) {
            (yyval.pn)->type = PT_NUMBER;
            (yyval.pn)->l1.val = lval1;
         } else {
          (yyval.pn)->type = PT_CONSTANT;
         }
       }
     ;}
    break;

  case 77:
#line 426 "parse.yy"
    {
      /* crude workaround for situations like a | 1, where 'a' is
	 actually a numeric constant */
       if ( (yyvsp[(1) - (3)].pn)->right ) {
	error(USR_ERR, "%s: parse error before '|'\n",
              error_file_and_line(lineno));
       }
       (yyval.pn) = new ParseNode(PT_EXPR, NULL, (yyvsp[(1) - (3)].pn), (yyvsp[(3) - (3)].pn), lineno, FUN_OR);
       lval1 = numeric_constant_table->Lookup((yyvsp[(1) - (3)].pn)->sval);
       if (lval1 != ERROR_NUMBER) {
         delete (yyval.pn)->left;
         (yyval.pn)->left = new ParseNode(PT_NUMBER, NULL, NULL, NULL, lineno,
                                  lval1);
       } else {
         (yyvsp[(1) - (3)].pn)->type = PT_CONSTANT;
       }
     ;}
    break;

  case 78:
#line 446 "parse.yy"
    { lval1 = numeric_constant_table->Lookup((yyvsp[(1) - (1)].s));
	  if (lval1 == ERROR_NUMBER)  
		(yyval.pn) = new ParseNode(PT_CONSTANT, (yyvsp[(1) - (1)].s), NULL, NULL,
                                   lineno);
	  else {
	     (yyval.pn) = new ParseNode(PT_NUMBER, NULL, NULL, NULL, lineno,lval1);
             delete [] (yyvsp[(1) - (1)].s); }
	;}
    break;

  case 79:
#line 455 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_NUMBER, NULL, NULL, NULL, lineno,
	 (yyvsp[(1) - (1)].l));;}
    break;

  case 80:
#line 460 "parse.yy"
    { 
        (yyval.pn) = new ParseNode(PT_RANGE, NULL, (yyvsp[(1) - (3)].pn), (yyvsp[(3) - (3)].pn), lineno);
      ;}
    break;

  case 81:
#line 464 "parse.yy"
    {
        (yyval.pn) = NULL;
	yyerror("conjunctive ranges are no longer supported, "
	        "please use conditions instead");
        /* lval1 = $1->Eval();
        lval2 = $3->Eval();
  	$$ = new ParseNode(PT_RANGE, NULL, NULL, NULL, lineno, lval1,
                           lval2, 1); */;}
    break;

  case 82:
#line 475 "parse.yy"
    { (yyval.pn) = (yyvsp[(2) - (3)].pn); (yyval.pn)->i1.paren =1; ;}
    break;

  case 83:
#line 477 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, NULL, (yyvsp[(2) - (3)].pn), lineno,
	                  FUN_ABS); (yyval.pn)->i1.paren = 1;;}
    break;

  case 84:
#line 480 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, NULL, (yyvsp[(2) - (3)].pn), lineno,
                          FUN_NORM); ;}
    break;

  case 85:
#line 483 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, (yyvsp[(1) - (3)].pn), (yyvsp[(3) - (3)].pn), lineno, FUN_EXP); ;}
    break;

  case 86:
#line 485 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, NULL, (yyvsp[(2) - (2)].pn) , lineno,
                          FUN_MINUS);
       (yyval.pn)->left = new ParseNode(PT_NUMBER, NULL, NULL, NULL, lineno, 0);;}
    break;

  case 87:
#line 489 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, (yyvsp[(1) - (3)].pn), (yyvsp[(3) - (3)].pn), lineno, FUN_PLUS); ;}
    break;

  case 88:
#line 491 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, (yyvsp[(1) - (3)].pn), (yyvsp[(3) - (3)].pn), lineno, FUN_MINUS); ;}
    break;

  case 89:
#line 493 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, (yyvsp[(1) - (3)].pn), (yyvsp[(3) - (3)].pn), lineno, FUN_TIMES); ;}
    break;

  case 90:
#line 495 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, (yyvsp[(1) - (3)].pn), (yyvsp[(3) - (3)].pn), lineno, FUN_DIV); ;}
    break;

  case 91:
#line 497 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, (yyvsp[(1) - (3)].pn), (yyvsp[(3) - (3)].pn), lineno, FUN_MOD); ;}
    break;

  case 92:
#line 499 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, (yyvsp[(1) - (3)].pn), (yyvsp[(3) - (3)].pn), lineno, FUN_EQ); ;}
    break;

  case 93:
#line 501 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, (yyvsp[(1) - (3)].pn), (yyvsp[(3) - (3)].pn), lineno, FUN_NEQ); ;}
    break;

  case 94:
#line 503 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, (yyvsp[(1) - (3)].pn), (yyvsp[(3) - (3)].pn), lineno, FUN_LT); ;}
    break;

  case 95:
#line 505 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, (yyvsp[(1) - (3)].pn), (yyvsp[(3) - (3)].pn), lineno, FUN_LE); ;}
    break;

  case 96:
#line 507 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, (yyvsp[(1) - (3)].pn), (yyvsp[(3) - (3)].pn), lineno, FUN_GT); ;}
    break;

  case 97:
#line 509 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, (yyvsp[(1) - (3)].pn), (yyvsp[(3) - (3)].pn), lineno, FUN_GE); ;}
    break;

  case 98:
#line 511 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, (yyvsp[(2) - (2)].pn) ,NULL , lineno,
                          FUN_NOT);
       (yyval.pn)->right = new ParseNode(PT_NUMBER, NULL, NULL, NULL, lineno,
                                 0);;}
    break;

  case 99:
#line 516 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, (yyvsp[(1) - (3)].pn), (yyvsp[(3) - (3)].pn), lineno, FUN_AND); ;}
    break;

  case 100:
#line 518 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, (yyvsp[(1) - (3)].pn), (yyvsp[(3) - (3)].pn), lineno, FUN_OR); ;}
    break;

  case 101:
#line 520 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, (yyvsp[(1) - (3)].pn), (yyvsp[(3) - (3)].pn), lineno, FUN_XOR); ;}
    break;

  case 102:
#line 522 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, (yyvsp[(1) - (3)].pn), (yyvsp[(3) - (3)].pn), lineno,
                         FUN_ASSIGN); ;}
    break;

  case 103:
#line 525 "parse.yy"
    { (yyval.pn) = (yyvsp[(1) - (1)].pn); ;}
    break;

  case 104:
#line 527 "parse.yy"
    { (yyval.pn) = (yyvsp[(1) - (1)].pn); ;}
    break;

  case 105:
#line 529 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_VARIABLE, (yyvsp[(1) - (1)].s), NULL, NULL, lineno); ;}
    break;

  case 106:
#line 533 "parse.yy"
    { (yyval.pn) = (yyvsp[(2) - (3)].pn); ;}
    break;

  case 107:
#line 535 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, NULL, (yyvsp[(2) - (3)].pn), lineno,
	                  FUN_ABS);;}
    break;

  case 108:
#line 538 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, NULL, (yyvsp[(2) - (2)].pn) , lineno,
                          FUN_MINUS);
       (yyval.pn)->left = new ParseNode(PT_NUMBER, NULL, NULL, NULL, lineno, 0);;}
    break;

  case 109:
#line 542 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, (yyvsp[(1) - (3)].pn), (yyvsp[(3) - (3)].pn), lineno, FUN_PLUS); ;}
    break;

  case 110:
#line 544 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, (yyvsp[(1) - (3)].pn), (yyvsp[(3) - (3)].pn), lineno, FUN_MINUS); ;}
    break;

  case 111:
#line 546 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, (yyvsp[(1) - (3)].pn), (yyvsp[(3) - (3)].pn), lineno, FUN_TIMES); ;}
    break;

  case 112:
#line 548 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, (yyvsp[(1) - (3)].pn), (yyvsp[(3) - (3)].pn), lineno, FUN_DIV); ;}
    break;

  case 113:
#line 550 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, (yyvsp[(1) - (3)].pn), (yyvsp[(3) - (3)].pn), lineno, FUN_MOD); ;}
    break;

  case 114:
#line 552 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, (yyvsp[(1) - (3)].pn), (yyvsp[(3) - (3)].pn), lineno, FUN_EQ); ;}
    break;

  case 115:
#line 554 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, (yyvsp[(1) - (3)].pn), (yyvsp[(3) - (3)].pn), lineno, FUN_NEQ); ;}
    break;

  case 116:
#line 556 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, (yyvsp[(1) - (3)].pn), (yyvsp[(3) - (3)].pn), lineno, FUN_LT); ;}
    break;

  case 117:
#line 558 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, (yyvsp[(1) - (3)].pn), (yyvsp[(3) - (3)].pn), lineno, FUN_LE); ;}
    break;

  case 118:
#line 560 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, (yyvsp[(1) - (3)].pn), (yyvsp[(3) - (3)].pn), lineno, FUN_GT); ;}
    break;

  case 119:
#line 562 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, (yyvsp[(1) - (3)].pn), (yyvsp[(3) - (3)].pn), lineno, FUN_GE); ;}
    break;

  case 120:
#line 564 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, (yyvsp[(2) - (2)].pn) ,NULL , lineno,
                          FUN_NOT);
       (yyval.pn)->right = new ParseNode(PT_NUMBER, NULL, NULL, NULL, lineno,
                                 0);;}
    break;

  case 121:
#line 569 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, (yyvsp[(1) - (3)].pn), (yyvsp[(3) - (3)].pn), lineno, FUN_AND); ;}
    break;

  case 122:
#line 571 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, (yyvsp[(1) - (3)].pn), (yyvsp[(3) - (3)].pn), lineno, FUN_OR); ;}
    break;

  case 123:
#line 573 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL, (yyvsp[(1) - (3)].pn), (yyvsp[(3) - (3)].pn), lineno, FUN_XOR); ;}
    break;

  case 124:
#line 575 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_EXPR, NULL,  (yyvsp[(1) - (3)].pn), (yyvsp[(3) - (3)].pn), lineno,
                          FUN_ASSIGN);;}
    break;

  case 125:
#line 578 "parse.yy"
    { (yyval.pn) = (yyvsp[(1) - (1)].pn); ;}
    break;

  case 126:
#line 583 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_LITERAL_LIST, NULL, (yyvsp[(3) - (3)].pn), (yyvsp[(1) - (3)].pn), lineno);;}
    break;

  case 127:
#line 585 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_LITERAL_LIST, NULL, (yyvsp[(1) - (1)].pn), NULL, lineno);;}
    break;

  case 128:
#line 589 "parse.yy"
    { (yyval.pn) = (yyvsp[(1) - (1)].pn); ;}
    break;

  case 129:
#line 591 "parse.yy"
    { (yyval.pn) = 0; ;}
    break;

  case 130:
#line 596 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_COMPUTE, NULL, NULL, (yyvsp[(4) - (5)].pn),
	                          lineno, (yyvsp[(2) - (5)].l));;}
    break;

  case 131:
#line 601 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_MAXIMIZE, NULL, NULL, (yyvsp[(3) - (4)].pn),
	                          lineno); ;}
    break;

  case 132:
#line 604 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_MAXIMIZE, NULL, NULL, (yyvsp[(3) - (4)].pn),
	                          lineno, 0, 0, 1); ;}
    break;

  case 133:
#line 607 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_MINIMIZE, NULL, NULL, (yyvsp[(3) - (4)].pn),
	                          lineno); ;}
    break;

  case 134:
#line 610 "parse.yy"
    { (yyval.pn) = new ParseNode(PT_MINIMIZE, NULL, NULL, (yyvsp[(3) - (4)].pn),
	                          lineno,0,0,1); ;}
    break;

  case 135:
#line 615 "parse.yy"
    {
         if ((yyvsp[(1) - (1)].pn)->type == PT_CONSTANT) {
           if (!strcmp((yyvsp[(1) - (1)].pn)->sval, "all")) {
             (yyval.l) = 0;
           } else {
             error(USR_ERR, "%s: invalid number of models. Use a "
                   "positive integer, a constant valued expression, "
                   "or \"all\"", 
                   error_file_and_line(lineno));
                   (yyval.l) = 1;
           }
         } else {
           (yyval.l) = (yyvsp[(1) - (1)].pn)->Eval();
           if ((yyval.l) == ERROR_NUMBER || (yyval.l) < 0) {
             error(USR_ERR, "%s: invalid number of models. Use a "
                  "positive integer, a constant valued expression, or "
	          "\"all\"", error_file_and_line(lineno));
           }
         }
         delete (yyvsp[(1) - (1)].pn);
       ;}
    break;

  case 136:
#line 637 "parse.yy"
    { (yyval.l) = 1; ;}
    break;


/* Line 1267 of yacc.c.  */
#line 2683 "parse.cc"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



