
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
	tmp.print();
	return 0;
}
