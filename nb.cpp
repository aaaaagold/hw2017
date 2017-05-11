
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

template<class T>
inline T ori(const T n){return n;}
template<class T>
class sortByAbs
{
	T v;
public:
	T &val(){return v;}
	const T &val()const{return v;}
	sortByAbs &operator=(const T&rhs){v=rhs;return *this;}
	bool operator<(const sortByAbs &rhs)const{return abs(v)<abs(rhs.v);}
};

double safeOp(const vector<double> &rhs,size_t b,size_t e,double (*f)(const double)=NULL )
{
	double rtv=0;
	if(b>=e||f==NULL){ return 0; }
	vector<sortByAbs<double> > tmp(e-b);
	for(size_t x=b;x<e;x++) tmp[x-b]=f(rhs[x]);
	sort(tmp.begin(),tmp.end());
	for(size_t x=b;x<e;x++) rtv+=tmp[x-b].val();
	return rtv;
}
inline double safeSumLog(const vector<double> &rhs,size_t b,size_t e)
{
	return safeOp(rhs,b,e,log);
}
inline double safeSum(const vector<double> &rhs,size_t b,size_t e)
{
	return safeOp(rhs,b,e,ori);
}

enum cdftype{normal,uniform,exponent,uqu};

#ifndef M_SQRT1_2
const double M_SQRT1_2=sqrt(0.5);
#endif
#ifndef M_SQRT1_12
const double M_SQRT1_12=sqrt(1.0/12);
#endif
inline double normalCDF(double value){ return 0.5*erfc(-value*M_SQRT1_2); } // N(0,1)
inline double exponentialCDF(double value){ return 1-exp(-value); } // lambda=1
inline double uniformalCDF(double value){ return 1; } // U[0,1]
class cdf
{
	double mean,var,sd;
	cdftype f;
	inline void setMean(double m){mean=m;}
	inline void setVar(double v){var=v;sd=sqrt(var);}
public:
	inline double normal(double val)const{return normalCDF((val-mean)/sd);} // mean,variance
	inline double unifor(double val)const{return uniformalCDF(val)/(M_SQRT1_12/sd);} // mean,variance
	inline double expone(double val)const{return exponentialCDF(val/mean);} // mean
	cdf():f(cdftype::normal){setMV(0,1);}
	cdf(double m,double v,cdftype f=cdftype::normal){setMV(m,v);}
	inline void setMV(double m,double v){setMean(m);setVar(v);}
	inline void setType(const cdftype t){f=t;}
	double p(double v)const
	{
		double rtv=0;
		switch(f)
		{
		default: break;
		case cdftype::normal: rtv=normal(v); break;
		case cdftype::uniform: rtv=unif(v); break;
		case cdftype::exponent: rtv=expone(v); break;
		case cdftype::uqu: rtv=normal(v); break;
		}
		return rtv;
	}
};

template<class I,class O>
class row
{
	vector<I> i;
	O o;
public:
	void set(const vector<I> &in,const O out){i=in;o=out;}
};
template<class I,class O>
class nb // naiveBayesian
{
	vector<cdf> f;
public:
	nb(){};
	nb(const vector<row<I,O> > &data){reset(data);}
	void reset(const vector<row<I,O> > &data)
	{
		
	}
};

int main(const int argc,const char *argv[])
{
	if(argc==1){return 0;}
	double t;if(sscanf(argv[1],"%lf",&t)!=1) return 1;
	printf("%f\n",normalCFD(t));
	vector<double> tmp(2,t);
	printf("%f\n",safeOp(tmp,0,2,log));
	return 0;
}

