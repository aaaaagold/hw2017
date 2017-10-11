#!
from __future__ import print_function
import sys
import time as tm
import numpy as np
import datetime as dtm
import matplotlib.pyplot as plt
from geopy.distance import great_circle
from sklearn import linear_model
from sklearn.metrics.pairwise import pairwise_distances
pd_strt=tm.time()
import pandas as pd
pd_ende=tm.time()
print("pandas loading time:",(pd_ende-pd_strt),"sec.")

def calt(foo,*arg,**kwarg):
	t0=tm.time()
	foo(*arg,**kwarg)
	t1=tm.time()
	print("time:",t1-t0,"sec.\n\n")

def caltr(foo,*arg,**kwarg):
	t0=tm.time()
	rtv=foo(*arg,**kwarg)
	t1=tm.time()
	print("time:",t1-t0,"sec.\n\n")
	return rtv

def parseLine(s):
	rtv=s
	if rtv[-2:]=='\r\n': rtv=rtv[:-2]
	if rtv[-1:]==  '\n': rtv=rtv[:-1]
	return rtv

def getHeader(fname):
	with open(fname,'rb') as f:
		return parseLine(f.readline()).split(',')

def readfilePart(head,fname,s,r):
	return pd.read_csv(fname,names=head,header=0,usecols=head,nrows=r,skiprows=s)

def readfile(fname,s=None,r=None):
	h=getHeader(fname)
	return readfilePart(h,fname,s,r)

def checking(fname):
	print('checking:')
	df=readfile(fname)
	print('# station id,name checks:')
	for sel in [['"start station id"','"start station name"'],['"end station id"','"end station name"']]:
		xx=df[sel].groupby(sel).size().reset_index(name='cnt')
		print('## groupby',sel,' count =',xx[['cnt']].count().values[0])
		zz=df[sel[0:1]].groupby(sel[0:1]).size().reset_index(name='cnt')
		print('## groupby',sel[0:1],' count =',zz[['cnt']].count().values[0])
		zz=df[sel[1:2]].groupby(sel[1:2]).size().reset_index(name='cnt')
		print('## groupby',sel[1:2],' count =',zz[['cnt']].count().values[0])
		print('## invalid name =',['','NA','N/A','NULL','Null','null'])
		print('## invalid name counts ',xx[(xx[sel[1:2]]=='')|(xx[sel[1:2]]=='NA')|(xx[sel[1:2]]=='N/A')|(xx[sel[1:2]]=='NULL')|(xx[sel[1:2]]=='Null')|(xx[sel[1:2]]=='null')][['cnt']].count().values[0])
	print('# station long/lat-itude min/max:')
	for sel in ['"start station longitude"','"start station latitude"','"end station longitude"','"end station latitude"']:
		print('## max',df[[sel]].max().values[0])
		print('## min',df[[sel]].min().values[0])
		xx=pd.isnull(df[[sel]])
		print('## count(isnull) ',xx[xx[sel]==True].count().values[0])
	print()
	return df

def preprocess(df,head):
	print('preprocessing')
	#df['_idx']=df.index
	for sel in head[1:3]:
		df[sel]=pd.to_datetime(df[sel])
		#df[sel+'-split48']=df[sel].map(lambda x: [(x.month<<11)+(x.day<<5)+(x.day<<4)+(x.hour<<1)+(x.minute/30),x.dayofweek])
		df[sel+'-split48']=df[sel].map(lambda x: dtm.datetime(x.year,x.month,x.day,x.hour,(x.minute/30)*30))
		df[sel+'-isWeekend']=df[sel].map(lambda x: x.weekday()>4)
	sel_out=[head[3]]+[head[1]+'-split48']
	sel_in =[head[7]]+[head[2]+'-split48']
	o=df[sel_out].groupby(sel_out).size().reset_index(name='out_flow_count')
	o['station_id']=o[sel_out[0]]
	o['time'      ]=o[sel_out[1]]
	i=df[sel_in ].groupby(sel_in ).size().reset_index(name= 'in_flow_count')
	i['station_id']=i[sel_in [0]]
	i['time'      ]=i[sel_in [1]]
	return pd.merge(o,i,how='outer',on=['station_id','time'])[['station_id','time','out_flow_count','in_flow_count']].fillna(0)

