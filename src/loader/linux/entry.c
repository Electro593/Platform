/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                             *
*  Author: Aria Seiler                                                        *
*                                                                             *
*  This program is in the public domain. There is no implied warranty, so     *
*  use it at your own risk.                                                   *
*                                                                             *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <loader/linux/linux.c>
#include <loader/linux/elf.c>

#ifdef INCLUDE_HEADER

typedef struct loader_state {
	usize PageSize;
	c08 **EnvParams;

	heap   *Heap;
	hashmap Links;
} loader_state;

typedef struct loader_module {
	elf_state Elf;
	usize	  RefCount;
} loader_module;

#endif	// INCLUDE_HEADER

#ifdef INCLUDE_SOURCE

global loader_state _G;

external vptr
Loader_OpenShared(c08 *Name)
{
	if (!Name) return NULL;

	loader_module *Module;
	string		   NameStr = CString(Name);
	if (HashMap_Get(&_G.Links, &NameStr, &Module)) {
		Module->RefCount++;
		return Module;
	}

	elf_state Elf;
	elf_error Error = Elf_Load(&Elf, Name, _G.EnvParams, _G.PageSize);
	if (Error) return NULL;

	Module	= Heap_AllocateA(_G.Heap, sizeof(loader_module) + NameStr.Length);
	*Module = (loader_module) { 0 };
	Module->Elf = Elf;

	NameStr.Text = (c08 *) (Module + 1);
	Mem_Cpy(NameStr.Text, Name, NameStr.Length);
	HashMap_Add(&_G.Links, &NameStr, &Module);

	return Module;
}

external vptr
Loader_GetSymbol(vptr Module, c08 *Name)
{
	vptr		   Symbol;
	loader_module *Loader = Module;
	if (!Loader || !Loader->RefCount) return NULL;

	elf_error Error = Elf_GetProcAddress(&Loader->Elf, Name, &Symbol);
	if (Error) return NULL;

	return Symbol;
}

external void
Loader_CloseShared(vptr Module)
{
	loader_module *Loader = Module;
	if (!Loader || !Loader->RefCount) return;
	Loader->RefCount--;

	if (!Loader->RefCount) {
		string Name = CString(Loader->Elf.FileName);
		Elf_Unload(&Loader->Elf);
		HashMap_Remove(&_G.Links, &Name, NULL, NULL);
		Mem_Set(Loader, 0, sizeof(loader_module) + Name.Length);
		Heap_FreeA(Loader);
	}
}

