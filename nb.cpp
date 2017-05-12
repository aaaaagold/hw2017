
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
#ifndef M_SQRT3_10
const double M_SQRT3_10=sqrt(3.0/10);
#endif
#ifndef M_SQRT1_Exp
const double M_SQRT1_Exp=sqrt(1/exp(1));
#endif
#ifndef M_SQRT1_2Pi
const double M_SQRT1_2Pi=sqrt(0.125/atan(1));
#endif
inline double normalCDF(double value){ return erfc(-value*M_SQRT1_2)*0.5; } // N(0,1)
inline double uniformalCDF(double value){ return value<0?0:(value>1?1:value); } // U[0,1]
inline double exponentialCDF(double value){ return value<0?0:(1-exp(-value)); } // lambda=1
inline double uquadraticCDF(double value){ return value<-1?0:(value>1?1:(1+value*value*value)*0.5); } // x in [-1,1], pdf=(3/2)x^2, var=3/10
inline double normalPDF(double value){ return M_SQRT1_2Pi*exp(-value*value*0.5); } // N(0,1)
inline double uniformalPDF(double value){ return value<0?0:(value>1?0:1); } // U[0,1]
inline double exponentialPDF(double value){ return value<0?0:exp(-value); } // lambda=1
inline double uquadraticPDF(double value){ return value*value>1?0:value*value*1.5; } // x in [-1,1], pdf=(3/2)x^2, var=3/10
class cdf //  default: N(0,1)
{
	double mean,var,sd;
	cdftype f;
public:
	inline double normal(double val)const{return normalCDF((val-mean)/sd);} // mean,variance
	inline double unifor(double val)const{return uniformalCDF((val-mean)*M_SQRT1_12/sd+0.5);} // mean,variance
	inline double expone(double val)const{return exponentialCDF((val-mean)/sd+1);} // mean,variance
	inline double uquadr(double val)const{return uquadraticCDF((val-mean)*M_SQRT3_10/sd);} // mean,variance
	inline double dnormal(double val)const{return normalPDF((val-mean)/sd);} // mean,variance
	inline double dunifor(double val)const{return uniformalPDF((val-mean)*M_SQRT1_12/sd+0.5);} // mean,variance
	inline double dexpone(double val)const{return exponentialPDF((val-mean)/sd+1);} // mean,variance
	inline double duquadr(double val)const{return uquadraticPDF((val-mean)*M_SQRT3_10/sd);} // mean,variance
	cdf():f(cdftype::normal){setMV(0,1);}
	cdf(double m,double v,cdftype f=cdftype::normal){setMV(m,v);}
	cdf(const vector<double> &rhs){setBest(rhs);}
	inline void setMean(double m){mean=m;}
	inline void setVar(double v){var=v;sd=sqrt(var);}
	inline void setMV(double m,double v){setMean(m);setVar(v);}
	inline void setType(const cdftype t){f=t;}
	inline cdftype getType()const{return f;}
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
	void setBest(const vector<double> &rhs)
	{
		{
			vector<sortByAbs<double> > tmp(rhs.size()); for(size_t x=tmp.size();x--;) tmp[x]=rhs[x];
			sort(tmp.begin(),tmp.end());
			vector<double> t(tmp.size()); for(size_t x=t.size();x--;) t[x]=tmp[x].val();
			setMean(safeSum(t,0,t.size())/t.size());
			for(size_t x=t.size();x--;) t[x]-=mean;
			setVar(safeSumSqr(t,0,t.size())/t.size());
		}
		vector<double> tmp=rhs; sort(tmp.begin(),tmp.end());
		cdftype c;
		double err=tmp.size();
		for(cdftype t;t!=cdftype::size;t=cdftype(t+1))
		{
			f=t;
			vector<double> tmperr;
			for(size_t x=tmp.size();x--;) tmperr.push_back((double)(x+1)/tmp.size()-p(tmp[x]));
			double e=safeSumSqr(tmperr,0,tmperr.size());
			if(e<err){err=e;c=f;}
			//cout<<"distribution type "<<(int)(f)<<" error: "<<e<<endl;
			//for(size_t x=tmp.size();x--;) cout<<" "<<(double)(x+1)/tmp.size()<<" "<<p(tmp[x])<<endl;
		}
		f=c;
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
	row(){}
	row(const vector<string> &in,const string &out){set(in,out);}
	void set(const vector<string> &in,const string &out){i=in;o=out;}
	size_t isize()const{return i.size();}
	const string &input(size_t n)const{return i[n];}
	const string &output()const{return o;}
};
class nb
{
	vector<cdf> f;
	map<string,vector<row> > cs;
	map<string,vector<cdf> > fs;
	map<string,double> cp;
	size_t total;
public:
	nb(){total=0;};
	nb(const vector<row> &data){reset(data);}
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
		cout<<"global"<<endl;
		for(size_t i=f.size();i--;)
		{
			vector<double> t;
			for(size_t x=0,xs=total;x<xs;x++)
			{
				register double d;
				if(sscanf(data[x].input(i).c_str(),"%lf",&d)==1) t.push_back(d);
			}
			f[i].setBest(t);
		}
		for(auto it=cs.begin();it!=cs.end();it++)
		{
			cout<<"class "<<it->first<<endl;
			vector<row> &DATA=it->second;
			vector<cdf> &F=fs[it->first]=vector<cdf>(f.size());
			for(size_t i=F.size();i--;)
			{
				vector<double> t;
				for(size_t x=0,xs=DATA.size();x<xs;x++)
				{
					register double d;
					if(sscanf(DATA[x].input(i).c_str(),"%lf",&d)==1) t.push_back(d);
				}
				if(t.size()!=1) F[i].setBest(t);
				else F[i].setMean(t[0]);
			}
		}
	}
	string distinct(const row &q)
	{
		//map<string,double> ps;
		string rtv="";
		double rtvp=0;
		for(auto it=fs.begin();it!=fs.end();it++)
		{
			vector<double> p=vector<double>(1,cp[it->first]);
			vector<cdf> &c=it->second;
			for(size_t x=c.size();x--;)
			{
				register double t;
				if(sscanf(q.input(x).c_str(),"%lf",&t)==1) p.push_back(c[x].dp(t));
			}
			{
				register double t=safeSumLog(p,0,p.size());
				if(t>rtvp || rtv==""){ rtvp=t; rtv=it->first; }
			}
		}
		return rtv;
	}
	vector<int> ff()const{vector<int> rtv; for(size_t x=f.size();x--;) rtv.push_back(f[x].getType()); return rtv;}
	void learn_continuous(const vector<double> &n)
	{
	}
};
// */
string i2s(int i)
{
	vector<char> t; if(i<0){t.push_back('-');i=-i;}
	do{t.push_back('0'+(i%10));i/=10;}while(i);
	string rtv;
	while(t.size()){ rtv+=t.back(); t.pop_back(); }
	return rtv;
}
vector<string> parseDataRow(const string &s,const char del)
{
	vector<string> rtv;
	string t;
	for(size_t x=0;s[x];x++)
	{
		if(s[x]==del){ rtv.push_back(t); t=""; }
		else t+=s[x];
	}
	rtv.push_back(t);
	return rtv;
}
vector<row> parseData(const string &fname,const char del)
{
	ifstream iii(fname,ios::binary);
	vector<row> rtv;
	for(string s;getline(iii,s);)
	{
		vector<string> v;
		string t;
		for(size_t x=0;s[x];x++)
		{
			if(s[x]==del){ v.push_back(t); t=""; }
			else t+=s[x];
		}
		if(t.back()<32) t.pop_back();
		rtv.push_back(row(v,t));
	}
	cout<<"row: "<<rtv.size()<<endl;
	return rtv;
}
vector<row> parseData_ml(const string &fname,const char del,const map<string,string > &convert,const string &mapTo=".")
{
	ifstream iii(fname,ios::binary);
	vector<row> rtv;
	for(string s;getline(iii,s);)
	{
		vector<string> v;
		string c="",t;
		for(size_t x=0;s[x];x++)
		{
			if(s[x]==del){ if(c=="") c=t; else v.push_back(t); t=""; }
			else t+=s[x];
		}
		if(t.back()<32) t.pop_back();
		v.push_back(t); auto it=convert.find(c);
		/* /
		cout<<c<<endl;
		{
			int sd=1,sm=1,sy=1; sscanf(c.c_str(),"D-%d/%d/%d",&sd,&sm,&sy);
			int x=((sy*12)+sm-1)*31+sd-1;
			int d=(x)%31+1,m=(x/31)%12+1,y=x/31/12;
			string t="D-"; t+=i2s(d); t+='/'; t+=i2s(m); t+='/'; t+=i2s(y);
			cout<<"get "<<t<<"$"<<endl;
		}
		// */
		if(it==convert.end()){ if(mapTo!="") continue; }else c=it->second;
		//cout<<"known as "<<c<<endl;
		rtv.push_back(row(v,c));
	}
	cout<<"row: "<<rtv.size()<<endl;
	return rtv;
}

