/*
 * parsing.cc
 *
 *  Created on: 14/gen/2010
 *      Author: confalonieri
 */

#include "parsing.h"


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
printPossibilisticModels(list<LpodModel> *modelList,list<string> *possibilisticModels) {

	list<string>::iterator possModels_it;
	if (possibilisticModels->size()>0) {
		int i=1;

		for (possModels_it=possibilisticModels->begin(); possModels_it!=possibilisticModels->end(); possModels_it++) {
			cout << endl;
			cout << endl;
			cout << "******** Possibilistic Model " << i << "***********" << endl;
			cout << "Possibilistic Atoms: " << endl;
			cout << *possModels_it << endl;
			LpodModel *m = getLpodModelById(modelList,i);
			cout << "Satisfaction degrees: " << endl;
			list<string>::iterator pos;
			if (m != 0) {
				for (pos=m->getSatisfaction()->begin(); pos!=m->getSatisfaction()->end(); pos++) {
					cout << *pos << " ";
				}
				cout << endl;
				//model_it = find()
			}
			else {
				cout << "The program was a possibilistic normal program, no satisfaction degrees indeed " << endl;
			}
			cout << "***************************************************" << endl;
			cout << endl;
			cout << endl;
			i++;
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


int parseLppodRule(vector<string> splittedLine, string line, LppodProgram *p) {

	//vector<string>::iterator it;
	int size = splittedLine.size();
	LppodRule *r = new LppodRule;
	r->setPossibilisticValue(splittedLine[0]);
	r->setRule(line);
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
		if (r->getRuleType()==UNDEFINED)
			r->setRule(line);
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



/*
//TODO need to be optimized / changed
int parsePsmodels(char *model_file,list<LpodModel> *modelList) {

	int prev_line_was_answer_num = 0;
	int new_line,type;
	int models;

	int inside_quotes;
	//tmp string for line saving
	char *tmp_line;

	long pos;
	char *p=NULL,*tline;
	LpodModel *model;


	int A_prolog_like=0;


	FILE* mfile = fopen(model_file,"r");
	if (mfile == NULL) {
		cout << "Error in opening the model file" << endl;
		return(EXIT_FAILURE);
	}

	tmp_line = (char *)calloc(1025,sizeof(char));


	tmp_line[0]=0;
	pos=0;
	new_line=1;
	prev_line_was_answer_num=0;
	models=0;
	type=MOD_SMODELS;

	while (!feof(mfile)) {
		//cout << "while 1" << endl;
		if (fgets(tmp_line,1024,mfile)==NULL)
			return(EXIT_FAILURE);

		while ((tmp_line[strlen(tmp_line)-1]=='\n') || (tmp_line[strlen(tmp_line)-1]=='\r')) {
			tmp_line[strlen(tmp_line)-1]=0;
			new_line=1;
		}

		if (is_answer_line(tmp_line)) {
			prev_line_was_answer_num=1;
			continue;
		}
		if (is_last_line(tmp_line)) {
			break;
		}
		if (is_model(tmp_line,prev_line_was_answer_num,&type)) {

			int read_more,trm,i=0;
			char *l;

			models++;

			model = new LpodModel(models);
			tline=tmp_line;

			switch(type) {
			case MOD_SMODELS:
				while (*tline!=':') tline++;
				tline+=2; 	 skip leading space
				break;
			}
			read_more=!new_line;

			do {
				l=tline;
				inside_quotes=0;
				while ((*l) && (*l!='}')) {

					p = l;
					//cout << "while *l and l!=}" << endl;
					while ((*l) && (inside_quotes || ((*l!='}') && (*l!=' ')))) {
						if (*l=='\"') inside_quotes=!inside_quotes;
						l++;
					}
					//if the line did not fit in the buffer...
					if (((*l)==0) && (read_more)) {
						//cout << "the line did not fit the buffer..." << endl;
						//move the rest of the line to the beginning of the buffer...
						for(i=0;i<(int)(l-p);i++)
							tmp_line[i]=p[i];
						tmp_line[i]=0;
						 ...and make sure we exit the while loop
						l=&tmp_line[i];
					}
					 otherwise, output the current atom
					else
					{
						//cout << "the line fit the buffer..." << endl;
 *l=0;
						l++;
						//pos+=printf("%s",p);
						string s = Utils::char2string(p);
						if (!is_satisfaction(p))
							model->getAtoms()->push_back(s);
						else
							model->getSatisfaction()->push_back(s);
						//pos+=printf("\n")+crlf_addon;
						i=0;
						//char* at = model.atoms->pop_back();
						//cout << "Inside model" << at << endl;
					}
				}
				trm=read_more;
				if (read_more)	{
					//cout << "read more." << endl;
					tline=tmp_line;
					if (fgets(&tmp_line[i],1024-i,mfile)==NULL) {
						if (i>0)	 if the file is over and we still have an atom to write, write it
							//pos+=printf("%s%s",p,(A_prolog_like) ? ".\n":"\n")+crlf_addon;
							break;
					}
					read_more=1;
					while ((tmp_line[strlen(tmp_line)-1]=='\n') || (tmp_line[strlen(tmp_line)-1]=='\r')) {
						tmp_line[strlen(tmp_line)-1]=0;
						read_more=0;
					}
				}
			} while(trm);
			//if (show_endmodel_marker && !meta_model)
			//pos+=printf("%sendmodel\n",(A_prolog_like) ? "%%":"::")+crlf_addon;
			//cout << "Push model " << model->getModel_id() << endl;
			modelList->push_back(*model);
			//delete(model);
			//printListElement(modelList);
		}
		prev_line_was_answer_num=0;

	}
	//printListElement(modelList);
	if (models==0)
		printf("%s no models found.\n",(A_prolog_like) ? "%***":"***");

	//cout <<"closing mfile" << endl;
	fclose(mfile);
	//free(tmp_line);
	return(1);
}*/


int
parseLppod(char* lppod, LppodProgram *p) {

	string line;
	vector<string> splittedLine;
	LppodRule *r;
	string filename = Utils::char2string(lppod);
	ifstream f;

	f.open(filename.c_str());

	if (f==NULL) {
		cout << "Error in opening the lppod program file" << endl;
		return(EXIT_FAILURE);
	}

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
				p->getRuleList()->push_back(*r);
				//delete r;
				//p->ruleNumber++;
				break;

			default:
				//cout << "Line to parse"<< endl;
				parseLppodRule(splittedLine,line,p);
			}
		}
	}
	//if (DEBUG)
	//	p->printRuleList();
	f.close();
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
generatePossibilisticModel(list<string> *possibilisticModels, string possmodel) {

	string line;
	ifstream f;

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
			if ( (line.find("(",0) != string::npos) && (line.find("(smodels)",0) == string::npos))
				possibilisticModels->push_back(line);
		}
	}
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


