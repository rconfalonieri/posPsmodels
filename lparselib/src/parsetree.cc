// parsetree.cc -- implementation of parse tree structure for lparse
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
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef PARSETREE_H
#include "parsetree.h"
#endif
#ifndef GLOBAL_H
#include "global.h"
#endif
#ifndef PREDICATE_H
#include "predicate.h"
#endif
#ifndef RULE_H
#include "rule.h"
#endif
#ifndef DEBUG_H
#include "debug.h"
#endif
#ifndef GRAPH_H
#include "graph.h"
#endif
#ifndef SYMBOL_H
#include "symbol.h"
#endif
#ifndef ERROR_H
#include "error.h"
#endif
#include "priority.h"



Literal *false_lit = NULL;


//define strings that correspond to node types 
char *parse_strings[] = { "PT_RULES", "PT_COMPUTE", "PT_DECLARATION",
                          "PT_RULE", "PT_HEAD", "PT_TAIL", "PT_SPECIAL_TAIL",
                          "PT_BASIC_TAIL", "PT_ATOM", "PT_FUNCTION",
                          "PT_ARGLIST", "PT_TERM", "PT_CONSTANT",
                          "PT_RANGE", "PT_EXPR", "PT_LITERAL_LIST",
                          "PT_COMPUTE_LIST", "PT_ARGUMENTS",
                          "PT_SPECIAL_HEAD", "PT_HEADLIST",
                          "PT_VARIABLE", "PT_NUMBER", "PT_WEIGHT",
                          "PT_CONDITION", "PT_MINIMIZE", "PT_MAXIMIZE",
                          "PT_HIDE", "PT_EXTERNAL", 
                          "PT_SHOW", "PT_BOUND", "PT_DIRECTIVE",
                          "PT_ORDERED_DISJUNCTION", "PT_UNKNOWN" };
  
#ifdef DEBUG
// for tree printing
inline void indent(int amount)
{
  while (amount-- > 0)
    output(cerr, " ");
}
#endif    
    
ParseNode::ParseNode(ParseType tp, char *st, ParseNode *l,
                     ParseNode *r, long ln, long first, long second,
                     int hd) 
{
  parent = NULL;
  type = tp;
  if (st)
    sval = st;
  else
    sval = NULL;

  left = l;
  right = r;

  if (left)
    left->parent = this;
  if (right)
    right->parent = this;
  
  lineno = ln;

  l1.start = (long) first;
  l2.end = (long) second;

  i1.weight = hd;
  
  if (type == PT_EXPR && left && !i1.paren && !left->i1.paren) {
    if (left->type == PT_EXPR ||
        (left->type == PT_FUNCTION && left->l1.op != FUN_NULL)) {
      if (l1.op == left->l1.op) 
        MergeFunctions();
      else if ((l1.op >= FUN_EQ) && (l1.op <= FUN_NEQ) &&
               (left->l1.op >= FUN_EQ) && (left->l1.op <= FUN_NEQ)) {
        error(USR_ERR, "%ld:\tYou may not mix different comparison operators "
              "in one expression \n\twithout parenthesis (even then, the result "
              "is probably not what\n\tyou want).", lineno);
        output(cerr, "\tmismatching operators: '%s' and '%s'.\n",
               operators[l1.op], operators[left->l1.op]);
      }
    }
  }
}

ParseNode::~ParseNode()
{
  if (sval)
    delete [] sval;
  
  if (left)
    delete left;
  if (right)
    delete right;

}

void ParseNode::MergeFunctions()
{
  type = PT_FUNCTION;
  sval = clone_string(function_strings[l1.op]);
  if (left->type == PT_EXPR) {
  
    // find the arguments
    ParseNode *first = left->left;
    ParseNode *second = left->right;
    ParseNode *third = right;

    // construct the argument list, the list should be in reverse order
    ParseNode *final_list = new ParseNode(PT_ARGLIST, NULL, first,
                                          NULL, lineno);
    ParseNode *second_list = new ParseNode(PT_ARGLIST, NULL, second,
                                           final_list, lineno);
    ParseNode *first_list = new ParseNode(PT_ARGLIST, NULL, third,
                                          second_list, lineno);
    left->left = 0;
    left->right = 0;
    delete left;
    left = 0;
    right = first_list;
  } else {
    // first one is already merged, now we have only to make a new
    // argument list and destroy the old
    ParseNode *new_argument = new ParseNode(PT_ARGLIST, NULL, right,
                                            left->right, lineno);
    right = new_argument;
    left->left = 0;
    left->right = 0;
    delete left;
    left = 0;
  }
  return;
}


int ParseNode::HidePredicate()
{
  Literal *lt = 0;
  ParseNode *tmp = 0;
  if (sys_data.output_version < 2)
    error(USR_ERR, "%s: you can't hide predicates with smodels 1.x",
          error_file_and_line(lineno));
  
  if (!left) {
    int_error("missing literal list", "");
  }
  tmp = left;

  while (tmp) {

    if (!tmp->left || !tmp->left->ext.lit)
      int_error("missing atom", "");
    
    lt = tmp->left->ext.lit;
    
    predicates[lt->pred]->hidden = 1;

    if (predicates[lt->pred]->shown) {
      warn(WARN_EXTENDED, lineno, "the predicate '%s/%d' is both shown and "
           "hidden at the same time.",
           predicates[lt->pred]->Name(),
           predicates[lt->pred]->Arity()); 
    }
    
    if (predicates[lt->pred]->external_atom) {
      error(USR_ERR, "%s: a predicate '%s/%d' may not be hidden and " 
            "external at the same time", error_file_and_line(lineno),
            predicates[lt->pred]->Name(),
            predicates[lt->pred]->Arity());
    }
    tmp = tmp->right;

    if (sys_data.true_negation) {
      // hide also the negated one
      long p = predicates[lt->pred]->negation;
      predicates[p]->hidden = 1;
    }
    
  }
  return 0;
}

int ParseNode::ShowPredicate()
{
  Literal *lt = 0;
  ParseNode *tmp = 0;
  
  if (!left) {
    int_error("missing literal list", "");
  }
  tmp = left;

  while (tmp) {
    if (!tmp->left || !tmp->left->ext.lit)
      int_error("missing atom", "");
    
    lt = tmp->left->ext.lit;

    if (predicates[lt->pred]->hidden && !sys_data.hide_all) {
      warn(WARN_EXTENDED, lineno, "the predicate '%s/%d' is both shown and "
           "hidden at the same time.",
           predicates[lt->pred]->Name(),
           predicates[lt->pred]->Arity()); 
    }
    
    predicates[lt->pred]->hidden = 0;
    predicates[lt->pred]->shown = 1;

    if (sys_data.true_negation) {
      long p = predicates[lt->pred]->negation;
      
      predicates[p]->hidden = 0;
      predicates[p]->shown = 1;
    }
    
    tmp = tmp->right;
  }
  return 0;
}

