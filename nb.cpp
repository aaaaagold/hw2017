
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <cmath>

using namespace std;

#ifndef M_SQRT1_2
const double M_SQRT1_2=sqrt(0.5);
#endif
double normalCFD(double value){ return 0.5*erfc(-value*M_SQRT1_2); } // N(0,1)

int main(const int argc,const char *argv[])
{
	if(argc==1){return 0;}
	double t;if(sscanf(argv[1],"%lf",&t)!=1) return 1;
	printf("%f\n",normalCFD(t));
	return 0;
}
