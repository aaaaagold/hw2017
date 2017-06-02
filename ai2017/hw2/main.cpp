
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
		head.print();
		for(int x=0,xs=rv.size();x<xs;x++) rv[x].print();
	}
};
alldata readAlldata(const string &fnprefix)
{
	alldata rtv;
	string tmpstr;
	tmpstr=fnprefix; tmpstr+=".names";
	rtv.head=readNames(tmpstr);
	for(int x=0,xs=rtv.head.iv.size();x<xs;x++){ auto &v=rtv.head.iv[x].vs; sort(v.begin(),v.end()); }
	{ auto &v=rtv.head.o.vs; sort(v.begin(),v.end()); }
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
			rtv.rv.push_back(row(iv,o));
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
