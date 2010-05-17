/*
 * utils.h
 *
 *  Created on: 18/dic/2009
 *      Author: confalonieri
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <iostream>
#include <vector>
#include <sstream>
using namespace std;

class Utils {


public:

	Utils();
	~Utils();


	 static char* string2char(string s);
	 static string char2string(char *c);
	 static string extractString(string s, int s1, int e1, int s2, int e2);
	 static vector<string> stringSplit(string str);

	 static int startsWith(string l, char* key);
	 static int contains(string str, string key);
	 static string removeSubString(string str, string key);
	 static void printVectorString(vector<string> v);
	 static int string2Int(string strConvert);
	 static string int2String(int intConvert);
	// static void tokenize(const string& str, vector<string>& tokens, const string& delimiters = " ");

};

#endif /* UTILS_H_ */
