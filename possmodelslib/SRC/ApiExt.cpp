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
#include "ApiExt.h"

// class AtomExt : subclass of Atom class from smodels

AtomExt::AtomExt(Program* p): Atom(p){
}
AtomExt::~AtomExt(){
}

void AtomExt::setMyAtom(MyAtom* a){
   myAtom = a;
}

MyAtom* AtomExt::getMyAtom(){
   return myAtom;
}

// class ApiExt :  subclass of Api class from smodels
//--------------
//Constructor and Destructor
ApiExt::ApiExt(Program* p): Api(p){
}
ApiExt::~ApiExt(){
}
// Create new atom(ext)
AtomExt* ApiExt::new_atom_ext (MyAtom* a){
  AtomExt *ae = new AtomExt (program);
  program->atoms.push (ae);
  program->number_of_atoms++;
  ae->setMyAtom(a);
  return ae;
}

AtomExt* ApiExt::get_atom_ext (const char *name){
   Atom* a= get_atom(name);
   AtomExt* ae = (AtomExt*) a;
   return ae;
}


