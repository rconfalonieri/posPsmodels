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
#include "LogProgReader.h"
#include "AtomSet.h"
#include "PosRule.h"
#include "lpio.h"
#include "ApiExt.h"
#include "timer.h"
#include <iterator>
#include <algorithm>

/* LogProgReader class methods definitions */

LogProgReader::LogProgReader() {
}

LogProgReader::~LogProgReader(){
    delete (cLogProg);
    delete sModels;
}

PosLogProg* LogProgReader::getProg(){
	return cLogProg;
}	
void LogProgReader::setProg(PosLogProg* p){
	cLogProg = p;
}
Smodels* LogProgReader::getSmodels(){
	return sModels;
}	
void LogProgReader::setSmodels(Smodels* m){
	sModels = m;
}
int LogProgReader::getNumber(){
	return modelsNumber;
}
void LogProgReader::setNumber(int n){
	modelsNumber = n;
}

int LogProgReader::read(char *fName){
Timer timer;
timer.start();
   lpio* interf=new lpio(fName);
   this->setProg(interf->readLp());
   if (!this->getProg()) return 1;
//   this->getProg()->writeInformation();
timer.stop();
cout<<"Reading , Duration : "<<timer.print()<<endl;
    delete interf;
	return 0;
}

void LogProgReader::cleanProg(){
   this->getProg()->getDictionnary()->clean();
//   this->getProg()->writeInformation();
}

void LogProgReader::constructProgInSmodels(){
Timer timer;
timer.start();
	Smodels* smodels = new Smodels;
        ApiExt api(&smodels->program);
	PosLogProg* lp = this->getProg();

        Atom *aTmp;
	MyAtom* myAtom;
	char* nameAtom;
	// Keep track of atom names
	api.remember();

	PosRuleVector::iterator iterRule = lp->begin();
	for (; iterRule != lp->end(); iterRule++){
	   PosRule* rTmp = *iterRule;
	   if (rTmp->getType() != COMPUTE_){
		// Creation of a new rule in smodels
		api.begin_rule(BASICRULE);
		// Head
		myAtom = rTmp->getHead();
		nameAtom = myAtom->getName();
		if (!(aTmp = api.get_atom(nameAtom))){ // Check if the atom doesn't exist
			// It is not referenced so we create a new atom
			aTmp = api.new_atom_ext(myAtom );
			api.set_name(aTmp, nameAtom);
		}
		api.add_head(aTmp);
		// Definition of the body's rule
		// Body Plus
		AtomSet* bodyPlus = rTmp->getBodyPlus();
		AtomSet::iterator iter;
		for (iter = bodyPlus->begin(); iter != bodyPlus->end(); iter++){
			myAtom = *iter;
			nameAtom = myAtom->getName();
			if (!(aTmp = api.get_atom(nameAtom))){
				aTmp = api.new_atom_ext(myAtom);
				api.set_name(aTmp, nameAtom);
			}
			api.add_body(aTmp, true);
		}
		// Body Minus
		AtomSet* bodyMinus = rTmp->getBodyMinus();
		for (iter = bodyMinus->begin(); iter != bodyMinus->end(); iter++){
			myAtom = *iter;
			nameAtom = myAtom->getName();
			if (!(aTmp = api.get_atom(nameAtom))){
				aTmp = api.new_atom_ext(myAtom);
				api.set_name(aTmp, nameAtom);
			}
			api.add_body(aTmp, false);
		}
		// Definition of the rule finished
		api.end_rule();
	   }
	   else{ // compute statement
	   // Body Plus 
	        AtomSet* bodyPlus = rTmp->getBodyPlus();
		AtomSet::iterator iter;
		for (iter = bodyPlus->begin(); iter != bodyPlus->end(); iter++){
		   myAtom = *iter;
		   nameAtom = myAtom->getName();
		   if (!(aTmp = api.get_atom(nameAtom))){
			 aTmp = api.new_atom_ext(myAtom);
			 api.set_name(aTmp, nameAtom);
		   }
		   api.set_compute(aTmp, true);
		}
		// Body Minus
		AtomSet* bodyMinus = rTmp->getBodyMinus();
		for (iter = bodyMinus->begin(); iter != bodyMinus->end(); iter++){
			myAtom = *iter;
			nameAtom = myAtom->getName();
		   	if (!(aTmp = api.get_atom(nameAtom))){
				aTmp = api.new_atom_ext(myAtom);
				api.set_name(aTmp, nameAtom);
		  	}
			api.set_compute(aTmp, false);
		}
	    }
	}
	// Definition of the program is finished
	api.done();
timer.stop();
cout<<"Constructing program in Smodels, Duration : "<<timer.print()<<endl;
	this->setSmodels(smodels);
}


void LogProgReader::writePosSmodels(){
  this->getProg()->writePosSmodels(this->getSmodels(), this->getNumber());
}

