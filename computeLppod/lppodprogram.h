/*
 * program.h
 *
 *  Created on: 15/gen/2010
 *      Author: confalonieri
 */

#ifndef LPPODPROGRAM_H_
#define LPPODPROGRAM_H_

#include <iostream>
#include <list>
#include "lppodrule.h"


class LppodRule;

class LppodProgram {


private:

	list<LppodRule> *ruleList;
	int ruleNumber;
	bool hasOrderedDisjunctionRules;


public:



	LppodProgram();
	~LppodProgram();
	void printRuleList();


	bool isOrderedDisjunctionProgram() const
	{

		return hasOrderedDisjunctionRules;
	}

	void isOrderedDisjunctionProgram(bool value)
	{

		this->hasOrderedDisjunctionRules = value;
	}

    list<LppodRule> *getRuleList() const
    {
        return ruleList;
    }

    int getRuleNumber() const
    {
        return ruleNumber;
    }

    void setRuleList(list<LppodRule> *ruleList)
    {
        this->ruleList = ruleList;
    }

    void setRuleNumber(int ruleNumber)
    {
        this->ruleNumber = ruleNumber;
    }

};

#endif /* LPPODPROGRAM_H_ */
