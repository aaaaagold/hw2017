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
	bool sorted;
	vector<string> vs;
public:
	dataType(){sorted=0;}
	bool isNumber()const{return vs.size()==1 && vs[0]=="continuous";}
	const vector<string> &getinfo()const{return vs;}
	bool have(const string &s)const
	{
		if(isNumber()){ double tmp; return sscanf(s.c_str(),"%lf",&tmp)==1; }
		else if(sorted) return binary_search(vs.begin(),vs.end(),s);
		else for(size_t x=vs.size();x--;) if(vs[x]==s) return 1;
		return 0;
	}
	void print(const string &sep="$\n")const // debug
	{
		for(int x=0,xs=vs.size();x<xs;x++) cout<<vs[x]<<sep;
	}
	void sort(){std::sort(vs.begin(),vs.end());sorted=1;}
	dataType &operator=(const vector<string> &rhs){vs=rhs;sort();return *this;}
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
	void readNames(const string &fn)
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
		set(attrs,targets);
	}
};

// suppose <string,...> => string
class row
{
	vector<string> i;
	string o;
public:
	row(){}
	row(const vector<string> &in,const string &out){set(in,out);}
	bool operator<(const row &rhs)const{return o<rhs.o;}
	void set(const vector<string> &in,const string &out){i=in;o=out;}
	size_t isize()const{return i.size();}
	vector<string> &inputv(){return i;}
	const vector<string> &inputv()const{return i;}
	string &input(size_t n){return i[n];}
	const string &input(size_t n)const{return i[n];}
	string &output(){return o;}
	const string &output()const{return o;}
	void print()const // debug
	{
		for(int x=0,xs=i.size();x<xs;x++) cout<<i[x]<<",";
		cout<<o<<endl;
	}
};

class alldata
{
public:
	dataFormat head;
	vector<row> rv;
	void print()const // debug
	{
		cout<<" * head"<<endl;
		head.print();
		cout<<" * rows"<<endl;
		for(int x=0,xs=rv.size();x<xs;x++) rv[x].print();
	}
	void add(const row &rhs){rv.push_back(rhs);}
	void sort(){std::sort(rv.begin(),rv.end());}
	vector<alldata> cut(size_t k)const
	{
		vector<alldata> rtv; { alldata tmp; tmp.head=head; rtv.resize(k,tmp); }
		bool sorted=1; for(size_t x=rv.size();--x;) if(!(rv[x-1]<rv[x])){sorted=0;break;}
		if(sorted) for(size_t x=0,xs=rv.size();x<xs;x++){rtv[x%k].add(rv[x]);}
		else
		{
			vector<row> tmp=rv; std::sort(tmp.begin(),tmp.end());
			for(size_t x=0,xs=rv.size();x<xs;x++){rtv[x%k].add(tmp[x]);}
		}
		return rtv;
	}
	void readAlldata(const string &fnprefix)
	{
		string tmpstr;
		tmpstr=fnprefix; tmpstr+=".names";
		head.readNames(tmpstr);
		for(int x=0,xs=head.iv.size();x<xs;x++){ head.iv[x].sort(); }
		{ head.o.sort(); }
		tmpstr=fnprefix; tmpstr+=".data";
		{
			ifstream iii(tmpstr.c_str(),ios::binary);
			vector<string> lines;
			for(string s;getline(iii,s);)
			{
				if(s.back()=='\r') s.pop_back();
				if(!commentOnly(s)) lines.push_back(cutTailingSpace(cutComment(s)));
			}
			for(int x=0,xs=lines.size();x<xs;x++)
			{
				vector<string> iv=splitRow(lines[x]); string o;
				o=iv.back(); iv.pop_back();
				if(head.o.have(o)) add(row(iv,o));
			}
		}
	}
};

#endif
