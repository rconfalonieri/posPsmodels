/*
 * possibilisticatom.h
 *
 *  Created on: 10/apr/2010
 *      Author: confalonieri
 */

#ifndef POSSIBILISTICATOM_H_
#define POSSIBILISTICATOM_H_

#include <string>

using namespace std;

class PossibilisticAtom {

private:


	string possibilisticValue;
	string atom;

public:


	PossibilisticAtom();
	~PossibilisticAtom();

	string getAtom() const
	{
		return atom;
	}

	string getPossibilisticValue() const
	{
		return possibilisticValue;
	}

	void setAtom(string atom)
	{
		this->atom = atom;
	}

	void setPossibilisticValue(string possibilisticValue)
	{
		this->possibilisticValue = possibilisticValue;
	}

};

#endif /* POSSIBILISTICATOM_H_ */