int ParseNode::DefineExternalPredicate()
{
  Literal *lt = 0;
  ParseNode *tmp = 0;
  if (sys_data.output_version < 2)
    error(USR_ERR, "%s: you can't use external predicates with smodels 1.x",
          error_file_and_line(lineno));
  
  if (!left) {
    int_error("missing literal list", "");
  }
  tmp = left;

  while (tmp) {

    if (!tmp->left || !tmp->left->ext.lit)
      int_error("missing atom", "");
    
    lt = tmp->left->ext.lit;
    
    predicates[lt->pred]->external_atom = 1;
    
    if (predicates[lt->pred]->hidden && !sys_data.hide_all) {
      error(USR_ERR, "%s: a predicate '%s/%d' may not be hidden and " 
            "external at the same time", error_file_and_line(lineno),
            predicates[lt->pred]->Name(),
            predicates[lt->pred]->Arity());
    }
    tmp = tmp->right;
  }
  return 0;
}

// Post-order walkthrough
int ParseNode::ProcessTree()
{
  int lresult = 0, rresult = 0;
  if (left)
    lresult = left->ProcessTree();

  if (right)
    rresult = right->ProcessTree();
  
  switch (type) {
    // these don't need any special treatment
  case PT_ARGUMENTS:
  case PT_TAIL:
  case PT_SPECIAL_TAIL:
  case PT_BASIC_TAIL:
  case PT_RULES:
  case PT_ARGLIST:
  case PT_TERM:
  case PT_WEIGHT:
  case PT_LITERAL_LIST:
  case PT_COMPUTE_LIST:
  case PT_SPECIAL_HEAD:
  case PT_HEADLIST:
  case PT_HEAD:
    break;

  case PT_CONSTANT:
  case PT_EXPR:
  case PT_VARIABLE:
  case PT_NUMBER:
    return ProcessTerm(lresult, rresult);
    break;

  case PT_ATOM:
    return ProcessAtom(lresult, rresult);
    break;

  case PT_RULE:
    return ProcessRule(lresult, rresult);
    break;

  case PT_FUNCTION:
    return ProcessFunction(lresult, rresult);
    break;
    
  case PT_CONDITION:
    return ProcessCondition();
    break;

  case PT_DIRECTIVE:
    return ProcessDirective();
    
  case PT_DECLARATION:
    return DelayProcessing();
    break;
    
  case PT_COMPUTE:
    if (!c_stmt)
      c_stmt = this;
    else if (!c_stmt->right) {
      c_stmt->right = right;
      right = NULL;
    } else {
      // add the right hand side to the end of atom list
      ParseNode *pn = c_stmt->right;
      while (pn->right) {
        pn = pn->right;
      }
      pn->right = right;
      right = NULL;
    }
    break;

  case PT_MAXIMIZE:
  case PT_MINIMIZE:
    return ProcessOptimize();

  case PT_HIDE:
    return HidePredicate();
    break;

  case PT_SHOW:
    return ShowPredicate();
    break;
    
  case PT_EXTERNAL:
    return DefineExternalPredicate();

  case PT_BOUND:
    return CheckBound();

  case PT_RANGE:
    return ProcessRange();

  case PT_ORDERED_DISJUNCTION:
    return ProcessPriority();
    
  default:
    int_error("misformed parse tree", "");
    break;
  }
  return lresult + rresult;
}

int ParseNode::ProcessCompute(int , int )
{
  assert(type == PT_COMPUTE);
  ParseNode *tmp = NULL;
  Literal *lt = NULL;
  
  // get number of models
  if (!compute->command_line)
    compute->models = l1.val;

  // add the literals to the compute statement
  tmp = right;
  while (tmp) {
    if (!tmp->left)
      int_error("literal missing from compute statement", "");
    if (tmp->left->type == PT_CONDITION)
      lt = tmp->left->left->ext.lit;
    else
      lt = tmp->left->ext.lit;

    if (!lt)
      int_error("literal missing from compute statement", "");

    compute->rl->AddLiteral(lt);

    tmp = tmp->right;
  }
  return 1;
}

int ParseNode::ProcessOptimize()
{
  Rule *rl = NULL;
  Literal *lt = NULL;
  ParseNode *tmp = NULL;

  if (sys_data.output_version < 2) {
    error(USR_ERR, "%s: optimize statements are not allowed prior to "
          "smodels version 2.x", error_file_and_line(lineno));
    return 0;
  }
  
  lt = new Literal;
  if (!lt)
    error(SYS_ERR, "malloc error");

  if (type == PT_MINIMIZE) {
    lt->pred = 0;
  } else {
    lt->pred = 1;
  }
  
  rl = new Rule(lt, OPTIMIZERULE);
  if (!rl)
    error(SYS_ERR, "malloc error");

  // use global weights?
  if (i1.weight) {
    rl->weight_head = 1;
  }
  
  tmp = right;
  while (tmp) {
    if (!tmp->left)
      int_error("misformed parse tree", "");

    // error checking is done in Rule::CalculateOptimizes
    if (tmp->left->type == PT_CONDITION) 
      lt = tmp->left->left->ext.lit;
    else
      lt = tmp->left->ext.lit;

    if (!i1.weight && lt->weight) {
      warn(WARN_EXTENDED, lt->lineno, "Explicit weight defined in a "
           "cardinality optimize statement.\n"
           "You should use `[' and `]' instead of '{' and '}'");
    }
    
    rl->AddLiteral(lt);
    tmp = tmp->right;
  }

  optimize->Insert(rl); // this keeps the order correct
  return 0;
}


int ParseNode::CountArity()
{
  int res = 0;
  ParseNode *tmp = NULL;

  // go to the start of the argument lists.
  tmp = right->left;
  while (tmp) {
    res++;
    tmp = tmp->right;
  }
  return res;
}
  

int ParseNode::ProcessAtom(int , int )
{
  ParseNode *new_node = NULL, *tmp = NULL; 
  long pred = -1;
  int arity = 0;
  Weight *wt = NULL;

  // find the number of arguments
  if (right) {
    if (right->left)
      arity = CountArity();
    else
      int_error("missing argument", "");
    // if there are many argument lists, add them to be processed. 
    tmp = right->right;
    if (tmp) {
      new_node = new ParseNode(PT_ATOM, clone_string(sval), left, right->right,
                               lineno);
      new_node->l2.negative = l2.negative;
      new_node->i1.weight = i1.weight;
      if (!new_node)
        error(SYS_ERR, "malloc error");
      parent->AddNewRight(new_node);
    }
  }

  if (arity > sys_data.term_max_arity) {
    sys_data.term_max_arity = arity;
  }
  pred = Predicate::DefinePredicate(sval,arity, lineno);
    
  // construct the literal
  ext.lit = new Literal(this, pred, arity);
  if (!ext.lit)
    error(SYS_ERR, "malloc error");

  // check the weights 
  if (left) {
    wt = new Weight;
    if (!wt)
      error(SYS_ERR, "malloc error");
    switch (left->type) {
    case PT_VARIABLE:
      wt->type = WT_VARIABLE;
      wt->v.var = left->l1.val;
      break;
    case PT_EXPR:
    case PT_FUNCTION:
      wt->type = WT_EXPR;
      wt->v.fun = left->ext.fun;
      break;
    case PT_NUMBER:
      wt->type = WT_NUMBER;
      wt->v.val = left->l1.val;
      break;
    case PT_CONSTANT:
      error(USR_ERR, "%s: a non numeric constant '%s' used as weight",
            error_file_and_line(lineno), left->sval);
      wt->type = WT_NUMBER;
      wt->v.val = 0;
      break;
    case PT_ATOM:
      wt->type = WT_ATOM;
      wt->v.lit = left->ext.lit->Duplicate();
      break;
    default:
      int_error("misformed parse tree", "");
    }
    ext.lit->weight = wt;
  }

  return 1;
}  


