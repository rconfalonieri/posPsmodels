//============================================================================
// Name        : main.cc
// Author      : confalonieri
// Version     :
// // Copyright (C) 2009-2010 Roberto Confalonieri
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
// confalonieri@lsi.upc.edu
//
// Description : A possibilistic solver for LPPODs
//============================================================================

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <list>
#include <algorithm>
#include <dlfcn.h>


using namespace std;

#include "../psmodelslib/psmodels.h"
#include "../lparselib/src/lparse.h"
//#include "../lparse2lib/src/lparse2.h"
#include "../possmodelslib/SRC/possmodels.h"
#include "../possmodelslib/SRC/preproclparse.h"
#include "preprocLPPODparse.h"
#include "computeLPPOD.h"
#include "utils.h"
#include "parsing.h"
#include "lppodprogram.h"
#include "lpodmodel.h"
#include "lppodreduction.h"



//TODO still-list:
/*
 * 1) parsing from lparse (delayed)
 * 2) transformation rules (the success specially)
 * 3) possibilistic preference relation
 * 3a) choose criteria of preference relation (for psmodel)
 * 4) bus error on loading liblparse2lib at runtime.....
 * 5) Java Wrapper, how?

 * 6) optimize and re-organize the code (modularity, free unused memory etc)
 *
 * DONE:
 * 1) if the input program is not LPOD does not work right now
 * 5) +/- fix the other packages to be able to use them as libraries (lparse, psmodels)
 * 4) find where stable models of psmodels are stored
 */



/*
 *
 * @inputs: an LPPOD P
 * @output: a set of possibilistic answer sets
 * algorithm:
 * 1. compute P* (parsing the possibilistic values)
 * 2. compute answer sets of P* (call psmodels)
 * 3. reduce P based on each of the M* found (apply the possibilistic reductions)
 * 4. compute possibilistic answer sets (call possmodels)
 *
 */

#define LPARSE_PAR 4