def q1(df,head):
	print('q1')
	sOut=pd.DataFrame(data=df[[head[3],head[5],head[ 6]]].groupby([head[3]])[[head[5],head[ 6],]].mean())
	sIn =pd.DataFrame(data=df[[head[7],head[9],head[10]]].groupby([head[7]])[[head[9],head[10],]].mean())
	ss=sOut.join(sIn,how='outer')
	ss['lat' ]=ss[head[ 5]]
	ss['long']=ss[head[ 6]]
	ss['lat' ]=ss['lat' ].fillna(ss[head[ 9]])
	ss['long']=ss['long'].fillna(ss[head[10]])
	print('# number of stations =',ss.groupby(ss.index).size().reset_index(name='cnt')[['cnt']].count().values[0])
	lll=range(len(ss.values))
	'''
	sum=0
	for j in lll:
		for i in lll:
			sum+=great_circle(ss[['lat','long']].values[j],ss[['lat','long']].values[i]).meters
	mean=sum/(len(lll)*len(lll))
	'''
	arr=ss[['lat','long']].values
	mean=sum([sum([great_circle(arr[j],arr[i]).meters for i in lll]) for j in lll])/(len(lll)**2)
	print('# avg(dist(stations)) =',mean)
	print()
	return ss[['lat','long']]

def q2(df,head,n=3):
	print('q2')
	# top freqPair
	print('# freqPair')
	## weekday,weekend
	for isWeekend_d in [0,1]:
		w_dst='weekend' if isWeekend_d else 'weekday'
		for isWeekend_s in [0,1]:
			w_src='weekend' if isWeekend_s else 'weekday'
			print(w_src+' -> '+w_dst)
			print(df[(df[head[1]+'-isWeekend']==isWeekend_s)&(df[head[2]+'-isWeekend']==isWeekend_d)][[head[3],head[7]]].groupby([head[3],head[7]]).size().reset_index(name='pairCount').sort_values('pairCount', ascending=False).head(n))
	print()

def q3(pdf,head,n=3):
	print('q3')
	# top avgFlow
	print('# avgFlow')
	g=pdf.groupby(['station_id'])
	for i in ['out','in']:
		t=g[i+'_flow_count'].agg('sum').reset_index(name=i+'_flow_mean').sort_values(i+'_flow_mean', ascending=False).head(n)
		t[i+'_flow_mean']=t[i+'_flow_mean'].map(lambda x: 1.0*x/(31*48))
		print(t)
	print()

def drawPNG(fname,idx,arri,arro,xtick_chosen,xtick_name,ylabel,title):
	plt.figure(figsize=(64,13))
	plt.xticks(xtick_chosen,xtick_name,rotation=90)
	plt.tick_params(axis='x',labelsize=8)
	plt.plot_date(idx,arri,label= 'in_flow_count',color='green',alpha=0.5,linewidth=2.5,linestyle='-')
	plt.plot_date(idx,arro,label='out_flow_count',color='red'  ,alpha=0.5,linewidth=2.5,linestyle='-')
	plt.legend(loc='best')
	plt.ylabel(ylabel)
	plt.title(title)
	plt.savefig(fname,dpi=64,pad_inches=1)
	print('## output to',fname,'done')

