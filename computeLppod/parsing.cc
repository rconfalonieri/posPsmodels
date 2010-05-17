/*
 * parsing.cc
 *
 *  Created on: 14/gen/2010
 *      Author: confalonieri
 */

#include "parsing.h"
#include "lppodmodel.h"


bool existModel(list<LpodModel>* l, LpodModel *model, bool bestModel) {

	bool isAtomListEqual = false;
	bool isSatListEqual = false;

	if (PARSING_DEBUG_NEW) {
		cout << "existModel" << endl;
	}
	//if (l!=0) {
	if (l->size()>0) {


		list<string> atomL = *model->getAtoms();
		list<string> satL = *model->getSatisfaction();
		list<LpodModel>::iterator pos;

		for (pos=l->begin(); pos!=l->end(); pos++) {


			list<string> avector = *pos->getAtoms();
			list<string> satvector = *pos->getSatisfaction();


			if (atomL==avector) {

				isAtomListEqual = true;
				if (PARSING_DEBUG_NEW) {
					cout << "atomL == avector" << endl;
				}
			}

			if (satL==satvector) {

				isSatListEqual = true;
				if (PARSING_DEBUG_NEW) {
					cout << "satL == satcvector" << endl;
				}

			}
			if (isAtomListEqual&&isSatListEqual)
				pos->setBestModel(bestModel);
		}
	}
	return isAtomListEqual&&isSatListEqual;
}

void
printListElement(list<LpodModel> *l) {


	if (PARSING_DEBUG_NEW) {
		cout << "Printing LpodModelList" << endl;
		list<LpodModel>::iterator pos;

		for (pos=l->begin(); pos!=l->end(); pos++) {

			cout << "Model id " << pos->getModel_id() << endl;
			cout << "Best Model " << pos->getBestModel() << endl;
			//vector<char*>::iterator pos2;
			//cout << "Before " << endl;
			list<string> *avector = pos->getAtoms();
			list<string> *satvector = pos->getSatisfaction();
			if (avector->empty())
				cout << "Atoms Empty" << endl;

			list<string>::iterator i;
			for (i=avector->begin(); i!=avector->end(); i++) {
				cout << "Atom  "<< *i << endl;
			}
			if (satvector->empty())
				cout << "Satsifaction Empty" << endl;

			for (i=satvector->begin(); i!=satvector->end(); i++) {
				cout << "Satisfation  "<< *i << endl;
			}
		}
	}
}


LpodModel *getLpodModelById(list<LpodModel> *modelList, int modelId) {

	list<LpodModel>::iterator model_it;
	LpodModel *model = 0;
	for (model_it=modelList->begin(); model_it!=modelList->end(); model_it++) {
		if (model_it->getModel_id()==modelId) {
			model = &*model_it;
			//return &*model_it;
		}
	}
	return model;
}



