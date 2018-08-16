#-*- coding: utf-8 -*-
import os
import codecs
import shutil
import sys

def proc(lines):
    for line in lines:
        if len(line)==0:
            break
        cp932num=int(line[0:4],16)
        gbk=line[5:]
        ch=cp932num.to_bytes(2,byteorder='big').decode('932')
        prifixdir=os.path.abspath(os.curdir)
        gbkbyte=bytes(gbk,encoding='unicode-escape').decode('ansi')[2:]
        sjisbyte=bytes(ch,encoding='unicode-escape').decode('ansi')[2:]
        unigbknum=int(gbkbyte,16)
        unisjisnum=int(sjisbyte,16)
        print('map:'+str(unigbknum)+'->'+str(unisjisnum))
        sourceFile=prifixdir+'\\CHSFont\\'+str(unigbknum)+'.svg'
        targetFile=prifixdir+'\\Map\\'+str(unisjisnum)+'.svg'
        shutil.copyfile(sourceFile,targetFile)


fs1=open(sys.argv[1], 'rb')
ls=fs1.read().decode('u16').split('\r\n')
proc(ls)