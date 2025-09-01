/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
**                                                                         **
**  Author: Aria Seiler                                                    **
**                                                                         **
**  This program is in the public domain. There is no implied warranty,    **
**  so use it at your own risk.                                            **
**                                                                         **
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef INCLUDE_HEADER

// TODO: We can track the number of 0-words at the beginning and only write the words after to
// memory, saving space.

typedef struct bigint {
	usize WordCount;
	union {
		usize *Words;
		ssize *SWords;
		ssize  Word;
	};
} bigint;

#define BIGINT_FUNCS \
	EXPORT(bigint,        BigInt,            ssize Value) \
	EXPORT(bigint,        BigInt_SAllocate,  usize WordCount) \
	EXPORT(bigint,        BigInt_SCopy,      bigint A) \
	\
	EXPORT(b08,           BigInt_IsZero,     bigint A) \
	EXPORT(b08,           BigInt_IsNegative, bigint A) \
	\
	EXPORT(bigint,        BigInt_Flatten,    bigint A) \
	EXPORT(bigint,        BigInt_Splice,     bigint *A, usize FromInclusive, usize ToExclusive) \
	\
	EXPORT(bigint,        BigInt_SInvert,    bigint A) \
	EXPORT(bigint,        BigInt_SAnd,       bigint A, bigint B) \
	EXPORT(bigint,        BigInt_SOr,        bigint A, bigint B) \
	EXPORT(bigint,        BigInt_SXor,       bigint A, bigint B) \
	EXPORT(bigint,        BigInt_SShift,     bigint A, ssize ShiftBy) \
	\
	EXPORT(bigint,        BigInt_SAdd,       bigint A, bigint B) \
	EXPORT(bigint,        BigInt_SSub,       bigint A, bigint B) \
	EXPORT(bigint,        BigInt_SAbs,       bigint A) \
	EXPORT(bigint,        BigInt_SNegate,    bigint A) \
	EXPORT(bigint,        BigInt_SMul,       bigint A, bigint B) \
	EXPORT(void,          BigInt_SDivRem,    bigint A, bigint B, bigint *Quot, bigint *Rem) \
	EXPORT(bigint,        BigInt_SDiv,       bigint A, bigint B) \
	EXPORT(bigint,        BigInt_SRem,       bigint A, bigint B) \
	\
	EXPORT(s08,           BigInt_Compare,    bigint A, bigint B) \
	EXPORT(ssize,         BigInt_ToInt,      bigint A)

#endif

#ifdef INCLUDE_SOURCE

internal bigint
BigInt(ssize Value)
{
	bigint Result;
	Result.WordCount = 0;
	Result.Word		 = Value;
	return Result;
}

internal bigint
BigInt_SAllocate(usize WordCount)
{
	if (!WordCount) return BigInt(0);
	bigint Result;
	Result.WordCount = WordCount;
	Result.Words	 = Stack_Allocate(WordCount * sizeof(usize));
	return Result;
}

internal bigint
BigInt_SCopy(bigint A)
{
	if (!A.WordCount) return BigInt(A.Word);
	if (A.WordCount == 1) return BigInt(A.SWords[0]);
	bigint Result = BigInt_SAllocate(A.WordCount);
	Mem_Cpy(Result.Words, A.Words, A.WordCount * sizeof(usize));
	return Result;
}

internal bigint
BigInt_Flatten(bigint A)
{
	if (!A.WordCount) return A;

	while (A.WordCount >= 2) {
		ssize W	  = A.SWords[A.WordCount - 1];
		usize Bit = A.Words[A.WordCount - 2] >> (SIZE_BITS - 1);
		if ((W == -1 && Bit) || (W == 0 && !Bit)) A.WordCount--;
		else break;
	}

	return A;
}

internal b08
BigInt_IsZero(bigint A)
{
	if (!A.WordCount) return A.Word == 0;
	for (usize I = 0; I < A.WordCount; I++)
		if (A.Words[I]) return FALSE;
	return TRUE;
}

internal b08
BigInt_IsNegative(bigint A)
{
	return (A.WordCount ? A.SWords[A.WordCount - 1] : A.Word) < 0;
}

global ssize BigIntBackingZero		  = 0;
global ssize BigIntBackingNegativeOne = -1;

internal bigint
BigInt_Splice(bigint *A, usize FromInclusive, usize ToExclusive)
{
	bigint Result = *A;
	if (!A->WordCount) {
		Result.WordCount = 1;
		Result.SWords	 = &A->Word;
	}

	if (FromInclusive >= ToExclusive || FromInclusive >= Result.WordCount) {
		Result.SWords = BigInt_IsNegative(Result) ? &BigIntBackingNegativeOne : &BigIntBackingZero;
		Result.WordCount = 1;
	} else {
		Result.SWords	 += FromInclusive;
		Result.WordCount  = MIN(ToExclusive, Result.WordCount) - FromInclusive;
	}

	return Result;
}

