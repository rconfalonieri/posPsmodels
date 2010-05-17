/*
 * possibilisticdegree.cc
 *
 *  Created on: 11/apr/2010
 *      Author: confalonieri
 */

#include "possibilisticdegree.h"


PossibilisticDegree::PossibilisticDegree() {

	this->ruleDegree = new map<int,int>();
	this->rulePossValue = new map<int,int>();
	this->ruleText = new map<int,string>();
	this->degreeSum = 0;
	this->ruleIdMinDegree = new map<int,int>();
	this->maxPossValue = 0;


}


PossibilisticDegree::~PossibilisticDegree() {

}