int ParseNode::ProcessRule(int, int)
{
  Literal *head = NULL, *lt = NULL;
  ParseNode *tail = NULL, *tmp=NULL;
  Rule *rl = NULL;
  Function *fun = NULL;
  int tail_length = 0;
  
  if (!left && !false_lit) {
    false_lit = Predicate::DefineFalseLiteral();
  }
  
  
  // If the rule is a special rule delay its processing
  if (left && ((left->type == PT_SPECIAL_HEAD) ||
               (left->type == PT_ORDERED_DISJUNCTION))) {
    DelayProcessing();
    return 1;
  }
  
  tmp = right;
  while (tmp) {
    if (!tmp->left)
      int_error("missing tail","");
    if (tmp->left->type == PT_SPECIAL_TAIL) {
      DelayProcessing();
      return 1;
    }
    tmp = tmp->right;
  }

  // Generate head, and associated conditions
  if (!left || left->type != PT_CONDITION) 
    head = ProcessHeadLiteral(left); /* Literal * */
  else 
    head = ProcessHeadLiteral(left->left);
  
  if (!head) { // shouldn't happen
    int_error("rule lost its head somehow", "");
  }
  
  rl = new Rule(head, BASICRULE);
  if (!rl)
    error(SYS_ERR, "malloc error");

  if (right)
    tail = right->left;
  else
    tail = NULL;
  
  while (tail) {
    tmp = tail->left;
    if (!tmp)
      int_error("missing literal", "");
    if (tmp->type != PT_FUNCTION && tmp->type != PT_EXPR) {
      if (tmp->type == PT_CONDITION) {
        lt = tmp->left->ext.lit;
      } else
        lt = tmp->ext.lit; 
      
      rl->AddLiteral(lt);

      if (lt->weight) {
        warn(WARN_EXTENDED, lt->lineno, "weight defined for the literal %s%s' in "
             "a basic rule", (lt->negative) ? "'not " : "'",
             lt->GetPrintString());
      }
      
      dependency_graph->AddEdge(head->pred, lt->pred, lt->negative);
      tail_length++;
    } else {
      fun = tmp->ext.fun;
      rl->AddFunction(fun);
      tail_length++;
    }
    tail = tail->right;
  }

  if ((rl->positive.Size() + rl->negative.Size() + rl->functions.Size()
       == 0)&& !head->ground && !sys_data.has_implicit_domains) {
    error(USR_ERR, "%s: a non-ground fact:", error_file_and_line(lineno));
    output(cerr, "\t\t");
    head->Print(0);
    output(cerr, ".\n   All terms within a fact must be ground terms.\n");
  }
  if (sys_data.print_debug) {
    (void*) rl->GetPrintString(); // cache the original text of the rule
  }
  predicates[head->pred]->AddRule(rl);
  return 1;
}


Literal *ParseNode::ProcessHeadLiteral(ParseNode *hd)
{
  // if there is no head, return false literal
  if (!hd) {
    if (! false_lit)
      false_lit = Predicate::DefineFalseLiteral();
    return false_lit;
  }

  /* not needed anymore */
#if 0 
  // check if the literal is negative. This might happen with choise
  // and generate rules 
  if (hd->ext.lit->negative) {
    error(USR_ERR, "%s: negative literals are not allowed "
          "within rule heads:", error_file_and_line(hd->ext.lit->lineno)); 
    output(cerr, "\t\tnot ");
    hd->ext.lit->Print(0);
    output(cerr, "\n");
    return NULL;
  }
#endif
  // otherwise, no change is needed
  return hd->ext.lit;
}


int ParseNode::DelayProcessing()
{
  // add special rules to the end of the list and weight definitions
  // in the beginning. 

  if (type == PT_DECLARATION) {
    weight_declarations->Insert(this);
  } else {
    FindDependencies(); 
    special_rules->Append(this);
  }
  return 1;
}

// goes through a special rule and adds all dependencies in the
// dependency graph
void ParseNode::FindDependencies()
{
  long *heads = NULL;
  long num_heads = 0;
  long pred = -1, pos = 0;
  Literal *lt = NULL;
  ParseNode *tmp1 = NULL, *tmp2 = NULL;
  
  // first find all heads
  tmp1 = left;
  if (!tmp1) { // only false literal as head.
    num_heads = 1;
    heads = new long[1];
    if (!heads)
      error(SYS_ERR, "malloc error");
    *heads = false_lit->pred;
    predicates[*heads]->special = 1;
    dependency_graph->AddEdge(*heads, *heads, true);
  } else if (tmp1->type == PT_ATOM) {
    num_heads = 1;
    heads = new long[1];
    if (!heads)
      error(SYS_ERR, "malloc error");
    *heads = left->ext.lit->pred;
    predicates[*heads]->special = 1;
    left->ext.lit->CountSpecialVars();
    predicates[*heads]->SetStatus(DM_FALSE);
    dependency_graph->AddEdge(*heads, *heads, true);
  } else {
    tmp1 = left->right;
    while (tmp1) {
      num_heads++;
      tmp1 = tmp1->right;
    }
    heads = new long[num_heads];
    tmp1 = left->right;
    while (tmp1) {
      if (tmp1->left->type == PT_CONDITION) {
        lt = tmp1->left->left->ext.lit;
      } else if (tmp1->left->type == PT_ATOM) {
        lt = tmp1->left->ext.lit;
      } else {
        // FIXME: think this again
        tmp1 = tmp1->right;
        continue;
      }
      if (!lt) // sanity check
        int_error("misformed parse tree", "");
      
      heads[pos++] = lt->pred;
      predicates[lt->pred]->special = 1;
      predicates[lt->pred]->SetStatus(DM_FALSE);
      lt->CountSpecialVars();
      // add a edge to the dependency graph from each head to itself
      // to ensure that it won't be marked as a domain predicate
      dependency_graph->AddEdge(lt->pred, lt->pred, true);
      tmp1 = tmp1->right;
    }
  }
    
  // then find all literals in the tail (conditions are not checked,
  // since cycles are transitive)
  tmp1 = right;
  while (tmp1) {
    if (!tmp1->left)
      int_error("missing tail section", "");

    if (tmp1->left->type == PT_SPECIAL_TAIL)
      tmp2 = tmp1->left->right;
    else
      tmp2 = tmp1->left;

    int skip = 0;
    while (tmp2) {
      skip = 0;

      if (!tmp2->left)
        int_error("missing literal", "");
      if (tmp2->left->type == PT_CONDITION) {
        lt = tmp2->left->left->ext.lit;
      } else if (tmp2->left->type == PT_ATOM) {
        lt = tmp2->left->ext.lit;
      } else {
        skip = 1;
      }
      if (!skip) {
        pred = lt->pred;
        for (pos = 0; pos < num_heads; pos++) {
          dependency_graph->AddEdge(heads[pos], pred, lt->negative);
        }
        lt->CountSpecialVars();
      }
      tmp2 = tmp2->right;
    }
    tmp1 = tmp1->right;
  }

  if (heads)
    delete [] heads;
  return ;
}

