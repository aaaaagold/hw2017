
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
	nb xd(tmp.rv,tmp.head);
	for(int x=0,xs=tmp.rv.size();x<xs;x++)
	{
		string c=xd.distinguish(tmp.rv[x],printClass);
		cout<<tmp.rv[x].output()<<" -> "<<c<<endl<<endl;
	}
	xd.printcp();
	xd.printfs();
	return 0;
}
