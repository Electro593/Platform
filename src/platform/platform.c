/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                            *
*  Author: Aria Seiler                                                       *
*                                                                            *
*  This program is in the public domain. There is no implied warranty, so    *
*  use it at your own risk.                                                  *
*                                                                            *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef INCLUDE_HEADER

typedef struct datetime {
	u16 Year;
	u08 Month;
	u08 Day;
	u08 DayOfWeek;
	u08 Hour;
	u08 Minute;
	u08 Second;
	u16 Millisecond;
} datetime;

typedef struct platform_module platform_module;
typedef struct opengl_funcs	   opengl_funcs;

typedef void func_Module_Load(platform_state *State, platform_module *Module);
typedef void func_Module_Init(platform_state *State);
typedef void func_Module_Update(platform_state *State);
typedef void func_Module_Deinit(platform_state *State);
typedef void func_Module_Unload(platform_state *State);

#define FONTS_DIR    "assets/fonts/"
#define SHADERS_DIR  "assets/shaders/"
#define TEXTURES_DIR "assets/textures/"

#if defined(_WIN32)
#include <platform/win32/win32.h>

typedef struct file_handle {
	win32_handle Handle;
} file_handle;

#define NULL_FILE_HANDLE (file_handle){ .Handle = INVALID_HANDLE_VALUE }

#elif defined(_LINUX)
#include <platform/linux/linux.h>

typedef struct file_handle {
	u32 FileDescriptor;
} file_handle;

#define NULL_FILE_HANDLE (file_handle){ .FileDescriptor = SYS_FILE_NONE }
#else
#error Unsupported platform
#endif

struct platform_module {
	c08 *FileName;
	c08 *Name;

	b08 IsUtil;

	func_Module_Load   *Load;
	func_Module_Init   *Init;
	func_Module_Update *Update;
	func_Module_Deinit *Deinit;
	func_Module_Unload *Unload;

	vptr Data;
	vptr Funcs;
	vptr DebugLoadAddress;

	datetime LastWriteTime;

#ifdef _WIN32
	win32_module DLL;
#else
	vptr ELF;
#endif
};

typedef enum platform_updates {
	WINDOW_RESIZED	= 0x01,
	CURSOR_DISABLED = 0x02,
} platform_updates;

typedef enum execution_state {
	EXECUTION_UNINITIALIZED,
	EXECUTION_RUNNING,
	EXECUTION_PAUSED,
	EXECUTION_ENDED,
} execution_state;

typedef enum focus_state { FOCUS_CLIENT, FOCUS_FRAME, FOCUS_NONE } focus_state;