#define BIGINT_BINOP(C0, AOP, BOP, BINOP, ITER, CARRY_CODE)            \
	bigint AP = BigInt_Flatten(A);                                     \
	bigint BP = BigInt_Flatten(B);                                     \
	if (!AP.WordCount) AP.Words = (usize *) &A.Word, AP.WordCount = 1; \
	if (!BP.WordCount) BP.Words = (usize *) &B.Word, BP.WordCount = 1; \
                                                                       \
	usize AC = BigInt_IsNegative(AP) ? USIZE_MAX : 0;                  \
	usize BC = BigInt_IsNegative(BP) ? USIZE_MAX : 0;                  \
                                                                       \
	usize Count = MAX(AP.WordCount, BP.WordCount);                     \
	usize Carry = C0, AW, BW, DW;                                      \
                                                                       \
	bigint Result = BigInt(0);                                         \
	for (usize I = 0; I < ITER; I++) {                                 \
		AW	  = I < AP.WordCount ? AP.Words[I] : AC;                   \
		BW	  = I < BP.WordCount ? BP.Words[I] : BC;                   \
		DW	  = ((AOP AW) BINOP (BOP BW)) + Carry;                     \
                                                                       \
		MAC_UNPACKAGE(CARRY_CODE)                                      \
                                                                       \
		if (!I) {                                                      \
			Result = BigInt(DW);                                       \
			continue;                                                  \
		}                                                              \
		if (I == 1) {                                                  \
			ssize First		 = Result.Word;                            \
			Result			 = BigInt_SAllocate(ITER);                 \
			Result.SWords[0] = First;                                  \
		}                                                              \
                                                                       \
		Result.Words[I] = DW;                                          \
	}                                                                  \
                                                                       \
	return BigInt_Flatten(Result);

#define BIGINT_LOGICAL_BINOP(AOP, BOP, BINOP) \
	BIGINT_BINOP(0, AOP, BOP, BINOP, Count, ())

#define BIGINT_ARITHMETIC_BINOP(C0, BOP)                                                         \
	BIGINT_BINOP(C0, , BOP, +, Count + 1, (                                                      \
		Carry = DW < AW || (DW == AW && Carry);                                                  \
                                                                                                 \
		if (I == Count) {                                                                        \
			usize Bit = (I == 1 ? (usize) Result.Word : Result.Words[I - 1]) >> (SIZE_BITS - 1); \
			if ((DW == -1 && Bit) || (DW == 0 && !Bit)) {                                        \
				if (Result.WordCount) Result.WordCount = Count;                                  \
				break;                                                                           \
			}                                                                                    \
		}))

internal bigint
BigInt_SAnd(bigint A, bigint B)
{
	BIGINT_LOGICAL_BINOP(, , &);
}

internal bigint
BigInt_SOr(bigint A, bigint B)
{
	BIGINT_LOGICAL_BINOP(, , |);
}

internal bigint
BigInt_SXor(bigint A, bigint B)
{
	BIGINT_LOGICAL_BINOP(, , ^);
}

internal bigint
BigInt_SInvert(bigint A)
{
	return BigInt_SXor(A, BigInt(-1));
}

internal bigint
BigInt_SShift(bigint A, ssize ShiftBy)
{
	if (ShiftBy == 0 || BigInt_IsZero(A)) return BigInt_Flatten(A);

	bigint AP = BigInt_Flatten(A);
	if (!AP.WordCount) AP.Words = (usize *) &A.Word, AP.WordCount = 1;

	usize AC = BigInt_IsNegative(AP) ? USIZE_MAX : 0;

	ssize WordsBy = ShiftBy / (ssize) SIZE_BITS;
	ssize BitsBy  = ShiftBy % (ssize) SIZE_BITS;
	if ((ssize) AP.WordCount + WordsBy <= 0) return BigInt(AC);

	s32	  Msb;
	usize MsbMask = AC ? ~AP.Words[AP.WordCount - 1] : AP.Words[AP.WordCount - 1];
	if (!Intrin_BitScanForward64((u32 *) &Msb, MsbMask)) Msb = -1;
	Msb++;

	usize  AH, AL, DW;
	ssize  I, J, Count;
	bigint Result;

	if (ShiftBy < 0) {
		Count = (ssize) AP.WordCount + WordsBy - (BitsBy <= -Msb - 1);
		if (!Count) return BigInt(AC);

		for (I = 0; I < Count; I++) {
			J  = I - WordsBy;
			AH = J + 1 < AP.WordCount ? AP.Words[J + 1] : AC;
			AL = J < AP.WordCount ? AP.Words[J] : AC;
			DW = (AH << (SIZE_BITS - -BitsBy)) | (AL >> -BitsBy);

			if (!I) {
				if (Count == 1) return BigInt(DW);
				else Result = BigInt_SAllocate(Count);
			}

			Result.Words[I] = DW;
		}
	} else {
		Count = (ssize) AP.WordCount + WordsBy + (BitsBy >= SIZE_BITS - Msb);

		for (I = 0; I < Count; I++) {
			J  = I - WordsBy;
			AH = J < 0 ? 0 : AP.Words[J];
			AL = J - 1 < 0 ? 0 : AP.Words[J - 1];
			DW = (AH << BitsBy) | (AL >> (SIZE_BITS - BitsBy));

			if (!I) {
				if (Count == 1) return BigInt(DW);
				else Result = BigInt_SAllocate(Count);
			}

			Result.Words[I] = DW;
		}
	}

	return BigInt_Flatten(Result);
}

