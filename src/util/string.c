/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
**                                                                         **
**  Author: Aria Seiler                                                    **
**                                                                         **
**  This program is in the public domain. There is no implied warranty,    **
**  so use it at your own risk.                                            **
**                                                                         **
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef INCLUDE_HEADER

#define EString() CLStringL("")
#define CLStringL(Literal) CLString(Literal, sizeof(Literal)-1)
#define CFStringL(Literal, ...) FString(CLStringL(Literal), __VA_ARGS__)

typedef struct string {
	u64 Length;
	u64 Capacity;
	
	b08 Resizable;
	
	c08 *Text;
} string;

#define STRING_FUNCS \
	EXPORT(string, CLString,         c08 *Chars, u64 Length) \
	EXPORT(string, CString,          c08 *Chars) \
	EXPORT(string, CNString,         c08 *Chars) \
	EXPORT(string, LString,          u64 Length) \
	EXPORT(string, FString,          string Format, ...) \
	EXPORT(string, CFString,         c08 *Format, ...) \
	\
	EXPORT(string, String_Cat,       string A, string B) \
	EXPORT(string, String_Terminate, string A) \
	EXPORT(s08,    String_Cmp,       string A, string B) \
	\
	EXPORT(string, U64_ToString,     u64 N, u08 Radix) \
	EXPORT(string, U32_ToString,     u32 N, u08 Radix) \
	EXPORT(string, U16_ToString,     u16 N, u08 Radix) \
	EXPORT(string, U08_ToString,     u08 N, u08 Radix) \
	EXPORT(string, S64_ToString,     s64 N) \
	EXPORT(string, S32_ToString,     s32 N) \
	EXPORT(string, S16_ToString,     s16 N) \
	EXPORT(string, S08_ToString,     s08 N) \
	EXPORT(string, R32_ToString,     r32 N, u32 DigitsAfterDecimal) \

#endif

#ifdef INCLUDE_SOURCE

persist c08 *BaseChars = "0123456789abcdef0123456789ABCDEF";

internal string
CLString(c08 *Chars,
		 u64 Length)
{
	string String;
	String.Length = Length;
	String.Capacity = Length;
	String.Resizable = FALSE;
	String.Text = Chars;
	return String;
}
internal string CString(c08 *Chars) { return CLString(Chars, Mem_BytesUntil(Chars, 0)); }
internal string CNString(c08 *Chars) { return CLString(Chars, Mem_BytesUntil(Chars, 0)+1); }
internal string LString(u64 Length) { return CLString(Stack_Allocate(Length), Length); }

internal void
SetVAArgsToIndex(va_list ArgsStart, va_list *Args, u32 Index)
{
	#ifdef _X64
		*Args = ArgsStart + 8*Index;
	#endif
}

typedef struct format_flags {
	s64 MinChars;
	s64 Precision;
	u32 ConversionIndex;
	b08 AlignLeft        : 1;
	b08 PrefixPlus       : 1;
	b08 PrefixSpace      : 1;
	b08 HashFlag         : 1;
	b08 HasSeparatorChar : 1;
	b08 PadZero          : 1;
	b08 Caps             : 1;
	b08 CustomPrecision  : 1;
	b08 CustomConversionIndex : 1;
	b08 TypeSize              : 2;
	b08 CustomTypeSize        : 1;
} format_flags;

internal void
WritePadding(
	c08 **Out,
	s32 *Padding)
{
	while(*Padding-- > 0) *(*Out)++ = ' ';
}

