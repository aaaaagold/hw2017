
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

#include "nb.h"
#include "getnames.h"

using namespace std;

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
	vector<alldata> kfold(size_t k)const
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
};
alldata readAlldata(const string &fnprefix)
{
	alldata rtv;
	string tmpstr;
	tmpstr=fnprefix; tmpstr+=".names";
	rtv.head=readNames(tmpstr);
	for(int x=0,xs=rtv.head.iv.size();x<xs;x++){ rtv.head.iv[x].sort(); }
	{ rtv.head.o.sort(); }
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
			if(rtv.head.o.have(o)) rtv.add(row(iv,o));
		}
	}
	return rtv;
}

int main(const int argc,const char *argv[])
{
	auto tmp=readAlldata(argv[1]);
	tmp.print();
	return 0;
}
