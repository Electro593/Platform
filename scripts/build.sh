#!/usr/bin/sh

Debug=1

if [[ -d build ]]; then rm -rf build > /dev/null; fi
mkdir build > /dev/null
pushd build > /dev/null

CompilerSwitches="$CompilerSwitches -std=c23 -ffast-math -nostdinc -Wall -Wextra -fno-stack-protector" # -Werror # /GS- /Gs0x100000
CompilerSwitches="$CompilerSwitches -Wno-cast-function-type -Wno-comment -Wno-sign-compare -Wno-missing-braces -Wno-unused-variable -Wno-unused-parameter -Wno-unused-but-set-parameter -Wno-unused-but-set-variable"
CompilerSwitches="$CompilerSwitches -D_LINUX -D_GCC -D_X64 -D_OPENGL"

LinkerSwitches="$LinkerSwitches -nostdlib" # /subsystem:windows /stack:0x100000,0x100000 /machine:x64

if [[ Debug -eq 1 ]]; then
   echo Building as DEBUG
   CompilerSwitches="$CompilerSwitches -O0 -g -D_DEBUG"
else
   echo Building as RELEASE
   CompilerSwitches="$CompilerSwitches -O3"
fi

DLLCompilerSwitches="$DLLCompilerSwitches $CompilerSwitches -fPIC"
DLLLinkerSwitches="$DLLLinkerSwitches $LinkerSwitches -shared -Bsymbolic"

if [[ -e *.pdb ]]; then rm *.pdb > /dev/null 2> /dev/null; fi

Result=0

build_module() {
   ModuleName=$(basename $Module)
   CapitalName=$(echo $ModuleName | awk '{ print toupper($0) }')

   if [ "$ModuleName" = "template" ]; then
      echo Skipping $Module
   elif [ "$ModuleName" = "platform" ]; then
      echo Building $Module as an executable
      # gcc $CompilerSwitches $LinkerSwitches -E -D_MODULE_NAMEC=$ModuleName -D_${CapitalName}_MODULE -I ../src -I ../Platform/src -o $ModuleName.i ${Module}linux/entry.c
      gcc $CompilerSwitches $LinkerSwitches -D_MODULE_NAMEC=$ModuleName -D_${CapitalName}_MODULE -I ../src -I ../Platform/src -o $ModuleName ${Module}linux/entry.c
      if [[ -e $ModuleName ]]; then
         objdump --source-comment -M intel $ModuleName > $ModuleName.dump.asm
         objdump -xrR $ModuleName > $ModuleName.dump.dat
      else
         Result=1
      fi
   else
      echo Building $Module as a library
      # gcc $DLLCompilerSwitches $DLLLinkerSwitches -E -D_MODULE_NAMEC=$ModuleName -D_${CapitalName}_MODULE -I ../src -I ../Platform/src -o $ModuleName.i ${Module}main.c
      gcc $DLLCompilerSwitches $DLLLinkerSwitches -D_MODULE_NAMEC=$ModuleName -D_${CapitalName}_MODULE -I ../src -I ../Platform/src -o $ModuleName.so ${Module}main.c
      if [[ -e $ModuleName.so ]]; then
         objdump --source-comment -M intel $ModuleName.so > $ModuleName.dump.asm
         objdump -xrR $ModuleName.so > $ModuleName.dump.dat
      else
         Result=1
      fi
   fi
}

for Module in ../src/*/; do build_module; done
for Module in ../Platform/src/*/; do build_module; done

if [[ -e *.obj ]]; then rm *.obj > /dev/null; fi
if [[ -e *.exp ]]; then rm *.exp > /dev/null; fi
popd > /dev/null

cp -r assets build/assets

exit $Result
