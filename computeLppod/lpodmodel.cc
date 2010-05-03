/*
 * model.cc
 *
 *  Created on: 13/gen/2010
 *      Author: confalonieri
 */

#include "lpodmodel.h"


LpodModel::LpodModel(int id) {

	this->model_id = id;
	this->atoms = new list<string>();
	this->satisfaction = new list<string>();
	this->bestModel = false;
	//cout <<"Model created " << model_id << endl;
	//atoms = new vector<char*>();
	//satisfaction = new vector<char*>();
}


LpodModel::~LpodModel ()
{

	model_id = -1;

	/*vector<string>::iterator i;
	for (i=atoms->begin(); i!=atoms->end(); i++) {

		delete i;
	}*/
	delete this->atoms;
	/*vector<string>::iterator i2;
		for (i2=satisfaction->begin(); i2!=satisfaction->end(); i2++) {

			delete i2;
		}*/
	delete this->satisfaction;

}




//vector <char*> Model::getAtom() {

//	return atoms;
//}
/*
void Model::setAtom(char *atom, int pos) {
	int ret = -1;
 *atom[pos] = atom;
	nratoms++;
	ret = pos;
	return ret;
}
 */

/*
void Model::setSatifaction(char *satisfaction, int pos) {
	int ret = -1;
 *satisfaction[pos] = satisfaction;
	nrsatisfaction++;
	ret = pos;
	return ret;
}
 */


/*
void Model::printModel() {

	for (int i=0; i < nratoms; i++) {

		cout << "Atom " << *atom[i] << endl;
	}

}
 */

/*
void Model::printSatisfaction() {

	for (int i=0; i < nrsatisfaction; i++) {

		cout << "Satisfaction " << *satisfaction[i] << endl;
	}

}
 */

