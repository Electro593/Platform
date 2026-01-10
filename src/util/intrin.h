/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                             *
*  Author: Aria Seiler                                                        *
*                                                                             *
*  This program is in the public domain. There is no implied warranty, so     *
*  use it at your own risk.                                                   *
*                                                                             *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef INCLUDE_HEADER

#ifdef _X64

#ifdef _MSVC

typedef union __declspec(intrin_type) __declspec(align(16)) __m128 {
	r32 R32[4];
	r64 R64[2];
} r128;

void __debugbreak(void);
void __nop(void);
u64	 __rdtsc(void);
u64	 __readgsqword(u32 Offset);
u64	 __popcnt64(u64 Value);
u08	 _BitScanForward64(u32 *Index, u64 Mask);
u08	 _BitScanReverse(u32 *Index, u32 Mask);
u08	 _BitScanReverse64(u32 *Index, u64 Mask);
r128 _mm_sqrt_ps(r128);
r128 _mm_set_ps(r32, r32, r32, r32);

#define Intrin_ReadGSQWord(u32_Offset)                     RETURNS(u64)  __readgsqword(u32_Offset)
#define Intrin_DebugBreak()                             RETURNS(void) __debugbreak()
#define Intrin_Nop()                                    RETURNS(void) __nop()
#define Intrin_Popcount64(u64_Value)                    RETURNS(u64)  __popcnt64(u64_Value)
#define Intrin_ReadTimeStampCounter()                      RETURNS(u64)  __rdtsc()
#define Intrin_BitScanForward64(u32_p_Index, u64_Value) RETURNS(b08)  _BitScanForward64(u32_p_Index, u64_Value)
#define Intrin_BitScanReverse32(u32_p_Index, u32_Value) RETURNS(b08)  _BitScanReverse(u32_p_Index, u32_Value)
#define Intrin_BitScanReverse64(u32_p_Index, u64_Value) RETURNS(b08)  _BitScanReverse(u32_p_Index, u64_Value)

inline r32
Intrin_Sqrt_R32(r32 Value)
{
	return _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(Value)));
}

typedef u08 *va_list;
void		 __va_start(va_list *Args, ...);
#define VA_Start(Args, Start) ((void)(__va_start(&Args, Start)))
#define VA_Next(Args, Type) \
	((sizeof(Type) > 8 || (sizeof(Type) & (sizeof(Type) - 1)) != 0) /*Pointer or not a power of 2*/ \
		? **(Type**)((Args += 8) - 8) \
		:  *(Type* )((Args += 8) - 8))
#define VA_Copy(Dest, Src) ((Dest) = (Src))
#define VA_End(Args) ((void)(Args = NULL))

#elif defined(_GCC)

intrin u64
Intrin_ReadGSQWord(u32 Offset)
{
	u64 Result;
	__asm__("mov %%gs:0(%1), %0" : "=r"(Result) : "r"(Offset));
	return Result;
}

#define Intrin_DebugBreak() __asm__ ( "int3" )
#define Intrin_Nop() __asm__ ( "nop" )

intrin u16
Intrin_ByteSwap16(u16 Value)
{
	__asm__("xchg %%al, %%ah" : "+a"(Value));
	return Value;
}

intrin u32
Intrin_ByteSwap32(u32 Value)
{
	__asm__("bswap %0" : "+r"(Value));
	return Value;
}

intrin u64
Intrin_ByteSwap64(u64 Value)
{
	__asm__("bswapq %0" : "+r"(Value));
	return Value;
}

intrin u64
Intrin_Popcount64(u64 Value)
{
	__asm__("popcnt %0, %0" : "+r"(Value));
	return Value;
}

intrin u64
Intrin_ReadTimeStampCounter()
{
	u64 Result;
	__asm__ volatile(
		"rdtsc\n"
		"shlq $32, %%rdx\n"
		"orq %%rdx, %%rax\n"
		: "=a"(Result)
	);
	return Result;
}

intrin b08
Intrin_BitScanForward64(u32 *Index, u64 Value)
{
	u64 Index64;
	__asm__("bsf %1, %0" : "=r"(Index64) : "r"(Value));
	*Index = Index64;
	return Value != 0;
}

intrin b08
Intrin_BitScanReverse32(u32 *Index, u32 Value)
{
	__asm__("bsr %1, %0" : "=r"(*Index) : "r"(Value));
	return Value != 0;
}

intrin b08
Intrin_BitScanReverse64(u32 *Index, u64 Value)
{
	u64 Index64;
	__asm__("bsr %1, %0" : "=r"(Index64) : "r"(Value));
	*Index = Index64;
	return Value != 0;
}

#define Intrin_BitScanForward MAC_CONCAT(Intrin_BitScanForward, _WORD_SIZE)
#define Intrin_BitScanReverse MAC_CONCAT(Intrin_BitScanReverse, _WORD_SIZE)

intrin r32
Intrin_Sqrt_R32(r32 Value)
{
	__asm__("sqrtss %1, %0" : "+x"(Value));
	return Value;
}

typedef __builtin_va_list va_list;
#define VA_Start(Args, ...) __builtin_c23_va_start(Args)
#define VA_Next(Args, Type) __builtin_va_arg(Args, Type)
#define VA_Copy(Dest, Src) __builtin_va_copy(Dest, Src)
#define VA_End(Args) __builtin_va_end(Args)

intrin usize
Intrin_CompareExchange(usize *Mutex, usize Target, usize NewValue)
{
	usize OldValue = Target;
	__asm__("lock cmpxchg %1, %2"
			: "+a"(Target)
			: "r"(NewValue), "m"(Mutex)
			: "a");
	return OldValue;
}

#endif

#endif

#endif
