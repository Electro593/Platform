/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
**                                                                         **
**  Author: Aria Seiler                                                    **
**                                                                         **
**  This program is in the public domain. There is no implied warranty,    **
**  so use it at your own risk.                                            **
**                                                                         **
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef INCLUDE_HEADER

typedef struct bigint {
	u64 *Words;
	u32	 WordCount;
	u32	 MaxCount;
} bigint;

typedef struct bigint_divmod {
	bigint Quotient;
	bigint Remainder;
} bigint_divmod;

#define BIGINT_FUNCS \
	EXPORT(bigint,        BigInt_Init,    u32 MaxCount) \
	EXPORT(bigint,        BigInt_InitV,   u64 Value) \
	EXPORT(bigint,        BigInt_Splice,  bigint A, u32 Start, u32 Count) \
	EXPORT(void,          BigInt_Copy,    bigint *Destination, bigint Source) \
	EXPORT(s32,           BigInt_Compare, bigint A, bigint B) \
	EXPORT(bigint,        BigInt_Add,     bigint A, bigint B) \
	// EXPORT(bigint_divmod, BigInt_DivMod,  bigint A, bigint B) \

#endif

#ifdef INCLUDE_SOURCE

internal bigint
BigInt_Init(u32 MaxCount)
{
	u64 *Words = Stack_Allocate(MaxCount);
	Mem_Set(Words, 0, MaxCount * sizeof(u64));

	return (bigint) {Words, 0, MaxCount};
}

internal bigint
BigInt_InitV(u64 Value)
{
	if (!Value) return BigInt_Init(0);
	bigint Result	 = BigInt_Init(1);
	Result.Words[0]	 = Value;
	Result.WordCount = 1;
	return Result;
}

internal bigint
BigInt_Splice(bigint A, u32 Start, u32 Count)
{
	bigint Result = BigInt_Init(Count);
	Mem_Cpy(Result.Words, A.Words + Start, Count * sizeof(u64));
	Result.WordCount = Count;
	return Result;
}

internal void
BigInt_Copy(bigint *Destination, bigint Source)
{
	Assert(Destination->MaxCount >= Source.WordCount);
	Mem_Cpy(Destination->Words, Source.Words, Source.WordCount * sizeof(u64));
	Destination->WordCount = Source.WordCount;
}

internal s32
BigInt_Compare(bigint A, bigint B)
{
	s64 Count = U32_Max(A.WordCount, B.WordCount);

	while (Count--) {
		u64 AW = Count < A.WordCount ? A.Words[Count] : 0;
		u64 BW = Count < B.WordCount ? B.Words[Count] : 0;
		if (AW < BW) return LESS;
		if (AW > BW) return GREATER;
	}

	return EQUAL;
}

internal bigint
BigInt_Add(bigint A, bigint B)
{
	u32	   Count  = U32_Max(A.WordCount, B.WordCount);
	bigint Result = BigInt_Init(Count + 1);

	u64 Carry = 0;
	for (u32 I = 0; I < Count; I++) {
		u64 AW	= I < A.WordCount ? A.Words[I] : 0;
		u64 BW	= I < B.WordCount ? B.Words[I] : 0;
		u64 Sum = AW + BW + Carry;
		Carry	= Sum < AW || (Sum == AW && Carry);

		if (!Sum && !Carry) break;

		Result.WordCount += I == Result.WordCount;
		Result.Words[I]	  = Sum;
	}

	return Result;
}

// internal void
// BigInt_DivModS(
// 	bigint Numerator,
// 	u64 Denominator,
//    bigint *Quotient,
//    u64 *Remainder)
// {
// 	bigint_divmod Result;
//
//    bigint A = Numerator, *Q = Quotient;
//    u64 B = Denominator, *R = Remainder;
//
// 	Assert(B, "Divisor must be greater than 0");
//    Assert(Q && R, "Quotient and Remainder cannot be null");
//    Assert(Q->MaxCount >= A.WordCount, "Quotient is too small");
//
// 	if(!A.WordCount) {
//       Q->WordCount = 0;
//       *R = 0;
//       return;
//    }
//
//
//
//    Q->WordCount = Q->Words[A.WordCount-1] == 0;
// }

#endif
