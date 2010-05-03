// literal.h -- declarations for literals and special rule sections
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

#ifndef LITERAL_H
#define LITERAL_H

#include "config.h"
#ifndef DEFINES_H
#include "defines.h"
#endif
#ifndef TERM_H
#include "term.h"
#endif
#ifndef LIST_H
#include "list.h"
#endif
#ifndef INSTANCE_H
#include "instance.h"
#endif
#ifndef ITERATE_H
#include "iterate.h"
#endif

struct Weight;

class Literal
{
public:
  int special : 1; // true, if literal is a special tail
  int ground : 1; 	// true if ground literal
  int negative : 1;	// true if negative literal
  int has_range : 1; // true if one of arguments is a range
  int has_function : 1; // ditto for functions
  int has_atom : 1; // true if one of the arguments is another atom
  int deleted : 1;
  int inverted : 1;
  int is_warned : 1; // true, if we have warned about missing weights
  // before
  int has_global_condition : 1;
  int ignore_global_condition : 1;
  int empty_condition : 1; // true, if expansion of condition is empty
  int is_symbolic_function : 1;
  long pred; 	// predicate symbol of the literal
  long lineno;
  int arity ;	// arity of the literal
  int numvars;	// how many variables literal has

  TermType *args;	// array of types of arguments
  // arrays for each possible argument type. vars, cons have index to
  // symbol table or -1, nums have the numbers or -1 and terms has
  // pointers to corresponding terms or NULL
  long *vars;
  Instance *cons;
  Term **terms;

  LiteralList *conditions;
  FunctionList *condition_func;
  Weight *weight;
  Weight *dynamic_weight;
  
  // returns pointer to statically allocated buffer which contains the
  // text representation of the literal. The buffer will be clobbered
  // by subsequent calls.
  const char *GetPrintString(int first_pos = 0);
  const char *GetInstanceString();
  
  Literal(ParseNode *, long, int);
  Literal();
  Literal(Variable, long);
  Literal(Variable *, long, int);
  Literal(long p, Instance *it, int ar, int neg);
  virtual ~Literal();
  
  virtual Literal *Duplicate(int force_negative = 0, int include_conditions = 1);
  void CountSpecialVars();
  
  int GetPos();
  void CreateInstance(int);

  // return a copy of the literal with instantiated variables 
  Literal *Instantiate();

  // return the atom value of emitted thingy
  Instance EmitGround(int print_weight = 0); 
  Instance EmitComplement();
  Instance GetComplement();
  void Restrict(int);
  int Test();
  void Simplify(FunctionList *func_list = NULL);
  void Print(int  print_weight = 0);
  Weight *WeightDefined(); // is there some weigth for this literal
  void FindWeight(); // try to set a weight definition for this
		     // literal

  void EmitWeight(int inverted);
  void SetWeight(long value);  // changes the weight to integral value
  
  int Matches(Literal *, Literal *);
  int Substitute(Variable, Variable, int);
  int AddVars(Variable *);
  void AddVarsByType(Variable *, int in_condition = 0);
  Instance *GetInstance();

  long GetWeight(); // returns the weight of this particular instance. 
  
  static int BindLiteral(Literal *, Instance *, int);
  int DomainLiteral();
  virtual Literal *CreateComplement();

  int NeedsSimplifying(); // does this literal need simplifying
  const char *GetGroundInstance();
};

struct AtomData
{
  AtomData(); 
  void Clear();
  long modified_atleast;
  long modified_atmost;
  int has_negative_weights;
  int removed_positive_domains;
  int removed_negative_domains;
  int inverted_pos;
  int inverted_neg;
  long total_weight;
  long removed_weight;
  long weight_left;
};


class SpecialLiteral : public Literal
{
public:
  SpecialLiteral(RuleType, Term * , Term *, long);
  ~SpecialLiteral();
  RuleType type;
  LiteralList tail;
  LiteralList negs;
  LiteralList inverted_pos, inverted_neg;
  LiteralList positive_conditions;
  LiteralList negative_conditions;
  FunctionList funs;
  Term *atleast;
  Term *atmost;
  int dynamic_atleast : 1;
  int weight_atleast : 1; 
  int negatives_counted : 1;
  
  long pred2; // for weightrules
  Literal *atmost_lit;
  long atom1; // the new atoms to use as heads during grounding
  long atom2;
  int num_conditions;
  int num_positive_conditions;
  int num_negative_conditions;
  AtomData weight_info;
  void RestrictSpecial();
  void PrintSpecial();
  Instance EmitLiteral();
  void FindWeights();
  void EmitRule();
  void EmitSmodels2(int);
  void EmitText(int);
  void AddVariables();
  void CheckWeights();
  virtual SpecialLiteral *CreateComplement();
  virtual Literal *Duplicate(int force_negative = 0, int include_conditions = 1);
};

  
enum WeightType { WT_NUMBER, WT_VARIABLE, WT_EXPR, WT_ATOM };

struct Weight
{
  Weight() { type=WT_NUMBER; v.val=ERROR_NUMBER;} 
  ~Weight() { if (type== WT_EXPR) delete v.fun; }
  WeightType type;
  union {
    long val;
    Variable var;
    Function *fun;
    Literal *lit;
  } v;
  long lineno;
  void Emit(int invert);
  long GetValue();
  
  // instantiates those variables in weight expression that are
  // defined in 'lt' which matches pattern 'pattern' 
  Weight *Instantiate(Literal *lt, Literal *pattern);

  // to be used with condition expansion
  Weight *Instantiate();

  // define a weight in terms of another literal
  Weight *TieWeight(Literal *lt, Literal *source);
};


// The weight list is maintained by hand all over the place
struct WeightNode
{
  Weight *wt;
  Literal *lt;
  struct WeightNode *prev;
};


struct ComputeStmt
{
  long models;
  int command_line;
  Rule *rl;
  LongList positive_atoms;
  LongList negative_atoms;
  ComputeStmt() {command_line = 0; models = 1; rl = NULL; }
  ~ComputeStmt();
};


#endif
