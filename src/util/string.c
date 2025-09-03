/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
**                                                                         **
**  Author: Aria Seiler                                                    **
**                                                                         **
**  This program is in the public domain. There is no implied warranty,    **
**  so use it at your own risk.                                            **
**                                                                         **
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef INCLUDE_HEADER

#define EString() CStringL("")
#define CStringL(Literal) CLStringL(Literal, sizeof(Literal)-1)
#define CNStringL(Literal) CLStringL(Literal, sizeof(Literal))
#define CLStringL(Literal, Len) (string){ .Length = (Len), .Encoding = STRING_ENCODING_ASCII, { .Text = (Literal) } }
#define CFStringL(Literal, ...) FString(CStringL(Literal), __VA_ARGS__)
#define CFString(Format, ...) FString(CString(Format), __VA_ARGS__)

typedef enum string_encoding : u08 {
	STRING_ENCODING_ASCII = 0,
	STRING_ENCODING_UTF8  = 1,
	STRING_ENCODING_UTF16 = 2,
	STRING_ENCODING_UTF32 = 3,
} string_encoding;

typedef struct string {
	usize			Length;
	string_encoding Encoding;
	union {
		c08 *Text;
		c16 *Text16;
		c32 *Text32;
	};
} string;

#define STRING_FUNCS \
	EXPORT(string, CLString,         c08 *Chars, u64 Length) \
	EXPORT(string, CString,          c08 *Chars) \
	EXPORT(string, CNString,         c08 *Chars) \
	EXPORT(string, LString,          u64 Length) \
	EXPORT(string, FString,          string Format, ...) \
	EXPORT(string, SString,          string String) \
	\
	EXPORT(string, String_Cat,       string A, string B) \
	EXPORT(s08,    String_Cmp,       string A, string B) \
	EXPORT(s08,    String_CmpPtr,    string *A, string *B, vptr _) \
	EXPORT(usize,  String_Hash,      string S) \
	EXPORT(usize,  String_HashPtr,   string *S, vptr _) \
	\
	EXPORT(b08,    String_TryParseS32, string S, s32 *NumOut)

#endif

#ifdef INCLUDE_SOURCE

persist c08 *BaseChars = "0123456789abcdef0123456789ABCDEF";

/*
 * Construct an immutable string view with C-string backing
 */
internal string
CLString(c08 *Chars, u64 Length)
{
	string String;
	String.Length	= Length;
	String.Encoding = STRING_ENCODING_ASCII;
	String.Text		= Chars;
	return String;
}

/*
 * Construct an immutable string with C-string backing
 */
internal string
CString(c08 *Chars)
{
	return CLString(Chars, Mem_BytesUntil((u08 *) Chars, 0));
}

/*
 * Construct an immutable string with C-string backing, including the null terminator
 */
internal string
CNString(c08 *Chars)
{
	return CLString(Chars, Mem_BytesUntil((u08 *) Chars, 0) + 1);
}

/*
 * Construct an uninitialized string with stack backing with a given length
 */
internal string
LString(u64 Length)
{
	return CLString(Stack_Allocate(Length), Length);
}

/*
 * Construct a string with stack backing and copy the given string's contents
 */
internal string
SString(string String)
{
	string Result = LString(String.Length);
	Mem_Cpy(Result.Text, String.Text, String.Length);
	return Result;
}

internal string
String_Cat(string A, string B)
{
	string Result = LString(A.Length + B.Length);

	Mem_Cpy(Result.Text, A.Text, A.Length);
	Mem_Cpy(Result.Text + A.Length, B.Text, B.Length);

	return Result;
}

internal usize
String_Hash(string S)
{
	u32 Hash = 5381;
	for (usize I = 0; I < S.Length; I++) Hash = (Hash << 5) + Hash + S.Text[I];
	return Hash;
}

internal usize
String_HashPtr(string *S, vptr _)
{
	if (!S) return 5381;
	return String_Hash(*S);
}

internal s08
String_Cmp(string A, string B)
{
	usize L1 = A.Length, L2 = B.Length;
	usize L	  = L1 < L2 ? L1 : L2;
	s08	  Cmp = Mem_Cmp((u08 *) A.Text, (u08 *) B.Text, L);

	if (L1 == L2 || Cmp) return Cmp;
	if (L1 < L2) return -1;
	return 1;
}

internal s08
String_CmpPtr(string *A, string *B, vptr _)
{
	if (!A) {
		if (!B) return 0;
		return -1;
	} else if (!B) return 1;
	return String_Cmp(*A, *B);
}

internal b08
String_TryParseS32(string String, s32 *NumOut)
{
	if (!String.Text || !String.Length) return FALSE;

	s32 Total = 0;

	b08 Negative = String.Text[0] == '-';
	if (Negative && String.Length == 1) return FALSE;
	for (usize I = Negative; I < String.Length; I++)
		if (String.Text[I] < '0' || String.Text[I] > '9') return FALSE;

	if (Negative)
		for (usize I = Negative; I < String.Length; I++)
			Total = Total * 10 - (String.Text[I] - '0');
	else
		for (usize I = Negative; I < String.Length; I++)
			Total = Total * 10 + (String.Text[I] - '0');

	*NumOut = Total;
	return TRUE;
}

/// @brief Bumps the string by the provided amount, up to the end of the string. This includes
/// increasing the text pointer based on the encoding and reducing the length accordingly. Note that
/// this does not consider character sequences, so bumping a 4-byte UTF8 sequence by 2 will result
/// in an invalid sequence.
/// @param String The string to be bumped.
/// @param Amount The amount to bump the string by.
/// @return The amount the string was actually bumped by. Zero if the string is invalid.
internal usize
String_BumpUnits(string *String, usize Amount)
{
	if (!String || !String->Text) return 0;
	if (String->Length < Amount) Amount = String->Length;
	String->Length -= Amount;
	switch (String->Encoding) {
		case STRING_ENCODING_ASCII:
		case STRING_ENCODING_UTF8 : String->Text += Amount; break;
		case STRING_ENCODING_UTF16: String->Text16 += Amount; break;
		case STRING_ENCODING_UTF32: String->Text32 += Amount; break;
		default					  : Assert(FALSE, "Unknown encoding format"); return 0;
	}
	return Amount;
}

/// @brief Reads the next codepoint from a string, based on its encoding, and returns it. Then bumps
/// the string.
/// @param[in,out] String A pointer to the string being read. Will be bumped past the character
/// being read.
/// @return A unicode codepoint equivalent to the character that was read. If the codepoint was
/// invalid, such as being a surrogate half or malformed UTF-8, 0xFFFD is returned instead.
internal u32
String_NextCodepoint(string *String)
{
	if (!String || !String->Text || !String->Length) return 0;

	u32 B1, B2, B3, B4;
	u32 Codepoint = 0xFFFD;
	u32 Delta	  = 1;

	switch (String->Encoding) {
		case STRING_ENCODING_ASCII:
			B1		  = (u08) String->Text[0];
			Codepoint = B1 & 0x7F;
			break;

		case STRING_ENCODING_UTF8:
			B1 = (u08) String->Text[0];
			B2 = String->Length < 2 ? 0 : (u08) String->Text[1];
			B3 = String->Length < 3 ? 0 : (u08) String->Text[2];
			B4 = String->Length < 4 ? 0 : (u08) String->Text[3];
			switch (B1) {
				case 0x00 ... 0x7F: Codepoint = B1; break;

				case 0xC2 ... 0xDF:
					if (B2 >= 0x80 && B2 <= 0xBF) {
						Delta	  = 2;
						Codepoint = ((B1 & 0x1F) << 6) | ((B2 & 0x3F) << 0);
					}
					break;

				case 0xE0 ... 0xEF:
					if ((B2 >= 0x80 && B2 <= 0xBF)
						&& (B1 != 0xE0 || B2 >= 0xA0)
						&& (B1 != 0xED || B2 <= 0x9F))
					{
						if (B3 < 0x80 || B3 > 0xBF) {
							Delta = 2;
						} else {
							Delta = 3;
							Codepoint =
								((B1 & 0x0F) << 12) | ((B2 & 0x3F) << 6) | ((B3 & 0x3F) << 0);
						}
					}
					break;

				case 0xF0 ... 0xF4:
					if ((B2 >= 0x80 && B2 <= 0xBF)
						&& (B1 != 0xF0 || B2 >= 0x90)
						&& (B1 != 0xF4 || B2 <= 0x8F))
					{
						if (B3 < 0x80 || B3 > 0xBF) {
							Delta = 2;
						} else if (B4 < 0x80 || B4 > 0xBF) {
							Delta = 3;
						} else {
							Delta	  = 4;
							Codepoint = ((B1 & 0x07) << 18)
									  | ((B2 & 0x3F) << 12)
									  | ((B3 & 0x3F) << 6)
									  | ((B4 & 0x3F) << 0);
						}
					}
					break;
			}
			break;

		case STRING_ENCODING_UTF16:
			B1 = (u16) String->Text16[0];
			B2 = String->Length < 2 ? 0 : (u16) String->Text16[1];
			if (B1 >= 0xD800 && B1 <= 0xDFFF) {
				if (B1 <= 0xDBFF && B2 >= 0xDC00 && B2 <= 0xDFFF) {
					Codepoint = 0x10000 + ((B1 - 0xD800) << 10) + (B2 - 0xDC00);
					Delta	  = 2;
				}
			} else {
				Codepoint = B1;
			}
			break;

		case STRING_ENCODING_UTF32:
			B1 = (u32) String->Text32[0];
			if (B1 < 0x10FFFF && (B1 < 0xD800 || B1 > 0xDFFF)) Codepoint = B1;
			break;

		default: Assert(FALSE, "Unknown encoding format"); return 0;
	}

	String_BumpUnits(String, Delta);
	return Codepoint;
}

/// @brief Attempts to convert the provided codepoint into a character sequence based on the target
/// encoding and write it into the string.
/// @param[in,out] String The string to be written into. Must be large enough to contain the encoded
/// character sequence or the call will fail. For reference, ASCII and UTF32 are max 1 character,
/// UTF16 is 2, and UTF8 is 4. May be null.
/// @param[in] Codepoint The codepoint to encode into the string. If this is invalid, such as being
/// a surrogate half or greater than `0x10FFFF`, it will be converted into `0xFFFD` before being
/// encoded.
/// @return The number of characters that were written. If `String` is null, this is the number of
/// characters that would have been written.
internal usize
String_WriteCodepoint(string *String, u32 Codepoint)
{
	usize Length = String ? String->Length : 0;
	if (Codepoint > 0x10FFFF || (Codepoint >= 0xD800 && Codepoint <= 0xDFFF)) Codepoint = 0xFFFD;

	switch (String->Encoding) {
		case STRING_ENCODING_ASCII:
			if (Codepoint <= 0x7F) {
				if (Length >= 1) String->Text[0] = Codepoint;
				return 1;
			}
			return 0;

		case STRING_ENCODING_UTF8:
			if (Codepoint <= 0x7F) {
				if (Length >= 1) String->Text[0] = Codepoint;
				return 1;
			} else if (Codepoint <= 0x7FF) {
				if (Length >= 2) {
					String->Text[0] = 0xC0 | (Codepoint & 0x1F);
					String->Text[1] = 0x80 | ((Codepoint >> 6) & 0x3F);
				}
				return 2;
			} else if (Codepoint <= 0xFFFF) {
				if (Length >= 3) {
					String->Text[0] = 0xE0 | (Codepoint & 0x0F);
					String->Text[1] = 0x80 | ((Codepoint >> 6) & 0x3F);
					String->Text[2] = 0x80 | ((Codepoint >> 12) & 0x3F);
				}
				return 3;
			} else {
				if (Length >= 4) {
					String->Text[0] = 0xF0 | (Codepoint & 0x07);
					String->Text[1] = 0x80 | ((Codepoint >> 6) & 0x3F);
					String->Text[2] = 0x80 | ((Codepoint >> 12) & 0x3F);
					String->Text[3] = 0x80 | ((Codepoint >> 18) & 0x3F);
				}
				return 4;
			}

		case STRING_ENCODING_UTF16:
			if (Codepoint <= 0xFFFF) {
				if (Length >= 1) String->Text16[0] = Codepoint;
				return 1;
			} else {
				if (Length >= 2) {
					Codepoint		  -= 0x10000;
					String->Text16[0]  = 0xD800 | ((Codepoint >> 10) & 0x3FF);
					String->Text16[1]  = 0xDC00 | (Codepoint & 0x3FF);
				}
				return 2;
			}

		case STRING_ENCODING_UTF32:
			if (Length >= 1) String->Text32[0] = Codepoint;
			return 1;

		default: Assert(FALSE, "Unknown encoding format"); return 0;
	}
}

/**************************************************************************************************
 *   String Formatting
 *-------------------------------------------------------------------------------------------------
 * Handles parsing, writing, and printing formatted strings.
 * Additionally, includes functions for converting various data types to strings.
 *-------------------------------------------------------------------------------------------------
 */

/// @brief Status code for FString operations, such as parsing and indexing errors.
typedef enum fstring_format_status {
	/// @brief The format was valid and parsed successfully.
	FSTRING_FORMAT_VALID,

	/// @brief A number wasn't found and thus couldn't be parsed. This will most likely be caused by
	/// a period without an asterisk or precision number following it.
	FSTRING_FORMAT_INT_NOT_PRESENT,

	/// @brief A number in the format, such as width, precision, or an index, was too large to fit
	/// within a U32. This will most likely be caused by the format extending past where it was
	/// intended to end.
	FSTRING_FORMAT_INT_OVERFLOW,

	/// @brief A param index wasn't found when it was expected to be. This occurs if the index
	/// number was present but it wasn't followed by '$'. Additionally, it can occur if not all
	/// index specifiers are used, such as if you reference 1$ and 3$ but never 2$.
	FSTRING_FORMAT_INDEX_NOT_PRESENT,

	/// @brief The usage of param indexes was inconsistent within the format string. Indexes are
	/// all-or-nothing, so either all values, variable widths, and variable precisions use
	/// them, or none do.
	FSTRING_FORMAT_INDEX_INCONSISTENT,

	/// @brief A format type didn't match a valid pattern, such as 'hhx' or 'Lc'. See
	/// `FString_ParseFormatType` for the regex of accepted types
	FSTRING_FORMAT_TYPE_INVALID,

	/// @brief An index was referenced with different types. This can occur, for
	/// example, if you use a param as both a width and a non-S32 value.
	FSTRING_FORMAT_PARAM_REDEFINED,

	/// @brief The provided output buffer was too small to contain the fully formatted output.
	FSTRING_FORMAT_BUFFER_TOO_SMALL,

	/// @brief A provided string param's encoding didn't match its specifier.
	FSTRING_FORMAT_ENCODING_INVALID,

	/// @brief The requested feature isn't yet implemented. Sorry!
	FSTRING_FORMAT_NOT_IMPLEMENTED,
} fstring_format_status;

/// @brief String descriptions for each format status, used for logging errors.
global string FStringFormatStatusDescriptions[] = {
	[FSTRING_FORMAT_VALID]			   = CStringL("The format specifier was valid"),
	[FSTRING_FORMAT_INT_NOT_PRESENT]   = CStringL("Expected a number"),
	[FSTRING_FORMAT_INT_OVERFLOW]	   = CStringL("Number was too large"),
	[FSTRING_FORMAT_INDEX_NOT_PRESENT] = CStringL("Not all parameters were referenced"),
	[FSTRING_FORMAT_INDEX_INCONSISTENT] =
		CStringL("Format strings cannot contain both indexed and non-indexed parameters"),
	[FSTRING_FORMAT_TYPE_INVALID] = CStringL("Invalid format type"),
	[FSTRING_FORMAT_PARAM_REDEFINED] =
		CStringL("Format strings cannot specify an indexed paramter to be multiple types"),
	[FSTRING_FORMAT_BUFFER_TOO_SMALL] = CStringL("The provided output buffer was too small"),
	[FSTRING_FORMAT_ENCODING_INVALID] = CStringL("A provided string encoding"),
	[FSTRING_FORMAT_NOT_IMPLEMENTED]  = CStringL("The requested format isn't implemented yet"),
};