internal bigint
BigInt_SAdd(bigint A, bigint B)
{
	BIGINT_ARITHMETIC_BINOP(0, );
}

internal bigint
BigInt_SSub(bigint A, bigint B)
{
	BIGINT_ARITHMETIC_BINOP(1, ~);
}

internal bigint
BigInt_SNegate(bigint A)
{
	return BigInt_SSub(BigInt(0), A);
}

internal bigint
BigInt_SAbs(bigint A)
{
	return BigInt_IsNegative(A) ? BigInt_SNegate(A) : A;
}

internal bigint
_BigInt_SMul(bigint A, bigint B, usize M)
{
	if (M == 1) {
		usize HalfShift = SIZE_BITS / 2;
		usize HalfMask	= (1ull << HalfShift) - 1;

		usize A0 = A.Words[0] & HalfMask;
		usize A1 = A.Words[0] >> HalfShift;
		usize B0 = B.Words[0] & HalfMask;
		usize B1 = B.Words[0] >> HalfShift;

		usize P00 = A0 * B0;
		usize P01 = A0 * B1;
		usize P10 = A1 * B0;
		usize P11 = A1 * B1;

		usize Cross = (P00 >> HalfShift) + (P10 & HalfMask) + P01;
		usize Upper = (P10 >> HalfShift) + (Cross >> HalfShift) + P11;
		usize Lower = (Cross << HalfShift) | (P00 & HalfMask);

		bigint Result	= BigInt_SAllocate(2);
		Result.Words[0] = Lower;
		Result.Words[1] = Upper;
		return BigInt_Flatten(Result);
	} else {
		bigint A0 = BigInt_Splice(&A, 0, M / 2);
		bigint B0 = BigInt_Splice(&B, 0, M / 2);
		bigint A1 = BigInt_Splice(&A, M / 2, M);
		bigint B1 = BigInt_Splice(&B, M / 2, M);

		bigint P00 = _BigInt_SMul(A0, B0, M / 2);
		bigint P01 = _BigInt_SMul(A0, B1, M - M / 2);
		bigint P10 = _BigInt_SMul(A1, B0, M - M / 2);
		bigint P11 = _BigInt_SMul(A1, B1, M - M / 2);

		bigint Cross = BigInt_SAdd(P01, P10);
		Cross		 = BigInt_SShift(Cross, M / 2 * SIZE_BITS);
		P11			 = BigInt_SShift(P11, 2 * (M / 2) * SIZE_BITS);

		bigint Result = BigInt_SAdd(P00, Cross);
		Result		  = BigInt_SAdd(Result, P11);
		return BigInt_Flatten(Result);
	}
}

internal bigint
BigInt_SMul(bigint A, bigint B)
{
	bigint AP = BigInt_Flatten(A);
	bigint BP = BigInt_Flatten(B);
	if (!AP.WordCount) AP.Words = (usize *) &A.Word, AP.WordCount = 1;
	if (!BP.WordCount) BP.Words = (usize *) &B.Word, BP.WordCount = 1;

	Stack_Push();

	b08 IsNegative = FALSE;
	if (BigInt_IsNegative(AP)) {
		AP		   = BigInt_SNegate(AP);
		IsNegative = !IsNegative;
	}
	if (BigInt_IsNegative(BP)) {
		BP		   = BigInt_SNegate(BP);
		IsNegative = !IsNegative;
	}

	bigint Result = _BigInt_SMul(AP, BP, MAX(AP.WordCount, BP.WordCount));

	if (IsNegative) Result = BigInt_SNegate(Result);

	Stack_Pop();

	if (Result.WordCount == 1) return BigInt(Result.SWords[0]);

	vptr OldWords = Result.Words;
	Result.Words  = Stack_Allocate(Result.WordCount * sizeof(usize));
	Mem_Cpy(Result.Words, OldWords, Result.WordCount * sizeof(usize));
	return Result;
}

internal void
BigInt_SDivRem(bigint A, bigint B, bigint *Quot, bigint *Rem)
{
	// TODO
}

internal bigint
BigInt_SDiv(bigint A, bigint B)
{
	// TODO: Tests
	bigint Quot;
	BigInt_SDivRem(A, B, &Quot, NULL);
	return Quot;
}

internal bigint
BigInt_SRem(bigint A, bigint B)
{
	// TODO: Tests
	bigint Rem;
	BigInt_SDivRem(A, B, NULL, &Rem);
	return Rem;
}

internal s08
BigInt_Compare(bigint A, bigint B)
{
	Stack_Push();
	bigint Diff = BigInt_SSub(A, B);
	s08	   Cmp	= BigInt_IsNegative(Diff) ? -1 : BigInt_IsZero(Diff) ? 0 : 1;
	Stack_Pop();
	return Cmp;
}

