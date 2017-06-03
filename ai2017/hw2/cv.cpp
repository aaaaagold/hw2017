
/*

xxx.data
xxx.names

=>

xxx_cv1.names // same as xxx.names
xxx_cv1.data
xxx_cv1.test

xxx_cv2.names // same as xxx.names
xxx_cv2.data
xxx_cv2.test

...

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

#include "getnames.h"

using namespace std;

int main(const int argc,const char *argv[])
{
	string target=argv[1];
	vector<char> names;
	{
		ifstream iii(target+".names",ios::binary);
		iii.seekg(0,iii.end);
		size_t len=iii.tellg();
		iii.seekg(0,iii.beg);
		names.resize(len);
		iii.read(&names[0],len);
	}
	alldata a; a.readAlldata(target);
	unsigned k; if(argc<=2 || sscanf(argv[2],"%u",&k)!=1) k=10;
	vector<alldata> folds=a.cut(k);
	for(int x=0,xs=folds.size();x<xs;x++)
	{
		stringstream ss;
		ss<<argv[1]<<"_cv"<<(x+1);
		string s;
		ss>>s;
		folds[x].tofile(s+".test");
		alldata data;
		for(int z=0;z<xs;z++)
			if(z==x) continue;
			else data.add(folds[z].rv);
		data.tofile(s+".data");
		ofstream ooo(s+".names",ios::binary); ooo.write(&names[0],names.size());
	}
	return 0;
}
