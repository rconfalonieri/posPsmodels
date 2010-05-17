/*
 * possprefrelation.cc
 *
 *  Created on: 11/apr/2010
 *      Author: confalonieri
 */

#include "possprefrelation.h"


void getOrdDisjunctionRuleValue(int ruleId, LppodProgram *p, int& possV, string& rule) {

	list<LppodRule>::iterator ruleIt;
	int ordDisjRuleScanned = 1;
	for (ruleIt=p->getRuleList()->begin();ruleIt!=p->getRuleList()->end();ruleIt++) {

		if (ruleIt->getRuleType()==DISJUNCTION && !ruleIt->getIsDeleted()) {
			if (ordDisjRuleScanned==ruleId) {
				possV = Utils::string2Int(ruleIt->getPossibilisticValue());
				rule = ruleIt->getTransformedRule();
			}
			ordDisjRuleScanned++;
		}
	}
}



void generatePossibilisticDegrees(list<LppodModel> *lppodModelL, LppodProgram *transformedLppod) {

	list<LppodModel>::iterator lppodModIt;

	//PossibilisticDegree *pD;

	list<string>::iterator satIt;

	for (lppodModIt=lppodModelL->begin();lppodModIt!=lppodModelL->end();lppodModIt++) {
		PossibilisticDegree *possDeg = new PossibilisticDegree();
		for (satIt=lppodModIt->getSatisfaction()->begin();satIt!=lppodModIt->getSatisfaction()->end();satIt++) {

			string sat = (*satIt);
			int ruleId = Utils::string2Int(sat.substr(5,1));
			int degree = Utils::string2Int(sat.substr(7,1));
			int possValue;
			string rule;
			getOrdDisjunctionRuleValue(ruleId,transformedLppod,possValue,rule);
			//cout << "ruleId " << ruleId << "| degree " << degree << "| possValue " << possValue << "| Text " << rule << endl;
			possDeg->getRuleDegree()->insert(pair<int, int>(ruleId,degree));
			possDeg->getRulePossValue()->insert(pair<int, int>(ruleId,possValue));
			possDeg->getRuleText()->insert(pair<int, string>(ruleId,rule));
			possDeg->setDegreeSum(possDeg->getDegreeSum()+degree);

			if (possDeg->getRuleIdMinDegree()->size()>0) {
				map<int,int>::iterator it = possDeg->getRuleIdMinDegree()->begin();
				int rId = it->first;

				int ruleD = (possDeg->getRuleDegree()->find(rId))->second;
				if (degree<ruleD) {
					possDeg->getRuleIdMinDegree()->clear();
					possDeg->getRuleIdMinDegree()->insert(pair<int, int>(ruleId,possValue));
					possDeg->setMaxPossValue(possValue);
				}
				if (degree=ruleD) {
					if (possValue<it->second) {
						possDeg->getRuleIdMinDegree()->clear();
						possDeg->getRuleIdMinDegree()->insert(pair<int, int>(ruleId,possValue));
						possDeg->setMaxPossValue(possValue);
					}
				}
			}
			else
				possDeg->getRuleIdMinDegree()->insert(pair<int, int>(ruleId,possValue));
			//	possDegL->push_back(*pD);

		}
		lppodModIt->setPossDegree(possDeg);
	}
}

LppodModel *getLppodModelbyId(list<LppodModel> *lppodModelL, int modelId) {

	list<LppodModel>::iterator lppodMIt;

	for (lppodMIt=lppodModelL->begin();lppodMIt!=lppodModelL->end();lppodMIt++) {
		if (lppodMIt->getModel_id()==modelId) {
			return &(*lppodMIt);
		}
	}


}

void sortPossibilisticModels(list<LppodModel> *lppodModelL, list<LppodModel> *sortedPossModelL) {

	list<LppodModel>::iterator lppodMIt;
	PossibilisticDegree *pD;
	list<int> *orderedRules = new list<int>();
	list<LppodModel> *tmpSortedPossModelL = new list<LppodModel>();
	list<int>::iterator it;
	multimap<int,int> degreeSet;
	multimap<int,int>::iterator degreeSetIt;

	//printPossibilisticModels(lppodModelL);
	//lppodModelL->
	for (lppodMIt=lppodModelL->begin();lppodMIt!=lppodModelL->end();lppodMIt++) {
		pD = lppodMIt->getPossDegree();
		degreeSet.insert(pair<int, int>(pD->getDegreeSum(),lppodMIt->getModel_id()));
	}
	for (degreeSetIt=degreeSet.begin();degreeSetIt!=degreeSet.end();degreeSetIt++) {
		//cout << "degree " << (*degreeSetIt).first << " Model(s) " << (*degreeSetIt).second << endl;
		(*degreeSetIt).first;
		(*degreeSetIt).second;
		//tmpSortedPossModelL->push_back(*(getLppodModelbyId(lppodModelL,(*degreeSetIt).second)));
		sortedPossModelL->push_back(*(getLppodModelbyId(lppodModelL,(*degreeSetIt).second)));

	}
	/*for (lppodMIt=tmpSortedPossModelL->begin();lppodMIt!=tmpSortedPossModelL->end();lppodMIt++) {

		if (orderedRules->size()>0) {
			for (it=orderedRules->begin();it!=orderedRules->end();it++) {
				LppodModel *m = getLppodModelbyId(lppodModelL,(*it));
				if (lppodMIt->getPossDegree()->getDegreeSum()==(m->getPossDegree()->getDegreeSum())) {
					if (lppodMIt->getPossDegree()->getMaxPossValue()<(m->getPossDegree()->getMaxPossValue())) {

						orderedRules->push_front(lppodMIt->getModel_id());
					}
					else
						orderedRules->push_back(lppodMIt->getModel_id());
				}
				else
					orderedRules->push_back(lppodMIt->getModel_id());
			}

		}
		else
			orderedRules->push_back(lppodMIt->getModel_id());
	}
	for (it=orderedRules->begin();it!=orderedRules->end();it++) {
		sortedPossModelL->push_back(*(getLppodModelbyId(lppodModelL,(*it))));
	}*/
}


void possibilisticPreferenceRelation(list<LppodModel> *lppodModelL, LppodProgram *transformedLppod, list<LppodModel> *sortedPossModelL) {

	generatePossibilisticDegrees(lppodModelL,transformedLppod);
	sortPossibilisticModels(lppodModelL,sortedPossModelL);

}
