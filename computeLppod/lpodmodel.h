/*
 * model.h
 *
 *  Created on: 13/gen/2010
 *      Author: confalonieri
 */

#ifndef LPODMODEL_H_
#define LPODMODEL_H_




#include <list>
#include <string>
#include <iostream>
using namespace std;

class LpodModel {

private:
	int model_id;
	list<string> *atoms;
	list<string> *satisfaction;
	bool bestModel;

public:


	LpodModel(int id);
	~LpodModel ();

    bool getBestModel() const
    {
        return bestModel;
    }

    void setBestModel(bool bestModel)
    {
        this->bestModel = bestModel;
    }

    list<string> *getAtoms() const
    {
        return atoms;
    }

    int getModel_id() const
    {
        return model_id;
    }

    list<string> *getSatisfaction() const
    {
        return satisfaction;
    }

    void setAtoms(list<string> *atoms)
    {
        this->atoms = atoms;
    }

    void setModel_id(int model_id)
    {
        this->model_id = model_id;
    }

    void setSatisfaction(list<string> *satisfaction)
    {
        this->satisfaction = satisfaction;
    }

    void printAtoms()
    {
        cout << "----------------------------------" << endl;
        cout << "printing atoms in model: " << this->model_id << endl;
        list<string>::iterator pos;
        for(pos = this->atoms->begin();pos != this->atoms->end();pos++){
            cout << "Atom: " << *pos << endl;
        }
        cout << "----------------------------------" << endl;
    }

    void printSatisfactionDegree()
    {
        //cout << "----------------------------------" << endl;
        //cout << "printing atoms in model: " << this->model_id << endl;
        list<string>::iterator pos;
        for(pos = this->satisfaction->begin();pos != this->satisfaction->end();pos++){
            cout << *pos << " ";
        }
    }


};

#endif /* LPODMODEL_H_ */