void
printPossibilisticModels(list<LppodModel> *possModelList) {

	list<LppodModel>::iterator possModelsIt;
	list<PossibilisticAtom>::iterator possAtomsIt;
	map<int,int>::iterator ruleDegreeIt;
	map<int,int>::iterator rulePossValueIt;
	map<int,string>::iterator ruleTextIt;
	list<string>::iterator atomIt;
	list<string>::iterator satIt;
	PossibilisticDegree *possDegree;
	if (possModelList->size()>0) {
		//int i=1;

		for (possModelsIt=possModelList->begin(); possModelsIt!=possModelList->end(); possModelsIt++) {
			cout << endl;
			cout << endl;
			cout << "******** Possibilistic Model " << possModelsIt->getModel_id() << "***********" << endl;
			//cout << "Possibilistic Atoms: " << endl;
			for (possAtomsIt=possModelsIt->getPossAtomList()->begin(); possAtomsIt!=possModelsIt->getPossAtomList()->end(); possAtomsIt++) {
				cout << "(" << possAtomsIt->getAtom() << "," << possAtomsIt->getPossibilisticValue() << ") ";
			}
			cout <<  endl;
			possDegree = possModelsIt->getPossDegree();
			cout << "[Rule_id,sat_degree]: ";
			for (ruleDegreeIt=possDegree->getRuleDegree()->begin(); ruleDegreeIt!=possDegree->getRuleDegree()->end(); ruleDegreeIt++) {
				cout << "[" << ruleDegreeIt->first << "," << ruleDegreeIt->second << "] " ;
				//cout
				//cout << "(" << possDegIt->getPossRuleValue() << "," << possDegIt->getRuleId() << "," << possDegIt->getDegree() << ")" << endl;
			}
			cout <<  endl;
			cout << "[Rule_id,Necessity_degree]: ";
			for (rulePossValueIt=possDegree->getRulePossValue()->begin(); rulePossValueIt!=possDegree->getRulePossValue()->end(); rulePossValueIt++) {
				cout << "[" << (*rulePossValueIt).first << "," << (*rulePossValueIt).second << "] " ;
				//cout
				//cout << "(" << possDegIt->getPossRuleValue() << "," << possDegIt->getRuleId() << "," << possDegIt->getDegree() << ")" << endl;
			}
			//cout << "[Rule_id,Rule]: ";
			cout <<  endl;
			for (ruleTextIt=possDegree->getRuleText()->begin(); ruleTextIt!=possDegree->getRuleText()->end(); ruleTextIt++) {
				cout << "[Rule_id,Rule]: " << (*ruleTextIt).first << ", " << (*ruleTextIt).second << endl;
				//cout
				//cout << "(" << possDegIt->getPossRuleValue() << "," << possDegIt->getRuleId() << "," << possDegIt->getDegree() << ")" << endl;
			}
			//cout <<  endl;
			//LpodModel *m = getLpodModelById(modelList,i);
			cout << "Was Best LpodModel " << possModelsIt->getBestModel() << endl;
			//cout << "Atoms: " << endl;
			//cout << "Atoms size: " << possModelsIt->getAtoms()->size() << endl;
			//for (atomIt=possModelsIt->getAtoms()->begin(); atomIt!=possModelsIt->getAtoms()->end(); atomIt++) {
			//	cout << "(" << *atomIt << ") ";
			//}
			//cout <<  endl;

			if (possModelsIt->getSatisfaction()->size()>0) {
				//cout << "Satisfaction degrees: " << endl;
				//for (satIt=possModelsIt->getSatisfaction()->begin(); satIt!=possModelsIt->getSatisfaction()->end(); satIt++) {
				//	cout << *satIt << " ";
				//}
				//cout <<  endl;

			}
			else {
				cout << "The program was a possibilistic normal program, no satisfaction degrees indeed " << endl;
			}
			cout << "***************************************************" << endl;
			//cout << endl;
			//cout << endl;
			//i++;
		}
	}
}


int startsWith(char *a,char *key)
{	if (strncmp(a,key,strlen(key))==0)
	return(1);
return(0);
}

int is_answer_line(char *l)
{	return(startsWith(l,"Answer:"));
}

int is_last_line(char *l)
{	return(startsWith(l,"Tester calls:"));
}

int is_satisfaction(char *l)
{	return(startsWith(l,"*sat"));
}

int is_smodels_model(char *l)
{	return(startsWith(l,"Stable Model:"));
}

int is_model(char *l,int prev_line_was_answer_num,int *type)
{	int new_type=MOD_NONE;

if (is_smodels_model(l))
	new_type=MOD_SMODELS;
else
	return(0);

if (*type==MOD_NONE)
{	*type=new_type;
return(1);
}

return(new_type==*type);
}