/// @brief Data type and flags of a format specifier. Values `FSTRING_FORMAT_SIZE_*` are used
/// internally for the type parser state machine; only `TYPE_*` and `FLAG_*` values are relevant
/// beyond that case.
typedef enum fstring_format_type {
	FSTRING_FORMAT_SIZE_INITIAL	  = 1,
	FSTRING_FORMAT_SIZE_HALFHALF  = 2,
	FSTRING_FORMAT_SIZE_HALF	  = 3,
	FSTRING_FORMAT_SIZE_LONG	  = 4,
	FSTRING_FORMAT_SIZE_LONGLONG  = 5,
	FSTRING_FORMAT_SIZE_EXTRALONG = 6,
	FSTRING_FORMAT_SIZE_SIZE	  = 7,
	FSTRING_FORMAT_SIZE_SPEC	  = 8,
	FSTRING_FORMAT_SIZE_FASTSPEC  = 9,
	FSTRING_FORMAT_SIZE_SPEC1	  = 10,
	FSTRING_FORMAT_SIZE_SPEC3	  = 11,
	FSTRING_FORMAT_SIZE_SPEC6	  = 12,
	FSTRING_FORMAT_SIZE_SPEC8	  = 13,
	FSTRING_FORMAT_SIZE_SPEC16	  = 14,
	FSTRING_FORMAT_SIZE_SPEC32	  = 15,
	FSTRING_FORMAT_SIZE_SPEC64	  = 16,

	/// @brief An easy barrier to check whether parsing is complete. All valid types are numbered
	/// above this, and all intermediate types are below.
	FSTRING_FORMAT_SIZE_DONE = 17,

	FSTRING_FORMAT_TYPE_S08		= 32,
	FSTRING_FORMAT_TYPE_U08		= 33,
	FSTRING_FORMAT_TYPE_S16		= 34,
	FSTRING_FORMAT_TYPE_U16		= 35,
	FSTRING_FORMAT_TYPE_S32		= 36,
	FSTRING_FORMAT_TYPE_U32		= 37,
	FSTRING_FORMAT_TYPE_S64		= 38,
	FSTRING_FORMAT_TYPE_U64		= 39,
	FSTRING_FORMAT_TYPE_SSIZE	= 40,
	FSTRING_FORMAT_TYPE_USIZE	= 41,
	FSTRING_FORMAT_TYPE_R64		= 42,
	FSTRING_FORMAT_TYPE_CHR		= 43,
	FSTRING_FORMAT_TYPE_STR		= 44,
	FSTRING_FORMAT_TYPE_PTR		= 45,
	FSTRING_FORMAT_TYPE_B08		= 46,
	FSTRING_FORMAT_TYPE_QUERY16 = 47,
	FSTRING_FORMAT_TYPE_QUERY32 = 48,
	FSTRING_FORMAT_TYPE_QUERY64 = 49,

	FSTRING_FORMAT_TYPE_MASK = 0x3F,

	FSTRING_FORMAT_FLAG_PARAM_WIDTH		= 0x40,
	FSTRING_FORMAT_FLAG_PARAM_PRECISION = 0x80,

	FSTRING_FORMAT_FLAG_INT_DEC = 0x0,
	FSTRING_FORMAT_FLAG_INT_BIN = 0x100,
	FSTRING_FORMAT_FLAG_INT_OCT = 0x200,
	FSTRING_FORMAT_FLAG_INT_HEX = 0x400,

	FSTRING_FORMAT_FLAG_FLOAT_STD = 0x0,
	FSTRING_FORMAT_FLAG_FLOAT_EXP = 0x100,
	FSTRING_FORMAT_FLAG_FLOAT_FIT = 0x200,
	FSTRING_FORMAT_FLAG_FLOAT_HEX = 0x400,

	FSTRING_FORMAT_FLAG_STR_BYTE = 0x0,
	FSTRING_FORMAT_FLAG_STR_WIDE = 0x100,

	FSTRING_FORMAT_FLAG_CHR_BYTE = 0x0,
	FSTRING_FORMAT_FLAG_CHR_WIDE = 0x100,

	FSTRING_FORMAT_FLAG_UPPERCASE		= 0x000800,
	FSTRING_FORMAT_FLAG_LEFT_JUSTIFY	= 0x001000,
	FSTRING_FORMAT_FLAG_PAD_WITH_ZERO	= 0x002000,
	FSTRING_FORMAT_FLAG_SEPARATE_GROUPS = 0x004000,
	FSTRING_FORMAT_FLAG_SPECIFY_RADIX	= 0x008000,
	FSTRING_FORMAT_FLAG_PREFIX_SIGN		= 0x010000,
	FSTRING_FORMAT_FLAG_PREFIX_SPACE	= 0x020000,
} fstring_format_type;

#define S(C, TYPE) [C - '1'] = FSTRING_FORMAT_##TYPE
/// @brief State machine for format type parsing. Each character is mapped into a lookup table in
/// the range '1'..'z'. Since this is a static table, it's initialized to zero, so any invalid
/// transition results in the invalid type 0. At most 5 iterations are required to completely parse
/// a type, such as `wf64`, but in practice most will only require 1-3.
global fstring_format_type FStringFormatTypeStateMachine[]['z' - '1' + 1] = {
	// clang-format off
	[FSTRING_FORMAT_SIZE_INITIAL] = {
		S('h', SIZE_HALF),
		S('l', SIZE_LONG),
		S('q', SIZE_LONGLONG),
		S('L', SIZE_EXTRALONG),
		S('t', SIZE_SIZE),
		S('j', SIZE_SIZE),
		S('z', SIZE_SIZE),
		S('Z', SIZE_SIZE),
		S('w', SIZE_SPEC),
		S('d', TYPE_S32 | FSTRING_FORMAT_FLAG_INT_DEC),
		S('i', TYPE_S32 | FSTRING_FORMAT_FLAG_INT_DEC),
		S('b', TYPE_U32 | FSTRING_FORMAT_FLAG_INT_BIN),
		S('B', TYPE_U32 | FSTRING_FORMAT_FLAG_INT_BIN | FSTRING_FORMAT_FLAG_UPPERCASE),
		S('o', TYPE_U32 | FSTRING_FORMAT_FLAG_INT_OCT),
		S('u', TYPE_U32 | FSTRING_FORMAT_FLAG_INT_DEC),
		S('x', TYPE_U32 | FSTRING_FORMAT_FLAG_INT_HEX),
		S('X', TYPE_U32 | FSTRING_FORMAT_FLAG_INT_HEX | FSTRING_FORMAT_FLAG_UPPERCASE),
		S('f', TYPE_R64 | FSTRING_FORMAT_FLAG_FLOAT_STD),
		S('F', TYPE_R64 | FSTRING_FORMAT_FLAG_FLOAT_STD | FSTRING_FORMAT_FLAG_UPPERCASE),
		S('e', TYPE_R64 | FSTRING_FORMAT_FLAG_FLOAT_EXP),
		S('E', TYPE_R64 | FSTRING_FORMAT_FLAG_FLOAT_EXP | FSTRING_FORMAT_FLAG_UPPERCASE),
		S('g', TYPE_R64 | FSTRING_FORMAT_FLAG_FLOAT_FIT),
		S('G', TYPE_R64 | FSTRING_FORMAT_FLAG_FLOAT_FIT | FSTRING_FORMAT_FLAG_UPPERCASE),
		S('a', TYPE_R64 | FSTRING_FORMAT_FLAG_FLOAT_HEX),
		S('A', TYPE_R64 | FSTRING_FORMAT_FLAG_FLOAT_HEX | FSTRING_FORMAT_FLAG_UPPERCASE),
		S('c', TYPE_CHR | FSTRING_FORMAT_FLAG_CHR_BYTE),
		S('C', TYPE_CHR | FSTRING_FORMAT_FLAG_CHR_WIDE),
		S('s', TYPE_STR | FSTRING_FORMAT_FLAG_STR_BYTE),
		S('S', TYPE_STR | FSTRING_FORMAT_FLAG_STR_WIDE),
		S('p', TYPE_PTR),
		S('T', TYPE_B08),
		S('n', TYPE_QUERY32),
		S('m', TYPE_S32 | FSTRING_FORMAT_FLAG_INT_DEC),
	},
	[FSTRING_FORMAT_SIZE_HALFHALF] = {
		S('d', TYPE_S08 | FSTRING_FORMAT_FLAG_INT_DEC),
		S('i', TYPE_S08 | FSTRING_FORMAT_FLAG_INT_DEC),
		S('b', TYPE_U08 | FSTRING_FORMAT_FLAG_INT_BIN),
		S('B', TYPE_U08 | FSTRING_FORMAT_FLAG_INT_BIN | FSTRING_FORMAT_FLAG_UPPERCASE),
		S('o', TYPE_U08 | FSTRING_FORMAT_FLAG_INT_OCT),
		S('u', TYPE_U08 | FSTRING_FORMAT_FLAG_INT_DEC),
		S('x', TYPE_U08 | FSTRING_FORMAT_FLAG_INT_HEX),
		S('X', TYPE_U08 | FSTRING_FORMAT_FLAG_INT_HEX | FSTRING_FORMAT_FLAG_UPPERCASE),
	},
	[FSTRING_FORMAT_SIZE_HALF] = {
		S('h', SIZE_HALFHALF),
		S('d', TYPE_S16 | FSTRING_FORMAT_FLAG_INT_DEC),
		S('i', TYPE_S16 | FSTRING_FORMAT_FLAG_INT_DEC),
		S('b', TYPE_U16 | FSTRING_FORMAT_FLAG_INT_BIN),
		S('B', TYPE_U16 | FSTRING_FORMAT_FLAG_INT_BIN | FSTRING_FORMAT_FLAG_UPPERCASE),
		S('o', TYPE_U16 | FSTRING_FORMAT_FLAG_INT_OCT),
		S('u', TYPE_U16 | FSTRING_FORMAT_FLAG_INT_DEC),
		S('x', TYPE_U16 | FSTRING_FORMAT_FLAG_INT_HEX),
		S('X', TYPE_U16 | FSTRING_FORMAT_FLAG_INT_HEX | FSTRING_FORMAT_FLAG_UPPERCASE),
		S('n', TYPE_QUERY16),
	},
	[FSTRING_FORMAT_SIZE_LONG] = {
		S('l', SIZE_LONGLONG),
		S('d', TYPE_S64 | FSTRING_FORMAT_FLAG_INT_DEC),
		S('i', TYPE_S64 | FSTRING_FORMAT_FLAG_INT_DEC),
		S('b', TYPE_U64 | FSTRING_FORMAT_FLAG_INT_BIN),
		S('B', TYPE_U64 | FSTRING_FORMAT_FLAG_INT_BIN | FSTRING_FORMAT_FLAG_UPPERCASE),
		S('o', TYPE_U64 | FSTRING_FORMAT_FLAG_INT_OCT),
		S('u', TYPE_U64 | FSTRING_FORMAT_FLAG_INT_DEC),
		S('x', TYPE_U64 | FSTRING_FORMAT_FLAG_INT_HEX),
		S('X', TYPE_U64 | FSTRING_FORMAT_FLAG_INT_HEX | FSTRING_FORMAT_FLAG_UPPERCASE),
		S('c', TYPE_CHR | FSTRING_FORMAT_FLAG_CHR_WIDE),
		S('s', TYPE_STR | FSTRING_FORMAT_FLAG_STR_WIDE),
		S('n', TYPE_QUERY64),
	},
	[FSTRING_FORMAT_SIZE_LONGLONG] = {
		S('d', TYPE_S64 | FSTRING_FORMAT_FLAG_INT_DEC),
		S('i', TYPE_S64 | FSTRING_FORMAT_FLAG_INT_DEC),
		S('b', TYPE_U64 | FSTRING_FORMAT_FLAG_INT_BIN),
		S('B', TYPE_U64 | FSTRING_FORMAT_FLAG_INT_BIN | FSTRING_FORMAT_FLAG_UPPERCASE),
		S('o', TYPE_U64 | FSTRING_FORMAT_FLAG_INT_OCT),
		S('u', TYPE_U64 | FSTRING_FORMAT_FLAG_INT_DEC),
		S('x', TYPE_U64 | FSTRING_FORMAT_FLAG_INT_HEX),
		S('X', TYPE_U64 | FSTRING_FORMAT_FLAG_INT_HEX | FSTRING_FORMAT_FLAG_UPPERCASE),
	},
	[FSTRING_FORMAT_SIZE_EXTRALONG] = {
		S('d', TYPE_S64 | FSTRING_FORMAT_FLAG_INT_DEC),
		S('i', TYPE_S64 | FSTRING_FORMAT_FLAG_INT_DEC),
		S('b', TYPE_U64 | FSTRING_FORMAT_FLAG_INT_BIN),
		S('B', TYPE_U64 | FSTRING_FORMAT_FLAG_INT_BIN | FSTRING_FORMAT_FLAG_UPPERCASE),
		S('o', TYPE_U64 | FSTRING_FORMAT_FLAG_INT_OCT),
		S('u', TYPE_U64 | FSTRING_FORMAT_FLAG_INT_DEC),
		S('x', TYPE_U64 | FSTRING_FORMAT_FLAG_INT_HEX),
		S('X', TYPE_U64 | FSTRING_FORMAT_FLAG_INT_HEX | FSTRING_FORMAT_FLAG_UPPERCASE),
		S('f', TYPE_R64 | FSTRING_FORMAT_FLAG_FLOAT_STD),
		S('F', TYPE_R64 | FSTRING_FORMAT_FLAG_FLOAT_STD | FSTRING_FORMAT_FLAG_UPPERCASE),
		S('e', TYPE_R64 | FSTRING_FORMAT_FLAG_FLOAT_EXP),
		S('E', TYPE_R64 | FSTRING_FORMAT_FLAG_FLOAT_EXP | FSTRING_FORMAT_FLAG_UPPERCASE),
		S('g', TYPE_R64 | FSTRING_FORMAT_FLAG_FLOAT_FIT),
		S('G', TYPE_R64 | FSTRING_FORMAT_FLAG_FLOAT_FIT | FSTRING_FORMAT_FLAG_UPPERCASE),
		S('a', TYPE_R64 | FSTRING_FORMAT_FLAG_FLOAT_HEX),
		S('A', TYPE_R64 | FSTRING_FORMAT_FLAG_FLOAT_HEX | FSTRING_FORMAT_FLAG_UPPERCASE),
	},
	[FSTRING_FORMAT_SIZE_SIZE] = {
		S('d', TYPE_SSIZE | FSTRING_FORMAT_FLAG_INT_DEC),
		S('i', TYPE_SSIZE | FSTRING_FORMAT_FLAG_INT_DEC),
		S('b', TYPE_USIZE | FSTRING_FORMAT_FLAG_INT_BIN),
		S('B', TYPE_USIZE | FSTRING_FORMAT_FLAG_INT_BIN | FSTRING_FORMAT_FLAG_UPPERCASE),
		S('o', TYPE_USIZE | FSTRING_FORMAT_FLAG_INT_OCT),
		S('u', TYPE_USIZE | FSTRING_FORMAT_FLAG_INT_DEC),
		S('x', TYPE_USIZE | FSTRING_FORMAT_FLAG_INT_HEX),
		S('X', TYPE_USIZE | FSTRING_FORMAT_FLAG_INT_HEX | FSTRING_FORMAT_FLAG_UPPERCASE),
	},
	[FSTRING_FORMAT_SIZE_SPEC] = {
		S('f', SIZE_FASTSPEC),
		S('8', SIZE_SPEC8),
		S('1', SIZE_SPEC1),
		S('3', SIZE_SPEC3),
		S('6', SIZE_SPEC6),
	},
	[FSTRING_FORMAT_SIZE_FASTSPEC] = {
		S('8', SIZE_SPEC8),
		S('1', SIZE_SPEC1),
		S('3', SIZE_SPEC3),
		S('6', SIZE_SPEC6),
	},
	[FSTRING_FORMAT_SIZE_SPEC1] = {
		S('6', SIZE_SPEC16),
	},
	[FSTRING_FORMAT_SIZE_SPEC3] = {
		S('2', SIZE_SPEC32),
	},
	[FSTRING_FORMAT_SIZE_SPEC6] = {
		S('4', SIZE_SPEC64),
	},
	[FSTRING_FORMAT_SIZE_SPEC8] = {
		S('d', TYPE_S08 | FSTRING_FORMAT_FLAG_INT_DEC),
		S('i', TYPE_S08 | FSTRING_FORMAT_FLAG_INT_DEC),
		S('b', TYPE_U08 | FSTRING_FORMAT_FLAG_INT_BIN),
		S('B', TYPE_U08 | FSTRING_FORMAT_FLAG_INT_BIN | FSTRING_FORMAT_FLAG_UPPERCASE),
		S('o', TYPE_U08 | FSTRING_FORMAT_FLAG_INT_OCT),
		S('u', TYPE_U08 | FSTRING_FORMAT_FLAG_INT_DEC),
		S('x', TYPE_U08 | FSTRING_FORMAT_FLAG_INT_HEX),
		S('X', TYPE_U08 | FSTRING_FORMAT_FLAG_INT_HEX | FSTRING_FORMAT_FLAG_UPPERCASE),
	},
	[FSTRING_FORMAT_SIZE_SPEC16] = {
		S('d', TYPE_S16 | FSTRING_FORMAT_FLAG_INT_DEC),
		S('i', TYPE_S16 | FSTRING_FORMAT_FLAG_INT_DEC),
		S('b', TYPE_U16 | FSTRING_FORMAT_FLAG_INT_BIN),
		S('B', TYPE_U16 | FSTRING_FORMAT_FLAG_INT_BIN | FSTRING_FORMAT_FLAG_UPPERCASE),
		S('o', TYPE_U16 | FSTRING_FORMAT_FLAG_INT_OCT),
		S('u', TYPE_U16 | FSTRING_FORMAT_FLAG_INT_DEC),
		S('x', TYPE_U16 | FSTRING_FORMAT_FLAG_INT_HEX),
		S('X', TYPE_U16 | FSTRING_FORMAT_FLAG_INT_HEX | FSTRING_FORMAT_FLAG_UPPERCASE),
	},
	[FSTRING_FORMAT_SIZE_SPEC32] = {
		S('d', TYPE_S32 | FSTRING_FORMAT_FLAG_INT_DEC),
		S('i', TYPE_S32 | FSTRING_FORMAT_FLAG_INT_DEC),
		S('b', TYPE_U32 | FSTRING_FORMAT_FLAG_INT_BIN),
		S('B', TYPE_U32 | FSTRING_FORMAT_FLAG_INT_BIN | FSTRING_FORMAT_FLAG_UPPERCASE),
		S('o', TYPE_U32 | FSTRING_FORMAT_FLAG_INT_OCT),
		S('u', TYPE_U32 | FSTRING_FORMAT_FLAG_INT_DEC),
		S('x', TYPE_U32 | FSTRING_FORMAT_FLAG_INT_HEX),
		S('X', TYPE_U32 | FSTRING_FORMAT_FLAG_INT_HEX | FSTRING_FORMAT_FLAG_UPPERCASE),
	},
	[FSTRING_FORMAT_SIZE_SPEC64] = {
		S('d', TYPE_S64 | FSTRING_FORMAT_FLAG_INT_DEC),
		S('i', TYPE_S64 | FSTRING_FORMAT_FLAG_INT_DEC),
		S('b', TYPE_U64 | FSTRING_FORMAT_FLAG_INT_BIN),
		S('B', TYPE_U64 | FSTRING_FORMAT_FLAG_INT_BIN | FSTRING_FORMAT_FLAG_UPPERCASE),
		S('o', TYPE_U64 | FSTRING_FORMAT_FLAG_INT_OCT),
		S('u', TYPE_U64 | FSTRING_FORMAT_FLAG_INT_DEC),
		S('x', TYPE_U64 | FSTRING_FORMAT_FLAG_INT_HEX),
		S('X', TYPE_U64 | FSTRING_FORMAT_FLAG_INT_HEX | FSTRING_FORMAT_FLAG_UPPERCASE),
	},
	// clang-format on
};
#undef S

