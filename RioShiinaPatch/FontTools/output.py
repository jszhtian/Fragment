#-*- coding: utf-8 -*-
import fontforge
import struct
import os

prifixdir=os.path.abspath(os.curdir)
fnt = fontforge.open("F:\debug\FontForge\msyh.ttc")
for w in fnt.glyphs():
    filename=prifixdir+'\\Font\\'+str(w.unicode)+'.svg'
    w.export(filename)
fnt.close()
