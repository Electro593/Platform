/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
**                                                                         **
**  Author: Aria Seiler                                                    **
**                                                                         **
**  This program is in the public domain. There is no implied warranty,    **
**  so use it at your own risk.                                            **
**                                                                         **
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if defined(_X64)

__asm__(
	".globl _start                 \n"
	"_start:                       \n"
	"	xor %rbp, %rbp             \n"
	"	mov  0(%rsp), %rdi         \n"
	"	lea  8(%rsp), %rsi         \n"
	"	lea 16(%rsp,%rdi,8), %rdx  \n"
	"	call Platform_Entry        \n"
	"	ud2                        \n"
);

#endif

#define CHECK(Status) ((ssize)(Status) >= 0 || (ssize)(Status) < -4096)

#include <platform/platform.c>
#include <platform/linux/elf.c>

#define VALIDATE(Result, ErrorMessage) \
	do { \
		ssize Status = (ssize) Result; \
		Assert(CHECK(Status), ErrorMessage); \
	} while(0)

global sys_timespec ClockResolution;

#ifdef _OPENGL

internal opengl_funcs *
Platform_LoadOpenGL(void)
{
	return 0;
}

#endif

internal void
Platform_CreateWindow(void)
{ }

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

internal vptr
Platform_AllocateMemory(u64 Size)
{
	vptr Address = Sys_MemMap(
		NULL,
		Size,
		SYS_PROT_READ | SYS_PROT_WRITE,
		SYS_MAP_PRIVATE | SYS_MAP_ANONYMOUS,
		SYS_FILE_NONE,
		0
	);
	VALIDATE(Address, "Failed to map memory");
	return Address;
}

internal void
Platform_FreeMemory(vptr Base, u64 Size)
{
	VALIDATE(Sys_MemUnmap(Base, Size), "Failed to unmap memory");
}

internal u64
Platform_GetFileLength(file_handle FileHandle)
{
	sys_stat Stat;
	s32		 Result = Sys_FileStat(FileHandle.FileDescriptor, &Stat);
	VALIDATE(Result, "Failed to stat the file");
	return Stat.Size;
}

internal b08
Platform_ConnectToLocalSocket(string Name, file_handle *FileHandleOut)
{
	Assert(FileHandleOut);
	if (Name.Length >= sizeof((sys_sockaddr_unix) {}.Data)) return FALSE;

	s32 FileDescriptor = Sys_Socket(SYS_ADDRESS_FAMILY_UNIX, SYS_SOCKET_STREAM, 0);
	if (FileDescriptor < 0) return FALSE;

	sys_sockaddr_unix Address;
	Address.Family = SYS_ADDRESS_FAMILY_UNIX;
	Mem_Cpy(Address.Data, Name.Text, Name.Length);
	Address.Data[Name.Length] = 0;

	s32 Result = Sys_Connect(FileDescriptor, (sys_sockaddr *) &Address, sizeof(sys_sockaddr_unix));
	if (Result < 0) {
		Sys_Close(FileDescriptor);
		return FALSE;
	}

	FileHandleOut->FileDescriptor = FileDescriptor;
	return TRUE;
}

internal b08
Platform_OpenFile(file_handle *FileHandle, c08 *FileName, file_mode OpenMode)
{
	s32 Flags = 0;
	u16 Mode  = 0;

	// TODO Append is incorrect, need to handle offsets better with win32

	if ((OpenMode & FILE_READ) && (OpenMode & FILE_WRITE)) Flags |= SYS_OPEN_READWRITE;
	else if (OpenMode & FILE_READ) Flags |= SYS_OPEN_READONLY;
	else if (OpenMode & FILE_WRITE) Flags |= SYS_OPEN_WRITEONLY;
	else return FALSE;

	if (OpenMode & FILE_CREATE) {
		Flags |= SYS_OPEN_CREATE;
		Mode  |= SYS_CREATE_USER_READ
			  | SYS_CREATE_USER_WRITE
			  | SYS_CREATE_GROUP_READ
			  | SYS_CREATE_GROUP_WRITE
			  | SYS_CREATE_OTHERS_READ
			  | SYS_CREATE_OTHERS_WRITE;
	}

	if ((OpenMode & FILE_APPEND) == FILE_APPEND) Flags |= SYS_OPEN_APPEND;
	if ((OpenMode & FILE_CLEAR) == FILE_CLEAR) Flags |= SYS_OPEN_TRUNCATE;

	s32 FD		= Sys_Open(FileName, Flags, Mode);
	b08 Success = CHECK(FD);

	FileHandle->FileDescriptor = Success ? FD : SYS_FILE_NONE;

	return Success;
}

internal u64
Platform_ReadFile(file_handle FileHandle, vptr Dest, u64 Length, u64 Offset)
{
	if (Offset != (usize) -1)
		VALIDATE(Sys_Seek(FileHandle.FileDescriptor, Offset, SYS_SEEK_BEGINNING), "Failed to seek file");
	s64 BytesRead = Sys_Read(FileHandle.FileDescriptor, Dest, Length);
	VALIDATE(BytesRead, "Failed to read file");
	return BytesRead;
}

internal u64
Platform_WriteFile(file_handle FileHandle, vptr Src, u64 Length, u64 Offset)
{
	if (Offset != (usize) -1)
		VALIDATE(Sys_Seek(FileHandle.FileDescriptor, Offset, SYS_SEEK_BEGINNING), "Failed to seek file");
	s64 BytesWritten = Sys_Write(FileHandle.FileDescriptor, Src, Length);
	VALIDATE(BytesWritten, "Failed to write file");
	return BytesWritten;
}

