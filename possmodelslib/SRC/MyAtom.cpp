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
#include "MyAtom.h"
#include <stdlib.h>
#include <string.h>

/* MyAtom class methods definitions */

MyAtom::MyAtom(int n) {
  value = n;
  headRules = new PosRuleVector;
  plusRules = new PosRuleVector;
  minusRules = new PosRuleVector;
}

MyAtom::~MyAtom() {
  delete[] name;
  delete headRules;
  delete plusRules;
  delete minusRules;
}

int MyAtom::getValue(){
   return value;
}

void MyAtom::setValue(int a){
   value = a;
}

void MyAtom::setName(char* n){
   name = new char[strlen(n)+1];
   strcpy(name, n);
}

char* MyAtom::getName(){
   return name;
}


PosRuleVector* MyAtom::getHeadRules(){
	return headRules;
}

PosRuleVector* MyAtom::getPlusRules(){
	return plusRules;
}

PosRuleVector* MyAtom::getMinusRules(){
	return minusRules;
}

// test if the atom's name is of the form "nu__(xxx)"
// return integer xxx if it is the case, else return 0
int MyAtom::is_nu__(){
  char* n=this->getName();
  char num[50];
  if (strncmp(n,"nu__(",5)==0){
     int i=5, j=0;
     while (n[i]>='0' && n[i]<='9'){
        num[j]=n[i];
	i++; j++;
     };
     num[j]='\0';
     return(atoi(num));
   } else
     return 0;
}