internal void
WriteSignedNumber(
	c08 **Out,
	s64 Value,
	format_flags Flags)
{
	if(!Flags.CustomTypeSize) Value = (s32) Value;
	else if(Flags.TypeSize == 0) Value = (s08) Value;
	else if(Flags.TypeSize == 1) Value = (s16) Value;
	else if(Flags.TypeSize == 2) Value = (s32) Value;
	else if(Flags.TypeSize == 3) Value = (s64) Value;
	
	s64 OriginalValue = (s64)Value;
	
	if(!Flags.CustomPrecision) Flags.Precision = 1;
	
	b08 Negative = (Value < 0);
	b08 HasPrefix = Negative || Flags.PrefixSpace || Flags.PrefixPlus;
	c08 Prefix;
	if(Negative) Prefix = '-';
	else if(Flags.PrefixPlus) Prefix = '+';
	else if(Flags.PrefixSpace) Prefix = ' ';
	
	s32 Len = 0;
	while(Value > 0) {
		Value /= 10;
		Len++;
	}
	Len = MAX(Len, Flags.Precision);
	
	s32 SeparatorCount = 0;
	if(Flags.HasSeparatorChar) SeparatorCount = (Len-1)/3;
	s32 MinDigits = Flags.MinChars - HasPrefix - SeparatorCount;
	if(Flags.PadZero && !Flags.AlignLeft && !Flags.CustomPrecision)
		Len = MAX(MinDigits, Len);
	s32 Padding = MinDigits - Len;
	
	Value = OriginalValue * (1-2*Negative);
	
	if(!Flags.AlignLeft) WritePadding(Out, &Padding);
	
	if(HasPrefix) *(*Out)++ = Prefix;
	
	u32 IStart = Len + SeparatorCount - 1;
	for(s32 I = IStart; I >= 0; I--) {
		if(Flags.HasSeparatorChar && (IStart-I+1)%4 == 0) {
			(*Out)[I] = ',';
		} else {
			(*Out)[I] = (Value%10) + '0';
			Value /= 10;
		}
	}
	*Out += IStart + 1;
	
	WritePadding(Out, &Padding);
}

internal void
WriteBoolean(
	c08 **Out,
	b64 Value,
	format_flags Flags)
{
	if(!Flags.CustomTypeSize) Value = (b08) Value;
	else if(Flags.TypeSize == 0) Value = (b08) Value;
	else if(Flags.TypeSize == 1) Value = (b16) Value;
	else if(Flags.TypeSize == 2) Value = (b32) Value;
	else if(Flags.TypeSize == 3) Value = (b64) Value;
	
	c08 FalseStr[] = "False";
	c08 TrueStr[] = "True";
	
	s32 Len;
	if(Value) Len = sizeof(TrueStr)-1;
	else      Len = sizeof(FalseStr)-1;
	s32 Padding = Flags.MinChars - Len;
	
	if(!Flags.AlignLeft) WritePadding(Out, &Padding);
	
	Mem_Cpy(*Out, (Value) ? TrueStr : FalseStr, Len);
	*Out += Len;
	
	WritePadding(Out, &Padding);
}

internal void
WriteUnsignedNumber(
	c08 **Out,
	u64 Value,
	c08 Conversion,
	format_flags Flags)
{
	if(!Flags.CustomTypeSize) Value = (u32) Value;
	else if(Flags.TypeSize == 0) Value = (u08) Value;
	else if(Flags.TypeSize == 1) Value = (u16) Value;
	else if(Flags.TypeSize == 2) Value = (u32) Value;
	else if(Flags.TypeSize == 3) Value = (u64) Value;
	
	s64 OriginalValue = (u64)Value;
	
	if(!Flags.CustomPrecision) Flags.Precision = 1;
	
	u32 Radix;
	u32 SeparatorGap;
	c08 SeparatorChar;
	if(Conversion == 'b' || Conversion == 'B') {
		Radix = 2;
		SeparatorGap = 8;
		SeparatorChar = '_';
	} else if(Conversion == 'o') {
		Radix = 8;
		SeparatorGap = 4;
		SeparatorChar = '_';
	} else if(Conversion == 'u') {
		Radix = 10;
		SeparatorGap = 3;
		SeparatorChar = ',';
	} else if(Conversion == 'x' || Conversion == 'X') {
		Radix = 16;
		SeparatorGap = 4;
		SeparatorChar = '_';
	}
	
	b08 PrefixLen = Flags.HashFlag * (2*(Radix == 2) + (Radix == 8) + 2*(Radix == 16));
	s32 Len = 0;
	while(Value > 0) {
		Value /= Radix;
		Len++;
	}
	Len = MAX(Len, Flags.Precision);
	
	s32 SeparatorCount = 0;
	if(Flags.HasSeparatorChar) SeparatorCount = (Len-1)/SeparatorGap;
	s32 MinDigits = Flags.MinChars - PrefixLen - SeparatorCount;
	if(Flags.PadZero && !Flags.AlignLeft && !Flags.CustomPrecision)
		Len = MAX(MinDigits, Len);
	s32 Padding = MinDigits - Len;
	
	if(!Flags.AlignLeft) WritePadding(Out, &Padding);
	
	if(PrefixLen && Radix == 2) {
		*(*Out)++ = '0';
		*(*Out)++ = Conversion;
	} else if(PrefixLen && Radix == 8)
		*(*Out)++ = '0';
	else if(PrefixLen && Radix == 16) {
		*(*Out)++ = '0';
		*(*Out)++ = Conversion;
	}
	
	u32 Offset = Flags.Caps * 16;
	Value = OriginalValue;
	u32 IStart = Len + SeparatorCount - 1;
	for(s32 I = IStart; I >= 0; I--) {
		if(Flags.HasSeparatorChar && (IStart-I+1)%(SeparatorGap+1) == 0) {
			(*Out)[I] = SeparatorChar;
		} else {
			(*Out)[I] = BaseChars[Value%Radix + Offset];
			Value /= Radix;
		}
	}
	*Out += IStart + 1;
	
	WritePadding(Out, &Padding);
}

