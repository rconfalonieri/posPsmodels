// Copyright (C) 2009 Roberto Confalonieri
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
// http://www.gnu.org/copyleft/gpl.html
//
// confalonieri@lsi.upc.edu
//

//
// syntax for logic program with possibilistic ordered disjunction
//
%{
#include <stdlib.h>
#include <set>
#include <iterator>
using namespace std;
#define YYERROR_VERBOSE
#define YYSTYPE char*
void yyerror(char* mess);
int yylex();
class ltstring{
public:	bool operator()(const char* s1,const char* s2)const{
		return strcmp(s1,s2)<0;
	}
};
typedef set<char*,ltstring> set_car;
set_car Degrees;
%}

%token IF NUM SEPAR COMMENT CAR DEF

%%

program :
	/* nothing */
	| program item
	;
item :
	NUM  rule 	{Degrees.insert($1);printf("%s",$2);}
	| SEPAR		{printf("%s",$1);}
	| COMMENT	{printf("%s",$1);}
	| DEF		{printf("%s",$1);}
	| error	'.'	{yyerrok;}
	;
rule :
	chain '.'		{$$=(char*)malloc(strlen($1)+3);
				strcpy($$,$1);
				strcat($$,".");
				}
	| IF chain '.'		{$$=(char*)malloc(strlen($2)+4);
				 strcpy($$, $1);
				 strcat($$, $2);
				 strcat($$,".");
				 }
	| chain IF chain '.'	{$$=(char*)malloc(strlen($1)+strlen($3)+4);
				 strcpy($$, $1);
				 strcat($$, $2);
				 strcat($$, $3);
				 strcat($$,".");
				 }
	;
chain :
	element			{$$=$1;}
	| chain element	{$$=(char*)malloc(strlen($1)+strlen($2)+1);
				 strcpy($$, $1);
				 strcat($$, $2);}
	;
element :
	CAR		{$$=$1;}
	| SEPAR		{$$=$1;}
	| NUM		{$$=$1;}
	
	;
	
%%
#include "preprocLPPODparse.lex.c"
#include "preprocLPPODparse.h"
#include "utils.h"
#include <string.h>
#include <stdio.h>

char* possibilistic_projection(char *lppod) {

//int main(int argc, char** argv){
//if (argc>1){

//string filename(lppod);
//Utils u;
string filename = Utils::char2string(lppod);
string filename2;
char *output;
FILE* f;

if (lppod!=NULL) {
	int nsize = filename.size();
	//cout << "\nLength filename is: "<<nsize;
	filename2 = filename.substr(4,nsize-10);
	//filename2 = filename.substr(6,nsize-10);
	
	//cout << "Program name is: " <<filename2;
	f = fopen (lppod,"r");
	if (!f){
	  cout << "impossible to open :" << lppod << endl;
	  //fprintf(stderr, "impossible to open %s \n", lppod);
	  //exit(1);
	  return NULL;
	  
	}
	yyin = f;
	
//cout <<"Program to apply projection * "<<filename;

}	
FILE* stream;
output = Utils::string2char(filename2);
filename2+= ".lpod";
string pref = "lpod/";
pref+=filename2;
//cout <<"\nFile to write projection * "<<pref;



char* finalname = Utils::string2char(pref);
//cout<<"\nfinalname is "<<finalname;


//redirectin stdout to a file
fpos_t pos;
int    fd;
//cout <<"\n stdout "<<stdout;
//cout <<"\n &pos "<<&pos;
fgetpos(stdout, &pos);
fd = dup(fileno(stdout));

  if((stream = freopen(finalname, "w", stdout)) == NULL) 
      exit(-1);
     
yyparse();

if (yynerrs)
	return NULL;

fflush(stdout);
//Close file and restore standard output to stdout - which should be the terminal
dup2(fd, fileno(stdout));
close(fd);
clearerr(stdout);
fsetpos(stdout, &pos);
//fclose(f);
//fclose(stream);
return output;
}

void yyerror(char* mess){
fprintf(stderr,"%s \n",mess);
}