typedef enum scancode {
	ScanCode_Unknown = 0x00,

	ScanCode_Escape			= 0x01,
	ScanCode_1				= 0x02,
	ScanCode_2				= 0x03,
	ScanCode_3				= 0x04,
	ScanCode_4				= 0x05,
	ScanCode_5				= 0x06,
	ScanCode_6				= 0x07,
	ScanCode_7				= 0x08,
	ScanCode_8				= 0x09,
	ScanCode_9				= 0x0A,
	ScanCode_0				= 0x0B,
	ScanCode_Hyphen			= 0x0C,
	ScanCode_Equals			= 0x0D,
	ScanCode_Backspace		= 0x0E,
	ScanCode_Tab			= 0x0F,
	ScanCode_Q				= 0x10,
	ScanCode_W				= 0x11,
	ScanCode_E				= 0x12,
	ScanCode_R				= 0x13,
	ScanCode_T				= 0x14,
	ScanCode_Y				= 0x15,
	ScanCode_U				= 0x16,
	ScanCode_I				= 0x17,
	ScanCode_O				= 0x18,
	ScanCode_P				= 0x19,
	ScanCode_BracketLeft	= 0x1A,
	ScanCode_BracketRight	= 0x1B,
	ScanCode_Enter			= 0x1C,
	ScanCode_ControlLeft	= 0x1D,
	ScanCode_A				= 0x1E,
	ScanCode_S				= 0x1F,
	ScanCode_D				= 0x20,
	ScanCode_F				= 0x21,
	ScanCode_G				= 0x22,
	ScanCode_H				= 0x23,
	ScanCode_K				= 0x25,
	ScanCode_J				= 0x24,
	ScanCode_L				= 0x26,
	ScanCode_Semicolon		= 0x27,
	ScanCode_Apostrophe		= 0x28,
	ScanCode_Grave			= 0x29,
	ScanCode_ShiftLeft		= 0x2A,
	ScanCode_Backslash		= 0x2B,
	ScanCode_Z				= 0x2C,
	ScanCode_X				= 0x2D,
	ScanCode_C				= 0x2E,
	ScanCode_V				= 0x2F,
	ScanCode_B				= 0x30,
	ScanCode_N				= 0x31,
	ScanCode_M				= 0x32,
	ScanCode_Comma			= 0x33,
	ScanCode_Period			= 0x34,
	ScanCode_Slash			= 0x35,
	ScanCode_ShiftRight		= 0x36,
	ScanCode_Multiply		= 0x37,
	ScanCode_AltLeft		= 0x38,
	ScanCode_Space			= 0x39,
	ScanCode_CapsLock		= 0x3A,
	ScanCode_F1				= 0x3B,
	ScanCode_F2				= 0x3C,
	ScanCode_F3				= 0x3D,
	ScanCode_F4				= 0x3E,
	ScanCode_F5				= 0x3F,
	ScanCode_F6				= 0x40,
	ScanCode_F7				= 0x41,
	ScanCode_F8				= 0x42,
	ScanCode_F9				= 0x43,
	ScanCode_F10			= 0x44,
	ScanCode_NumLock		= 0x45,
	ScanCode_ScrollLock		= 0x46,
	ScanCode_Numpad7		= 0x47,
	ScanCode_Numpad8		= 0x48,
	ScanCode_Numpad9		= 0x49,
	ScanCode_NumpadSubtract = 0x4A,
	ScanCode_Numpad4		= 0x4B,
	ScanCode_Numpad5		= 0x4C,
	ScanCode_Numpad6		= 0x4D,
	ScanCode_NumpadAdd		= 0x4E,
	ScanCode_Numpad1		= 0x4F,
	ScanCode_Numpad2		= 0x50,
	ScanCode_Numpad3		= 0x51,
	ScanCode_Numpad0		= 0x52,
	ScanCode_NumpadDecimal	= 0x53,
	ScanCode_AltPrintScreen = 0x54,
	ScanCode_AngleBracket	= 0x59,
	ScanCode_F11			= 0x57,
	ScanCode_F12			= 0x58,
	ScanCode_OEM1			= 0x5A,
	ScanCode_OEM2			= 0x5B,
	ScanCode_OEM3			= 0x5C,
	ScanCode_EraseEOF		= 0x5D,
	ScanCode_OEM4			= 0x5E,
	ScanCode_OEM5			= 0x5F,
	ScanCode_Zoom			= 0x62,
	ScanCode_Help			= 0x63,
	ScanCode_F13			= 0x64,
	ScanCode_F14			= 0x65,
	ScanCode_F15			= 0x66,
	ScanCode_F16			= 0x67,
	ScanCode_F17			= 0x68,
	ScanCode_F18			= 0x69,
	ScanCode_F19			= 0x6A,
	ScanCode_F20			= 0x6B,
	ScanCode_F21			= 0x6C,
	ScanCode_F22			= 0x6D,
	ScanCode_F23			= 0x6E,
	ScanCode_OEM6			= 0x6F,
	ScanCode_Katakana		= 0x70,
	ScanCode_OEM7			= 0x71,
	ScanCode_F24			= 0x76,
	ScanCode_SBCS			= 0x77,
	ScanCode_Convert		= 0x79,
	ScanCode_NonConvert		= 0x7B,

	ScanCode_MediaPrev		  = 0x80 | 0x10,
	ScanCode_MediaNext		  = 0x80 | 0x19,
	ScanCode_NumpadEnter	  = 0x80 | 0x1C,
	ScanCode_ControlRight	  = 0x80 | 0x1D,
	ScanCode_VolumeMute		  = 0x80 | 0x20,
	ScanCode_LaunchApp2		  = 0x80 | 0x21,
	ScanCode_MediaPlay		  = 0x80 | 0x22,
	ScanCode_MediaStop		  = 0x80 | 0x24,
	ScanCode_VolumeDown		  = 0x80 | 0x2E,
	ScanCode_VolumeUp		  = 0x80 | 0x30,
	ScanCode_BrowserHome	  = 0x80 | 0x32,
	ScanCode_NumpadDivide	  = 0x80 | 0x35,
	ScanCode_PrintScreen	  = 0x80 | 0x37,
	ScanCode_AltRight		  = 0x80 | 0x38,
	ScanCode_Cancel			  = 0x80 | 0x46,
	ScanCode_Home			  = 0x80 | 0x47,
	ScanCode_ArrowUp		  = 0x80 | 0x48,
	ScanCode_PageUp			  = 0x80 | 0x49,
	ScanCode_ArrowLeft		  = 0x80 | 0x4B,
	ScanCode_ArrowRight		  = 0x80 | 0x4D,
	ScanCode_End			  = 0x80 | 0x4F,
	ScanCode_ArrowDown		  = 0x80 | 0x50,
	ScanCode_PageDown		  = 0x80 | 0x51,
	ScanCode_Insert			  = 0x80 | 0x52,
	ScanCode_Delete			  = 0x80 | 0x53,
	ScanCode_MetaLeft		  = 0x80 | 0x5B,
	ScanCode_MetaRight		  = 0x80 | 0x5C,
	ScanCode_Application	  = 0x80 | 0x5D,
	ScanCode_Power			  = 0x80 | 0x5E,
	ScanCode_Sleep			  = 0x80 | 0x5F,
	ScanCode_Wake			  = 0x80 | 0x63,
	ScanCode_BrowserSearch	  = 0x80 | 0x65,
	ScanCode_BrowserFavorites = 0x80 | 0x66,
	ScanCode_BrowserRefresh	  = 0x80 | 0x67,
	ScanCode_BrowserStop	  = 0x80 | 0x68,
	ScanCode_BrowserForward	  = 0x80 | 0x69,
	ScanCode_BrowserBack	  = 0x80 | 0x6A,
	ScanCode_LaunchApp1		  = 0x80 | 0x6B,
	ScanCode_LaunchEmail	  = 0x80 | 0x6C,
	ScanCode_LaunchMedia	  = 0x80 | 0x6D,

	ScanCode_Pause = 0xFF,	// 0xE11D45,
} scancode;

