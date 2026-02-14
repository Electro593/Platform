/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                             *
*  Author: Aria Seiler                                                        *
*                                                                             *
*  This program is in the public domain. There is no implied warranty, so     *
*  use it at your own risk.                                                   *
*                                                                             *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if !defined(INCLUDE_SOURCE) && !defined(INCLUDE_HEADER)
#include <shared.h>

#define INCLUDE_HEADER
#include <util/main.c>
#ifdef _USE_LOADER
#include <loader/main.c>
#endif
#include "main.c"
#include <platform/opengl.h>
#undef INCLUDE_HEADER

#define INCLUDE_SOURCE
#include "main.c"
#undef INCLUDE_SOURCE
#else
#ifdef INCLUDE_SOURCE
global platform_state _G;
global platform_funcs _F;
#endif

#if defined(_WIN32)
#include <platform/win32/win32.c>
#include <platform/platform.c>
#elif defined(_LINUX)
#include <platform/linux/linux.c>
// #include <platform/linux/drm.c>
#include <platform/platform.c>
#include <platform/linux/wayland/api.c>
// #include <platform/linux/wayland/egl.c>
#include <platform/linux/wayland/wayland.c>
#endif

#ifdef INCLUDE_HEADER

#if defined(_WIN32)
#define PLATFORM_SPECIFIC_FUNCS
#elif defined(_LINUX)
#define PLATFORM_SPECIFIC_FUNCS \
	WAYLAND_API_FUNCS \
	WAYLAND_USER_FUNCS
#endif

#define PLATFORM_FUNCS \
	PLATFORM_SPECIFIC_FUNCS \
	PLATFORM_SHARED_FUNCS

struct platform_state {
	b08 CursorIsDisabled : 1;
	b08 WindowedApp		 : 1;
	b08 UtilIsLoaded	 : 1;
	b08 _Unused			 : 5;

	v2s32 RestoreCursorPos;
	v2s32 CursorPos;
	heap *Heap;

	hashmap ModuleTable;

	v2u32				   WindowSize;
	struct platform_funcs *Funcs;
	execution_state		   ExecutionState;
	focus_state			   FocusState;
	platform_updates	   Updates;
	u08					   Buttons[5];
	u08					   Keys[256];

	r32 FPS;

	usize	ArgCount;
	string *Args;

	hashmap EnvTable;

	wayland_api_state WaylandApi;
	wayland_state	  Wayland;
};

typedef struct platform_funcs {
#define EXPORT(R, N, ...) R (*N)(__VA_ARGS__);
#define X PLATFORM_FUNCS
#include <x.h>
} platform_funcs;

#if defined(_PLATFORM_MODULE)
#define DEFAULT(R, N, ...) internal R N(__VA_ARGS__);
#else
#define EXPORT(R, N, ...) global R (*N)(__VA_ARGS__);
#endif
#define X PLATFORM_FUNCS
#include <x.h>
#endif

#endif
