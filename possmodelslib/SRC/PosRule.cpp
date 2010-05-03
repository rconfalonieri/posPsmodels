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
#include "PosRule.h"
#include "AtomSet.h"
#include "PosAtomSet.h"
#include <iterator>



/* PosRule class methods definition */

PosRule::PosRule() : MyRule(){
degree = 0;}

PosRule::~PosRule() {};

void PosRule::setDegree(int newDegree){
    degree = newDegree;
}

int PosRule::getDegree(){
    return degree;
}

void PosRule::writeInformation(){
    cout << this->getDegree() << "   ";
    this->MyRule::writeInformation();
}

// return the min degree of body+ and current rule's degree
int PosRule::applicabilityDegree(PosAtomSet* pas){
	int minDegree = this->getDegree();
	AtomSet* body = this->getBodyPlus();
	
	AtomSet::iterator iter = body->begin();
	for (; iter != body->end(); iter++){
		int deg = pas->getDegree(*iter);
		if ( deg < minDegree)
			minDegree = deg;
	}
	return minDegree;
}


