#!
from __future__ import division
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

def sb(s,e='UTF-8'):
	try:
		rtv=bytes(s,e)
	except:
		rtv=bytes(s)
	
	return rtv

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
		return parseLine(f.readline()).split(sb(','))

def readfilePart(fname,s,r):
	return pd.read_csv(fname,header=0,nrows=r,skiprows=s)

def readfile(fname,s=None,r=None):
	return readfilePart(fname,s,r)

def checking(fname):
	print('checking:')
	df=readfile(fname)
	head=df.columns.values
	print('# station id,name checks:')
	for sel in [[head[3],head[4]],[head[7],head[8]]]:
		xx=df[sel].groupby(sel).size().reset_index(name='cnt')
		print('## groupby',sel,' count =',xx[['cnt']].count().values[0])
		zz=df[sel[0:1]].groupby(sel[0:1]).size().reset_index(name='cnt')
		print('## groupby',sel[0:1],' count =',zz[['cnt']].count().values[0])
		zz=df[sel[1:2]].groupby(sel[1:2]).size().reset_index(name='cnt')
		print('## groupby',sel[1:2],' count =',zz[['cnt']].count().values[0])
		print('## invalid name =',['','NA','N/A','NULL','Null','null'])
		print('## invalid name counts ',xx[(xx[sel[1]]=='')|(xx[sel[1]]=='NA')|(xx[sel[1]]=='N/A')|(xx[sel[1]]=='NULL')|(xx[sel[1]]=='Null')|(xx[sel[1]]=='null')][['cnt']].count().values[0])
	print('# station long/lat-itude min/max:')
	for sel in [head[5],head[6],head[9],head[10]]:
		print('## max',df[sel].max())
		print('## min',df[sel].min())
		xx=pd.isnull(df[[sel]])
		print('## count(isnull) ',xx[xx[sel]==True].count().values[0])
	print()
	return df

def preprocess(df,head):
	print('preprocessing')
	#df['_idx']=df.index
	for sel in head[1:3]:
		df[sel]=pd.to_datetime(df[sel])
		#df[sel+'-split48']=df[sel].map(lambda x: [(x.month<<11)+(x.day<<5)+(x.day<<4)+(x.hour<<1)+(x.minute//30),x.dayofweek])
		df[sel+'-split48']=df[sel].map(lambda x: dtm.datetime(x.year,x.month,x.day,x.hour,(x.minute//30)*30))
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

def getStationNameAndPosition(df,head):
	print("preparing [ 'id' , 'name' , 'lat' , 'long' ] ...")
	soid=df[[head[3],head[4],]].sort_values(head[3], ascending=True).drop_duplicates(subset=[head[4]],keep='first',inplace=False).set_index(head[3])
	siid=df[[head[7],head[8],]].sort_values(head[7], ascending=True).drop_duplicates(subset=[head[8]],keep='first',inplace=False).set_index(head[7])
	sOut=pd.DataFrame(data=df[[head[3],head[5],head[ 6],]].groupby([head[3]])[[head[5],head[ 6],]].mean())
	sIn =pd.DataFrame(data=df[[head[7],head[9],head[10],]].groupby([head[7]])[[head[9],head[10],]].mean())
	sn=soid.join(siid,how='outer')
	sp=sOut.join(sIn ,how='outer')
	sn['name']=sn[head[ 4]]
	sn['name']=sn['name'].fillna(sn[head[ 8]])
	sp['lat' ]=sp[head[ 5]]
	sp['lat' ]=sp['lat' ].fillna(sp[head[ 9]])
	sp['long']=sp[head[ 6]]
	sp['long']=sp['long'].fillna(sp[head[10]])
	rtv=sn[['name',]].join(sp[['lat','long',]],how='inner')
	print('# number of stations =',sp.groupby(sp.index).size().reset_index(name='cnt')[['cnt']].count().values[0])
	lll=range(len(sp.values))
	'''
	sum=0
	for j in lll:
		for i in lll:
			sum+=great_circle(sp[['lat','long']].values[j],sp[['lat','long']].values[i]).meters
	mean=sum/(len(lll)*len(lll))
	'''
	arr=sp[['lat','long']].values
	mean=sum([sum([great_circle(arr[j],arr[i]).meters for i in lll]) for j in lll])/(len(lll)**2)
	print('# avg(dist(stations)) =',mean)
	print()
	rtv.index.name='station_id'
	rtv.to_csv('preprocess.csv')
	return rtv

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
	#pop=pd.DataFrame(data={'station_id':pdf['station_id'],'flow_count':pdf['out_flow_count']+pdf['in_flow_count']}).groupby('station_id')
	pop=pd.concat([pd.DataFrame(data={'station_id':pdf['station_id'],'flow_count':pdf['out_flow_count']}),pd.DataFrame(data={'station_id':pdf['station_id'],'flow_count':pdf['in_flow_count']})],ignore_index=True).groupby('station_id')
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
	x=[[x,1] for x in idx]
	regi = linear_model.LinearRegression()
	regi.fit(x,arri)
	i=arri-np.array(idx)*regi.coef_[0]+regi.coef_[1]
	print('##  in-flow',regi.coef_)
	rego = linear_model.LinearRegression()
	rego.fit(x,arro)
	o=arro-np.array(idx)*rego.coef_[0]+rego.coef_[1]
	print('## out-flow',rego.coef_)
	drawPNG('q4e.png',idx,i,o,idx[::4],ts,' Flow_i - F(i) ) ','" in/out - F_in/out(i) " of the most popular station')
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
	'''
	if len(sys.argv)!=2:
		print("usage:  "+sys.argv[0]+"  csvFile")
		exit(1)
	'''
	try:
		df=caltr(checking,sys.argv[1])
	except:
		df=caltr(checking,'201707-citibike-tripdata.csv')
	
	head=df.columns.values
	print(head)
	
	#pdf=caltr(preprocess,df,head)
	#phead=pdf.columns.values
	
	# num(stations)
	ss=caltr(getStationNameAndPosition,df,head)
	
	print(ss)