int ParseNode::ProcessWeightDeclarations()
{
  ParseNode *tmp = NULL;
  
  while ((tmp = weight_declarations->Iterate())) {
    tmp->ProcessDeclaration();
  }
  return 1;
}

int ParseNode::ProcessDelayed()
{
  ParseNode *tmp = NULL;

  while ((tmp = special_rules->Iterate())) {
    tmp->ProcessSpecialRule();
  }
  return 1;
}

RuleType ParseNode::IdentifySpecialHead(int *nheads)
{
  RuleType tp = EMPTYRULE;
  ParseNode *tmp = NULL;

  if (!left || left->type == PT_ATOM)
    return BASICRULE;

  if (left->type == PT_ORDERED_DISJUNCTION) {
    return ORDERED_DISJUNCTION;
  } 
  if ((left->type == PT_SPECIAL_HEAD) &&
      (left->l2.is_disjunction))
    tp =  DISJUNCTIVERULE;
  else
    tp  =  CHOICERULE;
  tmp = left->right;
  while (tmp) {
    (*nheads)++;
    tmp = tmp->right;
  }
  return tp;
}

// checks what type of special tail is used.
// The algorithm is following:
// actually, after the syntax change choosing between special tail
// types is trivial, and this function only does some sanity checks. 

RuleType ParseNode::IdentifySpecialTail()
{
  RuleType tp = EMPTYRULE;
  int weights_in_tail = 0;
  ParseNode *tmp = NULL;
  Literal *lt = NULL;
  
  if (i1.weight) {
    tp = WEIGHTRULE;
  } else
    tp = CONSTRAINTRULE;

  // check the bound values. 
  if (!left)
    int_error("misformed parse tree", "");
  if ( !left->left && !left->right) { // lower bound
    error(USR_ERR, "%s: an extended rule section is lacking both bounds", 
          error_file_and_line(lineno));
    return tp;
  }
  
  // iterate through the tail and check if there are any weights.
  tmp = right;
  if (!tmp) {
    error(USR_ERR, "%s: empty extended rule section", error_file_and_line(lineno));
  }
  while (tmp) {
    if (!tmp->left)
      int_error("missing literal", "");
    switch (tmp->left->type) {
    case PT_ATOM:
      lt = tmp->left->ext.lit;
      if (lt->weight) {
        weights_in_tail++;
        if (tp == CONSTRAINTRULE) {
          warn(WARN_EXTENDED, lt->lineno, "weight defined for literal %s%s' in "
               "a constraint rule.", lt->negative ? "'not " : "'",
               lt->GetPrintString());
        }
      }
      break;
    case PT_FUNCTION:
    case PT_EXPR:
      break;
    case PT_CONDITION:
      if (tmp->left->left->ext.lit->weight) {
        weights_in_tail++;
        if (tp == CONSTRAINTRULE) {
          warn(WARN_EXTENDED, lt->lineno, "weight defined for literal %s%s' in "
               "a constraint rule.", lt->negative ? "'not " : "'",
               lt->GetPrintString());
        }
      }
      break;
    default:
      int_error("misformed parse tree", "");
    }
    tmp = tmp->right;
  }

  return tp;
}

void warn_about_disjunctive_problems(long lineno)
{
  error(USR_ERR, "%ld: when using the command line option `--dlp' only", lineno);
  output(cerr, "%sextended rules of the form:\n");
  output(cerr, "\t%sa1 | a2 | ... | an :- body. \n");
  output(cerr, "\t%s\tor\n");
  output(cerr, "\t%s{ a(X) : b(X) } :- body. \n");
  //  output(cerr, "\t%s1 { a(X) : d(X) } :- body.\n", indent_string);
  output(cerr, "%sare allowed.\n");
  output(cerr, "%s(You might want to use --dlp-choice instead.)\n");
}

