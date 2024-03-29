/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
**                                                                         **
**  Author: Aria Seiler                                                    **
**                                                                         **
**  This program is in the public domain. There is no implied warranty,    **
**  so use it at your own risk.                                            **
**                                                                         **
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// #define _OPENGL

#if defined(_MSVC)
    #define API_ENTRY  __stdcall
    #define API_IMPORT __declspec(dllimport)
    #define API_EXPORT __declspec(dllexport)
#elif defined(_GCC)
    #define API_ENTRY
    #define API_IMPORT
    #define API_EXPORT
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

#define _STRINGIFY(X) #X
#define STRINGIFY(X) _STRINGIFY(X)

#define Error(Message) Platform_Assert(__FILE__, __LINE__, "", Message)
#define Assert(Expression, ...) \
    do { \
        if(!(Expression)) { \
            Platform_Assert(__FILE__, __LINE__, #Expression, "" __VA_ARGS__); \
            STOP; \
        } \
    } while(0)

#ifdef _DEBUG
    #define STOP Intrin_DebugBreak()
    #define NOP Intrin_Nop();
#else
    #define STOP
    #define NOP
#endif

#define NULL ((vptr)0)
#define FALSE 0
#define TRUE  1
#define LESS  (-1)
#define EQUAL   0
#define GREATER 1

typedef signed   __int8  s08;
typedef signed   __int16 s16;
typedef signed   __int32 s32;
typedef signed   __int64 s64;

typedef unsigned __int8  u08;
typedef unsigned __int16 u16;
typedef unsigned __int32 u32;
typedef unsigned __int64 u64;

typedef float  r32;
typedef double r64;

typedef void* vptr;
typedef void (*fptr)(void);

typedef s08 b08;
typedef s16 b16;
typedef s32 b32;
typedef s64 b64;

typedef u08 c08;
typedef u16 c16;
typedef u32 c32;

typedef struct platform_state platform_state;
typedef struct platform_exports platform_exports;

#define TYPES \
    ENUM(U32,    u32) \
    ENUM(U64,    u64) \
    ENUM(STR,    c08*) \
    ENUM(VPTR,   vptr) \

typedef enum type_id {
    TYPEID_NONE,
    
    #define ENUM(Name, Type) \
        TYPEID_##Name,
    TYPES
    #undef ENUM
    
    TYPEID_EXTRA_EXP = 8,
    TYPEID_MOD_EXP   = 4,
    
    TYPEID_TYPE_MASK  = (1<<TYPEID_EXTRA_EXP)-1,
    TYPEID_MOD_MASK   = ~((1<<(32-TYPEID_MOD_EXP))-1), 
    TYPEID_EXTRA_MASK = ~(TYPEID_TYPE_MASK | TYPEID_MOD_MASK),
    
    TYPEID_MEMBER   = 0x10000000,
} type_id;

typedef struct type {
    type_id ID;
    u32 Size;
} type;

global type TYPE_NONE = {TYPEID_NONE, 0};
#define ENUM(Name, Type) \
    global type TYPE_##Name = {TYPEID_##Name, sizeof(Type)};
TYPES
#undef ENUM

#undef TYPES

internal type MakeMemberType(type_id TypeID, u32 Offset, u32 Size) {
    return (type){TYPEID_MEMBER | ((Offset<<TYPEID_EXTRA_EXP)&TYPEID_MOD_MASK) | TypeID, Size};
}

s32 _fltused;