def q4(pdf,head):
	print('q4')
	# get pop
	pop=pd.DataFrame(data={'station_id':pdf['station_id'],'flow_count':pdf['out_flow_count']+pdf['in_flow_count']}).groupby('station_id')
	pop=pop['flow_count'].agg('sum').reset_index(name='flow_mean').sort_values('flow_mean',ascending=False).head(1)
	pop['flow_mean']=pop['flow_mean'].map(lambda x: 1.0*x/(31*48))
	popid=pop['station_id'].values[0]
	print('# id of the most popular station =',popid)
	dts=[dtm.datetime(2017,7,1,0,0)+dtm.timedelta(minutes=i*30) for i in range(31*48)]
	timeitvl=pd.DataFrame({'time':dts})
	popdf=pd.merge(timeitvl,pdf[pdf['station_id']==popid],how='left',on='time').fillna({'station_id':popid,'out_flow_count':0,'in_flow_count':0})
	arri=popdf[ 'in_flow_count'].values
	arro=popdf['out_flow_count'].values
	# line chart:	inf(A), outf(B)
	ts=[t.isoformat(' ') for t in dts]
	idx=range(len(ts))
	'''
	fname='q4a.png'
	plt.figure(figsize=(64,13))
	plt.xticks(idx[0::4],ts,rotation=90)
	plt.tick_params(axis='x',labelsize=8)
	plt.plot_date(idx,arri,label= 'in_flow_count',color='green',alpha=0.5,linewidth=2.5,linestyle='-')
	plt.plot_date(idx,arro,label='out_flow_count',color='red'  ,alpha=0.5,linewidth=2.5,linestyle='-')
	plt.legend(loc='best')
	plt.ylabel('flow count')
	plt.title('in/out flow counts of the most popular station')
	plt.savefig(fname,dpi=64,pad_inches=1)
	print('## output to',fname)
	'''
	i=arri
	o=arro
	drawPNG('q4a.png',idx,i,o,idx[::4],ts,'flow count','in/out flow counts of the most popular station')
	# distFunc(A,B)
	print('# distFunc(A,B)',pairwise_distances([i,o])[1][0])
	# distFunc(A-mean(A),B-mean(B))
	i=arri-arri.mean()
	o=arro-arro.mean()
	print('# dist(A-mean(A),B-mean(B)) =',pairwise_distances([i,o])[1][0])
	drawPNG('q4c.png',idx,i,o,idx[::4],ts,'flow count - mean','"in/out flow counts - mean" of the most popular station')
	# distFunc((A-mean(A))/std(A),(B-mean(B))/std(B))
	i=(arri-arri.mean())/arri.std()
	o=(arro-arro.mean())/arri.std()
	print('# dist((A-mean(A))/std(A),(B-mean(B))/std(B)) =',pairwise_distances([i,o])[1][0])
	drawPNG('q4d.png',idx,i,o,idx[::4],ts,'( flow count - mean ) / std','"(in/out flow counts - mean ) / std" of the most popular station')
	# linearRegression(A,B)
	print('# linearRegression(A,B)')
	# disFunc(smooth(A),smooth(B)) # A(i)=(A(i-1)+A(i)+A(i+1))/3 or more elements
	i=(arri+np.concatenate([[arri[0]],arri[:-1]])+np.concatenate([arri[1:],[arri[-1]]]))/3.0
	o=(arro+np.concatenate([[arro[0]],arro[:-1]])+np.concatenate([arro[1:],[arro[-1]]]))/3.0
	print('# disFunc(smooth(A),smooth(B)) =',pairwise_distances([i,o])[1][0])
	print('## smooth(A_(  i  ))==(A_( i-1 )+A_(  i  )+A_( i+1 ))/3')
	print('## smooth(A_(first))==(A_(first)*2+A_(first+1))/3')
	print('## smooth(A_( last))==(A_( last)*2+A_( last-1))/3')
	drawPNG('q4f.png',idx,i,o,idx[::4],ts,'smooth(flow count)','smooth(in/out flow counts) of the most popular station')
	print()

def queries(df,head):
	sel=[head[3]]+[head[7]]
	#df[sel].groupby(sel).size().reset_index(name='cnt').sort_values(['cnt'], ascending=False).head(3)


if __name__=='__main__':
	if len(sys.argv)!=2:
		print("usage:  "+sys.argv[0]+"  csvFile")
		exit(1)
	'''
	data=caltr(readfile,sys.argv[1])
	head=data.columns.values
	calt(print,data)
	print(head)
	for i in range(len(head)): print(i,head[i])
	'''
	df=caltr(checking,sys.argv[1])
	head=df.columns.values
	
	pdf=caltr(preprocess,df,head)
	phead=pdf.columns.values
	
	# num(stations)
	ss=caltr(q1,df,head)
	
	# top freqPair
	calt(q2,df,head,3)
	
	# top avgFlow
	calt(q3,pdf,head,3)
	
	# pop station
	calt(q4,pdf,head)
	
	print(phead)
	#print(df)
	print(head)

