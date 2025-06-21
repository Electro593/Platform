/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
**                                                                         **
**  Author: Aria Seiler                                                    **
**                                                                         **
**  This program is in the public domain. There is no implied warranty,    **
**  so use it at your own risk.                                            **
**                                                                         **
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if defined(_X64)

__asm__ (
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

internal opengl_funcs*
Platform_LoadOpenGL(void)
{
	return 0;
}

#endif

internal void
Platform_CreateWindow(void)
{

}

internal void
Platform_Assert(c08 *File, u32 Line, c08 *Expression, c08 *Message)
{
	c08 LineStr[11];
	u32 Index = sizeof(LineStr);
	LineStr[--Index] = 0;
	do {
	   LineStr[--Index] = (Line % 10) + '0';
	   Line /= 10;
	} while(Line);

	Platform_WriteError(_CString(File), FALSE);
	Platform_WriteError(_CString(": "), FALSE);
	Platform_WriteError(_CString(LineStr + Index), FALSE);
	if(Expression[0] != 0) {
	   Platform_WriteError(_CString(": Assertion hit!\n\t("), FALSE);
	   Platform_WriteError(_CString(Expression), FALSE);
	   Platform_WriteError(_CString(") was FALSE\n"), FALSE);
	} else {
	   Platform_WriteError(_CString(": \n"), FALSE);
	}
	if(Message[0] != 0) {
	   Platform_WriteError(_CString("\t"), FALSE);
	   Platform_WriteError(_CString(Message), FALSE);
	}
	Platform_WriteError(_CString("\n"), FALSE);
}

internal vptr
Platform_AllocateMemory(u64 Size)
{
	vptr Address = Sys_MemMap(NULL, Size, SYS_PROT_READ | SYS_PROT_WRITE, SYS_MAP_PRIVATE | SYS_MAP_ANONYMOUS, SYS_FILE_NONE, 0);
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
	s32 Result = Sys_FileStat(FileHandle.FileDescriptor, &Stat);
	VALIDATE(Result, "Failed to stat the file");
	return Stat.Size;
}

internal b08
Platform_OpenFile(file_handle *FileHandle, c08 *FileName, file_mode OpenMode)
{
	s32 Flags = 0;
	u16 Mode = 0;

	//TODO Append is incorrect, need to handle offsets better with win32

	switch (OpenMode) {
		case FILE_READ: {
			Flags |= SYS_OPEN_READONLY;
		} break;
		case FILE_WRITE: {
			Flags |= SYS_OPEN_WRITEONLY | SYS_OPEN_CREATE | SYS_OPEN_TRUNCATE;
			Mode |= SYS_CREATE_USER_READ | SYS_CREATE_USER_WRITE
				| SYS_CREATE_GROUP_READ  | SYS_CREATE_GROUP_WRITE
				| SYS_CREATE_OTHERS_READ | SYS_CREATE_OTHERS_WRITE;
		} break;
		case FILE_APPEND: {
			Flags |= SYS_OPEN_WRITEONLY | SYS_OPEN_CREATE | SYS_OPEN_APPEND;
		} break;
	}

	u32 FD = Sys_Open(FileName, Flags, Mode);
	b08 Success = CHECK((s32) FD);

	FileHandle->FileDescriptor = Success ? FD : SYS_FILE_NONE;

	return Success;
}

internal u64
Platform_ReadFile(file_handle FileHandle, vptr Dest, u64 Length, u64 Offset)
{
	VALIDATE(Sys_Seek(FileHandle.FileDescriptor, Offset, SYS_SEEK_BEGINNING), "Failed to seek file");
	s64 BytesRead = Sys_Read(FileHandle.FileDescriptor, Dest, Length);
	VALIDATE(BytesRead, "Failed to read file");
	return BytesRead;
}

internal u64
Platform_WriteFile(file_handle FileHandle, vptr Src, u64 Length, u64 Offset)
{
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

//TODO Make this use a file handle

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
Platform_GetProcAddress(elf_state *State, c08 *Name, vptr *ProcAddress)
{
	elf_error Error = Elf_GetProcAddress(State, Name, ProcAddress);
	if (Error == ELF_ERROR_NOT_FOUND)
		*ProcAddress = (vptr) Platform_Stub;
	else
		Assert(!Error, "Failed to get proc address");
}

internal b08
Platform_UnloadModule(platform_module *Module)
{
	if(Module->ELF.State == ELF_STATE_LOADED) {
	   Module->Unload(Platform);
	   s32 Error = Elf_Unload(&Module->ELF);
	   Assert(!Error, "Failed to unload elf");
	   Error = Elf_Close(&Module->ELF);
	   Assert(!Error, "Failed to close elf");
	   return TRUE;
	}
	return FALSE;
}

internal b08
Platform_ReloadModule(platform_module *Module)
{
	datetime LastWriteTime;
	Platform_GetFileTime(Module->FileName, 0, 0, &LastWriteTime);
	if(Module->ELF.State == ELF_STATE_LOADED) {
	   if(Platform_CmpFileTime(Module->LastWriteTime, LastWriteTime) != LESS)
		  return FALSE;

	   Platform_UnloadModule(Module);
	}
	Module->LastWriteTime = LastWriteTime;

	s32 Error = Elf_Open(&Module->ELF, Module->FileName, Platform->PageSize);
	Assert(!Error, "Failed to read elf");
	Error = Elf_LoadProgram(&Module->ELF, Module->DebugLoadAddress);
	Assert(!Error, "Failed to load elf");

	Platform_GetProcAddress(&Module->ELF, "Load", (vptr*) &Module->Load);
	Platform_GetProcAddress(&Module->ELF, "Init", (vptr*) &Module->Init);
	Platform_GetProcAddress(&Module->ELF, "Update", (vptr*) &Module->Update);
	Platform_GetProcAddress(&Module->ELF, "Unload", (vptr*) &Module->Unload);

	Module->Load(Platform, Module);

	return TRUE;
}

internal void
Platform_Exit(u32 ExitCode)
{
	Sys_Exit(ExitCode);
    UNREACHABLE;
}

external void
Platform_Entry(u64 ArgCount, c08 **Args, c08 **EnvParams)
{
	platform_state _P = {0};
	Platform = &_P;

	#define EXPORT(ReturnType, Namespace, Name, ...) \
	Platform->Functions.Namespace##_##Name = Namespace##_##Name;
	#define X PLATFORM_FUNCS
	#include <x.h>

	VALIDATE(Sys_GetClockRes(SYS_CLOCK_REALTIME, &ClockResolution), "Failed to get clock resolution");

	elf_auxv *AuxVectors;
	u64 EnvCount = 0, AuxCount = 0;
	{
		c08 **EnvParam = EnvParams;
		while (*EnvParam) EnvCount++, EnvParam++;

		AuxVectors = (elf_auxv*) (EnvParam+1);
		elf_auxv *AuxVector = AuxVectors;
		while (AuxVector->Type != ELF_AT_NULL) AuxCount++, AuxVector++;
	}

	Platform->PageSize = 4096; // Most common page size
	for (u64 I = 0; I < AuxCount; I++) {
		if (AuxVectors[I].Type == ELF_AT_PAGESZ)
			Platform->PageSize = AuxVectors[I].Value;
	}

	platform_module *Util = Platform_LoadModule("util", (vptr) 0x7DB000000000);
	platform_module *Base = Platform_LoadModule("base", (vptr) 0x7DB100000000);

	Platform_LoadUtilFuncs(Util);

	Util->Init(Platform);
	Base->Init(Platform);

	Platform_UnloadModule(Util);
	Platform_UnloadModule(Base);

	Platform_Exit(0);
}