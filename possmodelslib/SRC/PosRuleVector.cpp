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
#include "PosRuleVector.h"
#include "AtomSet.h"
#include <iterator>


/* PosRuleVector class methods definitions */

PosRuleVector::PosRuleVector() 
{}

PosRuleVector::~PosRuleVector(){
// do not delete rules !
    this->clear();
}

void PosRuleVector::addRule(PosRule* r){
	push_back(r);
}	

void PosRuleVector::setAllCounters(){
    PosRuleVector::iterator iter= this->begin();
    for (; iter != this->end();iter++)    {
	(*iter)->setCounter();
    }
}

void PosRuleVector::setAllDegrees(int v){
    PosRuleVector::iterator iter= this->begin();
    for (; iter != this->end();iter++) {
	(*iter)->setDegree(v);
    }
}

void PosRuleVector::setAllTypes(int v){
    PosRuleVector::iterator iter= this->begin();
    for (; iter != this->end();iter++) {
	(*iter)->setType(v);
    }
}

void PosRuleVector::eraseAtom(MyAtom* a){
    PosRuleVector::iterator iter= this->begin();
    for (; iter != this->end();iter++) {
	(*iter)->getBodyPlus()->erase(a);
    }
}

void PosRuleVector::decAllCounters(){
    PosRuleVector::iterator iter= this->begin();
    for (; iter != this->end();iter++) {
	(*iter)->decCounter();
    }
}

void PosRuleVector::writeInformation(){
    PosRuleVector::iterator iter= this->begin();
    for (; iter != this->end(); iter++){
        (*iter)->writeInformation();
    }
}

