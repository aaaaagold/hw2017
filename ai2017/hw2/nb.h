#ifndef __nb_h____
#define __nb_h____

// c++11
/*
P(Y=y|X=x)
=P(Y=y,X=x)/P(X=x)
=P(Y=y)P(X=x|Y=y)/P(X=x)
=(P(Y=y) **P(Xi=xi|Y=y)** )/P(X=x)
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

template<class T>
inline T ori(const T n){return n;}
template<class T>
inline T sqr(const T n){return n*n;}
template<class T>
inline T log16(const T n){return log2(n)*0.25;}
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
inline double safeSumAbs(const vector<double> &rhs,size_t b,size_t e)
{
	return safeOp(rhs,b,e,abs);
}

enum cdftype{normal,uniform,exponent,uqua,size};
#ifndef M_SQRT1_2
const double M_SQRT1_2=sqrt(0.5);
#endif
#ifndef M_SQRT1_12
const double M_SQRT1_12=sqrt(1.0/12);
#endif
#ifndef M_SQRT3_5
const double M_SQRT3_5=sqrt(3.0/5);
#endif
#ifndef M_SQRT1_Exp
const double M_SQRT1_Exp=sqrt(1/exp(1));
#endif
#ifndef M_SQRT1_2Pi
const double M_SQRT1_2Pi=sqrt(0.125/atan(1));
#endif
const double veryLarge=32;
const double verySmall=1/veryLarge;
const double verySmall_minus1=verySmall-1;
inline double smallp(double value){return exp(-value*veryLarge);} // [0,inf)
inline double normalCDF(double value){ return erfc(-value*M_SQRT1_2)*0.5; } // N(0,1)
inline double uniformalCDF(double value){ return value<0?0:(value>1?1:value); } // U[0,1]
inline double exponentialCDF(double value){ return value<0?0:(1-exp(-value)); } // lambda=1
inline double uquadraticCDF(double value){ return value<-1?0:(value>1?1:(1+value*value*value)*0.5); } // x in [-1,1], pdf=(3/2)x^2, var=3/10
inline double normalPDF(double value){ return M_SQRT1_2Pi*exp(-value*value*0.5); } // N(0,1)
inline double exponentialPDF(double value){ return value<0?smallp(-value):exp(-value)*(-verySmall_minus1); } // lambda=1
class cdf //  default: N(0,1)
{
	double mean,var,sd;
	cdftype f;
	bool _isDiscrete;
	map<string,size_t> cc; // classes,counts
	size_t total;
public:
	inline double normal(double val)const{return normalCDF((val-mean)/sd);} // mean,variance
	inline double unifor(double val)const{return uniformalCDF((val-mean)*M_SQRT1_12/sd+0.5);} // mean,variance
	inline double expone(double val)const{return exponentialCDF((val-mean)+sd);} // mean,variance
	inline double uquadr(double val)const{return uquadraticCDF((val-mean)*M_SQRT3_5/sd);} // mean,variance
	inline double dnormal(double val)const{return normalPDF((val-mean)/sd)/sd;} // mean,variance
	inline double dunifor(double val)const
	{
		double t=M_SQRT1_12/sd,v=(val-mean)*t;
		//cout<<t/(verySmall*2+1)<<endl;
		return t/(verySmall*2+1);
		return ((v*v>0.25)?smallp(abs(v)-0.5):1)*t/(verySmall*2+1);
	} // mean,variance
	inline double dexpone(double val)const{return exponentialPDF((val-mean)+sd)*sd;} // mean,variance
	inline double duquadr(double val)const
	{
		double t=M_SQRT3_5/sd,v=(val-mean)*t;
		double sd3=sd*sd*sd;
		return ((v*v>1)?smallp(abs(v)-1):(v*v*(-verySmall_minus1)+verySmall))*t*1.5/(1+verySmall*5);
		// ?3k/t:(1+2k)/t
		return ((v*v>1)?(smallp(abs(v)-1)*1.5):((v*v*(-verySmall_minus1)+verySmall)*1.5))
			*t/(1+verySmall*5);
	} // mean,variance
	cdf():f(cdftype::normal){setMV(0,1);}
	cdf(double m,double v,cdftype cf=cdftype::normal){f=cf;setMV(m,v);}
	cdf(const vector<double> &rhs){setBest(rhs);}
	cdf(const vector<string> &classv,const vector<string> &sample){setDiscrete(classv,sample);}
	void setDiscrete(const vector<string> &classv,const vector<string> &sample,size_t reWeightOnZero=1)
	{
		_isDiscrete=1;
		for(int x=0,xs=classv.size();x<xs;x++) cc[classv[x]]=0;
		total=0;
		for(int x=0,xs=sample.size();x<xs;x++){ auto it=cc.find(sample[x]); if(it!=cc.end()){ it->second+=1; total++; } }
		bool haveZero=0; for(auto it=cc.begin();it!=cc.end();it++) if(it->second==0){ haveZero=1; break; }
		if(haveZero){ total+=classv.size()*reWeightOnZero; for(auto it=cc.begin();it!=cc.end();it++) it->second+=reWeightOnZero; }
	}
	double dp_discrete(const string &c)const
	{
		auto it=cc.find(c);
		return it!=cc.end()?1.0*(cc.find(c)->second)/total:1;
	}
	double dp(const string &c)const
	{
		if(isDiscrete()) return dp_discrete(c);
		else{ double t; if(sscanf(c.c_str(),"%lf",&t)==1) return dp(t); }
		return 1;
	}
	inline void setMean(double m){mean=m;_isDiscrete=0;}
	inline double getMean()const{return mean;}
	inline void setVar(double v){var=v;sd=sqrt(var);_isDiscrete=0;}
	inline double getVar()const{return var;}
	inline void setMV(double m,double v){setMean(m);setVar(v);}
	inline void setType(const cdftype t){f=t;}
	inline cdftype getType()const{return f;}
	inline bool isDiscrete()const{return _isDiscrete;}
	inline bool validq(const string &q)const
	{
		double t;
		return isDiscrete()?(cc.find(q)!=cc.end()):(sscanf(q.c_str(),"%lf",&t)==1);
	}
	double dp(double v)const
	{
		double rtv=0;
		switch(f)
		{
		default: break;
		case cdftype::normal: rtv=dnormal(v); break;
		case cdftype::uniform: rtv=dunifor(v); break;
		case cdftype::exponent: rtv=dexpone(v); break;
		case cdftype::uqua: rtv=duquadr(v); break;
		}
		return rtv;
	}
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
	void setBest(const vector<double> &rhs,const vector<cdftype> &omit=vector<cdftype>(0))
	{
		_isDiscrete=0;
		cc.clear();
		{
			vector<sortByAbs<double> > tmp(rhs.size()); for(size_t x=tmp.size();x--;) tmp[x]=rhs[x];
			sort(tmp.begin(),tmp.end());
			vector<double> t(tmp.size()); for(size_t x=t.size();x--;) t[x]=tmp[x].val();
			setMean(safeSum(t,0,t.size())/t.size());
			for(size_t x=t.size();x--;) t[x]-=mean;
			setVar(safeSumSqr(t,0,t.size())/t.size());
		}
		//return; // limited in normal distribution
		vector<double> tmp=rhs; sort(tmp.begin(),tmp.end());
		cdftype c=(cdftype)(0);
		double err=tmp.size();
		for(cdftype t=(cdftype)(0);t!=cdftype::size;t=cdftype(t+1))
		{
			{ bool f=0; for(int x=omit.size();x--;)if(t==omit[x]){f=1; break;} if(f)continue; }
			f=t;
			vector<double> tmperr;
			for(size_t x=tmp.size();x--;) tmperr.push_back((double)(x+1)/tmp.size()-p(tmp[x]));
			double e=safeSumSqr(tmperr,0,tmperr.size());
			if(e<err){err=e;c=f;}
			// if(t==cdftype::normal && tmperr.size()<11) break;
		}
		f=c;
	}
	string info()const
	{
		string rtv="isDiscrete:";
		rtv+=isDiscrete()?"1":"0";
		if(isDiscrete())
		{
			stringstream ss;
			string s;
			ss<<total; ss>>s;
			rtv+=" "; rtv+=s;
			for(auto it=cc.begin();it!=cc.end();it++)
			{
				stringstream ss;
				rtv+=" "; rtv+=it->first; rtv+=":";
				ss<<(it->second); ss>>s;
				rtv+=s;
			}
		}else{
			rtv+=" type:";
			rtv+=('0'+getType());
			stringstream ss;
			string s;
			ss<<"m:"<<getMean()<<" v:"<<getVar();
			rtv+=" "; ss>>s; rtv+=s;
			rtv+=" "; ss>>s; rtv+=s;
		}
		rtv+=" ";
		return rtv;
	}
};

class nb
{
	vector<cdf> f;
	map<string,vector<row> > cs; // rows in a class
	map<string,vector<cdf> > fs; // cdf each attr of a class
	map<string,double> cp; // P(Class=c)
	size_t total;
public:
	nb(){total=0;}
	nb(const vector<row> &data){reset(data);}
	nb(const vector<row> &data,const dataFormat &head){reset(data,head);}
	void reset(const vector<row> &data,const dataFormat &head)
	{
		cs.clear();
		size_t attrSize=head.iv.size();
		size_t attrContCnt=0; for(size_t x=head.iv.size();x--;) attrContCnt+=head.iv[x].isNumber();
		vector<cdftype> omit; // if(attrContCnt>9){omit.push_back(cdftype::uqua);omit.push_back(exponent);}
		total=0;
		// set cs
		for(size_t x=head.o.size();x--;) cs[head.o[x] ].resize(0); // create entry
		for(size_t x=data.size();x--;)
		{
			auto it=cs.find(data[x].output());
			if(it!=cs.end())
			{
				total++;
				it->second.push_back(data[x]);
			}
		}
		// set cp
		{
			double t=total; t=1/t;
			for(auto it=cs.begin();it!=cs.end();it++) cp[it->first]=it->second.size()*t;
		}
		// set fs
		for(auto it=cs.begin();it!=cs.end();it++)
		{
			vector<row> &DATA=it->second;
			vector<cdf> &F=fs[it->first]=vector<cdf>(attrSize);
			for(size_t i=F.size();i--;)
			{
				// set F
				if(head.iv[i].isNumber())
				{
					// continuous
					vector<double> t;
					for(size_t x=0,xs=DATA.size();x<xs;x++)
					{
						double d;
						if(sscanf(DATA[x].input(i).c_str(),"%lf",&d)==1) t.push_back(d);
					}
					// set F
					if(t.size()!=1) F[i].setBest(t,omit);
					else F[i].setMean(t[0]);
	//cout<<" c:"<<(it->first)<<" idx:"<<i<<" m:"<<F[i].getMean()<<" v:"<<F[i].getVar()<<" s:"<<t.size()<<endl; // debug
				}else{
					// discrete
					vector<string> t;
					for(size_t x=0,xs=DATA.size();x<xs;x++) t.push_back(DATA[x].input(i));
					// set F
					F[i].setDiscrete(head.iv[i].getinfo(),t);
				}
			}
		}
	}
	void reset(const vector<row> &data)
	{
		cs.clear();
		if(total=data.size()) f.resize(data[0].isize()); else return;
		{
			map<string,size_t> tcp;
			for(size_t x=total;x--;)
			{
				string t=data[x].output();
				if(tcp.find(t)!=tcp.end()){ tcp[t]++; cs[t].push_back(data[x]); }
				else{ tcp[t]=1; cs[t]=vector<row>(1,data[x]); }
			}
			for(auto it=tcp.begin();it!=tcp.end();it++) cp[it->first]=(double)(it->second)/total;
		}
		for(size_t i=f.size();i--;)
		{
			vector<double> t;
			for(size_t x=0,xs=total;x<xs;x++)
			{
				double d;
				if(sscanf(data[x].input(i).c_str(),"%lf",&d)==1) t.push_back(d);
			}
			f[i].setBest(t);
		}
		for(auto it=cs.begin();it!=cs.end();it++)
		{
			vector<row> &DATA=it->second;
			vector<cdf> &F=fs[it->first]=vector<cdf>(f.size());
			for(size_t i=F.size();i--;)
			{
				vector<double> t;
				for(size_t x=0,xs=DATA.size();x<xs;x++)
				{
					double d;
					if(sscanf(DATA[x].input(i).c_str(),"%lf",&d)==1) t.push_back(d);
				}
				if(t.size()!=1) F[i].setBest(t);
				else F[i].setMean(t[0]);
			}
		}
	}
	string distinguish(const row &q,bool printClasses=false)const
	{
		if(printClasses) cout<<q.output()<<endl;
		string rtv="";
		double rtvp=0;
	//vector<double> record; // debug
		for(auto it=fs.begin();it!=fs.end();it++)
		{
			vector<double> p=vector<double>(1,cp.find(it->first)->second);
			const vector<cdf> &c=it->second;
			for(size_t x=c.size();x--;) if(c[x].validq(q.input(x))){
				if(c[x].isDiscrete() || c[x].getVar()!=0) p.push_back(c[x].dp(q.input(x)));
				else{
					double t; sscanf(q.input(x).c_str(),"%lf",&t);
					p.push_back((t==c[x].getMean()?cs.find(it->first)->second.size():1)*1.0/(cs.find(it->first)->second.size()+1));
				}
	//if(p.back()==0) record.push_back(p.back()); // debug
	//if(p.back()==0) cout<<q.input(x)<<" "<<(c[x].isDiscrete() || c[x].getVar()!=0)<<" "<<c[x].dp(q.input(x))<<" "<<c[x].info()<<endl; // debug
			}
			double t=safeSumLog(p,0,p.size());
			if(printClasses) cout<<" "<<it->first<<" "<<t<<endl;
			if(t>rtvp || rtv==""){ rtvp=t; rtv=it->first; }
	//record.push_back(-1); // debug
		}
	//if(1/rtvp==0){ for(size_t x=0,xs=record.size();x<xs;x++) cout<<" "<<record[x]; cout<<endl; } // debug
		return rtv;
	}
	void printcp()const // debug
	{
		cout<<"cp"<<endl;
		size_t maxClassLen=0;
		{
			for(auto it=cp.begin();it!=cp.end();it++) if(maxClassLen<it->first.size()) maxClassLen=it->first.size();
			maxClassLen++;
		}
		size_t maxCntLen=0;
		{
			string s;
			for(auto it=cs.begin();it!=cs.end();it++)
			{
				stringstream ss; ss<<(it->second.size()); ss>>s;
				if(maxCntLen<s.size()) maxCntLen=s.size();
			}
		}
		for(auto it=cp.begin();it!=cp.end();it++)
			cout<<" "<<it->first<<setw(maxClassLen-(it->first.size()))
				<<" "<<setw(maxCntLen)<<(cs.find(it->first)->second.size())
				<<" "<<it->second
				<<endl;
	}
	void printfs()const // debug
	{
		cout<<"fs"<<endl;
		for(auto it=fs.begin();it!=fs.end();it++)
		{
			cout<<" "<<it->first<<endl;
			const vector<cdf> &t=it->second;
			for(size_t x=0,xs=t.size();x<xs;x++) cout<<"  "<<setw(2)<<x<<" "<<t[x].info()<<endl;
		}
	}
	void printcssize()const // debug
	{
		cout<<"cssize"<<endl;
		for(auto it=cs.begin();it!=cs.end();it++) cout<<" "<<it->first<<" "<<it->second.size()<<endl;
	}
	vector<cdf> getCDFByClass(const string &c)
	{
		auto it=fs.find(c);
		return it==fs.end()?vector<cdf>(0):it->second;
	}
};

class itvl // interval
{
public:
	double b,e;
	itvl(){}
	itvl(double begi,double ende){reset(begi,ende);}
	void reset(double begi,double ende){b=begi;e=ende;}
};
class itvls
{
	vector<itvl> v;
public:
	itvls(){}
	itvls(const vector<double> &rhs){reset(rhs);}
	void reset(const vector<double> &rhs)
	{
		double mid,min,max;
		{
			vector<double> tmp=rhs; sort(tmp.begin(),tmp.end());
			mid=((tmp.size()&1)==0)?(tmp[tmp.size()>>1]+tmp[(tmp.size()>>1)+1])*0.5:tmp[tmp.size()>>1];
			min=tmp[0];
			max=tmp.back();
		}
		double mean=safeSum(rhs,0,rhs.size())/rhs.size();
		vector<double> shf(rhs.size()); for(size_t x=shf.size();x--;) shf[x]=rhs[x]-mean;
		double var=safeSumSqr(shf,0,shf.size())/shf.size();
		if(var<=0) return;
		double sd=sqrt(var);
		double len=sd;
		double base=mid+len/2;
		int b=floor((min-mid)/len)-11;
		int e=ceil((max-mid)/len)+11;
		vector<double> cp; // cut point
		for(int x=b;x<e;x++) cp.push_back(base+x*len);
		vector<size_t> cnt(cp.size(),0); // [cut point,next cut point)
		for(size_t x=rhs.size();x--;) cnt[(upper_bound(cp.begin(),cp.end(),rhs[x])-cp.begin())-1]++;
		vector<double> newcp;
		bool zerostrt=0;
		size_t x=0,xs=cnt.size();
		for(;x<xs;x++) if(cnt[x]) break; else newcp.push_back(cp[x]);
		size_t qq=x;
		for(;x<xs;x++) if(cnt[x]) newcp.push_back(cp[x]);
		for(;x--;) if(cnt[x]) break;
		for(x++;qq<x&&x<xs;x++) newcp.push_back(cp[x]);
		for(size_t x=1,xs=newcp.size();x<xs;x++) v.push_back(itvl(newcp[x-1],newcp[x]));
		v.push_back(itvl(newcp.back(),base+e*len));
	}
	const vector<itvl> &getinfo()const{return v;}
	alldata mapto(const alldata &rhs,const vector<size_t> &attrs)const
	{
		
	}
};

#endif
