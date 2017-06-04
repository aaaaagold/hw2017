
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
	alldata tmp;
	tmp.readAlldata(argv[1]);
	bool printClass=argc>2;
	tmp.print();
	dataFormat &h=tmp.head;
	nb xd(tmp.rv,h);
	size_t errCnt=0;
	map<string,size_t> errCntEach; for(size_t x=h.o.getinfo().size();x--;) errCntEach[h.o.getinfo()[x] ]=0;
	for(int x=0,xs=tmp.rv.size();x<xs;x++)
	{
		string c=xd.distinguish(tmp.rv[x],printClass);
		cout<<tmp.rv[x].output()<<" -> "<<c<<endl<<endl;
		if(tmp.rv[x].output()!=c)
		{
			errCnt++;
			errCntEach[tmp.rv[x].output()]++;
		}
	}
	cout<<"error:"<<errCnt<<" total:"<<tmp.rv.size()<<endl;
	cout<<"error in each class: ";
		for(auto it=errCntEach.begin();it!=errCntEach.end();it++) cout<<"\t"<<it->first<<":"<<it->second;
	cout<<endl;
	xd.printcp();
	xd.printfs();
	return 0;
}
