Installing:
===========
  No need to install anything. Just copy the files from win32, linux32 or
  linux64 directory (depending on your os) to where you need it.

  On linux you usually want to compile it from source anyway...

Compiling (linux):
==================
Main binary only:

  cd src/
  make diffscgen

  compiled program will be copied to bin/diffscgen

Main binary & static linked binaries for win/linux:

  cd src/
  make

  attention: you may have to change WIN_GCC in the Makefile according to the
  mingw32-gcc installed on your system
