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

intrin u64
Intrin_ReadGSQWord(u32 Offset)
{
	u64 Result;
	__asm__("mov %%gs:0(%1), %0" : "=r"(Result) : "r"(Offset));
	return Result;
}

intrin void
Intrin_DebugBreak()
{ __asm__("int3"); }

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

intrin u32
Intrin_Exchange32(u32 *Data, u32 Value)
{
	__asm__("lock xchg %1, %2" : "+r"(Value) : "m"(*Data));
	return Value;
}

intrin u08
Intrin_CompareExchange08(u08 *Value, u08 Target, u08 NewValue)
{
	__asm__("lock cmpxchg %1, %2" : "+a"(Target) : "r"(NewValue), "m"(*Value));
	return Target;
}

intrin u16
Intrin_CompareExchange16(u16 *Value, u16 Target, u16 NewValue)
{
	__asm__("lock cmpxchg %1, %2" : "+a"(Target) : "r"(NewValue), "m"(*Value));
	return Target;
}

intrin u32
Intrin_CompareExchange32(u32 *Value, u32 Target, u32 NewValue)
{
	__asm__("lock cmpxchg %1, %2" : "+a"(Target) : "r"(NewValue), "m"(*Value));
	return Target;
}

intrin u64
Intrin_CompareExchange64(u64 *Value, u64 Target, u64 NewValue)
{
	u64 OldValue = Target;
	__asm__("lock cmpxchg %1, %2" : "+a"(Target) : "r"(NewValue), "m"(*Value));
	return Target;
}

#endif

#endif