typedef enum button {
	Button_Left	  = 0,
	Button_Right  = 1,
	Button_Middle = 2
} button;

typedef enum key_state {
	RELEASED = 0,
	PRESSED	 = 1,
	HELD	 = 2,
} key_state;

typedef enum file_mode {
	FILE_READ  = 0x0001,
	FILE_WRITE = 0x0002,

	// If the file doesn't exist, create it
	FILE_CREATE = 0x0004,

	// If writing, all writes seek to the end of the file
	FILE_APPEND = 0x0008 | FILE_WRITE,

	// If writing, empty the file if it already exists
	FILE_CLEAR = 0x0010 | FILE_WRITE,
} file_mode;

#define PLATFORM_SHARED_FUNCS \
	EXPORT(void,             Platform_Exit,                  u32 ExitCode) \
	EXPORT(void,             Platform_CreateWindow,          void) \
	EXPORT(void,             Platform_Assert,                c08 *File, u32 Line, c08 *Expression, c08 *Message) \
	EXPORT(vptr,             Platform_AllocateMemory,        u64 Size) \
	EXPORT(void,             Platform_CloseFile,             file_handle FileHandle) \
	EXPORT(void,             Platform_FreeMemory,            vptr Base, u64 Size) \
	EXPORT(u64,              Platform_GetFileLength,         file_handle FileHandle) \
	EXPORT(void,             Platform_GetFileTime,           c08 *FileName, datetime *CreationTime, datetime *LastAccessTime, datetime *LastWriteTime) \
	EXPORT(r64,              Platform_GetTime,               void) \
	EXPORT(void,             Platform_UnloadModule,          platform_module *Module) \
	EXPORT(b08,              Platform_ReloadModule,          platform_module *Module) \
	EXPORT(platform_module*, Platform_LoadModule,            string Name) \
	EXPORT(opengl_funcs*,    Platform_LoadOpenGL,            void) \
	INTERN(void,             Platform_GetProcAddress,        platform_module *Module, c08 *Name, vptr *ProcOut) \
	INTERN(b08,              Platform_IsModuleBackendOpened, platform_module *Module) \
	INTERN(void,             Platform_OpenModuleBackend,     platform_module *Module) \
	INTERN(void,             Platform_CloseModuleBackend,    platform_module *Module) \
	EXPORT(b08,              Platform_ConnectToLocalSocket,  string Name, file_handle *FileHandleOut) \
	EXPORT(string,           Platform_GetEnvParam,           string Name) \
	EXPORT(s08,              Platform_CmpFileTime,           datetime A, datetime B) \
	EXPORT(b08,              Platform_OpenFile,              file_handle *FileHandle, c08 *FileName, file_mode OpenMode) \
	EXPORT(u64,              Platform_ReadFile,              file_handle FileHandle, vptr Dest, u64 Length, u64 Offset) \
	EXPORT(void,             Platform_WriteConsole,          string Message) \
	EXPORT(void,             Platform_WriteError,            string Message, u32 Exit) \
	EXPORT(u64,              Platform_WriteFile,             file_handle FileHandle, vptr Src, u64 Length, u64 Offset)

