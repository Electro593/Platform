/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
**                                                                         **
**  Author: Aria Seiler                                                    **
**                                                                         **
**  This program is in the public domain. There is no implied warranty,    **
**  so use it at your own risk.                                            **
**                                                                         **
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if !defined(INCLUDE_SOURCE) && !defined(INCLUDE_HEADER)
#include <shared.h>

#define INCLUDE_HEADER
#include <util/main.c>
#include <platform/platform.h>
#include "main.c"
#undef INCLUDE_HEADER

#define INCLUDE_SOURCE
#include "main.c"
#undef INCLUDE_SOURCE
#else
#ifdef INCLUDE_SOURCE
extern wayland_state _G;
extern wayland_funcs _F;
#endif

#include <wayland/api.c>
#include <wayland/wayland.c>

#if defined(INCLUDE_HEADER) && !defined(NO_SYMBOLS)

#define WAYLAND_MODULE_NAME CStringL("wayland")

#define WAYLAND_FUNCS \
	WAYLAND_API_FUNCS \
	WAYLAND_USER_FUNCS

typedef struct wayland_state {
	usize HeapSize;
	heap *Heap;

	file_handle Socket;

	b08 Connected : 1;
	b08 Attempted : 1;

	// TODO Re-use deleted object ids
	u32		NextObjectId;
	hashmap IdTable;

	wayland_display	 *Display;
	wayland_registry *Registry;
} wayland_state;

typedef struct wayland_funcs {
#define EXPORT(R, N, ...) R (*N)(__VA_ARGS__);
#define X WAYLAND_FUNCS
#include <x.h>
} wayland_funcs;

#if defined(_WAYLAND_MODULE)
#define DEFAULT(R, N, ...) \
            internal R N(__VA_ARGS__);
#define X WAYLAND_FUNCS
#include <x.h>
#else
#define EXPORT(R, N, ...) \
            global R (*N)(__VA_ARGS__);
#define X WAYLAND_FUNCS
#include <x.h>
#endif
#endif

#ifdef INCLUDE_SOURCE
wayland_state _G;
wayland_funcs _F;

external API_EXPORT void
Load(platform_state *Platform, platform_module *Module)
{
	_F = (wayland_funcs) {
#define EXPORT(R, N, ...) N,
#define X WAYLAND_FUNCS
#include <x.h>
	};

	Module->Data  = &_G;
	Module->Funcs = &_F;

#define EXPORT(R, S, N, ...) S##_##N = Platform->Functions.S##_##N;
#define X PLATFORM_FUNCS
#include <x.h>

	platform_module *UtilModule = Platform_LoadModule(UTIL_MODULE_NAME);
	util_funcs		*UtilFuncs	= UtilModule->Funcs;
#define EXPORT(R, N, ...) N = UtilFuncs->N;
#define X UTIL_FUNCS
#include <x.h>
}

external API_EXPORT void
Deinit(platform_state *Platform)
{
	Wayland_TryClose();
}

#endif
#endif
