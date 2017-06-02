#ifndef __getnames_h____
#define __getnames_h____

/*

format example:

| comments
| comments

classes1, classes2, | undefined leading space/tab
classes3, | other class name can be in next line
classes 4 | a class name can have space. the last class name can either have '.' or not.

| at least 1 empty line

attr1: value1,value2
attr2: value1,value2. | the last value can have either '.' or not.
attr3: value1,value2,
   value3 | other values can be in next line, undefined leading space/tab
attr4: continuous | 'continuous' is a key word meaning that the value is a number.

*/

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <cmath>

bool debug=0;

using namespace std;

#define strpopbk string::pop_back
#ifdef strpopbk
bool defstrpopbk=1;
#else
bool defstrpopbk=0;
#endif

const char commentStrt='|';
inline bool isSpace(const char c){return c==' '||c=='\t'||c=='\n'||c=='\r';}
inline bool isSep(const char c){return c==','||c=='.';}
int colonAt(const string &s)
{
	for(int x=0;s[x];x++) if(s[x]==':') return x;
	return -1;
}
bool commentOnly(const string &s)
{
	int x=0;
	for(;s[x];x++) if(!isSpace(s[x])) break;
	return s[x]==commentStrt;
}
string &cutComment(string &s)
{
	int x=0;
	for(;s[x];x++) if(s[x]==commentStrt) break;
	s.resize(x);
	return s;
}
string &cutTailingSpace(string &s)
{
	while(s.size() && isSpace(s.back())) s.pop_back();
	return s;
}
vector<string> &splitLine(vector<string> &ctn,const string &src,int srcstrt=0)
{
	string t="";
	for(int x=srcstrt;src[x];x++)
	{
		if(isSep(src[x])){ ctn.push_back(cutTailingSpace(t)); t=""; }
		else if(t!=""||isSpace(src[x])==0) t+=src[x];
	}
	if(t!="") ctn.push_back(cutTailingSpace(t));
	return ctn;
}
vector<string> splitRow(const string &src,const char del=',')
{
	vector<string> rtv;
	string t="";
	for(int x=0;src[x];x++)
	{
		if(src[x]==del){ rtv.push_back(cutTailingSpace(t)); t=""; }
		else if(t!=""||isSpace(src[x])==0) t+=src[x];
	}
	if(t!="") rtv.push_back(cutTailingSpace(t));
	return rtv;
}

class dataType
{
public:
	vector<string> vs;
	bool isNumber(){return vs.size()==1 && vs[0]=="continuous";}
	dataType &operator=(const vector<string> &rhs){vs=rhs;return *this;}
	void print(const string &sep="$\n")const // debug
	{
		for(int x=0,xs=vs.size();x<xs;x++) cout<<vs[x]<<sep;
	}
};
class dataFormat
{
public:
	dataType o;
	vector<dataType> iv;
	dataFormat(){}
	dataFormat(const vector<vector<string> > &input,const vector<string> &output){set(input,output);}
	void set(const vector<vector<string> > &input,const vector<string> &output)
	{
		iv.resize(input.size());
		for(int x=0,xs=iv.size();x<xs;x++) iv[x]=input[x];
		o=output;
	}
	void print()const // debug
	{
		o.print("$\n");
		for(int x=0,xs=iv.size();x<xs;x++){ iv[x].print(","); cout<<endl;}
	}
};

dataFormat readNames(const string &fn)
{
	ifstream iii(fn.c_str(),ios::binary);
	vector<string> lines;
	for(string s;getline(iii,s);)
	{
		if(s.back()=='\r') s.pop_back();
		if(!commentOnly(s)) lines.push_back(cutComment(s));
	}
	if(0){ for(int x=0,xs=lines.size();x<xs;x++) cout<<setw(7)<<lines[x].size()<<" "<<lines[x]<<endl; } // debug
	int x=0;
	vector<string> targets;
	for(int xs=lines.size();x<xs;x++)
	{
		if(lines[x].size()==0) if(targets.size()==0) continue; else break;
		splitLine(targets,lines[x]);
	}
	while(lines[x].size()==0) x++;
	vector<vector<string> > attrs;
	for(int xs=lines.size();x<xs;x++)
	{
		int c=colonAt(lines[x]);
		if(c==-1) if(attrs.size()==0) continue; else splitLine(attrs.back(),lines[x]);
		else
		{
			vector<string> t;
			attrs.push_back(splitLine(t,lines[x],c+1));
		}
	}
	return dataFormat(attrs,targets);
}

#endif
