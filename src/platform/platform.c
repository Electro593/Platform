/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
**                                                                         **
**  Author: Aria Seiler                                                    **
**                                                                         **
**  This program is in the public domain. There is no implied warranty,    **
**  so use it at your own risk.                                            **
**                                                                         **
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <shared.h>

#define INCLUDE_HEADER
#include <util/main.c>
#undef INCLUDE_HEADER

#include <platform/platform.h>

#ifdef _OPENGL
#define INCLUDE_HEADER
#include <renderer_opengl/opengl.h>
#undef INCLUDE_HEADER
#endif

global platform_state *Platform;

internal void
Platform_Stub(void)
{ }

// These are simple implementations of various util functions that we need
// to have access to before we load the util module.

internal void
_Mem_Set(u08 *D, u08 B, u32 Bytes)
{
	while (Bytes--) *D++ = B;
}

internal void
_Mem_Cpy(u08 *D, u08 *S, u32 Bytes)
{
	while (Bytes--) *D++ = *S++;
}

internal void
_Mem_CpyRev(u08 *D, u08 *S, u32 Bytes)
{
	D += Bytes - 1;
	S += Bytes - 1;
	while (Bytes--) *D-- = *S--;
}

internal s08
_Mem_Cmp(u08 *A, u08 *B, u32 Bytes)
{
	while (Bytes) {
		if (*A > *B) return GREATER;
		if (*A < *B) return LESS;
		A++, B++, Bytes--;
	}

	return EQUAL;
}

internal u32
_Mem_BytesUntil(u08 *P, c08 B)
{
	u32 Count = 0;
	while (*P++ != B) Count++;
	return Count;
}

internal s08
_Str_Cmp(c08 *A, c08 *B)
{
	usize L1 = _Mem_BytesUntil((u08 *) A, 0);
	usize L2 = _Mem_BytesUntil((u08 *) B, 0);
	if (L1 > L2) L1 = L2;
	L1++;
	return _Mem_Cmp((u08 *) A, (u08 *) B, L1);
}

internal string
_CString(c08 *Chars)
{
	u32 Length = _Mem_BytesUntil((u08 *) Chars, 0);
	return (string) {.Text = Chars, .Length = Length, .Capacity = Length, .Resizable = FALSE};
}

internal void
Platform_UnloadModule(platform_module *Module)
{
	stack Stack;
	if (Platform->UtilIsLoaded) Stack = Stack_Get();
	Module->Unload(Platform);
	if (Platform->UtilIsLoaded) Stack_Set(Stack);

	Platform_CloseModuleBackend(Module);
}

internal b08
Platform_ReloadModule(platform_module *Module)
{
	datetime LastWriteTime;
	Platform_GetFileTime(Module->FileName, 0, 0, &LastWriteTime);
	if (Platform_IsModuleBackendOpened(Module)) {
		if (Platform_CmpFileTime(Module->LastWriteTime, LastWriteTime) != LESS) return FALSE;

		Platform_UnloadModule(Module);
	}
	Module->LastWriteTime = LastWriteTime;

	Platform_OpenModuleBackend(Module);

	Platform_GetProcAddress(Module, "Load", (vptr *) &Module->Load);
	Platform_GetProcAddress(Module, "Init", (vptr *) &Module->Init);
	Platform_GetProcAddress(Module, "Update", (vptr *) &Module->Update);
	Platform_GetProcAddress(Module, "Deinit", (vptr *) &Module->Deinit);
	Platform_GetProcAddress(Module, "Unload", (vptr *) &Module->Unload);

	stack Stack;
	if (Platform->UtilIsLoaded) Stack = Stack_Get();
	Module->Load(Platform, Module);
	if (Platform->UtilIsLoaded) Stack_Set(Stack);

	if (_Str_Cmp(Module->Name, "util") == EQUAL) {
		util_funcs Funcs = *(util_funcs *) Module->Funcs;
#define EXPORT(R, N, ...) N = Funcs.N;
#define X UTIL_FUNCS
#include <x.h>
	}

	return TRUE;
}

internal platform_module *
Platform_LoadModule(string Name)
{
	platform_module	 _UtilModule = {0};
	platform_module *Module;

#ifdef _WIN32
#define PLATFORM_DYNLIB_SUFFIX ".dll"
#else
#define PLATFORM_DYNLIB_SUFFIX ".so"
#endif

	Assert(Platform->UtilIsLoaded || _Str_Cmp(Name.Text, "util") == 0);
	if (Platform->UtilIsLoaded) {
		if (HashMap_Get(&Platform->ModuleTable, &Name, &Module)) return Module;

		Module = Heap_AllocateA(
			Platform->Heap,
			sizeof(platform_module) + Name.Length + sizeof(PLATFORM_DYNLIB_SUFFIX)
		);
		Mem_Set(Module, 0, sizeof(platform_module));
		HashMap_Add(&Platform->ModuleTable, &Name, &Module);

		Module->FileName = (c08 *) (Module + 1);
		Mem_Cpy(Module->FileName, Name.Text, Name.Length);
		Mem_Cpy(Module->FileName + Name.Length, PLATFORM_DYNLIB_SUFFIX, sizeof(PLATFORM_DYNLIB_SUFFIX));
	} else {
		Module			 = &_UtilModule;
		Module->FileName = "util" PLATFORM_DYNLIB_SUFFIX;
	}

	Module->Name			 = Name.Text;
	Module->DebugLoadAddress = NULL;
#ifdef _DEBUG
	if (_Str_Cmp(Name.Text, "util") == EQUAL) Module->DebugLoadAddress = (vptr) 0x7DB000000000;
	else if (_Str_Cmp(Name.Text, "base") == EQUAL) Module->DebugLoadAddress = (vptr) 0x7DB100000000;
	else if (_Str_Cmp(Name.Text, "renderer_opengl") == EQUAL)
		Module->DebugLoadAddress = (vptr) 0x7DB200000000;
	else if (_Str_Cmp(Name.Text, "wayland") == EQUAL) Module->DebugLoadAddress = (vptr) 0x7DB300000000;
#endif

	Platform_ReloadModule(Module);

	if (!Platform->UtilIsLoaded) {
		usize StackSize = 64 * 1024 * 1024;
		usize HeapSize	= 8 * 1024 * 1024;
		vptr  Mem		= Platform_AllocateMemory(StackSize + HeapSize);
		Stack_Init(Mem, StackSize);
		Stack_Push();
		Platform->Heap = Heap_Init(Mem + StackSize, HeapSize);

		Platform->ModuleTable = HashMap_InitCustom(
			Platform->Heap,
			sizeof(string),
			sizeof(platform_module *),
			32,
			0.5f,
			2.0f,
			(hash_func) String_HashPtr,
			NULL,
			(cmp_func) String_CmpPtr,
			NULL
		);

		Module = Heap_AllocateA(Platform->Heap, sizeof(platform_module));
		Mem_Cpy(Module, &_UtilModule, sizeof(platform_module));
		HashMap_Add(&Platform->ModuleTable, &Name, &Module);
		Platform->UtilIsLoaded = TRUE;
	}

	return Module;
}

internal string
Platform_GetEnvParam(string Name)
{
	string Value;
	if (HashMap_Get(&Platform->EnvTable, &Name, &Value)) return Value;
	return EString();
}