int parseLppodRule(vector<string> splittedLine, string line, LppodProgram *p, int ruleId) {

	//vector<string>::iterator it;
	int size = splittedLine.size();
	LppodRule *r = new LppodRule;
	r->setPossibilisticValue(splittedLine[0]);
	r->setRule(line);
	r->setRuleId(ruleId);
	bool new_symbol = false;
	bool head_done = false;
	bool is_fact = false;
	bool is_disjunction = false;


	for (int i=1; i<size; i++) {
		if (PARSING_DEBUG) {
			cout << "---------------------------------------" << endl;
			cout << "Splitted String size :" << size << endl;
			cout << "Iteration nr :" << i << endl;
			//cout << "---------------------------------------" << endl;
		}
		new_symbol = false;
		is_fact = false;
		if ( splittedLine[i].compare(":-") == 0) {
			if (i==1) {
				r->setRuleType(CONSTRAINT);
				if (PARSING_DEBUG)
					cout << "Rule is a constraint" << endl;
			}
			head_done = true;
			new_symbol = true;
		}

		if (splittedLine[i].compare("x") == 0) {
			r->setRuleType(DISJUNCTION);
			is_disjunction = true;
			if (PARSING_DEBUG)
				cout << "Rule has a disjunction" << endl;
			new_symbol = true;
		}
		if (!head_done && splittedLine[i].compare("x") != 0) {
			if (Utils::contains(splittedLine[i],".") == 0) {
				r->getHead()->push_back(splittedLine[i]);
				if (PARSING_DEBUG)
					cout << "Atom " << splittedLine[i] << " added to head" << endl;
			}
			else {
				if (r->getRuleType()!=DISJUNCTION) {
					r->setRuleType(FACT);
					if (PARSING_DEBUG)
						cout << "Rule is a fact" << endl;
					is_fact = true;
				}
				r->getHead()->push_back(Utils::removeSubString(splittedLine[i],"."));
				if (PARSING_DEBUG)
					cout << "Atom " << Utils::removeSubString(splittedLine[i],".") << " added to head" << endl;
			}
		}
		if (head_done && !is_fact && !new_symbol) {
			//is_body = true;
			string body_atom;
			if (r->getHead()->size()>0 && r->getRuleType()!=DISJUNCTION) {
				r->setRuleType(NORMAL);
				if (PARSING_DEBUG)
					cout << "Rule is a normal rule" << endl;
			}
			if ( splittedLine[i].compare("not") == 0) {
				if (PARSING_DEBUG)
					cout << "splittedLine[i+1] " << splittedLine[i+1] << endl;
				if (Utils::contains(splittedLine[i+1],",") != 0 )
					body_atom = Utils::removeSubString(splittedLine[i+1],",");
				if (Utils::contains(splittedLine[i+1],".") != 0 )
					body_atom = Utils::removeSubString(splittedLine[i+1],".");
				r->getNegBody()->push_back(body_atom);
				if (PARSING_DEBUG)
					cout << "Atom " << body_atom << " added to neg body" << endl;
			}
			else {
				//cout << "i-1 " << i-1 << endl;
				//cout << "splittedLine[i-1] " << splittedLine[i-1] << endl;
				if ( splittedLine[i-1].compare("not") != 0 && Utils::contains(splittedLine[i],",") != 0) {
					body_atom = Utils::removeSubString(splittedLine[i],",");
					r->getPosBody()->push_back(body_atom);
					if (PARSING_DEBUG)
						cout << "Atom " << body_atom << " added to pos body" << endl;
				}
				if ( splittedLine[i-1].compare("not") != 0 && Utils::contains(splittedLine[i],".") != 0 ) {
					body_atom = Utils::removeSubString(splittedLine[i],".");
					r->getPosBody()->push_back(Utils::removeSubString(splittedLine[i],"."));
					if (PARSING_DEBUG)
						cout << "Atom " << body_atom << " added to pos body" << endl;
				}
			}
		}
		if (r->getRuleType()==UNDEFINED) {
			r->setRule(line);
			r->setRuleId(ruleId);
		}
	}
	if (PARSING_DEBUG) {
		cout << "Rule list size: " << p->getRuleList()->size() << endl;
		cout << "---------------------------------------" << endl;
	}
	p->getRuleList()->push_back(*r);
	if (is_disjunction)
		p->isOrderedDisjunctionProgram(is_disjunction);
	if (PARSING_DEBUG) {
		cout << "Rule " << line << " added to the ruleList" << endl;
		cout << "---------------------------------------" << endl;
		cout << "Rule list size: " << p->getRuleList()->size() << endl;
		cout << "---------------------------------------" << endl;
	}

	return 1;
}

