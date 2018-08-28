#encoding=utf-8
import os
import re
import sys
import codecs
import pdb

if len(sys.argv) < 3:
    print ('mwege.py <source_tbl> <merge_tbl>')
    exit(0)

NumList=[]
CharList=[]

def loadRawList(line):
	for l in line:
		if len(l)!=0:
			sjisnum=l[0:4]
			mapedchar=l[5]
			NumList.append(sjisnum)
			CharList.append(mapedchar)

def proc(line):
	newls=[]
	idx=0
	for l in line:
		if len(l)!=0:
			sjisnum=l[0:4]
			mapedchar=l[5]
			idx=NumList.index(sjisnum)
			CharList[idx]=mapedchar
	count=0
	while(count<len(NumList)):
		newline=NumList[count]+"="+CharList[count]
		newls.append(newline)
		count+=1
	return newls

def	estarray(lines):
	array=[]
	for l in lines:
		l=l.strip('\n')
		if(len(l)==0):
			continue
		array.append(l)
	return array

fs1=open(sys.argv[1],'rb')
ls=fs1.read().decode('UTF-16').split('\r\n')
fs2=open(sys.argv[2],'rb')
rs=fs2.read().decode('UTF-16').split('\r\n')
loadRawList(ls)
newls=proc(rs)
fs3=open("rebuild.tbl",'wb')	
fs3.write('\r\n'.join(newls).encode('UTF-16'))
fs1.close()
fs2.close()
fs3.close()
