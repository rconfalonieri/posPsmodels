/*
 * lppodmodel.h
 *
 *  Created on: 10/mag/2010
 *      Author: confalonieri
 */

#ifndef LPPODMODEL_H_
#define LPPODMODEL_H_



#include "lpodmodel.h"
#include "possibilisticatom.h"
#include "possibilisticdegree.h"

using namespace std;

class PossibilisticAtom;
class PossibilisticDegree;

class LppodModel: public LpodModel {

private:
	string possibilisticValue;
	list<PossibilisticAtom> *possAtomList;
	//list<PossibilisticDegree> *possDegreeL;
	PossibilisticDegree *possDegree;


public:
    list<PossibilisticAtom> *getPossAtomList() const
    {
        return possAtomList;
    }

    PossibilisticDegree *getPossDegree() const
    {
        return possDegree;
    }

    string getPossibilisticValue() const
    {
        return possibilisticValue;
    }

    void setPossAtomList(list<PossibilisticAtom> *possAtomList)
    {
        this->possAtomList = possAtomList;
    }

    void setPossDegree(PossibilisticDegree *possDegree)
    {
        this->possDegree = possDegree;
    }

    void setPossibilisticValue(string possibilisticValue)
    {
        this->possibilisticValue = possibilisticValue;
    }

};

#endif /* LPPODMODEL_H_ */