void tokenize(const string& str,
		vector<string>& tokens,
		const string& delimiters = " ")
{
	// Skip delimiters at beginning.
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	string::size_type pos     = str.find_first_of(delimiters, lastPos);

	while (string::npos != pos || string::npos != lastPos)
	{
		// Found a token, add it to the vector.
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		// Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}
}

int parsePsmodelsNew(char *model_file,list<LpodModel> *modelList) {

	string line;
	LpodModel *model;
	string filename = Utils::char2string(model_file);
	ifstream f;
	bool newModel = false;
	bool bestModel = false;
	int modelId;
	string tmpId;

	vector<string> satL;
	vector<string> atomL;

	f.open(filename.c_str());

	if (f==NULL) {
		cout << "Error in opening the LPOD model file" << endl;
		return(EXIT_FAILURE);
	}

	while (getline(f, line)) {
		if (PARSING_DEBUG_NEW) {
			cout << "---------------------------------" << endl;
			cout << "Line: " << line << endl;
			cout << "---------------------------------" << endl;
		}
		if (Utils::contains(line,"[Generator] Stable Model: ")!=0) {

			tmpId = line.substr(26,line.size());

			if (PARSING_DEBUG_NEW) {
				cout << "---------------------------------" << endl;
				//cout << "Line: " << line << endl;
				cout << "Line contains [Generator] Stable Model: " << endl;
				cout << "tmpId: " << tmpId << endl;
				cout << "---------------------------------" << endl;
			}
		}
		if (Utils::contains(line,"[Generator] Stable Model: ")==0 && Utils::contains(line,"Stable Model: ")!=0 ) {

			modelId = Utils::string2Int(tmpId);
			model = new LpodModel(modelId);
			newModel = true;
			if (bestModel)
				model->setBestModel(true);
			string tmpModel = line.substr(14,line.size());
			if (PARSING_DEBUG_NEW) {
				cout << "---------------------------------" << endl;
				//cout << "Line: " << line << endl;
				cout << "Line contains Stable Model: " << endl;
				cout << "tmpModel: " << tmpModel << endl;
				cout << "---------------------------------" << endl;
			}
			vector<string> tokens;
			tokenize(tmpModel,tokens);
			for (int i=0;i<tokens.size();i++) {

				if (Utils::contains(tokens[i],"*")) {
					//its a satisfaction degree
					model->getSatisfaction()->push_back(tokens[i]);
				}
				else {
					// its an atom
					model->getAtoms()->push_back(tokens[i]);
				}
			}
		}
		if (Utils::contains(line,"Answer: ")!=0 ) {
			if (PARSING_DEBUG_NEW) {
				cout << "---------------------------------" << endl;
				//cout << "Line: " << line << endl;
				cout << "Line contains Answer: " << endl;
				//cout << "tmpModelCharCopy: " << tmpModelCharCopy << endl;
				//cout << "tmpModelChar: " << tmpModelChar << endl;
				cout << "---------------------------------" << endl;
			}
			bestModel = true;
			//model->setBestModel(true);
		}
		//if modelList
		if (PARSING_DEBUG_NEW) {
			cout << "Before existmodel" << endl;
		}
		if (newModel) {
			//check whether the model already exists in the list
			if (!existModel(modelList,model,bestModel)) {
				modelList->push_back(*model);
				newModel = false;
				bestModel = false;
			}
		}

	}
	if (PARSING_DEBUG_NEW) {

		//delete(model);
		printListElement(modelList);

	}
	f.close();
	//output.close();
	return(1);

}

void updateLppodProgramHead(LppodProgram *p) {

	list<string> *programHead = new list<string>();
	list<LppodRule>::iterator rules;

	list<LppodRule> *ruleL = p->getRuleList();
	for (rules=ruleL->begin(); rules!=ruleL->end(); rules++) {
		if (!rules->getIsDeleted()) {
			list<string>::iterator head;
			for (head=rules->getHead()->begin();head!=rules->getHead()->end();head++) {
				programHead->push_back(*head);
			}
		}
	}
	p->setProgramHead(programHead);
}

void updateLppodProgramFacts(LppodProgram *p) {

	list<Fact> *programFacts = new list<Fact>();
	list<LppodRule>::iterator rules;

	list<LppodRule> *ruleL = p->getRuleList();
	for (rules=ruleL->begin(); rules!=ruleL->end(); rules++) {
		if (!rules->getIsDeleted()) {
			if (rules->getRuleType()==FACT) {
				list<string>::iterator head;
				for (head=rules->getHead()->begin();head!=rules->getHead()->end();head++) {
					Fact *f = new Fact();
					f->setAtom(*head);
					f->setPossibilisticValue(rules->getPossibilisticValue());
					programFacts->push_back(*f);
				}
			}
		}
	}
	p->setProgramFacts(programFacts);
}



int
parseLppod(char* lppod, LppodProgram *p) {

	string line;
	vector<string> splittedLine;
	LppodRule *r;
	string filename = Utils::char2string(lppod);
	ifstream f;
	int ruleId = 1;


	//cout << "dwidnwi" << filename << endl;
	f.open(filename.c_str());

	if (f==NULL) {
		cout << "Error in opening the lppod program file" << endl;
		return(EXIT_FAILURE);
	}
	//cout << "dwidnwi" << filename << endl;

	while (getline(f, line)) {
		if (PARSING_DEBUG) {
			cout << "---------------------------------" << endl;
			cout << "Line: " << line << endl;
			cout << "---------------------------------" << endl;
		}
		r = new LppodRule;
		if (line.empty()) {

			r->setRuleType(EMPTY);
			r->setRule(line);
			r->setRuleId(EMPTY);
			p->getRuleList()->push_back(*r);

		} else {
			splittedLine = Utils::stringSplit(line);
			if (PARSING_DEBUG)
				Utils::printVectorString(splittedLine);
			//cout << "------------------------" << endl;
			char *tmp;
			tmp = Utils::string2char(splittedLine[0]);
			//cout << "tmp" << *tmp << endl;
			switch (*tmp) {
			case '%':
				//if (Utils::contains(splittedLine[0],"%")) {mer

				r->setRuleType(COMMENT);
				r->setRule(line);
				r->setRuleId(COMMENT);
				p->getRuleList()->push_back(*r);
				//delete r;
				//p->ruleNumber++;
				break;

			default:
				//cout << "Line to parse"<< endl;
				parseLppodRule(splittedLine,line,p,ruleId);
				ruleId++;
			}
		}

	}
	//if (DEBUG)
	//	p->printRuleList();
	f.close();

	updateLppodProgramHead(p);

	updateLppodProgramFacts(p);

	//output.close();
	return(1);

}

string
unParseNormalReductedRule(LppodRule *rule_it) {
	string rule;
	string body;
	string head;
	//list<string>::iterator head_it;
	list<string>::iterator body_it;
	list<string> *headL = rule_it->getHead();
	list<string> *bodyL = rule_it->getPosBody();

	if (UNPARSING_DEBUG) {
		cout << "**********************************" << endl;
		cout << "*****unParseNormalReductedRule****" << endl;
		cout << "**********************************" << endl;
		rule_it->printHeadAtoms();
		rule_it->printNegBody();
		rule_it->printPosBody();
	}
	//negative body should be empty and head contains only one atom
	if (rule_it->getNegBody()->size()==0 && rule_it->getHead()->size()==1) {
		if (UNPARSING_DEBUG) {
			cout << "**********************************" << endl;
			cout << "head is " << headL->front() << endl;
			cout << "**********************************" << endl;
		}
		head = headL->front();
		if (UNPARSING_DEBUG) {
			cout << "------------------------" << endl;
			cout << "head is " << head << endl;
			cout << "------------------------" << endl;
		}

		if (bodyL->size()>0) {
			head = rule_it->getPossibilisticValue() +" "+head + " :- ";
			for (body_it=bodyL->begin(); body_it!=bodyL->end(); body_it++) {
				body = body + *body_it + ", ";
			}
			//replacing the ", " last with "."
			if (UNPARSING_DEBUG) {
				cout << "------------------------" << endl;
				cout << "body is" << body << endl;
				cout << "------------------------" << endl;
			}
			body.replace(body.size()-2,2,".");
			if (UNPARSING_DEBUG) {
				cout << "------------------------" << endl;
				cout << "new body is" << body << endl;
				cout << "------------------------" << endl;
			}
		}
		else
			head = rule_it->getPossibilisticValue() +" "+head + ".";
		rule = head + body;
		if (UNPARSING_DEBUG) {
			cout << "------------------------" << endl;
			cout << "rule is" << rule << endl;
			cout << "------------------------" << endl;
		}
	}
	else if (rule_it->getNegBody()->size() > 0) {
		cout << "**************************************************" << endl;
		cout << "***FATAL ERRROR: negative body is not empty*******" << endl;
		cout << "**************************************************" << endl;
	}
	else if (rule_it->getHead()->size() > 1) {
		cout << "******************************************************" << endl;
		cout << "***FATAL ERRROR: head contains more than 1 atom*******" << endl;
		cout << "******************************************************" << endl;
	}
	return rule;
}


void
generatePossibilisticModels(list<LppodModel> *possModelList, string possmodel,LpodModel *lpodModel) {

	string line;
	ifstream f;
	LppodModel *possModel;
	list<PossibilisticAtom> *possAtomList;
	bool found =false;

	f.open(possmodel.c_str());
	if (f==NULL) {
		cout << "Error in opening the possibilisticModels program file" << endl;
		exit(EXIT_FAILURE);
	}
	while (getline(f, line)) {
		if (PARSING_DEBUG) {
			cout << "---------------------------------" << endl;
			cout << "Line: " << line << endl;
			cout << "---------------------------------" << endl;
		}

		if (!line.empty()) {
			if ( (line.find("(",0) != string::npos) && (line.find("(smodels)",0) == string::npos)) {
				vector<string> tokens;
				tokenize(line,tokens);
				PossibilisticAtom *pA;
				possAtomList = new list<PossibilisticAtom>();
				//cout << "possmodels line" << line << endl;
				for (int i=0;i<tokens.size();i++) {
					//cout << "possmodels line" << endl;
					//cout << "tokens " << tokens[i] << endl;
					if (Utils::contains(tokens[i],"(")) {
						//its a an atom

						pA = new PossibilisticAtom();
						string tmp = tokens[i].erase(0,1);
						//cout << "tmp " << tmp << endl;
						string tmp2 = tmp.erase(tmp.size()-1,tmp.size());
						//cout << "tmp2 " << tmp2 << endl;
						pA->setAtom(tmp2);
						found = true;

					}
					if (Utils::contains(tokens[i],")") && !found) {
						// its its possibilistic value
						//model->getAtoms()->push_back(tokens[i]);
						string tmp = tokens[i].erase(tokens[i].size()-1,tokens[i].size());
						//	cout << "tmp " << tmp << endl;
						pA->setPossibilisticValue(tmp);
						possAtomList->push_back(*pA);
						//found2 = true;

					}
					found = false;
				}
			}
		}

		//found2 = false;
	}
	possModel = new LppodModel();
	possModel->setModel_id(lpodModel->getModel_id());
	possModel->setBestModel(lpodModel->getBestModel());
	//possModel->setAtoms(new list<string>());
	possModel->setAtoms(lpodModel->getAtoms());
	//possModel->setSatisfaction(new list<string>());
	possModel->setSatisfaction(lpodModel->getSatisfaction());
	possModel->setPossAtomList(possAtomList);
	possModelList->push_back(*possModel);
	f.close();
}



int
generateReducedLppods(list<LppodProgram> *lppods,string program_name) {

	list<LppodProgram>::iterator lppod;
	list<LppodRule>::iterator rule_it;
	ofstream myfile;
	char *red_program_name;
	int i = 1;
	string nr;

	if (UNPARSING_DEBUG) {
		cout << "**********************************" << endl;
		cout << "*****generateReducedLppods********" << endl;
		cout << "**********************************" << endl;
	}

	if (lppods->size()>0) {

		for (lppod=lppods->begin(); lppod!=lppods->end(); lppod++) {

			stringstream out;
			ofstream myfile;
			out << i;
			nr = out.str();
			red_program_name = Utils::string2char("reducedlppod/"+program_name+".red"+nr);

			myfile.open (red_program_name);
			//myfile << "Writing this to a file.\n";
			for (rule_it=lppod->getRuleList()->begin(); rule_it!=lppod->getRuleList()->end(); rule_it++) {
				int rule_type = rule_it->getRuleType();
				if (UNPARSING_DEBUG) {
					cout << "----------------------------------" << endl;
					cout << "-------Rule type to unparse-------" << rule_type << endl;
					cout << "-------Rule to unparse------------" << rule_it->getRule() << endl;
					cout << "----------------------------------" << endl;
				}
				switch(rule_type){

				case UNDEFINED:
				case EMPTY:
				case COMMENT:
				case FACT:
				case CONSTRAINT:
					myfile << rule_it->getReducedRule() << "\n";
					break;
				case NORMAL:
				case DISJUNCTION:
					if (rule_it->getIsDeleted())
						myfile << rule_it->getReducedRule() << "\n";
					else
						myfile << unParseNormalReductedRule(&*rule_it) << "\n";
					break;

				default:
					cout << "**********************************" << endl;
					cout << "*******Unknown Rule founded*******" << endl;
					cout << "**********************************" << endl;
				}
			}
			myfile.flush();
			myfile.close();
			i++;
		}
	}
	else {
		cout << "*************************" << endl;
		cout << "*******FATAL ERROR*******" << endl;
		cout << "*************************" << endl;
		exit(EXIT_FAILURE);
	}
	return lppods->size();

}

string
unParseLppodRule(LppodRule *rule_it) {
	string rule;
	string body;
	string head;
	list<string>::iterator head_it;
	list<string>::iterator body_it;
	list<string> *headL = rule_it->getHead();
	list<string> *posL = rule_it->getPosBody();
	list<string> *negL = rule_it->getNegBody();

	if (GENERATE_DEBUG) {
		cout << "**********************************" << endl;
		cout << "*****unParseNormalReductedRule****" << endl;
		cout << "**********************************" << endl;
		rule_it->printHeadAtoms();
		rule_it->printNegBody();
		rule_it->printPosBody();
	}

	if (rule_it->getHead()->size()==1)
		head = headL->front();
	else {
		for (head_it=headL->begin(); head_it!=headL->end(); head_it++) {
			head = head + *head_it + " x ";
		}
		head.replace(head.size()-3,3," ");
	}

	if (posL->size()>0 && negL->size()==0) {
		head = rule_it->getPossibilisticValue() +" "+ head + " :- ";
		for (body_it=posL->begin(); body_it!=posL->end(); body_it++) {
			body = body + *body_it + ", ";
		}
		//replacing the ", " last with "."
		if (GENERATE_DEBUG) {
			cout << "------------------------" << endl;
			cout << "body is" << body << endl;
			cout << "------------------------" << endl;
		}
		body.replace(body.size()-2,2,".");
		if (GENERATE_DEBUG) {
			cout << "------------------------" << endl;
			cout << "new body is" << body << endl;
			cout << "------------------------" << endl;
		}
	}
	if (posL->size()==0 && negL->size()>0) {
		head = rule_it->getPossibilisticValue() +" "+ head + " :- ";
		body = "not ";
		for (body_it=negL->begin(); body_it!=negL->end(); body_it++) {
			body = body + *body_it + ", not ";
		}
		//replacing the ", " last with "."
		if (GENERATE_DEBUG) {
			cout << "------------------------" << endl;
			cout << "body is" << body << endl;
			cout << "------------------------" << endl;
		}
		body.replace(body.size()-6,6,".");
		if (GENERATE_DEBUG) {
			cout << "------------------------" << endl;
			cout << "new body is" << body << endl;
			cout << "------------------------" << endl;
		}
	}
	if (posL->size()>0 && negL->size()>0) {
		head = rule_it->getPossibilisticValue() +" "+ head + " :- ";
		for (body_it=posL->begin(); body_it!=posL->end(); body_it++) {
			body = body + *body_it + ", ";
		}
		body = body + "not ";
		for (body_it=negL->begin(); body_it!=negL->end(); body_it++) {
			body = body + *body_it + ", not ";
		}
		body.replace(body.size()-6,6,".");
	}
	if (posL->size()==0 && negL->size()==0)
		head = rule_it->getPossibilisticValue() +" "+ head + ".";
	rule = head + body;
	rule_it->setTransformedRule(rule);
	if (GENERATE_DEBUG) {
		cout << "------------------------" << endl;
		cout << "rule is " << rule << endl;
		cout << "------------------------" << endl;
	}
	//}
	//else if (rule_it->getHead()->size() > 1) {
	//	cout << "******************************************************" << endl;
	//	cout << "***FATAL ERRROR: head contains more than 1 atom*******" << endl;
	//	cout << "******************************************************" << endl;
	//}
	return rule;
}

void
generateLppod(LppodProgram *lppod,string program_name) {

	//list<LppodProgram>::iterator lppod;
	list<LppodRule>::iterator rule_it;
	ofstream myfile;
	char *transformedLppod;
	//int i = 1;
	//string nr;

	if (GENERATE_DEBUG) {
		cout << "**********************************" << endl;
		cout << "*****generateLppod********" << endl;
		cout << "**********************************" << endl;
	}

	//if (lppods->size()>0) {

	//for (lppod=lppods->begin(); lppod!=lppods->end(); lppod++) {

	stringstream out;
	//ofstream myfile;
	//out << i;
	//nr = out.str();
	transformedLppod = Utils::string2char("tmp/"+program_name+".lppod");

	myfile.open (transformedLppod);
	//myfile << "Writing this to a file.\n";
	for (rule_it=lppod->getRuleList()->begin(); rule_it!=lppod->getRuleList()->end(); rule_it++) {
		int rule_type = rule_it->getRuleType();
		if (GENERATE_DEBUG) {
			cout << "----------------------------------" << endl;
			cout << "-------Rule type to unparse-------" << rule_type << endl;
			cout << "-------Rule to unparse------------" << rule_it->getRule() << endl;
			cout << "-------Rule be deleted------------" << rule_it->getIsDeleted() << endl;
			cout << "----------------------------------" << endl;
		}
		switch(rule_type){

		case UNDEFINED:
		case EMPTY:
		case COMMENT:
		case CONSTRAINT:
			myfile << rule_it->getRule() << "\n";
			break;
		case FACT:
		case NORMAL:
		case DISJUNCTION:
			if (!rule_it->getIsDeleted()) {
				myfile << unParseLppodRule(&*rule_it) << "\n";
			}
			break;


		default:
			cout << "**********************************" << endl;
			cout << "*******Unknown Rule founded*******" << endl;
			cout << "**********************************" << endl;
		}
	}
	myfile.flush();
	myfile.close();
	//i++;
	//}
	//}
	//else {
	//	cout << "*************************" << endl;
	//	cout << "*******FATAL ERROR*******" << endl;
	//	cout << "*************************" << endl;
	//	exit(EXIT_FAILURE);
	//}
	//return lppods->size();

}