int
main(int argc, char* argv[]) {
	//int main() {
	int models = 1;
	char *fname = 0;
	char *lparse_par[4];
	char *lparse2_par[3];
	char *psmodel_par[3];
	char *possmodel_par[3];
	char *program_name;
	list <LpodModel> *modelList = new list<LpodModel>();



	for (int i = 1; i < argc; i++) {
		if (isdigit(*argv[i])) {
			models = strtol(argv[i], 0, 10);
		} else {
			fname = argv[i];
		}
	}
	//fname = "lppod/diagnosis.lppod";
	if (!fname) {
		cerr << "usage: posPsmodels input_file" << endl;
		return (EXIT_FAILURE);
	}

	//generate P*
	program_name = possibilistic_projection(fname);

	//program_name = "diagnosis";
	//char * program_name2 = "diagnosis";
	if (program_name == NULL) {
		cout << "\nPossibilistic projection failed....\n";
		return (EXIT_FAILURE);
	}

	LppodProgram *p = new LppodProgram;
	parseLppod(fname,p);

	//transformLppod(p);

	list<string> *possibilisticModels = new list<string>();
	string lparse_command;
	int reduced_programs_nr = 1;

	if (p->isOrderedDisjunctionProgram()) {


		//When it was a library...
		cout << "Program contains x" << endl;
		lparse_par[0] = "3";
		lparse_par[1] = "--true-negation";
		lparse_par[2] = "--priorities";
		lparse_par[3] = Utils::string2char("lpod/"+Utils::char2string(program_name)+".lpod");
		char *output_file = Utils::string2char("tmp/"+Utils::char2string(program_name)+".lparse");

		lparse(LPARSE_PAR,lparse_par,output_file);


		//lparse_command = "../lparselib/src/./lparse --true-negation --priorities lpod/"+Utils::char2string(program_name)+".lpod >tmp/"+Utils::char2string(program_name)+".lparse";
		//system(lparse_command.c_str());

		char *model_file = Utils::string2char("lpodmodels/"+Utils::char2string(program_name)+".models");
		//string psmodel_command = "./psmodels 0 tmp/"+Utils::char2string(program_name)+".lparse > "+Utils::char2string(model_file);

		psmodel_par[0] = "2";
		psmodel_par[1] = "0";
		psmodel_par[2] = Utils::string2char("tmp/"+Utils::char2string(program_name)+".lparse");
		psmodels(3,psmodel_par,model_file);
		//system(psmodel_command.c_str());


		//parse psmodel output and save models and satisfaction degrees
		parsePsmodelsNew(model_file,modelList);

		//for debug
		//printListElement(modelList);

		//reduce P* according to the M_i found by psmodels
		list<LppodProgram> *reduced_lppods = reduceLppod(modelList,p);
		reduced_programs_nr = generateReducedLppods(reduced_lppods,Utils::char2string(program_name));

	}
	string reducedLPPOD;


	for(int i=1; i<=reduced_programs_nr; i++) {

		string preProcInput1;
		stringstream out;
		out << i;
		string nr = out.str();
		if (p->isOrderedDisjunctionProgram()) {
			reducedLPPOD = Utils::char2string(program_name)+".red"+nr;
			preProcInput1 = "reducedlppod/"+reducedLPPOD;
		}
		else {
			reducedLPPOD = Utils::char2string(program_name)+".lppod";
			preProcInput1 = "lppod/"+reducedLPPOD;
		}



		string preProcInput2 = "tmp/"+reducedLPPOD+".prelparse";
		//cout << "preprocLparse inputs: " << preProcInput1 << "" << preProcInput2 << endl;
		//fflush(stdout);
		preproclparse(Utils::string2char(preProcInput1),Utils::string2char(preProcInput2));
		//fflush(stdout);

		//cout << "preprocLparse inputs: " << preProcInput1 << " " << preProcInput2 << endl;
		lparse_command = "../lparselib/src/./lparse --true-negation tmp/"+reducedLPPOD+".prelparse > tmp/"+reducedLPPOD+".lparse";

		//trying to load the library manually
		/*void *my_lib_handle = dlopen("liblparse2lib.dylib",RTLD_NOW);
		if(my_lib_handle==NULL) {
			cout << "Error loading the library" << endl;
			return (EXIT_FAILURE);
		}
		char *errstr = dlerror();
		if (errstr != NULL)
			cout <<"A dynamic linking error in loading the library occurred: " << errstr << endl;

		int (*fptr)(int,char**,char*);
		fptr ==  dlsym(my_lib_handle,"lparse2");
		if(fptr==NULL) {
			cout << "Error opening the lparse2 function of the library" << endl;
			return (EXIT_FAILURE);
		}
		errstr = dlerror();
		if (errstr != NULL)
			cout <<"A dynamic linking error occurred: " << errstr << endl;*/

		lparse2_par[0] = "2";
		lparse2_par[1] = "--true-negation";
		lparse2_par[2] = Utils::string2char("tmp/"+reducedLPPOD+".prelparse");
		//cout << "before call" << endl;
		//(*fptr)(3,lparse2_par,Utils::string2char("tmp/"+reducedLPPOD+".lparse"));
		//cout << "after call" << endl;
		//	lparse2(3,lparse2_par,Utils::string2char("tmp/"+reducedLPPOD+".lparse"));
		system(lparse_command.c_str());

		//trying to unload the library manually
		//dlclose(my_lib_handle);

		//cout << "before possmodels" << endl;
		//compute possibilistic answer sets using possmodels
		possmodel_par[0] = "2";
		possmodel_par[1] = "0";
		possmodel_par[2] = Utils::string2char("tmp/"+reducedLPPOD+".lparse");

		string output_file = "lpodmodels/"+reducedLPPOD+".possmodels";
		possmodels(2,possmodel_par,Utils::string2char(output_file));
		generatePossibilisticModel(possibilisticModels,output_file);


	}

	printPossibilisticModels(modelList,possibilisticModels);

	return (EXIT_SUCCESS);
}



