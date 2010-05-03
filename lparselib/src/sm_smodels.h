// Copyright 1998 by Patrik Simons
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston,
// MA 02111-1307, USA.
//
// Patrik.Simons@hut.fi

#ifndef SM_SMODELS_H
#define SM_SMODELS_H

typedef enum {
  endrule,
  basicrule,
  constraintrule,
  choicerule,
  generaterule,
  weightrule,
  optimizerule
} SM_RuleType;

#ifdef USEDOUBLE
typedef double SM_Weight;
#define WEIGHT_MAX DBL_MAX
#define WEIGHT_MIN -DBL_MAX
#else
typedef unsigned long SM_Weight;
#define WEIGHT_MAX ULONG_MAX
#define WEIGHT_MIN 0
#endif


class Program;
class Atom;
class Tree;
class Init;
class Queue;


class Api
{
public:
  Api (Program *);
  virtual ~Api ();

  void begin_rule (SM_RuleType);  // First call begin_rule
  void add_head (Atom *);      // Then call these
  void add_body (Atom *);         // Add atom to body of rule

  void change_body (long i, bool pos, SM_Weight); // Change weight in body
  void set_atleast_weight (SM_Weight);   // SM_Weight rule
  void set_atleast_body (long);       // Constraint rule
  void set_atleast_head (long);       // Generate rule
  void end_rule ();                   // Finally, end with end_rule

  virtual Atom *new_atom ();        // Create new atom
  void set_compute (Atom *, bool);  // Compute statement
  void reset_compute (Atom *, bool);  // Compute statement
  void set_name (Atom *, const char *);
  void remember ();  // Remember the set_name calls after this
  void forget ();    // Forget the set_name calls after this
  Atom *get_atom (const char *); // get_atom only works for the
                                 // set_name calls that have
                                 // been remembered
  void copy (Api *);             // Make a copy of the program
  void done ();  // Call done after the program is completely defined
		 // and before you begin to compute

protected:
  void set_init ();

  Program * const program;
  SM_RuleType type;
  class list {
  public:
    list ();
    ~list ();
    void push (Atom *a, SM_Weight w = 0);
    void reset ();
    void grow ();
    
    Atom **atoms;
    SM_Weight *weights;
    int top;
    int size;
  };
  long size (list &);
  list pbody;
  list nbody;
  list head;
  SM_Weight atleast_weight;
  long atleast_body;
  long atleast_head;
  bool maximize;
  Tree *tree;
  Tree *pointer_to_tree;
  Init *init;
};


class SM_Rule;
class Atom;
class Program;

struct Auxiliary
{
  Auxiliary (bool p = false) { positive = p; in_loop = p; };
  SM_Weight weight; // Local weight
  bool positive : 1; // Is the literal an atom?
  bool in_loop : 1;  // Is the atom in a strongly connected component?
  Atom **a;          // Position in rule body
};

struct Follows
{
  SM_Rule *r;
  Auxiliary *aux;
};

struct Init
{
  long hsize;
  long psize;
  long nsize;
  Atom **head;
  Atom **pbody;
  Atom **nbody;
  SM_Weight *pweight;
  SM_Weight *nweight;
  SM_Weight atleast_weight;
  long atleast_body;
  long atleast_head;
  bool maximize;
};


class Atom
{
public:
  Atom (Program *p);
  ~Atom ();

  void setBTrue ();
  void setBFalse ();
  void setTrue ();
  void reduce_head ();
  void reduce_pbody (bool strong);
  void reduce_nbody (bool strong);
  void backchainTrue ();
  void backchainFalse ();

  void reduce (bool strongly);
  void unreduce ();
  const char *atom_name ();
  void etrue_push ();
  void efalse_push ();
  void queue_push ();
  void visit ();

