/*
 * possprefrelation.h
 *
 *  Created on: 11/apr/2010
 *      Author: confalonieri
 */

#ifndef POSSPREFRELATION_H_
#define POSSPREFRELATION_H_

#include "lppodmodel.h"
#include "lppodprogram.h"
#include "utils.h"
#include "possibilisticdegree.h"
#include "parsing.h"
#include <map>

#include <set>


void possibilisticPreferenceRelation(list<LppodModel> *lppodModelL, LppodProgram *transformedLppod, list<LppodModel> *sortedPossModelL);



#endif /* POSSPREFRELATION_H_ */
