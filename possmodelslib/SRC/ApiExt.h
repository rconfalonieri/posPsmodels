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
#ifndef _API_EXT_H_
#define _API_EXT_H_

#include "MyAtom.h"
// SModels Header
#include "smodels.h"
#include "api.h"
#include "atomrule.h"

class AtomExt : public Atom {
  private :
     MyAtom* myAtom; // corresponding atom of posSmodels
  public :
     AtomExt(Program* p);
     ~AtomExt();
     void setMyAtom(MyAtom* a);
     MyAtom* getMyAtom();
};

class ApiExt : public Api {
	public:
        //Constructor
        ApiExt(Program* p);
        //Destructor
        ~ApiExt();
	
	AtomExt* new_atom_ext (MyAtom* a);  // Create new atom 
	AtomExt* get_atom_ext (const char *name);
	
};

#endif //_API_EXT_H_
