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
#include "Dictionnary.h"
#include "PosAtom.h"

#include <iterator>


/* Dictionnary class methods definitions */

Dictionnary::Dictionnary(){
   atomsNumber = 0;
}

Dictionnary::~Dictionnary(){
// atoms deletion
  int sizeVector = this->getSize();
  for (int i = 1; i <= sizeVector; i++)
	delete (*this)[i];
// clear vector	
  this->clear();
}

int Dictionnary::getSize(){
   return atomsNumber;
}

void Dictionnary::setSize(int n){
   atomsNumber = n;
}


MyAtom* Dictionnary::addAtom(int num){
    
    if (getSize()<= num) {
    	resize(num+1);
	setSize(num+1);
    }
    PosAtom* a = new PosAtom(num);
    (*this)[num] = a;
    return a;
}
        
void Dictionnary::writeInformation(){
    int sizeVector = this->getSize();
    for (int i = 1; i <= sizeVector; i++){
        cout << i << " - " << (*this)[i]->getName() << "\n";
    }
    cout << endl;
}

MyAtom* Dictionnary::getAtom(int nPlace){
	return (*this)[nPlace];
}


void Dictionnary::clean(){
   int i = 1, deg, n = this->getSize();
   MyAtom* atom;
   while (i <= n){
     atom = this->getAtom(i);
     deg = atom->is_nu__();
     if (deg){
	atom->getPlusRules()->setAllDegrees(deg);
	atom->getPlusRules()->setAllTypes(POSSIBILISTIC_);
	atom->getPlusRules()->eraseAtom(atom);
	delete atom;
	(*this)[i]=(*this)[n];
	n--;
     }
     else {
        atom->setValue(i);
	i++;
     }	
   }
  this->setSize(n); 
}