/// @brief Union representing a parameter passed into `FString`. Before the variadic args are
/// interpreted, this contains the expected type of the param. Afterward, it contains the param's
/// value.
typedef union fstring_param {
	fstring_format_type Type;

	b08		Bool;
	c32		Char;
	ssize	Signed;
	usize	Unsigned;
	r64		Float;
	vptr	Pointer;
	string *String;
} fstring_param;

/// @brief Structure containing the complete definition of a format specifier: type, flags, width,
/// precision, and optionally indexes.
typedef struct fstring_format {
	fstring_format_type Type;

	union {
		fstring_param Value;
		s32			  ValueIndex;
	};

	union {
		s32 Width;
		s32 WidthIndex;
	};

	union {
		s32 Precision;
		s32 PrecisionIndex;
	};

	string SpecifierString;
	usize  ContentLength;
	usize  ActualWidth;
} fstring_format;

/// @brief Structure containing all formats found within a format string, as well as an ordered list
/// of parameters and, if necessary, their backing data. Most fields are internal and should be
/// ignored, but `FormatString` and `TotalTextSize` can be read for error handling and buffer
/// allocation respectively.
typedef struct fstring_format_list {
	string FormatString;

	usize			FormatCount;
	usize			ParamCount;
	fstring_format *Formats;

	usize ExtraDataSize;
	vptr  ExtraData;

	/// @brief This field contains the total number of plain characters to be written, ignoring any
	/// format specifiers. Note that '%%' is treated as one character.
	usize NonFormattedTextSize;

	/// @brief This field contains the total size of the formatted string being written.
	usize TotalTextSize;
} fstring_format_list;

/// @brief Get the next character from a string cursor or return 0.
/// @param FormatCursor The string cursor to peek from.
/// @return The character the cursor is pointing at, or 0 if FormatCursor has no next character.
internal c08
FString_PeekCursor(string *FormatCursor)
{
	if (!FormatCursor || !FormatCursor->Text || !FormatCursor->Length) return 0;
	return FormatCursor->Text[0];
}

/// @brief Bump the cursor to the next character if it has one.
/// @param FormatCursor The cursor to be bumped.
internal void
FString_BumpCursor(string *FormatCursor)
{
	if (!FormatCursor || !FormatCursor->Text || !FormatCursor->Length) return;
	FormatCursor->Text++;
	FormatCursor->Length--;
}

/// @brief Parse an unsigned decimal int from a format specifier, such as a width, precision, or
/// param index, and report whether it was valid, invalid, or overflowed.
///
/// Regex: `[1-9][0-9]*`
/// @param[in,out] FormatCursor A cursor potentially pointing to the beginning of a number in the
/// format specifier. After return, this will point to the first character after the number or the
/// first invalid character encountered.
/// @param[out] IntOut A pointer to the resulting parsed int. Will only be modified on success. May
/// be null.
/// @return
/// - `FSTRING_FORMAT_VALID`: The number was parsed successfully.
///
/// - `FSTRING_FORMAT_INT_OVERFLOW`: The parsed number exceeds an s32.
///
/// - `FSTRING_FORMAT_INT_NOT_PRESENT`: No number was found, or the number started with 0.
internal fstring_format_status
FString_ParseFormatInt(string *FormatCursor, s32 *IntOut)
{
	// If the first character isn't a nonzero digit, fail.
	u08 Digit = FString_PeekCursor(FormatCursor) - '1';
	if (Digit >= 9) return FSTRING_FORMAT_INT_NOT_PRESENT;

	// Otherwise, iterate until we hit a non-digit.
	s32 Int = 0;
	while ((Digit = FString_PeekCursor(FormatCursor) - '0') < 10) {
		if (Int > (0x7FFFFFFF - Digit) / 10) return FSTRING_FORMAT_INT_OVERFLOW;
		Int = Int * 10 + Digit;
		FString_BumpCursor(FormatCursor);
	}

	if (IntOut) *IntOut = Int;
	return FSTRING_FORMAT_VALID;
}

/// @brief Extract a param index from a format specifier, and handle index usage consistency
/// accordingly. Note that param indexes cannot be 0.
///
/// Regex: `[1-9][0-9]*\\$`
/// @param[in,out] FormatCursor A cursor potentially pointing to the beginning of a parameter index
/// in the format specifier. After return, this will point to the first character after the index on
/// success, the first character after the number on `FSTRING_FORMAT_INDEX_INVALID`, or the first
/// invalid character encountered otherwise.
/// @param[out] IndexOut A pointer to the resulting parsed index. Will be modified as long as a
/// valid number is parsed, regardless of whether the index is valid. May be null.
/// @param[in,out] UseIndexes A pointer to a boolean dictating whether or not to use indexes. If
/// `SetIndexUsage` is true, this will be updated with whether or not the index is valid, and
/// parsing will succeed regardless of whether a valid index was parsed. Otherwise, success is
/// determined by whether index usage matches the presence of an index. Cannot be null.
/// @param[in] SetIndexUsage A boolean indicating whether to modify or respect the value in
/// `UseIndexes`.
/// @return
/// - `FSTRING_FORMAT_VALID`: The index was successfully parsed.
///
/// - `FSTRING_FORMAT_INDEX_INCONSISTENT`: The index's presence doesn't match the value in
/// `UseIndexes` and `SetIndexUsage` is true.
///
/// - `FSTRING_FORMAT_INDEX_NOT_PRESENT`: A number was found, but it wasn't an index.
///
/// - `FSTRING_FORMAT_INT_OVERFLOW`: A number was found, but it was too large.
internal fstring_format_status
FString_ParseFormatIndex(string *FormatCursor, s32 *IndexOut, b08 *UseIndexes, b08 SetIndexUsage)
{
	Assert(UseIndexes);

	s32 Index;

	fstring_format_status Status = FString_ParseFormatInt(FormatCursor, &Index);

	// Propagate the overflow error
	if (Status == FSTRING_FORMAT_INT_OVERFLOW) return Status;

	// There's no int, so we'll either report that we're not using indexes, or fail.
	if (Status == FSTRING_FORMAT_INT_NOT_PRESENT) {
		if (SetIndexUsage) *UseIndexes = FALSE;
		else if (*UseIndexes) return FSTRING_FORMAT_INDEX_INCONSISTENT;
		return FSTRING_FORMAT_VALID;
	}

	// Update IndexOut since the number is valid
	if (IndexOut) *IndexOut = Index;

	// This is an index, so we'll either report that we're using indexes or fail.
	if (FString_PeekCursor(FormatCursor) == '$') {
		if (SetIndexUsage) *UseIndexes = TRUE;
		else if (!*UseIndexes) return FSTRING_FORMAT_INDEX_INCONSISTENT;
		FString_BumpCursor(FormatCursor);
		return FSTRING_FORMAT_VALID;
	}

	// This is a number, but not an index, so it's invalid.
	return FSTRING_FORMAT_INDEX_NOT_PRESENT;
}

/// @brief Extract flags from a format specifier, such as alignment, signage, and notation. Flags
/// may be repeated, though you should try not to, and duplicates will be ignored.
///
/// Regex: `[\\-+ #'0]*`
/// @param[in,out] FormatCursor A cursor potentially pointing to the beginning of the list of flags
/// in a format specifier. After return, this will point to the first character after the flags or
/// the first invalid character encountered.
/// @param[out] FlagsOut A pointer to an enum representing the specifier's flags. Only the flag bits
/// will be modified, so it can point to an existing specifier type for efficient storage. Cannot be
/// null.
/// @return `FSTRING_FORMAT_VALID`.
internal fstring_format_status
FString_ParseFormatFlags(string *FormatCursor, fstring_format_type *FlagsOut)
{
	// Run through and see if we find any flags. We'll accept duplicates to be nice.
	while (1) {
		switch (FString_PeekCursor(FormatCursor)) {
			case '-' : *FlagsOut |= FSTRING_FORMAT_FLAG_LEFT_JUSTIFY; break;
			case '+' : *FlagsOut |= FSTRING_FORMAT_FLAG_PREFIX_SIGN; break;
			case ' ' : *FlagsOut |= FSTRING_FORMAT_FLAG_PREFIX_SPACE; break;
			case '#' : *FlagsOut |= FSTRING_FORMAT_FLAG_SPECIFY_RADIX; break;
			case '\'': *FlagsOut |= FSTRING_FORMAT_FLAG_SEPARATE_GROUPS; break;
			case '0' : *FlagsOut |= FSTRING_FORMAT_FLAG_PAD_WITH_ZERO; break;

			// If we encounter anything else, we're done parsing the flags.
			default	 : return FSTRING_FORMAT_VALID;
		}

		FString_BumpCursor(FormatCursor);
	}
}

/// @brief Traverse the state machine to extract the format specifier's type and size.
///
/// Regex:
/// `(?:hh|ll|[hlLqtjzZ]|wf?(?:8|16|32|64))?[dibBouxX]|L?[fFeEgGaA]|l?[cs]|[hl]?n|[CSpTm]`
/// @param [in,out] FormatCursor A cursor potentially pointing to the beginning of the type portion
/// of the format specifier, such as "lld" or "hX". After return, this will point to the first
/// character after the specifier or the first invalid character encountered.
/// @param [out] TypeOut A pointer to the resulting format type. Will only be modified on success,
/// and only the bits within `FSTRING_FORMAT_TYPE_MASK` will be modified. May be null.
/// @return
/// - `FSTRING_FORMAT_VALID`: The format type was parsed successfully.
///
/// - `FSTRING_FORMAT_TYPE_INVALID`: The string doesn't match any type specifier.
internal fstring_format_status
FString_ParseFormatType(string *FormatCursor, fstring_format_type *TypeOut)
{
	fstring_format_type State = FSTRING_FORMAT_SIZE_INITIAL;
	while (State < FSTRING_FORMAT_SIZE_DONE) {
		u08 Transition = FString_PeekCursor(FormatCursor) - '1';

		// Get the next state from the transition table. All invalid transitions return 0.
		State = Transition < 74 ? FStringFormatTypeStateMachine[State][Transition] : 0;
		if (!State) return FSTRING_FORMAT_TYPE_INVALID;

		FString_BumpCursor(FormatCursor);
	}

	if (TypeOut) *TypeOut = (*TypeOut & ~FSTRING_FORMAT_TYPE_MASK) | State;
	return FSTRING_FORMAT_VALID;
}

/// @brief Parse a format specifier into a data structure, and report whether it is valid or not.
/// @param[in,out] FormatCursor A cursor pointing to the beginning of a format specifier. After
/// return, this will point to the first character after the specifier or the first invalid
/// character encountered.
/// @param[out] FormatOut A pointer to a format structure representing the specifier's type, flags,
/// width, precision, and potentially param indexes. May be null.
/// @param[in,out] UseIndexes A pointer to a boolean dictating whether or not to use indexes. If
/// `SetIndexUsage` is true, this will be updated with whether or not indexes are used in this
/// specifier. Otherwise, parsing will fail if index usage does not match the value in `UseIndexes`.
/// Cannot be null.
/// @param[in] SetIndexUsage A boolean indicating whether to modify or respect the value in
/// `UseIndexes`.
/// @return
/// - `FSTRING_FORMAT_VALID`: The format was successfully parsed.
///
/// - `FSTRING_FORMAT_INT_OVERFLOW`: A width, precision, or param index was too large.
///
/// - `FSTRING_FORMAT_INDEX_INCONSISTENT`: Only if `SetIndexUsage` is false: `UseIndexes` was true
/// and a param's index was missing, or `UseIndexes` was false and a param index was found.
///
/// - `FSTRING_FORMAT_TYPE_INVALID`: The format's size and type had an invalid pattern.
internal fstring_format_status
FString_ParseFormat(
	string		   *FormatCursor,
	fstring_format *FormatOut,
	b08			   *UseIndexes,
	b08				SetIndexUsage
)
{
	fstring_format_status Status;

	fstring_format Format = { 0 };
	Format.Precision	  = -1;	 // Default

	// The specifier starts at the '%' right before.
	Format.SpecifierString.Text = FormatCursor->Text - 1;

	// First try to parse the first index
	Status = FString_ParseFormatIndex(FormatCursor, &Format.ValueIndex, UseIndexes, SetIndexUsage);
	if (Status == FSTRING_FORMAT_INDEX_NOT_PRESENT) {
		// This can still be valid if the number is actually a width.
		Format.Width = Format.ValueIndex;
		goto precision;
	}
	if (Status != FSTRING_FORMAT_VALID) return Status;

	// Now for the flags
	Status = FString_ParseFormatFlags(FormatCursor, &Format.Type);
	if (Status != FSTRING_FORMAT_VALID) return Status;

	// Now for width. First, we check if it's provided by a param.
	if (FString_PeekCursor(FormatCursor) == '*') {
		FString_BumpCursor(FormatCursor);

		Status = FString_ParseFormatIndex(FormatCursor, &Format.WidthIndex, UseIndexes, FALSE);
		if (Status != FSTRING_FORMAT_VALID) return Status;

		Format.Type |= FSTRING_FORMAT_FLAG_PARAM_WIDTH;
	} else {
		// It's not provided by an argument, so it's either a number or 0.
		Status = FString_ParseFormatInt(FormatCursor, &Format.Width);
		if (Status == FSTRING_FORMAT_INT_NOT_PRESENT) Format.Width = 0;
		else if (Status != FSTRING_FORMAT_VALID) return Status;
	}

precision:
	// Now for precision. We know it's present if there's a period.
	if (FString_PeekCursor(FormatCursor) == '.') {
		FString_BumpCursor(FormatCursor);

		// Check if it's provided by a param
		if (FString_PeekCursor(FormatCursor) == '*') {
			FString_BumpCursor(FormatCursor);

			Status =
				FString_ParseFormatIndex(FormatCursor, &Format.PrecisionIndex, UseIndexes, FALSE);
			if (Status != FSTRING_FORMAT_VALID) return Status;

			Format.Type |= FSTRING_FORMAT_FLAG_PARAM_PRECISION;
		} else {
			// It's not provided by an argument, so it's either a number or 0.
			Status = FString_ParseFormatInt(FormatCursor, &Format.Precision);
			if (Status == FSTRING_FORMAT_INT_NOT_PRESENT) Format.Precision = 0;
			else if (Status != FSTRING_FORMAT_VALID) return Status;
		}
	}

	// Finally, parse the size and type.
	Status = FString_ParseFormatType(FormatCursor, &Format.Type);
	if (Status != FSTRING_FORMAT_VALID) return Status;

	Format.SpecifierString.Length =
		(usize) FormatCursor->Text - (usize) Format.SpecifierString.Text;
	if (FormatOut) *FormatOut = Format;
	return FSTRING_FORMAT_VALID;
}

/// @brief Parse a format string into a data structure containing a list of formats. The arrays are
/// allocated via `Stack_Allocate`.
/// @param[in,out] FormatString A cursor pointing to the beginning of a format string. After return,
/// this will point to the original string or the first invalid character encountered.
/// @param[out] FormatListOut A pointer to the resulting data structure with the format list.
/// @return
/// - `FSTRING_FORMAT_VALID`: All formats were successfully parsed.
///
/// - `FSTRING_FORMAT_INT_OVERFLOW`: A width, precision, or param index was too large.
///
/// - `FSTRING_FORMAT_INDEX_INCONSISTENT`: At least one, but not all params specified an index.
///
/// - `FSTRING_FORMAT_TYPE_INVALID`: A format's size and type had an invalid pattern.
internal fstring_format_status
FString_ParseFormatString(string *FormatCursor, fstring_format_list *FormatListOut)
{
	if (!FormatCursor || !FormatCursor->Text || !FormatCursor->Length) return FSTRING_FORMAT_VALID;

	fstring_format_status Status	 = FSTRING_FORMAT_VALID;
	fstring_format_list	  FormatList = { 0 };
	Mem_Set(&FormatList, 0, sizeof(fstring_format_list));
	FormatList.FormatString = *FormatCursor;

	// First we have to iterate through to determine the number of formats and params.
	c08 C;
	b08 UseIndexes = FALSE;
	while ((C = FString_PeekCursor(FormatCursor))) {
		if (C == '%') {
			FString_BumpCursor(FormatCursor);

			if (FString_PeekCursor(FormatCursor) != '%') {
				fstring_format Format;

				// As an optimization, we only need to parse the formats if indexes or used, or to
				// determine whether params are used in the first place.
				b08 First = FormatList.FormatCount == 0;
				Status	  = FString_ParseFormat(FormatCursor, &Format, &UseIndexes, First);
				if (Status != FSTRING_FORMAT_VALID) return Status;

				FormatList.FormatCount++;

				// Update param count to be the max index, plus 1
				if (UseIndexes) {
					if (FormatList.ParamCount <= Format.ValueIndex)
						FormatList.ParamCount = Format.ValueIndex;

					if ((Format.Type & FSTRING_FORMAT_FLAG_PARAM_WIDTH)
						&& FormatList.ParamCount <= Format.WidthIndex)
						FormatList.ParamCount = Format.WidthIndex;

					if ((Format.Type & FSTRING_FORMAT_FLAG_PARAM_PRECISION)
						&& FormatList.ParamCount <= Format.PrecisionIndex)
						FormatList.ParamCount = Format.PrecisionIndex;
				} else {
					FormatList.ParamCount++;
					if (Format.Type & FSTRING_FORMAT_FLAG_PARAM_WIDTH) FormatList.ParamCount++;
					if (Format.Type & FSTRING_FORMAT_FLAG_PARAM_PRECISION) FormatList.ParamCount++;
				}

				// We also need to update if we'll be using any extra data
				if ((Format.Type & FSTRING_FORMAT_TYPE_MASK) == FSTRING_FORMAT_TYPE_STR)
					FormatList.ExtraDataSize += sizeof(string);

				continue;
			}
		}

		FString_BumpCursor(FormatCursor);
		FormatList.NonFormattedTextSize++;
	}

	// We can go ahead and early-out if there weren't any formats
	if (!FormatList.FormatCount) goto end;

	FormatList.Formats	 = Stack_Allocate(FormatList.FormatCount * sizeof(fstring_format));
	FormatList.ExtraData = Stack_Allocate(FormatList.ExtraDataSize);

	// Next, we have to run through again and this time, we'll actually save the formats.
	usize			ParamIndex = 1;
	fstring_format *Format	   = FormatList.Formats;
	*FormatCursor			   = FormatList.FormatString;
	while ((C = FString_PeekCursor(FormatCursor))) {
		if (C == '%') {
			FString_BumpCursor(FormatCursor);

			if (FString_PeekCursor(FormatCursor) != '%') {
				Status = FString_ParseFormat(FormatCursor, Format, &UseIndexes, FALSE);
				if (Status != FSTRING_FORMAT_VALID) return Status;

				// Specify the sequential indexes explicitly to make later steps easier
				if (!UseIndexes) {
					Format->ValueIndex = ParamIndex++;

					if (Format->Type & FSTRING_FORMAT_FLAG_PARAM_WIDTH)
						Format->WidthIndex = ParamIndex++;

					if (Format->Type & FSTRING_FORMAT_FLAG_PARAM_PRECISION)
						Format->PrecisionIndex = ParamIndex++;
				}

				Format++;
				continue;
			}
		}

		FString_BumpCursor(FormatCursor);
	}

end:
	if (FormatListOut) *FormatListOut = FormatList;
	*FormatCursor = FormatList.FormatString;
	return FSTRING_FORMAT_VALID;
}

