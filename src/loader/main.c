/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                             *
*  Author: Aria Seiler                                                        *
*                                                                             *
*  This program is in the public domain. There is no implied warranty, so     *
*  use it at your own risk.                                                   *
*                                                                             *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if !defined(INCLUDE_HEADER) && !defined(INCLUDE_SOURCE)
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

#ifdef _LINUX
#include <loader/linux/entry.c>
#endif

#ifdef INCLUDE_HEADER

#define LOADER_FUNCS \
	EXTERN(vptr, Loader_OpenShared, c08 *Name) \
	EXTERN(vptr, Loader_GetSymbol, vptr Module, c08 *Name) \
	EXTERN(void, Loader_CloseShared, vptr Module) \
	//

#ifdef _LOADER_MODULE
#define DEFAULT(R, N, ...) internal R N(__VA_ARGS__);
#define EXPORT(R, N, ...) external R N(__VA_ARGS__);
#define EXTERN(R, N, ...) external R N(__VA_ARGS__);
#else
#define EXPORT(R, N, ...) global R (*N)(__VA_ARGS__);
#define EXTERN(R, N, ...) extern R N(__VA_ARGS__);
#endif
#define X LOADER_FUNCS
#include <x.h>

#endif	// INCLUDE_HEADER

#ifdef INCLUDE_SOURCE

#endif	// INCLUDE_SOURCE

#endif