internal void
Platform_WriteConsole(string Message)
{
	VALIDATE(Sys_Write(SYS_FILE_OUT, Message.Text, Message.Length), "Failed to write to stdout");
}

internal void
Platform_WriteError(string Message, u32 Exit)
{
	// Can't validate here since Assert calls this
	Sys_Write(SYS_FILE_ERR, Message.Text, Message.Length);
	if (Exit) Platform_Exit(Exit);
}

internal void
Platform_CloseFile(file_handle FileHandle)
{
	VALIDATE(Sys_Close(FileHandle.FileDescriptor), "Failed to close the file");
}

// TODO Make this use a file handle

internal void
Platform_GetFileTime(c08 *FileName, datetime *CreationTime, datetime *LastAccessTime, datetime *LastWriteTime)
{
	// 	sys_stat Stat;
	//
	// 	file_handle FileHandle;
	// 	Platform_OpenFile(&FileHandle, FileName, FILE_READ);
	// 	s32 Result = Sys_FileStat(FileHandle.FileDescriptor, &Stat);
	// 	Platform_CloseFile(FileHandle);
	// 	VALIDATE(Result, "Failed to stat the file");
}

internal r64
Platform_GetTime(void)
{
	return 0;
}

internal s08
Platform_CmpFileTime(datetime A, datetime B)
{
	return 0;
}

internal void
Platform_GetProcAddress(platform_module *Module, c08 *Name, vptr *ProcOut)
{
	elf_error Error = Elf_GetProcAddress(&Module->ELF, Name, ProcOut);
	if (Error == ELF_ERROR_NOT_FOUND) *ProcOut = (vptr) Platform_Stub;
	else Assert(!Error, "Failed to get proc address");
}

internal b08
Platform_IsModuleBackendOpened(platform_module *Module)
{
	return Module->ELF.State == ELF_STATE_LOADED;
}

internal void
Platform_OpenModuleBackend(platform_module *Module)
{
	s32 Error = Elf_Open(&Module->ELF, Module->FileName, Platform->PageSize);
	Assert(!Error, "Failed to read elf");
	Error = Elf_LoadProgram(&Module->ELF, Module->DebugLoadAddress);
	Assert(!Error, "Failed to load elf");
}

internal void
Platform_CloseModuleBackend(platform_module *Module)
{
	s32 Error = Elf_Unload(&Module->ELF);
	Assert(!Error, "Failed to unload elf");
	Error = Elf_Close(&Module->ELF);
	Assert(!Error, "Failed to close elf");
}

internal void
Platform_Exit(u32 ExitCode)
{
	Sys_Exit(ExitCode);
	UNREACHABLE;
}

internal void
Platform_SetupEnvTable(usize EnvCount, c08 **EnvParams)
{
	Platform->EnvTable = HashMap_InitCustom(
		Platform->Heap,
		sizeof(string),
		sizeof(string),
		EnvCount * 2,
		0.5f,
		2.0f,
		(hash_func) String_HashPtr,
		NULL,
		(cmp_func) String_CmpPtr,
		NULL
	);

	for (usize I = 0; I < EnvCount; I++) {
		c08 *Param = EnvParams[I];

		usize  KeySize = Mem_BytesUntil((u08 *) Param, '=');
		string Key	   = CLString(Param, KeySize);

		usize  ValueSize = Mem_BytesUntil((u08 *) Param + KeySize + 1, '\0');
		string Value	 = CLString(Param + KeySize + 1, ValueSize);

		HashMap_Add(&Platform->EnvTable, &Key, &Value);
	}
}

external void
Platform_Entry(usize ArgCount, c08 **Args, c08 **EnvParams)
{
	platform_state _P = {0};
	Platform		  = &_P;

#define EXPORT(ReturnType, Namespace, Name, ...) \
	Platform->Functions.Namespace##_##Name = Namespace##_##Name;
#define X PLATFORM_FUNCS
#include <x.h>

	VALIDATE(Sys_GetClockRes(SYS_CLOCK_REALTIME, &ClockResolution), "Failed to get clock resolution");

	elf_auxv *AuxVectors;
	u64		  EnvCount = 0, AuxCount = 0;
	{
		c08 **EnvParam = EnvParams;
		while (*EnvParam) EnvCount++, EnvParam++;

		AuxVectors			= (elf_auxv *) (EnvParam + 1);
		elf_auxv *AuxVector = AuxVectors;
		while (AuxVector->Type != ELF_AT_NULL) AuxCount++, AuxVector++;
	}

	Platform->PageSize = 4096;	// Most common page size
	for (u64 I = 0; I < AuxCount; I++)
		if (AuxVectors[I].Type == ELF_AT_PAGESZ) Platform->PageSize = AuxVectors[I].Value;

	Platform_LoadModule(UTIL_MODULE_NAME);
	Platform_SetupEnvTable(EnvCount, EnvParams);

	Platform_LoadModule(CStringL("base"));

	HASHMAP_FOREACH (I, Hash, string, Key, platform_module *, Module, &Platform->ModuleTable) {
		stack Stack;
		if (Platform->UtilIsLoaded) Stack = Stack_Get();
		Module->Init(Platform);
		if (Platform->UtilIsLoaded) Stack_Set(Stack);
	}

	HASHMAP_FOREACH (I, Hash, string, Key, platform_module *, Module, &Platform->ModuleTable) {
		stack Stack;
		if (Platform->UtilIsLoaded) Stack = Stack_Get();
		Module->Deinit(Platform);
		if (Platform->UtilIsLoaded) Stack_Set(Stack);
		Platform_UnloadModule(Module);
	}

	Stack_Pop();

	Platform_Exit(0);
}
