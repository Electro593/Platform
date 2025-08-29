#!/usr/bin/sh

Mode="debug"
PositionalArgs=()

while [[ $# -gt 0 ]]; do
	case $1 in
		-h|--help)
			echo "build.sh [OPTIONS]"
			echo "  OPTIONS:"
			echo "    -p=  --platform=  |  'win32' for windows, 'linux' for linux"
			echo "    -a=  --arch=      |  'amd64' for AMD64"
			echo "    -m=  --mode=      |  'debug' (default), 'release' for optimizations"
			echo "    -h   --help       |  Display all options and their descriptions"
			echo
			shift
			;;
		-p=*|--platform=*)
			Platform="${1#*=}"
			shift
			;;
		-a=*|--arch=*)
			Arch="${1#*=}"
			shift
			;;
		-m=*|--mode=*)
			Mode="${1#*=}"
			shift
			;;
		-*|--*)
			echo "Unknown option $1"
			exit 1
			;;
		*)
			PositionalArgs+=("$1")
			shift
			;;
	esac
done

CompilerSwitches="$CompilerSwitches -std=c23 -ffast-math -nostdinc -Wall -Wextra -fno-stack-protector -Werror" # /GS- /Gs0x100000
CompilerSwitches="$CompilerSwitches -Wno-cast-function-type -Wno-comment -Wno-sign-compare -Wno-missing-braces -Wno-unused-variable -Wno-unused-parameter -Wno-unused-but-set-parameter -Wno-unused-but-set-variable"
CompilerSwitches="$CompilerSwitches -I src -I Platform/src"
CompilerSwitches="$CompilerSwitches -U_WIN32 -D_GCC -D_OPENGL"

LinkerSwitches="$LinkerSwitches -nostdlib" # /subsystem:windows /stack:0x100000,0x100000 /machine:x64

if [[ "$Platform" = "win32" ]]; then
	echo "Building for win32"
	CompilerSwitches="$CompilerSwitches -D_WIN32"
	ExeSuffix=".exe"
	DllSuffix=".dll"
elif [[ "$Platform" = "linux" ]]; then
	echo "Building for linux"
	CompilerSwitches="$CompilerSwitches -D_LINUX"
	ExeSuffix=""
	DllSuffix=".so"
else
	echo "Unknown platform option $Platform"
	exit 1
fi

if [[ "$Arch" = "amd64" ]]; then
	echo "Targetting amd64"
	CompilerSwitches="$CompilerSwitches -D_X64"
else
	echo "Unknown arch option $Arch"
	exit 1
fi

if [[ "$Mode" = "debug" ]]; then
	echo "Compiling in debug mode"
	CompilerSwitches="$CompilerSwitches -O0 -g -D_DEBUG"
elif [[ "$Mode" = "release" ]]; then
	echo "Compiling in release mode"
	CompilerSwitches="$CompilerSwitches -O3"
else
	echo "Unknown mode option $Mode"
	exit 1
fi

ExeCompilerSwitches="$ExeCompilerSwitches $CompilerSwitches"
ExeLinkerSwitches="$ExeLinkerSwitches $LinkerSwitches -ePlatform_Entry"

DllCompilerSwitches="$DllCompilerSwitches $CompilerSwitches -fPIC"
DLLLinkerSwitches="$DLLLinkerSwitches $LinkerSwitches -shared -Bsymbolic"

if [[ -d build ]]; then rm -rf build > /dev/null; fi
mkdir build > /dev/null

if [[ -e build/*.pdb ]]; then rm *.pdb > /dev/null 2> /dev/null; fi
if [[ -e build/*.i ]]; then rm *.i > /dev/null; fi

Result=0

build_module() {
	ModuleName=$(basename $Module)
	CapitalName=$(echo $ModuleName | awk '{ print toupper($0) }')

	if [ "$ModuleName" = "template" ]; then
		echo Skipping $Module
	elif [ "$ModuleName" = "platform" ]; then
		echo Building $Module as an executable
		# gcc $ExeCompilerSwitches $ExeLinkerSwitches -E -D_MODULE_NAME="$ModuleName" -D_${CapitalName}_MODULE -o "build/$ModuleName.i" "${Module}linux/entry.c"
		gcc $ExeCompilerSwitches $ExeLinkerSwitches -D_MODULE_NAME="$ModuleName" -D_${CapitalName}_MODULE -o "build/$ModuleName$ExeSuffix" "${Module}${Platform}/entry.c"
		if [[ -e "build/$ModuleName$ExeSuffix" ]]; then
			objdump --source-comment -M intel "build/$ModuleName$ExeSuffix" > "build/$ModuleName.dump.asm"
			objdump -xrR "build/$ModuleName$ExeSuffix" > "build/$ModuleName.dump.dat" 2> /dev/null
			if [[ $? -ne 0 ]]; then objdump -xr "build/$ModuleName$ExeSuffix" > "build/$ModuleName.dump.dat"; fi
		else
			echo Setting result after module $Module
			Result=1
		fi
	else
		echo Building $Module as a library
		# gcc $DllCompilerSwitches $DLLLinkerSwitches -E -D_MODULE_NAME="$ModuleName" -D_${CapitalName}_MODULE -o "build/$ModuleName.i" "${Module}main.c"
		gcc $DllCompilerSwitches $DLLLinkerSwitches -D_MODULE_NAME="$ModuleName" -D_${CapitalName}_MODULE -o "build/$ModuleName$DllSuffix" "${Module}main.c"
		if [[ -e "build/$ModuleName$DllSuffix" ]]; then
			objdump --source-comment -M intel "build/$ModuleName$DllSuffix" > "build/$ModuleName.dump.asm"
			objdump -xrR "build/$ModuleName$DllSuffix" > "build/$ModuleName.dump.dat" 2> /dev/null
			if [[ $? -ne 0 ]]; then objdump -xr "build/$ModuleName$DllSuffix" > "build/$ModuleName.dump.dat"; fi
		else
			echo Setting result after module $Module
			Result=1
		fi
	fi
}

for Module in src/*/; do build_module; done
for Module in Platform/src/*/; do build_module; done

if [[ -e build/*.obj ]]; then rm build/*.obj > /dev/null; fi
if [[ -e build/*.exp ]]; then rm build/*.exp > /dev/null; fi

cp -r assets build/assets

exit $Result
