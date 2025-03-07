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

#include <platform/platform.c>

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
	vptr Address = Sys_MemMap(NULL, Size, SYS_PROT_READ | SYS_PROT_WRITE, SYS_MAP_ANONYMOUS, SYS_FILE_NONE, 0);
	Assert ((s64) Address >= 0 || (s64) Address < -4096, "Failed to map memory");
	return Address;
}

internal void
Platform_FreeMemory(vptr Base, u64 Size)
{
	s32 Result = Sys_MemUnmap(Base, Size);
	Assert(Result == 0, "Failed to unmap memory");
}

internal u64
Platform_GetFileLength(file_handle FileHandle)
{
	return 0;
}

internal b08
Platform_OpenFile(file_handle *FileHandle, c08 *FileName, file_mode OpenMode)
{
	return 0;
}

internal u64
Platform_ReadFile(file_handle FileHandle, vptr Dest, u64 Length, u64 Offset)
{
	return 0;
}

internal u64
Platform_WriteFile(file_handle FileHandle, vptr Src, u64 Length, u64 Offset)
{
	// Sys_Write(FileHandle.FileDescriptor, );
	return 0;
}

internal void
Platform_WriteConsole(string Message)
{
	Sys_Write(SYS_FILE_OUT, Message.Text, Message.Length);
}

internal void
Platform_WriteError(string Message, u32 Exit)
{
	Sys_Write(SYS_FILE_ERR, Message.Text, Message.Length);
	if (Exit) Platform_Exit(Exit);
}

internal void
Platform_CloseFile(file_handle FileHandle)
{
	s32 Result = Sys_Close(FileHandle.FileDescriptor);
	Assert(Result == 0, "Failed to close the file");
}

internal void
Platform_GetFileTime(c08 *FileName, datetime *CreationTime, datetime *LastAccessTime, datetime *LastWriteTime)
{

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

internal platform_module*
Platform_LoadModule(c08 *Name)
{
	return 0;
}

internal void
Platform_Exit(u32 ExitCode)
{
	Sys_Exit(ExitCode);
    UNREACHABLE;
}

external void
Platform_Entry(s32 argc, c08 **argv, c08 **envp)
{
	platform_state _P = {0};
	Platform = &_P;

	#define EXPORT(ReturnType, Namespace, Name, ...) \
	Platform->Functions.Namespace##_##Name = Namespace##_##Name;
	#define X PLATFORM_FUNCS
	#include <x.h>

	Platform_Exit(0);
}