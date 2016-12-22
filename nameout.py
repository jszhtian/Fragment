# -*- encoding: utf-8 -*-
import sys
import os
import re

strproc11=re.compile(r'(@nm t=\")(\S+)(\")')
strproc12=re.compile(r'(@nm t=\")(\S+)(\")(\srt=\")(\S+)(\")')
#targetlist=[u'伊吹',u'？？？']
#replacelist=[u'伊吹',u'？？？']
#targetlist2=[u'？？？'.u'本を抱えた女の子',u'声',u'おしゃれな女の子',u'教師']
#replacelist2=[u'？？？',u'本を抱えた女の子',u'声',u'おしゃれな女の子',u'教師']
warlist=[]
writelist=[]
def proc(lines):
	newl=[]
	for l in lines:
		l=l.strip('\n')
		if len(l)==0:
			continue
		elif l.startswith('@'):
			mo2=strproc12.match(l)
			if mo2:
				try:
					tmpptr=warlist.index(mo2.group(2))
#					print(l)
					continue
				except:
					pass
			mo=strproc11.match(l)
			if mo:
				try:
					tmpptr=warlist.index(mo.group(2))
#					print(l)
#					newl.append(l.replace(u'"'+mo.group(2)+u'"',u'"'+replacelist[tmpptr]+u'"'+' rt="'+mo.group(2)+u'"'))
					continue
				except:
					warlist.append(mo.group(2))
					writelist.append(mo.group(2)+'->')

	return newl

path1='do'
#path2='out'
for f in os.listdir(path1):
    fs=open(os.path.join(path1,f),'rb')
    ls=fs.read().decode('u16').split('\r\n')
    print('Process:'+f)
    newls=proc(ls)
fs=open(os.path.join('result.txt'),'wb')
fs.write('\r\n'.join(writelist).encode('u16'))
fs.close()