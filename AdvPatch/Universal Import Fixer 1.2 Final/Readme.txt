==============================================================================
Universal Import Fixer (UIF) v1.2 (FINAL) by: Magic_h2001

Use this tool for fixing Import Elimination, Directly Imports and
Shuffled, Disordered, Scattered Imports.

So you can use this tool for changing IAT Base Address and Sorting IATs 
in New (other) Address.


Tested on:

Armadillo
ASProtect
Enigma
ExeCryptor
eXPressor
PeSpin
RlPack
TheMida
WinLicense

and any protector with Import Elimination, Directly Imports and
Shuffled, Disordered, Scattered Imports.

Notes:
======
This tool is an Import Fixer (not Import Rebuilder ImpRec etc) and Just work
in memory of target process. dont tell me how to use this Tool...if you can
not use this Simple Tool plz DRAG IT TO THE RECYCLE BIN ok?

Always first use UIF then Dump target process.

UIF can fix actual APIs, dont use it for fixing Emulated/Redirected APIs to
protector's stub.you must use UIF After fixing Magic IAT jump 
(or use any methods) to convert Emulated/Redirected APIs to Actual APIs.

Samples:

Armadillo  : Import Elimination
ASProtect  : Directly Imports
Enigma     : Shuffled, Disordered, Scattered Imports
ExeCryptor : Scattered Imports in Protector Stub
eXPressor  : Directly Imports
PeSpin     : Directly, Shuffled, Disordered, Scattered Imports 
RlPack     : Shuffled, Disordered, Scattered Imports
TheMida    : Directly Imports
WinLicense : Directly Imports



How to use :
============
1.fill <Process ID> with target Process ID
2.fill <Code Start> with start address of code that you want to fix it.
  if you fill it with ZERO, UIF will fill it automatically.
3.fill <Code End> with End address of code that you want to fix it.
  if you fill it with ZERO, UIF will fill it automatically.
4.fill <New IAT VA> with address of Empty or unused area 
  (in Code section or Data section or any...) that IAT will repair to it.
  if you fill it with ZERO, UIF will fill it automatically.

 so you can fill <code Start> , <Code End> with a Dll address area, UIF will
 detect it automatically.


for Fast Speed:
===============
  -After Click on <Start> you can Minimize UIF to the taskbar.
  -Just enter Code section start and end (.text section etc).
  -Dont check "Fix Directly Imports" if you dont need to it.


History:
========


v1.2 FINAL update (2008.06.15):
===============================
+Code Optimized again for better result.
+UIF.dll released (for using UIF in other applications).
 Coded with pure Api,very fast and small size.


v1.2 FINAL update (2008.04.24):
===============================
+Fast Speed option added.


v1.2 FINAL (2008.04.19):
========================
+Now UIF can process Ring0 Hooked APIs (KAV,ZoneAlarm,... etc).
-Minor Bugs fixed.


v1.2 Stable (2008.04.04):
=========================
+Algorithm improved for Fast Speed.
-Option 'Main exe Exports' removed (now UIF can detect it automatically)
-Option 'Fix NtDll to Kernel32' removed (now UIF can detect it automatically)
-Minor Bugs fixed.


v1.0 Final+ (2008.03.21):
=========================
+Code Optimized for Fast Speed.
+Always OnTop Added.
+Tested again on many targets:
  (TheMida,WinLicense,Armadillo,ASProtect,Enigma,eXPressor,PeSpin,...)
-Bug fixed in Fixing Directly Imports in Delphi,BCB,VC(MFC) Applications.


v1.0 Final update (2008.02.23):
===============================
+Algorithm improved for better fixing Directly imports.
+Show modules count and progress in StatusBar.
-GUI bug fixed on large fonts >=120 dpi.


v1.0 Final update (2008.01.15):
===============================
-Some small bugs fixed.
+Algorithm improved for very big IAT size.
+Auto fill improved for detecting dlls correctly.


v1.0 Public (2008.01.12):
=========================
First public release...


v1.0 Private (2005.02.23):
==========================
For personal use...


You can download last version of UIF from this link: 

http://magic.shabgard.org/UIF.zip


magic_h2001@yahoo.com
http://magic.shabgard.org
==============================================================================
