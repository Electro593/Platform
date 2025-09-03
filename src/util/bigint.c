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
		uhalf *Words;
		shalf *SWords;
		shalf  Word;
	};
} bigint;

#define BIGINT_FUNCS \
	EXPORT(bigint,        BigInt,            shalf Value) \
	EXPORT(bigint,        BigInt_SAllocate,  usize WordCount) \
	EXPORT(bigint,        BigInt_SInit,      usize WordCount, ...) \
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
BigInt(shalf Value)
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
	Result.Words	 = Stack_Allocate(WordCount * sizeof(uhalf));
	return Result;
}

internal bigint
BigInt_SInit(usize WordCount, ...)
{
	Assert(WordCount >= 1);

	bigint Result = BigInt_SAllocate(WordCount);

	va_list Args;
	VA_Start(Args, WordCount);

	for (usize I = 0; I < WordCount - 1; I++) Result.Words[I] = VA_Next(Args, uhalf);
	Result.Words[WordCount - 1] = VA_Next(Args, shalf);

	VA_End(Args);

	return Result;
}

internal bigint
BigInt_SCopy(bigint A)
{
	if (!A.WordCount) return BigInt(A.Word);
	if (A.WordCount == 1) return BigInt(A.SWords[0]);
	bigint Result = BigInt_SAllocate(A.WordCount);
	Mem_Cpy(Result.Words, A.Words, A.WordCount * sizeof(uhalf));
	return Result;
}

