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
#ifndef _ATOM_H_
#define _ATOM_H_

#include "PosRuleVector.h"
class MyAtom
{
  public:
       MyAtom(int n);
       virtual ~MyAtom();
       int getValue();
       void setValue(int a);
       PosRuleVector* getHeadRules();
       PosRuleVector* getPlusRules();
       PosRuleVector* getMinusRules();
       char* getName();
       void setName(char* name);
       int is_nu__(); 	// test if the atom's name is of the form "nu__(x)"
			// return integer x if it is the case, else return 0

     
  private:
       int value;
       char* name;
       PosRuleVector* headRules; // The rules whose heads are atom
       PosRuleVector* plusRules;  // The rules in which atom appears positively
       PosRuleVector* minusRules;  // The rules in which atom appears negatively
};

#endif // _ATOM_H_


