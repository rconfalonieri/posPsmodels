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
#ifndef _DEFINITIONS_H_
#define _DEFINITIONS_H_

#include <set>
#include <list>

using namespace std;
class PosRule;
typedef list<PosRule*> PosRuleList;
typedef  set<int, less<int> > IntSet;


#define NAME_LGTH          50  // Maximum atom name length


enum { NO_CHANGE = 0, NEW_DEGREE, NEW_ATOM};
enum { POSSIBILISTIC_ = 0, TRUE_NEG_, COMPUTE_};

#endif //_DEFINITIONS_H_