internal bigint
BigInt_Flatten(bigint A)
{
	if (!A.WordCount) return A;

	while (A.WordCount >= 2) {
		shalf W	  = A.SWords[A.WordCount - 1];
		uhalf Bit = A.Words[A.WordCount - 2] >> (UHALF_BITS - 1);
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

global shalf BigIntBackingZero		  = 0;
global shalf BigIntBackingNegativeOne = -1;

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

internal bigint
BigInt_SAnd(bigint A, bigint B)
{
	bigint AP = BigInt_Flatten(A);
	bigint BP = BigInt_Flatten(B);
	if (!AP.WordCount) AP.SWords = &A.Word, AP.WordCount = 1;
	if (!BP.WordCount) BP.SWords = &B.Word, BP.WordCount = 1;

	usize AC = BigInt_IsNegative(AP) ? UHALF_MAX : 0;
	usize BC = BigInt_IsNegative(BP) ? UHALF_MAX : 0;

	usize Count = MAX(AP.WordCount, BP.WordCount);
	usize AW, BW, DW;

	bigint Result = BigInt(0);
	for (usize I = 0; I < Count; I++) {
		AW = I < AP.WordCount ? AP.Words[I] : AC;
		BW = I < BP.WordCount ? BP.Words[I] : BC;
		DW = AW & BW;

		if (!I) {
			Result = BigInt(DW & UHALF_MAX);
			continue;
		}
		if (I == 1) {
			shalf First		 = Result.Word;
			Result			 = BigInt_SAllocate(Count);
			Result.SWords[0] = First;
		}

		Result.Words[I] = DW & UHALF_MAX;
	}

	return BigInt_Flatten(Result);
}

internal bigint
BigInt_SOr(bigint A, bigint B)
{
	bigint AP = BigInt_Flatten(A);
	bigint BP = BigInt_Flatten(B);
	if (!AP.WordCount) AP.SWords = &A.Word, AP.WordCount = 1;
	if (!BP.WordCount) BP.SWords = &B.Word, BP.WordCount = 1;

	usize AC = BigInt_IsNegative(AP) ? UHALF_MAX : 0;
	usize BC = BigInt_IsNegative(BP) ? UHALF_MAX : 0;

	usize Count = MAX(AP.WordCount, BP.WordCount);
	usize AW, BW, DW;

	bigint Result = BigInt(0);
	for (usize I = 0; I < Count; I++) {
		AW = I < AP.WordCount ? AP.Words[I] : AC;
		BW = I < BP.WordCount ? BP.Words[I] : BC;
		DW = AW | BW;

		if (!I) {
			Result = BigInt(DW & UHALF_MAX);
			continue;
		}
		if (I == 1) {
			shalf First		 = Result.Word;
			Result			 = BigInt_SAllocate(Count);
			Result.SWords[0] = First;
		}

		Result.Words[I] = DW & UHALF_MAX;
	}

	return BigInt_Flatten(Result);
}

internal bigint
BigInt_SXor(bigint A, bigint B)
{
	bigint AP = BigInt_Flatten(A);
	bigint BP = BigInt_Flatten(B);
	if (!AP.WordCount) AP.SWords = &A.Word, AP.WordCount = 1;
	if (!BP.WordCount) BP.SWords = &B.Word, BP.WordCount = 1;

	usize AC = BigInt_IsNegative(AP) ? UHALF_MAX : 0;
	usize BC = BigInt_IsNegative(BP) ? UHALF_MAX : 0;

	usize Count = MAX(AP.WordCount, BP.WordCount);
	usize AW, BW, DW;

	bigint Result = BigInt(0);
	for (usize I = 0; I < Count; I++) {
		AW = I < AP.WordCount ? AP.Words[I] : AC;
		BW = I < BP.WordCount ? BP.Words[I] : BC;
		DW = AW ^ BW;

		if (!I) {
			Result = BigInt(DW & UHALF_MAX);
			continue;
		}
		if (I == 1) {
			shalf First		 = Result.Word;
			Result			 = BigInt_SAllocate(Count);
			Result.SWords[0] = First;
		}

		Result.Words[I] = DW & UHALF_MAX;
	}

	return BigInt_Flatten(Result);
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
	if (!AP.WordCount) AP.SWords = &A.Word, AP.WordCount = 1;

	usize AC = BigInt_IsNegative(AP) ? USIZE_MAX : 0;

	ssize WordsBy = ShiftBy / (ssize) UHALF_BITS;
	ssize BitsBy  = ShiftBy % (ssize) UHALF_BITS;
	if ((ssize) AP.WordCount + WordsBy <= 0) return BigInt(AC);

	shalf Msb;
	uhalf MsbMask = AC ? ~AP.Words[AP.WordCount - 1] : AP.Words[AP.WordCount - 1];
	if (!Intrin_BitScanForward((uhalf *) &Msb, MsbMask)) Msb = -1;
	Msb++;

	uhalf  AH, AL, DW;
	ssize  I, J, Count;
	bigint Result;

	if (ShiftBy < 0) {
		Count = (ssize) AP.WordCount + WordsBy - (BitsBy <= -Msb - 1);
		if (!Count) return BigInt(AC);

		for (I = 0; I < Count; I++) {
			J  = I - WordsBy;
			AH = J + 1 < AP.WordCount ? AP.Words[J + 1] : AC;
			AL = J < AP.WordCount ? AP.Words[J] : AC;
			DW = (AH << (UHALF_BITS - -BitsBy)) | (AL >> -BitsBy);

			if (!I) {
				if (Count == 1) return BigInt(DW);
				else Result = BigInt_SAllocate(Count);
			}

			Result.Words[I] = DW;
		}
	} else {
		Count = (ssize) AP.WordCount + WordsBy + (BitsBy >= UHALF_BITS - Msb);

		for (I = 0; I < Count; I++) {
			J  = I - WordsBy;
			AH = J < 0 ? 0 : AP.Words[J];
			AL = J - 1 < 0 ? 0 : AP.Words[J - 1];
			DW = (AH << BitsBy) | (AL >> (UHALF_BITS - BitsBy));

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
	bigint AP = BigInt_Flatten(A);
	bigint BP = BigInt_Flatten(B);
	if (!AP.WordCount) AP.SWords = &A.Word, AP.WordCount = 1;
	if (!BP.WordCount) BP.SWords = &B.Word, BP.WordCount = 1;

	usize AC = BigInt_IsNegative(AP) ? UHALF_MAX : 0;
	usize BC = BigInt_IsNegative(BP) ? UHALF_MAX : 0;

	usize Count = MAX(AP.WordCount, BP.WordCount);
	usize Carry = 0, AW, BW, DW;

	bigint Result = BigInt(0);
	for (usize I = 0; I < Count + 1; I++) {
		AW	   = I < AP.WordCount ? AP.Words[I] : AC;
		BW	   = I < BP.WordCount ? BP.Words[I] : BC;
		DW	   = AW + BW + Carry;
		Carry  = DW >> UHALF_BITS;
		DW	  &= UHALF_MAX;

		if (I == Count) {
			usize Bit = (I == 1 ? (uhalf) Result.Word : Result.Words[I - 1]) >> (UHALF_BITS - 1);
			if ((DW == UHALF_MAX && Bit) || (DW == 0 && !Bit)) {
				if (Result.WordCount) Result.WordCount = Count;
				break;
			}
		}

		if (!I) {
			Result = BigInt(DW);
			continue;
		}
		if (I == 1) {
			shalf First		 = Result.Word;
			Result			 = BigInt_SAllocate(Count + 1);
			Result.SWords[0] = First;
		}

		Result.Words[I] = DW;
	}

	return BigInt_Flatten(Result);
}

internal bigint
BigInt_SSub(bigint A, bigint B)
{
	bigint AP = BigInt_Flatten(A);
	bigint BP = BigInt_Flatten(B);
	if (!AP.WordCount) AP.SWords = &A.Word, AP.WordCount = 1;
	if (!BP.WordCount) BP.SWords = &B.Word, BP.WordCount = 1;

	usize AC = BigInt_IsNegative(AP) ? UHALF_MAX : 0;
	usize BC = BigInt_IsNegative(BP) ? UHALF_MAX : 0;

	usize Count = MAX(AP.WordCount, BP.WordCount);
	usize Carry = 1, AW, BW, DW;

	bigint Result = BigInt(0);
	for (usize I = 0; I < Count + 1; I++) {
		AW	   = I < AP.WordCount ? AP.Words[I] : AC;
		BW	   = I < BP.WordCount ? BP.Words[I] : BC;
		DW	   = AW + (~BW & UHALF_MAX) + Carry;
		Carry  = DW >> UHALF_BITS;
		DW	  &= UHALF_MAX;

		if (I == Count) {
			usize Bit = (I == 1 ? (uhalf) Result.Word : Result.Words[I - 1]) >> (UHALF_BITS - 1);
			if ((DW == UHALF_MAX && Bit) || (DW == 0 && !Bit)) {
				if (Result.WordCount) Result.WordCount = Count;
				break;
			}
		}

		if (!I) {
			Result = BigInt(DW);
			continue;
		}
		if (I == 1) {
			shalf First		 = Result.Word;
			Result			 = BigInt_SAllocate(Count + 1);
			Result.SWords[0] = First;
		}

		Result.Words[I] = DW;
	}

	return BigInt_Flatten(Result);
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
		usize Product = A.Words[0] * B.Words[0];
		if (Product <= ((1ull << (UHALF_BITS - 1)) - 1)) return BigInt(Product);
		return BigInt_SInit(2, Product & UHALF_MAX, Product >> UHALF_BITS);
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
		Cross		 = BigInt_SShift(Cross, M / 2 * UHALF_BITS);
		P11			 = BigInt_SShift(P11, 2 * (M / 2) * UHALF_BITS);

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

	if (!AP.WordCount) AP.SWords = &A.Word, AP.WordCount = 1;
	if (!BP.WordCount) BP.SWords = &B.Word, BP.WordCount = 1;

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

	bigint Result;
	if (BigInt_IsZero(A) || BigInt_IsZero(B)) Result = BigInt(0);
	else if (AP.WordCount == 1 && AP.SWords[0] == 1) Result = BP;
	else if (BP.WordCount == 1 && BP.SWords[0] == 1) Result = AP;
	else Result = _BigInt_SMul(AP, BP, MAX(AP.WordCount, BP.WordCount));

	if (IsNegative) Result = BigInt_SNegate(Result);

	Stack_Pop();

	if (!Result.WordCount) return Result;
	if (Result.WordCount == 1) return BigInt(Result.SWords[0]);

	vptr OldWords = Result.Words;
	Result.Words  = Stack_Allocate(Result.WordCount * sizeof(uhalf));
	Mem_Cpy(Result.Words, OldWords, Result.WordCount * sizeof(uhalf));
	return Result;
}

internal void
BigInt_DivideUnsignedSingleword(bigint A, bigint B, bigint *QuotOut, bigint *RemOut)
{
	if (QuotOut) {
		*QuotOut = BigInt_SAllocate(MAX(A.WordCount - B.WordCount + 1, 1));
		Mem_Set(QuotOut->Words, 0, QuotOut->WordCount * sizeof(uhalf));
	}
	if (RemOut) {
		*RemOut = BigInt_SAllocate(B.WordCount);
		Mem_Set(RemOut->Words, 0, RemOut->WordCount * sizeof(uhalf));
	}

	Stack_Push();

	bigint U = BigInt_SCopy(A);

	Assert(B.WordCount == 1);
	usize VW = B.Words[0];

	for (usize I = 0; I < A.WordCount - B.WordCount; I++) {
		usize DigitIndex = A.WordCount - I;

		usize UW =
			((usize) U.Words[DigitIndex - 1] << UHALF_BITS) | (usize) U.Words[DigitIndex - 2];
		usize Q = UW / VW;
		usize R = UW % VW;

		U.Words[DigitIndex - 2] = R;

		if (QuotOut) QuotOut->Words[DigitIndex - B.WordCount] = Q;
	}

	if (RemOut) Mem_Cpy(RemOut->Words, B.Words, B.WordCount * sizeof(uhalf));

	Stack_Pop();
}

internal void
BigInt_DivideUnsignedMultiword(bigint A, bigint B, bigint *QuotOut, bigint *RemOut)
{
	// Since A and B are expected to be flattened multiword positive numbers, there won't be any
	// leading zeroes or 0-counts that we need to consider.

	if (QuotOut) {
		*QuotOut = BigInt_SAllocate(MAX(A.WordCount - B.WordCount + 1, 1));
		Mem_Set(QuotOut->Words, 0, QuotOut->WordCount * sizeof(uhalf));
	}
	if (RemOut) {
		*RemOut = BigInt_SAllocate(B.WordCount);
		Mem_Set(RemOut->Words, 0, RemOut->WordCount * sizeof(uhalf));
	}

	Stack_Push();

	// Obtain the normalization shift, e.g. the number of bits B[n-1] needs to be shifted by
	// until the leading bit is set.
	u32	  NormShift;
	uhalf BTop = B.Words[B.WordCount - 1];
	Intrin_BitScanForward(&NormShift, BTop);
	NormShift = 31 - NormShift;

	// Copy A and B into U and V respectively, shifting by NormShift. The normalization
	// allows the quotient estimation later to be more accurate. We're using mutable buffers
	// here instead of calling `BigInt_SShift` for performance.
	bigint U = BigInt_SAllocate(A.WordCount + 1);
	bigint V = BigInt_SAllocate(B.WordCount);

	U.Words[0] = A.Words[0] << NormShift;
	for (usize I = 1; I < A.WordCount; I++)
		U.Words[I] = (A.Words[I] << NormShift) | (A.Words[I - 1] >> (UHALF_BITS - NormShift));
	U.Words[A.WordCount] = A.Words[A.WordCount - 1] >> (UHALF_BITS - NormShift);

	V.Words[0] = B.Words[0] << NormShift;
	for (usize I = 1; I < B.WordCount; I++)
		V.Words[I] = (B.Words[I] << NormShift) | (B.Words[I - 1] >> (UHALF_BITS - NormShift));

	// Perform long division
	for (usize I = 0; I <= A.WordCount - B.WordCount; I++) {
		usize DigitIndex = A.WordCount - I;

		// Estimate a quotient based on the biggest two digits of A and the biggest of B. Refine the
		// estimate by checking the next least significant digits.
		usize UW   = ((usize) U.Words[DigitIndex] << UHALF_BITS) | (usize) U.Words[DigitIndex - 1];
		usize VW   = (usize) V.Words[V.WordCount - 1];
		usize QHat = UW / VW;
		usize RHat = UW % VW;

		do {
			if (QHat < UHALF_MAX
				&& QHat * (usize) V.Words[V.WordCount - 2]
					   <= (RHat << UHALF_BITS) + (usize) U.Words[DigitIndex - 2])
				break;
			QHat--;
			RHat += VW;
		} while (RHat < UHALF_MAX);

		usize UL   = (UW << UHALF_BITS) | (usize) U.Words[DigitIndex - 2];
		usize VL   = (VW << UHALF_BITS) | (usize) V.Words[V.WordCount - 2];
		usize Prod = QHat * VL;
		Assert(UL > Prod);
		usize R = UL - Prod;

		U.Words[DigitIndex - 1] = R >> UHALF_BITS;
		U.Words[DigitIndex - 2] = R & UHALF_MAX;

		if (QuotOut) QuotOut->Words[DigitIndex - B.WordCount] = QHat;
	}

	if (RemOut) {
		// Shift the remainder (now U) into RemOut
		for (usize I = 0; I < U.WordCount - 1; I++)
			RemOut->Words[I] =
				(U.Words[I + 1] << (UHALF_BITS - NormShift)) | (U.Words[I] >> NormShift);
		RemOut->Words[U.WordCount - 1] = U.Words[U.WordCount - 1] >> NormShift;
	}

	Stack_Pop();
}

internal void
BigInt_SDivRem(bigint A, bigint B, bigint *QuotOut, bigint *RemOut)
{
	// TODO: Tests

	Stack_Push();
	bigint AP = A, BP = B;

	Assert(!BigInt_IsZero(B));
	if (BigInt_IsZero(A)) {
		if (QuotOut) *QuotOut = BigInt(0);
		if (RemOut) *RemOut = BigInt(0);
		return;
	}

	b08 IsNegative = FALSE;
	if (BigInt_IsNegative(AP)) {
		AP		   = BigInt_SNegate(AP);
		IsNegative = !IsNegative;
	}
	if (BigInt_IsNegative(BP)) {
		BP		   = BigInt_SNegate(BP);
		IsNegative = !IsNegative;
	}

	AP = BigInt_Flatten(AP);
	BP = BigInt_Flatten(BP);
	if (!AP.WordCount) AP.SWords = &A.Word, AP.WordCount = 1;
	if (!BP.WordCount) BP.SWords = &B.Word, BP.WordCount = 1;
	if (AP.Words[AP.WordCount - 1] == 0) AP.WordCount--;
	if (BP.Words[BP.WordCount - 1] == 0) BP.WordCount--;

	bigint Quot, Rem;
	s08	   Cmp = BigInt_Compare(AP, BP);
	if (Cmp < 0) {
		Quot = A;
		Rem	 = B;
	} else if (Cmp == 0) {
		Quot = BigInt(1);
		Rem	 = BigInt(0);
	} else if (BP.WordCount == 1 && BP.SWords[0] == 1) {
		Quot = A;
		Rem	 = BigInt(0);
	} else if (BP.WordCount >= 2) {
		BigInt_DivideUnsignedMultiword(AP, BP, &Quot, &Rem);
	} else {
		BigInt_DivideUnsignedSingleword(AP, BP, &Quot, &Rem);
	}

	if (IsNegative) {
		Quot = BigInt_SNegate(Quot);
		Rem	 = BigInt_SNegate(Rem);
	}

	Stack_Pop();

	if (QuotOut) {
		if (!Quot.WordCount) *QuotOut = Quot;
		if (Quot.WordCount == 1) *QuotOut = BigInt(Quot.SWords[0]);
		else {
			*QuotOut	   = Quot;
			QuotOut->Words = Stack_Allocate(Quot.WordCount * sizeof(uhalf));
			Mem_Cpy(QuotOut->Words, Quot.Words, Quot.WordCount * sizeof(uhalf));
		}
	}

	if (RemOut) {
		if (!Rem.WordCount) *RemOut = Rem;
		if (Rem.WordCount == 1) *RemOut = BigInt(Rem.SWords[0]);
		else {
			*RemOut		  = Rem;
			RemOut->Words = Stack_Allocate(Rem.WordCount * sizeof(uhalf));
			Mem_Cpy(RemOut->Words, Rem.Words, Rem.WordCount * sizeof(uhalf));
		}
	}
}

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
	if (!A.WordCount) return A.Word;
	A = BigInt_Flatten(A);
	if (A.WordCount == 1) return A.SWords[0];
	Assert(A.WordCount == 2);
	return ((ssize) A.SWords[1] << SHALF_BITS) | (usize) A.Words[0];
}

#ifndef REGION_BIGINT_TESTS

#define BIGINT_TESTS                                                         \
	TEST(BigInt, ConstructsPositives, (                                      \
		bigint Result = BigInt(123);                                         \
		Assert(Result.WordCount == 0);                                       \
		Assert(Result.Word == 123);                                          \
	))                                                                       \
	TEST(BigInt, ConstructsNegatives, (                                      \
		bigint Result = BigInt(SHALF_MIN);                                   \
		Assert(Result.WordCount == 0);                                       \
		Assert(Result.Word == SHALF_MIN);                                    \
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
	TEST(BigInt_SInit, InitializesWithVarargs, (                             \
        bigint Result = BigInt_SInit(3, 1, 5, -8);                           \
        Assert(Result.WordCount == 3);                                       \
        Assert(Result.Words[0] == 1);                                        \
        Assert(Result.Words[1] == 5);                                        \
        Assert(Result.Words[2] == -8);                                       \
	))                                                                       \
	TEST(BigInt_SCopy, ReturnsAllocatedCopy, (                               \
		bigint Source = BigInt_SInit(3, 100, 200, 300);                      \
		bigint Result = BigInt_SCopy(Source);                                \
		Assert(Result.WordCount == 3);                                       \
		Assert(Mem_Cmp(Result.Words, Source.Words, 3 * sizeof(uhalf)) == 0); \
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
		bigint Source = BigInt_SInit(1, 333);                                \
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
		bigint Source = BigInt_SInit(3, 0, 0, 0);                            \
		Assert(BigInt_IsZero(Source));                                       \
		Source.Words[1] = 1;                                                 \
		Assert(!BigInt_IsZero(Source));                                      \
	))                                                                       \
	TEST(BigInt_IsNegative, ChecksWordOnZeroCount, (                         \
		Assert(BigInt_IsNegative(BigInt(-23)));                              \
		Assert(!BigInt_IsNegative(BigInt(0)));                               \
	))                                                                       \
	TEST(BigInt_IsNegative, ChecksAllWordsOnMultiCount, (                    \
		bigint Source = BigInt_SInit(3, 0, 0, 0);                            \
		Assert(!BigInt_IsNegative(Source));                                  \
		Source.SWords[2] = SHALF_MIN;                                        \
		Assert(BigInt_IsNegative(Source));                                   \
	))                                                                       \
	TEST(BigInt_Flatten, NoChangeForZeroCount, (                             \
		bigint Result = BigInt_Flatten(BigInt(3));                           \
		Assert(Result.WordCount == 0);                                       \
		Assert(Result.Word == 3);                                            \
	))                                                                       \
	TEST(BigInt_Flatten, FlattensUnnecessarySignExtensions, (                \
		bigint Source = BigInt_SInit(5, -32, 0, -35, -1, -1);                \
		Assert(BigInt_Flatten(Source).WordCount == 3);                       \
		Source.SWords[3] = 3;                                                \
		Assert(BigInt_Flatten(Source).WordCount == 5);                       \
	))                                                                       \
	TEST(BigInt_Flatten, FlattensUnnecessaryZeroes, (                        \
		bigint Source = BigInt_SInit(5, -32, 0, 35, 0, 0);                   \
		Assert(BigInt_Flatten(Source).WordCount == 3);                       \
		Source.Words[3] = UHALF_MAX;                                         \
		Assert(BigInt_Flatten(Source).WordCount == 5);                       \
	))                                                                       \
	TEST(BigInt_Splice, SplicesCorrectly, (                                  \
		bigint Source = BigInt_SInit(5, -32, 0, 35, 0, 0);                   \
		bigint Result = BigInt_Splice(&Source, 1, 4);                        \
		Assert(Result.WordCount == 3);                                       \
		Assert(Result.SWords[0] == 0);                                       \
		Assert(Result.SWords[1] == 35);                                      \
		Assert(Result.SWords[2] == 0);                                       \
	))                                                                       \
	TEST(BigInt_Splice, TruncatesIfOverflow, (                               \
		bigint Source = BigInt_SInit(2, -32, 35);                            \
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
		bigint A = BigInt_SInit(2, 0x29482715, 0x80192747);                  \
		bigint B = BigInt_SInit(3, 0x81acd923, 0x1a1a1a1a, 0x20400970);      \
		bigint Result = BigInt_SAnd(A, B);                                   \
		Assert(Result.WordCount == 3);                                       \
		Assert(Result.SWords[0] == (A.SWords[0] & B.SWords[0]));             \
		Assert(Result.SWords[1] == (A.SWords[1] & B.SWords[1]));             \
		Assert(Result.SWords[2] == B.SWords[2]);                             \
	))                                                                       \
	TEST(BigInt_SOr, ORsCorrectly, (                                         \
		bigint A = BigInt_SInit(2, 0x29482715, 0x80192747);                  \
		bigint B = BigInt_SInit(3, 0x81acd923, 0x1a1a1a1a, 0x20400970);      \
		bigint Result = BigInt_SOr(A, B);                                    \
		Assert(Result.WordCount == 2);                                       \
		Assert(Result.SWords[0] == (A.SWords[0] | B.SWords[0]));             \
		Assert(Result.SWords[1] == (A.SWords[1] | B.SWords[1]));             \
	))                                                                       \
	TEST(BigInt_SXor, XORsCorrectly, (                                       \
		bigint A = BigInt_SInit(2, 0x29482715, 0x80192747);                  \
		bigint B = BigInt_SInit(3, 0, -1, 0x20400970);                       \
		bigint Result = BigInt_SXor(A, B);                                   \
		Assert(Result.WordCount == 3);                                       \
		Assert(Result.SWords[0] == (A.SWords[0] ^ B.SWords[0]));             \
		Assert(Result.SWords[1] == (A.SWords[1] ^ B.SWords[1]));             \
		Assert(Result.SWords[2] == ~B.SWords[2]);                            \
	))                                                                       \
	TEST(BigInt_SInvert, InvertsCorrectly, (                                 \
		bigint Source = BigInt_SInit(4, 0, -1, SHALF_MAX, SHALF_MIN);        \
		bigint Result = BigInt_SInvert(Source);                              \
		Assert(Result.WordCount == 4);                                       \
		Assert(Result.SWords[0] == -1);                                      \
		Assert(Result.SWords[1] == 0);                                       \
		Assert(Result.SWords[2] == SHALF_MIN);                               \
		Assert(Result.SWords[3] == SHALF_MAX);                               \
	))                                                                       \
	TEST(BigInt_SShift, ReturnsSourceOnZeroShift, (                          \
		bigint Source = BigInt_SInit(2, 1, 0);                               \
		bigint Result = BigInt_SShift(Source, 0);                            \
		Assert(Result.WordCount == 1);                                       \
		Assert(Result.Words == Source.Words);                                \
	))                                                                       \
	TEST(BigInt_SShift, ShiftsRightMultipleWords, (                          \
		bigint Source = BigInt_SInit(3, 0, 0, 64);                           \
		bigint Result = BigInt_SShift(Source, -SHALF_BITS - 2);              \
		Assert(Result.WordCount == 2);                                       \
		Assert(Result.SWords[0] == 0);                                       \
		Assert(Result.SWords[1] == 16);                                      \
		Source.SWords[2] = -64;                                              \
		Result = BigInt_SShift(Source, -(SHALF_BITS * 3));                   \
		Assert(Result.WordCount == 0);                                       \
		Assert(Result.Word == -1);                                           \
	))                                                                       \
	TEST(BigInt_SShift, ShiftsRightWordBoundary, (                           \
		bigint Source = BigInt_SInit(3, 0, 1, 0);                            \
		bigint Result = BigInt_SShift(Source, -1);                           \
		Assert(Result.WordCount == 2);                                       \
		Assert(Result.SWords[0] == SHALF_MIN);                               \
		Assert(Result.SWords[1] == 0);                                       \
		Source.SWords[1] = SHALF_MIN;                                        \
		Result = BigInt_SShift(Source, -SHALF_BITS + 1);                     \
		Assert(Result.WordCount == 2);                                       \
		Assert(Result.SWords[0] == 0);                                       \
		Assert(Result.SWords[1] == 1);                                       \
	))                                                                       \
	TEST(BigInt_SShift, ShiftsLeftMultipleWords, (                           \
		bigint Source = BigInt_SInit(2, 0, 16);                              \
		bigint Result = BigInt_SShift(Source, SHALF_BITS + 2);               \
		Assert(Result.WordCount == 3);                                       \
		Assert(Result.SWords[0] == 0);                                       \
		Assert(Result.SWords[1] == 0);                                       \
		Assert(Result.SWords[2] == 64);                                      \
		Source.SWords[1] = -16;                                              \
		Result = BigInt_SShift(Source, SHALF_BITS);                          \
		Assert(Result.WordCount == 3);                                       \
		Assert(Result.SWords[0] == 0);                                       \
		Assert(Result.SWords[1] == 0);                                       \
		Assert(Result.SWords[2] == -16);                                     \
	))                                                                       \
	TEST(BigInt_SShift, ShiftsLeftWordBoundary, (                            \
		bigint Source = BigInt_SInit(2, SHALF_MIN, 0);                       \
		bigint Result = BigInt_SShift(Source, 1);                            \
		Assert(Result.WordCount == 2);                                       \
		Assert(Result.SWords[0] == 0);                                       \
		Assert(Result.SWords[1] == 1);                                       \
		Source.SWords[0] = 1;                                                \
		Result = BigInt_SShift(Source, SHALF_BITS - 1);                      \
		Assert(Result.WordCount == 2);                                       \
		Assert(Result.SWords[0] == SHALF_MIN);                               \
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
		bigint Result = BigInt_SAdd(BigInt(SHALF_MIN), BigInt(-1));          \
		Assert(Result.WordCount == 2);                                       \
		Assert(Result.SWords[0] == SHALF_MAX);                               \
		Assert(Result.SWords[1] == -1);                                      \
	))                                                                       \
	TEST(BigInt_SAdd, OverflowsCorrectly, (                                  \
		bigint Result = BigInt_SAdd(BigInt(SHALF_MAX), BigInt(1));           \
		Assert(Result.WordCount == 2);                                       \
		Assert(Result.SWords[0] == SHALF_MIN);                               \
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
		Source.SWords[1] = SHALF_MIN;                                        \
		bigint Result = BigInt_SNegate(Source);                              \
		Assert(Result.WordCount == 3);                                       \
		Assert(Result.SWords[0] == 0);                                       \
		Assert(Result.SWords[1] == SHALF_MIN);                               \
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
		Mem_Set(A.SWords, 0, sizeof(uhalf) * 6);                             \
		A.SWords[6] = 3;                                                     \
		bigint B = BigInt_SAllocate(3);                                      \
		Mem_Set(B.SWords, 0, sizeof(uhalf) * 2);                             \
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
	TEST(BigInt_ToInt, CoalescesTwoWords, (                                  \
		bigint Source = BigInt_SAllocate(2);                                 \
		Source.SWords[0] = 0;                                                \
		Source.SWords[1] = 1;                                                \
		ssize Result = BigInt_ToInt(Source);                                 \
		Assert(Result == (1ull << SHALF_BITS));                              \
	))                                                                       \
	//

#endif

#endif
