/*
 * possibilisticdegree.h
 *
 *  Created on: 11/apr/2010
 *      Author: confalonieri
 */

#ifndef POSSIBILISTICDEGREE_H_
#define POSSIBILISTICDEGREE_H_

#include <string>
#include <map>

using namespace std;

class PossibilisticDegree {

private:

	map<int,int> *ruleDegree;
	map<int,int> *rulePossValue;
	map<int,string> *ruleText;
	int degreeSum;
	map<int,int> *ruleIdMinDegree;
	int maxPossValue;
	//int ruleId;
	//int degree;
	//int possRuleValue;
	//string rule;

public:

	PossibilisticDegree();
	~PossibilisticDegree();
    int getMaxPossValue() const
    {
        return maxPossValue;
    }

    void setMaxPossValue(int maxPossValue)
    {
        this->maxPossValue = maxPossValue;
    }

    map<int,int> *getRuleIdMinDegree() const
    {
        return ruleIdMinDegree;
    }

    void setRuleIdMinDegree(map<int,int> *ruleIdMinDegree)
    {
        this->ruleIdMinDegree = ruleIdMinDegree;
    }

    int getDegreeSum() const
    {
        return degreeSum;
    }

    void setDegreeSum(int degreeSum)
    {
        this->degreeSum = degreeSum;
    }

    map<int,int> *getRuleDegree() const
    {
        return ruleDegree;
    }

    map<int,int> *getRulePossValue() const
    {
        return rulePossValue;
    }

    map<int,string> *getRuleText() const
    {
        return ruleText;
    }

    void setRuleDegree(map<int,int> *ruleDegree)
    {
        this->ruleDegree = ruleDegree;
    }

    void setRulePossValue(map<int,int> *rulePossValue)
    {
        this->rulePossValue = rulePossValue;
    }

    void setRuleText(map<int,string> *ruleText)
    {
        this->ruleText = ruleText;
    }

};


#endif /* POSSIBILISTICDEGREE_H_ */
