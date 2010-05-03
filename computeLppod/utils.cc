/*
 * utils.cc
 *
 *  Created on: 18/dic/2009
 *      Author: confalonieri
 */
#include "utils.h"
//#include <stdlib.h>
//#include <string.h>
//#include <iostream>


Utils::Utils() {}
Utils::~Utils() {}


//public:
char* Utils::string2char(string s) {

	//if (DEBUG)
	//cout << "\nString is: "<<s;
	char *finalc = new char [s.size()+1];
	strcpy (finalc, s.c_str());
	//if (DEBUG)
	//cout << "\nChar is: "<<finalc;
	return finalc;

}

string Utils::char2string(char *c) {

	// if (DEBUG)
	//cout << "\nChar is: "<<c;
	string finals(c);
	//if (DEBUG)
	//cout << "\nString is: "<<finals;
	return finals;
}

vector<string> Utils::stringSplit(string str) {


	vector<string> splittedString;
	string buf; // Have a buffer string
	stringstream ss(str); // Insert the string into a stream
	while (ss >> buf)
		splittedString.push_back(buf);
	return splittedString;
}



int Utils::startsWith(string l, char* key) {
	char *line = Utils::string2char(l);
	if (strncmp(line,key,strlen(key))==0)
		return(1);
	return(0);
}

int Utils::contains(string str, string key) {

	size_t found;
	//cout << "string to input " << str << endl;
	//cout << "substring to look for " << key << endl;
	found=str.find(key);
	//string s= NULL;
	if (found!=string::npos) {
		//s.substr(found,str.size()-key.size());
		//cout << "substring " << s << endl;
		//cout << "Founded" << key << endl;
		return(1);

	}
	//cout << "Not Founded" << key << endl;
	return 0;
}

string Utils::removeSubString(string str, string key) {

	size_t found;
	size_t input_s = str.size();
	size_t subs_to_remove = key.size();
	string output;
	//cout << "string to inout " << str << endl;
	//cout << "substring to remove " << key << endl;
	found=str.find(key);
	if (found!=string::npos) {
		if (key.compare(".") ==0 ) {
			if (found>0)
				output = str.substr(0,input_s-1);
			else
				cout <<  "Fatal error" << endl;
		}
		if (key.compare(",") == 0) {
			output = str.substr(0,input_s-1);
		}
		if (key.compare("not") == 0) {
			output = str.substr(3,input_s);
		}
		//cout << "String to return " << output << endl;
	}
	else {
		cout << "Fatal error, substring must be there" << endl;
		exit(1);
	}
	return output;
}

void Utils::printVectorString(vector<string> v) {

	for (int i=0; i<v.size();i++) {


		cout << v[i] << endl;
	}

}

int Utils::string2Int(string strConvert) {
	int intReturn;

	// NOTE: You should probably do some checks to ensure that
	// this string contains only numbers. If the string is not
	// a valid integer, zero will be returned.
	intReturn = atoi(strConvert.c_str());

	return(intReturn);
}


