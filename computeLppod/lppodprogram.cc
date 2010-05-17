/*
 * program.cc
 *
 *  Created on: 15/gen/2010
 *      Author: confalonieri
 */

#include "lppodprogram.h"

LppodProgram::LppodProgram() {

	ruleNumber = 0;
	ruleList = new list<LppodRule>();
}

LppodProgram::~LppodProgram() {
	ruleNumber = 0;
	delete ruleList;

}

void
LppodProgram::printRuleList() {

	cout << "----------------------------------------------------------------------------------------------------------------------------------------" << endl;
	cout << "Printing ruleList of program" << endl;
	cout << "----------------------------------" << endl;
	list<LppodRule>::iterator pos;
	for (pos=this->ruleList->begin(); pos!=this->ruleList->end(); pos++) {
		cout << "Rule type " << pos->getRuleType() << endl;
		cout << "Original rule" << pos->getRule() << endl;
		cout << "Possibilistic value " << pos->getPossibilisticValue() << endl;
		cout << "IsDeleted " << pos->getIsDeleted() << endl;
		pos->printHeadAtoms();
		pos->printPosBody();
		pos->printNegBody();
	}

	cout << "----------------------------------------------------------------------------------------------------------------------------------------" << endl;


}
