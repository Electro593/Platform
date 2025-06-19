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
#include <platform/linux/linux.h>

typedef struct file_handle {
	int dummy;
 } file_handle;
 
internal opengl_funcs*
Platform_LoadOpenGL(void)
{
	return 0;
}

internal void
Platform_CreateWindow(void)
{
	
}

internal void
Platform_Assert(c08 *File, u32 Line, c08 *Expression, c08 *Message)
{
	
}

internal vptr
Platform_AllocateMemory(u64 Size)
{
	return 0;
}

internal void
Platform_CloseFile(file_handle *FileHandle)
{
	
}

internal void
Platform_FreeMemory(vptr Base)
{
	
}

internal u64
Platform_GetFileLength(file_handle *FileHandle)
{
	return 0;
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

internal platform_module*
Platform_LoadModule(c08 *Name)
{
	return 0;
}

internal s08
Platform_CmpFileTime(datetime A, datetime B)
{
	return 0;
}

internal b08
Platform_OpenFile(file_handle **FileHandle, c08 *FileName, file_mode OpenMode)
{
	return 0;
}

internal u64
Platform_ReadFile(file_handle *FileHandle, vptr Dest, u64 Length, u64 Offset)
{
	return 0;
}

internal void
Platform_WriteConsole(string Message)
{
	
}

internal void
Platform_WriteError(string Message, u32 Exit)
{
	
}

internal u64
Platform_WriteFile(file_handle *FileHandle, vptr Src, u64 Length, u64 Offset)
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
	Platform_Exit(0);
}