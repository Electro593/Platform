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
internal string CString(c08 *Chars) { return CLString(Chars, Mem_BytesUntil((u08*) Chars, 0)); }
internal string CNString(c08 *Chars) { return CLString(Chars, Mem_BytesUntil((u08*) Chars, 0)+1); }
internal string LString(u64 Length) { return CLString(Stack_Allocate(Length), Length); }

// internal arg
// ReadArgAtIndex(va_args *Args, u32 Index, type Type) {
// 	//NOTE: Referring to the same argument with multiple types will always use the first one
// 	
// 	while (Args->CacheCount <= Index) {
// 		arg Arg;
// 		switch (Type.ID) {
// 			case TypeID_S08:  Arg.I = va_arg(Args->List, s08) \
// 			case TypeID_S16:  Arg.I = va_arg(Args->List, s16) \
// 			case TypeID_S32:  Arg.I = va_arg(Args->List, s32) \
// 			case TypeID_S64:  Arg.I = va_arg(Args->List, s64) \
// 			case TypeID_U08:  Arg.I = va_arg(Args->List, u08) \
// 			case TypeID_U16:  Arg.I = va_arg(Args->List, u16) \
// 			case TypeID_U32:  Arg.I = va_arg(Args->List, u32) \
// 			case TypeID_U64:  Arg.I = va_arg(Args->List, u64) \
// 			case TypeID_R32:  Arg.R = va_arg(Args->List, r32) \
// 			case TypeID_R64:  Arg.R = va_arg(Args->List, r64) \
// 			case TypeID_C08:  Arg.I = va_arg(Args->List, c08) \
// 			case TypeID_STR:  Arg.P = va_arg(Args->List, c08*) \
// 			case TypeID_VPTR: Arg.P = va_arg(Args->List, vptr) \
// 			default: STOP;
// 		}
// 		Args->Cache[Args->CacheCount++] = Arg;
// 	}
// }

// internal void
// SetVAArgsToIndex(va_list ArgsStart, va_list *Args, u32 Index)
// {
// 	#ifdef _X64
// 		*Args = ArgsStart + 8*Index;
// 	#endif
// }

typedef enum vstring_format_type {
	VSTRING_FORMAT_NONE,
	VSTRING_FORMAT_BOOL,
	VSTRING_FORMAT_INT,
	VSTRING_FORMAT_UINT_BIN,
	VSTRING_FORMAT_UINT_OCT,
	VSTRING_FORMAT_UINT_DEC,
	VSTRING_FORMAT_UINT_HEX,
	VSTRING_FORMAT_FLOAT,
	VSTRING_FORMAT_FLOAT_EXP,
	VSTRING_FORMAT_FLOAT_HEX,
	VSTRING_FORMAT_FLOAT_EITHER,
	VSTRING_FORMAT_CHAR,
	VSTRING_FORMAT_STRING,
	VSTRING_FORMAT_POINTER
} vstring_format_type;

