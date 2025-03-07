#!/usr/bin/sh

if [[ -d build ]]; then rm -rf build > /dev/null; fi
mkdir build > /dev/null
pushd build > /dev/null

CompilerSwitches="$CompilerSwitches -std=c23 -ffast-math -nostdinc -Wall -Wextra" # -Werror # /GS- /Gs0x100000
CompilerSwitches="$CompilerSwitches -Wno-cast-function-type -Wno-comment -Wno-sign-compare -Wno-missing-braces -Wno-unused-variable -Wno-unused-parameter -Wno-unused-but-set-parameter -Wno-unused-but-set-variable"
CompilerSwitches="$CompilerSwitches -D_LINUX -D_GCC -D_X64 -D_OPENGL"
CompilerSwitches="$CompilerSwitches -O0 -ggdb -D_DEBUG"
# CompilerSwitches="$CompilerSwitches -O3"

LinkerSwitches="$LinkerSwitches -nostdlib" # /subsystem:windows /stack:0x100000,0x100000 /machine:x64

DLLCompilerSwitches="$DLLCompilerSwitches $CompilerSwitches -fPIC"
DLLLinkerSwitches="$DLLLinkerSwitches $LinkerSwitches -shared"

if [[ -e *.pdb ]]; then rm *.pdb > /dev/null 2> /dev/null; fi

build_module() {
   ModuleName=$(basename $Module)
   CapitalName=$(echo $ModuleName | awk '{ print toupper($0) }')
   
   if [ "$ModuleName" = "template" ]; then
      echo Skipping $Module
   elif [ "$ModuleName" = "platform" ]; then
      echo Building $Module as an executable
      # gcc $CompilerSwitches $LinkerSwitches -E -D_MODULE_NAMEC=$ModuleName -D_${CapitalName}_MODULE -I ../src -I ../Platform/src -o $ModuleName.i ${Module}linux/entry.c
      # gcc $CompilerSwitches $LinkerSwitches -D_MODULE_NAMEC=$ModuleName -D_${CapitalName}_MODULE -I ../src -I ../Platform/src -o $ModuleName ${Module}linux/entry.c
   else
      echo Building $Module as a library
      # gcc $DLLCompilerSwitches $DLLLinkerSwitches -E -D_MODULE_NAMEC=$ModuleName -D_${CapitalName}_MODULE -I ../src -I ../Platform/src -o $ModuleName.i ${Module}main.c
      gcc $DLLCompilerSwitches $DLLLinkerSwitches -D_MODULE_NAMEC=$ModuleName -D_${CapitalName}_MODULE -I ../src -I ../Platform/src -o $ModuleName.so ${Module}main.c
   fi
}

for Module in ../src/*/; do build_module; done
for Module in ../Platform/src/*/; do build_module; done

if [[ -e *.obj ]]; then rm *.obj > /dev/null; fi
if [[ -e *.exp ]]; then rm *.exp > /dev/null; fi
popd > /dev/null
