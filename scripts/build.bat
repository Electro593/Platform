@echo off & setlocal
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" (
   call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
) else (
   call "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
)

if exist build rmdir /Q /S build
mkdir build
pushd build

set CompilerSwitches=%CompilerSwitches% /nologo /std:c23 /fp:fast /EHa- /FC /GF /GR- /GS- /Gs0x100000 /J /WX /Wall /X
set CompilerSwitches=%CompilerSwitches% /wd4702 /wd4101 /wd5045 /wd4820 /wd4701 /wd4213 /wd4242 /wd4244 /wd4201 /wd4061 /wd4062 /wd4127 /wd4214 /wd4189 /wd4100 /wd4094 /wd4200 /wd4018 /wd4710 /wd4706 /wd4703 /wd4152 /wd4010
set CompilerSwitches=%CompilerSwitches% /D_WIN32 /D_MSVC /D_X64 /D_DEBUG /D_OPENGL
set CompilerSwitches=%CompilerSwitches% /Od /Z7 /Oi
set LinkerSwitches=%LinkerSwitches% /wx /incremental:no /opt:ref /opt:icf /nodefaultlib /subsystem:windows /stack:0x100000,0x100000 /machine:x64
set DLLCompilerSwitches=%DLLCompilerSwitches% /LD
set DLLLinkerSwitches=%DLLLinkerSwitches% /noimplib /noentry

if exist *.pdb del *.pdb > nul 2> nul

for /F "delims=" %%A in ('dir /AD /b ..\src') do call :build_module "..\src" %%A

for /F "delims=" %%A in ('dir /AD /b ..\Platform\src') do (
   if "%%A"=="platform" (
      echo Building platform...
      cl %CompilerSwitches% /D  "_PLATFORM_MODULE" /I ..\src /I ..\Platform\src ..\Platform\src\platform\win32\entry.c /link %LinkerSwitches% /entry:Platform_Entry /out:platform.exe
   ) else if not "%%A"=="template" (
      call :build_module "..\Platform\src" %%A
   )
)

if exist *.obj del *.obj
if exist *.exp del *.exp
popd
exit /b 0

:build_module
   set CapitalName=
   for /F "skip=2 delims=" %%I in ('tree "\%2"') do if not defined CapitalName set "CapitalName=%%~I"
   set "CapitalName=%CapitalName:~3%"
   
   echo Building %2...
   if "%2"=="base" cl %CompilerSwitches% %DLLCompilerSwitches% /D "_MODULE_NAMEC=%2" /D "_%CapitalName%_MODULE" /P /I ..\src\ /I ..\Platform\src %1\%2\main.c
   cl %CompilerSwitches% %DLLCompilerSwitches% /D "_MODULE_NAMEC=%2" /D "_%CapitalName%_MODULE" /I ..\src\ /I ..\Platform\src %1\%2\main.c /link %LinkerSwitches% %DLLLinkerSwitches% /pdb:%2_%random%.pdb /out:%2.dll
exit /b 0