  Program * const p;  // The program in which the atom appears
  Follows *endHead;   // Sentinel of the head array
  Follows *endPos;    // Sentinel of the pos array
  Follows *endNeg;    // Sentinel of the neg array 
  Follows *endUpper;  // Sentinel of the upper array = pos array, used
		      // to localize and thereby speed up the upper
		      // closure computation
  Follows *end;       // The end of the array
  Follows *head;      // The rules whose heads are atom
  Follows *pos;       // The rules in which atom appears positively
		      // (allocated in head)
  Follows *neg;       // The rules in which atom appears negatively
		      // (allocated in head)
  union {
    SM_Rule *source;     // The rule that keeps this atom in the upper closure
    Atom *copy;       // Only used when copying
  };
  long headof;        // The number of rules in the head array whose
		      // inactive counter is zero
  bool closure : 1;       // True if the atom is in the upper closure
  bool Bpos : 1;          // True if the atom is in B+
  bool Bneg : 1;          // True if the atom is in B-
  bool visited : 1;       // For use in backjumping and improvement of
		          // upper closure
  bool guess : 1;         // True if the atom currently is a guess
  bool isnant : 1;        // True if the atom is in NAnt(P)
  bool dependsonTrue : 1; // Used by lookahead
  bool dependsonFalse : 1;// Used by lookahead
  bool computeTrue : 1;   // Compute statement
  bool computeFalse : 1;  // Compute statement
  bool backtracked : 1;   // For use in printStack
  bool forced : 1;        // For use in printStack
  bool in_etrue_queue : 1;// Atom is in equeue, will get Bpos == true
  bool in_efalse_queue : 1;// Atom is in equeue, will get Bneg == true
  bool in_queue : 1;      // Atom is in queue
  bool pos_tested : 1;    // True if the atom been tested in lookahead
  bool neg_tested : 1;    // True if the atom been tested in lookahead
  unsigned long posScore; // Used in heuristic, in improvement of upper
		          // closure, and in print_internal
  unsigned long negScore; // Used in heuristic, in improvement of upper
		          // closure, and in print_internal
  char *name;             // The name of the atom
};


class SM_Rule 
{
public:
  SM_Rule ();
  ~SM_Rule ();

  void swapping (Follows *, Follows *) {}
  void inactivate (const Follows *);
  void fireInit ();
  void not_in_loop (Follows *){}
  void in_loop (Follows *) {}
  void mightFireInit (const Follows *);
  void unInit ();
  void mightFire (const Follows *);
  void backChainTrue ();
  void backChainFalse ();
  void backtrackFromActive (const Follows *);
  void backtrackFromInactive (const Follows *);
  void propagateFalse (const Follows *);
  void propagateTrue (const Follows *);
  void backtrackUpper (const Follows *);

  bool isInactive ();
  bool isUpperActive ();
  bool isFired ();
  void search (Atom *);
  void reduce (bool strongly);
  void unreduce ();
  void setup ();
  void init (Init *);
  void print ();
  void print_internal ();

  Atom *head;        // The head of the rule
  Atom **nbody;      // The negative literals in the body
  Atom **pbody;      // The positive literals in the body (allocated
		     // in nbody)
  Atom **nend;       // Sentinel of the nbody array
  Atom **pend;       // Sentinel of the pbody array
  Atom **end;        // The end of the body array

  long lit;          // The number of negative literals in body
                     // that are not in B- plus the number of positive
                     // literals in body that are not in B+
  long upper;        // The number of positive literals in body that
		     // are in the upper closure
  long inactive;     // The number of positive literals in B- plus
		     // the number of negative literals in B+
  SM_RuleType type;
  bool visited;
};

class Atom;
class Program;
class Smodels;

class Dcl
{
public:
  Dcl (Smodels *s);
  ~Dcl ();
  void init ();
  void setup ();
  void revert ();
  void dcl ();
  bool propagateFalse ();
  int path (Atom *a, Atom *b);
  void reduce (bool strongly);
  void unreduce ();
  void improve ();
  void denant ();
  void undenant ();
  void print ();

protected:
  Smodels * const smodels;
  Program * const program;
  
  Atom **tmpfalse;
  long tmpfalsetop;
};

class SM_Rule;
class Atom;

class Node
{
public:
  Node () { next = 0; rule = 0; };
  ~Node () {};

  Node *next;
  union {
    SM_Rule *rule;
    Atom *atom;
  };
};

class SM_List
{
public:
  SM_List ();
  ~SM_List () {};

  void push (Node *n);
  Node *head ();

private:
  Node list;
  Node *tail;
};

