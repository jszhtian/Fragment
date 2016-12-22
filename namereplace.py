# -*- encoding: utf-8 -*-
import sys
import os
import re

strproc11=re.compile(r'(@nm t=\")(\S+)(\")')
strproc12=re.compile(r'(@nm t=\")(\S+)(\")(\srt=\")(\S+)(\")')
targetlist=[u'本を抱えた女の子',u'おしゃれな女の子',u'教師',u'松岡先生',u'学園長',u'薬王寺先生',u'隣の席の女の子',u'外国の女の子',u'クラス委員',u'エレクトリーチカ',u'食堂のおばちゃん',u'咲良の友達Ａ',u'咲良の友達Ｂ',u'英語教師',u'火ノ山先生',u'新聞勧誘員',u'女子アナ']
replacelist=[u'抱着书的女孩子',u'外国的女主',u'教师',u'松岡先生',u'学园长',u'薬王寺先生',u'隔壁座位的女孩子',u'外国的女孩',u'班级委员',u'электри́чка',u'食堂大婶',u'咲良的朋友Ａ',u'咲良的朋友Ｂ',u'英语教师',u'火之山老师',u'新闻劝诱员',u'女主播']
warlist=[]

def proc(lines):
	newl=[]
	for l in lines:
		check=0
		l=l.strip('\n')
		if len(l)==0:
			continue
		elif l.startswith('@'):
			mo2=strproc12.match(l)
			if mo2:
				try:
					tmpptr=targetlist.index(mo2.group(2))
					#print(l)
					newl.append(l.replace(mo2.group(2),replacelist[tmpptr]))
					check=1
				except:
					check=0
					#print(u'Error!'+l)
					pass
			mo=strproc11.match(l)
			if mo and check!=1:
				try:
					tmpptr=targetlist.index(mo.group(2))
					#print(l)
					newl.append(l.replace(u'"'+mo.group(2)+u'"',u'"'+replacelist[tmpptr]+u'"'+' rt="'+mo.group(2)+u'"'))
					check=1
				except:
					#print(u'Error!'+l)
					check=0
					pass
			if check==0:
				newl.append(l)
		else:
			newl.append(l)

	return newl

path1='do'
path2='out'
if len(targetlist)!=len(replacelist):
	print(u'Error!! List is not equal!')
	sys.exit()
for f in os.listdir(path1):
    fs=open(os.path.join(path1,f),'rb')
    ls=fs.read().decode('u16').split('\r\n')
    print('Process:'+f)
    newls=proc(ls)
    fs=open(os.path.join(path2,f.replace('.ks','.ks')),'wb')
    fs.write('\r\n'.join(newls).encode('u16'))
    fs.close()