internal vptr
Loader_LoadProcess(usize ArgCount, c08 **Args, c08 **EnvParams)
{
	usize EnvCount = 0;
	for (; EnvParams[EnvCount]; EnvCount++);

	vptr BaseAddress	= NULL;
	s32	 FileDescriptor = -1;
	c08 *FileName		= NULL;
	_G.PageSize			= 4096;
	_G.EnvParams		= EnvParams;

	elf_program_header *ProgramHeaders = NULL;
	usize				ProgramHeaderEntrySize;
	usize				ProgramHeaderCount;

	elf_aux *AuxVector = (vptr) (EnvParams + EnvCount + 1);
	usize	 AuxCount  = 0;
	for (; AuxVector[AuxCount].Type != ELF_AUX_NULL; AuxCount++) {
		elf_aux Aux = AuxVector[AuxCount];
		switch (Aux.Type) {
			case ELF_AUX_EXECFD: FileDescriptor = Aux.Value; break;
			case ELF_AUX_PHDR  : ProgramHeaders = Aux.Pointer; break;
			case ELF_AUX_PHENT : ProgramHeaderEntrySize = Aux.Value; break;
			case ELF_AUX_PHNUM : ProgramHeaderCount = Aux.Value; break;
			case ELF_AUX_BASE  : BaseAddress = Aux.Pointer; break;
			case ELF_AUX_PAGESZ: _G.PageSize = Aux.Value; break;
			case ELF_AUX_EXECFN: FileName = Aux.Pointer; break;
		}
	}

	elf_state LoaderElf;
	elf_error Error =
		Elf_ReadLoadedImage(&LoaderElf, BaseAddress, _G.EnvParams, _G.PageSize);
	if (!LoaderElf.FileName) LoaderElf.FileName = "loader.so";
	if (Error) return NULL;

	elf_state UtilElf;
	Error = Elf_Load(&UtilElf, "util.so", _G.EnvParams, _G.PageSize);
	if (Error) Sys_Exit(-1);

	platform_module UtilPModule = { 0 };
	Elf_GetProcAddress(&UtilElf, "Load", (vptr *) &UtilPModule.Load);
	UtilPModule.Load(NULL, &UtilPModule);
	util_funcs Funcs = *(util_funcs *) UtilPModule.Funcs;
#define EXPORT(R, N, ...) N = Funcs.N;
#define X UTIL_FUNCS
#include <x.h>

	vptr HeapBase = Sys_MemMap(
		NULL,
		1024 * 1024,
		SYS_PROT_READ | SYS_PROT_WRITE,
		SYS_MAP_PRIVATE | SYS_MAP_ANONYMOUS,
		SYS_FILE_NONE,
		0
	);
	_G.Heap	 = Heap_Init(HeapBase, 1024 * 1024);
	_G.Links = HashMap_InitCustom(
		_G.Heap,
		sizeof(string),
		sizeof(loader_module *),
		16,
		0.5f,
		2.0f,
		(hash_func) String_HashPtr,
		NULL,
		(cmp_func) String_CmpPtr,
		NULL
	);

	loader_module *LoaderModule =
		Heap_AllocateA(_G.Heap, sizeof(loader_module));
	LoaderModule->Elf	   = LoaderElf;
	LoaderModule->RefCount = 1;
	string LoaderStr	   = HString(_G.Heap, LoaderElf.FileName);
	HashMap_Add(&_G.Links, &LoaderStr, &LoaderModule);

	loader_module *UtilModule = Heap_AllocateA(_G.Heap, sizeof(loader_module));
	UtilModule->Elf			  = UtilElf;
	UtilModule->RefCount	  = 1;
	string UtilStr			  = HString(_G.Heap, UtilElf.FileName);
	HashMap_Add(&_G.Links, &UtilStr, &UtilModule);

	elf_state ProgramElf;
	if (ProgramHeaders) {
		vptr				ProgramBase = NULL;
		elf_program_header *Header		= ProgramHeaders;
		for (usize I = 0; I < ProgramHeaderCount; I++) {
			if (Header->Type == ELF_SEGMENT_TYPE_PHDR)
				ProgramBase = (vptr) ProgramHeaders - Header->VirtualAddress;
			Header = (vptr) ((usize) Header + ProgramHeaderEntrySize);
		}
		Error = Elf_ReadLoadedImage(
			&ProgramElf,
			ProgramBase,
			EnvParams,
			_G.PageSize
		);
		Sys_Close(FileDescriptor);
		if (Error) Sys_Exit(-2);
	} else if (FileDescriptor != -1) {
		Error = Elf_LoadWithDescriptor(
			&ProgramElf,
			FileDescriptor,
			EnvParams,
			_G.PageSize
		);
		if (Error) Sys_Exit(-2);
	} else {
		Error = Elf_Load(&ProgramElf, FileName, _G.EnvParams, _G.PageSize);
		if (Error) Sys_Exit(-2);
	}

	return (vptr) (ProgramElf.ImageAddress
				   - ProgramElf.VAddrOffset
				   + ProgramElf.Header.Entry);
}

#if defined(_X64)

__asm__(
	".globl Loader_Entry          \n"
	"Loader_Entry:                \n"
	"	mov 0(%rsp), %rdi         \n"
	"	lea 8(%rsp), %rsi         \n"
	"	lea 16(%rsp,%rdi,8), %rdx \n"
	"	call Loader_LoadProcess   \n"
	"   jmp *%rax                 \n"
);

#endif

#endif
