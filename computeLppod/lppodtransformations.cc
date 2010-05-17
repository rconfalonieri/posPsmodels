/*
 * lppodtransformations.cc
 *
 *  Created on: 23/gen/2010
 *      Author: confalonieri
 */

#include "lppodtransformations.h"


bool existContradiction(list<string> *posBody, list<string> *negBody) {

	bool result = false;
	list<string>::iterator posB;
	//list<string>::iterator negB;
	list<string>::iterator found;
	for (posB=posBody->begin();posB!=posBody->end();posB++) {
		found = find( negBody->begin(), negBody->end(), *posB );
		if (found!=negBody->end())
			result = true;
	}
	return result;
}

bool findAtomInAtomList(list<string> *programHead,string atom) {

	bool result = false;
	//list<string>::iterator headIt;
	list<string>::iterator found;
	for (found=programHead->begin();found!=programHead->end();found++) {
	//found = find( programHead->begin(), programHead->end(), atom );
	if ((*found)==atom)
		result = true;
	}
	return result;
}

string min(string val1, string val2) {

	string result = val1;
	int i = Utils::string2Int(val1);
	int j = Utils::string2Int(val2);
	if (i>j)
		result = val2;
	return result;
}


void removeAtomFromBody(int id, string atom, list<LppodRule> *ruleL, bool fromNegBody) {
	list<LppodRule>::iterator ruleIt;
	if (TRANSFORMATIONS_DEBUG) {
		cout << "***************************************" << endl;
		cout << "******removeAtomFromBody***************" << endl;
		//cout << "***************************************" << endl;
	}
	for (ruleIt=ruleL->begin();ruleIt!=ruleL->end();ruleIt++) {
		if (ruleIt->getRuleId()==id) {
			list<string>::iterator bodyL;
			list<string> *newBodyL = new list<string>();
			if (fromNegBody) {
				if (TRANSFORMATIONS_DEBUG) {
					//cout << "***************************************" << endl;
					cout << "******fromNegBody***************" << endl;
					cout << "Negative body before: " << endl;
					ruleIt->printNegBody();
				}
				for (bodyL=ruleIt->getNegBody()->begin();bodyL!=ruleIt->getNegBody()->end();bodyL++) {
					if (TRANSFORMATIONS_DEBUG) {

						cout << "bodyL " << *bodyL << endl;
						cout << "atom " << atom << endl;
						ruleIt->printNegBody();
					}
					if ((*bodyL)!=atom)
						newBodyL->push_back(*bodyL);
				}
				ruleIt->setNegBody(newBodyL);
				//bodyL = remove(ruleIt->getNegBody()->begin(),ruleIt->getNegBody()->end(),atom);
				if (TRANSFORMATIONS_DEBUG) {
					cout << "***************************************" << endl;
					//cout << "bodyL " << *bodyL << endl;
					cout << "******removeAtomFromBody***************" << endl;
					cout << "Negative body after: " << endl;
					ruleIt->printNegBody();
				}
			}
			else {
				if (TRANSFORMATIONS_DEBUG) {
					//cout << "***************************************" << endl;
					cout << "******fromPosBody***************" << endl;
					cout << "Positive body before: " << endl;
					ruleIt->printPosBody();
				}
				//bodyL = remove(ruleIt->getPosBody()->begin(),ruleIt->getPosBody()->end(),atom);
				for (bodyL=ruleIt->getPosBody()->begin();bodyL!=ruleIt->getPosBody()->end();bodyL++) {
					if (TRANSFORMATIONS_DEBUG) {

						cout << "bodyL " << *bodyL << endl;
						cout << "atom " << atom << endl;
						ruleIt->printNegBody();
					}
					if ((*bodyL)!=atom)
						newBodyL->push_back(*bodyL);
				}
				ruleIt->setPosBody(newBodyL);
				if (TRANSFORMATIONS_DEBUG) {
					cout << "***************************************" << endl;
					//cout << "bodyL " << *bodyL << endl;
					cout << "******removeAtomFromBody***************" << endl;
					cout << "Positive body after: " << endl;
					ruleIt->printPosBody();
				}
			}
		}
	}
	if (TRANSFORMATIONS_DEBUG) {
		cout << "***************************************" << endl;

	}
}

void updateRuleType(list<LppodRule> *ruleL) {
	list<LppodRule>::iterator ruleIt;
	for (ruleIt=ruleL->begin();ruleIt!=ruleL->end();ruleIt++) {
		if (!ruleIt->getIsDeleted() && ruleIt->getRuleType()==NORMAL) {
			if (ruleIt->getNegBody()->size()==0 && ruleIt->getPosBody()->size()==0)
				ruleIt->setRuleType(FACT);
		}
	}
}

/*
 * P \rightarrow_{PC} P', if $P$ contains:
 *  	- a rule $r = \alpha: {\cal C}^{\times} \leftarrow {\cal B}^+, \; not \; {\cal B}^-$ which has an atom $b$ such that,
 *  	- $b \in {\cal B}^+$ and $b \in {\cal B}^-$,
 *  	-> $P' = P \backslash \{r\}$
 */
