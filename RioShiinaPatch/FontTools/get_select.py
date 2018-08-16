import json
import codecs
import sys

def proc(lines):
    newls=[]
    idx=0
    while idx<len(lines):
        if lines[idx].startswith(u'○'):
            if lines[idx][7]!=u'○':
                pos=lines[idx].rfind(u'○')
                name=lines[idx][7:pos]
                txt=lines[idx+1]
                newls.append(name)
                newls.append(txt)
                idx+=2
            else:
                txt=lines[idx+1]
                newls.append(txt)
                idx+=2
        else:
            idx+=1
    return newls

fs1=open(sys.argv[1], 'rb')
ls=fs1.read().decode('utf-16le').split('\n')
newl=proc(ls)
fs3=open("out.txt",'wb')
fs3.write(''.join(newl).encode('utf-8'))