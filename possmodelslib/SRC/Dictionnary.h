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
#ifndef _DICO_H_
#define _DICO_H_

#include "MyAtom.h"
#include <vector>
#include <iostream>

using namespace std;

class Dictionnary : public vector<MyAtom*>
{
    public:
        //Constructor
        Dictionnary();
        //Destructor
        ~Dictionnary();

	int getSize();
	void setSize(int n);
	MyAtom* getAtom(int n);
        MyAtom* addAtom(int n);
        void writeInformation();
	void clean();
    private:
         int atomsNumber;

};

#endif //_DICO_H_