internal void
WriteCharacter(
	c08 **Out,
	s64 Value,
	format_flags Flags)
{
	if(Flags.CustomTypeSize && Flags.TypeSize == 2)
		Assert(FALSE, "Wide chars not implemented!");
	else Value = (c08) Value;
	
	s32 Len = 1;
	s32 Padding = Flags.MinChars - Len;
	
	if(!Flags.AlignLeft) WritePadding(Out, &Padding);
	
	*(*Out)++ = Value;
	
	WritePadding(Out, &Padding);
}

internal void
WriteString(
	c08 **Out,
	string Value,
	format_flags Flags)
{
	if(Flags.CustomTypeSize && Flags.TypeSize == 2)
		Assert(FALSE, "Wide strings not implemented!");
	
	if(!Value.Text) Value = CLStringL("(null)");
	
	c08 *In = Value.Text;
	c08 *Start = *Out;
	
	s32 Len = Value.Length;
	if(Flags.CustomPrecision) Len = MIN(Len, Flags.Precision);
	s32 Padding = Flags.MinChars - Len;
	
	if(!Flags.AlignLeft) WritePadding(Out, &Padding);
	
	while(Len--) *(*Out)++ = *In++;
	
	WritePadding(Out, &Padding);
}

internal void
WritePointer(
	c08 **Out,
	vptr Value,
	format_flags Flags)
{
	format_flags NewFlags = {0};
	NewFlags.MinChars = Flags.MinChars;
	NewFlags.AlignLeft = Flags.AlignLeft;
	NewFlags.HasSeparatorChar = Flags.HasSeparatorChar;
	
	if(!Value) {
		WriteString(Out, CLStringL("(nil)"), NewFlags);
	} else {
		*(*Out)++ = '0';
		*(*Out)++ = 'x';
		WriteUnsignedNumber(Out, (u64) Value, 'X', NewFlags);
	}
}

internal b08
WriteSpecialtyFloat(
	c08 **Out,
	r64 Value,
	format_flags Flags)
{
	if(Flags.CustomTypeSize && Flags.TypeSize == 3)
		Assert(FALSE, "Long doubles not implemented!");
	
	format_flags StringFlags = {0};
	StringFlags.MinChars = Flags.MinChars;
	StringFlags.AlignLeft = Flags.AlignLeft;
	
	if(Value == R64_INF) {
		WriteString(Out, Flags.Caps ? CLStringL("INF") : CLStringL("inf"), StringFlags);
	} else if(Value == R64_NINF) {
		WriteString(Out, Flags.Caps ? CLStringL("-INF") : CLStringL("-inf"), StringFlags);
	} else if(Value != Value) {
		u64 Binary = FORCE_CAST(u64, Value);
		u32 FracMax = (R64_MANTISSA_BITS+3)/4;
		u32 Offset = Flags.Caps * 16;
		
		string String = Flags.Caps ? CLStringL("-SNAN[0XFFFFFFFFFFFFF]") : CLStringL("-snan[0xfffffffffffff]");
		String.Text[1] = (Binary & R64_QUIET_MASK) ? (Flags.Caps ? 'Q' : 'q') : (Flags.Caps ? 'S' : 's');
		
		u32 Len = 8;
		u64 MantissaBits = Binary & R64_MANTISSA_MASK;
		do { Len++; } while(MantissaBits /= 16);
		
		MantissaBits = Binary & R64_MANTISSA_MASK;
		for(s32 I = Len-1; I >= 0; I--) {
			u32 Digit = MantissaBits & 0xF;
			String.Text[I] = BaseChars[Digit + Offset];
			MantissaBits >>= 4;
		}
		String.Text[Len] = ']';
		String.Length = Len+1;
		
		if(Binary & R64_SIGN_MASK) {
			String.Text++;
			String.Length--;
		}
		
		WriteString(Out, String, StringFlags);
	} else {
		return FALSE;
	}
	
	return TRUE;
}