class AtomSM_List : public SM_List
{
public:
  AtomSM_List () {};
  ~AtomSM_List ();

  void push (Atom *);
};

class SM_RuleSM_List : public SM_List
{
public:
  SM_RuleSM_List () {};
  ~SM_RuleSM_List ();

  void push (SM_Rule *);
};


inline Node *
SM_List::head ()
{
  return list.next;
}

inline void
SM_List::push (Node *n)
{
  tail->next = n;
  tail = n;
}

inline void
AtomSM_List::push (Atom *a)
{
  Node *n = new Node;
  n->atom = a;
  SM_List::push (n);
}

inline void
SM_RuleSM_List::push (SM_Rule *r)
{
  Node *n = new Node;
  n->rule = r;
  SM_List::push (n);
}


class Atom;

class Queue
{
public:
  Queue (long size = 0);
  ~Queue ();
  void Init (long size);

  void reset ();
  void push (Atom *n);
  Atom *pop ();
  int empty ();

  Atom **queue;
  long last;
  long top;
  long bottom;
};


class Program
{
public:
  Program ();
  ~Program ();
  void init ();
  void print ();
  void print_internal (long models = 1);
  void set_optimum ();

  Queue equeue;
  Queue queue;
  
  AtomSM_List atoms;
  SM_RuleSM_List rules;

  long number_of_atoms;
  long number_of_rules;

  bool conflict;
};

#include <assert.h>

inline int
Queue::empty ()
{
  return top == bottom;
}

inline void 
Queue::reset ()
{
  top = 0;
  bottom = 0;
}

inline void 
Queue::push (Atom *n)
{
  queue[bottom] = n;
  bottom++;
  if (bottom > last)
    bottom = 0;
  assert (top != bottom);
}

inline Atom *
Queue::pop ()
{
  assert (top != bottom);
  Atom *n = queue[top];
  top++;
  if (top > last)
    top = 0;
  return n;
}

class Stack
{
public:
  Stack (long size = 0);
  ~Stack ();
  void Init (long size);
  
  void push (Atom *item);
  Atom *pop ();
  bool empty ();
  bool full ();
  void reset ();
  
  Atom **stack;
  long top;
  long last;
};


class Smodels
{
public:
  Smodels ();
  ~Smodels ();
  void init ();
  void setToBTrue (Atom *a);
  void setToBFalse (Atom *a);
  void pick ();
  void expand ();
  void setup ();
  int model ();
  bool testPos (Atom *);
  bool testNeg (Atom *);
  void reset_dependency ();
  void clear_dependency ();
  void add_dependency (Atom *, bool);
  void reset_queues ();
  void print ();
  void printAnswer ();
  void removeAtom (long);
  void addAtom (Atom *);

  Dcl dcl;
  Program program;
  Atom **atom;           // The atoms in the program. We walk the
			 // array when we do lookahead or compute the
			 // heuristic.
  long atomtop;          // Atoms on and above atomtop are in B and
			 // can be ignored.
  Stack depends;         // Holds atoms with dependson{True,False} true
  Stack stack;           // Holds the backtrack information (the atoms
			 // in B). When full we have a stable model.
  long setup_top;        // Top of stack after setup

};

class Atom;


inline void
Stack::push (Atom *item)
{
  assert (top < last);
  stack[top] = item;
  top++;
}

inline Atom *
Stack::pop ()
{
  assert (top > 0);
  top--;
  return stack[top];
}

inline bool
Stack::empty ()
{
  return top == 0;
}

inline bool
Stack::full ()
{
  return top == last;
}

inline void
Stack::reset ()
{
  top = 0;
}

class Tree
{
public:
  Tree ();
  ~Tree ();

  Atom *find (const char *);
  void insert (Atom *);
  void remove (Atom *);

  void check_consistency ();

private:
  class Node
  {
  public:
    Node (Atom *k = 0, Node *l = 0, Node *r = 0);
    ~Node ();
    
    Atom *key;
    Node *left;
    Node *right;
  };

  Node *root;

  Node *splay (const char *, Node *);

  void check_consistency (Node *);
  void flush (Node *);

  int compare (const char *, Atom *);
};

#endif