#endif

#ifdef INCLUDE_SOURCE

internal void
Platform_Stub(void)
{ }

// These are simple implementations of various util functions that we need
// to have access to before we load the util module.

inline internal void
_Mem_Set(vptr D, u08 B, u32 Bytes)
{
	if (Mem_Set) {
		Mem_Set(D, B, Bytes);
		return;
	}
	u08 *DB = D;
	while (Bytes--) *DB++ = B;
}

inline internal void
_Mem_Cpy(vptr D, vptr S, u32 Bytes)
{
	if (Mem_Cpy) {
		Mem_Cpy(D, S, Bytes);
		return;
	}
	u08 *DB = D;
	u08 *SB = S;
	while (Bytes--) *DB++ = *SB++;
}

inline internal s08
_Mem_Cmp(vptr A, vptr B, usize Bytes)
{
	if (Mem_Cmp) return Mem_Cmp(A, B, Bytes);
	u08 *AB = A;
	u08 *BB = B;
	while (Bytes) {
		if (*AB > *BB) return GREATER;
		if (*AB < *BB) return LESS;
		AB++, BB++, Bytes--;
	}

	return EQUAL;
}

inline internal u32
_Mem_BytesUntil(vptr P, c08 B)
{
	u32	 Count = 0;
	u08 *PB	   = P;
	while (*PB++ != B) Count++;
	return Count;
}

inline internal string
_CString(c08 *Chars)
{
	u32 Length = _Mem_BytesUntil(Chars, 0);
	return (string) { .Text		= Chars,
					  .Encoding = STRING_ENCODING_ASCII,
					  .Length	= Length };
}

inline internal s08
_Str_Cmp(vptr A, vptr B)
{
	if (!A && !B) return 0;
	if (!A) return -1;
	if (!B) return 1;
	usize AL = _Mem_BytesUntil(A, 0) + 1;
	usize BL = _Mem_BytesUntil(B, 0) + 1;
	return _Mem_Cmp(A, B, MAX(AL, BL));
}

internal void
Platform_Assert(c08 *File, u32 Line, c08 *Expression, c08 *Message)
{
	c08 LineStr[11];
	u32 Index		 = sizeof(LineStr);
	LineStr[--Index] = 0;
	do {
		LineStr[--Index]  = (Line % 10) + '0';
		Line			 /= 10;
	} while (Line);

	Platform_WriteError(_CString(File), FALSE);
	Platform_WriteError(_CString(": "), FALSE);
	Platform_WriteError(_CString(LineStr + Index), FALSE);
	if (Expression[0] != 0) {
		Platform_WriteError(_CString(": Assertion hit!\n\t("), FALSE);
		Platform_WriteError(_CString(Expression), FALSE);
		Platform_WriteError(_CString(") was FALSE\n"), FALSE);
	} else {
		Platform_WriteError(_CString(": \n"), FALSE);
	}
	if (Message[0] != 0) {
		Platform_WriteError(_CString("\t"), FALSE);
		Platform_WriteError(_CString(Message), FALSE);
	}
	Platform_WriteError(_CString("\n"), FALSE);
}