internal void
WriteHexadecimalFloat(
	c08 **Out,
	r64 Value,
	format_flags Flags)
{
	if(WriteSpecialtyFloat(Out, Value, Flags)) return;
	
	if(Flags.CustomTypeSize && Flags.TypeSize == 3)
		Assert(FALSE, "Long doubles not implemented!");
	
	u32 FracMax = (R64_MANTISSA_BITS+3)/4;
	u32 Offset = Flags.Caps * 16;
	
	u64 Binary = FORCE_CAST(u64, Value);
	
	s32 Sign         = (Binary & R64_SIGN_MASK) >> R64_SIGN_SHIFT;
	s32 Exponent     = (s32)((Binary & R64_EXPONENT_MASK) >> R64_EXPONENT_SHIFT) - R64_EXPONENT_BIAS;
	u64 MantissaBits = Binary & R64_MANTISSA_MASK;
	
	b08 Negative = Sign;
	b08 HasPrefix = Negative || Flags.PrefixSpace || Flags.PrefixPlus;
	c08 Prefix = Negative ? '-' : Flags.PrefixPlus ? '+' : ' ';
	
	b08 IsDenormal = (Exponent == -R64_EXPONENT_BIAS);
	if(IsDenormal) Exponent++;
	if(Value == 0) Exponent = 0;
	
	u32 Index;
	b08 ValidIndex = Intrin_BitScanForward64(&Index, MantissaBits);
	u32 FracLen = ValidIndex ? (R64_MANTISSA_BITS - Index) / 4 + 1 : 0;
	FracLen = MAX(Flags.Precision, FracLen);
	
	b08 ExpIsNegative = Exponent < 0;
	Exponent *= 1 - 2*ExpIsNegative;
	b08 HasExpSign = ExpIsNegative || Flags.PrefixPlus;
	b08 ExpHasComma = Flags.HasSeparatorChar && (Exponent > 999);
	s32 ExpLen = 1 + ExpHasComma;
	s32 ExpCpy = Exponent;
	while(ExpCpy /= 10) ExpLen++;
	
	b08 HasDecimal = FracLen || Flags.HashFlag;
	u32 TotalLen = HasPrefix + 2 + 1 + HasDecimal + FracLen + 1 + HasExpSign + ExpLen;
	
	s32 Padding = Flags.MinChars - TotalLen;
	
	if(!Flags.AlignLeft) WritePadding(Out, &Padding);
	
	if(HasPrefix) *(*Out)++ = Prefix;
	
	*(*Out)++ = '0';
	*(*Out)++ = (Flags.Caps) ? 'X' : 'x';
	
	*(*Out)++ = IsDenormal ? '0' : '1';
	
	if(HasDecimal) *(*Out)++ = '.';
	
	for(u32 I = 0; I < FracLen; I++) {
		u32 Digit = (MantissaBits >> (4*(FracMax-I-1))) & 0xF;
		*(*Out)++ = BaseChars[Digit + Offset];
	}
	
	*(*Out)++ = (Flags.Caps) ? 'P' : 'p';
	
	if(HasExpSign)
		*(*Out)++ = ExpIsNegative ? '-' : '+';
	
	for(s32 I = ExpLen-1; I >= 0; I--) {
		if(ExpHasComma && I == 1) {
			(*Out)[I] = ',';
		} else {
			(*Out)[I] = (Exponent%10) + '0';
			Exponent /= 10;
		}
	}
	*Out += ExpLen;
	
	WritePadding(Out, &Padding);
}

