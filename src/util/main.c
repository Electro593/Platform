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
#include <platform/main.c>
#undef INCLUDE_HEADER

#define INCLUDE_SOURCE
#include "main.c"
#undef INCLUDE_SOURCE
#else
#ifdef INCLUDE_SOURCE
extern struct util_state _G;
extern struct util_funcs _F;
#endif

#include <util/intrin.h>
#include <util/scalar.c>
#include <util/vector.c>
#include <util/memory.c>
#include <util/bigint.c>
#include <util/string.c>
#include <util/set.c>
#include <util/msdf.c>
#include <util/font.c>
#include <util/file.c>

#ifdef INCLUDE_HEADER

#define UTIL_MODULE_NAME CStringL("util")

#define UTIL_FUNCS \
    SCALAR_FUNCS   \
    VECTOR_FUNCS   \
    MEMORY_FUNCS   \
    BIGINT_FUNCS   \
    STRING_FUNCS   \
    SET_FUNCS      \
    MSDF_FUNCS     \
    FONT_FUNCS     \
    FILE_FUNCS

typedef struct util_state {
	stack *Stack;
} util_state;

typedef struct util_funcs {
#define EXPORT(R, N, ...) R (*N)(__VA_ARGS__);
#define X UTIL_FUNCS
#include <x.h>
} util_funcs;

#if defined(_UTIL_MODULE)
#define DEFAULT(R, N, ...) internal R N(__VA_ARGS__);
#define X UTIL_FUNCS
#include <x.h>
#else
#define EXPORT(R, N, ...) global R (*N)(__VA_ARGS__);
#define X UTIL_FUNCS
#include <x.h>
#endif
#endif

#ifdef INCLUDE_SOURCE
util_state _G;
util_funcs _F;

external void
Load(platform_state *Platform, platform_module *Module)
{
	platform_funcs *PlatformFuncs = Platform->Funcs;
#define EXPORT(R, N, ...) N = PlatformFuncs->N;
#define X PLATFORM_FUNCS
#include <x.h>

	_F = (util_funcs) {
#define EXPORT(R, N, ...) N,
#define X UTIL_FUNCS
#include <x.h>
	};

	Module->Data  = &_G;
	Module->Funcs = &_F;
}

#define TEST(FunctionName, TestName, TestCode)         \
	static void Test_##FunctionName##_##TestName(void) \
	{                                                  \
		MAC_UNPACKAGE(TestCode)                        \
	}
BIGINT_TESTS
STRING_TESTS
#undef TEST

external void
Init(platform_state *Platform)
{
	b08 RunTests = FALSE;
	for (usize I = 1; I < Platform->ArgCount; I++) {
		if (String_Cmp(Platform->Args[I], CStringL("-RunTests")) == 0) {
			RunTests = TRUE;
			break;
		}
	}

	if (RunTests) {
		Stack_Push();

#define TEST(FunctionName, TestName, TestCode)                                           \
		Platform_WriteConsole(CStringL("Testing " #FunctionName ": " #TestName "...\n")); \
		Test_##FunctionName##_##TestName();

		Platform_WriteConsole(CStringL("\n===== BigInt Tests ======\n"));
		BIGINT_TESTS

		Platform_WriteConsole(CStringL("\n===== String Tests ======\n"));
		STRING_TESTS

#undef TEST

		Platform_WriteConsole(CStringL("\nAll tests passed!\n"));
		Stack_Pop();
	}
}
#endif
#endif
