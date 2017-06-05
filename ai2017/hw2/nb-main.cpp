
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

bool opt_vd=0;
bool opt_vn=0;
bool opt_u =0;

void test(const string &fnprefix,nb &xd,bool isTest=false)
{
	cout<<fnprefix<<(isTest?".test":".data")<<endl;
	alldata tmp;
	tmp.readAlldata(fnprefix,isTest);
	dataFormat &h=tmp.head;
	if(!isTest) xd.reset(tmp.rv,h);
	size_t errCnt=0;
	map<string,size_t> errCntEach; for(size_t x=h.o.getinfo().size();x--;) errCntEach[h.o.getinfo()[x] ]=0;
	for(int x=0,xs=tmp.rv.size();x<xs;x++)
	{
		string c=xd.distinguish(tmp.rv[x],opt_vd);
		if(opt_vd) cout<<tmp.rv[x].output()<<" -> "<<c<<endl<<endl;
		if(tmp.rv[x].output()!=c)
		{
			errCnt++;
			errCntEach[tmp.rv[x].output()]++;
		}
	}
	cout<<"error:"<<errCnt<<" total:"<<tmp.rv.size()<<endl;
	cout<<"error rate: "<<errCnt*1.0/tmp.rv.size()<<endl;
	cout<<"accurency: "<<1-errCnt*1.0/tmp.rv.size()<<endl;
	cout<<"error in each class:\t";
		for(auto it=errCntEach.begin();it!=errCntEach.end();it++) cout<<" "<<it->first<<":"<<it->second;
	cout<<endl;
}

int main(const int argc,const char *argv[])
{
	const string arg_vd="-vd"; // no print distinguish
	const string arg_vn="-vn"; // no print nb info
	const string arg_u ="-u" ; // test unseen data
	for(int x=1;x<argc;x++)
	{
		if(arg_vd==argv[x]) opt_vd=1;
		if(arg_vn==argv[x]) opt_vn=1;
		if(arg_u ==argv[x]) opt_u =1;
	}
	nb xd;
	test(argv[1],xd);
	if(opt_u ) test(argv[1],xd,1);
	if(opt_vn)
	{
		xd.printcp();
		xd.printfs();
	}
	return 0;
}
