// Copyright (C) 2005 Claire Lefèvre, Pascal Nicolas
// 
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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
// http://www.gnu.org/copyleft/gpl.html
//
// claire.lefevre@univ-angers.fr       pascal.nicolas@univ-angers.fr
// 

#include "lpio.h"

// --- Constructor/Destructor --------------------------------------
lpio::lpio(char * fName){
  strcpy(fileName,fName);
  lpFile=fopen(fName,"r");
  atoms = new IntSet;
}

lpio::lpio(){
  lpFile=stdin;
  atoms = new IntSet;
}

lpio::~lpio(){
  if (lpFile!=stdin) fclose(lpFile);
  delete atoms;
}

// test if atom "a" appears for the first time or not
bool lpio::existsAtom(int a){
  return (atoms->find(a) != atoms->end());
}

// creates a new MyAtom if "a" appears for the first time
// return the MyAtom corresponding to number "a"
MyAtom* lpio::addAtom(int a, PosLogProg* plp){
  if (existsAtom(a)){
      return plp->getAtom(a);
  } else {
      atoms->insert(a);
      return plp->addAtom(a);
  }   
}  

// Reads the logicProgram file 
PosLogProg* lpio::readLp(){
  PosLogProg* plp = new PosLogProg;
  PosRule* rule;
  bool end;
  // rules
  rule = new PosRule();
  end=fReadRule(plp,rule);
  while (not end){
    plp->addRule(rule);
    rule = new PosRule();
    end=fReadRule(plp,rule);
  };
  delete rule;
  // Atoms names
  int atomsNbr = fReadAtoms(plp);
  plp->getDictionnary()->setSize(atomsNbr);
  // compute statement
  rule = new PosRule();
  rule->setType(COMPUTE_);
  fReadComputeStat(plp,rule); // positive part
  fReadComputeStat(plp,rule); // negative part
  plp->addRule(rule);

  return plp;
}

// Reads a rule in the file.
//  (return "true" if the EOF is reached).
bool lpio::fReadRule(PosLogProg* plp, PosRule* rule) {
  int cardBody;      // number of rules in body+
  int cardNegBody;   // number of rules in body-
  int atomNum;       // atom number
  MyAtom* atom;	     // pointer to the atom
  int i;
  if (fReadInt()==0) return true;  // it is the end of rules declaration.
  else {
    // We read the rule's head :
    atomNum = fReadInt();
    atom = addAtom(atomNum, plp);
    rule->setHead(atom);
    atom->getHeadRules()->addRule(rule);
    // We read the rule's bodies (Body+ and Body-) cardinalities :
    cardBody=fReadInt();
    cardNegBody=fReadInt();
    // We read the rule negative body :
    for (i=0;i<cardNegBody;i++){
       atomNum = fReadInt();
       atom = addAtom(atomNum, plp);
       rule->addBodyMinus(atom);
    }   
    // We read the rule positive body :
    for (i=0;i<(cardBody-cardNegBody);i++){
       atomNum = fReadInt();
       atom = addAtom(atomNum, plp);
       if (!rule->getBodyPlus()->contains(atom)){ // pb doublons dans body+
       	rule->addBodyPlus(atom);
       	atom->getPlusRules()->addRule(rule);
       }	
    }
    return false;
  }
}
// Reads the atoms names and return the atoms number :
int lpio::fReadAtoms(PosLogProg* plp){
  int idAtom,atomsNb=0, i=1;
  char str[NAME_LGTH];
  idAtom=fReadInt();
  while (idAtom!=0){
    if (idAtom != i){ // there are "anonymous" atoms
       MyAtom* atom = addAtom(i, plp);
       atom->setName("_false_");
    }   
    else {   
       atomsNb=idAtom;
       fReadStr(str);
       plp->getAtom(idAtom)->setName(str);
       idAtom=fReadInt();
    }
    i++;
  };
  return atomsNb;
}

// Reads the positive (resp. negative) part of the compute statement
void lpio::fReadComputeStat(PosLogProg* plp, PosRule* rule){
  char str[NAME_LGTH];
  bool positive;
  int atomNum;
  MyAtom* atom;
  fReadStr(str);
  positive = (strcmp(str,"B+")==0);
  do {
    atomNum=fReadInt();
    if (atomNum!=0){
       atom = addAtom(atomNum, plp);
       if (positive) rule->addBodyPlus(atom);
       else rule->addBodyMinus(atom);
    }  
  } while (atomNum!=0);
}

// Reads an integer in the file.
int lpio::fReadInt(){
  char c = fgetc(lpFile);
  char s[NAME_LGTH];
  int i=0;
  while (c==' ') c = fgetc(lpFile);
  while (c=='\n') c = fgetc(lpFile);
  while ((c!=' ')&&(c!='\n')){
        s[i]=c;
        c = fgetc(lpFile);
        i++;
  }
  s[i]='\0';
  return atoi(s);
}


// Reads a string (separated by spaces) in the file.
void lpio::fReadStr(char * s){
  char c = fgetc(lpFile);
  int i=0;
  while (c==' ') c = fgetc(lpFile);
  while (c=='\n') c = fgetc(lpFile);
  while ((c!=' ')&&(c!='\n')){
        s[i]=c;
        c = fgetc(lpFile);
        i++;
  }
  s[i]='\0';
}


