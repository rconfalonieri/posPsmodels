/*
 * lppodreduction.h
 *
 *  Created on: 19/gen/2010
 *      Author: confalonieri
 */

#ifndef LPPODREDUCTION_H_
#define LPPODREDUCTION_H_

#include <list>
#include <iostream>
#include <algorithm>
#include "lppodrule.h"
#include "lpodmodel.h"
#include "parsing.h"

LppodRule *reduceNormalLppodRule(LppodRule *lppodr,LpodModel *lpodm);
LppodRule *reduceConstraintLppodRule(LppodRule *lppodr,LpodModel *lpodm);
LppodRule *reduceOrderedDisjunctionLppodRule(LppodRule *lppodr,LpodModel *lpodm);
list<LppodProgram> *reduceLppod(list<LpodModel> *modelList,LppodProgram *p);


#endif /* LPPODREDUCTION_H_ */
