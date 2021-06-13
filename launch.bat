echo off
SET PWD=%CD%
SET PROGS=c:\Program Files (x86)
set PATH=%PROGS%\mingw-w64\i686-8.1.0-posix-dwarf-rt_v6-rev0\mingw32\bin;%PWD%\SDL2-2.0.14\i686-w64-mingw32\bin;%PWD%\SDL2_image-2.0.5\i686-w64-mingw32\bin;%PWD%\SDL2_ttf-2.0.15\i686-w64-mingw32\bin;%PWD%\SDL2_mixer-2.0.4\i686-w64-mingw32\bin;%PATH%
"C:\WINDOWS\system32\cmd.exe"
