/*
 * lppodreduction.cc
 *
 *  Created on: 19/gen/2010
 *      Author: confalonieri
 */

#include "lppodreduction.h"

bool
checkNegativeBody(list<string> *negBody, list<string> *atoms) {

	list<string>::iterator atom_model;
	list<string>::iterator atom_neg_body;
	bool found = false;

	if (REDUCTION_DEBUG) {
		cout << "******************************************" << endl;
		cout << "*************checkNegativeBody************"<< endl;
		cout << "******************************************" << endl;
	}
	if (negBody->size()>0) {

		for (atom_model=atoms->begin(); atom_model!=atoms->end() && !found; atom_model++) {
			if (REDUCTION_DEBUG) {
				cout << "-------------------------" << endl;
				cout << "Looking for :" << *atom_model << endl;
				cout << "-------------------------" << endl;
			}
			//TODO: patch for handling predicates

			atom_neg_body = find(negBody->begin(), negBody->end(), *atom_model);
			if (atom_neg_body != negBody->end()) {
				found = true;
				if (REDUCTION_DEBUG) {
					cout << "Founded! Negated body contains :" << *atom_neg_body << endl;
					cout << "This rule will be deleted " << endl;
				}
			}
		}
	}
	return found;
}


bool checkHead(list<string> *head, list<string> *atoms) {

	list<string>::iterator atom_model;
	string atom_head;
	bool found = false;

	if (REDUCTION_DEBUG) {
		cout << "******************************************" << endl;
		cout << "******checkHead********"<< endl;
		cout << "******************************************" << endl;
	}

	if (head->size()>1) {
		cout << "*************************************" << endl;
		cout << "**FATAL ERROR: normal rule head > 1**" << endl;
		cout << "*************************************" << endl;
		exit(1);
	}
	else {
		atom_head = head->front();
		atom_model = find(atoms->begin(), atoms->end(), atom_head);
		if (atom_model != atoms->end()) {
			found = true;
			if (REDUCTION_DEBUG) {
				cout << "Head " << *atom_head.begin() << "founded in model  :" << *atom_model << endl;
				cout << "This rule will not be deleted " << endl;
			}
		}

	}
	return found;
}


list<string> *removeFromList(list<string> *l, string el) {


	list<string> *lcopy = new list<string>();
	if (l->size()>0) {
		list<string>::iterator rem_it;
		for (rem_it=l->begin(); rem_it!=l->end(); rem_it++)
			if (el.compare(*rem_it) != 0)
				lcopy->push_back(*rem_it);
	}
	return lcopy;

}

list<string> *copyList(list<string> *listToCopy) {

	list<string> *copy = new list<string>();
	if (listToCopy->size()>0) {
		list<string>::iterator copy_it;
		for (copy_it=listToCopy->begin(); copy_it!=listToCopy->end(); copy_it++)
			copy->push_back(*copy_it);
	}
	return copy;
}

void
printList4Debug(list<string> *listToPrint) {

	if (REDUCTION_DEBUG) {

		list<string>::iterator print_it;
		cout << "Atoms: " ;
		for (print_it=listToPrint->begin(); print_it!=listToPrint->end(); print_it++)
			cout << *print_it;
		cout << endl;
	}
}


void
reduceNegativeBody(list<string> *negBody, list<string> *atoms, LppodRule &rule) {

	list<string>::iterator atom_model;
	list<string>::iterator atom_neg_body;
	//cout << "*******in reduceNegativeBody***********" << endl;
	//bool found = false;
	if (negBody->size()>0) {
		cout << "********in for***********" << endl;
		list<string> *negBodyCopy = copyList(negBody);
		//list<string>::iterator negBodyCopyIt;
		//copy ( negBody->begin(), negBody->end(), negBodyCopy.begin() );

		if (REDUCTION_DEBUG) {
			cout << "******************************************" << endl;
			cout << "***********reduceNegativeBody*************"<< endl;
			cout << "******************************************" << endl;
		}
		for (atom_neg_body=negBody->begin(); atom_neg_body!=negBody->end(); atom_neg_body++) {
			if (VERBOSE_DEBUG) {
				cout << "-------------------------" << endl;
				cout << "Looking for :" << *atom_neg_body << endl;
				cout << "-------------------------" << endl;
			}

			atom_model = find(atoms->begin(), atoms->end(), *atom_neg_body);
			if (atom_model == atoms->end()) {
				//found = true;
				if (REDUCTION_DEBUG) {
					cout << "Not Founded! Model does not contains :" << *atom_neg_body << endl;
					cout << "The atom will be removed from negative body" << endl;

					printList4Debug(negBodyCopy);
				}
				//this does not seem to work,...
				//remove(negBodyCopy->begin(), negBodyCopy->end(),*atom_neg_body);
				negBodyCopy  = removeFromList(negBodyCopy,*atom_neg_body);
			}
		}
		rule.setNegBody(negBodyCopy);

	}
	//return found
}

