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
#ifndef _LP_IO_H_
#define _LP_IO_H_

#include "Definitions.h"
#include "PosLogProg.h"

class lpio{
// for reading a program (with nu__() atoms) in smodels internal format
  private:
    char fileName[50];
    FILE * lpFile;
    IntSet* atoms; // atoms that appear in the program file

  protected:
    //! Read an integer in the file.
    int fReadInt();
    //! Read a string (separated by spaces) in the file.
    void fReadStr(char *);

    // --- Various methods ---
    bool existsAtom(int a);
    MyAtom* addAtom(int a, PosLogProg* plp);


  public:
    // --- Constructor/Destructor -----------------------------------
    lpio(char* name);
    lpio();
    ~lpio();

    //! Read the logicProgram file.
    PosLogProg* readLp();
    //! Read a rule in the file. return "true" if the EOF is reached
    bool fReadRule(PosLogProg* plp, PosRule * r);
    //! Read the atoms names and return the atoms number. 
    int fReadAtoms(PosLogProg* plp);
    //! Read the compute statement 
    void fReadComputeStat(PosLogProg* plp, PosRule* rule);
};

#endif //_LP_IO_H_

