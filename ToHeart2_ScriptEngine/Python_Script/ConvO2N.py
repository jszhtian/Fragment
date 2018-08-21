#encoding=utf-8
import os
import re
import codecs
import pdb

linepat = re.compile(r'^(.+)(,.+,)(.+)')
def proc(txt1,txt2):
	newl=[]
	idx=0
	while(idx<len(txt1)):
		m1=linepat.match(txt1[idx])
		m2=linepat.match(txt2[idx])
		newl.append("//"+m1.group(3))
		newl.append(m2.group(3))
		idx+=1
	return newl

def	estarray(lines):
	array=[]
	for l in lines:
		l=l.strip('\n')
		if(len(l)==0):
			continue
		array.append(l)
	return array

path1='raw'
path2='chs'
path3='out'
for f in os.listdir(path2):
	print("Process:"+f)
	fs1=open(os.path.join(path1,f),'rb')
	ls=fs1.read().decode('u16').split('\r\n')
	fs2=open(os.path.join(path2,f),'rb')
	rs=fs2.read().decode('u16').split('\r\n')
	txt1=estarray(ls)
	txt2=estarray(rs)
	newls=proc(txt1,txt2)
	fs3=open(os.path.join(path3,f),'wb')	
	fs3.write('\r\n'.join(newls).encode('utf-8'))
	fs1.close()
	fs2.close()
	fs3.close()