bool
reduceOrderedDisjunctionHead(list<string> *head, list<string> *atoms, LppodRule &disjunctionRule) {

	list<string>::iterator atom_model;
	list<string>::iterator atom_head;
	bool found = false;
	if (head->size()==1) {
		cout << "******************************************" << endl;
		cout << "**FATAL ERROR: disjunction rule head = 1**" << endl;
		cout << "******************************************" << endl;
		exit(1);
	}
	else {
		if (REDUCTION_DEBUG) {
			cout << "******************************************" << endl;
			cout << "******reduceOrderedDisjunctionHead********"<< endl;
			cout << "******************************************" << endl;
		}
		for (atom_head=head->begin(); atom_head!=head->end() && !found; atom_head++) {
			if (VERBOSE_DEBUG) {
				cout << "-------------------------" << endl;
				cout << "Looking for :" << *atom_head << endl;
				cout << "-------------------------" << endl;
			}
			//TODO: patch for handling predicates

			atom_model = find(atoms->begin(), atoms->end(), *atom_head);
			if (atom_model != atoms->end()) {
				found = true;
				if (REDUCTION_DEBUG) {
					cout << "Founded! Model contains :" << *atom_model << endl;
					cout << "The rest of the head will be deleted " << endl;
				}
			}
		}
		if (found) {
			list<string> *newHead = new list<string>();
			newHead->push_back(*atom_model);
			disjunctionRule.setHead(newHead);
		}
		else {
			cout << "***************************************************" << endl;
			cout << "**FATAL ERROR: disjunction rule reduction failed **" << endl;
			cout << "***************************************************" << endl;
			exit(1);
		}
		return found;

	}

}

LppodRule *reduceNormalLppodRule(LppodRule *lppodr,LpodModel *lpodm) {

	LppodRule *redNormalRule = new LppodRule;
	redNormalRule->setRuleType(lppodr->getRuleType());
	redNormalRule->setPossibilisticValue(lppodr->getPossibilisticValue());
	redNormalRule->setRule(lppodr->getRule());
	redNormalRule->setPosBody(lppodr->getPosBody());
	string possValue;
	string redRule;

	if (REDUCTION_DEBUG) {
		cout << "***************************************************" << endl;
		cout << "************* REDUCENORMALLPPODRULE ***************" << endl;
		cout << "***************************************************" << endl;
	}

	if (REDUCTION_DEBUG) {
		//cout << "-------------------------" << endl;
		lpodm->printAtoms();
		//cout << "-------------------------" << endl;

		//cout << "-------------------------" << endl;
		lppodr->printNegBody();
		//cout << "-------------------------" << endl;
	}

	//the rule will be deleted if in the negative body there is at least one negated literal present in the model
	if (checkNegativeBody(lppodr->getNegBody(),lpodm->getAtoms())) {
		possValue = "%% REDUCED "+lppodr->getPossibilisticValue();
		redRule = possValue+" "+lppodr->getRule();
		redNormalRule->setReducedRule(redRule);
		redNormalRule->setIsDeleted(true);
	}
	//the rule will be deleted if the body is positively evaluated but the atom in the head is not present in the model
	else {
		if (checkHead(lppodr->getHead(),lpodm->getAtoms())) {

			redNormalRule->setHead(lppodr->getHead());
			redNormalRule->setPosBody(lppodr->getPosBody());
			possValue = lppodr->getPossibilisticValue();
			redRule = possValue+" "+lppodr->getRule();
			redNormalRule->setReducedRule(redRule);
			reduceNegativeBody(lppodr->getNegBody(),lpodm->getAtoms(),*redNormalRule);
			printList4Debug(redNormalRule->getNegBody());
		}
		else {
			possValue = "%% REDUCED "+lppodr->getPossibilisticValue();
			redRule = possValue+" "+lppodr->getRule();
			redNormalRule->setReducedRule(redRule);
			redNormalRule->setIsDeleted(true);
		}
	}
	//list<string> *tmpHead = lppodr.getNegBody();
	//redNormalRule.setHead(reduceRuleHead(lpodm,lppodr));
	return redNormalRule;

}

