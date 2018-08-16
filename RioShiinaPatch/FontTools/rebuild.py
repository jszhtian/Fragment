#-*- coding: utf-8 -*-
import fontforge
import struct
import os

prifixdir=os.path.abspath(os.curdir)
fnt = fontforge.open("F:\debug\FontForge\JP.TTF")
for w in fnt.glyphs():
    filename=prifixdir+'\\Font\\'+str(w.unicode)+'.svg'
    if os.path.exists(filename):
        print "write unicode @"+str(w.unicode)
        w.clear()
        w.importOutlines(filename)
fnt.generate('out.ttf')
fnt.close()
