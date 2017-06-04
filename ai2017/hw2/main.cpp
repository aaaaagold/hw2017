
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

int main(const int argc,const char *argv[])
{
	const string arg_qd="-qd"; // no print distinguish
	const string arg_qn="-qn"; // no print nb info
	bool opt_qd=0;
	bool opt_qn=0;
	for(int x=1;x<argc;x++)
	{
		if(arg_qd==argv[x]) opt_qd=1;
		if(arg_qn==argv[x]) opt_qn=1;
	}
	
	alldata tmp;
	tmp.readAlldata(argv[1]);
	dataFormat &h=tmp.head;
	nb xd(tmp.rv,h);
	size_t errCnt=0;
	map<string,size_t> errCntEach; for(size_t x=h.o.getinfo().size();x--;) errCntEach[h.o.getinfo()[x] ]=0;
	for(int x=0,xs=tmp.rv.size();x<xs;x++)
	{
		string c=xd.distinguish(tmp.rv[x],!opt_qd);
		if(!opt_qd) cout<<tmp.rv[x].output()<<" -> "<<c<<endl<<endl;
		if(tmp.rv[x].output()!=c)
		{
			errCnt++;
			errCntEach[tmp.rv[x].output()]++;
		}
	}
	cout<<"error:"<<errCnt<<" total:"<<tmp.rv.size()<<endl;
	cout<<"error in each class:\t";
		for(auto it=errCntEach.begin();it!=errCntEach.end();it++) cout<<" "<<it->first<<":"<<it->second;
	cout<<endl;
	if(!opt_qn)
	{
		xd.printcp();
		xd.printfs();
	}
	return 0;
}