void possibilisticContradiction(LppodProgram *p) {

	bool found = false;
	list<LppodRule>::iterator ruleIt;
	if (TRANSFORMATIONS_DEBUG) {
		cout << "***************************************" << endl;
		cout << "******possibilisticContradiction*******" << endl;
		//cout << "***************************************" << endl;
	}

	for (ruleIt=p->getRuleList()->begin(); ruleIt!=p->getRuleList()->end() && !found; ruleIt++) {
		if (!ruleIt->getIsDeleted()) {
			if (existContradiction(ruleIt->getPosBody(),ruleIt->getNegBody())) {
				if (TRANSFORMATIONS_DEBUG) {
					cout << "existContradiction in rule " << ruleIt->getRuleId() << endl;
					cout << "Rule " << ruleIt->getRule() << endl;
					cout << "***************************************" << endl;
				}
				found = true;
				ruleIt->setIsDeleted(found);
				updateLppodProgramHead(p);
			}
		}
	}

}


/*
 * P \rightarrow_{PRED^+} P', if
 * 		- there is a rule $r = \alpha: {\cal C}^{\times} \leftarrow {\cal B}^+, \; not \; ({\cal B}^- \cup \{b\} )$ in $P$ and,
 * 		- such that $b \notin HEAD(P)$, and
 * 		-> $P' = (P \backslash  \{r\}) \cup \{\alpha: {\cal C}^{\times} \leftarrow {\cal B}^+, \; not \; {\cal B}^- \}$
 */
void possibilisticPositiveReduction(LppodProgram *p,list<string> *programHead) {

	list<LppodRule>::iterator ruleIt;
	list<string> atomToRemove;
	multimap<int,string> rule2AtomToremove;
	if (TRANSFORMATIONS_DEBUG) {
		cout << "*******************************************" << endl;
		cout << "******possibilisticPositiveReduction*******" << endl;
		//cout << "***************************************" << endl;
	}
	for (ruleIt=p->getRuleList()->begin(); ruleIt!=p->getRuleList()->end(); ruleIt++) {
		if (!ruleIt->getIsDeleted()) {
			list<string>::iterator negBodyIt;
			for (negBodyIt=ruleIt->getNegBody()->begin(); negBodyIt!=ruleIt->getNegBody()->end(); negBodyIt++) {
				if (!findAtomInAtomList(programHead,*negBodyIt)) {
					if (TRANSFORMATIONS_DEBUG) {
						cout << "findAtomInAtomList " << *negBodyIt << " in rule " << ruleIt->getRuleId() << endl;
						cout << "Rule " << ruleIt->getRule() << endl;
						//cout << "***************************************" << endl;
					}
					rule2AtomToremove.insert(pair<int, string>(ruleIt->getRuleId(),*negBodyIt));
				}
			}
		}
	}
	if (rule2AtomToremove.size()>0) {
		if (TRANSFORMATIONS_DEBUG) {
			cout << "rule2AtomToremove.size()>0" << endl;

		}
		multimap<int,string>::iterator mIt;
		for (mIt=rule2AtomToremove.begin();mIt!=rule2AtomToremove.end();mIt++) {
			//cout << (*it).first << " => " << (*it).second << endl;
			//int id = (*mIt).first;
			removeAtomFromBody((*mIt).first,(*mIt).second,p->getRuleList(),true);
			updateRuleType(p->getRuleList());
		}
		if (TRANSFORMATIONS_DEBUG) {
			cout << "***************************************" << endl;

		}
		updateLppodProgramFacts(p);
		updateLppodProgramHead(p);
	}
}


/*
 * P \rightarrow_{PRED^-} P'$, if $P$ contains the rules:
 * 		- $r = \alpha: a \leftarrow \top $, and
 * 		- $r' = \beta: {\cal C}^{\times} \leftarrow {\cal B}^+, \; not \; ({\cal B}^- \cup \{a\} )$,
 * 		-> $P' = (P \backslash  \{r'\}) $
 */
void possibilisticNegativeReduction(LppodProgram *p,list<Fact> *programFacts) {

	list<LppodRule>::iterator ruleIt;
	if (TRANSFORMATIONS_DEBUG) {
		cout << "*******************************************" << endl;
		cout << "******possibilisticNegativeReduction*******" << endl;
		//cout << "***************************************" << endl;
	}
	for (ruleIt=p->getRuleList()->begin(); ruleIt!=p->getRuleList()->end(); ruleIt++) {
		if (!ruleIt->getIsDeleted()) {
			if (ruleIt->getRuleType()==NORMAL || ruleIt->getRuleType()==DISJUNCTION) {
				list<Fact>::iterator factsIt;
				for (factsIt=programFacts->begin(); factsIt!=programFacts->end(); factsIt++) {
					if (findAtomInAtomList(ruleIt->getNegBody(),factsIt->getAtom())) {
						if (TRANSFORMATIONS_DEBUG) {
							cout << "findAtomInAtomList " << factsIt->getAtom() << " in rule " << ruleIt->getRuleId() << endl;
							cout << "Rule " << ruleIt->getRule() << endl;
							cout << "***************************************" << endl;
						}
						ruleIt->setIsDeleted(true);
						updateLppodProgramHead(p);

					}
				}
			}
		}
	}
}


