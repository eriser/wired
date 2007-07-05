How to build Wired on win32

You need to:
- install MINGW + GDB                  http://www.mingw.org/
- install MSYS + Developer Toolkit     http://www.mingw.org/
  or
  install Code::Blocks IDE last build  http://forums.codeblocks.org/index.php?board=20.0
- install wired-libs-win32 package     oxesoft@users.sourceforge.net
  or
  compile it by yourself the packages:
      wxWidgets                        http://www.wxwidgets.org/
      libsamplerate                    http://www.mega-nerd.com/SRC/
      SoundTouch                       http://www.surina.net/soundtouch/
      sndfile                          http://www.mega-nerd.com/libsndfile/
      portaudio                        src/portaudio or http://www.cs.cmu.edu/~music/portmusic/
      portmidi                         src/midi/portmidi or http://www.cs.cmu.edu/~music/portmusic/

Using MSYS:
Inside MSYS prompt, change the direcory to "win32" and type "make".

Using Code::Blocks IDE:
Open the file "win32/CodeBlocks/Wired.workspace" and run "Build > Build workspace" to compile everything.
More details at http://www.codeblocks.org/
Run "copydata.bat" to copy src/data content to bin directory.

For both cases the required DLLs need to be on PATH or at the same directory where wired.exe is.

--------------------------------------------------------------------------------

How to generate a installer

You need to:
- install Inno Setup Compiler          http://www.jrsoftware.org/isdl.php

Open the file "win32/installer.iss" using Inno Setup and press Ctrl+F9.
Take a look at "win32/Output" and enjoy!