int ParseNode::ProcessSpecialRule()
{
  RuleType head_type = EMPTYRULE, tail_type = EMPTYRULE;
  ParseNode *tmp = NULL, *tmp2 = NULL;
  Literal *head = NULL;
  int num_heads = 0;
  Literal *lt = NULL;
  SpecialLiteral *tail = NULL;
  Function *fun = NULL;
  Rule *rl = NULL;
  Term *lower_bound = NULL;
  Term *upper_bound = NULL;
  
  head_type = IdentifySpecialHead(&num_heads);

  switch (head_type) {
  case BASICRULE:
    head = ProcessHeadLiteral(left);
    break;
  case CHOICERULE:
  case GENERATERULE:
  case ORDERED_DISJUNCTION:
  case DISJUNCTIVERULE:
    break;
  default:
    int_error("misformed extended rule", "");
  }

  // then generate the new rule according to type just found
  if (head_type == BASICRULE) {
    rl = new Rule(head, BASICRULE);
    if (!rl)
      error(SYS_ERR, "malloc error");
  } else {
    // special head  
    rl = new Rule(head_type, num_heads);
    
    if (!rl)
      error(SYS_ERR, "malloc error");

    int constant_atleast = 0;
    int constant_atmost = 0;

    if (!left->left && head_type != ORDERED_DISJUNCTION) {
      // a rule of the the form: a | ... | b :- ...
      constant_atmost = 1;
      constant_atleast = 1;
      if (sys_data.dlp_semantics) {
        rl->atmost = new Term(T_CONSTANT, 0, rl->line_start);
        rl->atleast = new Term(T_CONSTANT, 0, rl->line_start);
      } else {
        error(USR_ERR, "%ld: The program has a disjunctive rule"
              " but command line option --dlp or --dlp-choice was"
              " not selected\n", lineno);
        return 0;
      } 
    } else {
      if (!(head_type == ORDERED_DISJUNCTION)) {
        if (left->left->left)
          rl->atleast = Term::CreateBound(left->left->left);
        if (left->left->right)
          rl->atmost = Term::CreateBound(left->left->right);
	
        if (rl->atleast && rl->atleast->type == T_CONSTANT)
          constant_atleast = 1;
	
        if (rl->atmost && rl->atmost->type == T_CONSTANT)
          constant_atmost = 1;
	
        if (left->i1.weight) {
          rl->weight_head = 1;
        }
        // check the bounds
        if (sys_data.dlp_semantics) {
          if (sys_data.allow_only_disjunctive_rules) {
            if (rl->atmost || rl->atleast) {
              warn_about_disjunctive_problems(lineno);
              return 0;
            }
          } 
        }
      }
    }
    
    if (constant_atmost && constant_atleast &&
        (rl->atleast->val > rl->atmost->val)) { 
      warn(WARN_EXTENDED, lineno, "%ld: lower bound (%ld) in an extended rule "
           "section is "
           "bigger than the upper bound (%ld).", rl->atleast->val, 
           rl->atmost->val);
    }
    
    // add the heads
    tmp = left->right;
    while (tmp) {
      if (tmp->left->type != PT_CONDITION)
        lt = tmp->left->ext.lit;
      else
        lt = tmp->left->left->ext.lit;
      
      if (!rl->weight_head && lt->weight) {
        warn(WARN_EXTENDED, lt->lineno, "atom '%s' in a choice rule head "
             "has weight assigned for it.", lt->GetPrintString());
      }
      rl->AddHead(lt);
      if (lt->negative) {
        error(USR_ERR, "%ld: there is a negative literal 'not %s' in a"
              "choice rule head.", lineno, lt->GetPrintString());
      }
      predicates[lt->pred]->has_rule = 1;
      tmp = tmp->right;
    }
  }

  // finally go through the tail and generate a special literal for
  // each special rule in the tail, and add the normal tail as normal
  // literals.

  tmp = right;
  while (tmp) {  // this loop processes special tails.
    if (!tmp->left)
      int_error("missing extended tail section", "");
    
    if (tmp->left->type == PT_BASIC_TAIL) 
      break; // there are no special tails after basic tail

    if (sys_data.allow_only_disjunctive_rules) {
      warn_about_disjunctive_problems(tmp->lineno);
      return 0;
    }
    
    // process special tail
    tail_type = tmp->left->IdentifySpecialTail();
    
    // construct the corresponding special literal

    lower_bound = upper_bound = NULL;
    if (!tmp->left->left)
      int_error("missing bound definition", "");
    if (tmp->left->left->left) {
      lower_bound = Term::CreateBound(tmp->left->left->left);
    }
    if (tmp->left->left->right) {
      upper_bound = Term::CreateBound(tmp->left->left->right);
    }
    
    tail = new SpecialLiteral(tail_type, lower_bound, upper_bound, lineno);
    if (!tail)
      error(SYS_ERR, "malloc error");
    
    // iterate through special tail and add literals and functions
    // into special literal
    tmp2 = tmp->left->right;
    while (tmp2) {
      if (!tmp2->left)
        int_error("missing literal", "");
      switch(tmp2->left->type) {
      case PT_ATOM:
        lt = tmp2->left->ext.lit;
        if (lt->has_range || lt->has_function || lt->has_atom)
          lt->Simplify();
        tail->tail.Insert(lt);
        predicates[lt->pred]->is_only_symbolic_function = 0;
        if (lt->conditions || lt->condition_func) {
          tail->num_conditions++;
          if (lt->negative)
            tail->num_negative_conditions++;
          else
            tail->num_positive_conditions++;
        }
        if (!lt->ground)
          tail->ground = 0;
        break;
      case PT_FUNCTION:
      case PT_EXPR:
        fun = tmp2->left->ext.fun;
        tail->funs.Insert(fun);
        if (!fun->ground)
          tail->ground = 0;
        break;
      case PT_CONDITION:
        lt = tmp2->left->left->ext.lit;
        if (lt->has_range || lt->has_function || lt->has_atom)
          lt->Simplify();
        tail->tail.Insert(lt);
        predicates[lt->pred]->is_only_symbolic_function = 0;
        tail->num_conditions++;

        if (lt->negative)
          tail->num_negative_conditions++;
        else
          tail->num_positive_conditions++;
	
        if (!lt->ground)
          tail->ground = 0;
        break;
      default:
        int_error("misformed parse tree", "");
        break;
      }
      tmp2 = tmp2->right;
    }
    // add the special literal in rule body
    rl->AddLiteral(tail);
    tmp = tmp->right;
  }
  
  // process the normal tail, if there is any
  if (tmp) {
    tmp2 = tmp->left;
    while (tmp2) {
      if (!tmp2->left)
        int_error("missing literal", "");
      switch (tmp2->left->type) {
      case PT_FUNCTION:
      case PT_EXPR:
        fun = tmp2->left->ext.fun;
        rl->AddFunction(fun);
        break;
      case PT_ATOM:
        lt = tmp2->left->ext.lit; 
        rl->AddLiteral(lt);
        break;
      case PT_CONDITION:
        lt = tmp2->left->left->ext.lit; 
        rl->AddLiteral(lt);
        break;
      default:
        int_error("missing literal", "");
        break;
      }
      tmp2 = tmp2->right;
    }
    if (tmp->right) {
      int_error("too many tails", "");
    }
  }

  if (sys_data.has_implicit_domains)
    rl->DefineImplicitDomain();
  
  if (rl->CheckSpecialRestrict() != RT_STRONG)
    return 0;

  if (sys_data.print_debug) {
    (void*) rl->GetPrintString(); // cache the original text of the rule
  }
  rl->SimplifySpecialRule();

  if (head_type == BASICRULE) 
    predicates[head->pred]->AddSpecialRule(rl);
  else if (head_type == ORDERED_DISJUNCTION) {
    if (left && left->left) // label?
      rl->label = left->left->ext.lit;
    priority_data.AddChoice(rl);
  } else {
    lt = rl->head_array[0];
    predicates[lt->pred]->AddSpecialRule(rl);
  }
  return 1;
  
}

