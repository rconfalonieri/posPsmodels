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
#ifndef _POS_ATOM_SET_H_
#define _POS_ATOM_SET_H_

#include "PosAtom.h"
#include "Definitions.h"
#include <set>

class ltPosAtom{
public :
bool operator()(PosAtom* a1, PosAtom* a2){
	return (  a1 < a2);
}
};


class PosAtomSet : public set<PosAtom*, ltPosAtom>
{
  public :
       void writeInformation();
       int getDegree(MyAtom* a);
       int update(MyAtom* a, int deg);
       void clean();
       void resetAllDegrees();
};

#endif // _POS_ATOM_SET_H_
