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
#include <platform/main.c>
#include "main.c"
#undef INCLUDE_HEADER

#define INCLUDE_SOURCE
#include "main.c"
#undef INCLUDE_SOURCE
#else
#ifdef INCLUDE_SOURCE
extern renderer_opengl_state _G;
extern renderer_opengl_funcs _F;
#endif

#include <renderer/renderer.c>

#ifdef INCLUDE_HEADER

#define RENDERER_OPENGL_MODULE_NAME CStringL("renderer_opengl")

#define RENDERER_OPENGL_FUNCS \
	RENDERER_FUNCS

typedef struct renderer_opengl_state {
} renderer_opengl_state;

typedef struct renderer_opengl_funcs {
#define EXPORT(R, N, ...) R (*N)(__VA_ARGS__);
#define X RENDERER_OPENGL_FUNCS
#include <x.h>
} renderer_opengl_funcs;

#if defined(_RENDERER_OPENGL_MODULE)
#define EXPORT(R, N, ...) \
            internal R N(__VA_ARGS__);
#define X RENDERER_OPENGL_FUNCS
#include <x.h>
#else
#define EXPORT(R, N, ...) \
            global R (*N)(__VA_ARGS__);
#define X RENDERER_OPENGL_FUNCS
#include <x.h>
#endif
#endif

#ifdef INCLUDE_SOURCE
renderer_opengl_state _G;
renderer_opengl_funcs _F;

external void
Load(platform_state *Platform, platform_module *Module)
{
	_F = (renderer_opengl_funcs) {
#define EXPORT(R, N, ...) N,
#define X RENDERER_OPENGL_FUNCS
#include <x.h>
	};

	Module->Data  = &_G;
	Module->Funcs = &_F;

	platform_funcs *PlatformFuncs = Platform->Funcs;
#define EXPORT(R, N, ...) N = PlatformFuncs->N;
#define X PLATFORM_FUNCS
#include <x.h>

	platform_module *UtilModule = Platform_LoadModule(UTIL_MODULE_NAME);
	util_funcs		*UtilFuncs	= UtilModule->Funcs;
#define EXPORT(R, N, ...) N = UtilFuncs->N;
#define X UTIL_FUNCS
#include <x.h>
}

external void
Init(platform_state *Platform)
{ }

external void
Update(platform_state *Platform)
{ }

external void
Unload(platform_state *Platform)
{ }
#endif
#endif