int ParseNode::ProcessTerm(int , int )
{
  Term *lt = NULL, *rt = NULL;
  InstFunc fp = NULL;
  static Term *zero = NULL;
  char *op = NULL;
  char *st = NULL;
  
  switch (type) {
  case PT_CONSTANT:
    l1.val =  constant_table->Insert(sval);

    if (sys_data.warnings & WARN_SIMILAR) {
      if ((numeric_constant_table->Lookup(sval) >= 0) &&
		  (warnings_table->Lookup(sval) < 0)) {
		warn(WARN_SIMILAR, lineno, "the constant '%s' is used as "
			 "both numeric and symbolic constant. This is probably"
			 " caused by using the constant before the `const' "
			 "declaration.", sval);
		warnings_table->Insert(sval);
      }
      st = clone_string(sval);
      if (!st)
		error(SYS_ERR, "malloc error");
	  
      // is the string quoted?
      if (*st == '"') {
		char *st2 = st+1;
		size_t pos = strlen(st2) -1 ;
		char tmpchar = st2[pos];
		st2[pos]= '\0';
		if ((constant_table->Lookup(st2) >= 0) &&
			(warnings_table->Lookup(sval)< 0)) {
		  warn(WARN_SIMILAR, lineno, "constant '%s' is similar to"
			   " the constant '%s' (further occurrences of '%s' are"
			   " not checked)", sval, st2, sval);
		  warnings_table->Insert(sval);
		}
		st2[pos] = tmpchar;
      }
      *st = toupper(*st);
      if ((variable_table->Lookup(st) >= 0) &&
		  (warnings_table->Lookup(sval)< 0)) {
		warn(WARN_SIMILAR, lineno, "constant '%s' is similar to variable '%s'"
			 " (other occurrences of '%s' are not checked)", 
			 sval, st, sval);
		warnings_table->Insert(sval);
      }
      
      delete [] st;
    }
    break;
  case PT_VARIABLE:
    l1.val = variable_table->Insert(sval);
	
    if (sys_data.warnings & WARN_SIMILAR) {
      st = clone_string(sval);
      if (!st)
		error(SYS_ERR, "malloc error");
      *st = tolower(*st);
      if ((constant_table->Lookup(st) >= 0) &&
		  (warnings_table->Lookup(sval) < 0)) {
		warn(WARN_SIMILAR, lineno, "variable '%s' is similar to constant '%s'"
			 " (other occurrences of '%s' are not checked)", 
			 sval, st, sval);
		warnings_table->Insert(sval);
		warnings_table->Insert(st);
      }
      delete [] st;
    }
    break;
  case PT_NUMBER:
    // check if the number is biggest seen this far
    if (sys_data.max_number < l1.val)
      sys_data.max_number = l1.val;
    break;
  case PT_EXPR:
    // construct the call tree of the function and store it to 'fun'.
    // first get arguments
    if (left) {
      lt = Term::ConstructArgument(left);
      
      // check that symbolic constants are not passed on
      //      if ((lt->type == T_CONSTANT) && (IS_CONSTANT(lt->val)) &&
      //	  !(((InternalFunction) l1.val == FUN_EQ) ||
      //	    ((InternalFunction) l1.val == FUN_NEQ))) {
      //	error(USR_ERR, "%ld: non-numeric constant '%s' used as an function "
      //	      "argument:", lineno, left->sval);
      //  }
    } else if (((InternalFunction) l1.val != FUN_MINUS) &&
               ((InternalFunction) l1.val != FUN_ABS) &&
               ((InternalFunction) l1.val != FUN_WEIGHT) &&
    	       ((InternalFunction) l1.val != FUN_NORM)) {
      int_error("operator '%s' without a left operand", function_strings[l1.val]);
    } else { // if uminus set left arg to zero
      if (!zero) {
        zero = new Term(T_CONSTANT, MAKE_NUMBER(0), lineno);
        if (!zero)
          error(SYS_ERR, "malloc error");
      }
      lt = zero;
    }
    // check that nobody tries to assign to something that is not a
    // variable
    if (((InternalFunction)l1.val == FUN_ASSIGN) &&
        (lt->type != T_VARIABLE))
      error(USR_ERR, "%s: invalid lvalue in assignment",
            error_file_and_line(lineno)); 
    
    if (right) {
      rt = Term::ConstructArgument(right);
      //      if ((rt->type == T_CONSTANT) && IS_CONSTANT(rt->val) &&
      //	  !(((InternalFunction) l1.val == FUN_EQ) ||
      //	    ((InternalFunction) l1.val == FUN_NEQ))) {
      //	error(USR_ERR, "%ld: non-numeric constant '%s' used as an function "
      //	      "argument:", lineno, right->sval);
      //      }
    } else 
      int_error("operator '%d' without right operand", l1.val);
    
    // get operator
    op = function_strings[l1.op];
    if (!op)
      int_error("Unknown operator '%ld'", l1.op);
    fp = function_table->GetFunction(op);
    if (!fp)
      int_error("unknown function '%s'", op);

    if (sys_data.term_max_arity < 2)
      sys_data.term_max_arity = 2;
        
    if (((InternalFunction)l1.val != FUN_ABS) &&
        ((InternalFunction)l1.val != FUN_WEIGHT) &&
        ((InternalFunction)l1.val != FUN_NORM)) {
      ext.fun = new Function(fp, op, 2, lineno);
      ext.fun->AddArgument(lt);
    } else {
      ext.fun = new Function(fp, op, 1, lineno);
    }

    if ((InternalFunction)l1.val == FUN_NORM) {
      if (rt->type != T_ATOM) {
        error(USR_ERR, "%s: Invalid argument type for the function"
              " `norm'. The norm function accepts only one basic literal"
              " as its argument.", error_file_and_line(lineno));
      }
      Literal *l = ((LiteralTerm*) rt)->lt;
      long p = l->pred;
      delete rt;
      Term *t = new Term(T_CONSTANT, MAKE_NUMBER(p),
                         lineno);
      ext.fun->AddArgument(t);
    } else
      ext.fun->AddArgument(rt);
    
    if ((InternalFunction)l1.val == FUN_WEIGHT) {
      ext.fun->is_weight_function = 1;
      if (ext.fun->args[0]->type != T_ATOM) {
        error(USR_ERR, "%s: Invalid argument type for the function"
              " `weight'. The weight function accepts only one basic literal"
              " as its argument.", error_file_and_line(lineno));
      }
    } 
    break;
  default:
    int_error("misformed parse tree:"
              "Invalid term %ld", parse_strings[type]);
  }
  return 1;
}

int ParseNode::ProcessFunction(int, int rresult)
{
  //  Term *arg = NULL;
  //ParseNode *tmp = NULL, *parg = NULL;
  InstFunc fp = NULL;
  int arity = rresult;
  
  // Find the pointer to function
  fp = function_table->GetFunction(sval);

  if (!fp)
    int_error("unknown function %s", sval);

  
  // allocate the function
  ext.fun = new Function(fp, this, sval, lineno, arity);
  if (!ext.fun)
    error(SYS_ERR, "malloc error");

  if (l2.negative)
    ext.fun->negative = 1;

  if (arity > MAX_NUMBER_OF_FUNCTION_ARGUMENTS) {
    error(USR_ERR, "%s: too many arguments (%d, max %d) in function"
          " %s", error_file_and_line(lineno), arity,
          MAX_NUMBER_OF_FUNCTION_ARGUMENTS, sval);
    return 1;
  }
  if (sys_data.term_max_arity < arity)
    sys_data.term_max_arity = arity;

  return 1;
}  

int ParseNode::ProcessDeclaration()
{
  // first, reverse the declaration list as parse.yy gives it in
  // reverse order
  ParseNode *first = 0, *second = 0, *tmp = 0;
  ParseNode *current = right;
  
  while (current) {
    second = first;
    first = current;
    current = current->right;
    first->right = second;
  }
  tmp = right = first;
  while (tmp) {
    tmp->ProcessWeight();
    tmp = tmp->right;
  }
  return 0;
}

