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

	string AbsoluteProgramName;

	heap   *Heap;
	hashmap Links;

	sys_r_debug	  Debug;
	sys_link_map *DebugLastLink;
} loader_state;

typedef struct loader_module {
	elf_state	  Elf;
	sys_link_map *Link;
	usize		  RefCount;
} loader_module;

#endif	// INCLUDE_HEADER

#ifdef INCLUDE_SOURCE

global loader_state _G;

void
Loader_DebugRendezvousCallback(void)
{ }

internal string
Loader_ResolveModuleName(c08 *ModuleName)
{
	string Str = CString(ModuleName);
	if (Str.Length == 0) return _G.AbsoluteProgramName;

	usize SlashIndex = String_FindCharFromLeft(Str, '/');

	// Absolute path
	if (SlashIndex == 0) {
		string Result = Str;
		Result.Text	  = Heap_AllocateA(_G.Heap, Result.Length + 1);
		Mem_Cpy(Result.Text, Str.Text, Str.Length);
		Result.Text[Result.Length] = 0;
		return Result;
	}

	char Cwd[4096];
	Sys_GetCwd(Cwd, 4096);

	// Relative path
	if (SlashIndex != (usize) -1) {
		string CwdStr = CString(Cwd);

		string Result = EString();
		Result.Length = CwdStr.Length + 1 + Str.Length;
		Result.Text	  = Heap_AllocateA(_G.Heap, Result.Length + 1);

		Mem_Cpy(Result.Text, CwdStr.Text, CwdStr.Length);
		Result.Text[CwdStr.Length] = '/';
		Mem_Cpy(Result.Text + CwdStr.Length + 1, Str.Text, Str.Length);
		Result.Text[Result.Length] = 0;

		return Result;
	}

	// Search path (not supported)
	return EString();
}

internal void
Loader_RegisterElfModule(elf_state Elf)
{
	_G.Debug.State = SYS_R_DEBUG_STATE_ADD;
	_G.Debug.Breakpoint();

	sys_link_map *LinkEntry = Heap_AllocateA(_G.Heap, sizeof(sys_link_map));
	LinkEntry->DeltaAddr	= (usize) Elf.ImageAddress;
	LinkEntry->FileName		= Elf.FileName;
	LinkEntry->Dynamics		= (vptr) Elf.Dynamic.Dynamics;
	LinkEntry->Next			= NULL;
	LinkEntry->Prev			= _G.DebugLastLink;
	if (_G.DebugLastLink) _G.DebugLastLink->Next = LinkEntry;
	else _G.Debug.LinkMap = LinkEntry;
	_G.DebugLastLink = LinkEntry;

	loader_module *Module = Heap_AllocateA(_G.Heap, sizeof(loader_module));
	Module->Elf			  = Elf;
	Module->RefCount	  = 1;
	Module->Link		  = LinkEntry;

	HashMap_Add(&_G.Links, &Elf.FileName, &Module);

	_G.Debug.State = SYS_R_DEBUG_STATE_CONSISTENT;
	_G.Debug.Breakpoint();
}

external vptr
Loader_OpenShared(c08 *Name)
{
	loader_module *Module;
	if (!Name) {
		string EmptyString = EString();
		if (HashMap_Get(&_G.Links, &EmptyString, &Module)) {
			Module->RefCount++;
			return Module;
		}
		return NULL;
	}

	string AbsoluteFileName = Loader_ResolveModuleName(Name);
	if (!AbsoluteFileName.Length) return NULL;

	if (HashMap_Get(&_G.Links, &AbsoluteFileName, &Module)) {
		Module->RefCount++;
		return Module;
	}

	elf_state Elf;
	elf_error Error =
		Elf_Load(&Elf, AbsoluteFileName.Text, _G.EnvParams, _G.PageSize);
	if (Error) return NULL;

	Loader_RegisterElfModule(Elf);

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
		_G.Debug.State = SYS_R_DEBUG_STATE_DELETE;
		_G.Debug.Breakpoint();

		string FileName = CString(Loader->Elf.FileName);

		Elf_Unload(&Loader->Elf);

		if (Loader->Link->Prev) Loader->Link->Prev->Next = Loader->Link->Next;
		if (Loader->Link->Next) Loader->Link->Next->Prev = Loader->Link->Prev;
		if (_G.Debug.LinkMap == Loader->Link)
			_G.Debug.LinkMap = Loader->Link->Next;
		if (_G.DebugLastLink == Loader->Link)
			_G.DebugLastLink = Loader->Link->Prev;
		Heap_FreeA(Loader->Link);

		HashMap_Remove(&_G.Links, &FileName, NULL, NULL);
		Heap_FreeA(FileName.Text);

		Mem_Set(Loader, 0, sizeof(loader_module));
		Heap_FreeA(Loader);

		_G.Debug.State = SYS_R_DEBUG_STATE_CONSISTENT;
		_G.Debug.Breakpoint();
	}
}

internal elf_state
Loader_LoadUtil()
{
	elf_state UtilElf;
	elf_error Error = Elf_Load(&UtilElf, "util.so", _G.EnvParams, _G.PageSize);
	if (Error) Sys_Exit(-1);

	platform_module UtilPlatformModule = { 0 };
	Elf_GetProcAddress(&UtilElf, "Load", (vptr *) &UtilPlatformModule.Load);
	UtilPlatformModule.Load(NULL, &UtilPlatformModule);
	util_funcs Funcs = *(util_funcs *) UtilPlatformModule.Funcs;

#define EXPORT(R, N, ...) N = Funcs.N;
#define X UTIL_FUNCS
#include <x.h>

	return UtilElf;
}

internal vptr
Loader_LoadProcess(usize ArgCount, c08 **Args, c08 **EnvParams)
{
	usize EnvCount = 0;
	for (; EnvParams[EnvCount]; EnvCount++);

	vptr BaseAddress	= NULL;
	s32	 FileDescriptor = -1;
	c08 *FileName		= ArgCount > 0 ? Args[0] : NULL;
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
	if (Error) return NULL;

	elf_state UtilElf = Loader_LoadUtil();

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

	_G.AbsoluteProgramName = CString(FileName);

	_G.Debug.Version	= 1;
	_G.Debug.LoaderBase = BaseAddress;
	_G.Debug.State		= SYS_R_DEBUG_STATE_CONSISTENT;
	_G.Debug.Breakpoint = Loader_DebugRendezvousCallback;
	_G.Debug.LinkMap	= NULL;

	LoaderElf.FileName = Loader_ResolveModuleName("loader.so").Text;
	Loader_RegisterElfModule(LoaderElf);

	UtilElf.FileName = Loader_ResolveModuleName("util.so").Text;
	Loader_RegisterElfModule(UtilElf);

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

	elf_dynamic *Dynamic = ProgramElf.Dynamic.Dynamics;
	while (Dynamic->Tag != ELF_DYNAMIC_TAG_NULL) {
		if (Dynamic->Tag == ELF_DYNAMIC_TAG_DEBUG)
			Dynamic->Pointer = (elf_addr) &_G.Debug;
		Dynamic++;
	}

	ProgramElf.FileName = _G.AbsoluteProgramName.Text;
	Loader_RegisterElfModule(ProgramElf);

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