internal void
WriteFloat(
	c08 **Out,
	r64 Value,
	format_flags Flags)
{
	if(WriteSpecialtyFloat(Out, Value, Flags)) return;
	
	if(Flags.CustomTypeSize && Flags.TypeSize == 3)
		Assert(FALSE, "Long doubles not implemented!");
	
	u64 Binary = FORCE_CAST(u64, Value);
	
	b08 Sign     = (Binary & R64_SIGN_MASK) >> R64_SIGN_SHIFT;
	s32 Exponent = (s32)((Binary & R64_EXPONENT_MASK) >> R64_EXPONENT_SHIFT) - R64_EXPONENT_BIAS;
	u64 Mantissa = Binary & R64_MANTISSA_MASK;
	
	b08 Denormal = FALSE;
	if(Exponent == -R64_EXPONENT_BIAS) {
		Exponent++;
		Denormal = TRUE;
	}
	
	b08 Negative = Sign;
	b08 HasPrefix = Negative || Flags.PrefixSpace || Flags.PrefixPlus;
	c08 Prefix = Negative ? '-' : Flags.PrefixPlus ? '+' : ' ';
	
	vptr Cursor = Stack_GetCursor();
	vptr TempCursor = (vptr)ALIGN_UP((u64)Out + Flags.MinChars, 8);
	Stack_SetCursor(TempCursor);
	
	bigint Whole = BigInt_InitV(0);
	s32 WholeLen = 1;
	if(Exponent >= 0) {
		Whole = BigInt_Init(1 + Exponent / 64);
		Whole.WordCount = Whole.MaxCount;
		
		Whole.Words[Exponent / 64] |= !Denormal << (Exponent % 64);
		for(s32 I = 0; I <= R64_MANTISSA_BITS; I++) {
			if(Exponent + I < R64_MANTISSA_BITS) continue;
			u64 Bit = Exponent - R64_MANTISSA_BITS + I;
			u64 IsSet = (Mantissa >> I) & 1;
			if(Whole.WordCount < Bit / 64) Whole.WordCount += IsSet;
			Whole.Words[Bit / 64] |= IsSet << (Bit % 64);
		}
		
		TempCursor = Stack_GetCursor();
		
		bigint_divmod DivMod = {Whole, BigInt_InitV(0)};
		bigint Divisor = BigInt_InitV(10);
		do {
			DivMod = BigInt_DivMod(DivMod.Quotient, Divisor);
		} while(DivMod.Quotient.WordCount && WholeLen++);
	}
	
	u32 TotalLen = HasPrefix;
	
	s32 Padding = Flags.MinChars - TotalLen;
	
	if(!Flags.AlignLeft) WritePadding(Out, &Padding);
	
	Stack_SetCursor(TempCursor);
	
	bigint_divmod DivMod = {Whole, BigInt_InitV(0)};
	bigint Divisor = BigInt_InitV(10);
	while(WholeLen--) {
		DivMod = BigInt_DivMod(DivMod.Quotient, Divisor);
		*(*Out)++ = '0' + (DivMod.Remainder.WordCount ? DivMod.Remainder.Words[0] : 0);
	}
	
	if(HasPrefix) *(*Out)++ = Prefix;
	
	WritePadding(Out, &Padding);
	
	Stack_SetCursor(Cursor);
}

internal u32
ReadNumber(
	c08 **In)
{
	u32 Value = 0;
	
	while(**In >= '0' && **In <= '9')
		Value = Value * 10 + *(*In)++ - '0';
	
	return Value;
}

internal void
ReadConversionIndex(
	c08 **In,
	format_flags *Flags)
{
	if(**In < '0' || **In > '9') return;

	u32 Index = ReadNumber(In);
	
	if(**In == '$') {
		Flags->CustomConversionIndex = TRUE;
		Flags->ConversionIndex = Index;
		(*In)++;
	} else {
		Flags->MinChars = Index;
	}
}

internal void
ReadActualFlags(
	c08 **In,
	format_flags *Flags)
{
	while(TRUE) {
		switch(**In) {
			case '-':  Flags->AlignLeft        = TRUE; break;
			case '+':  Flags->PrefixPlus       = TRUE; break;
			case ' ':  Flags->PrefixSpace      = TRUE; break;
			case '#':  Flags->HashFlag         = TRUE; break;
			case '\'': Flags->HasSeparatorChar = TRUE; break;
			case '0':  Flags->PadZero          = TRUE; break;
			default: return;
		}
		(*In)++;
	}
}

