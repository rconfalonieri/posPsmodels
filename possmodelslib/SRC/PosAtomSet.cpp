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
#include "PosAtomSet.h"

/* PosAtomSet class */

// return the degree of an atom that is known to be in the set
// (else return -1)
int PosAtomSet::getDegree(MyAtom* a){
  PosAtom* pa = (PosAtom*) a;
  return (pa->getDegree());
}

// Add (a,deg) in the set if it is not 
// else update its degree if the new one is greater than the old one 
int PosAtomSet::update(MyAtom* a, int deg){
  PosAtom* pa = (PosAtom*) a;
  int old_deg = pa->getDegree();
  if  (old_deg != -1){ // atom is already in the set
     if (deg > old_deg){
	 pa->setDegree(deg);
	 return NEW_DEGREE;
     }	 
     else
         return NO_CHANGE;
  }
  else{
     pa->setDegree(deg);
     this->insert(pa);
     return NEW_ATOM;
  }
}

void PosAtomSet::clean(){ // cleans possibilistic atoms and clear m
  this->resetAllDegrees();
  this->clear();
}  

void PosAtomSet::resetAllDegrees(){
  PosAtomSet::iterator iter = this->begin();
  for (; iter != this->end(); iter++)
      (*iter)->setDegree(-1);
}

void PosAtomSet::writeInformation(){
	
  PosAtomSet::iterator iter = this->begin();
  for (; iter != this->end(); iter++){
    PosAtom* pa = *iter;
    cout<<"("<<pa->getName()<<", "<<pa->getDegree()<<") ";
  }
  cout<<endl;
}