/// @brief Uses the formats within a format list, alongside a va_list, to read the params from the
/// va_list and write them into the FormatList's params. This includes updating the extra data for
/// strings.
/// @param[in,out] FormatList A pointer to the format list to be used to interpret the params.
/// Cannot be null.
/// @param[in] Args The va_list to source the params from. Note that you must call `VA_End` before
/// using this again.
/// @return
/// - `FSTRING_FORMAT_VALID`: The params were read and updated successfully.
///
/// - `FSTRING_FORMAT_PARAM_REDEFINED`: An index was defined as multiple types, which is disallowed.
/// `FormatList.FormatString` will span the specifier where this occurred.
///
/// - `FSTRING_FORMAT_INDEX_NOT_PRESENT`: At least one param was not referenced by any specifier.
/// `FormatList.FormatString` will span the entire format string.
///
/// - `FSTRING_FORMAT_INT_OVERFLOW`: A width param was -2^31, which overflows on absolute value.
/// `FormatList.FormatString` will span the specifier where this occurred.
///
/// - `FSTRING_FORMAT_ENCODING_INVALID`: A provided string param had an encoding mismatch.
/// `FormatList.FormatString` will span the specifier where this occurred.
internal fstring_format_status
FVString_UpdateParamReferences(fstring_format_list *FormatList, va_list Args)
{
	Assert(FormatList);
	fstring_format *Format;

	vptr ExtraData = FormatList->ExtraData;

	fstring_format_type IndexType = FSTRING_FORMAT_TYPE_S32;
	fstring_format_type Mask	  = FSTRING_FORMAT_TYPE_MASK;

	// First, we need to order the params by collecting them into a temporary list
	Stack_Push();
	fstring_param *Params = Stack_Allocate(FormatList->ParamCount * sizeof(fstring_param));
	Mem_Set(Params, 0, FormatList->ParamCount * sizeof(fstring_param));

	for (usize I = 0; I < FormatList->FormatCount; I++) {
		Format = &FormatList->Formats[I];

		// It's an error to re-reference a param under a different type. Architectures can
		// depend on the size of args you provide to determine the next ones, so redefining
		// a type can make later param types ambiguous.
		if (Params[Format->ValueIndex - 1].Type
			&& (Params[Format->ValueIndex - 1].Type & Mask) != Format->Type)
			goto redefined;
		Params[Format->ValueIndex - 1].Type = Format->Type;

		if (Format->Type & FSTRING_FORMAT_FLAG_PARAM_WIDTH) {
			if (Params[Format->WidthIndex - 1].Type
				&& (Params[Format->WidthIndex - 1].Type & Mask) != IndexType)
				goto redefined;
			Params[Format->WidthIndex - 1].Type = IndexType;
		}

		if (Format->Type & FSTRING_FORMAT_FLAG_PARAM_PRECISION) {
			if (Params[Format->PrecisionIndex - 1].Type
				&& (Params[Format->PrecisionIndex - 1].Type & Mask) != IndexType)
				goto redefined;
			Params[Format->PrecisionIndex - 1].Type = IndexType;
		}
	}

	// Then we run through each param and update its value
	for (usize I = 0; I < FormatList->ParamCount; I++) {
		switch (Params[I].Type & FSTRING_FORMAT_TYPE_MASK) {
			case FSTRING_FORMAT_TYPE_B08	: Params[I].Bool = (b08) VA_Next(Args, s32); break;
			case FSTRING_FORMAT_TYPE_CHR	: Params[I].Char = (c32) VA_Next(Args, c32); break;
			case FSTRING_FORMAT_TYPE_S08	: Params[I].Signed = (s08) VA_Next(Args, s32); break;
			case FSTRING_FORMAT_TYPE_S16	: Params[I].Signed = (s16) VA_Next(Args, s32); break;
			case FSTRING_FORMAT_TYPE_S32	: Params[I].Signed = VA_Next(Args, s32); break;
			case FSTRING_FORMAT_TYPE_S64	: Params[I].Signed = VA_Next(Args, s64); break;
			case FSTRING_FORMAT_TYPE_SSIZE	: Params[I].Signed = VA_Next(Args, ssize); break;
			case FSTRING_FORMAT_TYPE_U08	: Params[I].Unsigned = (u08) VA_Next(Args, u32); break;
			case FSTRING_FORMAT_TYPE_U16	: Params[I].Unsigned = (u16) VA_Next(Args, u32); break;
			case FSTRING_FORMAT_TYPE_U32	: Params[I].Unsigned = VA_Next(Args, u32); break;
			case FSTRING_FORMAT_TYPE_U64	: Params[I].Unsigned = VA_Next(Args, u64); break;
			case FSTRING_FORMAT_TYPE_USIZE	: Params[I].Unsigned = VA_Next(Args, usize); break;
			case FSTRING_FORMAT_TYPE_R64	: Params[I].Float = VA_Next(Args, r64); break;
			case FSTRING_FORMAT_TYPE_PTR	: Params[I].Pointer = VA_Next(Args, vptr); break;
			case FSTRING_FORMAT_TYPE_QUERY16: Params[I].Pointer = VA_Next(Args, s16 *); break;
			case FSTRING_FORMAT_TYPE_QUERY32: Params[I].Pointer = VA_Next(Args, s32 *); break;
			case FSTRING_FORMAT_TYPE_QUERY64: Params[I].Pointer = VA_Next(Args, s64 *); break;

			// Because C varargs suck, we have to copy the string into buffer data
			case FSTRING_FORMAT_TYPE_STR:
				Params[I].String  = ExtraData;
				ExtraData		  = Params[I].String + 1;
				*Params[I].String = VA_Next(Args, string);
				break;

			// We can't allow skipping any, sadly, similarly because C varargs suck
			default: Stack_Pop(); return FSTRING_FORMAT_INDEX_NOT_PRESENT;
		}
	}

	// Finally, we can run through the formats again, update their values, and pop the params
	for (usize I = 0; I < FormatList->FormatCount; I++) {
		Format = &FormatList->Formats[I];

		Format->Value = Params[Format->ValueIndex - 1];
		if ((Format->Type & FSTRING_FORMAT_TYPE_MASK) == FSTRING_FORMAT_TYPE_STR) {
			if (Format->Type & FSTRING_FORMAT_FLAG_STR_WIDE
					? Format->Value.String->Encoding != STRING_ENCODING_UTF32
					: Format->Value.String->Encoding != STRING_ENCODING_ASCII
						  && Format->Value.String->Encoding != STRING_ENCODING_UTF8)
			{
				Stack_Pop();
				FormatList->FormatString = Format->SpecifierString;
				return FSTRING_FORMAT_ENCODING_INVALID;
			}
		}

		if (Format->Type & FSTRING_FORMAT_FLAG_PARAM_WIDTH) {
			Format->Width = Params[Format->WidthIndex - 1].Signed;
			// If width is negative, it means the format is left justified
			if (Format->Width < 0) {
				if (Format->Width == 0x80000000) {
					Stack_Pop();
					FormatList->FormatString = Format->SpecifierString;
					return FSTRING_FORMAT_INT_OVERFLOW;
				}
				Format->Width  = -Format->Width;
				Format->Type  |= FSTRING_FORMAT_FLAG_LEFT_JUSTIFY;
			}
		}

		if (Format->Type & FSTRING_FORMAT_FLAG_PARAM_PRECISION)
			Format->Precision = Params[Format->PrecisionIndex - 1].Signed;
	}
	Stack_Pop();
	return FSTRING_FORMAT_VALID;

redefined:
	Stack_Pop();
	FormatList->FormatString = Format->SpecifierString;
	return FSTRING_FORMAT_PARAM_REDEFINED;
}

/// @brief Utility function to call `FVString_UpdateParamReferences` via varargs. Mainly used for
/// testing.
/// @param[in,out] FormatList A pointer to the format list to be used to interpret the params.
/// Cannot be null.
/// @param[in] ... The params to update FormatList's references with.
/// @return See `FVString_UpdateParamReferences` for possible return values.
internal fstring_format_status
FString_UpdateParamReferences(fstring_format_list *FormatList, ...)
{
	va_list Args;
	VA_Start(Args, FormatList);

	fstring_format_status Result = FVString_UpdateParamReferences(FormatList, Args);

	VA_End(Args);
	return Result;
}

/// @brief Write a string into the buffer, based on the format. Only alignment and width are
/// considered. The string's encoding is used instead of any flags. The resulting string will be
/// encoded as UTF-8.
/// @param Format A pointer to the format being written. On return, if actual width was 0, it will
/// contain the updated actual width. Cannot be null.
/// @param Buffer A pointer to be buffer being written into. If this is null, only the format's
/// actual width will be updated. Otherwise, the buffer must be valid and large enough to contain
/// the format's full length.
/// @return
/// - `FSTRING_FORMAT_VALID`: The format was written successfully.
///
/// - `FSTRING_FORMAT_BUFFER_TOO_SMALL`: The provided buffer was invalid or too small.
internal fstring_format_status
FString_WriteString(fstring_format *Format, string *Buffer)
{
	Assert(Format);

	if (Format->ContentLength == 0) {
		string Cursor = *Format->Value.String;
		while (Cursor.Length) {
			u32 Codepoint		   = String_NextCodepoint(&Cursor);
			Format->ContentLength += String_WriteCodepoint(NULL, Codepoint);
		}
		Format->ActualWidth = MAX(Format->Width, Format->ContentLength);
	}

	if (!Buffer) return FSTRING_FORMAT_VALID;
	if (!Buffer->Text || Buffer->Length < Format->ActualWidth)
		return FSTRING_FORMAT_BUFFER_TOO_SMALL;

	string Cursor = *Format->Value.String;
	string Dest	  = *Buffer;

	b08	  IsLeft	= !!(Format->Type & FSTRING_FORMAT_FLAG_LEFT_JUSTIFY);
	usize PadLength = Format->ActualWidth - Format->ContentLength;

	if (!IsLeft)
		for (usize I = 0; I < PadLength; I++)
			String_BumpUnits(&Dest, String_WriteCodepoint(&Dest, ' '));

	while (Cursor.Length) {
		u32	  Codepoint = String_NextCodepoint(&Cursor);
		usize Written	= String_WriteCodepoint(&Dest, Codepoint);
		String_BumpUnits(&Dest, Written);
		Dest.Text += Written;
	}

	if (IsLeft)
		for (usize I = 0; I < PadLength; I++)
			String_BumpUnits(&Dest, String_WriteCodepoint(&Dest, ' '));

	return FSTRING_FORMAT_VALID;
}

/// @brief Write a character into the buffer, based on the format. Accepts ASCII and UTF32.
/// Only alignment, width, and `FLAG_CHR_WIDE` are considered.
/// @param Format A pointer to the format being written. On return, if actual width was 0, it will
/// contain the updated actual width. Cannot be null.
/// @param Buffer A pointer to be buffer being written into. If this is null, only the format's
/// actual width will be updated. Otherwise, the buffer must be valid and large enough to contain
/// the format's full length.
/// @return
/// - `FSTRING_FORMAT_VALID`: The format was written successfully.
///
/// - `FSTRING_FORMAT_BUFFER_TOO_SMALL`: The provided buffer was invalid or too small.
internal fstring_format_status
FString_WriteChar(fstring_format *Format, string *Buffer)
{
	Assert(Format);

	string String;
	String.Length = 1;

	b08 IsWide	  = !!(Format->Type & FSTRING_FORMAT_FLAG_CHR_WIDE);
	Format->Type &= FSTRING_FORMAT_FLAG_LEFT_JUSTIFY;

	c32 C32 = Format->Value.Char;
	c08 C08 = (c08) Format->Value.Char;
	if (IsWide) {
		String.Encoding = STRING_ENCODING_UTF32;
		String.Text32	= &C32;
	} else {
		String.Encoding = STRING_ENCODING_ASCII;
		String.Text		= &C08;
	}
	Format->Value.String = &String;

	return FString_WriteString(Format, Buffer);
}

/// @brief Write a boolean 'true' or 'false' into the buffer, based on the format. Only alignment
/// and width are considered.
/// @param Format A pointer to the format being written. On return, if actual width was 0, it will
/// contain the updated actual width. Cannot be null.
/// @param Buffer A pointer to be buffer being written into. If this is null, only the format's
/// actual width will be updated. Otherwise, the buffer must be valid and large enough to contain
/// the format's full length.
/// @return
/// - `FSTRING_FORMAT_VALID`: The format was written successfully.
///
/// - `FSTRING_FORMAT_BUFFER_TOO_SMALL`: The provided buffer was invalid or too small.
internal fstring_format_status
FString_WriteBool(fstring_format *Format, string *Buffer)
{
	Assert(Format);

	string True	 = CStringL("true");
	string False = CStringL("false");

	Format->Value.String = Format->Value.Bool ? &True : &False;
	return FString_WriteString(Format, Buffer);
}

/// @brief Write an unsigned int into the buffer, based on the format. Alignment, width, precision,
/// prefixing, grouping, and 0-padding are considered.
/// @param Format A pointer to the format being written. On return, if actual width was 0, it will
/// contain the updated actual width. Cannot be null.
/// @param Buffer A pointer to be buffer being written into. If this is null, only the format's
/// actual width will be updated. Otherwise, the buffer must be valid and large enough to contain
/// the format's full length.
/// @return
/// - `FSTRING_FORMAT_VALID`: The format was written successfully.
///
/// - `FSTRING_FORMAT_BUFFER_TOO_SMALL`: The provided buffer was invalid or too small.
internal fstring_format_status
FString_WriteUnsigned(fstring_format *Format, string *Buffer)
{
	Assert(Format);

	b08 IsLeft		 = !!(Format->Type & FSTRING_FORMAT_FLAG_LEFT_JUSTIFY);
	b08 SpecifyRadix = !!(Format->Type & FSTRING_FORMAT_FLAG_SPECIFY_RADIX);
	b08 IsGrouped	 = !!(Format->Type & FSTRING_FORMAT_FLAG_SEPARATE_GROUPS);
	b08 IsUppercase	 = !!(Format->Type & FSTRING_FORMAT_FLAG_UPPERCASE);
	b08 PadZero		 = !!(Format->Type & FSTRING_FORMAT_FLAG_PAD_WITH_ZERO);

	string RadixPrefix = EString();
	string GroupStr	   = CStringL(",");
	usize  Radix	   = 10;
	usize  GroupSize   = 3;

	if (Format->Type & FSTRING_FORMAT_FLAG_INT_BIN) {
		RadixPrefix = IsUppercase ? CStringL("0B") : CStringL("0b");
		GroupStr	= CStringL("_");
		Radix		= 2;
		GroupSize	= 8;
	} else if (Format->Type & FSTRING_FORMAT_FLAG_INT_OCT) {
		RadixPrefix = CStringL("0");
		GroupStr	= CStringL("_");
		Radix		= 8;
	} else if (Format->Type & FSTRING_FORMAT_FLAG_INT_HEX) {
		RadixPrefix = IsUppercase ? CStringL("0X") : CStringL("0x");
		GroupStr	= CStringL("_");
		Radix		= 16;
		GroupSize	= 4;
	}

	if (!SpecifyRadix) RadixPrefix = EString();
	if (!IsGrouped) GroupStr = EString();

	usize MinDigits	 = Format->Precision < 0 ? 1 : Format->Precision;
	usize GroupCount = (MinDigits - 1) / GroupSize;

	if (Format->ContentLength == 0) {
		usize Value = Format->Value.Unsigned;
		while (Value > 0) {
			Value /= Radix;
			Format->ContentLength++;
		}
		Format->ContentLength = MAX(MinDigits, Format->ContentLength);

		Format->ContentLength += RadixPrefix.Length;
		Format->ContentLength += GroupCount * GroupStr.Length;

		Format->ActualWidth = MAX(Format->Width, Format->ContentLength);
	}

	if (!Buffer) return FSTRING_FORMAT_VALID;
	if (!Buffer->Text || Buffer->Length < Format->ActualWidth)
		return FSTRING_FORMAT_BUFFER_TOO_SMALL;

	string RadixChars = IsUppercase ? CStringL("0123456789ABCDEF") : CStringL("0123456789abcdef");
	usize  PadLength  = Format->ActualWidth - Format->ContentLength;

	string Dest = *Buffer;
	if (!IsLeft) {
		u32 PadChar = ' ';
		if (Format->Precision < 0 && PadZero) PadChar = '0';
		for (usize I = 0; I < PadLength; I++)
			String_BumpUnits(&Dest, String_WriteCodepoint(&Dest, PadChar));
	}

	while (RadixPrefix.Length) {
		u32 Codepoint = String_NextCodepoint(&RadixPrefix);
		String_BumpUnits(&Dest, String_WriteCodepoint(&Dest, Codepoint));
	}

	usize Value		 = Format->Value.Unsigned;
	usize DigitCount = Format->ContentLength - RadixPrefix.Length - GroupCount * GroupStr.Length;
	usize DigitsSinceGroup = 0;

	Dest.Text	+= Format->ContentLength - RadixPrefix.Length;
	Dest.Length += Format->ContentLength - RadixPrefix.Length;
	for (usize I = 0; I < DigitCount; I++) {
		u32 Codepoint  = RadixChars.Text[Value % Radix];
		Value		  /= Radix;

		String_BumpUnits(&Dest, -String_WriteCodepoint(NULL, Codepoint));
		String_WriteCodepoint(&Dest, Codepoint);
		DigitsSinceGroup++;

		if (DigitsSinceGroup == GroupSize) {
			DigitsSinceGroup = 0;
			string Group	 = GroupStr;
			while (Group.Length) {
				u32 Codepoint = String_NextCodepoint(&Group);
				String_BumpUnits(&Dest, -String_WriteCodepoint(NULL, Codepoint));
				String_WriteCodepoint(&Dest, Codepoint);
			}
		}
	}

	if (IsLeft) {
		Dest.Text	= Buffer->Text + Format->ContentLength;
		Dest.Length = PadLength;
		for (usize I = 0; I < PadLength; I++)
			String_BumpUnits(&Dest, String_WriteCodepoint(&Dest, ' '));
	}

	return FSTRING_FORMAT_VALID;
}

