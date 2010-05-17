/*
 * rule.h
 *
 *  Created on: 15/gen/2010
 *      Author: confalonieri
 */

#ifndef RULE_H_
#define RULE_H_

#include <string>
#include <list>
#include <iostream>
#include <functional>


using namespace std;

enum RuleType {
	UNDEFINED,
	EMPTY,
	COMMENT,
	FACT,
	NORMAL,
	CONSTRAINT,
	DISJUNCTION,
};


class LppodRule {

private:


	string possibilisticValue;
	bool isDeleted;
	list<string> *head;
	list<string> *posBody;
	list<string> *negBody;
	RuleType ruleType;
	//for debug
	string rule,reducedRule,ruleHead,ruleBody;
	string transformedRule;
	int ruleId;

public:


	LppodRule();
	~LppodRule();

    string getTransformedRule() const
    {
        return transformedRule;
    }

    void setTransformedRule(string transformedRule)
    {
        this->transformedRule = transformedRule;
    }

    int getRuleId() const
    {
        return ruleId;
    }

    void setRuleId(int ruleId)
    {
        this->ruleId = ruleId;
    }

    bool getIsDeleted() const
	{
		return isDeleted;
	}

	void setIsDeleted(bool isDeleted)
	{
		this->isDeleted = isDeleted;
	}

	list<string> *getHead() const
					{
		return head;
					}

	list<string> *getNegBody() const
					{
		return negBody;
					}

	list<string> *getPosBody() const
					{
		return posBody;
					}

	string getPossibilisticValue() const
	{
		return possibilisticValue;
	}

	string getRule() const
	{
		return rule;
	}

	string getReducedRule() const
	{
		return reducedRule;
	}

	RuleType getRuleType() const
	{
		return ruleType;
	}

	void setHead(list<string> *head)
	{
		this->head = head;
	}

	void setNegBody(list<string> *negBody)
	{
		this->negBody = negBody;
	}

	void setPosBody(list<string> *posBody)
	{
		this->posBody = posBody;
	}

	void setPossibilisticValue(string possibilisticValue)
	{
		this->possibilisticValue = possibilisticValue;
	}

	void setRule(string rule)
	{
		this->rule = rule;
	}

	void setReducedRule(string reducedRule)
	{
		this->reducedRule = reducedRule;
	}

	void setRuleType(RuleType ruleType)
	{
		this->ruleType = ruleType;
	}

	void printHeadAtoms() {

		cout << "----------------------------------" << endl;
		if (this->head->size()!=0) {

			list<string>::iterator pos;
			for (pos=this->head->begin(); pos!=this->head->end(); pos++) {
				cout << "Atom in head : " << *pos << endl;
			}
		}
		else
			cout << "------------Empty Head -----------" << endl;
		cout << "----------------------------------" << endl;
	}

	void printPosBody() {
		cout << "----------------------------------" << endl;
		if (this->posBody->size()!=0) {
			list<string>::iterator pos;
			for (pos=this->posBody->begin(); pos!=this->posBody->end(); pos++) {
				cout << "Atom in pos body : " << *pos << endl;
			}
		}
		else
			cout << "------------Empty Pos Body -----------" << endl;
		cout << "----------------------------------" << endl;
	}

	void printNegBody() {
		cout << "----------------------------------" << endl;
		if (this->negBody->size()!=0) {
			list<string>::iterator pos;
			for (pos=this->negBody->begin(); pos!=this->negBody->end(); pos++) {
				cout << "Atom in neg body : " << *pos << endl;
			}
		}
		else
			cout << "------------Empty Neg Body -----------" << endl;
		cout << "----------------------------------" << endl;
	}

};

#endif /* RULE_H_ */
