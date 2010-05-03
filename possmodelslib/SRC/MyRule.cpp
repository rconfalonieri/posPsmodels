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
#include "MyRule.h"
#include "AtomSet.h"
#include <iterator>


/* MyRule class */

MyRule::MyRule() : ruleType(TRUE_NEG_) {
    bodyPlus = new AtomSet;
    bodyMinus = new AtomSet;
}

MyRule::~MyRule(){
    delete bodyPlus;
    delete bodyMinus;
}

void MyRule::setHead(MyAtom* newHead){
    head = newHead;
}
        
MyAtom* MyRule::getHead(){
    return head;
}

void MyRule::addBodyPlus(MyAtom* newEntry){
    bodyPlus->insert(newEntry);
}
        
void MyRule::addBodyMinus(MyAtom* newEntry){
   bodyMinus->insert(newEntry);
}

void MyRule::setType(int t){
   ruleType = t;
}

int MyRule::getType(){
   return ruleType;
}

void MyRule::writeInformation(){
    if (this->getType()==COMPUTE_)
    	cout << "compute " << endl;
    else{
        cout<< counter <<"  ";
    	cout << this->getHead()->getName() << " :- ";
    }	
    this->getBodyPlus()->writeInformation();
    cout << " NOT   ";
    this->getBodyMinus()->writeInformation();
    cout << endl;
}


AtomSet* MyRule::getBodyPlus(){
	return bodyPlus;
}

AtomSet* MyRule::getBodyMinus(){
	return bodyMinus;
}

void MyRule::setCounter(){
	counter = this->getBodyPlus()->size();
}

void MyRule::decCounter(){
	this->counter--;
}

bool MyRule::isFirable(){
	return (counter == 0);
}	
