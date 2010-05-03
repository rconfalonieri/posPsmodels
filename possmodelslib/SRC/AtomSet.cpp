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
#include "AtomSet.h"

/* AtomSet class */

AtomSet::AtomSet() {}
AtomSet::~AtomSet() {}

bool AtomSet::contains(MyAtom* a){
  return (this->find(a) != this->end());
}


bool AtomSet::emptyIntersection(AtomSet* b){
  AtomSet::iterator i = this->begin();
  for (; i != this->end(); i++) {
     if (b->contains(*i))
      	  return false;
  }
  return true;
}

bool AtomSet::includes(AtomSet* b){
  AtomSet::iterator i = b->begin();
  for (; i != b->end(); i++) {
     if (!this->contains(*i))
      	  return false;
  }
  return true;
}


void AtomSet::writeInformation(){
	
  AtomSet::iterator i = this->begin();
  for (; i != this->end(); i++){
    cout<<(*i)->getName()<<"  ";
  }
}

PosRuleList* AtomSet::reduct_more(){

	PosRuleList* reduct= new PosRuleList; 
	PosRuleVector* rules;
	PosRule* rule;
	AtomSet::iterator i = this->begin();
  	for (; i != this->end(); i++) {
	  rules = (*i)->getHeadRules();
 	  PosRuleVector::iterator  iterRule = rules->begin();
	  for (; iterRule != rules->end(); iterRule++){
	     rule = *iterRule;
	     AtomSet* rAtomSet = rule->getBodyMinus();
	     if (rAtomSet->emptyIntersection(this)){
		if (this->includes(rule->getBodyPlus())){
		   	 reduct->push_front(rule);
			 rule->setCounter();// gives the number of positive atoms in each rule (which are not in the - initialy empty - possModel)
		}	 
	     }
	  }	
	}
	return reduct;
}

