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
#include "PosLogProg.h"
#include "PosRuleVector.h"
#include "ApiExt.h"
#include "timer.h"
#include <iterator>
#include <algorithm>


/* PosLogProg class methods definitions */

PosLogProg::PosLogProg() {
	Dictionnary* d = new Dictionnary;
	dictionnary =d ;
}

PosLogProg::~PosLogProg(){
 //atoms deletion
    delete dictionnary;
 // rules deletion   
    PosRuleVector::iterator iter= this->begin();
    for (;iter != this->end();iter++)
	delete (*iter);
    this->clear();
}

Dictionnary* PosLogProg::getDictionnary(){
    return dictionnary;
}

void PosLogProg::setDictionnary(Dictionnary* dic){
    dictionnary = dic;
}

MyAtom* PosLogProg::getAtom(int num){
   return this->getDictionnary()->getAtom(num);
}

MyAtom* PosLogProg::addAtom(int num){
   return this->getDictionnary()->addAtom(num);
}

void PosLogProg::writeInformation(){   
    cout << "\n\nDICTIONNARY :\n";
    this->getDictionnary()->writeInformation();

    cout << "\nRULES :\n" << endl;
    this->PosRuleVector::writeInformation();
}

// Possibilistic consequenses of a plp
PosAtomSet* PosLogProg::possCn(AtomSet* atomSet){
  Timer timer;
  timer.start();
  PosRuleList* reduct = atomSet->reduct_more();
  PosAtomSet* possModel = new PosAtomSet; // possibilistic stable model
  bool fixPoint ;
  do{   
  	fixPoint = true;
  	PosRuleList::iterator iter= reduct->begin();
  	while (iter != reduct->end()){
	   PosRuleList::iterator it = iter;
	   iter++;
     	   if ((*it)->isFirable()){// a rule is firable if each positive atom of the body is in the possModel
		MyAtom* head = (*it)->getHead();
		int deg = (*it)->applicabilityDegree(possModel);
		if (deg == (*it)->getDegree())// the best is done with this rule
			reduct->erase(it);		 
		int res = possModel->update(head, deg);
		if (res!=NO_CHANGE){
			fixPoint = false;
			if (res==NEW_ATOM){
			   head->getPlusRules()->decAllCounters();
			}   
		}	
    	    } 
	 };
  }while (!fixPoint);
  delete reduct;
  timer.stop();
  cout<<"Possibilistic consequenses, Duration : "<< timer.print()<<endl;
   return possModel;
}


PosAtomSet* PosLogProg::nextPosSmodel(Smodels* smodels){
   if (smodels->model ()){  // Returns 0 when there is no more smodel
	AtomSet atomSet; // the non-possibilistic stable model
	Node* nd = smodels->program.atoms.head();
	for (; nd; nd = nd->next){
	  if (nd->atom->Bpos) {  
	    // The atom is in the stable model
	    AtomExt * ae = (AtomExt*) nd->atom; // cast
            atomSet.insert(ae->getMyAtom());
	  }
	}
	return this->possCn(&atomSet); //the correponding possibilistic stable model
   }
   else 
   	return NULL; // there is no more possibilistic smodel
}

void PosLogProg::writePosSmodels(Smodels* smodels, int n){
// n determines the number of stable models to be computed
// A zero indicates all	
	int ctr = 0;
	if (n==0) n--; 
	PosAtomSet* m;
	// initialize the computation of stable models
	smodels->init();
	while (n!=ctr && (m = this->nextPosSmodel(smodels))){  // Returns NULL when there are no more models
	     ctr++;
	     cout << "\n********** Possibilistic stable model n° "<<ctr<<" **********\n" << endl;
	      m->writeInformation(); // Prints the answer
	      m->clean(); // cleans possibilistic atoms and clear m
	      delete m;
	}
	if (!ctr)
	      cout << "\nCannot find stable model\n" << endl;
	cout << "Number of atoms (smodels): " << smodels->program.number_of_atoms <<
    endl;
        cout << "Number of rules (smodels): " << smodels->program.number_of_rules <<
    endl;
      
}


