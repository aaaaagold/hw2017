
// c++11

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
inline T sqr(const T n){return n*n;}
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
inline double safeSumSqr(const vector<double> &rhs,size_t b,size_t e)
{
	return safeOp(rhs,b,e,sqr);
}

enum cdftype{normal,uniform,exponent,uqua,size};
#ifndef M_SQRT1_2
const double M_SQRT1_2=sqrt(0.5);
#endif
#ifndef M_SQRT1_12
const double M_SQRT1_12=sqrt(1.0/12);
#endif
#ifndef M_SQRT3_10
const double M_SQRT3_10=sqrt(3.0/10);
#endif
inline double normalCDF(double value){ return 0.5*erfc(-value*M_SQRT1_2); } // N(0,1)
inline double uniformalCDF(double value){ return 1; } // U[0,1]
inline double exponentialCDF(double value){ return 1-exp(-value); } // lambda=1
inline double uquadraticCDF(double value){ return 0.5*(1+value*value*value); } // x in [-1,1], pdf=(3/2)x^2, var=3/10
class cdf
{
	double mean,var,sd;
	cdftype f;
	class typeit
	{
		cdftype f;
	public:
		typeit():f(cdftype::normal){}
		void setFirst(){f=cdftype::normal;}
		bool isEnd const(){return f==cdftype::size;}
		cdftype getVal const(){return f;}
		cdftype &operator++(){return f=cdftype(f+1);} // ++f
		cdftype operator++(int){cdftype r=f;f=cdftype(f+1);return r;} // f++
	};
	inline void setMean(double m){mean=m;}
	inline void setVar(double v){var=v;sd=sqrt(var);}
public:
	inline double normal(double val)const{return normalCDF((val-mean)/sd);} // mean,variance
	inline double unifor(double val)const{return (M_SQRT1_12/sd);} // variance
	inline double expone(double val)const{return exponentialCDF(val/mean);} // mean
	inline double uquadr(double val)const{return uquadraticCDF((val-mean)*M_SQRT3_10/sd);} // mean,variance
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
		case cdftype::uniform: rtv=unifor(v); break;
		case cdftype::exponent: rtv=expone(v); break;
		case cdftype::uqua: rtv=uquadr(v); break;
		}
		return rtv;
	}
	void setBest(const vector<double> &rhs)
	{
		vector<sortByAbs<double> > tmp(rhs.size()); for(size_t x=rhs.size();x--;) tmp[x]=rhs[x];
		sort(tmp.begin(),tmp.end());
		mean=safeSum(tmp)/tmp.size();
		var=safeSumSqr(tmp)/tmp.size();
		cdftype c=cdftype::normal;
		double err=1e9;
		for(typeit t;t!=isEnd();t++)
		{
			f=t.getVal();
			vector<double> tmperr;
			for(size_t x=tmp.size();x--;)
			{
				double d=lower_bound(tmp.begin(),tmp.end(),tmp[x])-tmp.begin();
				d/=tmp.size();
				tmperr.push_back(d-p(tmp[x]));
			}
		}
	}
};

/* /
template<class I,class O>
class row
{
	vector<I> i;
	O o;
public:
	void set(const vector<I> &in,const O out){i=in;o=out;}
	size_t size(){return i.size();}
	const I &input(size_t n)const{return i[n];}
	const O &output()const{return o;}
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
	void learn_continuous(const vector<double> &n)
	{
	}
};
// */
// suppose <string,...> => string
class row
{
	vector<string> i;
	string o;
public:
	void set(const vector<string> &in,const string &out){i=in;o=out;}
	size_t isize()const{return i.size();}
	const string &input(size_t n)const{return i[n];}
	const string &output()const{return o;}
};
class nb
{
	vector<cdf> f;
	map<string,vector<row> > cs;
public:
	nb(){};
	nb(const vector<row> &data){reset(data);}
	void reset(const vector<row> &data)
	{
		cs.clear();
		if(data.size()) f.resize(data[0].isize()); else return;
		for(size_t x=0,xs=data.size();x<xs;x++)
		{
			string t=data[x].output();
			if(cs.find(t)!=cs.end()) cs[t].push_back(data[x]);
			else cs[t]=vector<row>(1,data[x]);
		}
		for(size_t i=0,is=f.size();i<is;i++)
		{
			vector<double> t;
			for(size_t x=0,xs=data.size();x<xs;x++)
			{
				register double d;
				if(sscanf(data[x].input(i).c_str(),"%lf",&d)==1) t.push_back(d);
			}
			f[i].setBest(t);
		}
	}
	void learn_continuous(const vector<double> &n)
	{
		
	}
};
// */

int main(const int argc,const char *argv[])
{
	if(argc==1){return 0;}
	double t;if(sscanf(argv[1],"%lf",&t)!=1) return 1;
	printf("%f\n", normalCDF(t));
	printf("%f\n", uniformalCDF(t));
	printf("%f\n", exponentialCDF(t));
	printf("%f\n", uquadraticCDF(t));
	vector<double> tmp(2,t);
	printf("%f\n",safeOp(tmp,0,2,log));
	printf("%d\n",int(sizeof(cdftype)));
	return 0;
}