internal ssize
BigInt_ToInt(bigint A)
{
	Assert(
		BigInt_IsNegative(A) ? BigInt_Compare(A, BigInt(SSIZE_MIN)) >= 0
							 : BigInt_Compare(A, BigInt(SSIZE_MAX)) <= 0
	);
	return A.WordCount ? ((ssize *) A.Words)[0] : A.Word;
}

#ifndef REGION_BIGINT_TESTS

#define BIGINT_TESTS                                                         \
	TEST(BigInt, ConstructsPositives, (                                      \
		bigint Result = BigInt(123);                                         \
		Assert(Result.WordCount == 0);                                       \
		Assert(Result.Word == 123);                                          \
	))                                                                       \
	TEST(BigInt, ConstructsNegatives, (                                      \
		bigint Result = BigInt(SSIZE_MIN);                                   \
		Assert(Result.WordCount == 0);                                       \
		Assert(Result.Word == SSIZE_MIN);                                    \
	))                                                                       \
	TEST(BigInt_SAllocate, AllocatesAndSetsWordCount, (                      \
		bigint Result = BigInt_SAllocate(4);                                 \
		Assert(Result.WordCount == 4);                                       \
		Assert(Result.Words + 4 == Stack_GetCursor());                       \
	))                                                                       \
	TEST(BigInt_SAllocate, ReturnsEmptyForZeroWords, (                       \
		vptr Cursor = Stack_GetCursor();                                     \
		bigint Result = BigInt_SAllocate(0);                                 \
		Assert(Result.WordCount == 0);                                       \
		Assert(Result.Word == 0);                                            \
		Assert(Stack_GetCursor() == Cursor);                                 \
	))                                                                       \
	TEST(BigInt_SCopy, ReturnsAllocatedCopy, (                               \
		bigint Source = BigInt_SAllocate(3);                                 \
		Source.Words[0] = 100;                                               \
		Source.Words[1] = 200;                                               \
		Source.Words[2] = 300;                                               \
		bigint Result = BigInt_SCopy(Source);                                \
		Assert(Result.WordCount == 3);                                       \
		Assert(Mem_Cmp(Result.Words, Source.Words, 3 * sizeof(usize)) == 0); \
		Assert(Source.Words + 6 == Stack_GetCursor());                       \
	))                                                                       \
	TEST(BigInt_SCopy, ReturnsWordOnZeroCount, (                             \
		vptr Cursor = Stack_GetCursor();                                     \
		bigint Source = BigInt(312);                                         \
		bigint Result = BigInt_SCopy(Source);                                \
		Assert(Result.WordCount == 0);                                       \
		Assert(Result.Word == Source.Word);                                  \
		Assert(Stack_GetCursor() == Cursor);                                 \
	))                                                                       \
	TEST(BigInt_SCopy, ReturnsWordOnOneCount, (                              \
		bigint Source = BigInt_SAllocate(1);                                 \
		Source.SWords[0] = 333;                                              \
		bigint Result = BigInt_SCopy(Source);                                \
		Assert(Result.WordCount == 0);                                       \
		Assert(Result.Word == Source.SWords[0]);                             \
		Assert(Source.Words + 1 == Stack_GetCursor());                       \
	))                                                                       \
	TEST(BigInt_IsZero, ChecksWordOnZeroCount, (                             \
		Assert(BigInt_IsZero(BigInt(0)));                                    \
		Assert(!BigInt_IsZero(BigInt(1)));                                   \
	))                                                                       \
	TEST(BigInt_IsZero, ChecksAllWordsOnMultiCount, (                        \
		bigint Source = BigInt_SAllocate(3);                                 \
		Mem_Set(Source.Words, 0, 3 * sizeof(usize));                         \
		Assert(BigInt_IsZero(Source));                                       \
		Source.Words[1] = 1;                                                 \
		Assert(!BigInt_IsZero(Source));                                      \
	))                                                                       \
	TEST(BigInt_IsNegative, ChecksWordOnZeroCount, (                         \
		Assert(BigInt_IsNegative(BigInt(-23)));                              \
		Assert(!BigInt_IsNegative(BigInt(0)));                               \
	))                                                                       \
	TEST(BigInt_IsNegative, ChecksAllWordsOnMultiCount, (                    \
		bigint Source = BigInt_SAllocate(3);                                 \
		Mem_Set(Source.Words, 0, 3 * sizeof(usize));                         \
		Assert(!BigInt_IsNegative(Source));                                  \
		Source.SWords[2] = SSIZE_MIN;                                        \
		Assert(BigInt_IsNegative(Source));                                   \
	))                                                                       \
	TEST(BigInt_Flatten, NoChangeForZeroCount, (                             \
		bigint Result = BigInt_Flatten(BigInt(3));                           \
		Assert(Result.WordCount == 0);                                       \
		Assert(Result.Word == 3);                                            \
	))                                                                       \
	TEST(BigInt_Flatten, FlattensUnnecessarySignExtensions, (                \
		bigint Source = BigInt_SAllocate(5);                                 \
		Source.SWords[0] = -32;                                              \
		Source.SWords[1] = 0;                                                \
		Source.SWords[2] = -35;                                              \
		Source.SWords[3] = -1;                                               \
		Source.SWords[4] = -1;                                               \
		Assert(BigInt_Flatten(Source).WordCount == 3);                       \
		Source.SWords[3] = 3;                                                \
		Assert(BigInt_Flatten(Source).WordCount == 5);                       \
	))                                                                       \
	TEST(BigInt_Flatten, FlattensUnnecessaryZeroes, (                        \
		bigint Source = BigInt_SAllocate(5);                                 \
		Source.SWords[0] = -32;                                              \
		Source.SWords[1] = 0;                                                \
		Source.SWords[2] = 35;                                               \
		Source.SWords[3] = 0;                                                \
		Source.SWords[4] = 0;                                                \
		Assert(BigInt_Flatten(Source).WordCount == 3);                       \
		Source.Words[3] = USIZE_MAX;                                         \
		Assert(BigInt_Flatten(Source).WordCount == 5);                       \
	))                                                                       \
	TEST(BigInt_Splice, SplicesCorrectly, (                                  \
		bigint Source = BigInt_SAllocate(5);                                 \
		Source.SWords[0] = -32;                                              \
		Source.SWords[1] = 0;                                                \
		Source.SWords[2] = 35;                                               \
		Source.SWords[3] = 0;                                                \
		Source.SWords[4] = 0;                                                \
		bigint Result = BigInt_Splice(&Source, 1, 4);                        \
		Assert(Result.WordCount == 3);                                       \
		Assert(Result.SWords[0] == 0);                                       \
		Assert(Result.SWords[1] == 35);                                      \
		Assert(Result.SWords[2] == 0);                                       \
	))                                                                       \
	TEST(BigInt_Splice, TruncatesIfOverflow, (                               \
		bigint Source = BigInt_SAllocate(2);                                 \
		Source.SWords[0] = -32;                                              \
		Source.SWords[1] = 35;                                               \
		bigint Result = BigInt_Splice(&Source, 1, 3);                        \
		Assert(Result.WordCount == 1);                                       \
		Assert(Result.SWords == Source.SWords+1);                            \
	))                                                                       \
	TEST(BigInt_Splice, ReturnsSignIfEmpty, (                                \
		bigint Source = BigInt(-2);                                          \
		bigint Result = BigInt_Splice(&Source, 1, 2);                        \
		Assert(Result.WordCount == 1);                                       \
		Assert(Result.SWords[0] == -1);                                      \
		Source = BigInt(4);                                                  \
		Result = BigInt_Splice(&Source, 1, 2);                               \
		Assert(Result.WordCount == 1);                                       \
		Assert(Result.SWords[0] == 0);                                       \
		Result = BigInt_Splice(&Source, 0, 0);                               \
		Assert(Result.WordCount == 1);                                       \
		Assert(Result.SWords[0] == 0);                                       \
	))                                                                       \
	TEST(BigInt_SAnd, ANDsCorrectly, (                                       \
		bigint A = BigInt_SAllocate(2);                                      \
		A.SWords[0] = 0x2948271534;                                          \
		A.SWords[1] = SSIZE_MIN | 0x80192747193;                             \
		bigint B = BigInt_SAllocate(3);                                      \
		B.SWords[0] = 0x81acd923;                                            \
		B.SWords[1] = 0x1a1a1a1a1a1a1a;                                      \
		B.SWords[2] = 0x2040097021009;                                       \
		bigint Result = BigInt_SAnd(A, B);                                   \
		Assert(Result.WordCount == 3);                                       \
		Assert(Result.SWords[0] == (A.SWords[0] & B.SWords[0]));             \
		Assert(Result.SWords[1] == (A.SWords[1] & B.SWords[1]));             \
		Assert(Result.SWords[2] == B.SWords[2]);                             \
	))                                                                       \
	TEST(BigInt_SOr, ORsCorrectly, (                                         \
		bigint A = BigInt_SAllocate(2);                                      \
		A.SWords[0] = 0x2948271534;                                          \
		A.SWords[1] = 0x80192747193;                                         \
		bigint B = BigInt_SAllocate(3);                                      \
		B.SWords[0] = 0x81acd923;                                            \
		B.SWords[1] = 0x1a1a1a1a1a1a1a;                                      \
		B.SWords[2] = 0x2040097021009;                                       \
		bigint Result = BigInt_SOr(A, B);                                    \
		Assert(Result.WordCount == 3);                                       \
		Assert(Result.SWords[0] == (A.SWords[0] | B.SWords[0]));             \
		Assert(Result.SWords[1] == (A.SWords[1] | B.SWords[1]));             \
		Assert(Result.SWords[2] == B.SWords[2]);                             \
	))                                                                       \
	TEST(BigInt_SXor, XORsCorrectly, (                                       \
		bigint A = BigInt_SAllocate(2);                                      \
		A.SWords[0] = 0x2948271534;                                          \
		A.SWords[1] = SSIZE_MIN | 0x80192747193;                             \
		bigint B = BigInt_SAllocate(3);                                      \
		B.SWords[0] = 0;                                                     \
		B.SWords[1] = -1;                                                    \
		B.SWords[2] = 0x2040097021009;                                       \
		bigint Result = BigInt_SXor(A, B);                                   \
		Assert(Result.WordCount == 3);                                       \
		Assert(Result.SWords[0] == (A.SWords[0] ^ B.SWords[0]));             \
		Assert(Result.SWords[1] == (A.SWords[1] ^ B.SWords[1]));             \
		Assert(Result.SWords[2] == ~B.SWords[2]);                            \
	))                                                                       \
	TEST(BigInt_SInvert, InvertsCorrectly, (                                 \
		bigint Source = BigInt_SAllocate(4);                                 \
		Source.SWords[0] = 0;                                                \
		Source.SWords[1] = -1;                                               \
		Source.SWords[2] = SSIZE_MAX;                                        \
		Source.SWords[3] = SSIZE_MIN;                                        \
		bigint Result = BigInt_SInvert(Source);                              \
		Assert(Result.WordCount == 4);                                       \
		Assert(Result.SWords[0] == -1);                                      \
		Assert(Result.SWords[1] == 0);                                       \
		Assert(Result.SWords[2] == SSIZE_MIN);                               \
		Assert(Result.SWords[3] == SSIZE_MAX);                               \
	))                                                                       \
	TEST(BigInt_SShift, ReturnsSourceOnZeroShift, (                          \
		bigint Source = BigInt_SAllocate(2);                                 \
		Source.SWords[0] = 1;                                                \
		Source.SWords[1] = 0;                                                \
		bigint Result = BigInt_SShift(Source, 0);                            \
		Assert(Result.WordCount == 1);                                       \
		Assert(Result.Words == Source.Words);                                \
	))                                                                       \
	TEST(BigInt_SShift, ShiftsRightMultipleWords, (                          \
		bigint Source = BigInt_SAllocate(3);                                 \
		Source.SWords[0] = 0;                                                \
		Source.SWords[1] = 0;                                                \
		Source.SWords[2] = 64;                                               \
		bigint Result = BigInt_SShift(Source, -SIZE_BITS - 2);               \
		Assert(Result.WordCount == 2);                                       \
		Assert(Result.SWords[0] == 0);                                       \
		Assert(Result.SWords[1] == 16);                                      \
		Source.SWords[2] = -64;                                              \
		Result = BigInt_SShift(Source, -(SIZE_BITS * 3));                    \
		Assert(Result.WordCount == 0);                                       \
		Assert(Result.Word == -1);                                           \
	))                                                                       \
	TEST(BigInt_SShift, ShiftsRightWordBoundary, (                           \
		bigint Source = BigInt_SAllocate(3);                                 \
		Source.SWords[0] = 0;                                                \
		Source.SWords[1] = 1;                                                \
		Source.SWords[2] = 0;                                                \
		bigint Result = BigInt_SShift(Source, -1);                           \
		Assert(Result.WordCount == 2);                                       \
		Assert(Result.SWords[0] == SSIZE_MIN);                               \
		Assert(Result.SWords[1] == 0);                                       \
		Source.SWords[1] = SSIZE_MIN;                                        \
		Result = BigInt_SShift(Source, -SIZE_BITS + 1);                      \
		Assert(Result.WordCount == 2);                                       \
		Assert(Result.SWords[0] == 0);                                       \
		Assert(Result.SWords[1] == 1);                                       \
	))                                                                       \
	TEST(BigInt_SShift, ShiftsLeftMultipleWords, (                           \
		bigint Source = BigInt_SAllocate(2);                                 \
		Source.SWords[0] = 0;                                                \
		Source.SWords[1] = 16;                                               \
		bigint Result = BigInt_SShift(Source, SIZE_BITS + 2);                \
		Assert(Result.WordCount == 3);                                       \
		Assert(Result.SWords[0] == 0);                                       \
		Assert(Result.SWords[1] == 0);                                       \
		Assert(Result.SWords[2] == 64);                                      \
		Source.SWords[1] = -16;                                              \
		Result = BigInt_SShift(Source, SIZE_BITS);                           \
		Assert(Result.WordCount == 3);                                       \
		Assert(Result.SWords[0] == 0);                                       \
		Assert(Result.SWords[1] == 0);                                       \
		Assert(Result.SWords[2] == -16);                                     \
	))                                                                       \
	TEST(BigInt_SShift, ShiftsLeftWordBoundary, (                            \
		bigint Source = BigInt_SAllocate(2);                                 \
		Source.SWords[0] = SSIZE_MIN;                                        \
		Source.SWords[1] = 0;                                                \
		bigint Result = BigInt_SShift(Source, 1);                            \
		Assert(Result.WordCount == 2);                                       \
		Assert(Result.SWords[0] == 0);                                       \
		Assert(Result.SWords[1] == 1);                                       \
		Source.SWords[0] = 1;                                                \
		Result = BigInt_SShift(Source, SIZE_BITS - 1);                       \
		Assert(Result.WordCount == 2);                                       \
		Assert(Result.SWords[0] == SSIZE_MIN);                               \
		Assert(Result.SWords[1] == 0);                                       \
	))                                                                       \
	TEST(BigInt_SAdd, AddsSmallNumbers, (                                    \
		bigint Result = BigInt_SAdd(BigInt(3), BigInt(5));                   \
		Assert(Result.WordCount == 0);                                       \
		Assert(Result.Word == 8);                                            \
		Result = BigInt_SAdd(BigInt(4), BigInt(-7));                         \
		Assert(Result.WordCount == 0);                                       \
		Assert(Result.Word == -3);                                           \
		Result = BigInt_SAdd(BigInt(-2), BigInt(-9));                        \
		Assert(Result.WordCount == 0);                                       \
		Assert(Result.Word == -11);                                          \
	))                                                                       \
	TEST(BigInt_SAdd, UnderflowsCorrectly, (                                 \
		bigint Result = BigInt_SAdd(BigInt(SSIZE_MIN), BigInt(-1));          \
		Assert(Result.WordCount == 2);                                       \
		Assert(Result.SWords[0] == SSIZE_MAX);                               \
		Assert(Result.SWords[1] == -1);                                      \
	))                                                                       \
	TEST(BigInt_SAdd, OverflowsCorrectly, (                                  \
		bigint Result = BigInt_SAdd(BigInt(SSIZE_MAX), BigInt(1));           \
		Assert(Result.WordCount == 2);                                       \
		Assert(Result.SWords[0] == SSIZE_MIN);                               \
		Assert(Result.SWords[1] == 0);                                       \
	))                                                                       \
	TEST(BigInt_SAdd, FlattensWhenArgsAreOverlong, (                         \
		bigint A = BigInt_SAllocate(4);                                      \
		A.SWords[0] = 0;                                                     \
		A.SWords[1] = 1;                                                     \
		A.SWords[2] = 0;                                                     \
		A.SWords[3] = 0;                                                     \
		bigint B = BigInt_SAllocate(3);                                      \
		B.SWords[0] = 0;                                                     \
		B.SWords[1] = -1;                                                    \
		B.SWords[2] = -1;                                                    \
		bigint Result = BigInt_SAdd(A, B);                                   \
		Assert(Result.WordCount == 1);                                       \
		Assert(Result.SWords[0] == 0);                                       \
	))                                                                       \
	TEST(BigInt_SSub, SubtractsCorrectly, (                                  \
		bigint A = BigInt_SAllocate(4);                                      \
		A.SWords[0] = 0;                                                     \
		A.SWords[1] = 1;                                                     \
		A.SWords[2] = 0;                                                     \
		A.SWords[3] = 0;                                                     \
		bigint B = BigInt_SAllocate(3);                                      \
		B.SWords[0] = 0;                                                     \
		B.SWords[1] = 1;                                                     \
		B.SWords[2] = 0;                                                     \
		bigint Result = BigInt_SSub(A, B);                                   \
		Assert(Result.WordCount == 1);                                       \
		Assert(Result.SWords[0] == 0);                                       \
	))                                                                       \
	TEST(BigInt_SNegate, NegatesSmallAndLargeNumbers, (                      \
		bigint Result = BigInt_SNegate(BigInt(32));                          \
		Assert(Result.WordCount == 0);                                       \
		Assert(Result.Word == -32);                                          \
		bigint Source = BigInt_SAllocate(2);                                 \
		Source.SWords[0] = 1;                                                \
		Source.SWords[1] = -1;                                               \
		Result = BigInt_SNegate(Source);                                     \
		Assert(Result.WordCount == 2);                                       \
		Assert(Result.SWords[0] == -1);                                      \
		Assert(Result.SWords[1] == 0);                                       \
	))                                                                       \
	TEST(BigInt_SNegate, HandlesZero, (                                      \
		bigint Source = BigInt_SAllocate(3);                                 \
		Source.SWords[0] = 0;                                                \
		Source.SWords[1] = 0;                                                \
		Source.SWords[2] = 0;                                                \
		bigint Result = BigInt_SNegate(Source);                              \
		Assert(Result.WordCount == 0);                                       \
		Assert(Result.Word == 0);                                            \
	))                                                                       \
	TEST(BigInt_SNegate, HandlesUnderflow, (                                 \
		bigint Source = BigInt_SAllocate(2);                                 \
		Source.SWords[0] = 0;                                                \
		Source.SWords[1] = SSIZE_MIN;                                        \
		bigint Result = BigInt_SNegate(Source);                              \
		Assert(Result.WordCount == 3);                                       \
		Assert(Result.SWords[0] == 0);                                       \
		Assert(Result.SWords[1] == SSIZE_MIN);                               \
		Assert(Result.SWords[2] == 0);                                       \
	))                                                                       \
	TEST(BigInt_SAbs, NegatesOrReturnsUnchanged, (                           \
		bigint Result = BigInt_SAbs(BigInt(-3));                             \
		Assert(Result.WordCount == 0);                                       \
		Assert(Result.Word == 3);                                            \
		bigint Source = BigInt_SAllocate(2);                                 \
		Source.SWords[0] = 1;                                                \
		Source.SWords[1] = 2;                                                \
		Result = BigInt_SAbs(Source);                                        \
		Assert(Result.WordCount == 2);                                       \
		Assert(Result.SWords == Source.SWords);                              \
	))                                                                       \
	TEST(BigInt_SMul, MultipliesCorrectly, (                                 \
		bigint A = BigInt_SAllocate(2);                                      \
		A.SWords[0] = 0;                                                     \
		A.SWords[1] = 1;                                                     \
		bigint B = BigInt_SAllocate(2);                                      \
		B.SWords[0] = 0;                                                     \
		B.SWords[1] = 1;                                                     \
		bigint Result = BigInt_SMul(A, B);                                   \
		Assert(Result.WordCount == 3);                                       \
		Assert(Result.SWords[0] == 0);                                       \
		Assert(Result.SWords[1] == 0);                                       \
		Assert(Result.SWords[2] == 1);                                       \
	))                                                                       \
	TEST(BigInt_SMul, HandlesNegatives, (                                    \
		bigint A = BigInt_SAllocate(2);                                      \
		A.SWords[0] = 0;                                                     \
		A.SWords[1] = 1;                                                     \
		bigint B = BigInt_SAllocate(2);                                      \
		B.SWords[0] = 0;                                                     \
		B.SWords[1] = -2;                                                    \
		bigint Result = BigInt_SMul(A, B);                                   \
		Assert(Result.WordCount == 3);                                       \
		Assert(Result.SWords[0] == 0);                                       \
		Assert(Result.SWords[1] == 0);                                       \
		Assert(Result.SWords[2] == -2);                                      \
	))                                                                       \
	TEST(BigInt_SMul, HandlesDoubleNegation, (                               \
		bigint A = BigInt_SAllocate(2);                                      \
		A.SWords[0] = 0;                                                     \
		A.SWords[1] = -3;                                                    \
		bigint B = BigInt_SAllocate(2);                                      \
		B.SWords[0] = 0;                                                     \
		B.SWords[1] = -2;                                                    \
		bigint Result = BigInt_SMul(A, B);                                   \
		Assert(Result.WordCount == 3);                                       \
		Assert(Result.SWords[0] == 0);                                       \
		Assert(Result.SWords[1] == 0);                                       \
		Assert(Result.SWords[2] == 6);                                       \
	))                                                                       \
	TEST(BigInt_SMul, RespectsUnevenSizes, (                                 \
		bigint A = BigInt_SAllocate(7);                                      \
		Mem_Set(A.SWords, 0, sizeof(usize) * 6);                             \
		A.SWords[6] = 3;                                                     \
		bigint B = BigInt_SAllocate(3);                                      \
		Mem_Set(B.SWords, 0, sizeof(usize) * 2);                             \
		B.SWords[2] = 2;                                                     \
		bigint Result = BigInt_SMul(A, B);                                   \
		Assert(Result.WordCount == 9);                                       \
		Assert(Result.SWords[8] == 6);                                       \
	))                                                                       \
	TEST(BigInt_Compare, ReturnsNegativeForLess, (                           \
		bigint A = BigInt(2);                                                \
		bigint B = BigInt_SAllocate(2);                                      \
		B.Words[0] = 1;                                                      \
		B.Words[1] = 1;                                                      \
		Assert(BigInt_Compare(A, B) < 0);                                    \
	))                                                                       \
	TEST(BigInt_Compare, ReturnsPositiveForGreater, (                        \
		bigint A = BigInt(2);                                                \
		bigint B = BigInt_SAllocate(2);                                      \
		B.Words[0] = 1;                                                      \
		B.Words[1] = 0;                                                      \
		Assert(BigInt_Compare(A, B) > 0);                                    \
	))                                                                       \
	TEST(BigInt_Compare, ReturnsZeroForEqual, (                              \
		bigint A = BigInt(2);                                                \
		bigint B = BigInt_SAllocate(2);                                      \
		B.Words[0] = 1;                                                      \
		B.Words[1] = 0;                                                      \
		Assert(BigInt_Compare(A, B) > 0);                                    \
	))                                                                       \
	TEST(BigInt_ToInt, ReturnsWordOnZeroCount, (                             \
		bigint Source = BigInt(543);                                         \
		ssize Result = BigInt_ToInt(Source);                                 \
		Assert(Result == 543);                                               \
	))                                                                       \
	TEST(BigInt_ToInt, ReturnsWordOnMultiCount, (                            \
		bigint Source = BigInt_SAllocate(2);                                 \
		Source.SWords[0] = -345;                                             \
		Source.SWords[1] = -1;                                               \
		ssize Result = BigInt_ToInt(Source);                                 \
		Assert(Result == -345);                                              \
	))                                                                       \
	;
#endif

#endif
