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
	EXPORT(bigint,        BigInt_SInvert,    bigint A) \
	EXPORT(bigint,        BigInt_SAnd,       bigint A, bigint B) \
	EXPORT(bigint,        BigInt_SOr,        bigint A, bigint B) \
	EXPORT(bigint,        BigInt_SXor,       bigint A, bigint B) \
	EXPORT(bigint,        BigInt_SShift,     bigint A, ssize ShiftBy) \
	\
	EXPORT(bigint,        BigInt_SAbs,       bigint A) \
	EXPORT(bigint,        BigInt_SNegate,    bigint A) \
	EXPORT(bigint,        BigInt_SAdd,       bigint A, bigint B) \
	EXPORT(bigint,        BigInt_SSub,       bigint A, bigint B) \
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

// Returns a view of the data, does not copy
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

internal bigint
BigInt_SInvert(bigint A)
{
	return BigInt(0);
}

internal bigint
BigInt_SAnd(bigint A, bigint B)
{
	return BigInt(0);
}

internal bigint
BigInt_SOr(bigint A, bigint B)
{
	return BigInt(0);
}

internal bigint
BigInt_SXor(bigint A, bigint B)
{
	return BigInt(0);
}

internal bigint
BigInt_SShift(bigint A, ssize ShiftBy)
{
	return BigInt(0);
}

internal bigint
_BigInt_SAdd(bigint A, bigint B, usize C0, b08 InvB)
{
	bigint AP = BigInt_Flatten(A);
	bigint BP = BigInt_Flatten(B);
	if (!AP.WordCount) AP.Words = (usize *) &A.Word, AP.WordCount = 1;
	if (!BP.WordCount) BP.Words = (usize *) &B.Word, BP.WordCount = 1;

	usize AC = BigInt_IsNegative(AP) ? USIZE_MAX : 0;
	usize BC = BigInt_IsNegative(BP) ? USIZE_MAX : 0;

	usize Count = MAX(AP.WordCount, BP.WordCount);
	usize Carry = C0, AW, BW, Sum;

	bigint Result = BigInt(0);
	for (usize I = 0; I < Count + 1; I++) {
		AW	  = I < AP.WordCount ? AP.Words[I] : AC;
		BW	  = I < BP.WordCount ? BP.Words[I] : BC;
		Sum	  = AW + (InvB ? ~BW : BW) + Carry;
		Carry = Sum < AW || (Sum == AW && Carry);

		if (I == Count) {
			usize Bit = (I == 1 ? (usize) Result.Word : Result.Words[I - 1]) >> (SIZE_BITS - 1);
			if ((Sum == -1 && Bit) || (Sum == 0 && !Bit)) {
				if (Result.WordCount) Result.WordCount = Count;
				break;
			}
		}

		if (!I) {
			Result = BigInt(Sum);
			continue;
		}
		if (I == 1) {
			ssize First		 = Result.Word;
			Result			 = BigInt_SAllocate(Count + 1);
			Result.SWords[0] = First;
		}

		Result.Words[I] = Sum;
	}

	return BigInt_Flatten(Result);
}

internal bigint
BigInt_SNegate(bigint A)
{
	return _BigInt_SAdd(BigInt(0), A, 1, TRUE);
}

internal bigint
BigInt_SAbs(bigint A)
{
	return BigInt_IsNegative(A) ? BigInt_SNegate(A) : BigInt_SCopy(A);
}

internal bigint
BigInt_SAdd(bigint A, bigint B)
{
	return _BigInt_SAdd(A, B, 0, FALSE);
}

internal bigint
BigInt_SSub(bigint A, bigint B)
{
	return _BigInt_SAdd(A, B, 1, TRUE);
}

internal bigint
BigInt_SMul(bigint A, bigint B)
{
	return BigInt(0);
}

internal void
BigInt_SDivRem(bigint A, bigint B, bigint *Quot, bigint *Rem)
{ }

internal bigint
BigInt_SDiv(bigint A, bigint B)
{
	bigint Quot;
	BigInt_SDivRem(A, B, &Quot, NULL);
	return Quot;
}

internal bigint
BigInt_SRem(bigint A, bigint B)
{
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
