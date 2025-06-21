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
#include "main.c"
#include <util/main.c>
#undef INCLUDE_HEADER

#include <platform/platform.h>

#define INCLUDE_SOURCE
#include "main.c"
#undef INCLUDE_SOURCE
#else
#ifdef INCLUDE_SOURCE
extern wayland_state _G;
extern wayland_funcs _F;
#endif

#include <wayland/wayland.c>

#if defined(INCLUDE_HEADER) && !defined(NO_SYMBOLS)

typedef struct wayland_state {
	b08 Initialized;
} wayland_state;

typedef struct wayland_funcs {
#define EXPORT(R, N, ...) R (*N)(__VA_ARGS__);
#define X WAYLAND_FUNCS
#include <x.h>
} wayland_funcs;

#if defined(_WAYLAND_MODULE)
#define EXPORT(R, N, ...) \
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

	platform_module *UtilModule = Platform_LoadModule("util", (vptr) 0x7DB000000000);
	util_funcs		*UtilFuncs	= UtilModule->Funcs;
#define EXPORT(R, N, ...) N = UtilFuncs->N;
#define X UTIL_FUNCS
#include <x.h>
}

#endif
#endif