/// @brief Write a signed int into the buffer, based on the format. Alignment, width, precision,
/// signage, grouping, and 0-padding are considered.
/// @param Format A pointer to the format being written. On return, if actual width was 0, it will
/// contain the updated actual width. Cannot be null.
/// @param Buffer A pointer to be buffer being written into. If this is null, only the format's
/// actual width will be updated. Otherwise, the buffer must be valid and large enough to contain
/// the format's full length.
/// @return
/// - `FSTRING_FORMAT_VALID`: The format was written successfully.
///
/// - `FSTRING_FORMAT_BUFFER_TOO_SMALL`: The provided buffer was invalid or too small.
internal fstring_format_status
FString_WriteSigned(fstring_format *Format, string *Buffer)
{
	Assert(Format);

	b08 IsLeft		= !!(Format->Type & FSTRING_FORMAT_FLAG_LEFT_JUSTIFY);
	b08 PrefixSign	= !!(Format->Type & FSTRING_FORMAT_FLAG_PREFIX_SIGN);
	b08 PrefixSpace = !!(Format->Type & FSTRING_FORMAT_FLAG_PREFIX_SPACE);
	b08 IsGrouped	= !!(Format->Type & FSTRING_FORMAT_FLAG_SEPARATE_GROUPS);
	b08 IsUppercase = !!(Format->Type & FSTRING_FORMAT_FLAG_UPPERCASE);
	b08 PadZero		= !!(Format->Type & FSTRING_FORMAT_FLAG_PAD_WITH_ZERO);

	string Prefix = Format->Value.Signed < 0 ? CStringL("-")
				  : PrefixSign				 ? CStringL("+")
				  : PrefixSpace				 ? CStringL(" ")
											 : EString();

	string GroupStr	  = IsGrouped ? CStringL(",") : EString();
	usize  GroupSize  = 3;
	usize  MinDigits  = Format->Precision < 0 ? 1 : Format->Precision;
	usize  GroupCount = (MinDigits - 1) / GroupSize;

	if (Format->ContentLength == 0) {
		ssize Value = Format->Value.Signed;
		while (Value) {
			Value /= 10;
			Format->ContentLength++;
		}
		Format->ContentLength = MAX(MinDigits, Format->ContentLength);

		Format->ContentLength += Prefix.Length;
		Format->ContentLength += GroupCount * GroupStr.Length;

		Format->ActualWidth = MAX(Format->Width, Format->ContentLength);
	}

	if (!Buffer) return FSTRING_FORMAT_VALID;
	if (!Buffer->Text || Buffer->Length < Format->ActualWidth)
		return FSTRING_FORMAT_BUFFER_TOO_SMALL;

	usize PadLength = Format->ActualWidth - Format->ContentLength;

	string Dest = *Buffer;
	if (!IsLeft) {
		u32 PadChar = ' ';
		if (Format->Precision < 0 && PadZero) PadChar = '0';
		for (usize I = 0; I < PadLength; I++)
			String_BumpUnits(&Dest, String_WriteCodepoint(&Dest, PadChar));
	}

	while (Prefix.Length) {
		u32 Codepoint = String_NextCodepoint(&Prefix);
		String_BumpUnits(&Dest, String_WriteCodepoint(&Dest, Codepoint));
	}

	ssize Value			   = Format->Value.Signed;
	usize DigitCount	   = Format->ContentLength - Prefix.Length - GroupCount * GroupStr.Length;
	usize DigitsSinceGroup = 0;

	Dest.Text	+= Format->ContentLength - Prefix.Length;
	Dest.Length += Format->ContentLength - Prefix.Length;
	for (usize I = 0; I < DigitCount; I++) {
		u32 Codepoint  = '0' + (Value < 0 ? -(Value % 10) : Value % 10);
		Value		  /= 10;

		String_BumpUnits(&Dest, -String_WriteCodepoint(NULL, Codepoint));
		String_WriteCodepoint(&Dest, Codepoint);
		DigitsSinceGroup++;

		if (DigitsSinceGroup == GroupSize) {
			DigitsSinceGroup = 0;
			string Group	 = GroupStr;
			while (Group.Length) {
				u32 Codepoint = String_NextCodepoint(&Group);
				String_BumpUnits(&Dest, -String_WriteCodepoint(NULL, Codepoint));
				String_WriteCodepoint(&Dest, Codepoint);
			}
		}
	}

	if (IsLeft) {
		Dest.Text	= Buffer->Text + Format->ContentLength;
		Dest.Length = PadLength;
		for (usize I = 0; I < PadLength; I++)
			String_BumpUnits(&Dest, String_WriteCodepoint(&Dest, ' '));
	}

	return FSTRING_FORMAT_VALID;
}