internal s64
ReadArgsFlag(
	c08 **In,
	format_flags *Flags,
	va_list ArgsStart,
	va_list *Args)
{
	if(**In == '*') {
		(*In)++;
		
		if(**In >= '0' && **In <= '9') {
			u32 Index = ReadNumber(In);
			Assert(**In == '$', "Invalid format!");
			(*In)++;
			
			SetVAArgsToIndex(ArgsStart, Args, Index - 1);
		}
		
		return VA_Next(*Args, s64);
	}
	
	return ReadNumber(In);
}

internal void
ReadMinWidth(
	c08** In,
	format_flags *Flags,
	va_list ArgsStart,
	va_list *Args)
{
	Flags->MinChars = ReadArgsFlag(In, Flags, ArgsStart, Args);
	
	if(Flags->MinChars < 0) {
		Flags->AlignLeft = !Flags->AlignLeft;
		Flags->MinChars = -Flags->MinChars;
	}
}

internal void
ReadPrecision(
	c08 **In,
	format_flags *Flags,
	va_list ArgsStart,
	va_list *Args)
{
	if(**In != '.') return;
	(*In)++;
	
	Flags->CustomPrecision = TRUE;
	Flags->Precision = ReadArgsFlag(In, Flags, ArgsStart, Args);
	
	if(Flags->Precision < 0) Flags->Precision = 0;
}

internal void
ReadTypeModifiers(
	c08 **In,
	format_flags *Flags)
{
	c08 *C = *In;
	Flags->CustomTypeSize = TRUE;
	
	if(C[0] == 'h' && C[1] == 'h') {
		Flags->TypeSize = 0;
		*In += 2;
	} else if(C[0] == 'l' && C[1] == 'l') {
		Flags->TypeSize = 3;
		*In += 2;
	} else {
		if(C[0] == 'h')
			Flags->TypeSize = 1;
		else if(C[0] == 'l')
			Flags->TypeSize = 2;
		else if(C[0] == 'L' || C[0] == 'q' || *C == 'j' || *C == 'z' || *C == 'Z' || *C == 't')
			Flags->TypeSize = 3;
		else {
			Flags->CustomTypeSize = FALSE;
			return;
		}
		(*In)++;
	}
}

internal format_flags
ReadFlags(
	c08 **In,
	va_list ArgsStart,
	va_list *Args)
{
	format_flags Flags = {0};
	
	ReadConversionIndex(In, &Flags);
	ReadActualFlags(In, &Flags);
	ReadMinWidth(In, &Flags, ArgsStart, Args);
	ReadPrecision(In, &Flags, ArgsStart, Args);
	ReadTypeModifiers(In, &Flags);
	
	if (**In >= 'A' && **In <= 'Z')
		Flags.Caps = TRUE;
	
	if(Flags.CustomConversionIndex)
		SetVAArgsToIndex(ArgsStart, Args, Flags.ConversionIndex - 1);
	
	return Flags;
}

internal void
WriteFormattedString(
	c08 **In,
	c08 **Out,
	string Result,
	va_list ArgsStart,
	va_list *Args)
{
	format_flags Flags = ReadFlags(In, ArgsStart, Args);
	
	switch(**In) {
		case 'T': WriteBoolean(Out, VA_Next(*Args, s64), Flags); break;
		case 'c': WriteCharacter(Out, VA_Next(*Args, s64), Flags); break;
		case 's': WriteString(Out, VA_Next(*Args, string), Flags); break;
		case 'p': WritePointer(Out, VA_Next(*Args, vptr), Flags); break;
		case 'd': case 'i': WriteSignedNumber(Out, VA_Next(*Args, s64), Flags); break;
		case 'a': case 'A': WriteHexadecimalFloat(Out, VA_Next(*Args, r64), Flags); break;
		case 'f': case 'F': WriteFloat(Out, VA_Next(*Args, r64), Flags); break;
		case 'b': case 'B': case 'o': case 'u': case 'x': case 'X':
			WriteUnsignedNumber(Out, VA_Next(*Args, u64), **In, Flags); break;
		
		case 'e':
		case 'E':
		case 'g':
		case 'G':
			Assert(FALSE, "Floats not implemented!");
			break;
		
		case 'n': {
			s64 Diff = (u64)*Out - (u64)Result.Text;
			if(Flags.CustomTypeSize) {
				s32 *Value = VA_Next(*Args, s32*);
				*Value = Diff;
			} else if(Flags.TypeSize == 0) {
				s08 *Value = VA_Next(*Args, s08*);
				*Value = Diff;
			} else if(Flags.TypeSize == 1) {
				s16 *Value = VA_Next(*Args, s16*);
				*Value = Diff;
			} else if(Flags.TypeSize == 2) {
				s32 *Value = VA_Next(*Args, s32*);
				*Value = Diff;
			} else if(Flags.TypeSize == 3) {
				s64 *Value = VA_Next(*Args, s64*);
				*Value = Diff;
			}
		} break;
		
		default: Assert(FALSE, "Invalid format!");
	}
	
	(*In)++;
}

