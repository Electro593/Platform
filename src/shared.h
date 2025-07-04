/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
**                                                                         **
**  Author: Aria Seiler                                                    **
**                                                                         **
**  This program is in the public domain. There is no implied warranty,    **
**  so use it at your own risk.                                            **
**                                                                         **
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if defined(_MSVC)
#define API_ENTRY  __stdcall
#define API_IMPORT __declspec(dllimport)
#define API_EXPORT __declspec(dllexport)
#define UNREACHABLE __assume(false)
#elif defined(_GCC)
#define API_ENTRY
#define API_IMPORT
#define API_EXPORT
#define UNREACHABLE __builtin_unreachable();
#endif

#define global   static
#define persist  static
#define internal static inline
#define external

#define RETURNS(...)

#define FORCE_CAST(Type, Variable)               (*(Type*)&(Variable))
#define LITERAL_CAST(OldType, Constant, NewType) (*(NewType*)&(OldType){Constant})
#define OFFSET_OF(Type, Element) ((u64)&(((Type*)0)->Element))
#define INDEX_2D(X, Y, MaxX) ((X) + ((Y) * (MaxX)))
#define INDEX_3D(X, Y, Z, MaxX, MaxY) ((X) + ((Y) * (MaxX)) + ((Z) * (MaxX) * (MaxY)))
#define SWAP(A, B, Type) do { Type _A = A; A = B; B = _A; } while(0)
#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define MAX(A, B) ((A) > (B) ? (A) : (B))
#define FLAG_SET(Value, Flag) (((Value) & (Flag)) == (Flag))
#define ALIGN_UP(Value, Size) (((Value) + ((Size) - 1)) & ~((Size) - 1))

#include <macro.h>

#define Error(Message) do { Platform_Assert(__FILE__, __LINE__, "", Message); STOP; } while(0)
#define Assert(Expression, ...) \
    do { \
        if(!(Expression)) { \
            Platform_Assert(__FILE__, __LINE__, #Expression, "" __VA_ARGS__); \
            STOP; \
        } \
    } while(0)

#ifdef _DEBUG
#define STOP Intrin_DebugBreak()
#define NOP Intrin_Nop()
#else
#define STOP Platform_Exit(-1)
#define NOP
#endif

#define NULL ((vptr)0)
#define FALSE 0
#define TRUE  1
#define LESS  (-1)
#define EQUAL   0
#define GREATER 1

#ifdef _MSVC
typedef signed __int8  s08;
typedef signed __int16 s16;
typedef signed __int32 s32;
typedef signed __int64 s64;

typedef unsigned __int8	 u08;
typedef unsigned __int16 u16;
typedef unsigned __int32 u32;
typedef unsigned __int64 u64;
#else
typedef signed char		 s08;
typedef signed short	 s16;
typedef signed int		 s32;
typedef signed long long s64;
static_assert(sizeof(s08) == 1, "s08 must be 1 byte!");
static_assert(sizeof(s16) == 2, "s16 must be 2 bytes!");
static_assert(sizeof(s32) == 4, "s32 must be 4 bytes!");
static_assert(sizeof(s64) == 8, "s64 must be 8 bytes!");

typedef unsigned char	   u08;
typedef unsigned short	   u16;
typedef unsigned int	   u32;
typedef unsigned long long u64;
static_assert(sizeof(u08) == 1, "u08 must be 1 byte!");
static_assert(sizeof(u16) == 2, "u16 must be 2 bytes!");
static_assert(sizeof(u32) == 4, "u32 must be 4 bytes!");
static_assert(sizeof(u64) == 8, "u64 must be 8 bytes!");
#endif

#ifdef _X64
typedef s64 ssize;
typedef u64 usize;

#define SSIZE_MIN ((ssize)0x8000000000000000LL)
#define SSIZE_MAX ((ssize)0x7FFFFFFFFFFFFFFFLL)
#define USIZE_MIN ((usize)0x0000000000000000ULL)
#define USIZE_MAX ((usize)0xFFFFFFFFFFFFFFFFULL)
#else
typedef s32 ssize;
typedef u32 usize;

#define SSIZE_MIN ((ssize)0x80000000)
#define SSIZE_MAX ((ssize)0x7FFFFFFF)
#define USIZE_MIN ((usize)0x00000000U)
#define USIZE_MAX ((usize)0xFFFFFFFFU)
#endif

typedef float  r32;
typedef double r64;
static_assert(sizeof(r32) == 4, "r32 must be 4 bytes!");
static_assert(sizeof(r64) == 8, "r64 must be 8 bytes!");

typedef void *vptr;
typedef void (*fptr)(void);

typedef s08 b08;
typedef s16 b16;
typedef s32 b32;
typedef s64 b64;

typedef char c08;
typedef u16	 c16;
typedef u32	 c32;

typedef struct platform_state	platform_state;
typedef struct platform_exports platform_exports;

#define TYPES \
    ENUM(S08,  s08) \
    ENUM(S16,  s16) \
    ENUM(S32,  s32) \
    ENUM(S64,  s64) \
    \
    ENUM(U08,  u08) \
    ENUM(U16,  u16) \
    ENUM(U32,  u32) \
    ENUM(U64,  u64) \
    \
    ENUM(R32,  r32) \
    ENUM(R64,  r64) \
    \
    ENUM(C08,  c08) \
    ENUM(STR,  c08*) \
    \
    ENUM(VPTR, vptr)

typedef enum type_id {
	TYPEID_NONE,

#define ENUM(Name, Type) \
        TYPEID_##Name,
	TYPES
#undef ENUM

		TYPEID_EXTRA_EXP = 8,
	TYPEID_MOD_EXP		 = 4,

	TYPEID_TYPE_MASK  = (1 << TYPEID_EXTRA_EXP) - 1,
	TYPEID_MOD_MASK	  = ~((1 << (32 - TYPEID_MOD_EXP)) - 1),
	TYPEID_EXTRA_MASK = ~(TYPEID_TYPE_MASK | TYPEID_MOD_MASK),

	TYPEID_MEMBER = 0x10000000,
} type_id;

typedef struct type {
	type_id ID;
	u16		Size;
} type;

#ifndef NO_SYMBOLS

global type TYPE_NONE = { TYPEID_NONE, 0 };
#define ENUM(Name, Type) \
    global type TYPE_##Name = {TYPEID_##Name, sizeof(Type)};
TYPES
#undef ENUM

#endif

// TODO fix this whole mess up
internal type
MakeMemberType(type_id TypeID, u32 Offset, u16 Size)
{
	return (type) { TYPEID_MEMBER | ((Offset << TYPEID_EXTRA_EXP) & TYPEID_MOD_MASK) | TypeID,
					Size };
}

typedef s08 (*cmp_func)(vptr A, vptr B, vptr Param);
typedef usize (*hash_func)(vptr Data, vptr Param);

#ifdef _WIN32
// Win32 depends on some weird symbols, so we add them here
s32 _fltused;
#endif

#ifdef __MINGW32__
s32
DllMainCRTStartup()
{
	return TRUE;
}
#endif
