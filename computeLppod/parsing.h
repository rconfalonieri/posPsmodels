/*
 * parsing.h
 *
 *  Created on: 14/gen/2010
 *      Author: confalonieri
 */
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <list>

//#include "lpodmodel.h"
#include "lppodmodel.h"
#include "utils.h"
#include "lppodprogram.h"


#ifndef PARSING_H_
#define PARSING_H_

#define MOD_NONE	0
#define MOD_SMODELS	1
#define PARSING_DEBUG 0
#define PARSING_DEBUG_NEW 0
#define REDUCTION_DEBUG 0
#define TRANSFORMATIONS_DEBUG 0
#define GENERATE_DEBUG 0
#define DEBUG 0
#define UNPARSING_DEBUG 0
#define VERBOSE_DEBUG 0


int parsePsmodels(char *model_file,list <LpodModel> *modelList);
int parsePsmodelsNew(char *model_file,list <LpodModel> *modelList);
int parseLppodRule(vector<string> splittedLine, string line, LppodProgram *p, int ruleId);
int parseLppod(char* lppod, LppodProgram *p);
void updateLppodProgramHead(LppodProgram *p);
void updateLppodProgramFacts(LppodProgram *p);
int generateReducedLppods(list<LppodProgram> *lppods,string program_name);
void generateLppod(LppodProgram *lppod,string program_name);
void generatePossibilisticModels(list<LppodModel> *possModelList, string possmodel, LpodModel *lpodModel);
void printPossibilisticModels(list<LppodModel> *possModelList);



#endif /* PARSING_H_ */