/* TODO:
  - %f/%F, %e/%E, %g/%G, %m, %S, %C, 
  - %lc, %ls, %La
  - wn, wfn
*/
internal string
VString(
	string Format,
	va_list Args)
{
	va_list ArgsStart = Args;
	
	c08 *Out = Stack_GetCursor();
	string Result;
	Result.Resizable = FALSE;
	Result.Text = Out;
	
	c08 *C = Format.Text;
	while(C < Format.Text+Format.Length) {
		if(*C == '%') {
			C++;
			if(*C == '%') {
				*Out++ = *C++;
			} else {
				WriteFormattedString(&C, &Out, Result, ArgsStart, &Args);
			}
		} else {
			*Out++ = *C++;
		}
	}
	
	Result.Length = (u64)Out - (u64)Result.Text;
	Result.Capacity = Result.Length;
	
	Stack_SetCursor(Out);
	
	return Result;
}

internal string
FString(string Format, ...)
{
	va_list Args;
	VA_Start(Args, Format);
	
	string Result = VString(Format, Args);
	
	VA_End(Args);
	
	return Result;
}

internal string
CFString(c08 *Format, ...)
{
	va_list Args;
	VA_Start(Args, Format);
	
	string Result = VString(CString(Format), Args);
	
	VA_End(Args);
	
	return Result;
}

internal string
String_Cat(string A, string B)
{
	string Result = LString(A.Length + B.Length);
	
	Mem_Cpy(Result.Text, A.Text, A.Length);
	Mem_Cpy(Result.Text+A.Length, B.Text, B.Length);
	
	return Result;
}

internal string
String_Terminate(string A)
{
	if(A.Text[A.Length-1] == 0) return A;
	
	string Result = LString(A.Length + 1);
	
	Mem_Cpy(Result.Text, A.Text, A.Length);
	Result.Text[A.Length] = 0;
	
	return Result;
}

internal s08
_String_CmpCaseInsensitive(string *A, string *B)
{
	Assert(A && B);
	
	if(A->Length < B->Length) return LESS;
	if(A->Length > B->Length) return GREATER;
	
	u32 I = 0;
	while(I < A->Length) {
		c08 CA = (A->Text[I] < 'a') ? A->Text[I] : (A->Text[I] - ('a'-'A'));
		c08 CB = (B->Text[I] < 'a') ? B->Text[I] : (B->Text[I] - ('a'-'A'));
		if(CA != CB) break;
		I++;
	}
	
	if(I == A->Length) return EQUAL;
	if(A->Text[I] < B->Text[I]) return LESS;
	return GREATER;
}

internal s08
String_Cmp(string A, string B)
{
	if(A.Length < B.Length) return LESS;
	if(A.Length > B.Length) return GREATER;
	Assert(A.Text && B.Text);
	
	u32 I = 0;
	while(I < A.Length && A.Text[I] == B.Text[I]) I++;
	
	if(I == A.Length) return EQUAL;
	if(A.Text[I] < B.Text[I]) return LESS;
	return GREATER;
}

internal s08
_String_Cmp(string *A, string *B)
{
	Assert(A && B);
	return String_Cmp(*A, *B);
}

