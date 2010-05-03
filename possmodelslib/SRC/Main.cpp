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
#include "LogProgReader.h"
#include "timer.h"

int main(int argc, char** argv)
{
    long n = 0;
    char *endptr;
    if (argc>1){
    	n = strtol (argv[1], &endptr, 0);
	if (n < 0 || *endptr != '\0')
	    return 1;
    }
    LogProgReader* reader = new LogProgReader;
    reader->setNumber(n);

    Timer timer;
    timer.start();   
 
    int bad = reader->read();
    if (bad){
    	cerr<< "Error in input" << endl;
        return 1;
    }
    reader->cleanProg();
    reader->constructProgInSmodels();

    reader->writePosSmodels();

    timer.stop();
    cout<<"Total duration : "<<timer.print()<<endl;
  
    delete reader;
   
    return 0;
}


