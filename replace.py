#encoding=utf-8
import os
import re
strproc11=re.compile(r'(.+)(\[np\])')
strproc12=re.compile(r'(.+)(\[r\])')
strproc13=re.compile(r'(@exlink txt=\")(\S+)(\")')
def proc(lines,reparray):
	newl=[]
	tmpptr=0
	i=0
	for l in lines:
		l=l.strip('\n')
		if len(l)==0:
			newl.append(l)
			continue	
		elif l.startswith(u'@'):
			mo3=strproc13.match(l)
			if mo3:
				print(u'Taget Text:'+mo3.group(2))
				try:
					tmpptr=reparray.index(mo3.group(2))
				except:
					print(u'Select Error:'+l)
					newl.append(l)
					continue
				else:
					pass
				try:
					newl.append(l.replace(mo3.group(2),reparray[tmpptr+1]))
				except:
					print(u'Select Error:'+l)
					newl.append(l)
					continue
				else:
					pass
				continue
			newl.append(l)
			continue
		else:
			mo1=strproc11.match(l)
			mo2=strproc12.match(l)
			#print(u'process:'+l)
			if mo2:
				#print(u'Taget Text:'+mo2.group(1))
				try:
					tmpptr=reparray.index(mo2.group(1))
				except:
					print(u'Error:'+l)
					newl.append(l)
					continue
				else:
					pass
				try:
					newl.append(l.replace(mo2.group(1),reparray[tmpptr+1]))
				except:
					print(u'Error:'+l)
					newl.append(l)
					continue
				else:
					pass
				continue
			if mo1:
				#print(u'Target Text:'+mo1.group(1))
				try:
					tmpptr=reparray.index(mo1.group(1))
				except:
					print(u'Error:'+l)
					newl.append(l)
					continue
				else:
					pass
				try:
					newl.append(l.replace(mo1.group(1),reparray[tmpptr+1]))
				except:
					print(u'Error:'+l)
					newl.append(l)
					continue
				else:
					pass
				continue
			newl.append(l)
	return newl

def	estarray(lines):
	array=[]
	for l in lines:
		l=l.strip('\n')
		if len(l)==0:
			continue
		elif l.startswith(u'◇'):
			continue
		elif l.startswith(u'◆'):
			continue
		else:
			array.append(l[8:])
	return array
	

path1='raw'
path2='txt'
path3='out'
for f in os.listdir(path2):
	print(u'File:'+f)
	trans=[]
	fs1=open(os.path.join(path1,f.replace('.txt','.ks')),'rb')
	ls=fs1.read().decode('u16').split('\r\n')
	fs2=open(os.path.join(path2,f.replace('.txt','.txt')),'rb')
	rs=fs2.read().decode('u16').split('\r\n')
	trans=estarray(rs)
	newls=proc(ls,trans)
	fs3=open(os.path.join(path3,f.replace('.txt','.ks')),'wb')	
	fs3.write('\r\n'.join(newls).encode('u16'))
	fs1.close()
	fs2.close()
	fs3.close()