int test0(const int argc,const char *argv[]);
int test1(const int argc,const char *argv[]);
int test2(const int argc,const char *argv[]);
map<string,string> parseAttr_ml(const string &fname);

int main(const int argc,const char *argv[])
{
	return test2(argc,argv);
	return 0;
}

int test0(const int argc,const char *argv[])
{
	if(argc==1){cout<<"usage: "<<argv[0]<<"  value"<<endl;return 0;}
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
int test1(const int argc,const char *argv[])
{
	if(argc<3){cout<<"usage: "<<argv[0]<<"  delim  file  [query]"<<endl;return 0;}
	nb xd;
	xd.reset(parseData(argv[2],argv[1][0]));
	vector<int> a=xd.ff();
	for(size_t x=a.size();x--;)cout<<a[x]<<endl;
	for(int x=3;x<argc;x++) cout<<argv[x]<<" -> "<<xd.distinct(row(parseDataRow(argv[x],argv[1][0]),""))<<endl;
	return 0;
}
int test2(const int argc,const char *argv[])
{
	if(argc<5){cout<<"usage: "<<argv[0]<<"  delim  file-train  file-test  attr"<<endl;return 0;}
	nb xd;
	map<string,string> conv=parseAttr_ml(argv[4]);
	xd.reset(parseData_ml(argv[2],argv[1][0],conv));
	vector<int> a=xd.ff();
	for(size_t x=a.size();x--;)cout<<a[x]<<endl;
	vector<row> test=parseData_ml(argv[3],argv[1][0],conv,"");
	for(size_t x=0,xs=test.size();x<xs;x++) cout<<test[x].output()<<" "<<xd.distinct(test[x])<<endl;
	return 0;
}

vector<string> parseAttr_ml_expand_to(const string &src,const string &dst)
{
	int sd=1,sm=1,sy=1; sscanf(src.c_str(),"D-%d/%d/%d",&sd,&sm,&sy);
	int dd=1,dm=1,dy=1; sscanf(dst.c_str(),"D-%d/%d/%d",&dd,&dm,&dy);
	int s=((sy*12)+sm-1)*31+sd-1;
	int f=((dy*12)+dm-1)*31+dd-1;
	vector<string> rtv;
	for(int x=s;x<=f;x++)
	{
		int d=(x)%31+1,m=(x/31)%12+1,y=x/31/12;
		string t="D-"; t+=i2s(d); t+='/'; t+=i2s(m); t+='/'; t+=i2s(y);
		rtv.push_back(t);
	}
	return rtv;
}
map<string,string> parseAttr_ml(const string &fname)
{
	ifstream iii(fname,ios::binary);
	vector<string> v;{string t;while(iii>>t)v.push_back(t);}iii.close();
	map<string,vector<string> >rtv_;
	enum states{notyet,strt,parsing};
	states stat=states::notyet;
	string tmpc;
	for(size_t x=0,xs=v.size();x<xs;x++)
	{
		switch(stat)
		{
		case states::notyet:
			if(x+1<xs && v[x]=="--"&&v[x+1]=="Class"){tmpc=v[x+2];rtv_[tmpc]=vector<string>(0);stat=states::strt;}
			break;
		case states::strt:
		{
			int d,m,y;
			if(sscanf(v[x].c_str(),"D-%d/%d/%d",&d,&m,&y)==3){x--;stat=states::parsing;
				//cout<<"S "<<v[x]<<endl;
			}
		}
			break;
		case states::parsing:
		{
			//cout<<"P "<<v[x]<<endl;
			int d,m,y;
			if(sscanf(v[x].c_str(),"D-%d/%d/%d",&d,&m,&y)!=3){x--;stat=states::notyet;
				//cout<<" bye"<<endl;
			}else{
				bool single=(x+1>=xs || v[x+1]!="to");
				vector<string> tmp=parseAttr_ml_expand_to(v[x],single?v[x]:v[x+2]);
				if(!single) x+=2;
				for(size_t z=0,zs=tmp.size();z<zs;z++) rtv_[tmpc].push_back(tmp[z]);
			}
		}
			break;
		default:
			break;
		}
	}
	map<string,string> rtv;
	for(auto it=rtv_.begin();it!=rtv_.end();it++)
	{
		vector<string> &sv=it->second;
		for(size_t x=sv.size();x--;) rtv[sv[x] ]=it->first;
	}
	return rtv;
}