internal string
U64_ToString(u64 N,
			 u08 Radix)
{
	Assert(2 <= Radix && Radix <= 64);
	
	persist c08 Chars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_-";
	
	u32 StrSize = 64+10;
	u32 Index = StrSize;
	
	c08 *LineStr = Stack_Allocate(StrSize);
	
	if(Radix != 2 && Radix != 10 && Radix != 16)
	{
		LineStr[--Index] = ')';
		LineStr[--Index] = (Radix%10) + '0';
		if(Radix >= 10)
			LineStr[--Index] = (Radix/10) + '0';
		LineStr[--Index] = ' ';
		LineStr[--Index] = 'e';
		LineStr[--Index] = 's';
		LineStr[--Index] = 'a';
		LineStr[--Index] = 'b';
		LineStr[--Index] = '(';
		LineStr[--Index] = ' ';
	}
	
	do {
		LineStr[--Index] = Chars[N % Radix];
		N /= Radix;
	} while(N > 0);
	
	if(Radix == 2) {
		LineStr[--Index] = 'b';
		LineStr[--Index] = '0';
	} else if(Radix == 16) {
		LineStr[--Index] = 'x';
		LineStr[--Index] = '0';
	}
	
	return CLString(LineStr + Index, StrSize - Index);
}

internal string U32_ToString(u32 N, u08 Radix) { return U64_ToString((u64)N, Radix); }
internal string U16_ToString(u16 N, u08 Radix) { return U64_ToString((u64)N, Radix); }
internal string U08_ToString(u08 N, u08 Radix) { return U64_ToString((u64)N, Radix); }

internal string
S64_ToString(s64 N)
{
	if(N >= 0)
		return U64_ToString(N, 10);
	
	string Str;
	
	if(N == S64_MIN) {
		Str = U64_ToString(-(N+1), 10);
		Str.Text[Str.Length-1]++;
	} else Str = U64_ToString(-N, 10);
	
	// Guaranteed because U64_ToString over-allocates, to account for bases.
	Str.Length++;
	Str.Capacity++;
	Str.Text--;
	Str.Text[0] = '-';
	
	return Str;
}

internal string
S64_ToCString(s64 N, c08 *Buffer)
{
	b08 Negative = (N < 0);
	
	u32 Len = Negative;
	do Len++; while(N /= 10);
	
	string Result;
	Result.Length = Len;
	Result.Capacity = Len;
	Result.Resizable = FALSE;
	Result.Text = Buffer;
	
	if(Buffer == NULL) return Result;
	
	do {
		Buffer[--Len] = (N % 10) + '0';
		N /= 10;
	} while(N);
	
	if(Negative) Buffer[--Len] = '-';
	
	return Result;
}

internal s64
String_ToS64(string String)
{
	if(!String.Text || !String.Length) return 0;
	
	// 2 so it can be used as a 'for' offset
	b08 Negative = 2*(String.Text[0] == '-');
	
	s64 Total = 0;
	if(Negative)
		Total = -(String.Text[1] + '0');
	
	for(u32 I = Negative; I < String.Length; I++)
		Total = Total*10 + String.Text[I]-'0';
	
	return Total;
}

internal s16
String_ToS16(string String)
{
	s64 Result = String_ToS64(String);
	Assert(S16_MIN <= Result && Result < S16_MAX);
	return Result;
}

internal string S32_ToString(s32 N) { return S64_ToString((s64)N); }
internal string S16_ToString(s16 N) { return S64_ToString((s64)N); }
internal string S08_ToString(s08 N) { return S64_ToString((s64)N); }

internal string
R32_ToString(r32 N, u32 DigitsAfterDecimal)
{
	string Whole = S32_ToString((s32)N);
	
	r32 Frac = N - (s32)N;
	if(Frac == 0) return Whole;
	
	if(Frac < 0) {
		if((s32)N == 0) {
			Whole.Length++;
			Whole.Text--;
			Whole.Text[0] = '-';
		}
		
		Frac *= -1;
	}
	
	string Str = LString(Whole.Length + 1 + DigitsAfterDecimal);
	Mem_Cpy(Str.Text, Whole.Text, Whole.Length);
	Str.Text[Whole.Length] = '.';
	
	for(u32 I = 0; I < DigitsAfterDecimal; I++) {
		Frac *= 10;
		Str.Text[Whole.Length+1+I] = '0' + (s32)Frac;
		Frac -= (s32)Frac;
	}
	
	//NOTE: This truncates, not rounds, the last decimal
	
	return Str;
}

#endif