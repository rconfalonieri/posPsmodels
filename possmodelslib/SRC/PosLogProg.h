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
#ifndef _POS_LOG_PROG_H_
#define _POS_LOG_PROG_H_

#include "Dictionnary.h"
#include "AtomSet.h"
#include "PosAtomSet.h"
// SModels Header
#include "smodels.h"

using namespace std;


class PosLogProg : public PosRuleVector
{
	public:
        //Constructor
        PosLogProg();
        //Destructor
        ~PosLogProg();
	
	Dictionnary* getDictionnary();
	void setDictionnary(Dictionnary* dic);
	MyAtom* getAtom(int n);
	MyAtom* addAtom(int n);
	void writeInformation();
	PosAtomSet* possCn(AtomSet* aps);
	PosAtomSet* nextPosSmodel(Smodels* smodels);
	void writePosSmodels(Smodels* smodels, int n);
	
    private:
         Dictionnary* dictionnary;
};

#endif //_POS_LOG_PROG_H_