internal void
Platform_UnloadModule(platform_module *Module)
{
	stack Stack;
	if (_G.UtilIsLoaded) Stack = Stack_Get();
	Module->Unload(&_G);
	if (_G.UtilIsLoaded) Stack_Set(Stack);

	Platform_CloseModuleBackend(Module);
	if (Module->IsUtil) _G.UtilIsLoaded = FALSE;
}

internal b08
Platform_ReloadModule(platform_module *Module)
{
	datetime LastWriteTime;
	Platform_GetFileTime(Module->FileName, 0, 0, &LastWriteTime);
	if (Platform_IsModuleBackendOpened(Module)) {
		if (Platform_CmpFileTime(Module->LastWriteTime, LastWriteTime) != LESS)
			return FALSE;

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
	if (_G.UtilIsLoaded) Stack = Stack_Get();
	if (Module->Load) Module->Load(&_G, Module);
	if (_G.UtilIsLoaded) Stack_Set(Stack);

	if (Module->IsUtil) {
		util_funcs Funcs = *(util_funcs *) Module->Funcs;
#define EXPORT(R, N, ...) N = Funcs.N;
#define X UTIL_FUNCS
#include <x.h>
		_G.UtilIsLoaded = TRUE;
	}

	return TRUE;
}

internal platform_module *
Platform_LoadModule(string Name)
{
	platform_module	 _UtilModule = { 0 };
	platform_module *Module;

#ifdef _WIN32
#define PLATFORM_DYNLIB_SUFFIX ".dll"
#else
#define PLATFORM_DYNLIB_SUFFIX ".so"
#endif

	b08 UtilIsLoaded = _G.UtilIsLoaded;
	Assert(UtilIsLoaded || _Str_Cmp(Name.Text, "util") == 0);
	if (UtilIsLoaded) {
		if (HashMap_Get(&_G.ModuleTable, &Name, &Module)) return Module;

		Module = Heap_AllocateA(
			_G.Heap,
			sizeof(platform_module)
				+ Name.Length
				+ sizeof(PLATFORM_DYNLIB_SUFFIX)
		);
		Mem_Set(Module, 0, sizeof(platform_module));
		HashMap_Add(&_G.ModuleTable, &Name, &Module);

		Module->FileName = (c08 *) (Module + 1);
		Mem_Cpy(Module->FileName, Name.Text, Name.Length);
		Mem_Cpy(
			Module->FileName + Name.Length,
			PLATFORM_DYNLIB_SUFFIX,
			sizeof(PLATFORM_DYNLIB_SUFFIX)
		);
	} else {
		Module			 = &_UtilModule;
		Module->FileName = "util" PLATFORM_DYNLIB_SUFFIX;
		Module->IsUtil	 = TRUE;
	}

	Module->Name			 = Name.Text;
	Module->DebugLoadAddress = NULL;
#ifdef _DEBUG
	if (_Str_Cmp(Name.Text, "util") == EQUAL)
		Module->DebugLoadAddress = (vptr) 0x7DB000000000;
	else if (_Str_Cmp(Name.Text, "base") == EQUAL)
		Module->DebugLoadAddress = (vptr) 0x7DB100000000;
	else if (_Str_Cmp(Name.Text, "renderer_opengl") == EQUAL)
		Module->DebugLoadAddress = (vptr) 0x7DB200000000;
	else if (_Str_Cmp(Name.Text, "wayland") == EQUAL)
		Module->DebugLoadAddress = (vptr) 0x7DB300000000;
#endif

	Platform_ReloadModule(Module);

	if (!UtilIsLoaded) {
		usize StackSize = 64 * 1024 * 1024;
		usize HeapSize	= 8 * 1024 * 1024;
		vptr  Mem		= Platform_AllocateMemory(StackSize + HeapSize);
		Stack_Init(Mem, StackSize);
		Stack_Push();
		_G.Heap = Heap_Init(Mem + StackSize, HeapSize);

		_G.ModuleTable = HashMap_InitCustom(
			_G.Heap,
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

		Module = Heap_AllocateA(_G.Heap, sizeof(platform_module));
		Mem_Cpy(Module, &_UtilModule, sizeof(platform_module));
		HashMap_Add(&_G.ModuleTable, &Name, &Module);
	}

	return Module;
}

internal string
Platform_GetEnvParam(string Name)
{
	string Value;
	if (HashMap_Get(&_G.EnvTable, &Name, &Value)) return Value;
	return EString();
}

#endif