internal fstring_format_status
FString_WriteHexFloat(fstring_format *Format, string *Buffer)
{
	Assert(Format);
	Stack_Push();

	static const string Empty		  = EString();
	static const string NanUpper	  = CStringL("NAN");
	static const string NanLower	  = CStringL("nan");
	static const string InfUpper	  = CStringL("INF");
	static const string InfLower	  = CStringL("inf");
	static const string SignPos		  = CStringL("+");
	static const string SignNeg		  = CStringL("-");
	static const string SignSpace	  = CStringL(" ");
	static const string HexUpper	  = CStringL("0X");
	static const string HexLower	  = CStringL("0x");
	static const string ExpHexUpper	  = CStringL("P");
	static const string ExpHexLower	  = CStringL("p");
	static const string Period		  = CStringL(".");
	static const u32	DigitsUpper[] = { '0', '1', '2', '3', '4', '5', '6', '7',
										  '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	static const u32	DigitsLower[] = { '0', '1', '2', '3', '4', '5', '6', '7',
										  '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

	b08 IsLeft		 = !!(Format->Type & FSTRING_FORMAT_FLAG_LEFT_JUSTIFY);
	b08 SpecifyRadix = !!(Format->Type & FSTRING_FORMAT_FLAG_SPECIFY_RADIX);
	b08 PrefixSign	 = !!(Format->Type & FSTRING_FORMAT_FLAG_PREFIX_SIGN);
	b08 PrefixSpace	 = !!(Format->Type & FSTRING_FORMAT_FLAG_PREFIX_SPACE);
	b08 IsGrouped	 = !!(Format->Type & FSTRING_FORMAT_FLAG_SEPARATE_GROUPS);
	b08 IsUpper		 = !!(Format->Type & FSTRING_FORMAT_FLAG_UPPERCASE);
	b08 PadZero		 = !!(Format->Type & FSTRING_FORMAT_FLAG_PAD_WITH_ZERO);

	r64 Value  = Format->Value.Float;
	u64 Binary = *(u64 *) &Value;

	b08	  Negative = Binary >> 63;
	ssize Exponent = (ssize) ((Binary >> 52) & 0x3FF) - 1023;
	usize Mantissa = Binary & ((1ull << 52) - 1);

	const u32 *DigitsList = IsUpper ? DigitsUpper : DigitsLower;

	b08	   IsSpecial	 = Exponent == 1024;
	string SpecialString = !IsSpecial ? Empty
						 : Mantissa	  ? (IsUpper ? NanUpper : NanLower)
									  : (IsUpper ? InfUpper : InfLower);

	if (Exponent == -1023) Exponent = -1022;
	else Mantissa |= 1ull << 52;

	b08 NegativeExp = Exponent < 0;
	Exponent		= NegativeExp ? -Exponent : Exponent;

	s32 MantissaLSB;
	if (!Intrin_BitScanForward64((u32 *) &MantissaLSB, Mantissa)) MantissaLSB = 52;

	bigint WholePart, FracPart, Radix, WholeDivisor, FracDivisor;
	usize  WholeDigits = 0, FracDigits = 0, ExpDigits = 0, ExpDivisor = 1;
	if (!IsSpecial) {
		Radix = BigInt(16);

		WholePart	 = BigInt(Mantissa >> 52);
		WholeDivisor = BigInt(1);
		while (BigInt_Compare(WholePart, WholeDivisor) >= 0) {
			WholeDivisor = BigInt_SMul(WholeDivisor, Radix);
			WholeDigits++;
		}
		if (WholeDigits < 1) WholeDigits = 1;

		FracPart	= BigInt(Mantissa & ((1ull << 52) - 1));
		FracDivisor = BigInt(1);
		while (BigInt_Compare(FracPart, FracDivisor) >= 0) {
			FracDivisor = BigInt_SMul(FracDivisor, Radix);
			FracDigits++;
		}
		if (Format->Precision < 0)
			while (!BigInt_SRem(FracPart, Radix).Words[0]) FracDigits--;
		else FracDigits = Format->Precision;

		while (Exponent >= ExpDivisor) {
			ExpDivisor *= 10;
			ExpDigits++;
		}
		if (ExpDigits < 1) ExpDigits = 1;
	}

	string SignString = Negative ? SignNeg : PrefixSign ? SignPos : PrefixSpace ? SignSpace : Empty;
	string HexString  = IsSpecial ? Empty : IsUpper ? HexUpper : HexLower;
	string RadixString	 = IsSpecial || (!SpecifyRadix && !FracDigits) ? Empty : Period;
	string ExpSignString = IsSpecial ? Empty : NegativeExp ? SignNeg : SignPos;
	string ExpString	 = IsSpecial ? Empty : IsUpper ? ExpHexUpper : ExpHexLower;

	if (Format->ContentLength == 0) {
		Format->ContentLength = SignString.Length
							  + SpecialString.Length
							  + HexString.Length
							  + WholeDigits
							  + RadixString.Length
							  + FracDigits
							  + ExpSignString.Length
							  + ExpString.Length
							  + ExpDigits;

		Format->ActualWidth = MAX(Format->Width, Format->ContentLength);
	}

	if (!Buffer) {
		Stack_Pop();
		return FSTRING_FORMAT_VALID;
	}
	if (!Buffer->Text || Buffer->Length < Format->ActualWidth) {
		Stack_Pop();
		return FSTRING_FORMAT_BUFFER_TOO_SMALL;
	}

	usize PadLength = Format->ActualWidth - Format->ContentLength;

	string Dest = *Buffer;
	if (!IsLeft) {
		u32 PadChar = ' ';
		if (Format->Precision < 0 && PadZero) PadChar = '0';
		for (usize I = 0; I < PadLength; I++)
			String_BumpUnits(&Dest, String_WriteCodepoint(&Dest, PadChar));
	}

	while (SignString.Length) {
		u32 Codepoint = String_NextCodepoint(&SignString);
		String_BumpUnits(&Dest, String_WriteCodepoint(&Dest, Codepoint));
	}

	while (SpecialString.Length) {
		u32 Codepoint = String_NextCodepoint(&SpecialString);
		String_BumpUnits(&Dest, String_WriteCodepoint(&Dest, Codepoint));
	}

	while (HexString.Length) {
		u32 Codepoint = String_NextCodepoint(&HexString);
		String_BumpUnits(&Dest, String_WriteCodepoint(&Dest, Codepoint));
	}

	while (WholeDigits) {
		bigint Quot		 = BigInt_SDiv(WholePart, WholeDivisor);
		bigint Rem		 = BigInt_SRem(Quot, Radix);
		u32	   Codepoint = DigitsList[Rem.Words[0]];
		String_BumpUnits(&Dest, String_WriteCodepoint(&Dest, Codepoint));
		WholeDivisor = BigInt_SDiv(WholeDivisor, Radix);
		WholeDigits--;
	}

	while (RadixString.Length) {
		u32 Codepoint = String_NextCodepoint(&RadixString);
		String_BumpUnits(&Dest, String_WriteCodepoint(&Dest, Codepoint));
	}

	while (FracDigits) {
		bigint Quot		 = BigInt_SDiv(FracPart, FracDivisor);
		bigint Rem		 = BigInt_SRem(Quot, Radix);
		u32	   Codepoint = DigitsList[Rem.Words[0]];
		String_BumpUnits(&Dest, String_WriteCodepoint(&Dest, Codepoint));
		FracDivisor = BigInt_SDiv(FracDivisor, Radix);
		FracDigits--;
	}

	while (ExpSignString.Length) {
		u32 Codepoint = String_NextCodepoint(&ExpSignString);
		String_BumpUnits(&Dest, String_WriteCodepoint(&Dest, Codepoint));
	}

	while (ExpString.Length) {
		u32 Codepoint = String_NextCodepoint(&ExpString);
		String_BumpUnits(&Dest, String_WriteCodepoint(&Dest, Codepoint));
	}

	while (ExpDigits) {
		u32 Codepoint = DigitsList[(Exponent / ExpDivisor) % 10];
		String_BumpUnits(&Dest, String_WriteCodepoint(&Dest, Codepoint));
		ExpDivisor /= 10;
		ExpDigits--;
	}

	if (IsLeft) {
		Dest.Text	= Buffer->Text + Format->ContentLength;
		Dest.Length = PadLength;
		for (usize I = 0; I < PadLength; I++)
			String_BumpUnits(&Dest, String_WriteCodepoint(&Dest, ' '));
	}

	Stack_Pop();
	return FSTRING_FORMAT_VALID;
}

/// @brief Write a floating point value into the buffer, based on the format. Alignment, width,
/// precision, signage, radixing, grouping, and 0-padding are considered.
/// @param Format A pointer to the format being written. On return, if actual width was 0, it will
/// contain the updated actual width. Cannot be null.
/// @param Buffer A pointer to be buffer being written into. If this is null, only the format's
/// actual width will be updated. Otherwise, the buffer must be valid and large enough to contain
/// the format's full length.
/// @return
/// - `FSTRING_FORMAT_VALID`: The format was written successfully.
///
/// - `FSTRING_FORMAT_BUFFER_TOO_SMALL`: The provided buffer was invalid or too small.
internal fstring_format_status
FString_WriteFloat(fstring_format *Format, string *Buffer)
{
	return FString_WriteHexFloat(Format, Buffer);
}

/// @brief Write a pointer into the buffer, based on the format. Effectively the same as `%#X` unles
/// the pointer is null, in which case `(nil)` will be printed.` Only alignment and width are
/// considered.
/// @param Format A pointer to the format being written. On return, if actual width was 0, it will
/// contain the updated actual width. Cannot be null.
/// @param Buffer A pointer to be buffer being written into. If this is null, only the format's
/// actual width will be updated. Otherwise, the buffer must be valid and large enough to contain
/// the format's full length.
/// @return
/// - `FSTRING_FORMAT_VALID`: The format was written successfully.
///
/// - `FSTRING_FORMAT_BUFFER_TOO_SMALL`: The provided buffer was invalid or too small.
internal fstring_format_status
FString_WritePointer(fstring_format *Format, string *Buffer)
{
	Assert(Format);

	// We're only keeping this one
	Format->Type &= FSTRING_FORMAT_FLAG_LEFT_JUSTIFY;

	// Depending on if value is null, interpret as unsigned or print a string
	if (Format->Value.Pointer) {
		Format->Type |= FSTRING_FORMAT_FLAG_INT_HEX | FSTRING_FORMAT_FLAG_UPPERCASE;
		return FString_WriteUnsigned(Format, Buffer);
	}

	string Nil			 = CStringL("(nil)");
	Format->Value.String = &Nil;
	return FString_WriteString(Format, Buffer);
}

/// @brief Writes a formatted value into the provided buffer.
/// @param[in,out] Format A pointer to the format specifier containing the type, flags, and value.
/// Cannot be null.
/// @param[in,out] Buffer A pointer to the string being written into. If this is null, only
/// `Format->ActualWidth` is updated. Otherwise, the buffer must be valid and large enough to
/// contain the full output.
/// @return
/// - `FSTRING_FORMAT_VALID`: The format was written successfully.
///
/// - `FSTRING_FORMAT_BUFFER_TOO_SMALL`: The provided output buffer was too small to contain the
/// full output.
///
/// - `FSTRING_FORMAT_TYPE_INVALID`: The provided format's type was unsupported. This can occur, for
/// example, if you provide a non-writable query type.
internal fstring_format_status
FString_WriteFormat(fstring_format *Format, string *Buffer)
{
	Assert(Format);
	switch (Format->Type & FSTRING_FORMAT_TYPE_MASK) {
		case FSTRING_FORMAT_TYPE_B08  : return FString_WriteBool(Format, Buffer);
		case FSTRING_FORMAT_TYPE_CHR  : return FString_WriteChar(Format, Buffer);
		case FSTRING_FORMAT_TYPE_S08  : return FString_WriteSigned(Format, Buffer);
		case FSTRING_FORMAT_TYPE_U08  : return FString_WriteUnsigned(Format, Buffer);
		case FSTRING_FORMAT_TYPE_S16  : return FString_WriteSigned(Format, Buffer);
		case FSTRING_FORMAT_TYPE_U16  : return FString_WriteUnsigned(Format, Buffer);
		case FSTRING_FORMAT_TYPE_S32  : return FString_WriteSigned(Format, Buffer);
		case FSTRING_FORMAT_TYPE_U32  : return FString_WriteUnsigned(Format, Buffer);
		case FSTRING_FORMAT_TYPE_S64  : return FString_WriteSigned(Format, Buffer);
		case FSTRING_FORMAT_TYPE_U64  : return FString_WriteUnsigned(Format, Buffer);
		case FSTRING_FORMAT_TYPE_SSIZE: return FString_WriteSigned(Format, Buffer);
		case FSTRING_FORMAT_TYPE_USIZE: return FString_WriteUnsigned(Format, Buffer);
		case FSTRING_FORMAT_TYPE_R64  : return FString_WriteFloat(Format, Buffer);
		case FSTRING_FORMAT_TYPE_PTR  : return FString_WritePointer(Format, Buffer);
		case FSTRING_FORMAT_TYPE_STR  : return FString_WriteString(Format, Buffer);
		default						  : return FSTRING_FORMAT_TYPE_INVALID;
	}
}

/// @brief Compute the minimum size the output buffer needs to be to contain the fully formatted
/// string, and if the buffer is large enough, write into it.
/// @param[in,out] FormatList A pointer to the list of formats and other useful data. Format actual
/// widths and total text size will be updated after this call. Cannot be null.
/// @param[in,out] Buffer A pointer to thestring being written into. If this is null, only
/// `FormatList->TotalTextSize` and format actual widths are updated. Otherwise, the buffer must be
/// valid and large enough to contain the full output.
/// @return
/// - `FSTRING_FORMAT_VALID`: The total size was computed successfully.
///
/// - `FSTRING_FORMAT_BUFFER_TOO_SMALL`: The provided output buffer was too small to contain the
/// full output.
///
/// - `FSTRING_FORMAT_TYPE_INVALID`: One of the provided formats had an invalid type.
internal fstring_format_status
FString_WriteFormats(fstring_format_list *FormatList, string *Buffer)
{
	Assert(FormatList);

	// We haven't computed the total text size yet
	if (FormatList->TotalTextSize == 0) {
		FormatList->TotalTextSize = FormatList->NonFormattedTextSize;

		string *Strings = FormatList->ExtraData;

		// Run through the formats and accumulate their text sizes
		for (usize I = 0; I < FormatList->FormatCount; I++) {
			fstring_format *Format = &FormatList->Formats[I];

			usize Size = 0;
			switch (Format->Type & FSTRING_FORMAT_TYPE_MASK) {
				// Queries don't add anything to the total length. We'll set the current text size
				// as its actual width instead though, so we can easily update the query value
				// later.
				case FSTRING_FORMAT_TYPE_QUERY16:
				case FSTRING_FORMAT_TYPE_QUERY32:
				case FSTRING_FORMAT_TYPE_QUERY64:
					Format->ActualWidth = FormatList->TotalTextSize;
					break;

				default:
					fstring_format_status Status = FString_WriteFormat(Format, NULL);
					if (Status != FSTRING_FORMAT_VALID) return Status;
					FormatList->TotalTextSize += Size;
			}
		}
	}

	if (!Buffer) return FSTRING_FORMAT_VALID;
	if (!Buffer->Text || Buffer->Length < FormatList->TotalTextSize)
		return FSTRING_FORMAT_BUFFER_TOO_SMALL;

	// Now we can actually write into the buffer.
	c08	  *Src	= FormatList->FormatString.Text;
	string Dest = *Buffer;
	for (usize I = 0; I < FormatList->FormatCount; I++) {
		fstring_format *Format = &FormatList->Formats[I];

		// Write the plaintext into Dest
		usize PlaintextSize = (usize) Format->SpecifierString.Text - (usize) Src;
		Mem_Cpy(Dest.Text, Src, PlaintextSize);
		Dest.Text	+= PlaintextSize;
		Dest.Length -= PlaintextSize;

		// Skip past the format specifier
		Src = Format->SpecifierString.Text + Format->SpecifierString.Length;

		// Write the format into Dest
		fstring_format_status Status  = FString_WriteFormat(Format, &Dest);
		Dest.Text					 += Format->ActualWidth;
		Dest.Length					 -= Format->ActualWidth;

		// This shouldn't actually happen unless FormatList was corrupted somehow, but we'll respect
		// it to be safe.
		if (Status != FSTRING_FORMAT_VALID) return Status;
	}

	// Write the last bit of plaintext into Dest
	usize PlaintextSize =
		(usize) FormatList->FormatString.Text + FormatList->FormatString.Length - (usize) Src;
	Mem_Cpy(Dest.Text, Src, PlaintextSize);

	// Sanity check that we didn't just overwrite the buffer
	Assert(Dest.Length == PlaintextSize);

	return FSTRING_FORMAT_VALID;
}

/// @brief Format the provided template string with the given args.
/// @param[in] Format A template string to insert the parameters into. See `FString` for more
/// details.
/// @param[in] Args A va_list of parameters to insert into the format string. These must align with
/// the patterns in `Format` or stack corruption may occur.
/// @return A stack-backed formatted string. If any errors occurred during parsing, this will be a
/// copy of the format string, and an error will be logged.
internal string
FVString(string Format, va_list Args)
{
	fstring_format_list	  FormatList;
	fstring_format_status Status;

	string OriginalFormat = Format;
	string ErrorString	  = EString();

	// First we have to parse the format string
	Status = FString_ParseFormatString(&Format, &FormatList);
	if (Status != FSTRING_FORMAT_VALID) {
		ErrorString.Text   = Format.Text;
		ErrorString.Length = 1;
		goto failed;
	}

	// Now we populate its params with Args
	Status = FVString_UpdateParamReferences(&FormatList, Args);
	if (Status != FSTRING_FORMAT_VALID) {
		ErrorString = FormatList.FormatString;
		goto failed;
	}

	// Need to compute the total size to know how large the buffer should be
	Status = FString_WriteFormats(&FormatList, NULL);
	if (Status != FSTRING_FORMAT_VALID) {
		ErrorString = FormatList.FormatString;
		goto failed;
	}

	// Write into the buffer, and if everything looks good, return it.
	string Buffer = LString(FormatList.TotalTextSize);
	Status		  = FString_WriteFormats(&FormatList, &Buffer);
	if (Status != FSTRING_FORMAT_VALID) {
		ErrorString = FormatList.FormatString;
		goto failed;
	}

	return Buffer;

// The parsing failed, so we'll return a copy of the original format string and log an error
failed:
	Stack_Push();
	string ErrorPointer = LString(OriginalFormat.Length + 1);
	Mem_Set(ErrorPointer.Text, ' ', ErrorPointer.Length);
	Mem_Set(
		ErrorPointer.Text + (usize) ErrorString.Text - (usize) OriginalFormat.Text,
		'^',
		ErrorString.Length
	);
	Platform_WriteError(
		FString(
			CStringL("Error parsing format string, in specifier %d: %s\n\n%s\n%s\n\n"),
			FormatList.FormatCount + 1,
			FStringFormatStatusDescriptions[Status],
			OriginalFormat,
			ErrorPointer
		),
		FALSE
	);
	Stack_Pop();
	return SString(OriginalFormat);
}

/// @brief Format the provided template string with the given args.
/// @param[in] Format A template string to insert the parameters into. It follows the standard
/// `printf` format:
///
/// `'%' [ param-idx '$' ] [ flags ] [ width | '*' [ param-idx '$' ] ] [ '.' precision | '.' '*' [
/// param-idx '$' ] ] [ size ] type`
///
/// See the individual parsing functions for regex. Note that either all params use `param-idx` or
/// none do, and param indexes must span 1 to N with no skipped values.
/// @param[in] ... A vaiadic list of parameters to insert into the format string. These must align
/// with the patterns in `Format` or stack corruption may occur.
/// @return A stack-backed formatted string. If any errors occurred during parsing, this will be a
/// copy of the format string, and an error will be logged.
internal string
FString(string Format, ...)
{
	va_list Args;
	VA_Start(Args, Format);

	string Result = FVString(Format, Args);

	VA_End(Args);
	return Result;
}

// TODO:
// - Full formatting pattern documentation (don't want to pull up GCC every time)
// - Floating point support
// - Errno? Might need an architectural rework...
// - Standardize length (# code units) vs codepoint count, update functions accordingly
// - Match format string encoding with output encoding

#ifndef REGION_STRING_TESTS

#define STRING_TESTS                                                                                        \
	TEST(FString_PeekCursor, WorksCorrectly, (                                                              \
	    c08 Result = FString_PeekCursor(NULL);                                                              \
		Assert(Result == 0);                                                                                \
		string Cursor = {0};                                                                                \
	    Result = FString_PeekCursor(&Cursor);                                                               \
		Assert(Result == 0);                                                                                \
		Cursor = CString("");                                                                               \
	    Result = FString_PeekCursor(&Cursor);                                                               \
		Assert(Result == 0);                                                                                \
		Cursor = CString("Test");                                                                           \
	    Result = FString_PeekCursor(&Cursor);                                                               \
		Assert(Result == 'T');                                                                              \
	))                                                                                                      \
	TEST(FString_BumpCursor, WorksCorrectly, (                                                              \
	    FString_BumpCursor(NULL);                                                                           \
		string Cursor = {0};                                                                                \
	    FString_BumpCursor(&Cursor);                                                                        \
		Assert(Cursor.Text == NULL);                                                                        \
		Assert(Cursor.Length == 0);                                                                         \
		c08 *Empty = "";                                                                                    \
		Cursor = CString(Empty);                                                                            \
	    FString_BumpCursor(&Cursor);                                                                        \
		Assert(Cursor.Text == Empty);                                                                       \
		Assert(Cursor.Length == 0);                                                                         \
		Cursor = CString("Test");                                                                           \
	    FString_BumpCursor(&Cursor);                                                                        \
		Assert(String_Cmp(Cursor, CString("est")) == 0);                                                    \
	))                                                                                                      \
	TEST(FString_ParseFormatInt, ReportsNotPresentOnNonDigit, (                                             \
	    fstring_format_status Result = FString_ParseFormatInt(NULL, NULL);                                  \
		Assert(Result == FSTRING_FORMAT_INT_NOT_PRESENT);                                                   \
		s32 Int = -92;                                                                                      \
	    Result = FString_ParseFormatInt(NULL, &Int);                                                        \
		Assert(Result == FSTRING_FORMAT_INT_NOT_PRESENT);                                                   \
		Assert(Int == -92);                                                                                 \
		string Cursor = CString("-23");                                                                     \
	    Result = FString_ParseFormatInt(&Cursor, &Int);                                                     \
		Assert(Result == FSTRING_FORMAT_INT_NOT_PRESENT);                                                   \
		Assert(String_Cmp(Cursor, CString("-23")) == 0);                                                    \
		Assert(Int == -92);                                                                                 \
		Cursor = CString("-23");                                                                            \
	    Result = FString_ParseFormatInt(&Cursor, NULL);                                                     \
		Assert(Result == FSTRING_FORMAT_INT_NOT_PRESENT);                                                   \
		Assert(String_Cmp(Cursor, CString("-23")) == 0);                                                    \
	))                                                                                                      \
	TEST(FString_ParseFormatInt, ReportsNotPresentOnZero, (                                                 \
		s32 Int = -92;                                                                                      \
		string Cursor = CString("02");                                                                      \
	    fstring_format_status Result = FString_ParseFormatInt(&Cursor, &Int);                               \
		Assert(Result == FSTRING_FORMAT_INT_NOT_PRESENT);                                                   \
		Assert(String_Cmp(Cursor, CString("02")) == 0);                                                     \
		Assert(Int == -92);                                                                                 \
		Cursor = CString("02");                                                                             \
	    Result = FString_ParseFormatInt(&Cursor, NULL);                                                     \
		Assert(Result == FSTRING_FORMAT_INT_NOT_PRESENT);                                                   \
		Assert(String_Cmp(Cursor, CString("02")) == 0);                                                     \
	))                                                                                                      \
	TEST(FString_ParseFormatInt, ReportsIntOverflowOnTooLarge, (                                            \
		s32 Int = -92;                                                                                      \
		string Cursor = CString("2147483648");                                                              \
	    fstring_format_status Result = FString_ParseFormatInt(&Cursor, &Int);                               \
		Assert(Result == FSTRING_FORMAT_INT_OVERFLOW);                                                      \
		Assert(String_Cmp(Cursor, CString("8")) == 0);                                                      \
		Assert(Int == -92);                                                                                 \
		Cursor = CString("2147483648");                                                                     \
	    Result = FString_ParseFormatInt(&Cursor, NULL);                                                     \
		Assert(Result == FSTRING_FORMAT_INT_OVERFLOW);                                                      \
		Assert(String_Cmp(Cursor, CString("8")) == 0);                                                      \
	))                                                                                                      \
	TEST(FString_ParseFormatInt, ReturnsValidOnSuccesssfulParse, (                                          \
		s32 Int = -92;                                                                                      \
		string Cursor = CString("2147483647");                                                              \
	    fstring_format_status Result = FString_ParseFormatInt(&Cursor, &Int);                               \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Cursor, CString("")) == 0);                                                       \
		Assert(Int == 2147483647);                                                                          \
		Cursor = CString("1");                                                                              \
	    Result = FString_ParseFormatInt(&Cursor, &Int);                                                     \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Cursor, CString("")) == 0);                                                       \
		Assert(Int == 1);                                                                                   \
		Cursor = CString("1");                                                                              \
	    Result = FString_ParseFormatInt(&Cursor, NULL);                                                     \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Cursor, CString("")) == 0);                                                       \
	))                                                                                                      \
	TEST(FString_ParseFormatIndex, PropagatesIndexOverflow, (                                               \
		s32 Index = -92;                                                                                    \
		b08 UseIndexes = 2;                                                                                 \
		string Cursor = CString("2147483648");                                                              \
	    fstring_format_status Result = FString_ParseFormatIndex(&Cursor, &Index, &UseIndexes, TRUE);        \
		Assert(Result == FSTRING_FORMAT_INT_OVERFLOW);                                                      \
		Assert(String_Cmp(Cursor, CString("8")) == 0);                                                      \
		Assert(UseIndexes == 2);                                                                            \
		Assert(Index == -92);                                                                               \
		Cursor = CString("2147483648");                                                                     \
	    Result = FString_ParseFormatIndex(&Cursor, NULL, &UseIndexes, TRUE);                                \
		Assert(Result == FSTRING_FORMAT_INT_OVERFLOW);                                                      \
		Assert(String_Cmp(Cursor, CString("8")) == 0);                                                      \
	))                                                                                                      \
	TEST(FString_ParseFormatIndex, ReportsInconsistentIfIndexNotPresentAndExpected, (                       \
		b08 UseIndexes = TRUE;                                                                              \
		s32 Index = -92;                                                                                    \
		string Cursor = CString("-2$");                                                                     \
	    fstring_format_status Result = FString_ParseFormatIndex(&Cursor, &Index, &UseIndexes, FALSE);       \
		Assert(Result == FSTRING_FORMAT_INDEX_INCONSISTENT);                                                \
		Assert(String_Cmp(Cursor, CString("-2$")) == 0);                                                    \
		Assert(UseIndexes == TRUE);                                                                         \
		Assert(Index == -92);                                                                               \
	    Result = FString_ParseFormatIndex(NULL, NULL, &UseIndexes, FALSE);                                  \
		Assert(Result == FSTRING_FORMAT_INDEX_INCONSISTENT);                                                \
		Assert(UseIndexes == TRUE);                                                                         \
	))                                                                                                      \
	TEST(FString_ParseFormatIndex, ReturnsValidIfIndexNotPresentAndProhibited, (                            \
		b08 UseIndexes = FALSE;                                                                             \
		s32 Index = -92;                                                                                    \
		string Cursor = CString("-2$");                                                                     \
	    fstring_format_status Result = FString_ParseFormatIndex(&Cursor, &Index, &UseIndexes, FALSE);       \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Cursor, CString("-2$")) == 0);                                                    \
		Assert(UseIndexes == FALSE);                                                                        \
		Assert(Index == -92);                                                                               \
	    Result = FString_ParseFormatIndex(NULL, NULL, &UseIndexes, FALSE);                                  \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Cursor, CString("-2$")) == 0);                                                    \
		Assert(UseIndexes == FALSE);                                                                        \
	))                                                                                                      \
	TEST(FString_ParseFormatIndex, UpdatesUseIndexesIfIndexNotPresentAndUseIndexesUndefined, (              \
		b08 UseIndexes = 2;                                                                                 \
		s32 Index = -92;                                                                                    \
		string Cursor = CString("-2$");                                                                     \
	    fstring_format_status Result = FString_ParseFormatIndex(&Cursor, &Index, &UseIndexes, TRUE);        \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Cursor, CString("-2$")) == 0);                                                    \
		Assert(UseIndexes == FALSE);                                                                        \
		Assert(Index == -92);                                                                               \
	))                                                                                                      \
	TEST(FString_ParseFormatIndex, ReportsInconsistentIfIndexPresentAndProhibited, (                        \
		b08 UseIndexes = FALSE;                                                                             \
		s32 Index = -92;                                                                                    \
		string Cursor = CString("2$");                                                                      \
	    fstring_format_status Result = FString_ParseFormatIndex(&Cursor, &Index, &UseIndexes, FALSE);       \
		Assert(Result == FSTRING_FORMAT_INDEX_INCONSISTENT);                                                \
		Assert(String_Cmp(Cursor, CString("$")) == 0);                                                      \
		Assert(UseIndexes == FALSE);                                                                        \
		Assert(Index == 2);                                                                                 \
	))                                                                                                      \
	TEST(FString_ParseFormatIndex, ReturnsValidIfIndexIsValid, (                                            \
		b08 UseIndexes = TRUE;                                                                              \
		s32 Index = -92;                                                                                    \
		string Cursor = CString("2$");                                                                      \
	    fstring_format_status Result = FString_ParseFormatIndex(&Cursor, &Index, &UseIndexes, FALSE);       \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Cursor, CString("")) == 0);                                                       \
		Assert(UseIndexes == TRUE);                                                                         \
		Assert(Index == 2);                                                                                 \
	))                                                                                                      \
	TEST(FString_ParseFormatIndex, UpdatesUseIndexesIfIndexValidAndUseIndexesUndefined, (                   \
		b08 UseIndexes = FALSE;                                                                             \
		s32 Index = -92;                                                                                    \
		string Cursor = CString("2$");                                                                      \
	    fstring_format_status Result = FString_ParseFormatIndex(&Cursor, &Index, &UseIndexes, TRUE);        \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Cursor, CString("")) == 0);                                                       \
		Assert(UseIndexes == TRUE);                                                                         \
		Assert(Index == 2);                                                                                 \
	))                                                                                                      \
	TEST(FString_ParseFormatIndex, ReportsNotPresentIfNoDollarSignFollows, (                                \
		b08 UseIndexes = 2;                                                                                 \
		s32 Index = -92;                                                                                    \
		string Cursor = CString("2");                                                                       \
	    fstring_format_status Result = FString_ParseFormatIndex(&Cursor, &Index, &UseIndexes, TRUE);        \
		Assert(Result == FSTRING_FORMAT_INDEX_NOT_PRESENT);                                                 \
		Assert(String_Cmp(Cursor, CString("")) == 0);                                                       \
		Assert(UseIndexes == 2);                                                                            \
		Assert(Index == 2);                                                                                 \
	))                                                                                                      \
	TEST(FString_ParseFormatFlags, ReturnsValidAndKeepsExistingDataInOutput, (                              \
		fstring_format_type Flags = 29;                                                                     \
		string Cursor = CString("...");                                                                     \
	    fstring_format_status Result = FString_ParseFormatFlags(&Cursor, &Flags);                           \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Cursor, CString("...")) == 0);                                                    \
		Assert(Flags == 29);                                                                                \
	))                                                                                                      \
	TEST(FString_ParseFormatFlags, HyphenIsLeftJustify, (                                                   \
		fstring_format_type Flags = 0;                                                                      \
		string Cursor = CString("-");                                                                       \
	    FString_ParseFormatFlags(&Cursor, &Flags);                                                          \
		Assert(String_Cmp(Cursor, CString("")) == 0);                                                       \
		Assert(Flags == FSTRING_FORMAT_FLAG_LEFT_JUSTIFY);                                                  \
	))                                                                                                      \
	TEST(FString_ParseFormatFlags, PlusIsPrefixSign, (                                                      \
		fstring_format_type Flags = 0;                                                                      \
		string Cursor = CString("+");                                                                       \
	    FString_ParseFormatFlags(&Cursor, &Flags);                                                          \
		Assert(String_Cmp(Cursor, CString("")) == 0);                                                       \
		Assert(Flags == FSTRING_FORMAT_FLAG_PREFIX_SIGN);                                                   \
	))                                                                                                      \
	TEST(FString_ParseFormatFlags, SpaceIsPrefixSpace, (                                                    \
		fstring_format_type Flags = 0;                                                                      \
		string Cursor = CString(" ");                                                                       \
	    FString_ParseFormatFlags(&Cursor, &Flags);                                                          \
		Assert(String_Cmp(Cursor, CString("")) == 0);                                                       \
		Assert(Flags == FSTRING_FORMAT_FLAG_PREFIX_SPACE);                                                  \
	))                                                                                                      \
	TEST(FString_ParseFormatFlags, HashIsSpecifyRadix, (                                                    \
		fstring_format_type Flags = 0;                                                                      \
		string Cursor = CString("#");                                                                       \
	    FString_ParseFormatFlags(&Cursor, &Flags);                                                          \
		Assert(String_Cmp(Cursor, CString("")) == 0);                                                       \
		Assert(Flags == FSTRING_FORMAT_FLAG_SPECIFY_RADIX);                                                 \
	))                                                                                                      \
	TEST(FString_ParseFormatFlags, ApostropheIsSeparateGroups, (                                            \
		fstring_format_type Flags = 0;                                                                      \
		string Cursor = CString("'");                                                                       \
	    FString_ParseFormatFlags(&Cursor, &Flags);                                                          \
		Assert(String_Cmp(Cursor, CString("")) == 0);                                                       \
		Assert(Flags == FSTRING_FORMAT_FLAG_SEPARATE_GROUPS);                                               \
	))                                                                                                      \
	TEST(FString_ParseFormatFlags, ZeroIsPadWithZero, (                                                     \
		fstring_format_type Flags = 0;                                                                      \
		string Cursor = CString("0");                                                                       \
	    FString_ParseFormatFlags(&Cursor, &Flags);                                                          \
		Assert(String_Cmp(Cursor, CString("")) == 0);                                                       \
		Assert(Flags == FSTRING_FORMAT_FLAG_PAD_WITH_ZERO);                                                 \
	))                                                                                                      \
	TEST(FString_ParseFormatFlags, IgnoresDuplicates, (                                                     \
		fstring_format_type Flags = FSTRING_FORMAT_TYPE_MASK;                                               \
		string Cursor = CString("-+++00-");                                                                 \
	    fstring_format_status Result = FString_ParseFormatFlags(&Cursor, &Flags);                           \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Cursor, CString("")) == 0);                                                       \
		Assert(Flags == (FSTRING_FORMAT_TYPE_MASK | FSTRING_FORMAT_FLAG_LEFT_JUSTIFY                        \
			| FSTRING_FORMAT_FLAG_PREFIX_SIGN | FSTRING_FORMAT_FLAG_PAD_WITH_ZERO));                        \
	    Result = FString_ParseFormatFlags(NULL, NULL);                                                      \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
	))                                                                                                      \
	TEST(FString_ParseFormatType, ReportsInvalidOnInvalidChars, (                                           \
		fstring_format_type Type = -92;                                                                     \
		string Cursor = CString("hhc");                                                                     \
	    fstring_format_status Result = FString_ParseFormatType(&Cursor, &Type);                             \
		Assert(Result == FSTRING_FORMAT_TYPE_INVALID);                                                      \
		Assert(String_Cmp(Cursor, CString("c")) == 0);                                                      \
		Assert(Type == -92);                                                                                \
	    Result = FString_ParseFormatType(NULL, NULL);                                                       \
		Assert(Result == FSTRING_FORMAT_TYPE_INVALID);                                                      \
	))                                                                                                      \
	TEST(FString_ParseFormatType, KeepsExistingDataInType, (                                                \
		fstring_format_type Type = ~FSTRING_FORMAT_TYPE_MASK;                                               \
		string Cursor = CString("d");                                                                       \
	    fstring_format_status Result = FString_ParseFormatType(&Cursor, &Type);                             \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Cursor, CString("")) == 0);                                                       \
		Assert(Type == (~FSTRING_FORMAT_TYPE_MASK | FSTRING_FORMAT_TYPE_S32));                              \
		Cursor = CString("d");                                                                              \
	    Result = FString_ParseFormatType(&Cursor, NULL);                                                    \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
	))                                                                                                      \
	TEST(FString_ParseFormatType, ParsesFullType, (                                                         \
		fstring_format_type Type = 0;                                                                       \
		string Cursor = CString("wf64x");                                                                   \
	    fstring_format_status Result = FString_ParseFormatType(&Cursor, &Type);                             \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Cursor, CString("")) == 0);                                                       \
		Assert(Type == (FSTRING_FORMAT_TYPE_U64 | FSTRING_FORMAT_FLAG_INT_HEX));                            \
	))                                                                                                      \
	TEST(FString_ParseFormatType, FlagsUppercaseForRespectiveTypes, (                                       \
		fstring_format_type Type = 0;                                                                       \
		string Cursor = CString("BXFEGACSTZdLd");                                                           \
	    FString_ParseFormatType(&Cursor, &Type);                                                            \
		Assert(Type == (FSTRING_FORMAT_TYPE_U32 | FSTRING_FORMAT_FLAG_INT_BIN                               \
			| FSTRING_FORMAT_FLAG_UPPERCASE));                                                              \
		Type = 0;                                                                                           \
		FString_ParseFormatType(&Cursor, &Type);                                                            \
		Assert(Type == (FSTRING_FORMAT_TYPE_U32 | FSTRING_FORMAT_FLAG_INT_HEX                               \
			| FSTRING_FORMAT_FLAG_UPPERCASE));                                                              \
		Type = 0;                                                                                           \
		FString_ParseFormatType(&Cursor, &Type);                                                            \
		Assert(Type == (FSTRING_FORMAT_TYPE_R64 | FSTRING_FORMAT_FLAG_UPPERCASE));                          \
		Type = 0;                                                                                           \
		FString_ParseFormatType(&Cursor, &Type);                                                            \
		Assert(Type == (FSTRING_FORMAT_TYPE_R64 | FSTRING_FORMAT_FLAG_FLOAT_EXP                             \
			| FSTRING_FORMAT_FLAG_UPPERCASE));                                                              \
		Type = 0;                                                                                           \
		FString_ParseFormatType(&Cursor, &Type);                                                            \
		Assert(Type == (FSTRING_FORMAT_TYPE_R64 | FSTRING_FORMAT_FLAG_FLOAT_FIT                             \
			| FSTRING_FORMAT_FLAG_UPPERCASE));                                                              \
		Type = 0;                                                                                           \
		FString_ParseFormatType(&Cursor, &Type);                                                            \
		Assert(Type == (FSTRING_FORMAT_TYPE_R64 | FSTRING_FORMAT_FLAG_FLOAT_HEX                             \
			| FSTRING_FORMAT_FLAG_UPPERCASE));                                                              \
		Type = 0;                                                                                           \
		FString_ParseFormatType(&Cursor, &Type);                                                            \
		Assert(Type == (FSTRING_FORMAT_TYPE_CHR | FSTRING_FORMAT_FLAG_CHR_WIDE));                           \
		Type = 0;                                                                                           \
		FString_ParseFormatType(&Cursor, &Type);                                                            \
		Assert(Type == (FSTRING_FORMAT_TYPE_STR | FSTRING_FORMAT_FLAG_CHR_WIDE));                           \
		Type = 0;                                                                                           \
		FString_ParseFormatType(&Cursor, &Type);                                                            \
		Assert(Type == FSTRING_FORMAT_TYPE_B08);                                                            \
		Type = 0;                                                                                           \
	    FString_ParseFormatType(&Cursor, &Type);                                                            \
		Assert(Type == FSTRING_FORMAT_TYPE_SSIZE);                                                          \
		Type = 0;                                                                                           \
		FString_ParseFormatType(&Cursor, &Type);                                                            \
		Assert(Type == FSTRING_FORMAT_TYPE_S64);                                                            \
	))                                                                                                      \
	TEST(FString_ParseFormat, MustContainAtLeastAType, (                                                    \
		b08 UseIndexes = 2;                                                                                 \
		string Cursor = CString("%d");                                                                      \
		fstring_format Format;                                                                              \
		FString_BumpCursor(&Cursor);                                                                        \
	    fstring_format_status Result = FString_ParseFormat(&Cursor, &Format, &UseIndexes, TRUE);            \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Cursor, CString("")) == 0);                                                       \
		Assert(UseIndexes == FALSE);                                                                        \
		Assert(Format.Type == FSTRING_FORMAT_TYPE_S32);                                                     \
		Assert(Format.ValueIndex == 0);                                                                     \
		Assert(Format.Width == 0);                                                                          \
		Assert(Format.Precision == -1);                                                                     \
		Assert(String_Cmp(Format.SpecifierString, CString("%d")) == 0);                                     \
		Assert(Format.ContentLength == 0);                                                                  \
		Assert(Format.ActualWidth == 0);                                                                    \
		Cursor = CString("%d");                                                                             \
	    FString_BumpCursor(&Cursor);                                                                        \
	    Result = FString_ParseFormat(&Cursor, NULL, &UseIndexes, FALSE);                                    \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
	))                                                                                                      \
	TEST(FString_ParseFormat, ReportsIndexInconsistentWhenIndexPresentAndProhibited, (                      \
		b08 UseIndexes = FALSE;                                                                             \
		string Cursor = CString("%1$d");                                                                    \
		fstring_format Format;                                                                              \
		FString_BumpCursor(&Cursor);                                                                        \
	    fstring_format_status Result = FString_ParseFormat(&Cursor, &Format, &UseIndexes, FALSE);           \
		Assert(Result == FSTRING_FORMAT_INDEX_INCONSISTENT);                                                \
		Assert(String_Cmp(Cursor, CString("$d")) == 0);                                                     \
		Assert(UseIndexes == FALSE);                                                                        \
	))                                                                                                      \
	TEST(FString_ParseFormat, ReportsIndexInconsistentWhenIndexNotPresentAndExpected, (                     \
		b08 UseIndexes = TRUE;                                                                              \
		string Cursor = CString("%d");                                                                      \
		fstring_format Format;                                                                              \
		FString_BumpCursor(&Cursor);                                                                        \
	    fstring_format_status Result = FString_ParseFormat(&Cursor, &Format, &UseIndexes, FALSE);           \
		Assert(Result == FSTRING_FORMAT_INDEX_INCONSISTENT);                                                \
		Assert(String_Cmp(Cursor, CString("d")) == 0);                                                      \
		Assert(UseIndexes == TRUE);                                                                         \
	))                                                                                                      \
	TEST(FString_ParseFormat, MaximalCaseWithExplicitNumbers, (                                             \
		b08 UseIndexes = TRUE;                                                                              \
		string FormatStr = CString("%--++  ''##002147483647.2147483647wf64d");                              \
		string Cursor = FormatStr;                                                                          \
		fstring_format Format;                                                                              \
		FString_BumpCursor(&Cursor);                                                                        \
	    fstring_format_status Result = FString_ParseFormat(&Cursor, &Format, &UseIndexes, TRUE);            \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Cursor, CString("")) == 0);                                                       \
		Assert(UseIndexes == FALSE);                                                                        \
		Assert(Format.Type == (FSTRING_FORMAT_TYPE_S64 | FSTRING_FORMAT_FLAG_LEFT_JUSTIFY                   \
			| FSTRING_FORMAT_FLAG_PREFIX_SIGN | FSTRING_FORMAT_FLAG_PREFIX_SPACE                            \
			| FSTRING_FORMAT_FLAG_SPECIFY_RADIX | FSTRING_FORMAT_FLAG_SEPARATE_GROUPS                       \
			| FSTRING_FORMAT_FLAG_PAD_WITH_ZERO));                                                          \
		Assert(Format.ValueIndex == 0);                                                                     \
		Assert(Format.Width == 2147483647);                                                                 \
		Assert(Format.Precision == 2147483647);                                                             \
		Assert(String_Cmp(Format.SpecifierString, FormatStr) == 0);                                         \
		Assert(Format.ContentLength == 0);                                                                  \
		Assert(Format.ActualWidth == 0);                                                                    \
	))                                                                                                      \
	TEST(FString_ParseFormat, MaximalCaseWithParamNumbers, (                                                \
		b08 UseIndexes = TRUE;                                                                              \
		string FormatStr = CString("%--++  ''##00*.*wf64d");                                                \
		string Cursor = FormatStr;                                                                          \
		fstring_format Format;                                                                              \
		FString_BumpCursor(&Cursor);                                                                        \
	    fstring_format_status Result = FString_ParseFormat(&Cursor, &Format, &UseIndexes, TRUE);            \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Cursor, CString("")) == 0);                                                       \
		Assert(UseIndexes == FALSE);                                                                        \
		Assert(Format.Type == (FSTRING_FORMAT_TYPE_S64 | FSTRING_FORMAT_FLAG_LEFT_JUSTIFY                   \
			| FSTRING_FORMAT_FLAG_PREFIX_SIGN | FSTRING_FORMAT_FLAG_PREFIX_SPACE                            \
			| FSTRING_FORMAT_FLAG_SPECIFY_RADIX | FSTRING_FORMAT_FLAG_SEPARATE_GROUPS                       \
			| FSTRING_FORMAT_FLAG_PAD_WITH_ZERO | FSTRING_FORMAT_FLAG_PARAM_WIDTH                           \
		    | FSTRING_FORMAT_FLAG_PARAM_PRECISION));                                                        \
		Assert(Format.ValueIndex == 0);                                                                     \
		Assert(Format.Width == 0);                                                                          \
		Assert(Format.Precision == -1);                                                                     \
		Assert(String_Cmp(Format.SpecifierString, FormatStr) == 0);                                         \
	))                                                                                                      \
	TEST(FString_ParseFormat, MaximalCaseWithParamIndexes, (                                                \
		b08 UseIndexes = FALSE;                                                                             \
		string FormatStr = CString("%2147483647$--++  ''##00*2147483647$.*2147483647$wf64d");               \
		string Cursor = FormatStr;                                                                          \
		fstring_format Format;                                                                              \
		FString_BumpCursor(&Cursor);                                                                        \
	    fstring_format_status Result = FString_ParseFormat(&Cursor, &Format, &UseIndexes, TRUE);            \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Cursor, CString("")) == 0);                                                       \
		Assert(UseIndexes == TRUE);                                                                         \
		Assert(Format.Type == (FSTRING_FORMAT_TYPE_S64 | FSTRING_FORMAT_FLAG_LEFT_JUSTIFY                   \
			| FSTRING_FORMAT_FLAG_PREFIX_SIGN | FSTRING_FORMAT_FLAG_PREFIX_SPACE                            \
			| FSTRING_FORMAT_FLAG_SPECIFY_RADIX | FSTRING_FORMAT_FLAG_SEPARATE_GROUPS                       \
			| FSTRING_FORMAT_FLAG_PAD_WITH_ZERO | FSTRING_FORMAT_FLAG_PARAM_WIDTH                           \
		    | FSTRING_FORMAT_FLAG_PARAM_PRECISION));                                                        \
		Assert(Format.ValueIndex == 2147483647);                                                            \
		Assert(Format.Width == 2147483647);                                                                 \
		Assert(Format.Precision == 2147483647);                                                             \
		Assert(String_Cmp(Format.SpecifierString, FormatStr) == 0);                                         \
	))                                                                                                      \
	TEST(FString_ParseFormat, ImplicitPrecisionIsZero, (                                                    \
		b08 UseIndexes = FALSE;                                                                             \
		string FormatStr = CString("%.d");                                                                  \
		string Cursor = FormatStr;                                                                          \
		fstring_format Format;                                                                              \
		FString_BumpCursor(&Cursor);                                                                        \
	    fstring_format_status Result = FString_ParseFormat(&Cursor, &Format, &UseIndexes, FALSE);           \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(Format.Precision == 0);                                                                      \
	))                                                                                                      \
	TEST(FString_ParseFormat, PropagatesIntOverflow, (                                                      \
		b08 UseIndexes = TRUE;                                                                              \
		string Cursor = CString("%2147483648$d");                                                           \
		fstring_format Format;                                                                              \
		FString_BumpCursor(&Cursor);                                                                        \
	    fstring_format_status Result = FString_ParseFormat(&Cursor, &Format, &UseIndexes, TRUE);            \
		Assert(Result == FSTRING_FORMAT_INT_OVERFLOW);                                                      \
		Assert(String_Cmp(Cursor, CString("8$d")) == 0);                                                    \
		Cursor = CString("%2147483648d");                                                                   \
		FString_BumpCursor(&Cursor);                                                                        \
	    Result = FString_ParseFormat(&Cursor, &Format, &UseIndexes, TRUE);                                  \
		Assert(Result == FSTRING_FORMAT_INT_OVERFLOW);                                                      \
		Assert(String_Cmp(Cursor, CString("8d")) == 0);                                                     \
		Cursor = CString("%*2147483648$d");                                                                 \
		FString_BumpCursor(&Cursor);                                                                        \
	    Result = FString_ParseFormat(&Cursor, &Format, &UseIndexes, TRUE);                                  \
		Assert(Result == FSTRING_FORMAT_INT_OVERFLOW);                                                      \
		Assert(String_Cmp(Cursor, CString("8$d")) == 0);                                                    \
		Cursor = CString("%.2147483648d");                                                                  \
		FString_BumpCursor(&Cursor);                                                                        \
	    Result = FString_ParseFormat(&Cursor, &Format, &UseIndexes, TRUE);                                  \
		Assert(Result == FSTRING_FORMAT_INT_OVERFLOW);                                                      \
		Assert(String_Cmp(Cursor, CString("8d")) == 0);                                                     \
		Cursor = CString("%.*2147483648$d");                                                                \
		FString_BumpCursor(&Cursor);                                                                        \
	    Result = FString_ParseFormat(&Cursor, &Format, &UseIndexes, TRUE);                                  \
		Assert(Result == FSTRING_FORMAT_INT_OVERFLOW);                                                      \
		Assert(String_Cmp(Cursor, CString("8$d")) == 0);                                                    \
	))                                                                                                      \
	TEST(FString_ParseFormat, PropagatesInvalidType, (                                                      \
		b08 UseIndexes = FALSE;                                                                             \
		string Cursor = CString("%hhc");                                                                    \
		fstring_format Format;                                                                              \
		FString_BumpCursor(&Cursor);                                                                        \
	    fstring_format_status Result = FString_ParseFormat(&Cursor, &Format, &UseIndexes, TRUE);            \
		Assert(Result == FSTRING_FORMAT_TYPE_INVALID);                                                      \
		Assert(String_Cmp(Cursor, CString("c")) == 0);                                                      \
	))                                                                                                      \
	TEST(FString_ParseFormatString, AllowsNullArgumentsButWhy, (                                            \
	    fstring_format_status Result = FString_ParseFormatString(NULL, NULL);                               \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		string Cursor = CString("...%d...");                                                                \
	    Result = FString_ParseFormatString(&Cursor, NULL);                                                  \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
	))                                                                                                      \
	TEST(FString_ParseFormatString, IgnoresEscapedPercentSigns, (                                           \
		usize StackCursor = (usize) Stack_GetCursor();                                                      \
		string FormatStr = CString("%d%%%s");                                                               \
		string Cursor = FormatStr;                                                                          \
		fstring_format_list FormatList;                                                                     \
	    fstring_format_status Result = FString_ParseFormatString(&Cursor, &FormatList);                     \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Cursor, FormatStr) == 0);                                                         \
		Assert(String_Cmp(FormatList.FormatString, FormatStr) == 0);                                        \
		Assert(FormatList.FormatCount == 2);                                                                \
		Assert(FormatList.ParamCount == 2);                                                                 \
		Assert(FormatList.Formats != NULL);                                                                 \
		Assert(FormatList.ExtraDataSize == sizeof(string));                                                 \
		Assert(FormatList.ExtraData != NULL);                                                               \
		Assert(FormatList.NonFormattedTextSize == 1);                                                       \
		Assert(FormatList.TotalTextSize == 0);                                                              \
		Assert(FormatList.Formats[0].Type == FSTRING_FORMAT_TYPE_S32);                                      \
		Assert(FormatList.Formats[0].ValueIndex == 1);                                                      \
		Assert(FormatList.Formats[1].Type == FSTRING_FORMAT_TYPE_STR);                                      \
		Assert(FormatList.Formats[1].ValueIndex == 2);                                                      \
		Assert((usize) Stack_GetCursor() == StackCursor + 2 * sizeof(fstring_format) + sizeof(string));     \
	))                                                                                                      \
	TEST(FString_ParseFormatString, NoAllocationIfNoFormats, (                                              \
		usize StackCursor = (usize) Stack_GetCursor();                                                      \
		string FormatStr = CString("%%d%%%%s");                                                             \
		string Cursor = FormatStr;                                                                          \
		fstring_format_list FormatList;                                                                     \
	    fstring_format_status Result = FString_ParseFormatString(&Cursor, &FormatList);                     \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Cursor, FormatStr) == 0);                                                         \
		Assert(String_Cmp(FormatList.FormatString, FormatStr) == 0);                                        \
		Assert(FormatList.FormatCount == 0);                                                                \
		Assert(FormatList.ParamCount == 0);                                                                 \
		Assert(FormatList.Formats == NULL);                                                                 \
		Assert(FormatList.ExtraDataSize == 0);                                                              \
		Assert(FormatList.ExtraData == NULL);                                                               \
		Assert(FormatList.NonFormattedTextSize == 5);                                                       \
		Assert((usize) Stack_GetCursor() == StackCursor);                                                   \
	))                                                                                                      \
	TEST(FString_ParseFormatString, ExplicitParamIndexesAreCorrect, (                                       \
		string Cursor = CString("%2$d%1$s%4$*3$.*3$f");                                                     \
		fstring_format_list FormatList;                                                                     \
	    fstring_format_status Result = FString_ParseFormatString(&Cursor, &FormatList);                     \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(FormatList.FormatCount == 3);                                                                \
		Assert(FormatList.ParamCount == 4);                                                                 \
		Assert(FormatList.NonFormattedTextSize == 0);                                                       \
		Assert(FormatList.Formats[0].ValueIndex == 2);                                                      \
		Assert(FormatList.Formats[1].ValueIndex == 1);                                                      \
		Assert(FormatList.Formats[2].ValueIndex == 4);                                                      \
		Assert(FormatList.Formats[2].WidthIndex == 3);                                                      \
		Assert(FormatList.Formats[2].PrecisionIndex == 3);                                                  \
		Assert(String_Cmp(FormatList.Formats[0].SpecifierString, CString("%2$d")) == 0);                    \
		Assert(String_Cmp(FormatList.Formats[1].SpecifierString, CString("%1$s")) == 0);                    \
		Assert(String_Cmp(FormatList.Formats[2].SpecifierString, CString("%4$*3$.*3$f")) == 0);             \
	))                                                                                                      \
	TEST(FString_ParseFormatString, ImplicitParamIndexesAreCorrect, (                                       \
		string Cursor = CString("%d %*s %.*f %*.*x");                                                       \
		fstring_format_list FormatList;                                                                     \
	    fstring_format_status Result = FString_ParseFormatString(&Cursor, &FormatList);                     \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(FormatList.FormatCount == 4);                                                                \
		Assert(FormatList.ParamCount == 8);                                                                 \
		Assert(FormatList.Formats[0].ValueIndex == 1);                                                      \
		Assert(FormatList.Formats[1].ValueIndex == 2);                                                      \
		Assert(FormatList.Formats[1].WidthIndex == 3);                                                      \
		Assert(FormatList.Formats[2].ValueIndex == 4);                                                      \
		Assert(FormatList.Formats[2].PrecisionIndex == 5);                                                  \
		Assert(FormatList.Formats[3].ValueIndex == 6);                                                      \
		Assert(FormatList.Formats[3].WidthIndex == 7);                                                      \
		Assert(FormatList.Formats[3].PrecisionIndex == 8);                                                  \
		Assert(String_Cmp(FormatList.Formats[0].SpecifierString, CString("%d")) == 0);                      \
		Assert(String_Cmp(FormatList.Formats[1].SpecifierString, CString("%*s")) == 0);                     \
		Assert(String_Cmp(FormatList.Formats[2].SpecifierString, CString("%.*f")) == 0);                    \
		Assert(String_Cmp(FormatList.Formats[3].SpecifierString, CString("%*.*x")) == 0);                   \
	))                                                                                                      \
	TEST(FString_ParseFormatString, ReportsInconsistentIndexes, (                                           \
		string FormatStr = CString("%2$d %s %3$f");                                                         \
		string Cursor = FormatStr;                                                                          \
		fstring_format_list FormatList;                                                                     \
	    fstring_format_status Result = FString_ParseFormatString(&Cursor, &FormatList);                     \
		Assert(Result == FSTRING_FORMAT_INDEX_INCONSISTENT);                                                \
		Assert(String_Cmp(Cursor, CString("s %3$f")) == 0);                                                 \
		Cursor = CString("%d %1$s %f");                                                                     \
	    Result = FString_ParseFormatString(&Cursor, &FormatList);                                           \
		Assert(Result == FSTRING_FORMAT_INDEX_INCONSISTENT);                                                \
		Assert(String_Cmp(Cursor, CString("$s %f")) == 0);                                                  \
	))                                                                                                      \
	TEST(FString_ParseFormatString, PropagatesIntOverflow, (                                                \
		string Cursor = CString("%d %3000000000s %f");                                                      \
		fstring_format_list FormatList;                                                                     \
	    fstring_format_status Result = FString_ParseFormatString(&Cursor, &FormatList);                     \
		Assert(Result == FSTRING_FORMAT_INT_OVERFLOW);                                                      \
		Assert(String_Cmp(Cursor, CString("0s %f")) == 0);                                                  \
	))                                                                                                      \
	TEST(FString_ParseFormatString, PropagatesInvalidType, (                                                \
		string Cursor = CString("%d %y %f");                                                                \
		fstring_format_list FormatList;                                                                     \
	    fstring_format_status Result = FString_ParseFormatString(&Cursor, &FormatList);                     \
		Assert(Result == FSTRING_FORMAT_TYPE_INVALID);                                                      \
		Assert(String_Cmp(Cursor, CString("y %f")) == 0);                                                   \
	))                                                                                                      \
	TEST(FString_UpdateParamReferences, ReportsRedefinedParamsWithinSpecifier, (                            \
		string FormatStr = CString("%2$d %1$.*1$f %%");                                                     \
		fstring_format_list FormatList;                                                                     \
	    fstring_format_status Result = FString_ParseFormatString(&FormatStr, &FormatList);                  \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		usize Cursor = (usize) Stack_GetCursor();                                                           \
	    Result = FString_UpdateParamReferences(&FormatList, 1.2f, 23);                                      \
		Assert(Result == FSTRING_FORMAT_PARAM_REDEFINED);                                                   \
		Assert(String_Cmp(FormatList.FormatString, CString("%1$.*1$f")) == 0);                              \
		Assert((usize) Stack_GetCursor() == Cursor);                                                        \
	))                                                                                                      \
	TEST(FString_UpdateParamReferences, ReportsRedefinedParamsBetweenSpecifiers, (                          \
		string FormatStr = CString("%2$d %2$.*1$f %%");                                                     \
		fstring_format_list FormatList;                                                                     \
	    fstring_format_status Result = FString_ParseFormatString(&FormatStr, &FormatList);                  \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
	    Result = FString_UpdateParamReferences(&FormatList, 23, 1.2f);                                      \
		Assert(Result == FSTRING_FORMAT_PARAM_REDEFINED);                                                   \
		Assert(String_Cmp(FormatList.FormatString, CString("%2$.*1$f")) == 0);                              \
	))                                                                                                      \
	TEST(FString_UpdateParamReferences, ReportsSkippedParams, (                                             \
		string FormatStr = CString("%3$d %1$f");                                                            \
		fstring_format_list FormatList;                                                                     \
	    fstring_format_status Result = FString_ParseFormatString(&FormatStr, &FormatList);                  \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
	    usize Cursor = (usize) Stack_GetCursor();                                                           \
	    Result = FString_UpdateParamReferences(&FormatList, 23, 1.2f);                                      \
		Assert(Result == FSTRING_FORMAT_INDEX_NOT_PRESENT);                                                 \
		Assert(String_Cmp(FormatList.FormatString, FormatStr) == 0);                                        \
		Assert((usize) Stack_GetCursor() == Cursor);                                                        \
	))                                                                                                      \
	TEST(FString_UpdateParamReferences, UpdatesValuesAndReturnsValid, (                                     \
		string FormatStr = CString("%*d %s");                                                               \
		fstring_format_list FormatList;                                                                     \
	    fstring_format_status Result = FString_ParseFormatString(&FormatStr, &FormatList);                  \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
	    usize Cursor = (usize) Stack_GetCursor();                                                           \
	    Result = FString_UpdateParamReferences(&FormatList, 23, 2, CString("Hi!"));                         \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(FormatList.FormatString, FormatStr) == 0);                                        \
		Assert(FormatList.Formats[0].Value.Signed == 23);                                                   \
		Assert(FormatList.Formats[0].Width == 2);                                                           \
		Assert(FormatList.Formats[1].Value.String == FormatList.ExtraData);                                 \
		Assert(FormatList.ExtraData = FormatList.Formats[1].Value.String + 1);                              \
		Assert(String_Cmp(*FormatList.Formats[1].Value.String, CString("Hi!")) == 0);                       \
		Assert((usize) Stack_GetCursor() == Cursor);                                                        \
	))                                                                                                      \
	TEST(FString_UpdateParamReferences, ReportsInvalidStringEncoding, (                                     \
		string FormatStr = CString("%*d %ls");                                                              \
		fstring_format_list FormatList;                                                                     \
	    fstring_format_status Result = FString_ParseFormatString(&FormatStr, &FormatList);                  \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
	    usize Cursor = (usize) Stack_GetCursor();                                                           \
	    Result = FString_UpdateParamReferences(&FormatList, 23, 2, CString("Hi!"));                         \
		Assert(Result == FSTRING_FORMAT_ENCODING_INVALID);                                                  \
		Assert(String_Cmp(FormatList.FormatString, CString("%ls")) == 0);                                   \
		FormatStr = CString("%*d %s");                                                                      \
	    Result = FString_ParseFormatString(&FormatStr, &FormatList);                                        \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
	    string Param = LString(12);                                                                         \
	    Param.Encoding = STRING_ENCODING_UTF32;                                                             \
		Param.Text32[0] = 'H';                                                                              \
		Param.Text32[1] = 'i';                                                                              \
		Param.Text32[2] = '!';                                                                              \
	    Cursor = (usize) Stack_GetCursor();                                                                 \
	    Result = FString_UpdateParamReferences(&FormatList, 23, 2, Param);                                  \
		Assert(Result == FSTRING_FORMAT_ENCODING_INVALID);                                                  \
		Assert(String_Cmp(FormatList.FormatString, CString("%s")) == 0);                                    \
		Assert((usize) Stack_GetCursor() == Cursor);                                                        \
	))                                                                                                      \
	TEST(FString_UpdateParamReferences, ReportsOverflowedWidthParam, (                                      \
		string FormatStr = CString("%*d %s");                                                               \
		fstring_format_list FormatList;                                                                     \
	    fstring_format_status Result = FString_ParseFormatString(&FormatStr, &FormatList);                  \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
	    usize Cursor = (usize) Stack_GetCursor();                                                           \
	    Result = FString_UpdateParamReferences(&FormatList, 23, S32_MIN, CString("Hi!"));                   \
		Assert(Result == FSTRING_FORMAT_INT_OVERFLOW);                                                      \
		Assert(String_Cmp(FormatList.FormatString, CString("%*d")) == 0);                                   \
		Assert(FormatList.Formats[0].Value.Signed == 23);                                                   \
		Assert(FormatList.Formats[0].Width == S32_MIN);                                                     \
		Assert((usize) Stack_GetCursor() == Cursor);                                                        \
	))                                                                                                      \
	//

#endif

#endif