/*
 * P \rightarrow_{PS} P', if $P$ contains:
 * 		- a fact $ \alpha: a \leftarrow \top$ and
 * 		- a rule $r = \beta: {\cal C}^{\times} \leftarrow {\cal B}^+, \; not \; {\cal B}^-$ such that $a \in {\cal B}^+$,
 * 		-> $P' = (P \backslash  \{r\}) \cup \{GLB\{\alpha,\beta\}: {\cal C}^{\times} \leftarrow ({\cal B}^+\backslash\{a\}), \; not \; {\cal B}^- \}$
 */
void possibilisticSuccess(LppodProgram *p,list<Fact> *programFacts) {

	list<LppodRule>::iterator ruleIt;
	multimap<int,string> rule2AtomToremove;
	if (TRANSFORMATIONS_DEBUG) {
		cout << "*******************************************" << endl;
		cout << "**********possibilisticSuccess*************" << endl;
		//cout << "***************************************" << endl;
	}
	for (ruleIt=p->getRuleList()->begin(); ruleIt!=p->getRuleList()->end(); ruleIt++) {
		if (!ruleIt->getIsDeleted()) {
			if (ruleIt->getRuleType()==NORMAL || ruleIt->getRuleType()==DISJUNCTION) {
				list<Fact>::iterator factsIt;
				for (factsIt=programFacts->begin(); factsIt!=programFacts->end(); factsIt++) {
					if (findAtomInAtomList(ruleIt->getPosBody(),factsIt->getAtom())) {

						rule2AtomToremove.insert(pair<int, string>(ruleIt->getRuleId(),factsIt->getAtom()));
						ruleIt->setPossibilisticValue(min(ruleIt->getPossibilisticValue(),factsIt->getPossibilisticValue()));
						if (TRANSFORMATIONS_DEBUG) {
							cout << "findAtomInAtomList " << factsIt->getAtom() << " in rule " << ruleIt->getRuleId() << endl;
							cout << "Rule " << ruleIt->getRule() << endl;
							cout << "New Posssibilistic value  " << ruleIt->getPossibilisticValue() << endl;
							cout << "***************************************" << endl;
						}
					}
				}
			}
		}
	}
	if (rule2AtomToremove.size()>0) {
		multimap<int,string>::iterator mIt;
		for (mIt=rule2AtomToremove.begin();mIt!=rule2AtomToremove.end();mIt++) {
			//cout << (*it).first << " => " << (*it).second << endl;
			//int id = (*mIt).first;
			removeAtomFromBody((*mIt).first,(*mIt).second,p->getRuleList(),false);
			updateRuleType(p->getRuleList());
		}
		updateLppodProgramFacts(p);
		updateLppodProgramHead(p);
	}
}


/*
 * P \rightarrow_{PF} P', if $P$ contains:
 * 		- a rule $r = \alpha: {\cal C}^{\times} \leftarrow {\cal B}^+, \; not \; {\cal B}^-$ such that
 * 		- $a \in {\cal B}^+$ and $a \notin HEAD(P)$, and
 * 		-> P' = (P \backslash  \{r\})$.
 *
 */
void possibilisticFailure(LppodProgram *p, list<string> *programHead) {

	list<LppodRule>::iterator ruleIt;
	if (TRANSFORMATIONS_DEBUG) {
		cout << "*******************************************" << endl;
		cout << "**********possibilisticFailure*************" << endl;
		//cout << "***************************************" << endl;
		list<string>::iterator headIt;
		cout << "ProgramHead" << endl;
		for (headIt=programHead->begin();headIt!=programHead->end();headIt++) {
			cout << (*headIt) << " | ";
		}
		cout << endl;
	}
	for (ruleIt=p->getRuleList()->begin(); ruleIt!=p->getRuleList()->end(); ruleIt++) {
		if (!ruleIt->getIsDeleted()) {
			list<string>::iterator posBodyit;
			for (posBodyit=ruleIt->getPosBody()->begin(); posBodyit!=ruleIt->getPosBody()->end(); posBodyit++) {
				if (!findAtomInAtomList(programHead,*posBodyit)) {
					if (TRANSFORMATIONS_DEBUG) {
						cout << "findAtomInAtomList " << *posBodyit << " in rule " << ruleIt->getRuleId() << endl;
						cout << "Rule " << ruleIt->getRule() << endl;
						//cout << "New Posssibilistic value  " << ruleIt->getPossibilisticValue() << endl;
						cout << "***************************************" << endl;
					}
					ruleIt->setIsDeleted(true);
					updateLppodProgramFacts(p);
					updateLppodProgramHead(p);
				}
			}
		}
	}
}

//TODO find a way to execute all combinations of transformation rules until the normal form is found
void transformLppod(LppodProgram *p) {

	possibilisticContradiction(p);
	possibilisticPositiveReduction(p,p->getProgramHead());
	possibilisticNegativeReduction(p,p->getProgramFacts());
	possibilisticSuccess(p,p->getProgramFacts());
	possibilisticFailure(p,p->getProgramHead());

}
