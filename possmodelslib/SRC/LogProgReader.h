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
#ifndef _LOG_PROG_READER_H_
#define _LOG_PROG_READER_H_

#include "PosLogProg.h"
// SModels Header
#include "smodels.h"

class LogProgReader {
	public:
        //Constructor
        LogProgReader();
        //Destructor
        ~LogProgReader();
	
	PosLogProg* getProg();
	void setProg(PosLogProg* p);
	Smodels* getSmodels();
	void setSmodels(Smodels* m);
	int getNumber();
	void setNumber(int n);
	int read(char *fName);  // reads output of lparse, and constructs non possibilistic normal lp
	void cleanProg(); // transforms non possibilistic lp (with nu__() atoms) into possibilistic lp
	void constructProgInSmodels();
	void writePosSmodels();

    private:
    	PosLogProg* cLogProg;
	Smodels* sModels;
	int modelsNumber;
};

#endif //_LOG_PROG_READER_H_
