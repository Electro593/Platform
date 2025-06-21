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
#include <platform/platform.h>
#include <util/main.c>
#undef INCLUDE_HEADER

#define INCLUDE_SOURCE
#include "main.c"
#undef INCLUDE_SOURCE
#else
#ifdef INCLUDE_SOURCE
extern $module$_state _G;
extern $module$_funcs _F;
#endif

// #include <...>

#if defined(INCLUDE_HEADER) && !defined(NO_SYMBOLS)
#define $MODULE$_FUNCS

typedef struct $module$_state {
} $module$_state;

typedef struct $module$_funcs {
#define EXPORT(R, N, ...) R (*N)(__VA_ARGS__);
#define X $MODULE$_FUNCS
#include <x.h>
} $module$_funcs;

#if defined(_$MODULE$_MODULE)
#define EXPORT(R, N, ...) \
            internal R N(__VA_ARGS__);
#define X $MODULE$_FUNCS
#include <x.h>
#else
#define EXPORT(R, N, ...) \
            global R (*N)(__VA_ARGS__);
#define X $MODULE$_FUNCS
#include <x.h>
#endif
#endif

#ifdef INCLUDE_SOURCE
$module$_state _G;
$module$_funcs _F;

external API_EXPORT void
Load(platform_state *Platform, platform_module *Module)
{
	_F = {
#define EXPORT(R, N, ...) \
               N,
#define X FUNCS
#include <x.h>
		0
	};

	Module->Data  = &_G;
	Module->Funcs = &_F;

#define EXPORT(R, S, N, ...) S##_##N = Platform->Functions.S##_##N;
#define X PLATFORM_FUNCS
#include <x.h>

	platform_module *UtilModule = Platform_LoadModule("util");
	util_funcs		*UtilFuncs	= UtilModule->Funcs;
#define EXPORT(R, N, ...) N = UtilFuncs->N;
#define X UTIL_FUNCS
#include <x.h>
}

external API_EXPORT void
Init(platform_state *Platform)
{
}

external API_EXPORT void
Update(platform_state *Platform)
{
}

external API_EXPORT void
Unload(platform_state *Platform)
{
}
#endif
#endif
