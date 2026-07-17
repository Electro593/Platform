/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                             *
*  Author: Aria Seiler                                                        *
*                                                                             *
*  This program is in the public domain. There is no implied warranty, so     *
*  use it at your own risk.                                                   *
*                                                                             *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef _PLATFORM_SHARED_H
#define _PLATFORM_SHARED_H

typedef struct platform_state  platform_state;
typedef struct platform_module platform_module;
typedef struct opengl_funcs	   opengl_funcs;

typedef struct datetime		 datetime;
typedef struct timestamp	 timestamp;
typedef struct mutex_handle	 mutex_handle;
typedef struct file_handle	 file_handle;
typedef struct thread_handle thread_handle;

#if defined(_WIN32)

#include <platform/win32/win32.c>

struct datetime {
	win32_file_time Value;
};

struct timestamp {
	s64 Value;
};

struct mutex_handle {
	win32_handle Handle;
};

struct file_handle {
	win32_handle Handle;
};

struct thread_handle {
	s32 ThreadId;
};

#define NULL_FILE_HANDLE (file_handle){ .Handle = INVALID_HANDLE_VALUE }

#elif defined(_LINUX)

#include <platform/linux/linux.h>

struct datetime {
	sys_statx_timestamp Value;
};

struct timestamp {
	sys_timespec Value;
};

struct mutex_handle {
	u32 Value;
};

struct file_handle {
	u32 FileDescriptor;
};

struct thread_handle {
	s32	  ThreadId;
	vptr  Stack;
	usize StackSize;
};

#define NULL_FILE_HANDLE (file_handle){ .FileDescriptor = SYS_FILE_NONE }

#else
#error Unsupported platform
#endif

#endif