typedef struct vstring_format {
	u64 MinChars  : 30;
	u64 Precision : 30;
	u64 Type      :  4;
	
	// u16 MinCharsIndex;
	u16 PrecisionIndex;
	u16 ConversionIndex;
	
	u08 AlignLeft        : 1;
	u08 PrefixPlus       : 1;
	u08 PrefixSpace      : 1;
	u08 HashFlag         : 1;
	u08 HasSeparatorChar : 1;
	u08 PadZero          : 1;
	u08 Caps             : 1;
	u08 CustomTypeSize   : 1;
	u08 TypeSize              : 2;
	// u08 CustomMinChars        : 1;
	u08 CustomPrecision       : 1;
	u08 CustomConversionIndex : 1;
} vstring_format;

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
	vstring_format Format)
{
	if(!Format.CustomTypeSize) Value = (s32) Value;
	else if(Format.TypeSize == 0) Value = (s08) Value;
	else if(Format.TypeSize == 1) Value = (s16) Value;
	else if(Format.TypeSize == 2) Value = (s32) Value;
	else if(Format.TypeSize == 3) Value = (s64) Value;
	
	s64 OriginalValue = (s64)Value;
	
	if(!Format.CustomPrecision) Format.Precision = 1;
	
	b08 Negative = (Value < 0);
	b08 HasPrefix = Negative || Format.PrefixSpace || Format.PrefixPlus;
	c08 Prefix;
	if(Negative) Prefix = '-';
	else if(Format.PrefixPlus) Prefix = '+';
	else if(Format.PrefixSpace) Prefix = ' ';
	
	s32 Len = 0;
	while(Value > 0) {
		Value /= 10;
		Len++;
	}
	Len = MAX(Len, Format.Precision);
	
	s32 SeparatorCount = 0;
	if(Format.HasSeparatorChar) SeparatorCount = (Len-1)/3;
	s32 MinDigits = Format.MinChars - HasPrefix - SeparatorCount;
	if(Format.PadZero && !Format.AlignLeft && !Format.CustomPrecision)
		Len = MAX(MinDigits, Len);
	s32 Padding = MinDigits - Len;
	
	Value = OriginalValue * (1-2*Negative);
	
	if(!Format.AlignLeft) WritePadding(Out, &Padding);
	
	if(HasPrefix) *(*Out)++ = Prefix;
	
	u32 IStart = Len + SeparatorCount - 1;
	for(s32 I = IStart; I >= 0; I--) {
		if(Format.HasSeparatorChar && (IStart-I+1)%4 == 0) {
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
	vstring_format Format)
{
	if(!Format.CustomTypeSize) Value = (b08) Value;
	else if(Format.TypeSize == 0) Value = (b08) Value;
	else if(Format.TypeSize == 1) Value = (b16) Value;
	else if(Format.TypeSize == 2) Value = (b32) Value;
	else if(Format.TypeSize == 3) Value = (b64) Value;
	
	c08 FalseStr[] = "False";
	c08 TrueStr[] = "True";
	
	s32 Len;
	if(Value) Len = sizeof(TrueStr)-1;
	else      Len = sizeof(FalseStr)-1;
	s32 Padding = Format.MinChars - Len;
	
	if(!Format.AlignLeft) WritePadding(Out, &Padding);
	
	Mem_Cpy(*Out, (Value) ? TrueStr : FalseStr, Len);
	*Out += Len;
	
	WritePadding(Out, &Padding);
}

internal void
WriteUnsignedNumber(
	c08 **Out,
	u64 Value,
	c08 Conversion,
	vstring_format Format)
{
	if(!Format.CustomTypeSize) Value = (u32) Value;
	else if(Format.TypeSize == 0) Value = (u08) Value;
	else if(Format.TypeSize == 1) Value = (u16) Value;
	else if(Format.TypeSize == 2) Value = (u32) Value;
	else if(Format.TypeSize == 3) Value = (u64) Value;
	
	s64 OriginalValue = (u64)Value;
	
	if(!Format.CustomPrecision) Format.Precision = 1;
	
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
	
	b08 PrefixLen = Format.HashFlag * (2*(Radix == 2) + (Radix == 8) + 2*(Radix == 16));
	s32 Len = 0;
	while(Value > 0) {
		Value /= Radix;
		Len++;
	}
	Len = MAX(Len, Format.Precision);
	
	s32 SeparatorCount = 0;
	if(Format.HasSeparatorChar) SeparatorCount = (Len-1)/SeparatorGap;
	s32 MinDigits = Format.MinChars - PrefixLen - SeparatorCount;
	if(Format.PadZero && !Format.AlignLeft && !Format.CustomPrecision)
		Len = MAX(MinDigits, Len);
	s32 Padding = MinDigits - Len;
	
	if(!Format.AlignLeft) WritePadding(Out, &Padding);
	
	if(PrefixLen && Radix == 2) {
		*(*Out)++ = '0';
		*(*Out)++ = Conversion;
	} else if(PrefixLen && Radix == 8)
		*(*Out)++ = '0';
	else if(PrefixLen && Radix == 16) {
		*(*Out)++ = '0';
		*(*Out)++ = Conversion;
	}
	
	u32 Offset = Format.Caps * 16;
	Value = OriginalValue;
	u32 IStart = Len + SeparatorCount - 1;
	for(s32 I = IStart; I >= 0; I--) {
		if(Format.HasSeparatorChar && (IStart-I+1)%(SeparatorGap+1) == 0) {
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
	vstring_format Format)
{
	if(Format.CustomTypeSize && Format.TypeSize == 2)
		Assert(FALSE, "Wide chars not implemented!");
	else Value = (c08) Value;
	
	s32 Len = 1;
	s32 Padding = Format.MinChars - Len;
	
	if(!Format.AlignLeft) WritePadding(Out, &Padding);
	
	*(*Out)++ = Value;
	
	WritePadding(Out, &Padding);
}

internal void
WriteString(
	c08 **Out,
	string Value,
	vstring_format Format)
{
	if(Format.CustomTypeSize && Format.TypeSize == 2)
		Assert(FALSE, "Wide strings not implemented!");
	
	if(!Value.Text) Value = CLStringL("(null)");
	
	c08 *In = Value.Text;
	c08 *Start = *Out;
	
	s32 Len = Value.Length;
	if(Format.CustomPrecision) Len = MIN(Len, Format.Precision);
	s32 Padding = Format.MinChars - Len;
	
	if(!Format.AlignLeft) WritePadding(Out, &Padding);
	
	while(Len--) *(*Out)++ = *In++;
	
	WritePadding(Out, &Padding);
}

internal void
WritePointer(
	c08 **Out,
	vptr Value,
	vstring_format Format)
{
	vstring_format NewFormat = {0};
	NewFormat.MinChars = Format.MinChars;
	NewFormat.AlignLeft = Format.AlignLeft;
	NewFormat.HasSeparatorChar = Format.HasSeparatorChar;
	
	if(!Value) {
		WriteString(Out, CLStringL("(nil)"), NewFormat);
	} else {
		*(*Out)++ = '0';
		*(*Out)++ = 'x';
		WriteUnsignedNumber(Out, (u64) Value, 'X', NewFormat);
	}
}

internal b08
WriteSpecialtyFloat(
	c08 **Out,
	r64 Value,
	vstring_format Format)
{
	if(Format.CustomTypeSize && Format.TypeSize == 3)
		Assert(FALSE, "Long doubles not implemented!");
	
	vstring_format StringFormat = {0};
	StringFormat.MinChars = Format.MinChars;
	StringFormat.AlignLeft = Format.AlignLeft;
	
	if(Value == R64_INF) {
		WriteString(Out, Format.Caps ? CLStringL("INF") : CLStringL("inf"), StringFormat);
	} else if(Value == R64_NINF) {
		WriteString(Out, Format.Caps ? CLStringL("-INF") : CLStringL("-inf"), StringFormat);
	} else if(Value != Value) {
		u64 Binary = FORCE_CAST(u64, Value);
		u32 FracMax = (R64_MANTISSA_BITS+3)/4;
		u32 Offset = Format.Caps * 16;
		
		string String = Format.Caps ? CLStringL("-SNAN[0XFFFFFFFFFFFFF]") : CLStringL("-snan[0xfffffffffffff]");
		String.Text[1] = (Binary & R64_QUIET_MASK) ? (Format.Caps ? 'Q' : 'q') : (Format.Caps ? 'S' : 's');
		
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
		
		WriteString(Out, String, StringFormat);
	} else {
		return FALSE;
	}
	
	return TRUE;
}

internal void
WriteHexadecimalFloat(
	c08 **Out,
	r64 Value,
	vstring_format Format)
{
	if(WriteSpecialtyFloat(Out, Value, Format)) return;
	
	if(Format.CustomTypeSize && Format.TypeSize == 3)
		Assert(FALSE, "Long doubles not implemented!");
	
	u32 FracMax = (R64_MANTISSA_BITS+3)/4;
	u32 Offset = Format.Caps * 16;
	
	u64 Binary = FORCE_CAST(u64, Value);
	
	s32 Sign         = (Binary & R64_SIGN_MASK) >> R64_SIGN_SHIFT;
	s32 Exponent     = (s32)((Binary & R64_EXPONENT_MASK) >> R64_EXPONENT_SHIFT) - R64_EXPONENT_BIAS;
	u64 MantissaBits = Binary & R64_MANTISSA_MASK;
	
	b08 Negative = Sign;
	b08 HasPrefix = Negative || Format.PrefixSpace || Format.PrefixPlus;
	c08 Prefix = Negative ? '-' : Format.PrefixPlus ? '+' : ' ';
	
	b08 IsDenormal = (Exponent == -R64_EXPONENT_BIAS);
	if(IsDenormal) Exponent++;
	if(Value == 0) Exponent = 0;
	
	u32 Index;
	b08 ValidIndex = Intrin_BitScanForward64(&Index, MantissaBits);
	u32 FracLen = ValidIndex ? (R64_MANTISSA_BITS - Index) / 4 + 1 : 0;
	FracLen = MAX(Format.Precision, FracLen);
	
	b08 ExpIsNegative = Exponent < 0;
	Exponent *= 1 - 2*ExpIsNegative;
	b08 HasExpSign = ExpIsNegative || Format.PrefixPlus;
	b08 ExpHasComma = Format.HasSeparatorChar && (Exponent > 999);
	s32 ExpLen = 1 + ExpHasComma;
	s32 ExpCpy = Exponent;
	while(ExpCpy /= 10) ExpLen++;
	
	b08 HasDecimal = FracLen || Format.HashFlag;
	u32 TotalLen = HasPrefix + 2 + 1 + HasDecimal + FracLen + 1 + HasExpSign + ExpLen;
	
	s32 Padding = Format.MinChars - TotalLen;
	
	if(!Format.AlignLeft) WritePadding(Out, &Padding);
	
	if(HasPrefix) *(*Out)++ = Prefix;
	
	*(*Out)++ = '0';
	*(*Out)++ = (Format.Caps) ? 'X' : 'x';
	
	*(*Out)++ = IsDenormal ? '0' : '1';
	
	if(HasDecimal) *(*Out)++ = '.';
	
	for(u32 I = 0; I < FracLen; I++) {
		u32 Digit = (MantissaBits >> (4*(FracMax-I-1))) & 0xF;
		*(*Out)++ = BaseChars[Digit + Offset];
	}
	
	*(*Out)++ = (Format.Caps) ? 'P' : 'p';
	
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
	vstring_format Format)
{
	if(WriteSpecialtyFloat(Out, Value, Format)) return;
	
	if(Format.CustomTypeSize && Format.TypeSize == 3)
		Assert(FALSE, "Long doubles not implemented!");
	
	vptr Cursor = Stack_GetCursor();
	Stack_SetCursor(*Out);
	string String = R32_ToString((r32) Value, Format.CustomPrecision ? Format.Precision : 5);
	Mem_Cpy(*Out, String.Text, String.Length);
	*Out += String.Length;
	Stack_SetCursor(Cursor);
	
	// u64 Binary = FORCE_CAST(u64, Value);
	// 
	// b08 Sign     = (Binary & R64_SIGN_MASK) >> R64_SIGN_SHIFT;
	// s32 Exponent = (s32)((Binary & R64_EXPONENT_MASK) >> R64_EXPONENT_SHIFT) - R64_EXPONENT_BIAS;
	// u64 Mantissa = Binary & R64_MANTISSA_MASK;
	// 
	// b08 Denormal = FALSE;
	// if(Exponent == -R64_EXPONENT_BIAS) {
	// 	Exponent++;
	// 	Denormal = TRUE;
	// }
	// 
	// b08 Negative = Sign;
	// b08 HasPrefix = Negative || Format.PrefixSpace || Format.PrefixPlus;
	// c08 Prefix = Negative ? '-' : Format.PrefixPlus ? '+' : ' ';
	// 
	// vptr Cursor = Stack_GetCursor();
	// vptr TempCursor = (vptr)ALIGN_UP((u64)Out + Format.MinChars, 8);
	// Stack_SetCursor(TempCursor);
	// 
	// bigint Whole = BigInt_InitV(0);
	// s32 WholeLen = 1;
	// if(Exponent >= 0) {
	// 	Whole = BigInt_Init(1 + Exponent / 64);
	// 	Whole.WordCount = Whole.MaxCount;
	// 	
	// 	Whole.Words[Exponent / 64] |= !Denormal << (Exponent % 64);
	// 	for(s32 I = 0; I <= R64_MANTISSA_BITS; I++) {
	// 		if(Exponent + I < R64_MANTISSA_BITS) continue;
	// 		u64 Bit = Exponent - R64_MANTISSA_BITS + I;
	// 		u64 IsSet = (Mantissa >> I) & 1;
	// 		if(Whole.WordCount < Bit / 64) Whole.WordCount += IsSet;
	// 		Whole.Words[Bit / 64] |= IsSet << (Bit % 64);
	// 	}
	// 	
	// 	TempCursor = Stack_GetCursor();
	// 	
	// 	bigint_divmod DivMod = {Whole, BigInt_InitV(0)};
	// 	bigint Divisor = BigInt_InitV(10);
	// 	do {
	// 		DivMod = BigInt_DivMod(DivMod.Quotient, Divisor);
	// 	} while(DivMod.Quotient.WordCount && WholeLen++);
	// }
	// 
	// u32 TotalLen = HasPrefix;
	// 
	// s32 Padding = Format.MinChars - TotalLen;
	// 
	// if(!Format.AlignLeft) WritePadding(Out, &Padding);
	// 
	// Stack_SetCursor(TempCursor);
	// 
	// bigint_divmod DivMod = {Whole, BigInt_InitV(0)};
	// bigint Divisor = BigInt_InitV(10);
	// while(WholeLen--) {
	// 	DivMod = BigInt_DivMod(DivMod.Quotient, Divisor);
	// 	*(*Out)++ = '0' + (DivMod.Remainder.WordCount ? DivMod.Remainder.Words[0] : 0);
	// }
	// 
	// if(HasPrefix) *(*Out)++ = Prefix;
	// 
	// WritePadding(Out, &Padding);
	// 
	// Stack_SetCursor(Cursor);
}

// internal u32
// ReadNumber(
// 	c08 **In)
// {
// 	u32 Value = 0;
// 	
// 	while(**In >= '0' && **In <= '9')
// 		Value = Value * 10 + *(*In)++ - '0';
// 	
// 	return Value;
// }

// internal void
// ReadConversionIndex(
// 	c08 **In,
// 	format_flags *Format)
// {
// 	if(**In < '0' || **In > '9') return;
// 
// 	u32 Index = ReadNumber(In);
// 	
// 	if(**In == '$') {
// 		Format->CustomConversionIndex = TRUE;
// 		Format->ConversionIndex = Index;
// 		(*In)++;
// 	} else {
// 		Format->MinChars = Index;
// 	}
// }
// 
// internal void
// ReadActualFlags(
// 	c08 **In,
// 	format_flags *Format)
// {
// 	while(TRUE) {
// 		switch(**In) {
// 			case '-':  Format->AlignLeft        = TRUE; break;
// 			case '+':  Format->PrefixPlus       = TRUE; break;
// 			case ' ':  Format->PrefixSpace      = TRUE; break;
// 			case '#':  Format->HashFlag         = TRUE; break;
// 			case '\'': Format->HasSeparatorChar = TRUE; break;
// 			case '0':  Format->PadZero          = TRUE; break;
// 			default: return;
// 		}
// 		(*In)++;
// 	}
// }
// 
// internal s64
// ReadArgsFlag(
// 	c08 **In,
// 	format_flags *Format,
// 	va_list ArgsStart,
// 	va_list *Args)
// {
// 	if(**In == '*') {
// 		(*In)++;
// 		
// 		if(**In >= '0' && **In <= '9') {
// 			u32 Index = ReadNumber(In);
// 			Assert(**In == '$', "Invalid format!");
// 			(*In)++;
// 			
// 			SetVAArgsToIndex(ArgsStart, Args, Index - 1);
// 		}
// 		
// 		return VA_Next(*Args, s64);
// 	}
// 	
// 	return ReadNumber(In);
// }
// 
// internal void
// ReadMinWidth(
// 	c08** In,
// 	format_flags *Format,
// 	va_list ArgsStart,
// 	va_list *Args)
// {
// 	Format->MinChars = ReadArgsFlag(In, Format, ArgsStart, Args);
// 	
// 	if(Format->MinChars < 0) {
// 		Format->AlignLeft = !Format->AlignLeft;
// 		Format->MinChars = -Format->MinChars;
// 	}
// }
// 
// internal void
// ReadPrecision(
// 	c08 **In,
// 	format_flags *Format,
// 	va_list ArgsStart,
// 	va_list *Args)
// {
// 	if(**In != '.') return;
// 	(*In)++;
// 	
// 	Format->CustomPrecision = TRUE;
// 	Format->Precision = ReadArgsFlag(In, Format, ArgsStart, Args);
// 	
// 	if(Format->Precision < 0) Format->Precision = 0;
// }
// 
// internal void
// ReadTypeModifiers(
// 	c08 **In,
// 	format_flags *Format)
// {
// 	c08 *C = *In;
// 	Format->CustomTypeSize = TRUE;
// 	
// 	if(C[0] == 'h' && C[1] == 'h') {
// 		Format->TypeSize = 0;
// 		*In += 2;
// 	} else if(C[0] == 'l' && C[1] == 'l') {
// 		Format->TypeSize = 3;
// 		*In += 2;
// 	} else {
// 		if(C[0] == 'h')
// 			Format->TypeSize = 1;
// 		else if(C[0] == 'l')
// 			Format->TypeSize = 2;
// 		else if(C[0] == 'L' || C[0] == 'q' || *C == 'j' || *C == 'z' || *C == 'Z' || *C == 't')
// 			Format->TypeSize = 3;
// 		else {
// 			Format->CustomTypeSize = FALSE;
// 			return;
// 		}
// 		(*In)++;
// 	}
// }
// 
// internal void
// VString_ReadFormat(
// 	c08 **In,
// 	vstring_format *Args,
// 	u32 *ArgCount)
// {
// 	vstring_format Format = {0};
// 	
// 	ReadConversionIndex(In, &Format);
// 	ReadActualFlags(In, &Format);
// 	ReadMinWidth(In, &Format, ArgsStart, Args);
// 	ReadPrecision(In, &Format, ArgsStart, Args);
// 	ReadTypeModifiers(In, &Format);
// 	
// 	if (**In >= 'A' && **In <= 'Z')
// 		Format.Caps = TRUE;
// 	
// 	if(Format.CustomConversionIndex)
// 		SetVAArgsToIndex(ArgsStart, Args, Format.ConversionIndex - 1);
// 	
// 	return Format;
// }
// 
// internal void
// VString_WriteFormattedValue(
// 	c08 **In,
// 	c08 **Out,
// 	string Result,
// 	format_flags *Args,
// 	va_list ArgsStart,
// 	va_list *Args)
// {
// 	vstring_format Format = ReadFlags(In, ArgsStart, Args);
// 	
// 	switch(**In) {
// 		case 'T': WriteBoolean(Out, VA_Next(*Args, s64), Format); break;
// 		case 'c': WriteCharacter(Out, VA_Next(*Args, s64), Format); break;
// 		case 's': WriteString(Out, VA_Next(*Args, string), Format); break;
// 		case 'p': WritePointer(Out, VA_Next(*Args, vptr), Format); break;
// 		case 'd': case 'i': WriteSignedNumber(Out, VA_Next(*Args, s64), Format); break;
// 		case 'a': case 'A': WriteHexadecimalFloat(Out, VA_Next(*Args, r64), Format); break;
// 		case 'f': case 'F': WriteFloat(Out, VA_Next(*Args, r64), Format); break;
// 		case 'b': case 'B': case 'o': case 'u': case 'x': case 'X':
// 			WriteUnsignedNumber(Out, VA_Next(*Args, u64), **In, Format); break;
// 		
// 		case 'e':
// 		case 'E':
// 		case 'g':
// 		case 'G':
// 			Assert(FALSE, "Floats not implemented!");
// 			break;
// 		
// 		case 'n': {
// 			s64 Diff = (u64)*Out - (u64)Result.Text;
// 			if(Format.CustomTypeSize) {
// 				s32 *Value = VA_Next(*Args, s32*);
// 				*Value = Diff;
// 			} else if(Format.TypeSize == 0) {
// 				s08 *Value = VA_Next(*Args, s08*);
// 				*Value = Diff;
// 			} else if(Format.TypeSize == 1) {
// 				s16 *Value = VA_Next(*Args, s16*);
// 				*Value = Diff;
// 			} else if(Format.TypeSize == 2) {
// 				s32 *Value = VA_Next(*Args, s32*);
// 				*Value = Diff;
// 			} else if(Format.TypeSize == 3) {
// 				s64 *Value = VA_Next(*Args, s64*);
// 				*Value = Diff;
// 			}
// 		} break;
// 		
// 		default: Assert(FALSE, "Invalid format!");
// 	}
// 	
// 	(*In)++;
// }

/* TODO:
  - %f/%F, %e/%E, %g/%G, %m, %S, %C, %m
  - %lc, %ls, %La
  - wn, wfn
  - Custom MinChars index
*/
internal string
VString(
	string Format,
	va_list List)
{
	// First, read through and parse all format specifiers
	
	// va_arg *Args = (va_arg*) Stack_GetCursor();
	// u32 ArgCount = 0;
	// 
	// c08 *C = Format.Text;
	// while(C < Format.Text + Format.Length) {
	// 	C++;
	// 	if(*(C-1) == '%') {
	// 		if(*C != '%') VString_ReadFlags(&C, Args, &ArgCount);
	// 		C++;
	// 	}
	// }
	
	// Next, apply the formats 
	
	// c08 *Out = (c08*) (Args + ArgCount);
	// 
	// string Result;
	// Result.Resizable = FALSE;
	// Result.Text = Out;
	// 
	// c08 *C = Format.Text;
	// while(C < Format.Text + Format.Length) {
	// 	if(*C == '%') {
	// 		C++;
	// 		if(*C == '%') {
	// 			*Out++ = *C++;
	// 		} else {
	// 			WriteFormattedString(&C, &Out, Result, ArgsStart, &Args);
	// 		}
	// 	} else {
	// 		*Out++ = *C++;
	// 	}
	// }
	// 
	// Result.Length = (u64)Out - (u64)Result.Text;
	// Result.Capacity = Result.Length;
	// 
	// Stack_SetCursor(Out);
	
	// return Result;
	
	return Format;
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