int ParseNode::ProcessWeight()
{
  int num_free = 0, unrestricted = 0, total = 0;
  Weight *new_weight = NULL;
  Literal *lt = NULL;
  Literal *weight_literal = NULL;
  ParseNode *tmp = NULL;
  Function *fun = NULL;
  long weight = 0;
  int i = 0;
  static Variable *free_vars = NULL;
  WeightType wt = WT_NUMBER;

  if (!free_vars) {
    free_vars= new Variable[sys_data.total_vars+1];
    if (!free_vars)
      error(SYS_ERR, "malloc error");
  }

  if (!left)
    int_error("missing declaration", "");

  lt = left->ext.lit;

  if (left->type == PT_CONDITION) {
    error(USR_ERR, "%s: Conditions are not allowed in weight declarations",
          error_file_and_line(lineno));
    return 0;
  }
  if (!left->left) {
    error(USR_ERR, "%s: Global weight declaration for predicate '%s' is "
          "missing weight expression", error_file_and_line(lineno),
          predicates[lt->pred]->Name());  
    return 0;
  }
  // first clear all variables
  for (i = 0; i < sys_data.total_vars; i++) {
    variables[i] = VAR_NONE;
  }
  // then check the type of weight expression
  tmp = left->left;

  switch(tmp->type) {
  case PT_VARIABLE:
    wt = WT_VARIABLE;
    break;
  case PT_EXPR:
  case PT_FUNCTION:
    fun = tmp->ext.fun;
    
    if (fun->ground) {
      weight = fun->Call();
      wt = WT_NUMBER;
    } else {
      fun->SetVars(VAR_WEIGHT);
      wt = WT_EXPR;
    }
    break;
    
  case PT_NUMBER:
    wt = WT_NUMBER;
    weight = tmp->l1.val;
    break;

  case PT_ATOM:
    wt = WT_ATOM;
    weight_literal = tmp->ext.lit;
    break;
  default:
    int_error("unknown weight expression type", "");
  }
  
  if (lt->has_range) {
    error(USR_ERR, "%s: a global weight definition literal shouldn't "
          "have any ranges", error_file_and_line(lineno));
    output(cerr, "\toffending literal: ");
    lt->Print(1);
    output(cerr, "\n");
  }

  if (lt->has_function) {
    error(USR_ERR, "%s: a global weight definition literal shouldn't "
          "have any functions", error_file_and_line(lineno));
    output(cerr, "\tOffending literal");
    lt->Print(1);
    output(cerr, "\n");
  }

  if (lt->conditions) {
    error(USR_ERR, "%s: a global weight definition literal shouldn't "
          "have any conditions", error_file_and_line(lineno));
    output(cerr, "\tOffending literal");
    lt->Print(1);
    output(cerr, "\n");
  }

  // add all variables of actual expression
  lt->Restrict(R_NEGATIVE_LOCAL);

  
  // go through the variables and count the free and restricted ones.
  for (i=0; i < sys_data.total_vars; i++) {
    switch (variables[i]) {
    case R_CONDITION_LOCAL:
      total++;
      break;
    case R_WEIGHT_LOCAL:
      free_vars[num_free++] = i;
      total++;
      break;
    case R_NEGATIVE_LOCAL:
      total++;
      unrestricted++;
      break;
    default:
      break;
    }
  }
  
  // if there are free variables in the weight expression output an
  // error
  if (num_free > 0) {
    error(USR_ERR, "%s: Free variables are not allowed in global weight "
          "expression", error_file_and_line(lineno));
    output(cerr, "\t\tweight ");
    lt->Print(1);
    output(cerr, ".\n   Free variables: ");
    for (i =0; i < num_free; i++) {
      output(cerr, "%s ", variable_table->symbols[free_vars[i]]);
    }
    output(cerr, "\n");
    return 0;
  }

  // construct the weight definition
  new_weight = new Weight;
  if (!new_weight)
    error(SYS_ERR, "malloc error");
  new_weight->type = wt;
  new_weight->lineno = tmp->lineno;
  switch (wt) {
  case WT_NUMBER:
    new_weight->v.val = weight;
    break;
  case WT_VARIABLE:
    new_weight->v.var = variable_table->Lookup(tmp->sval);
    break;
  case WT_EXPR:
    new_weight->v.fun = fun;
    break;
  case WT_ATOM:
    new_weight->v.lit = weight_literal;
    break;
  default:
    int_error("unknown weight type", "");
  }
  predicates[lt->pred]->AddWeight(new_weight, lt, 0);
  return 1;
}
      
long ParseNode::Eval()
{
  long args[2] = { 0 };
  long res = 0;
  int nargs = 0;

  if ((type != PT_EXPR) && (type != PT_NUMBER) &&
      (type != PT_CONSTANT))  { 
    return ERROR_NUMBER;
  }
  if (type == PT_NUMBER)
    return l1.val;

  if (type == PT_CONSTANT) {
    return constant_table->Insert(sval);
  }
  
  if (l1.op == FUN_ASSIGN ||
      l1.op == FUN_NORM) {
    //    error(USR_ERR, "%ld: Assignment not allowed in a constant expression",
    //	  lineno, sval);
    return ERROR_NUMBER;
  }

  if (left) {
    args[nargs++] = left->Eval();
  }
  if (right) {
    args[nargs++] = right->Eval();
  }

  if ((args[0] == ERROR_NUMBER) || (args[1] == ERROR_NUMBER))
    return ERROR_NUMBER;

#ifdef CXX_HAS_NO_EXCEPTIONS
  res = internal_functions[l1.op](nargs, args);
#else
  try {
    res = internal_functions[l1.op](nargs, args);
  }
  catch (RuntimeError& r) {
    output(cerr, "%s: ", error_file_and_line(lineno));
    r.Print();
    output(cerr, " in function '%s'\n", function_strings[l1.op]);
    sys_data.num_errors++;
    return ERROR_NUMBER;
  }
#endif
  return res;
}

int ParseNode::ProcessCondition()
{
  assert(type == PT_CONDITION);
  ParseNode *tmp = NULL;

  // iterate through all conditions
  tmp = right;
  if (!tmp)
    int_error("empty condition", "");

  left->ext.lit->conditions = new LiteralList();
  left->ext.lit->condition_func = new FunctionList();

  // check for negative atoms
  if (l2.negative)
    left->ext.lit->negative = 1;
  
  while (tmp) {
    if (!tmp->left)
      int_error("missing conditional", "");
    if ((tmp->left->type != PT_EXPR) &&
        (tmp->left->type != PT_FUNCTION)) {
      
      // add edge to dependency graph. 
      if (left->ext.lit->pred != tmp->left->ext.lit->pred)
        dependency_graph->AddEdge(left->ext.lit->pred,
                                  tmp->left->ext.lit->pred,
                                  tmp->left->ext.lit->negative);
      // and put condition to right place
      left->ext.lit->conditions->Insert(tmp->left->ext.lit);
      predicates[tmp->left->ext.lit->pred]->is_only_symbolic_function = 0;
    } else {
      left->ext.lit->condition_func->Insert(tmp->left->ext.fun); 
    }
    tmp = tmp->right;
  }
      
  return 1;
}

// this assumes preorder tree travelsal 
int ParseNode::AddNewRight(ParseNode *new_r)
{
  ParseNode *new1 =NULL, *new2 = NULL; 
  switch (type) {

  case PT_RULE:
    // in case of PT_RULE we should construct a wholly new rule that
    // is otherwise identical to this but has different head
    new1 = new ParseNode(PT_RULE, NULL, new_r, right, lineno);
    if (!new1)
      error(SYS_ERR, "malloc error");
    new2 = new ParseNode(PT_RULES, NULL, new1, parent->right, lineno);
    if (!new2)
      error(SYS_ERR, "malloc error");
    parent->right = new2;
    break;
  case PT_BASIC_TAIL:
    // in this case we should simply add the new literal to the tail
    new1 = new ParseNode(PT_BASIC_TAIL, NULL, new_r, right, lineno);
    if (!new1)
      error(SYS_ERR, "malloc error");
    right = new1;
    break;

  case PT_LITERAL_LIST:
    // either compute_stmt, condition or special head
    // here we should add the extra argument to the list of possible
    // heads.
    new1 = new ParseNode(PT_LITERAL_LIST, NULL, new_r, right, lineno);
    if (!new1)
      error(SYS_ERR, "malloc error");
    right = new1;
    break;
  default:
    int_error("trying to add a new argument to wrong rule type", "");
  }
  return 1;
}
  

void ParseNode::PrintExpression()
{
  switch (type) {
  case PT_VARIABLE:
  case PT_CONSTANT:
    output(cerr, "%s", sval);
    break;
  case PT_NUMBER:
    output(cerr, "%ld", l1.val);
    break;
  case PT_EXPR:
    if (l1.val != FUN_ABS) {
      if (left)
        left->PrintExpression();
      output(cerr, "%s", operators[l1.val]);
      if (right)
        right->PrintExpression();
    } else {
      output(cerr, "|");
      if (right)
        right->PrintExpression();
    }
    break;
  default:
    break;
  }
}
    
