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
#include "possmodels.h"
#include <fstream>

//int main(int argc, char** argv)
int possmodels(int argc, char** argv, char *output_file)
{
	//cout << "In possmodel" << endl;

	char *fName = argv[2];
	ifstream::pos_type size;
	long n = 0;
	char *endptr;
	if (argc>1){
		n = strtol (argv[1], &endptr, 0);
		//cout << "n "<< n << endl;

		//cout << "*endptr" << *endptr << endl;
		if (n < 0 || *endptr != '\0') {
			cout << "Exiting..." << endl;
			return 1;
		}
	}
	LogProgReader* reader = new LogProgReader;
	reader->setNumber(n);

	Timer timer;
	timer.start();

	int bad = reader->read(fName);
	if (bad){
		cerr<< "Error in input" << endl;
		return 1;
	}
	reader->cleanProg();
	reader->constructProgInSmodels();

	FILE* stream;
	fpos_t pos;
	int    fd;
	//redirectin stdout to a file
	//char *out_prefix_final = strcat(output_prefix,reducedLPPOD);
	//char *finalname = strcat(out_prefix_final,".prelparse");
	fgetpos(stdout, &pos);
	fd = dup(fileno(stdout));

	  if((stream = freopen(output_file, "w", stdout)) == NULL)
	      exit(-1);

	reader->writePosSmodels();

	timer.stop();
	cout<<"Total duration : "<<timer.print()<<endl;

	//Close file and restore standard output to stdout - which should be the terminal
	fflush(stdout);
	dup2(fd, fileno(stdout));
	close(fd);
	clearerr(stdout);
	fsetpos(stdout, &pos);

	delete reader;

	return 0;
}


