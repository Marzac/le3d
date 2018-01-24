@echo ============================
@echo LightEngine 3D: cube example
@echo ============================
@echo Compiling and linking
@echo.
g++.exe -march=pentium4 -Wall -m32 -ffast-math -mfpmath=sse -fno-exceptions -fno-rtti -fno-stack-protector -fno-math-errno -fno-ident -ffunction-sections -D__MSVCRT_VERSION__=0x0700 -O2 -I..\..\..\le3d "cube_win.cpp" ..\..\..\le3d\engine\*.cpp  ..\..\..\le3d\tools\*.cpp -o cube.exe -lgdi32 -lwinmm -Wl,--gc-sections -static-libgcc -s
@echo.
@echo off
pause
@echo.
cube.exe
