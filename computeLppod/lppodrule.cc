/*
 * rule.cc
 *
 *  Created on: 15/gen/2010
 *      Author: confalonieri
 */
#include "lppodrule.h"

LppodRule::LppodRule() {

	this->ruleType = UNDEFINED;
	this->isDeleted = false;
	this->head = new list<string>();
	this->posBody = new list<string>();
	this->negBody = new list<string>();


}

LppodRule::~LppodRule() {

	delete this->head;
	delete this->posBody;
	delete this->negBody;

}
