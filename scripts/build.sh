#!/usr/bin/sh

Mode="debug"
PositionalArgs=()
UseLoader="false"

echo_help() {
	echo "build.sh [OPTIONS]"
	echo "  OPTIONS:"
	echo "    -p=  --platform=  |  'win32' for windows, 'linux' for linux"
	echo "    -a=  --arch=      |  'amd64' for AMD64"
	echo "    -m=  --mode=      |  'debug' (default), 'release' for optimizations"
	echo "    -h   --help       |  Display all options and their descriptions"
	echo
}

while [[ $# -gt 0 ]]; do
	case $1 in
		-h|--help)
			echo_help
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
			echo_help
			exit 1
			;;
		*)
			PositionalArgs+=("$1")
			shift
			;;
	esac
done

CompilerSwitches="$CompilerSwitches -std=c23 -ffast-math -nostdinc -fno-builtin -Wall -Wextra -funsigned-char -fno-stack-check -fno-stack-protector -mno-stack-arg-probe -Werror -masm=intel -flto -fverbose-asm -Winvalid-pch"
CompilerSwitches="$CompilerSwitches -Wno-unused-function -Wno-cast-function-type -Wno-comment -Wno-sign-compare -Wno-missing-braces -Wno-error=unused-variable -Wno-error=unused-parameter -Wno-error=unused-but-set-parameter -Wno-error=unused-but-set-variable"
CompilerSwitches="$CompilerSwitches -I src -I Platform/src"
CompilerSwitches="$CompilerSwitches -U_WIN32 -D_GCC -D_WORD_SIZE=64"

LinkerSwitches="$LinkerSwitches -flto -nostdlib"

if [[ "$Platform" = "win32" ]]; then
	echo "Building for win32"
	CompilerSwitches="$CompilerSwitches -D_WIN32"
	ExeSuffix=".exe"
	DllSuffix=".dll"
	DllLinkerSwitches="--entry=0"
elif [[ "$Platform" = "linux" ]]; then
	echo "Building for linux"
	CompilerSwitches="$CompilerSwitches -D_LINUX"
	ExeSuffix=""
	DllSuffix=".so"
else
	echo "Unknown platform option $Platform"
	echo_help
	exit 1
fi

if [[ "$Arch" = "amd64" ]]; then
	echo "Targetting amd64"
	CompilerSwitches="$CompilerSwitches -D_X64"
else
	echo "Unknown arch option $Arch"
	echo_help
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
	echo_help
	exit 1
fi

ExeCompilerSwitches="$ExeCompilerSwitches $CompilerSwitches"
ExeLinkerSwitches="$ExeLinkerSwitches $LinkerSwitches"
if [ "$UseLoader" = "true" ]; then
	ExeCompilerSwitches="$ExeCompilerSwitches -D_USE_LOADER=1 -L./build -l:loader$DllSuffix"
	ExeLinkerSwitches="$ExeLinkerSwitches -Wl,--dynamic-linker=./loader$DllSuffix"
elif [[ "$Platform" = "linux" ]];  then
	ExeCompilerSwitches="$ExeCompilerSwitches -lc"
fi

DllCompilerSwitches="$DllCompilerSwitches $CompilerSwitches -fPIC"
DllLinkerSwitches="$DllLinkerSwitches $LinkerSwitches -shared -Bsymbolic"

mkdir -p build

if [[ -e build/*.pdb ]]; then rm *.pdb > /dev/null 2> /dev/null; fi

Result=0

print_dump() {
	objdump --source-comment -M intel "build/$1$2" > "build/$1.dump.asm"
	if [ "$Platform" = "win32" ]; then
		objdump -x "build/$1$2" > "build/$1.dump.dat"
	else
		readelf -a -x .data -x .got.plt "build/$1$2" > "build/$1.dump.dat"
	fi
}

build_module() {
	ModuleName=$(basename $Module)
	CapitalName=$(echo $ModuleName | awk '{ print toupper($0) }')

	if [ "$ModuleName" = "template" ]; then
		echo Skipping $Module
	elif [ "$ModuleName" = "loader" ]; then
		if [ "$UseLoader" = "true" ]; then
			echo Building $Module as a library
			gcc $DllCompilerSwitches $DllLinkerSwitches -eLoader_Entry -Wl,-z,now,-rpath,.,-soname,$ModuleName$DllSuffix -E -D_MODULE_NAME="$ModuleName" -D_${CapitalName}_MODULE -o "build/$ModuleName.i" "${Module}main.c"
			gcc $DllCompilerSwitches $DllLinkerSwitches -eLoader_Entry -Wl,-z,now,-rpath,.,-soname,$ModuleName$DllSuffix -D_MODULE_NAME="$ModuleName" -D_${CapitalName}_MODULE -o "build/$ModuleName$DllSuffix" "${Module}main.c"
			if [[ -e "build/$ModuleName$DllSuffix" ]]; then
				print_dump $ModuleName $DllSuffix
			else
				echo Setting result after module $Module
				Result=1
			fi
		fi
	elif [ "$ModuleName" = "platform" ]; then
		echo Building $Module as an executable
		gcc $ExeCompilerSwitches $ExeLinkerSwitches -ePlatform_Entry -Wl,-rpath,. -E -D_MODULE_NAME="$ModuleName" -D_${CapitalName}_MODULE -o "build/$ModuleName.i" "${Module}linux/entry.c"
		gcc $ExeCompilerSwitches $ExeLinkerSwitches -ePlatform_Entry -Wl,-rpath,. -D_MODULE_NAME="$ModuleName" -D_${CapitalName}_MODULE -o "build/$ModuleName$ExeSuffix" "${Module}${Platform}/entry.c"
		if [[ -e "build/$ModuleName$ExeSuffix" ]]; then
			print_dump $ModuleName $ExeSuffix
		else
			echo Setting result after module $Module
			Result=1
		fi
	elif [ -e "${Module}module.h" ]; then
		echo Building $Module as a library with make
		if ! make -f Platform/scripts/Makefile \
			"build/$ModuleName$DllSuffix" \
			"SRC_DIR=${Module%/}" \
			"TMP_DIR=build/${ModuleName}" \
			"OUT_DIR=build" \
			"CFLAGS=$DllCompilerSwitches -D_MODULE_NAME=\"$ModuleName\" -D_${CapitalName}_MODULE" \
			"LDFLAGS=$DllLinkerSwitches -rpath . -soname $ModuleName$DllSuffix" \
		; then
			echo Setting result after module $Module
			Result=1
		fi
	else
		echo Building $Module as a library
		gcc $DllCompilerSwitches $DllLinkerSwitches -Wl,-rpath,.,-soname,$ModuleName$DllSuffix -E -D_MODULE_NAME="$ModuleName" -D_${CapitalName}_MODULE -o "build/$ModuleName.i" "${Module}main.c"
		gcc $DllCompilerSwitches $DllLinkerSwitches -Wl,-rpath,.,-soname,$ModuleName$DllSuffix -D_MODULE_NAME="$ModuleName" -D_${CapitalName}_MODULE -o "build/$ModuleName$DllSuffix" "${Module}main.c"
		if [[ -e "build/$ModuleName$DllSuffix" ]]; then
			print_dump $ModuleName $DllSuffix
		else
			echo Setting result after module $Module
			Result=1
		fi
	fi
}

find_modules() {
	Modules=`find $1 -name main.c`

	for ModuleMain in $Modules; do
		Module="$(dirname $ModuleMain)/"
		build_module
	done
}

find_modules Platform/src
find_modules src

if [[ -e assets/ ]]; then cp -r assets build/assets; fi

exit $Result