LppodRule *reduceConstraintLppodRule(LppodRule *lppodr,LpodModel *lpodm) {

	if (REDUCTION_DEBUG) {
		cout << "***************************************************" << endl;
		cout << "********* reduceConstraintLppodRule ***************" << endl;
		cout << "***************************************************" << endl;
	}

	string possValue;
	string redRule;
	LppodRule *redConstraint = new LppodRule;
	redConstraint->setRuleType(lppodr->getRuleType());
	//redConstraint.setHead(new list<string>());
	//redConstraint.setPosBody(new list<string>());
	//redConstraint.setNegBody(new list<string>());
	redConstraint->setPossibilisticValue(lppodr->getPossibilisticValue());
	redConstraint->setRule(lppodr->getRule());
	//possValue = "%% REDUCED "+lppodr->getPossibilisticValue();
	redRule = "%% REDUCED "+lppodr->getRule();
	redConstraint->setReducedRule(redRule);
	redConstraint->setIsDeleted(true);

	return redConstraint;

}

LppodRule *reduceOrderedDisjunctionLppodRule(LppodRule *lppodr,LpodModel *lpodm) {

	LppodRule *redDisjunctionRule = new LppodRule;
	string possValue;
	string redRule;

	if (REDUCTION_DEBUG) {
		cout << "***************************************************" << endl;
		cout << "********* reduceOrderedDisjunctionLppodRule *******" << endl;
		cout << "***************************************************" << endl;
	}

	redDisjunctionRule->setRuleType(lppodr->getRuleType());
	redDisjunctionRule->setPossibilisticValue(lppodr->getPossibilisticValue());
	redDisjunctionRule->setRule(lppodr->getRule());
	redDisjunctionRule->setPosBody(lppodr->getPosBody());

	if (REDUCTION_DEBUG) {
		lpodm->printAtoms();
		lppodr->printNegBody();
	}

	//the rule will be deleted if in the negative body there is at least one negated literal present in the model
	if (checkNegativeBody(lppodr->getNegBody(),lpodm->getAtoms())) {
		//possValue = "%% REDUCED "+lppodr->getPossibilisticValue();
		//redRule = possValue+" "+lppodr->getRule();
		redDisjunctionRule->setReducedRule("%% REDUCED "+lppodr->getRule());
		redDisjunctionRule->setIsDeleted(true);
	}
	else {
		//reduce the ordered disjunction head
		if (reduceOrderedDisjunctionHead(lppodr->getHead(),lpodm->getAtoms(),*redDisjunctionRule)) {
			printList4Debug(redDisjunctionRule->getHead());
			//cout << "********before reduceNegativeBody***********" << endl;
			reduceNegativeBody(lppodr->getNegBody(),lpodm->getAtoms(),*redDisjunctionRule);
			//printList4Debug(redDisjunctionRule->getNegBody());
			//redDisjunctionRule->setReducedRule("REDUCED "+lppodr->getRule());
		}
	}

	//list<string> *tmpHead = lppodr.getNegBody();
	//redNormalRule.setHead(reduceRuleHead(lpodm,lppodr));


	return redDisjunctionRule;

}

/*
 * Implements the P_x^M reduction
 * Inputs: a list of Model of an LPOD
 * 		   an LPPOD
 * Output: the reduced LPPODs
 *
 * P_x^M = Union r_x^M
 * r_x^M = { alpha: c_i <-- B+ | c_i in M and M inters ({ c_1,...,c_{i-1}} Union B-) = 0 }
 *
 */
