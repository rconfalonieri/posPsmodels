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
#ifndef _MY_RULE_H_
#define _MY_RULE_H_

#include "Definitions.h"
#include <iostream>

class MyAtom;
class AtomSet;

class MyRule
{
    public:
        //Constructor
        MyRule();
        //Destructor
        ~MyRule();
        
        void setHead(MyAtom* newHead);
        MyAtom* getHead();
	AtomSet* getBodyPlus();
	AtomSet* getBodyMinus();
        void addBodyPlus(MyAtom* newEntry);
        void addBodyMinus(MyAtom* newEntry);
	void setType(int t);
	int getType();
        void writeInformation();
	void setDiscarded(bool v);
	bool isDiscarded();
	void setCounter();
	void decCounter();
	bool isFirable();
    private:
        MyAtom* head;
        AtomSet* bodyPlus;
        AtomSet* bodyMinus;
	int ruleType;
	int counter;	
};

#endif // _MY_RULE_H_
