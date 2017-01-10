#encoding=utf-8
import os
import re
strproc1=re.compile(r':(\S+):')

# COD FILETOMD5 计算文件的MD5
def cod_file2md5(filepathname):
	import hashlib
	try:
		umd5str=hashlib.md5()
		#需要使用二进制格式读取文件内容
		ufile = open(unicode(filepathname,'utf8'), 'rb')
		umd5str.update(ufile.read())
		uoutstr=str(umd5str.hexdigest())
		ufile.close()
		return uoutstr
	except:
		return ''

def proc(lines):
	newl=[]
	i=0
	for l in lines:
		l=l.strip('\n')
		if len(l)==0:
			i+=1
			continue
		elif l.startswith('.'):
			i+=1
			continue
		elif l.startswith(';'):
#			mo=strproc2.match(l)
#			if mo:
#				newl.append(mo.group(1))
			i+=1
			continue
		elif l.startswith('	@'):
			i+=1
			continue
		elif l.startswith(':'):
			mo=strproc1.match(l)
			if mo:
				newl.append('<'+str(i)+'>')
				newl.append('//Name:'+mo.group(1))
				newl.append('')
				newl.append('')
			i+=1
			continue
		elif l.startswith(u'「'):
			newl.append('<'+str(i)+'>')
			newl.append('//'+l)
			newl.append('')
			newl.append('')
			i+=1
			continue
		else:
			newl.append('<'+str(i)+'>')
			newl.append('//'+l)
			newl.append('')
			newl.append('')
			i+=1
	return newl

path1='unpacked'
path2='output'
for f in os.listdir(path1):
    print('File:'+f+'\n')
    Md5str=cod_file2md5(os.path.join(path1,f)) 
    print('MD5:'+Md5str+'\n')
    fs=open(os.path.join(path1,f),'rb')
    ls=fs.read().decode('932').split('\r\n')
    newls=proc(ls)
    newls.insert(0,Md5str)
    fs=open(os.path.join(path2,f+'.txt'),'wb')
    fs.write('\r\n'.join(newls).encode('utf-8'))
    fs.close()