int ParseNode::CheckBound()
{
  if (left && (left->type == PT_CONSTANT)) {
    error(USR_ERR, "%s: a symbolic constant '%s' is not allowed "
          "in an extended rule bound.", error_file_and_line(left->lineno),
          left->sval);
  }
  if (right && (right->type == PT_CONSTANT)) {
    error(USR_ERR, "%s: a symbolic constant '%s' is not allowed "
          "in an extended rule bound.", error_file_and_line(right->lineno),
          right->sval);
  }
  return 1;
}
  

int ParseNode::ProcessDirective()
{
  ParseNode *tmp = right;
  Literal *lt = NULL;

  if (!l1.val) { // domain definition
	sys_data.has_implicit_domains = true;
	while (tmp) {
	  lt = tmp->left->ext.lit;
	  implicit_domain_list->Insert(lt);
	  tmp->left->ext.lit = NULL;
	  tmp = tmp->right;
	}
  } else {
	// nondomain definition
	while (tmp) {
	  lt = tmp->left->ext.lit;
	  dependency_graph->AddEdge(lt->pred, lt->pred, true);
	  tmp->left->ext.lit = NULL;
	  tmp = tmp->right;
	}
  }
  return 1;
}

int ParseNode::ProcessRange()
{
  return 1;
}

int ParseNode::ProcessPriority()
{
  static int done = 0;
  
  if (done)
    return 0;

  if (!sys_data.use_priorities) {
    error(USR_ERR, "%s: You should enable option `--priorities' to"
          " allow ordered disjunctions",
          error_file_and_line(right->lineno));
    done = 1;
    return 0;
  }
  if (!priority_data.choices) {
    priority_data.choices = new ChoiceList();
    priority_data.tester = new RuleList();
    priority_data.mp = new MetaPriorities();
    
    priority_data.choice_pred =
      Predicate::DefinePredicate(clone_string(CHOICE_PRED), 2, lineno);
    dependency_graph->AddEdge(priority_data.choice_pred,
                              priority_data.choice_pred, true);
    priority_data.meta_pred =
      Predicate::DefinePredicate(clone_string(META_PRED), 2, lineno);
    dependency_graph->AddEdge(priority_data.meta_pred,
                              priority_data.meta_pred, true);
    predicates[priority_data.meta_pred]->is_priority_predicate = 1;
    predicates[priority_data.meta_pred]->is_priority_meta_predicate = 1;
    priority_data.old_satisfied_pred  =
      Predicate::DefinePredicate(clone_string(OLD_SATISFIED_PRED), 2,
                                 lineno);
    dependency_graph->AddEdge(priority_data.old_satisfied_pred,
                              priority_data.old_satisfied_pred, true);
    priority_data.satisfied_pred  =
      Predicate::DefinePredicate(clone_string(SATISFIED_PRED), 2, lineno);
    dependency_graph->AddEdge(priority_data.satisfied_pred,
                              priority_data.satisfied_pred, true);
    predicates[priority_data.satisfied_pred]->is_priority_predicate = 1;
    priority_data.rule_pred =
      Predicate::DefinePredicate(clone_string(RULE_PRED), 1, lineno);
    predicates[priority_data.rule_pred]->is_priority_predicate = 1;
    priority_data.degree_pred =
      Predicate::DefinePredicate(clone_string(DEGREE_PRED), 1, lineno);
    
    priority_data.activate_pred =
      Predicate::DefinePredicate(clone_string(ACTIVATE_PRED), 1, lineno);
    dependency_graph->AddEdge(priority_data.activate_pred,
                              priority_data.activate_pred, true);
    predicates[priority_data.activate_pred]->is_priority_predicate = 1;

    priority_data.better_pred =
      Predicate::DefinePredicate(clone_string(BETTER_PRED), 1, lineno);
    dependency_graph->AddEdge(priority_data.better_pred,
                              priority_data.satisfied_pred, true);
    priority_data.worse_pred =
      Predicate::DefinePredicate(clone_string(WORSE_PRED), 1, lineno);
    dependency_graph->AddEdge(priority_data.worse_pred,
                              priority_data.satisfied_pred, true);
    priority_data.excused_pred =
      Predicate::DefinePredicate(clone_string(EXCUSED_PRED), 1, lineno);
    dependency_graph->AddEdge(priority_data.excused_pred,
                              priority_data.satisfied_pred, true);

    if (priority_data.semantics == PRI_CARDINALITY) {
      priority_data.sat_card_pred =
        Predicate::DefinePredicate(clone_string(CARDINALITY_PRED), 2,
                                   lineno);
      priority_data.old_sat_card_pred =
        Predicate::DefinePredicate(clone_string(OLD_CARDINALITY_PRED),
                                   2, lineno);
      priority_data.possibly_sat_pred =
        Predicate::DefinePredicate(clone_string(POSSIBLY_PRED),
                                   1, lineno);

      dependency_graph->AddEdge(priority_data.sat_card_pred,
                                priority_data.sat_card_pred,
                                true);
      dependency_graph->AddEdge(priority_data.old_sat_card_pred,
                                priority_data.old_sat_card_pred,
                                true);
      
    }
    
    if (sys_data.use_interactive_priorities) {
      priority_data.control_pred =
        Predicate::DefinePredicate(clone_string(META_CONTROL_PRED), 1,
                                   lineno);
      dependency_graph->AddEdge(priority_data.control_pred,
                                priority_data.control_pred, true);
    }
  
  }
  done = 1;
  return 0;
}

#ifdef DEBUG
// recursively print the tree in preorder
void ParseNode::Print(int ind)
{
  indent(ind);
  output(cerr, "Type: %s\n", parse_strings[type]);
  if (sval) {
    indent(ind);
    output(cerr, "SVal: %s\n", sval);
  }
  switch (type) {
  case PT_COMPUTE:
    if (l1.val > 0) {
      indent(ind);
      output(cerr, "Models: %ld\n", l1.val);
    }
    break;
  case PT_SPECIAL_TAIL:
  case PT_SPECIAL_HEAD:
    break;
  case PT_NUMBER:
    indent(ind);
    output(cerr, "value: %ld\n", l1.val);
    break;
  case PT_RANGE:
    indent(ind);
    output(cerr, "conj: %d\n", i1.conj);
    indent(ind);
    output(cerr, "start: %ld\n", l1.start);
    indent(ind);
    output(cerr, "end: %ld\n", l2.end);
    break;
  case PT_EXPR:
    indent(ind);
    output(cerr,"function: %s\n", function_strings[l1.op]);
    break;
  case PT_ATOM:
    indent(ind);
    output(cerr,"weight head: %d\n", i1.weight);
  case PT_FUNCTION: /* fall through */
    indent(ind);
    output(cerr,"negative: %d\n", l2.negative);
    break;
  default:
    break;
  }
  indent(ind);
  output(cerr, "Lineno: %ld\n", lineno);

  if (left){
    indent(ind);
    output(cerr, "left:\n");
    left->Print(ind +3);
  }
  if (right) {
    indent(ind);
    output(cerr, "right:\n");
    right->Print(ind+3);
  } 
}
#endif

  
