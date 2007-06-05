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
If everything is OK, type "make install" and a "win32/bin" directory will be created.
Optionally, you can inform a directory for installation typing "make install PREFIX=target_dir"
If the DLLs needed are not in you system PATH, copy all to the root of binary wired directory.

Using Code::Blocks IDE:
Open the file "Wired.workspace" and run "Build > Build workspace" to compile everything.
More details at http://www.codeblocks.org/
Run "copydata.bat" to copy src/data content to bin directory.