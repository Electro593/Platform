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
	u32 WordCount;
	u32 MaxCount;
} bigint;

typedef struct bigint_divmod {
	bigint Quotient;
	bigint Remainder;
} bigint_divmod;

#define BIGINT_FUNCS \
	EXPORT(bigint,        BigInt_Init,    u32 MaxCount) \
	EXPORT(bigint,        BigInt_InitV,   u64 Value) \
	EXPORT(bigint,        BigInt_Splice,  bigint A, u32 Start, u32 Count) \
	EXPORT(bigint,        BigInt_Copy,    bigint A) \
	EXPORT(s32,           BigInt_Compare, bigint A, bigint B) \
	EXPORT(bigint,        BigInt_Add,     bigint A, bigint B) \
	EXPORT(bigint_divmod, BigInt_DivMod,  bigint A, bigint B) \

#endif

#ifdef INCLUDE_SOURCE

internal bigint
BigInt_Init(
	u32 MaxCount)
{
	u64 *Words = Stack_Allocate(MaxCount);
	Mem_Set(Words, 0, MaxCount * sizeof(u64));
	
	return (bigint){
		Words,
		0,
		MaxCount
	};
}

internal bigint
BigInt_InitV(
	u64 Value)
{
	if(!Value) return BigInt_Init(0);
	bigint Result = BigInt_Init(1);
	Result.Words[0] = Value;
	Result.WordCount = 1;
	return Result;
}

internal bigint
BigInt_Splice(
	bigint A,
	u32 Start,
	u32 Count)
{
	bigint Result = BigInt_InitM(Count);
	Mem_Cpy(Result.Words, A.Words + Start, Count * sizeof(u64));
	Result.WordCount = Count;
	return Result;
}

internal bigint
BigInt_Copy(
	bigint A)
{
	return BigInt_Splice(A, 0, A.WordCount);
}

internal s32
BigInt_Compare(
	bigint A,
	bigint B)
{
	s64 Count = U32_Max(A.WordCount, B.WordCount);
	
	while(Count--) {
		u64 AW = Count < A.WordCount ? A.Words[Count] : 0;
		u64 BW = Count < B.WordCount ? B.Words[Count] : 0;
		if(AW < BW) return LESS;
		if(AW > BW) return GREATER;
	}
	
	return EQUAL;
}

internal bigint
BigInt_Add(
	bigint A,
	bigint B)
{
	u32 Count = U32_Max(A.WordCount, B.WordCount);
	bigint Result = BigInt_Init(Count + 1);
	
	u64 Carry = 0;
	for(u32 I = 0; I < Count; I++) {
		u64 AW = I < A.WordCount ? A.Words[I] : 0;
		u64 BW = I < B.WordCount ? B.Words[I] : 0;
		u64 Sum = AW + BW + Carry;
		Carry = Sum < AW || (Sum == AW && Carry);
		
		if(!Sum && !Carry) break;
		
		Result.WordCount += I == Result.WordCount;
		Result.Words[I] = Sum;
	}
	
	return Result;
}

//CREDIT: https://mathsanew.com/articles/implementing_large_integers_division.pdf
internal bigint_divmod
BigInt_DivMod(
	bigint A,
	bigint B)
{
	bigint_divmod Result;
	
	Assert(B.WordCount, "Divisor must be greater than 0");
	
	if(!A.WordCount) {
		Result.Quotient = BigInt_InitV(0);
		Result.Remainder = BigInt_InitV(0);
		return Result;
	}
	if(B.WordCount == 1 && B.Words[1] == 1) {
		Result.Quotient = BigInt_Copy(A);
		Result.Remainder = BigInt_InitV(0);
		return Result;
	}
	
	s32 Cmp = BigInt_Compare(A, B);
	if(Cmp == LESS) {
		Result.Quotient = BigInt_InitV(0);
		Result.Remainder = BigInt_Copy(A);
		return Result;
	}
	if(Cmp == EQUAL) {
		Result.Quotient = BigInt_InitV(1);
		Result.Remainder = BigInt_InitV(0);
		return Result;
	}
	
	bigint IDD = BigInt_Init(B.WordCount + 1);
	IDD.WordCount = B.WordCount + 1;
	u32 I = A.WordCount - B.WordCount;
	Result.Quotient = BigInt_Init(I+1);
	Stack_Push();
	
	bigint AF = A
	bigint BF = B;
	
	u64 E = BF.Words[BF.WordCount-1];
	bigint F = BigInt_InitV(1);
	u64 HalfRadix = 1ull << 32;
	if(BF.WordCount > 1 && E < HalfRadix) {
		u64 Denom = E + 1;
		F = BigInt_InitV(2*(HalfRadix / Denom) + (2*(HalfRadix % Denom) >= Denom));
		AF = BigInt_Mul(AF, F);
		BF = BigInt_Mul(BF, F);
		E = BF.Words[BF.WordCount-1];
	}
	Mem_Cpy(IDD.Words, AF.Words+I, BF.WordCount * sizeof(u64));
	
	while(I >= 0) {
		Stack_Push();
		
		bigint YZ = BigInt_Splice(IDD, IDD.WordCount-2, 2);
		u64 Y = YZ.Words[1];
		u64 Z = YZ.Words[0];
		bigint D;
		if(E > 1) {
			bigint RadixOverE = BigInt_InitV(2*(HalfRadix / E) + (2*(HalfRadix % E) >= E));
			bigint RadixModE = BigInt_InitV(-(RadixOverE.Words[0] * E));
			bigint RemSum = BigInt_Add(BigInt_Mul(Y, RadixModE), BigInt_InitV(Z % E));
			bigint QuotSum = BigInt_Add(BigInt_Mul(Y, RadixOverE), BigInt_InitV(Z / E));
		} else {
			D = YZ;
		}
		
		if(D || Result.Quotient.WordCount) {
			Result.Quotient.Words[I] = D;
			Result.Quotient.WordCount++;
		}
		
		bigint Diff;
		if(BF.WordCount > 1) {
			
			
			bigint Prod = BigInt_Mul(B, D);
			Diff = BigInt_Sub(IDD, Prod);
		} else {
			Diff = BigInt_Sub(YZ, BigInt_Mul(BigInt_InitV(E), D));
		}
		
		if(I) {
			Mem_Cpy(IDD.Words+1, Diff.Words, B.WordCount * sizeof(u64));
			IDD.Words[0] = A.Words[I];
		} else {
			Mem_Cpy(IDD.Words, Diff.Words, B.WordCount * sizeof(u64));
			IDD.WordCount--;
		}
		
		I--;
		Stack_Pop();
	}
	
	if(F.Words[0] > 1) {
		bigint Remainder = BigInt_DivMod(IDD, F).Quotient;
		Mem_Cpy(IDD.Words, Remainder.Words, Remainder.WordCount * sizeof(u64));
		IDD.WordCount = Remainder.WordCount;
	}
	
	Result.Remainder = IDD;
	Stack_Pop();
	
	return Result;
}

#endif