list<LppodProgram> *reduceLppod(list<LpodModel> *modelList,LppodProgram *p) {

	LppodProgram *reduced_lppod;
	list<LppodRule> *reduced_lppod_rules;
	list<LppodProgram> *reduced_lppods = new list<LppodProgram>();
	int rule_type;
	list<LpodModel>::iterator model_it;
	list<LppodRule>::iterator rule_it;
	string possValue;
	string redRule;
	if (DEBUG) {
		cout << "*****************************************" << endl;
		cout << "*********REDUCING LPPOD****************" << endl;
		cout << "*****************************************" << endl;
	}
	for (model_it=modelList->begin(); model_it!=modelList->end();model_it++) {

		reduced_lppod = new LppodProgram;
		reduced_lppod_rules = new list<LppodRule>();
		for (rule_it=p->getRuleList()->begin(); rule_it!=p->getRuleList()->end();rule_it++) {

			rule_type = rule_it->getRuleType();
			switch(rule_type) {

			case UNDEFINED:
			case EMPTY:
			case COMMENT:
			case FACT:
				if (REDUCTION_DEBUG) {
					cout << "--------------------------" << endl;
					cout << "Rule type to reduce " << rule_type << endl;
					cout << "Rule to reduce " << rule_it->getRule() << endl;
					cout << "--------------------------" << endl;
				}
				//possValue = rule_it->getPossibilisticValue();
				//redRule = possValue+" "+rule_it->getRule();
				rule_it->setReducedRule(rule_it->getRule());
				reduced_lppod_rules->push_back(*rule_it);
				if (REDUCTION_DEBUG) {
					cout << "--------------------------" << endl;
					cout << "Rule added, reduced_lppod_rules size :" << reduced_lppod_rules->size() << endl;
					cout << "--------------------------" << endl;
				}
				break;

			case NORMAL:
				if (REDUCTION_DEBUG) {
					cout << "--------------------------" << endl;
					cout << "Rule type to reduce " << rule_type << endl;
					cout << "Rule to reduce " << rule_it->getRule() << endl;
					cout << "--------------------------" << endl;
				}

				reduced_lppod_rules->push_back(*(reduceNormalLppodRule(&*rule_it,&*model_it)));
				if (REDUCTION_DEBUG) {
					cout << "--------------------------" << endl;
					cout << "Rule added, reduced_lppod_rules size :" << reduced_lppod_rules->size() << endl;
					cout << "--------------------------" << endl;
				}
				break;

			case CONSTRAINT:
				if (REDUCTION_DEBUG) {
					cout << "--------------------------" << endl;
					cout << "Rule type to reduce " << rule_type << endl;
					cout << "Rule to reduce " << rule_it->getRule() << endl;
					cout << "--------------------------" << endl;
				}

				reduced_lppod_rules->push_back(*reduceConstraintLppodRule(&*rule_it,&*model_it));
				if (REDUCTION_DEBUG) {
					cout << "--------------------------" << endl;
					cout << "Rule added, reduced_lppod_rules size :" << reduced_lppod_rules->size() << endl;
					cout << "--------------------------" << endl;
				}
				break;

			case DISJUNCTION:
				if (REDUCTION_DEBUG) {
					cout << "--------------------------" << endl;
					cout << "Rule type to reduce " << rule_type << endl;
					cout << "Rule to reduce " << rule_it->getRule() << endl;
					cout << "--------------------------" << endl;
				}

				reduced_lppod_rules->push_back(*reduceOrderedDisjunctionLppodRule(&*rule_it,&*model_it));
				if (REDUCTION_DEBUG) {
					cout << "--------------------------" << endl;
					cout << "Rule added, reduced_lppod_rules size :" << reduced_lppod_rules->size() << endl;
					cout << "--------------------------" << endl;
				}
				break;

			default:
				cout << "*******Unexpected rule to reduce*******" << endl;
				exit(1);
			}
			reduced_lppod->setRuleList(reduced_lppod_rules);
			//reduced_lppod->printRuleList();
		}
		if (REDUCTION_DEBUG) {
			cout << "--------------------------" << endl;
			cout << "reduced_lppods size " << reduced_lppods->size() << endl;
			cout << "--------------------------" << endl;
		}
		reduced_lppods->push_back(*reduced_lppod);
		if (REDUCTION_DEBUG) {
			cout << "--------------------------" << endl;
			cout << "reduced_lppods size " << reduced_lppods->size() << endl;
			cout << "--------------------------" << endl;
		}
	}
	//if (DEBUG) {
	if (modelList->size() != reduced_lppods->size()) {
		cout << "--------------------------" << endl;
		cout << "*******FATAL ERROR*******" << endl;
		cout << "--------------------------" << endl;
		exit(1);
	}
	//}
	return reduced_lppods;
}

