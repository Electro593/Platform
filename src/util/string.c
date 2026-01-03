/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                             *
*  Author: Aria Seiler                                                        *
*                                                                             *
*  This program is in the public domain. There is no implied warranty, so     *
*  use it at your own risk.                                                   *
*                                                                             *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/// @brief This file contains string constructors and operations, including
/// string building, transcoding, parsing, and formatting. All strings are
/// represented via the `string` datastructure, which contains information about
/// its length and encoding.
///
/// All string operations currently allocate on the stack, so be sure to copy it
/// to a heap buffer for longer-term storage.
///
/// TODO:
///  - Rework printing to write to UTF32 and transcode afterward? It would
///    reduce a lot of the nastier float performance issues...
///  - Only FString & co. are tested currently. Test everything else.
///  - Finish documenting the operation functions

#ifdef INCLUDE_HEADER

/***************************************************************************\
|  SECTION: Data Types                                                      |
|---------------------------------------------------------------------------|
|  All data types required to store and represent strings.                  |
\***************************************************************************/

#ifndef SECTION_STRING_TYPES

/// @brief Represents the currently supported encoding formats for a string to
/// take on.
typedef enum string_encoding {
	/// @brief 7-bit ASCII
	STRING_ENCODING_ASCII = 0,

	/// @brief Unicode UTF-8
	STRING_ENCODING_UTF8 = 1,

	/// @brief Unicode UTF-16
	STRING_ENCODING_UTF16 = 2,

	/// @brief Unicode UTF-32
	STRING_ENCODING_UTF32 = 3,

	/// @brief Number of valid string encodings. An encoding is invalid if it's
	/// greater than or
	/// equal to this number.
	STRING_ENCODING_COUNT
} string_encoding;

/// @brief Status code for FString operations, such as parsing and indexing
/// errors.
typedef enum fstring_format_status {
	/// @brief The format was valid and parsed successfully.
	FSTRING_FORMAT_VALID,

	/// @brief A number wasn't found and thus couldn't be parsed. This will most
	/// likely be caused by
	/// a period without an asterisk or precision number following it.
	FSTRING_FORMAT_INT_NOT_PRESENT,

	/// @brief A number in the format, such as width, precision, or an index,
	/// was too large to fit
	/// within a U32. This will most likely be caused by the format extending
	/// past where it was
	/// intended to end.
	FSTRING_FORMAT_INT_OVERFLOW,

	/// @brief A param index wasn't found when it was expected to be. This
	/// occurs if the index
	/// number was present but it wasn't followed by '$'. Additionally, it can
	/// occur if not all
	/// index specifiers are used, such as if you reference 1$ and 3$ but never
	/// 2$.
	FSTRING_FORMAT_INDEX_NOT_PRESENT,

	/// @brief The usage of param indexes was inconsistent within the format
	/// string. Indexes are
	/// all-or-nothing, so either all values, variable widths, and variable
	/// precisions use
	/// them, or none do.
	FSTRING_FORMAT_INDEX_INCONSISTENT,

	/// @brief A format type didn't match a valid pattern, such as 'hhx' or
	/// 'Lc'. See
	/// `FString_ParseFormatType` for the regex of accepted types
	FSTRING_FORMAT_TYPE_INVALID,

	/// @brief An index was referenced with different types. This can occur, for
	/// example, if you use a param as both a width and a non-S32 value.
	FSTRING_FORMAT_PARAM_REDEFINED,

	/// @brief The provided output buffer was too small to contain the fully
	/// formatted output.
	FSTRING_FORMAT_BUFFER_TOO_SMALL,

	/// @brief The requested feature isn't yet implemented. Sorry!
	FSTRING_FORMAT_NOT_IMPLEMENTED,
} fstring_format_status;

/// @brief Data type and flags of a format specifier. Values
/// `FSTRING_FORMAT_SIZE_*` are used internally for the type parser state
/// machine; only `TYPE_*` and `FLAG_*` values are relevant beyond that case.
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

	/// @brief An easy barrier to check whether parsing is complete. All valid
	/// types are numbered above this, and all intermediate types are below.
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

	FSTRING_FORMAT_FLAG_CHR_BYTE = 0x0,
	FSTRING_FORMAT_FLAG_CHR_WIDE = 0x100,

	FSTRING_FORMAT_FLAG_UPPERCASE		= 0x000800,
	FSTRING_FORMAT_FLAG_LEFT_JUSTIFY	= 0x001000,
	FSTRING_FORMAT_FLAG_PAD_WITH_ZERO	= 0x002000,
	FSTRING_FORMAT_FLAG_SEPARATE_GROUPS = 0x004000,
	FSTRING_FORMAT_FLAG_SPECIFY_RADIX	= 0x008000,
	FSTRING_FORMAT_FLAG_PREFIX_SIGN		= 0x010000,
	FSTRING_FORMAT_FLAG_PREFIX_SPACE	= 0x020000,
	FSTRING_FORMAT_FLAG_NO_WRITE		= 0x040000,
} fstring_format_type;

/// @brief Stores all relevant information about a string, including its size
/// and encoding. Null terminators are not required, and null bytes can be
/// stored within the string without issue.
typedef struct string {
	/// @brief Stores the string's total number of bytes.
	usize Length;

	/// @brief The string's total number of codepoints. May be zero, in which
	/// case either the string is empty or the count needs to be recomputed.
	usize Count;

	/// @brief The string's encoding. Must be a valid value.
	string_encoding Encoding;

	/// @brief The string's data pointer. A null pointer is treated as an empty
	/// string.
	union {
		c08 *Text;
		c16 *Text16;
		c32 *Text32;
	};
} string;

/// @brief Union representing a parameter passed into `FString`. Before the
/// variadic args are interpreted, this contains the expected type of the param.
/// Afterward, it contains the param's value.
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

/// @brief Structure containing the complete definition of a format specifier:
/// type, flags, width, precision, and optionally indexes.
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
	usize  ActualWidth;
} fstring_format;

/// @brief Structure containing all formats found within a format string, as
/// well as an ordered list of parameters and, if necessary, their backing data.
/// Most fields are internal and should be ignored, but `FormatString` and
/// `TotalTextSize` can be read for error handling and buffer allocation
/// respectively.
typedef struct fstring_format_list {
	string FormatString;

	usize			FormatCount;
	usize			ParamCount;
	fstring_format *Formats;

	usize ExtraDataSize;
	vptr  ExtraData;

	/// @brief This field contains the total size of the formatted string being
	/// written.
	usize TotalTextSize;
} fstring_format_list;

#endif	// SECTION_STRING_TYPES

/***************************************************************************\
|  SECTION: Macros                                                          |
|---------------------------------------------------------------------------|
|  Helper macros for conveniently converting literal C strings.             |
\***************************************************************************/

#ifndef SECTION_STRING_MACROS

/// @brief Constructs an empty ASCII string.
/// @return The newly constructed string.
#define EString() CStringL("")

/// @brief Constructs an ASCII string from a literal C string. Preferable over
/// `CString` since it can determine the string's size via `sizeof`. Note that
/// this does not include the null terminator.
/// @param[in] LITERAL The string literal to use. It must be a literal without
/// any width or encoding modifiers.
/// @return The newly constructed string.
#define CStringL(LITERAL)                 \
	((string){                            \
		.Text     = LITERAL,              \
		.Length   = sizeof(LITERAL)-1,    \
		.Count    = sizeof(LITERAL)-1,    \
		.Encoding = STRING_ENCODING_ASCII \
	})

/// @brief Constructs a Unicode UTF-8 string from a literal C string. Note that
/// this does not include the null terminator.
/// @param[in] LITERAL The string literal to use. It must be a literal without
/// any width or encoding modifiers.
/// @return The newly constructed string.
#define CStringL_UTF8(LITERAL) CLEString(MAC_CONCAT(u8, LITERAL), sizeof(MAC_CONCAT(u8, LITERAL)) - 1, STRING_ENCODING_UTF8)

/// @brief Constructs a Unicode UTF-16 string from a literal C string. Note that
/// this does not include the null terminator.
/// @param[in] LITERAL The string literal to use. It must be a literal without
/// any width or encoding modifiers.
/// @return The newly constructed string.
#define CStringL_UTF16(LITERAL) CLEString(MAC_CONCAT(u, LITERAL), sizeof(MAC_CONCAT(u, LITERAL)) - 2, STRING_ENCODING_UTF16)

/// @brief Constructs a Unicode UTF-32 string from a literal C string. Note that
/// this does not include the null terminator.
/// @param[in] LITERAL The string literal to use. It must be a literal without
/// any width or encoding modifiers.
/// @return The newly constructed string.
#define CStringL_UTF32(LITERAL) CLEString(MAC_CONCAT(U, LITERAL), sizeof(MAC_CONCAT(U, LITERAL)) - 4, STRING_ENCODING_UTF32)

/// @brief Constructs an ASCII string from a literal C string. Preferable over
/// `CString` since it can determine the string's size via `sizeof`. Includes
/// the null terminator.
/// @param[in] LITERAL The string literal to use. It must be a literal without
/// any width or encoding modifiers.
/// @return The newly constructed string.
#define CNStringL(LITERAL) CLEString(LITERAL, sizeof(LITERAL), STRING_ENCODING_ASCII)

/// @brief Calls FString with a literal format string, without its null
/// terminator.
/// @param[in] LITERAL The literal format string to use.
/// @param[in] ... The arguments to pass into FString.
/// @return The resulting formatted string.
#define FStringL(LITERAL, ...) FString(CStringL(LITERAL), __VA_ARGS__)

/// @brief Calls FString with a literal format string, including its null
/// terminator.
/// @param[in] LITERAL The literal format string to use.
/// @param[in] ... The arguments to pass into FString.
/// @return The resulting formatted string.
#define FNStringL(LITERAL, ...) FString(CNStringL(LITERAL), __VA_ARGS__)

#endif	// SECTION_STRING_MACROS

/***************************************************************************\
|  SECTION: Function Prototypes                                             |
|---------------------------------------------------------------------------|
|  A listing of this file's function prototypes.                            |
\***************************************************************************/

#ifndef SECTION_STRING_PROTOTYPES

#define STRING_FUNCS \
	EXPORT(string, CLEString, vptr Text, usize Length, string_encoding Encoding) \
	EXPORT(string, CString,   c08 *Text) \
	EXPORT(string, HString,   heap *Heap, c08 *Text) \
	EXPORT(string, LString,   usize Length) \
	\
	EXPORT(usize,  String_Cpy,  string Dest, string Src) \
	EXPORT(usize,  String_Fill, string Dest, u32 Codepoint, usize Count) \
	\
	EXPORT(s08,    String_Cmp,     string A, string B) \
	EXPORT(s08,    String_CmpPtr,  string *A, string *B, vptr _) \
	EXPORT(usize,  String_Hash,    string S) \
	EXPORT(usize,  String_HashPtr, string *S, vptr _) \
	EXPORT(b08,    String_TryParseS32, string S, s32 *NumOut) \
	\
	EXPORT(usize,  String_BumpBytes,           string *String, usize Count) \
	EXPORT(usize,  String_BumpCodepoints,      string *String, usize Count) \
	EXPORT(u32,    String_NextCodepoint,       string *String) \
	EXPORT(u32,    String_PeekCodepoint,       string *String) \
	EXPORT(usize,  String_WriteCodepoint,      string String, u32 Codepoint) \
	EXPORT(void,   String_Recount,             string *String) \
	EXPORT(usize,  String_GetCount,            string *String) \
	EXPORT(usize,  String_GetCodepointLength,  u32 Codepoint, string_encoding Encoding) \
	EXPORT(usize,  String_GetTranscodedLength, string String, string_encoding Encoding) \
	\
	INTERN(fstring_format_status, FString_ParseFormatInt,         string *FormatCursor, s32 *IntOut) \
	INTERN(fstring_format_status, FString_ParseFormatIndex,       string *FormatCursor, s32 *IndexOut, b08 *UseIndexes, b08 SetIndexUsage) \
	INTERN(fstring_format_status, FString_ParseFormatFlags,       string *FormatCursor, fstring_format_type *FlagsOut) \
	INTERN(fstring_format_status, FString_ParseFormatType,        string *FormatCursor, fstring_format_type *TypeOut) \
	INTERN(fstring_format_status, FString_ParseFormat,            string *FormatCursor, fstring_format *FormatOut, b08 *UseIndexes, b08 SetIndexUsage) \
	INTERN(fstring_format_status, FString_ParseFormatString,      string *FormatCursor, fstring_format_list *FormatListOut) \
	INTERN(fstring_format_status, FVString_UpdateParamReferences, fstring_format_list *FormatList, va_list Args) \
	INTERN(fstring_format_status, FString_UpdateParamReferences,  fstring_format_list *FormatList, ...) \
	INTERN(fstring_format_status, FString_WriteString,            fstring_format *Format, string Buffer) \
	INTERN(fstring_format_status, FString_WriteChar,              fstring_format *Format, string Buffer) \
	INTERN(fstring_format_status, FString_WriteBool,              fstring_format *Format, string Buffer) \
	INTERN(fstring_format_status, FString_WriteUnsigned,          fstring_format *Format, string Buffer) \
	INTERN(fstring_format_status, FString_WritePointer,           fstring_format *Format, string Buffer) \
	INTERN(fstring_format_status, FString_WriteSigned,            fstring_format *Format, string Buffer) \
	INTERN(fstring_format_status, FString_WriteHexFloat,          fstring_format *Format, string Buffer) \
	INTERN(fstring_format_status, FString_WriteFloat,             fstring_format *Format, string Buffer) \
	INTERN(fstring_format_status, FString_WriteFormat,            fstring_format *Format, string Buffer) \
	INTERN(fstring_format_status, FString_WriteFormats,           fstring_format_list *FormatList, string Buffer) \
	EXPORT(string,                FVString,                       string Format, va_list Args) \
	EXPORT(string,                FString,                        string Format, ...) \
	//

#endif	// SECTION_STRING_PROTOTYPES

#endif	// INCLUDE_HEADER

#ifdef INCLUDE_SOURCE

/***************************************************************************\
|  SECTION: String Construction                                             |
|---------------------------------------------------------------------------|
|  Handles creating and allocating strings.                                 |
\***************************************************************************/

#ifndef SECTION_STRING_CONSTRUCTION

/// @brief Constructs a new string given a text pointer, length, and encoding.
/// Count is computed based on the encoding. Make sure `Length` is no larger
/// than `Text`'s allocated size.
/// @param Text A pointer to the string's text. If null, the string will be
/// considered empty.
/// @param Length The string's length. Make sure this is correct to avoid
/// segfaults.
/// @param Encoding The string's encoding. Must be valid.
/// @return The newly constructed string.
internal string
CLEString(vptr Text, usize Length, string_encoding Encoding)
{
	Assert(Encoding < STRING_ENCODING_COUNT);

	string String;
	String.Text		= Text;
	String.Encoding = Encoding;
	String.Length	= Length;

	String_Recount(&String);
	return String;
}

/// @brief Constructs a new string given a null-terminated ASCII C string. The
/// string's length is the number of bytes up to, but not including, the first
/// null byte encountered. This may segfault if there isn't a null terminator.
/// @param Text A pointer to the C string to use. Will be interpreted as ASCII.
/// @return The newly constructed string.
internal string
CString(c08 *Text)
{
	if (!Text) return EString();
	usize Length = Mem_BytesUntil((u08 *) Text, 0);
	return CLEString(Text, Length, STRING_ENCODING_ASCII);
}

/// @brief Same as `CString`, except the new string's data is allocated on the
/// heap and copied in from the provided text.
/// @param Text A pointer to the C string to use. Will be interpreted as ASCII.
/// @return The newly constructed string.
internal string
HString(heap *Heap, c08 *Text)
{
	usize Length = Mem_BytesUntil((u08 *) Text, 0);
	c08	 *HText	 = Heap_AllocateA(Heap, Length);
	return CLEString(HText, Length, STRING_ENCODING_ASCII);
}

/// @brief Constructs a new ASCII string with a stack-allocated buffer. The
/// buffer will not be initialized.
/// @param Length The number of bytes the new string should contain.
/// @return The newly constructed string.
internal string
LString(usize Length)
{
	c08 *Text = Stack_Allocate(Length);
	return CLEString(Text, Length, STRING_ENCODING_ASCII);
}

#endif	// SECTION_STRING_CONSTRUCTION

/***************************************************************************\
|  SECTION: String Operations                                               |
|---------------------------------------------------------------------------|
|  Handles parsing, combining, and modifying strings.                       |
\***************************************************************************/

#ifndef SECTION_STRING_OPERATIONS

/// @brief Copy a string's entire contents into another, respecting encoding.
/// @param Dest The string to copy into.
/// @param Src The string to copy from.
/// @return The number of copied bytes.
internal usize
String_Cpy(string Dest, string Src)
{
	usize Written = 0;
	while (Dest.Length && Src.Length) {
		u32	  Codepoint	 = String_NextCodepoint(&Src);
		usize Delta		 = String_WriteCodepoint(Dest, Codepoint);
		Written			+= String_BumpBytes(&Dest, Delta);
	}
	return Written;
}

/// @brief Fill a string with a repeated codepoint, starting at index 0.
/// @param Dest The string to fill into.
/// @param Codepoint The codepoint to write.
/// @param Count The number of codepoints to fill.
/// @return The number of filled bytes.
internal usize
String_Fill(string Dest, u32 Codepoint, usize Count)
{
	usize Written = 0;
	for (usize I = 0; I < Count && Dest.Length; I++) {
		usize Delta	 = String_WriteCodepoint(Dest, Codepoint);
		Written		+= String_BumpBytes(&Dest, Delta);
	}
	return Written;
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

#endif	// SECTION_STRING_OPERATIONS

/***************************************************************************\
|  SECTION: Encoding Operations                                             |
|---------------------------------------------------------------------------|
|  Handles interpreting codepoints and transcoding.                         |
\***************************************************************************/

#ifndef SECTION_STRING_ENCODING

/// @brief Bumps the string by the provided amount of bytes, up to the end of
/// the string. This includes increasing the text pointer based on the encoding
/// and reducing the length accordingly. Note that this does not consider
/// encoding, and bumping a string by an invalid amount for its encoding (such
/// as bumping a UTF-32 string by 2 bytes) may result in an invalid string. You
/// will also need to recompute the string's count afterward.
/// @param String The string to be bumped.
/// @param Amount The amount of bytes to bump the string by.
/// @return The amount the string was actually bumped by. Zero if the string is
/// invalid.
internal usize
String_BumpBytes(string *String, usize Amount)
{
	if (!String || !String->Text) return 0;
	if (String->Length < Amount) Amount = String->Length;
	String->Length -= Amount;
	String->Text   += Amount;
	String->Count	= 0;
	return Amount;
}

/// @brief Bumps the string by the provided amount of codepoints, based on its
/// encoding, up to the end of the string. This includes increasing the text
/// pointer based on the encoding and reducing the length and count accordingly.
/// @param String The string to be bumped.
/// @param Amount The amount of codepoints to bump the string by.
/// @return The amount the string was actually bumped by. Zero if the string is
/// invalid.
internal usize
String_BumpCodepoints(string *String, usize Amount)
{
	if (!String || !String->Text) return 0;
	usize I = 0;
	for (; I < Amount && String->Length; I++) String_NextCodepoint(String);
	return I;
}

/// @brief Reads the next codepoint from a string, based on its encoding, and
/// returns it. Then bumps the string.
/// @param[in,out] String A pointer to the string being read. Will be bumped
/// past the character being read.
/// @return A unicode codepoint equivalent to the character that was read. If
/// the codepoint was invalid, such as being a surrogate half or malformed
/// UTF-8, 0xFFFD is returned instead.
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
							Delta	  = 3;
							Codepoint = ((B1 & 0x0F) << 12)
									  | ((B2 & 0x3F) << 6)
									  | ((B3 & 0x3F) << 0);
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
			B1	  = String->Length < 2 ? 0 : (u16) String->Text16[0];
			B2	  = String->Length < 4 ? 0 : (u16) String->Text16[1];
			Delta = 2;
			if (String->Length < 2) Delta = String->Length;
			else if (B1 >= 0xD800 && B1 <= 0xDFFF) {
				if (B1 <= 0xDBFF && B2 >= 0xDC00 && B2 <= 0xDFFF) {
					Codepoint = 0x10000 + ((B1 - 0xD800) << 10) + (B2 - 0xDC00);
					Delta	  = 4;
				}
			} else {
				Codepoint = B1;
			}
			break;

		case STRING_ENCODING_UTF32:
			B1	  = String->Length < 4 ? 0 : (u32) String->Text32[0];
			Delta = 4;
			if (String->Length < 4) Delta = String->Length;
			else if (B1 <= 0x10FFFF && (B1 < 0xD800 || B1 > 0xDFFF))
				Codepoint = B1;
			break;

		default: Assert(FALSE, "Unknown encoding format"); return 0;
	}

	String_BumpBytes(String, Delta);
	if (String->Count) String->Count--;
	return Codepoint;
}

/// @brief Reads the next codepoint from a string, based on its encoding, and
/// returns it.
/// @param[in] String A pointer to the string being read.
/// @return A unicode codepoint equivalent to the character that was read. If
/// the codepoint was invalid, such as being a surrogate half or malformed
/// UTF-8, 0xFFFD is returned instead.
internal u32
String_PeekCodepoint(string *String)
{
	if (!String) return 0;
	string Dummy = *String;
	return String_NextCodepoint(&Dummy);
}

/// @brief Attempts to convert the provided codepoint into a character sequence
/// based on the target encoding and write it into the string.
/// @param[in] String The string to be written into. If it's not large enough to
/// contain the encoded character sequence, it remains unchanged and the
/// necessary size is returned.
/// @param[in] Codepoint The codepoint to encode into the string. If this is
/// invalid, such as being a surrogate half or greater than `0x10FFFF`, it will
/// be converted into `0xFFFD` before being encoded.
/// @return The number of characters that were written. If `String` is too
/// small, this is the number of characters that would have been written.
internal usize
String_WriteCodepoint(string String, u32 Codepoint)
{
	usize Length = String.Text ? String.Length : 0;
	if (Codepoint > 0x10FFFF || (Codepoint >= 0xD800 && Codepoint <= 0xDFFF))
		Codepoint = 0xFFFD;

	switch (String.Encoding) {
		case STRING_ENCODING_ASCII:
			if (Codepoint <= 0x7F) {
				if (Length >= 1) String.Text[0] = Codepoint;
				return 1;
			}
			return 0;

		case STRING_ENCODING_UTF8:
			if (Codepoint <= 0x7F) {
				if (Length >= 1) String.Text[0] = Codepoint & 0x7F;
				return 1;
			} else if (Codepoint <= 0x7FF) {
				if (Length >= 2) {
					String.Text[0] = 0xC0 | ((Codepoint >> 6) & 0x1F);
					String.Text[1] = 0x80 | (Codepoint & 0x3F);
				}
				return 2;
			} else if (Codepoint <= 0xFFFF) {
				if (Length >= 3) {
					String.Text[0] = 0xE0 | ((Codepoint >> 12) & 0x0F);
					String.Text[1] = 0x80 | ((Codepoint >> 6) & 0x3F);
					String.Text[2] = 0x80 | (Codepoint & 0x3F);
				}
				return 3;
			} else {
				if (Length >= 4) {
					String.Text[0] = 0xF0 | ((Codepoint >> 18) & 0x07);
					String.Text[1] = 0x80 | ((Codepoint >> 12) & 0x3F);
					String.Text[2] = 0x80 | ((Codepoint >> 6) & 0x3F);
					String.Text[3] = 0x80 | (Codepoint & 0x3F);
				}
				return 4;
			}

		case STRING_ENCODING_UTF16:
			if (Codepoint <= 0xFFFF) {
				if (Length >= 2) String.Text16[0] = Codepoint;
				return 2;
			} else {
				if (Length >= 4) {
					Codepoint		 -= 0x10000;
					String.Text16[0]  = 0xD800 | ((Codepoint >> 10) & 0x3FF);
					String.Text16[1]  = 0xDC00 | (Codepoint & 0x3FF);
				}
				return 4;
			}

		case STRING_ENCODING_UTF32:
			if (Length >= 4) String.Text32[0] = Codepoint;
			return 4;

		default: Assert(FALSE, "Unknown encoding format"); return 0;
	}
}

/// @brief Recomputes a string's count based on its encoding, text, and length.
/// Only count is overwritten. Invalid code sequences are counted as if they
/// were encoded as error replacement codepoints.
/// @param String The string to recount.
internal void
String_Recount(string *String)
{
	if (!String) return;
	if (!String->Text || !String->Length) {
		String->Count = 0;
		return;
	}
	if (String->Encoding == STRING_ENCODING_ASCII) {
		String->Count = String->Length;
		return;
	}

	String->Count = 0;
	string Cursor = *String;
	while (Cursor.Length) {
		String_NextCodepoint(&Cursor);
		String->Count++;
	}
}

/// @brief Retrieves a string's count, and recomputes it if necessary.
/// @param String The string to retrieve from.
/// @return The string's count.
internal usize
String_GetCount(string *String)
{
	if (!String) return 0;
	if (!String->Count) String_Recount(String);
	return String->Count;
}

/// @brief Queries the number of bytes a given codepoint will take up in an
/// encoding.
/// @param Codepoint The codepoint to query the length of.
/// @param Encoding The encoding to convert the codepoint into.
/// @return The number of bytes necessary to represent the codepoint in the
/// provided encoding.
internal usize
String_GetCodepointLength(u32 Codepoint, string_encoding Encoding)
{
	return String_WriteCodepoint((string) { .Encoding = Encoding }, Codepoint);
}

/// @brief Query what the length of a string would be if it was transcoded into
/// another format.
/// @param String The string to query.
/// @param Encoding The encoding to check the length of.
/// @return The number of bytes the transcoded string would require.
internal usize
String_GetTranscodedLength(string String, string_encoding Encoding)
{
	if (!String.Text) return 0;
	string EncodingStr	 = EString();
	EncodingStr.Encoding = Encoding;

	usize Length = 0;
	while (String.Length) {
		u32 Codepoint  = String_NextCodepoint(&String);
		Length		  += String_GetCodepointLength(Codepoint, Encoding);
	}
	return Length;
}

#endif	// SECTION_STRING_ENCODING

/***************************************************************************\
|  SECTION: Formatting Operations                                           |
|---------------------------------------------------------------------------|
|  Handles interpreting primitives as strings and string formatting.        |
\***************************************************************************/

#ifndef SECTION_STRING_FORMATTING

/// @brief String descriptions for each format status, used for logging errors.
global string FStringFormatStatusDescriptions[] = {
	[FSTRING_FORMAT_VALID] = CStringL("The format specifier was valid"),
	[FSTRING_FORMAT_INT_NOT_PRESENT] = CStringL("Expected a number"),
	[FSTRING_FORMAT_INT_OVERFLOW]	 = CStringL("Number was too large"),
	[FSTRING_FORMAT_INDEX_NOT_PRESENT] =
		CStringL("Not all parameters were referenced"),
	[FSTRING_FORMAT_INDEX_INCONSISTENT] = CStringL(
		"Format strings cannot contain both indexed and non-indexed parameters"
	),
	[FSTRING_FORMAT_TYPE_INVALID]	 = CStringL("Invalid format type"),
	[FSTRING_FORMAT_PARAM_REDEFINED] = CStringL(
		"Format strings cannot specify an indexed parameter to be multiple "
		"types"
	),
	[FSTRING_FORMAT_BUFFER_TOO_SMALL] =
		CStringL("The provided output buffer was too small"),
	[FSTRING_FORMAT_NOT_IMPLEMENTED] =
		CStringL("The requested format isn't implemented yet"),
};

#define S(C, TYPE) [C - '1'] = FSTRING_FORMAT_##TYPE
/// @brief State machine for format type parsing. Each character is mapped into
/// a lookup table in the range '1'..'z'. Since this is a static table, it's
/// initialized to zero, so any invalid transition results in the invalid type
/// 0. At most 5 iterations are required to completely parse a type, such as
/// `wf64`, but in practice most will only require 1-3.
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
		S('s', TYPE_STR),
		S('S', TYPE_STR),
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
		S('s', TYPE_STR),
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

/// @brief Parse an unsigned decimal int from a format specifier, such as a
/// width, precision, or param index, and report whether it was valid, invalid,
/// or overflowed.
///
/// Regex: `[1-9][0-9]*`
/// @param[in,out] FormatCursor A cursor potentially pointing to the beginning
/// of a number in the format specifier. After return, this will point to the
/// first character after the number or the first invalid character encountered.
/// @param[out] IntOut A pointer to the resulting parsed int. Will only be
/// modified on success. May be null.
/// @return
/// - `FSTRING_FORMAT_VALID`: The number was parsed successfully.
///
/// - `FSTRING_FORMAT_INT_OVERFLOW`: The parsed number exceeds an s32.
///
/// - `FSTRING_FORMAT_INT_NOT_PRESENT`: No number was found, or the number
/// started with 0.
internal fstring_format_status
FString_ParseFormatInt(string *FormatCursor, s32 *IntOut)
{
	// If the first character isn't a nonzero digit, fail.
	u32 Digit = String_PeekCodepoint(FormatCursor) - '1';
	if (Digit >= 9) return FSTRING_FORMAT_INT_NOT_PRESENT;

	// Otherwise, iterate until we hit a non-digit.
	s32 Int = 0;
	while ((Digit = String_PeekCodepoint(FormatCursor) - '0') < 10) {
		if (Int > (0x7FFFFFFF - Digit) / 10) return FSTRING_FORMAT_INT_OVERFLOW;
		Int = Int * 10 + Digit;
		String_BumpCodepoints(FormatCursor, 1);
	}

	if (IntOut) *IntOut = Int;
	return FSTRING_FORMAT_VALID;
}

/// @brief Extract a param index from a format specifier, and handle index usage
/// consistency accordingly. Note that param indexes cannot be 0.
///
/// Regex: `[1-9][0-9]*\\$`
/// @param[in,out] FormatCursor A cursor potentially pointing to the beginning
/// of a parameter index in the format specifier. After return, this will point
/// to the first character after the index on success, the first character after
/// the number on `FSTRING_FORMAT_INDEX_INVALID`, or the first invalid character
/// encountered otherwise.
/// @param[out] IndexOut A pointer to the resulting parsed index. Will be
/// modified as long as a valid number is parsed, regardless of whether the
/// index is valid. May be null.
/// @param[in,out] UseIndexes A pointer to a boolean dictating whether or not to
/// use indexes. If `SetIndexUsage` is true, this will be updated with whether
/// or not the index is valid, and parsing will succeed regardless of whether a
/// valid index was parsed. Otherwise, success is determined by whether index
/// usage matches the presence of an index. Cannot be null.
/// @param[in] SetIndexUsage A boolean indicating whether to modify or respect
/// the value in `UseIndexes`.
/// @return
/// - `FSTRING_FORMAT_VALID`: The index was successfully parsed.
///
/// - `FSTRING_FORMAT_INDEX_INCONSISTENT`: The index's presence doesn't match
/// the value in `UseIndexes` and `SetIndexUsage` is true.
///
/// - `FSTRING_FORMAT_INDEX_NOT_PRESENT`: A number was found, but it wasn't an
/// index.
///
/// - `FSTRING_FORMAT_INT_OVERFLOW`: A number was found, but it was too large.
internal fstring_format_status
FString_ParseFormatIndex(
	string *FormatCursor,
	s32	   *IndexOut,
	b08	   *UseIndexes,
	b08		SetIndexUsage
)
{
	Assert(UseIndexes);

	s32 Index;

	fstring_format_status Status = FString_ParseFormatInt(FormatCursor, &Index);

	// Propagate the overflow error
	if (Status == FSTRING_FORMAT_INT_OVERFLOW) return Status;

	// There's no int, so we'll either report that we're not using indexes, or
	// fail.
	if (Status == FSTRING_FORMAT_INT_NOT_PRESENT) {
		if (SetIndexUsage) *UseIndexes = FALSE;
		else if (*UseIndexes) return FSTRING_FORMAT_INDEX_INCONSISTENT;
		return FSTRING_FORMAT_VALID;
	}

	// Update IndexOut since the number is valid
	if (IndexOut) *IndexOut = Index;

	// This is an index, so we'll either report that we're using indexes or
	// fail.
	if (String_PeekCodepoint(FormatCursor) == '$') {
		if (SetIndexUsage) *UseIndexes = TRUE;
		else if (!*UseIndexes) return FSTRING_FORMAT_INDEX_INCONSISTENT;
		String_BumpCodepoints(FormatCursor, 1);
		return FSTRING_FORMAT_VALID;
	}

	// This is a number, but not an index, so it's invalid.
	return FSTRING_FORMAT_INDEX_NOT_PRESENT;
}

/// @brief Extract flags from a format specifier, such as alignment, signage,
/// and notation. Flags may be repeated, though you should try not to, and
/// duplicates will be ignored.
///
/// Regex: `[\\-+ #'0_]*`
/// @param[in,out] FormatCursor A cursor potentially pointing to the beginning
/// of the list of flags in a format specifier. After return, this will point to
/// the first character after the flags or the first invalid character
/// encountered.
/// @param[out] FlagsOut A pointer to an enum representing the specifier's
/// flags. Only the flag bits will be modified, so it can point to an existing
/// specifier type for efficient storage. Cannot be null.
/// @return `FSTRING_FORMAT_VALID`.
internal fstring_format_status
FString_ParseFormatFlags(string *FormatCursor, fstring_format_type *FlagsOut)
{
	// Run through and see if we find any flags. We'll accept duplicates to be
	// nice.
	while (1) {
		switch (String_PeekCodepoint(FormatCursor)) {
			case '-' : *FlagsOut |= FSTRING_FORMAT_FLAG_LEFT_JUSTIFY; break;
			case '+' : *FlagsOut |= FSTRING_FORMAT_FLAG_PREFIX_SIGN; break;
			case ' ' : *FlagsOut |= FSTRING_FORMAT_FLAG_PREFIX_SPACE; break;
			case '#' : *FlagsOut |= FSTRING_FORMAT_FLAG_SPECIFY_RADIX; break;
			case '\'': *FlagsOut |= FSTRING_FORMAT_FLAG_SEPARATE_GROUPS; break;
			case '0' : *FlagsOut |= FSTRING_FORMAT_FLAG_PAD_WITH_ZERO; break;
			case '_' : *FlagsOut |= FSTRING_FORMAT_FLAG_NO_WRITE; break;

			// If we encounter anything else, we're done parsing the flags.
			default	 : return FSTRING_FORMAT_VALID;
		}

		String_BumpCodepoints(FormatCursor, 1);
	}
}

/// @brief Traverse the state machine to extract the format specifier's type and
/// size.
///
/// Regex:
/// `(?:hh|ll|[hlLqtjzZ]|wf?(?:8|16|32|64))?[dibBouxX]|L?[fFeEgGaA]|l?[cs]|[hl]?n|[CSpTm]`
/// @param [in,out] FormatCursor A cursor potentially pointing to the beginning
/// of the type portion of the format specifier, such as "lld" or "hX". After
/// return, this will point to the first character after the specifier or the
/// first invalid character encountered.
/// @param [out] TypeOut A pointer to the resulting format type. Will only be
/// modified on success, and only the bits within `FSTRING_FORMAT_TYPE_MASK`
/// will be modified. May be null.
/// @return
/// - `FSTRING_FORMAT_VALID`: The format type was parsed successfully.
///
/// - `FSTRING_FORMAT_TYPE_INVALID`: The string doesn't match any type
/// specifier.
internal fstring_format_status
FString_ParseFormatType(string *FormatCursor, fstring_format_type *TypeOut)
{
	fstring_format_type State = FSTRING_FORMAT_SIZE_INITIAL;
	while (State < FSTRING_FORMAT_SIZE_DONE) {
		u32 Transition = String_PeekCodepoint(FormatCursor) - '1';

		// Get the next state from the transition table. All invalid transitions
		// return 0.
		State = Transition < 74
				  ? FStringFormatTypeStateMachine[State][Transition]
				  : 0;
		if (!State) return FSTRING_FORMAT_TYPE_INVALID;

		String_BumpCodepoints(FormatCursor, 1);
	}

	if (TypeOut) *TypeOut = (*TypeOut & ~FSTRING_FORMAT_TYPE_MASK) | State;
	return FSTRING_FORMAT_VALID;
}

/// @brief Parse a format specifier into a data structure, and report whether it
/// is valid or not.
/// @param[in,out] FormatCursor A cursor pointing to the beginning of a format
/// specifier. After return, this will point to the first character after the
/// specifier or the first invalid character encountered.
/// @param[out] FormatOut A pointer to a format structure representing the
/// specifier's type, flags, width, precision, and potentially param indexes.
/// May be null.
/// @param[in,out] UseIndexes A pointer to a boolean dictating whether or not to
/// use indexes. If `SetIndexUsage` is true, this will be updated with whether
/// or not indexes are used in this specifier. Otherwise, parsing will fail if
/// index usage does not match the value in `UseIndexes`. Cannot be null.
/// @param[in] SetIndexUsage A boolean indicating whether to modify or respect
/// the value in `UseIndexes`.
/// @return
/// - `FSTRING_FORMAT_VALID`: The format was successfully parsed.
///
/// - `FSTRING_FORMAT_INT_OVERFLOW`: A width, precision, or param index was too
/// large.
///
/// - `FSTRING_FORMAT_INDEX_INCONSISTENT`: Only if `SetIndexUsage` is false:
/// `UseIndexes` was true and a param's index was missing, or `UseIndexes` was
/// false and a param index was found.
///
/// - `FSTRING_FORMAT_TYPE_INVALID`: The format's size and type had an invalid
/// pattern.
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
	Format.SpecifierString.Text		= FormatCursor->Text;
	Format.SpecifierString.Count	= 0;
	Format.SpecifierString.Encoding = FormatCursor->Encoding;
	String_BumpCodepoints(FormatCursor, 1);

	// First try to parse the first index
	Status = FString_ParseFormatIndex(
		FormatCursor,
		&Format.ValueIndex,
		UseIndexes,
		SetIndexUsage
	);
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
	if (String_PeekCodepoint(FormatCursor) == '*') {
		String_BumpCodepoints(FormatCursor, 1);

		Status = FString_ParseFormatIndex(
			FormatCursor,
			&Format.WidthIndex,
			UseIndexes,
			FALSE
		);
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
	if (String_PeekCodepoint(FormatCursor) == '.') {
		String_BumpCodepoints(FormatCursor, 1);

		// Check if it's provided by a param
		if (String_PeekCodepoint(FormatCursor) == '*') {
			String_BumpCodepoints(FormatCursor, 1);

			Status = FString_ParseFormatIndex(
				FormatCursor,
				&Format.PrecisionIndex,
				UseIndexes,
				FALSE
			);
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

/// @brief Parse a format string into a data structure containing a list of
/// formats. The arrays are allocated via `Stack_Allocate`.
/// @param[in,out] FormatString A cursor pointing to the beginning of a format
/// string. After return, this will point to the original string or the first
/// invalid character encountered.
/// @param[out] FormatListOut A pointer to the resulting data structure with the
/// format list.
/// @return
/// - `FSTRING_FORMAT_VALID`: All formats were successfully parsed.
///
/// - `FSTRING_FORMAT_INT_OVERFLOW`: A width, precision, or param index was too
/// large.
///
/// - `FSTRING_FORMAT_INDEX_INCONSISTENT`: At least one, but not all params
/// specified an index.
///
/// - `FSTRING_FORMAT_TYPE_INVALID`: A format's size and type had an invalid
/// pattern.
internal fstring_format_status
FString_ParseFormatString(
	string				*FormatCursor,
	fstring_format_list *FormatListOut
)
{
	if (FormatListOut) Mem_Set(FormatListOut, 0, sizeof(fstring_format_list));
	if (!FormatCursor || !FormatCursor->Text || !FormatCursor->Length)
		return FSTRING_FORMAT_VALID;

	fstring_format_status Status	 = FSTRING_FORMAT_VALID;
	fstring_format_list	  FormatList = { 0 };
	Mem_Set(&FormatList, 0, sizeof(fstring_format_list));
	FormatList.FormatString = *FormatCursor;

	// First we have to iterate through to determine the number of formats and
	// params.
	u32 C;
	b08 UseIndexes = FALSE;
	while ((C = String_PeekCodepoint(FormatCursor))) {
		if (C == '%') {
			string Peeker = *FormatCursor;
			String_BumpCodepoints(&Peeker, 1);

			if (String_PeekCodepoint(&Peeker) != '%') {
				fstring_format Format;

				// As an optimization, we only need to parse the formats if
				// indexes or used, or to determine whether params are used in
				// the first place.
				b08 First = FormatList.FormatCount == 0;
				Status	  = FString_ParseFormat(
					   FormatCursor,
					   &Format,
					   &UseIndexes,
					   First
				   );
				if (Status != FSTRING_FORMAT_VALID) goto end;

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
					if (Format.Type & FSTRING_FORMAT_FLAG_PARAM_WIDTH)
						FormatList.ParamCount++;
					if (Format.Type & FSTRING_FORMAT_FLAG_PARAM_PRECISION)
						FormatList.ParamCount++;
				}

				// We also need to update if we'll be using any extra data
				if ((Format.Type & FSTRING_FORMAT_TYPE_MASK)
					== FSTRING_FORMAT_TYPE_STR)
					FormatList.ExtraDataSize += sizeof(string);

				continue;
			} else {
				*FormatCursor = Peeker;
			}
		}

		String_BumpCodepoints(FormatCursor, 1);
	}

	// We can go ahead and early-out if there weren't any formats
	if (!FormatList.FormatCount) goto end;

	FormatList.Formats =
		Stack_Allocate(FormatList.FormatCount * sizeof(fstring_format));
	FormatList.ExtraData = Stack_Allocate(FormatList.ExtraDataSize);

	// Next, we have to run through again and this time, we'll actually save the
	// formats.
	usize			ParamIndex = 1;
	fstring_format *Format	   = FormatList.Formats;
	*FormatCursor			   = FormatList.FormatString;
	while ((C = String_PeekCodepoint(FormatCursor))) {
		if (C == '%') {
			string Peeker = *FormatCursor;
			String_BumpCodepoints(&Peeker, 1);

			if (String_PeekCodepoint(&Peeker) != '%') {
				Status = FString_ParseFormat(
					FormatCursor,
					Format,
					&UseIndexes,
					FALSE
				);
				if (Status != FSTRING_FORMAT_VALID) goto end;

				// Specify the sequential indexes explicitly to make later steps
				// easier
				if (!UseIndexes) {
					Format->ValueIndex = ParamIndex++;

					if (Format->Type & FSTRING_FORMAT_FLAG_PARAM_WIDTH)
						Format->WidthIndex = ParamIndex++;

					if (Format->Type & FSTRING_FORMAT_FLAG_PARAM_PRECISION)
						Format->PrecisionIndex = ParamIndex++;
				}

				Format++;
				continue;
			} else {
				*FormatCursor = Peeker;
			}
		}

		String_BumpCodepoints(FormatCursor, 1);
	}

end:
	if (FormatListOut) *FormatListOut = FormatList;
	if (Status == FSTRING_FORMAT_VALID) *FormatCursor = FormatList.FormatString;
	return Status;
}

/// @brief Uses the formats within a format list, alongside a va_list, to read
/// the params from the va_list and write them into the FormatList's params.
/// This includes updating the extra data for strings.
/// @param[in,out] FormatList A pointer to the format list to be used to
/// interpret the params. Cannot be null. On error, FormatCount will report the
/// format index where the error occurred.
/// @param[in] Args The va_list to source the params from. Note that you must
/// call `VA_End` before using this again.
/// @return
/// - `FSTRING_FORMAT_VALID`: The params were read and updated successfully.
///
/// - `FSTRING_FORMAT_PARAM_REDEFINED`: An index was defined as multiple types,
/// which is disallowed. `FormatList.FormatString` will span the specifier where
/// this occurred.
///
/// - `FSTRING_FORMAT_INDEX_NOT_PRESENT`: At least one param was not referenced
/// by any specifier. `FormatList.FormatString` will span the entire format
/// string.
///
/// - `FSTRING_FORMAT_INT_OVERFLOW`: A width param was -2^31, which overflows on
/// absolute value. `FormatList.FormatString` will span the specifier where this
/// occurred.
internal fstring_format_status
FVString_UpdateParamReferences(fstring_format_list *FormatList, va_list Args)
{
	Assert(FormatList);
	fstring_format *Format;

	vptr ExtraData = FormatList->ExtraData;

	fstring_format_type IndexType = FSTRING_FORMAT_TYPE_S32;
	fstring_format_type Mask	  = FSTRING_FORMAT_TYPE_MASK;

	// First, we need to order the params by collecting them into a temporary
	// list
	Stack_Push();
	fstring_param *Params =
		Stack_Allocate(FormatList->ParamCount * sizeof(fstring_param));
	Mem_Set(Params, 0, FormatList->ParamCount * sizeof(fstring_param));

	for (usize I = 0; I < FormatList->FormatCount; I++) {
		Format = &FormatList->Formats[I];

		// It's an error to re-reference a param under a different type.
		// Architectures can depend on the size of args you provide to determine
		// the next ones, so redefining a type can make later param types
		// ambiguous.
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
				&& (Params[Format->PrecisionIndex - 1].Type & Mask)
					   != IndexType)
				goto redefined;
			Params[Format->PrecisionIndex - 1].Type = IndexType;
		}

		continue;

	redefined:
		Stack_Pop();
		FormatList->FormatString = Format->SpecifierString;
		FormatList->FormatCount	 = I;
		return FSTRING_FORMAT_PARAM_REDEFINED;
	}

	// Then we run through each param and update its value
	for (usize I = 0; I < FormatList->ParamCount; I++) {
		switch (Params[I].Type & FSTRING_FORMAT_TYPE_MASK) {
			case FSTRING_FORMAT_TYPE_B08:
				Params[I].Bool = (b08) VA_Next(Args, s32);
				break;
			case FSTRING_FORMAT_TYPE_CHR:
				Params[I].Char = (c32) VA_Next(Args, c32);
				break;
			case FSTRING_FORMAT_TYPE_S08:
				Params[I].Signed = (s08) VA_Next(Args, s32);
				break;
			case FSTRING_FORMAT_TYPE_S16:
				Params[I].Signed = (s16) VA_Next(Args, s32);
				break;
			case FSTRING_FORMAT_TYPE_S32:
				Params[I].Signed = VA_Next(Args, s32);
				break;
			case FSTRING_FORMAT_TYPE_S64:
				Params[I].Signed = VA_Next(Args, s64);
				break;
			case FSTRING_FORMAT_TYPE_SSIZE:
				Params[I].Signed = VA_Next(Args, ssize);
				break;
			case FSTRING_FORMAT_TYPE_U08:
				Params[I].Unsigned = (u08) VA_Next(Args, u32);
				break;
			case FSTRING_FORMAT_TYPE_U16:
				Params[I].Unsigned = (u16) VA_Next(Args, u32);
				break;
			case FSTRING_FORMAT_TYPE_U32:
				Params[I].Unsigned = VA_Next(Args, u32);
				break;
			case FSTRING_FORMAT_TYPE_U64:
				Params[I].Unsigned = VA_Next(Args, u64);
				break;
			case FSTRING_FORMAT_TYPE_USIZE:
				Params[I].Unsigned = VA_Next(Args, usize);
				break;
			case FSTRING_FORMAT_TYPE_R64:
				Params[I].Float = VA_Next(Args, r64);
				break;
			case FSTRING_FORMAT_TYPE_PTR:
				Params[I].Pointer = VA_Next(Args, vptr);
				break;
			case FSTRING_FORMAT_TYPE_QUERY16:
				Params[I].Pointer = VA_Next(Args, s16 *);
				break;
			case FSTRING_FORMAT_TYPE_QUERY32:
				Params[I].Pointer = VA_Next(Args, s32 *);
				break;
			case FSTRING_FORMAT_TYPE_QUERY64:
				Params[I].Pointer = VA_Next(Args, s64 *);
				break;

			// Because C varargs suck, we have to copy the string into buffer
			// data
			case FSTRING_FORMAT_TYPE_STR:
				Params[I].String  = ExtraData;
				ExtraData		  = Params[I].String + 1;
				*Params[I].String = VA_Next(Args, string);
				break;

			// We can't allow skipping any, sadly, similarly because C varargs
			// suck
			default: Stack_Pop(); return FSTRING_FORMAT_INDEX_NOT_PRESENT;
		}
	}

	// Finally, we can run through the formats again, update their values, and
	// pop the params
	for (usize I = 0; I < FormatList->FormatCount; I++) {
		Format = &FormatList->Formats[I];

		Format->Value = Params[Format->ValueIndex - 1];

		if (Format->Type & FSTRING_FORMAT_FLAG_PARAM_WIDTH) {
			Format->Width = Params[Format->WidthIndex - 1].Signed;
			// If width is negative, it means the format is left justified
			if (Format->Width < 0) {
				if (Format->Width == 0x80000000) {
					Stack_Pop();
					FormatList->FormatString = Format->SpecifierString;
					FormatList->FormatCount	 = I;
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
}

/// @brief Utility function to call `FVString_UpdateParamReferences` via
/// varargs. Mainly used for testing.
/// @param[in,out] FormatList A pointer to the format list to be used to
/// interpret the params. Cannot be null.
/// @param[in] ... The params to update FormatList's references with.
/// @return See `FVString_UpdateParamReferences` for possible return values.
internal fstring_format_status
FString_UpdateParamReferences(fstring_format_list *FormatList, ...)
{
	va_list Args;
	VA_Start(Args, FormatList);

	fstring_format_status Result =
		FVString_UpdateParamReferences(FormatList, Args);

	VA_End(Args);
	return Result;
}

/// @brief Write a string into the buffer, based on the format. Only alignment
/// and width are considered. The param's encoding is used instead of any flags
/// on read, and the written encoding matches the one passed in.
/// @param[in,out] Format A pointer to the format being written. On return, if
/// actual width was 0, it will contain the updated actual width. Cannot be
/// null.
/// @param[in] Buffer The buffer being written into. If this is too small, only
/// the format's actual width and content size will be updated. Otherwise, the
/// buffer will be written into with its specified encoding.
/// @return
/// - `FSTRING_FORMAT_VALID`: The format was written successfully.
///
/// - `FSTRING_FORMAT_BUFFER_TOO_SMALL`: Could not write into the buffer because
/// it was too small.
internal fstring_format_status
FString_WriteString(fstring_format *Format, string Buffer)
{
	Assert(Format);

	b08 IsLeft = !!(Format->Type & FSTRING_FORMAT_FLAG_LEFT_JUSTIFY);

	u32	  PadCodepoint = ' ';
	usize PadCharLen = String_GetCodepointLength(PadCodepoint, Buffer.Encoding);

	usize ContentCount = String_GetCount(Format->Value.String);
	usize ContentLength =
		String_GetTranscodedLength(*Format->Value.String, Buffer.Encoding);

	usize PadCount		= MAX(Format->Width, ContentCount) - ContentCount;
	Format->ActualWidth = PadCount * PadCharLen + ContentLength;

	if (!Buffer.Text || Buffer.Length < Format->ActualWidth)
		return FSTRING_FORMAT_BUFFER_TOO_SMALL;

	if (!IsLeft)
		String_BumpBytes(&Buffer, String_Fill(Buffer, PadCodepoint, PadCount));
	String_BumpBytes(&Buffer, String_Cpy(Buffer, *Format->Value.String));
	if (IsLeft)
		String_BumpBytes(&Buffer, String_Fill(Buffer, PadCodepoint, PadCount));

	return FSTRING_FORMAT_VALID;
}

/// @brief Write a character into the buffer, based on the format. Accepts ASCII
/// and UTF32. Only alignment, width, and `FLAG_CHR_WIDE` are considered.
/// @param[in,out] Format A pointer to the format being written. On return, if
/// actual width was 0, it will contain the updated actual width. Cannot be
/// null.
/// @param[in] Buffer The buffer being written into. If this is too small, only
/// the format's actual width and content size will be updated. Otherwise, the
/// buffer will be written into with its specified encoding.
/// @return
/// - `FSTRING_FORMAT_VALID`: The format was written successfully.
///
/// - `FSTRING_FORMAT_BUFFER_TOO_SMALL`: Could not write into the buffer because
/// it was too small.
internal fstring_format_status
FString_WriteChar(fstring_format *Format, string Buffer)
{
	Assert(Format);

	b08 IsWide = !!(Format->Type & FSTRING_FORMAT_FLAG_CHR_WIDE);

	c08	   Backing[4];
	string String = { .Text = Backing, .Length = 4 };

	u32 C32 = Format->Value.Char;
	u08 C08 = (u08) Format->Value.Char;
	if (IsWide) {
		String.Encoding = STRING_ENCODING_UTF32;
		String.Length	= String_WriteCodepoint(String, C32);
	} else {
		String.Encoding = STRING_ENCODING_UTF8;
		String.Length	= String_WriteCodepoint(String, C08);
	}

	fstring_format NewFormat = *Format;
	NewFormat.Value.String	 = &String;

	fstring_format_status Status = FString_WriteString(&NewFormat, Buffer);
	Format->ActualWidth			 = NewFormat.ActualWidth;
	return Status;
}

/// @brief Write a boolean 'true' or 'false' into the buffer, based on the
/// format. Only alignment and width are considered.
/// @param[in,out] Format A pointer to the format being written. On return, if
/// actual width was 0, it will contain the updated actual width. Cannot be
/// null.
/// @param[in] Buffer The buffer being written into. If this is too small, only
/// the format's actual width and content size will be updated. Otherwise, the
/// buffer will be written into with its specified encoding.
/// @return
/// - `FSTRING_FORMAT_VALID`: The format was written successfully.
///
/// - `FSTRING_FORMAT_BUFFER_TOO_SMALL`: Could not write into the buffer because
/// it was too small.
internal fstring_format_status
FString_WriteBool(fstring_format *Format, string Buffer)
{
	Assert(Format);

	static string TrueString  = CStringL("true");
	static string FalseString = CStringL("false");

	fstring_format NewFormat = *Format;
	NewFormat.Value.String	 = Format->Value.Bool ? &TrueString : &FalseString;

	fstring_format_status Status = FString_WriteString(&NewFormat, Buffer);
	Format->ActualWidth			 = NewFormat.ActualWidth;
	return Status;
}

/// @brief Write an unsigned int into the buffer, based on the format.
/// Alignment, width, precision, prefixing, grouping, and 0-padding are
/// considered.
/// @param[in,out] Format A pointer to the format being written. On return, if
/// actual width was 0, it will contain the updated actual width. Cannot be
/// null.
/// @param[in] Buffer The buffer being written into. If this is too small, only
/// the format's actual width and content size will be updated. Otherwise, the
/// buffer will be written into with its specified encoding.
/// @return
/// - `FSTRING_FORMAT_VALID`: The format was written successfully.
///
/// - `FSTRING_FORMAT_BUFFER_TOO_SMALL`: Could not write into the buffer because
/// it was too small.
internal fstring_format_status
FString_WriteUnsigned(fstring_format *Format, string Buffer)
{
	Assert(Format);

	b08 IsLeft		 = !!(Format->Type & FSTRING_FORMAT_FLAG_LEFT_JUSTIFY);
	b08 SpecifyRadix = !!(Format->Type & FSTRING_FORMAT_FLAG_SPECIFY_RADIX);
	b08 IsGrouped	 = !!(Format->Type & FSTRING_FORMAT_FLAG_SEPARATE_GROUPS);
	b08 IsUpper		 = !!(Format->Type & FSTRING_FORMAT_FLAG_UPPERCASE);
	b08 PadZero		 = !!(Format->Type & FSTRING_FORMAT_FLAG_PAD_WITH_ZERO);

	u32 PadCodepoint =
		(Format->Precision < 0 && PadZero && !IsLeft) ? '0' : ' ';
	usize PadCharLen = String_GetCodepointLength(PadCodepoint, Buffer.Encoding);

	string RadixPrefix = EString();
	string GroupString = CStringL(",");
	usize  Radix	   = 10;
	usize  GroupSize   = 3;
	if (Format->Type & FSTRING_FORMAT_FLAG_INT_BIN) {
		RadixPrefix = IsUpper ? CStringL("0B") : CStringL("0b");
		GroupString = CStringL("_");
		Radix		= 2;
		GroupSize	= 8;
	} else if (Format->Type & FSTRING_FORMAT_FLAG_INT_OCT) {
		RadixPrefix = CStringL("0");
		GroupString = CStringL("_");
		Radix		= 8;
	} else if (Format->Type & FSTRING_FORMAT_FLAG_INT_HEX) {
		RadixPrefix = IsUpper ? CStringL("0X") : CStringL("0x");
		GroupString = CStringL("_");
		Radix		= 16;
		GroupSize	= 4;
	}
	if (!SpecifyRadix) RadixPrefix = EString();
	if (!IsGrouped) GroupString = EString();

	usize ContentLength =
		String_GetTranscodedLength(RadixPrefix, Buffer.Encoding);
	usize ContentCount = RadixPrefix.Count;

	usize MinDigits = Format->Precision < 0 ? 1 : Format->Precision;
	if (PadCodepoint == '0' && Format->Width > ContentCount) {
		usize PadDigits	 = Format->Width - ContentCount;
		PadDigits		-= ((PadDigits - 1) / (GroupSize + GroupString.Count))
				   * GroupString.Count;
		MinDigits = MAX(MinDigits, PadDigits);
	}

	usize ValueDivisor = 1;
	usize Value		   = Format->Value.Unsigned;
	usize ValueDigits  = !!Value;
	while (Value / Radix >= ValueDivisor) {
		ValueDivisor *= Radix;
		ValueDigits++;
	}
	usize ExtraZeroes = ValueDigits < MinDigits ? MinDigits - ValueDigits : 0;
	usize TotalDigits = ExtraZeroes + ValueDigits;
	usize GroupCount  = TotalDigits ? ((TotalDigits - 1) / GroupSize) : 0;

	usize DigitLen = String_GetCodepointLength('0', Buffer.Encoding);
	ContentLength +=
		TotalDigits * DigitLen
		+ GroupCount * String_GetTranscodedLength(GroupString, Buffer.Encoding);
	ContentCount += TotalDigits + GroupString.Count * GroupCount;

	usize PadCount		= MAX(Format->Width, ContentCount) - ContentCount;
	Format->ActualWidth = PadCount * PadCharLen + ContentLength;

	if (!Buffer.Text || Buffer.Length < Format->ActualWidth)
		return FSTRING_FORMAT_BUFFER_TOO_SMALL;

	if (!IsLeft)
		String_BumpBytes(&Buffer, String_Fill(Buffer, PadCodepoint, PadCount));
	String_BumpBytes(&Buffer, String_Cpy(Buffer, RadixPrefix));

	usize DigitsSinceGroup = GroupSize - (TotalDigits % GroupSize);
	for (usize I = 0; I < TotalDigits; I++) {
		if (DigitsSinceGroup == GroupSize) {
			DigitsSinceGroup = 0;
			String_BumpBytes(&Buffer, String_Cpy(Buffer, GroupString));
		}

		u32 Codepoint;
		if (I < ExtraZeroes) {
			Codepoint = '0';
		} else {
			usize Digit	  = (Value / ValueDivisor) % Radix;
			ValueDivisor /= Radix;

			Codepoint =
				(Digit < 10 ? '0' + Digit
				 : IsUpper	? 'A' + (Digit - 10)
							: 'a' + (Digit - 10));
		}

		String_BumpBytes(&Buffer, String_WriteCodepoint(Buffer, Codepoint));
		DigitsSinceGroup++;
	}

	if (IsLeft)
		String_BumpBytes(&Buffer, String_Fill(Buffer, PadCodepoint, PadCount));
	return FSTRING_FORMAT_VALID;
}

/// @brief Write a pointer into the buffer, based on the format. Effectively the
/// same as `%#x` unles the pointer is null, in which case `(nil)` will be
/// printed.` Only alignment and width are considered.
/// @param[in,out] Format A pointer to the format being written. On return, if
/// actual width was 0, it will contain the updated actual width. Cannot be
/// null.
/// @param[in] Buffer The buffer being written into. If this is too small, only
/// the format's actual width and content size will be updated. Otherwise, the
/// buffer will be written into with its specified encoding.
/// @return
/// - `FSTRING_FORMAT_VALID`: The format was written successfully.
///
/// - `FSTRING_FORMAT_BUFFER_TOO_SMALL`: Could not write into the buffer because
/// it was too small.
internal fstring_format_status
FString_WritePointer(fstring_format *Format, string Buffer)
{
	Assert(Format);
	fstring_format_status Status;

	fstring_format NewFormat  = { .Precision = -1, .Width = Format->Width };
	NewFormat.Type			 |= Format->Type & FSTRING_FORMAT_FLAG_LEFT_JUSTIFY;

	if (Format->Value.Pointer) {
		NewFormat.Type |=
			FSTRING_FORMAT_FLAG_INT_HEX | FSTRING_FORMAT_FLAG_SPECIFY_RADIX;
		NewFormat.Value.Unsigned = (usize) Format->Value.Pointer;
		Status					 = FString_WriteUnsigned(&NewFormat, Buffer);
	} else {
		static string NilString = CStringL("(nil)");
		NewFormat.Value.String	= &NilString;
		Status					= FString_WriteString(&NewFormat, Buffer);
	}

	Format->ActualWidth = NewFormat.ActualWidth;
	return Status;
}

/// @brief Write a signed int into the buffer, based on the format. Alignment,
/// width, precision, signage, grouping, and 0-padding are considered.
/// @param[in,out] Format A pointer to the format being written. On return, if
/// actual width was 0, it will contain the updated actual width. Cannot be
/// null.
/// @param[in] Buffer The buffer being written into. If this is too small, only
/// the format's actual width and content size will be updated. Otherwise, the
/// buffer will be written into with its specified encoding.
/// @return
/// - `FSTRING_FORMAT_VALID`: The format was written successfully.
///
/// - `FSTRING_FORMAT_BUFFER_TOO_SMALL`: Could not write into the buffer because
/// it was too small.
internal fstring_format_status
FString_WriteSigned(fstring_format *Format, string Buffer)
{
	Assert(Format);

	b08 IsLeft		= !!(Format->Type & FSTRING_FORMAT_FLAG_LEFT_JUSTIFY);
	b08 PrefixSign	= !!(Format->Type & FSTRING_FORMAT_FLAG_PREFIX_SIGN);
	b08 PrefixSpace = !!(Format->Type & FSTRING_FORMAT_FLAG_PREFIX_SPACE);
	b08 IsGrouped	= !!(Format->Type & FSTRING_FORMAT_FLAG_SEPARATE_GROUPS);
	b08 PadZero		= !!(Format->Type & FSTRING_FORMAT_FLAG_PAD_WITH_ZERO);

	u32 PadCodepoint =
		(Format->Precision < 0 && PadZero && !IsLeft) ? '0' : ' ';
	usize PadCharLen = String_GetCodepointLength(PadCodepoint, Buffer.Encoding);

	string Prefix	   = Format->Value.Signed < 0 ? CStringL("-")
					   : PrefixSign				  ? CStringL("+")
					   : PrefixSpace			  ? CStringL(" ")
												  : EString();
	string GroupString = IsGrouped ? CStringL(",") : EString();

	usize ContentCount	= Prefix.Count;
	usize ContentLength = String_GetTranscodedLength(Prefix, Buffer.Encoding);

	usize DigitLen	= String_GetCodepointLength('0', Buffer.Encoding);
	usize MinDigits = Format->Precision < 0 ? 1 : Format->Precision;
	if (PadCodepoint == '0' && Format->Width > ContentCount) {
		usize PadDigits = Format->Width - ContentCount;
		PadDigits -=
			((PadDigits - 1) / (3 + GroupString.Count)) * GroupString.Count;
		MinDigits = MAX(MinDigits, PadDigits);
	}

	ssize ValueDivisor = 1;
	ssize Value		   = Format->Value.Signed;
	usize ValueDigits  = !!Value;
	if (Value < 0) {
		while (Value / 10 <= -ValueDivisor) {
			ValueDivisor *= 10;
			ValueDigits++;
		}
	} else {
		while (Value / 10 >= ValueDivisor) {
			ValueDivisor *= 10;
			ValueDigits++;
		}
	}
	usize ExtraZeroes = ValueDigits < MinDigits ? MinDigits - ValueDigits : 0;
	usize TotalDigits = ExtraZeroes + ValueDigits;
	usize GroupCount  = TotalDigits ? ((TotalDigits - 1) / 3) : 0;

	ContentLength +=
		TotalDigits * DigitLen
		+ GroupCount * String_GetTranscodedLength(GroupString, Buffer.Encoding);
	ContentCount += TotalDigits + GroupCount * GroupString.Count;

	usize PadCount		= MAX(Format->Width, ContentCount) - ContentCount;
	Format->ActualWidth = PadCount * PadCharLen + ContentLength;

	if (!Buffer.Text || Buffer.Length < Format->ActualWidth)
		return FSTRING_FORMAT_BUFFER_TOO_SMALL;

	if (!IsLeft)
		String_BumpBytes(&Buffer, String_Fill(Buffer, PadCodepoint, PadCount));
	String_BumpBytes(&Buffer, String_Cpy(Buffer, Prefix));

	usize DigitsSinceGroup = 3 - (TotalDigits % 3);
	for (usize I = 0; I < TotalDigits; I++) {
		if (DigitsSinceGroup == 3) {
			DigitsSinceGroup = 0;
			String_BumpBytes(&Buffer, String_Cpy(Buffer, GroupString));
		}

		u32 Codepoint = '0';
		if (I >= ExtraZeroes) {
			ssize Digit	  = (Value / ValueDivisor) % 10;
			ValueDivisor /= 10;
			Codepoint	 += Digit < 0 ? -Digit : Digit;
		}

		String_BumpBytes(&Buffer, String_WriteCodepoint(Buffer, Codepoint));
		DigitsSinceGroup++;
	}

	if (IsLeft)
		String_BumpBytes(&Buffer, String_Fill(Buffer, PadCodepoint, PadCount));
	return FSTRING_FORMAT_VALID;
}

/// @brief Write a floating point value into the buffer, based on the format,
/// with hexadecimal formatting. Alignment, width, precision, signage, radixing,
/// grouping, and 0-padding are considered.
/// @param[in,out] Format A pointer to the format being written. On return, if
/// actual width was 0, it will contain the updated actual width. Cannot be
/// null.
/// @param[in] Buffer The buffer being written into. If this is too small, only
/// the format's actual width and content size will be updated. Otherwise, the
/// buffer will be written into with its specified encoding.
/// @return
/// - `FSTRING_FORMAT_VALID`: The format was written successfully.
///
/// - `FSTRING_FORMAT_BUFFER_TOO_SMALL`: Could not write into the buffer because
/// it was too small.
internal fstring_format_status
FString_WriteHexFloat(fstring_format *Format, string Buffer)
{
	Assert(Format);

	b08 IsLeft		 = !!(Format->Type & FSTRING_FORMAT_FLAG_LEFT_JUSTIFY);
	b08 SpecifyRadix = !!(Format->Type & FSTRING_FORMAT_FLAG_SPECIFY_RADIX);
	b08 PrefixSign	 = !!(Format->Type & FSTRING_FORMAT_FLAG_PREFIX_SIGN);
	b08 PrefixSpace	 = !!(Format->Type & FSTRING_FORMAT_FLAG_PREFIX_SPACE);
	b08 IsUpper		 = !!(Format->Type & FSTRING_FORMAT_FLAG_UPPERCASE);
	b08 IsGrouped	 = !!(Format->Type & FSTRING_FORMAT_FLAG_SEPARATE_GROUPS);
	b08 PadZero		 = !!(Format->Type & FSTRING_FORMAT_FLAG_PAD_WITH_ZERO);

	u32	  PadCodepoint = PadZero && !IsLeft ? '0' : ' ';
	usize PadCharLen = String_GetCodepointLength(PadCodepoint, Buffer.Encoding);

	r64 Value	 = Format->Value.Float;
	u64 Binary	 = FORCE_CAST(r64, Value, u64);
	b08 Negative = Binary >> 63;
	s64 Exponent = (ssize) ((Binary >> 52) & 0x7FF) - 1023;
	u64 Mantissa = Binary & ((1ull << 52) - 1);

	u64 FracPart = Mantissa;
	if (Exponent == -1023) Exponent = -1022;
	else Mantissa |= 1ull << 52;

	u64 FracDigits	= 13;
	u64 FracDivisor = 1ull << 48;
	if (Format->Precision < 0) {
		if (!FracPart) FracDigits = 0;
		else {
			u64 DigitCheck = FracPart;
			while (DigitCheck % 16 == 0) {
				DigitCheck /= 16;
				FracDigits--;
			}
		}
	} else {
		FracDigits = Format->Precision;
		if (FracDigits < 13) {
			usize RoundingDigit = (FracPart >> (48 - FracDigits * 4)) % 16;
			if (RoundingDigit >= 8) Mantissa += 1ull << (52 - FracDigits * 4);
			FracPart = Mantissa & R64_MANTISSA_MASK;
		}
	}
	u64 WholePart = Mantissa >> 52;

	usize ExpDigits	  = 1;
	usize ExpDivisor  = 1;
	b08	  NegativeExp = Exponent < 0;
	usize ExpPart	  = NegativeExp ? -Exponent : Exponent;
	while (ExpPart / 10 >= ExpDivisor) {
		ExpDivisor *= 10;
		ExpDigits++;
	}
	if (ExpDigits < 1) ExpDigits = 1;

	string SignString  = Negative	 ? CStringL("-")
					   : PrefixSign	 ? CStringL("+")
					   : PrefixSpace ? CStringL(" ")
									 : EString();
	string HexString   = IsUpper ? CStringL("0X") : CStringL("0x");
	string GroupString = IsGrouped ? CStringL("_") : EString();
	string RadixString =
		!SpecifyRadix && !FracDigits ? EString() : CStringL(".");
	string ExpSignString = NegativeExp ? CStringL("-") : CStringL("+");
	string ExpString	 = IsUpper ? CStringL("P") : CStringL("p");

	usize DigitLen = String_GetCodepointLength('0', Buffer.Encoding);
	usize ContentLength =
		String_GetTranscodedLength(SignString, Buffer.Encoding)
		+ String_GetTranscodedLength(HexString, Buffer.Encoding)
		+ String_GetTranscodedLength(RadixString, Buffer.Encoding)
		+ FracDigits * DigitLen
		+ String_GetTranscodedLength(ExpSignString, Buffer.Encoding)
		+ String_GetTranscodedLength(ExpString, Buffer.Encoding)
		+ ExpDigits * DigitLen;
	usize ContentCount = SignString.Count
					   + HexString.Count
					   + RadixString.Count
					   + FracDigits
					   + ExpSignString.Count
					   + ExpString.Count
					   + ExpDigits;

	usize WholeDigits = 1;
	if (PadCodepoint == '0' && Format->Width > ContentCount) {
		usize PadDigits = Format->Width - ContentCount;
		PadDigits -=
			((PadDigits - 1) / (4 + GroupString.Count)) * GroupString.Count;
		WholeDigits = MAX(WholeDigits, PadDigits);
	}
	usize GroupCount = (WholeDigits - 1) / 4;

	ContentLength +=
		WholeDigits * DigitLen
		+ GroupCount * String_GetTranscodedLength(GroupString, Buffer.Encoding);
	ContentCount += WholeDigits + GroupCount * GroupString.Count;

	usize PadCount		= MAX(Format->Width, ContentCount) - ContentCount;
	Format->ActualWidth = PadCount * PadCharLen + ContentLength;

	if (!Buffer.Text || Buffer.Length < Format->ActualWidth)
		return FSTRING_FORMAT_BUFFER_TOO_SMALL;

	if (!IsLeft)
		String_BumpBytes(&Buffer, String_Fill(Buffer, PadCodepoint, PadCount));
	String_BumpBytes(&Buffer, String_Cpy(Buffer, SignString));
	String_BumpBytes(&Buffer, String_Cpy(Buffer, HexString));

	usize DigitsSinceGroup = 4 - (WholeDigits % 4);
	for (usize I = 0; I < WholeDigits; I++) {
		if (DigitsSinceGroup == 4) {
			DigitsSinceGroup = 0;
			String_BumpBytes(&Buffer, String_Cpy(Buffer, GroupString));
		}

		u32 Codepoint = '0';
		if (I == WholeDigits - 1) Codepoint += WholePart;
		String_BumpBytes(&Buffer, String_WriteCodepoint(Buffer, Codepoint));
		DigitsSinceGroup++;
	}

	String_BumpBytes(&Buffer, String_Cpy(Buffer, RadixString));

	while (FracDigits) {
		usize Digit		= (FracPart / FracDivisor) % 16;
		u32	  Codepoint = Digit < 10 ? '0' + Digit
						: IsUpper	 ? 'A' + (Digit - 10)
									 : 'a' + (Digit - 10);
		String_BumpBytes(&Buffer, String_WriteCodepoint(Buffer, Codepoint));
		FracDivisor /= 16;
		FracDigits--;
	}

	String_BumpBytes(&Buffer, String_Cpy(Buffer, ExpString));
	String_BumpBytes(&Buffer, String_Cpy(Buffer, ExpSignString));

	while (ExpDigits) {
		usize Digit		= (ExpPart / ExpDivisor) % 10;
		u32	  Codepoint = '0' + Digit;
		String_BumpBytes(&Buffer, String_WriteCodepoint(Buffer, Codepoint));
		ExpDivisor /= 10;
		ExpDigits--;
	}

	if (IsLeft)
		String_BumpBytes(&Buffer, String_Fill(Buffer, PadCodepoint, PadCount));

	return FSTRING_FORMAT_VALID;
}

/// @brief Write a floating point value into the buffer, based on the format.
/// Alignment, width, precision, signage, radixing, grouping, and 0-padding are
/// considered.
/// @param[in,out] Format A pointer to the format being written. On return, if
/// actual width was 0, it will contain the updated actual width. Cannot be
/// null.
/// @param[in] Buffer The buffer being written into. If this is too small, only
/// the format's actual width and content size will be updated. Otherwise, the
/// buffer will be written into with its specified encoding.
/// @return
/// - `FSTRING_FORMAT_VALID`: The format was written successfully.
///
/// - `FSTRING_FORMAT_BUFFER_TOO_SMALL`: Could not write into the buffer because
/// it was too small.
internal fstring_format_status
FString_WriteFloat(fstring_format *Format, string Buffer)
{
	Assert(Format);

	b08 IsLeft		 = !!(Format->Type & FSTRING_FORMAT_FLAG_LEFT_JUSTIFY);
	b08 SpecifyRadix = !!(Format->Type & FSTRING_FORMAT_FLAG_SPECIFY_RADIX);
	b08 PrefixSign	 = !!(Format->Type & FSTRING_FORMAT_FLAG_PREFIX_SIGN);
	b08 PrefixSpace	 = !!(Format->Type & FSTRING_FORMAT_FLAG_PREFIX_SPACE);
	b08 IsUpper		 = !!(Format->Type & FSTRING_FORMAT_FLAG_UPPERCASE);
	b08 IsGrouped	 = !!(Format->Type & FSTRING_FORMAT_FLAG_SEPARATE_GROUPS);
	b08 PadZero		 = !!(Format->Type & FSTRING_FORMAT_FLAG_PAD_WITH_ZERO);
	b08 IsExp		 = !!(Format->Type & FSTRING_FORMAT_FLAG_FLOAT_EXP);
	b08 IsFit		 = !!(Format->Type & FSTRING_FORMAT_FLAG_FLOAT_FIT);
	b08 IsHex		 = !!(Format->Type & FSTRING_FORMAT_FLAG_FLOAT_HEX);
	b08 IsStd		 = !IsExp && !IsFit && !IsHex;
	Assert(IsExp + IsFit + IsHex <= 1);

	r64 Value  = Format->Value.Float;
	u64 Binary = FORCE_CAST(r64, Value, u64);

	b08	  Negative = (Binary & R64_SIGN_MASK) >> R64_SIGN_SHIFT;
	ssize Exponent =
		(ssize) ((Binary & R64_EXPONENT_MASK) >> R64_EXPONENT_SHIFT)
		- R64_EXPONENT_BIAS;
	u64 Mantissa = Binary & R64_MANTISSA_MASK;

	if (Exponent == R64_EXPONENT_MAX) {
		c08	   SpecialChars[4];
		string SpecialStr = CLEString(SpecialChars, 4, STRING_ENCODING_ASCII);
		if (Mantissa) Mem_Cpy(SpecialChars + 1, IsUpper ? "NAN" : "nan", 3);
		else Mem_Cpy(SpecialChars + 1, IsUpper ? "INF" : "inf", 3);
		if (Negative) SpecialChars[0] = '-';
		else if (PrefixSign) SpecialChars[0] = '+';
		else if (PrefixSpace) SpecialChars[0] = ' ';
		else String_BumpBytes(&SpecialStr, 1);

		fstring_format NewFormat	 = *Format;
		NewFormat.Value.String		 = &SpecialStr;
		fstring_format_status Status = FString_WriteString(&NewFormat, Buffer);
		Format->ActualWidth			 = NewFormat.ActualWidth;
		return Status;
	}

	if (IsHex) return FString_WriteHexFloat(Format, Buffer);

	Stack_Push();

	u32	  PadCodepoint = PadZero && !IsLeft ? '0' : ' ';
	usize PadCharLen = String_GetCodepointLength(PadCodepoint, Buffer.Encoding);

	// Account for zero and denormals
	if (Exponent == -1023) Exponent = -1022;
	else Mantissa |= 1ull << 52;
	if (!Mantissa) Exponent = 0;

	// Deconstruct the mantissa into fractional and whole parts, removing
	// trailing zero bits
	usize WholeBits			= Exponent < 0 ? 0 : Exponent;
	usize FracBits			= Exponent > 52 ? 0 : 52 - Exponent;
	usize MantissaWholePart = FracBits > 52 ? 0 : Mantissa >> FracBits;
	usize MantissaFracPart =
		FracBits > 52 ? Mantissa : Mantissa & ((1ull << FracBits) - 1);
	u32 FracPartLSB;
	if (Intrin_BitScanForward(&FracPartLSB, MantissaFracPart)) {
		MantissaFracPart >>= FracPartLSB;
		FracBits		  -= MIN(FracBits, FracPartLSB);
	} else {
		FracBits = 0;
	}

	// Utility bigints
	bigint Zero = BigInt(0);
	bigint One	= BigInt(1);
	bigint Five = BigInt(5);
	bigint Ten	= BigInt(10);

	// Get the number of significant digits
	ssize SigDigitCount = Format->Precision;
	if (IsFit && SigDigitCount <= 0) SigDigitCount = 1;
	else if (SigDigitCount < 0) SigDigitCount = 6;

	// Include the whole digit for exponential form
	if (IsExp && SigDigitCount < S32_MAX) SigDigitCount++;

	// Compute the whole portion
	ssize  WholeDigitCount = 0;
	usize  WholeShift	   = WholeBits < 52 ? 0 : WholeBits - 52;
	bigint MinDigitDivisor = One;
	bigint MaxDigitDivisor = One;
	bigint SigDigitDivisor = One;
	bigint WholePart =
		BigInt_SShift(BigInt_SScalar(MantissaWholePart), WholeShift);
	while (BigInt_Compare(WholePart, MaxDigitDivisor) >= 0) {
		if (!IsStd && SigDigitCount == WholeDigitCount)
			SigDigitDivisor = MaxDigitDivisor;
		MaxDigitDivisor = BigInt_SMul(MaxDigitDivisor, Ten);
		WholeDigitCount++;
	}
	if (!IsStd) MinDigitDivisor = BigInt_SDiv(MaxDigitDivisor, SigDigitDivisor);
	if (Value == 0) {
		WholeDigitCount = 1;
		MaxDigitDivisor = Ten;
		MinDigitDivisor = Ten;
	}

	// Standard floats' precision don't include whole digits, so we account for
	// that by adding the whole digit count. This can technically overflow
	// if precision is near max.
	if (IsStd) {
		if (SigDigitCount <= S32_MAX - WholeDigitCount)
			SigDigitCount += WholeDigitCount;
		else SigDigitCount = S32_MAX;
	}

	// Compute the fractional portion
	ssize  DigitCount	= 0;
	ssize  DecimalIndex = 0;
	bigint FullValue	= Zero;

	// We already have MaxDigitDivisor if a whole part exists
	if (WholeDigitCount) {
		bigint FracPart	   = Zero;
		bigint FracDivisor = One;

		// We don't need the fractional part if we've already found all
		// necessary significant digits
		if (SigDigitCount < WholeDigitCount) FracBits = 0;

		for (; DecimalIndex < FracBits; DecimalIndex++) {
			// As we shift the mantissa, existing bits are worth 1/2 as much
			// (e.g., 0.5 -> 0.25 -> 0.125, etc.). This is the same as
			// multiplying by five and keeping track of the decimal place.
			FracPart = BigInt_SMul(FracPart, Five);

			// Grab this divisor so we don't need to recompute it
			if (FracBits - DecimalIndex == SigDigitCount - WholeDigitCount)
				MinDigitDivisor = FracDivisor;

			if (DecimalIndex < 53
				&& (MantissaFracPart & (1ull << DecimalIndex)))
			{
				// Insert a 5 left of the decimal
				FracPart =
					BigInt_SAdd(FracPart, BigInt_SMul(FracDivisor, Five));
			}

			// Bump the decimal place
			FracDivisor = BigInt_SMul(FracDivisor, Ten);
		}
		if (SigDigitCount == WholeDigitCount) MinDigitDivisor = FracDivisor;

		// Combine the two parts together
		DigitCount = WholeDigitCount + DecimalIndex;
		FullValue  = BigInt_SAdd(FracPart, BigInt_SMul(WholePart, FracDivisor));
		MaxDigitDivisor = BigInt_SMul(MaxDigitDivisor, FracDivisor);
	} else {
		bigint FracDivisor = One;

		for (; DecimalIndex < FracBits; DecimalIndex++) {
			// Shift the mantissa
			FullValue = BigInt_SMul(FullValue, Five);

			// Grab this divisor so we don't need to recompute it
			if (DecimalIndex == SigDigitCount) SigDigitDivisor = FracDivisor;

			if (DecimalIndex < 53
				&& (MantissaFracPart & (1ull << DecimalIndex)))
			{
				// Bump the decimal place
				FracDivisor		= BigInt_SMul(FracDivisor, Ten);
				// Bump the max digit's place
				MaxDigitDivisor = FracDivisor;
				DigitCount		= DecimalIndex + 1;
				// Insert a 5 right of the decimal
				FullValue =
					BigInt_SAdd(FullValue, BigInt_SShift(FracDivisor, -1));
			} else {
				FracDivisor = BigInt_SMul(FracDivisor, Ten);

				// The multiply by 5 bumped the max digit
				if (BigInt_Compare(FullValue, MaxDigitDivisor) >= 0) {
					MaxDigitDivisor = BigInt_SMul(MaxDigitDivisor, Ten);
					DigitCount++;
				}
			}
		}

		MinDigitDivisor = BigInt_SDiv(MaxDigitDivisor, SigDigitDivisor);
	}

	bigint SigValue = FullValue;
	if (DigitCount > SigDigitCount) {
		// Truncate to significant digits
		bigint RoundingDigit;
		bigint RoundingFullValue = BigInt_SMul(FullValue, Ten);
		bigint RoundingSigValue =
			BigInt_SDiv(RoundingFullValue, MinDigitDivisor);
		BigInt_SDivRem(RoundingSigValue, Ten, &SigValue, &RoundingDigit);
		MaxDigitDivisor	 = BigInt_SDiv(MaxDigitDivisor, MinDigitDivisor);
		DecimalIndex	-= DigitCount - SigDigitCount;
		DigitCount		 = SigDigitCount;

		// Perform rounding
		if (BigInt_Compare(RoundingDigit, Five) >= 0) {
			SigValue = BigInt_SAdd(SigValue, One);

			// Carrying may have added a new whole digit
			if (BigInt_Compare(MaxDigitDivisor, SigValue) <= 0) {
				if (IsStd) {
					DigitCount++;
					SigDigitCount++;
					MaxDigitDivisor = BigInt_SMul(MaxDigitDivisor, Ten);
				} else {
					DecimalIndex--;
					SigValue = BigInt_SDiv(SigValue, Ten);
				}
			}
		}
	}

	// Adjust the decimal point for exponential form
	ssize PowTen = DigitCount - DecimalIndex - 1;
	if (IsFit && ((PowTen < -4) || PowTen >= SigDigitCount)) IsExp = TRUE;
	if (IsExp) DecimalIndex = DigitCount - 1;

	// Remove trailing zeroes
	if (IsFit && !SpecifyRadix) {
		bigint Quot, Rem;
		while (DigitCount && DecimalIndex > 0) {
			BigInt_SDivRem(SigValue, Ten, &Quot, &Rem);
			if (!BigInt_IsZero(Rem)) break;
			SigValue		= Quot;
			MaxDigitDivisor = BigInt_SDiv(MaxDigitDivisor, Ten);
			DecimalIndex--;
			DigitCount--;
		}
		SigDigitCount = DigitCount;
	}

	// Compute the digit count and divisor of the exponent
	b08 NegativePowTen	= PowTen < 0;
	PowTen				= PowTen < 0 ? -PowTen : PowTen;
	usize ExpDigitCount = IsExp ? 2 : 0;
	usize ExpDivisor	= 10;
	if (IsExp) {
		if (PowTen >= 100) {
			ExpDigitCount++;
			ExpDivisor *= 10;
		}
		if (PowTen >= 1000) {
			ExpDigitCount++;
			ExpDivisor *= 10;
		}
	}

	// Setup the string components
	string SignString	 = Negative	   ? CStringL("-")
						 : PrefixSign  ? CStringL("+")
						 : PrefixSpace ? CStringL(" ")
									   : EString();
	string GroupString	 = IsGrouped ? CStringL(",") : EString();
	string DecimalString = CStringL(".");
	string ExpString =
		IsExp ? (IsUpper ? CStringL("E") : CStringL("e")) : EString();
	string ExpSignString =
		IsExp ? (NegativePowTen ? CStringL("-") : CStringL("+")) : EString();
	usize DigitLen = String_GetCodepointLength('0', Buffer.Encoding);

	WholeDigitCount	 = MAX(DigitCount - DecimalIndex, 1);
	ssize GroupCount = (WholeDigitCount - 1) / 3;
	ssize GroupStrLen =
		String_GetTranscodedLength(GroupString, Buffer.Encoding);

	// Calculate most of the content length, barring value digits and groups
	ssize ContentLength =
		String_GetTranscodedLength(SignString, Buffer.Encoding)
		+ WholeDigitCount * DigitLen
		+ GroupStrLen * GroupCount
		+ String_GetTranscodedLength(DecimalString, Buffer.Encoding)
		+ String_GetTranscodedLength(ExpString, Buffer.Encoding)
		+ String_GetTranscodedLength(ExpSignString, Buffer.Encoding)
		+ ExpDigitCount * DigitLen;
	ssize ContentCount = SignString.Count
					   + WholeDigitCount
					   + GroupCount * GroupString.Count
					   + DecimalString.Count
					   + ExpString.Count
					   + ExpSignString.Count
					   + ExpDigitCount;

	// Recompute the number of digits
	ssize FracDigitCount = 0;
	if (DecimalIndex > 0) FracDigitCount = DecimalIndex;
	if (SigDigitCount > MAX(DigitCount, DecimalIndex)) {
		ssize MaxFracCount = (SSIZE_MAX - ContentLength) / DigitLen;
		ssize DigitsToAdd  = SigDigitCount - MAX(DigitCount, DecimalIndex);
		if (MaxFracCount - FracDigitCount < DigitsToAdd)
			FracDigitCount = MaxFracCount;
		else FracDigitCount += DigitsToAdd;
	}
	ContentLength += FracDigitCount * DigitLen;
	ContentCount  += FracDigitCount;

	// Remove the decimal string if we truly have no frac digits
	if (!FracDigitCount && !SpecifyRadix) {
		ContentLength -= DecimalString.Length;
		ContentCount  -= DecimalString.Count;
		DecimalString  = EString();
	}

	// Add zero-padding if necessary and account for groups
	ssize ExtraWholeDigits = DigitCount <= DecimalIndex;
	if (PadCodepoint == '0' && Format->Width > ContentCount) {
		ContentLength -=
			WholeDigitCount * DigitLen + GroupCount * GroupString.Length;
		ContentCount -= WholeDigitCount + GroupCount * GroupString.Count;

		usize PadDigits = Format->Width - ContentCount;
		PadDigits -=
			((PadDigits - 1) / (3 + GroupString.Count)) * GroupString.Count;
		GroupCount = (PadDigits - 1) / 3;

		usize UnitLen	   = 3 * PadCharLen + GroupStrLen;
		usize MaxUnitCount = (USIZE_MAX - ContentLength) / UnitLen;
		if (GroupCount > MaxUnitCount) {
			GroupCount	 = MaxUnitCount;
			usize RemLen = USIZE_MAX - ContentLength - MaxUnitCount * UnitLen;
			usize RemDigits = MIN(PadDigits % 3, RemLen / PadCharLen);
			PadDigits		= MaxUnitCount * 3 + RemDigits;
		}

		ExtraWholeDigits += PadDigits - WholeDigitCount;
		WholeDigitCount	  = PadDigits;
		ContentLength += WholeDigitCount * DigitLen + GroupCount * GroupStrLen;
		ContentCount  += WholeDigitCount + GroupCount;
	}

	ssize PadCount		= MAX(Format->Width, ContentCount) - ContentCount;
	Format->ActualWidth = PadCount * PadCharLen + ContentLength;

	if (!Buffer.Text || Buffer.Length < Format->ActualWidth) {
		Stack_Pop();
		return FSTRING_FORMAT_BUFFER_TOO_SMALL;
	}

	if (!IsLeft)
		String_BumpBytes(&Buffer, String_Fill(Buffer, PadCodepoint, PadCount));
	String_BumpBytes(&Buffer, String_Cpy(Buffer, SignString));

	usize DigitsSinceGroup = 3 - (WholeDigitCount % 3);
	for (ssize I = 0; I < WholeDigitCount; I++) {
		if (DigitsSinceGroup == 3) {
			DigitsSinceGroup = 0;
			String_BumpBytes(&Buffer, String_Cpy(Buffer, GroupString));
		}

		u32 Codepoint = '0';
		if (I >= ExtraWholeDigits && I - ExtraWholeDigits < DigitCount) {
			bigint Digit;
			MaxDigitDivisor = BigInt_SDiv(MaxDigitDivisor, Ten);
			BigInt_SDivRem(SigValue, MaxDigitDivisor, &Digit, &SigValue);
			Assert(BigInt_Compare(Digit, Ten) < 0);
			Assert(BigInt_Compare(Digit, Zero) >= 0);
			Codepoint += BigInt_ToInt(Digit);
		}

		String_BumpBytes(&Buffer, String_WriteCodepoint(Buffer, Codepoint));
		DigitsSinceGroup++;
	}

	String_BumpBytes(&Buffer, String_Cpy(Buffer, DecimalString));

	for (ssize I = 0; I < FracDigitCount; I++) {
		u32 Codepoint = '0';

		if (I < DecimalIndex && I + DigitCount >= DecimalIndex) {
			bigint Digit;
			MaxDigitDivisor = BigInt_SDiv(MaxDigitDivisor, Ten);
			BigInt_SDivRem(SigValue, MaxDigitDivisor, &Digit, &SigValue);
			Assert(BigInt_Compare(Digit, Ten) < 0);
			Assert(BigInt_Compare(Digit, Zero) >= 0);
			Codepoint += BigInt_ToInt(Digit);
		}

		String_BumpBytes(&Buffer, String_WriteCodepoint(Buffer, Codepoint));
	}

	String_BumpBytes(&Buffer, String_Cpy(Buffer, ExpString));
	String_BumpBytes(&Buffer, String_Cpy(Buffer, ExpSignString));

	for (usize I = 0; I < ExpDigitCount; I++) {
		u32 Codepoint  = '0' + ((PowTen / ExpDivisor) % 10);
		ExpDivisor	  /= 10;
		String_BumpBytes(&Buffer, String_WriteCodepoint(Buffer, Codepoint));
	}

	if (IsLeft)
		String_BumpBytes(&Buffer, String_Fill(Buffer, PadCodepoint, PadCount));

	Stack_Pop();
	return FSTRING_FORMAT_VALID;
}

/// @brief Writes a formatted value into the provided buffer.
/// @param[in,out] Format A pointer to the format specifier containing the
/// type, flags, and value. Cannot be null.
/// @param[in] Buffer The buffer being written into. If this is too small,
/// only the format's actual width and content size will be updated.
/// Otherwise, the buffer will be written into with its specified encoding.
/// @return
/// - `FSTRING_FORMAT_VALID`: The format was written successfully.
///
/// - `FSTRING_FORMAT_BUFFER_TOO_SMALL`: The provided output buffer was too
/// small to contain the full output.
///
/// - `FSTRING_FORMAT_TYPE_INVALID`: The provided format's type was
/// unsupported. This can occur, for example, if you provide a non-writable
/// query type.
internal fstring_format_status
FString_WriteFormat(fstring_format *Format, string Buffer)
{
	Assert(Format);

	fstring_format_status Status;

	b08 NoWrite = !!(Format->Type & FSTRING_FORMAT_FLAG_NO_WRITE);
	if (NoWrite) Buffer.Length = 0;

	switch (Format->Type & FSTRING_FORMAT_TYPE_MASK) {
		case FSTRING_FORMAT_TYPE_B08:
			Status = FString_WriteBool(Format, Buffer);
			break;

		case FSTRING_FORMAT_TYPE_CHR:
			Status = FString_WriteChar(Format, Buffer);
			break;

		case FSTRING_FORMAT_TYPE_S08:
		case FSTRING_FORMAT_TYPE_S16:
		case FSTRING_FORMAT_TYPE_S32:
		case FSTRING_FORMAT_TYPE_S64:
		case FSTRING_FORMAT_TYPE_SSIZE:
			Status = FString_WriteSigned(Format, Buffer);
			break;

		case FSTRING_FORMAT_TYPE_U08:
		case FSTRING_FORMAT_TYPE_U16:
		case FSTRING_FORMAT_TYPE_U32:
		case FSTRING_FORMAT_TYPE_U64:
		case FSTRING_FORMAT_TYPE_USIZE:
			Status = FString_WriteUnsigned(Format, Buffer);
			break;

		case FSTRING_FORMAT_TYPE_R64:
			Status = FString_WriteFloat(Format, Buffer);
			break;

		case FSTRING_FORMAT_TYPE_PTR:
			Status = FString_WritePointer(Format, Buffer);
			break;

		case FSTRING_FORMAT_TYPE_STR:
			Status = FString_WriteString(Format, Buffer);
			break;

		default: Status = FSTRING_FORMAT_TYPE_INVALID;
	}

	if (NoWrite) {
		Format->ActualWidth = 0;
		if (Status == FSTRING_FORMAT_BUFFER_TOO_SMALL)
			Status = FSTRING_FORMAT_VALID;
	}

	return Status;
}

/// @brief Compute the minimum size the output buffer needs to be to contain
/// the fully formatted string, and if the buffer is large enough, write
/// into it.
/// @param[in,out] FormatList A pointer to the list of formats and other
/// useful data. Format actual widths and total text size will be updated
/// after this call. Cannot be null.
/// @param[in] Buffer The buffer being written into. If this is too small,
/// `FormatList->TotalTextSize` will be set to the required size. The buffer
/// will be written into either way with its specified encoding, up to its
/// max length.
/// @return
/// - `FSTRING_FORMAT_VALID`: The total size was computed successfully.
///
/// - `FSTRING_FORMAT_BUFFER_TOO_SMALL`: The provided output buffer was too
/// small to contain the full output.
///
/// - `FSTRING_FORMAT_TYPE_INVALID`: One of the provided formats had an
/// invalid type.
internal fstring_format_status
FString_WriteFormats(fstring_format_list *FormatList, string Buffer)
{
	Assert(FormatList);

	b08 TooSmall			  = FALSE;
	FormatList->TotalTextSize = 0;
	string Src				  = FormatList->FormatString;

	// Run through the formats and accumulate their text sizes
	for (usize I = 0; I < FormatList->FormatCount; I++) {
		fstring_format *Format = &FormatList->Formats[I];

		// Add the length from the plaintext prior
		Src.Length = (usize) Format->SpecifierString.Text - (usize) Src.Text;
		while (Src.Length) {
			u32 Codepoint = String_NextCodepoint(&Src);
			if (Codepoint == '%') String_NextCodepoint(&Src);
			usize Delta = String_WriteCodepoint(Buffer, Codepoint);
			if (Buffer.Length < Delta) TooSmall = TRUE;
			String_BumpBytes(&Buffer, Delta);
			FormatList->TotalTextSize += Delta;
		}
		Src.Text += Format->SpecifierString.Length;

		switch (Format->Type & FSTRING_FORMAT_TYPE_MASK) {
			case FSTRING_FORMAT_TYPE_QUERY16:
				*(s16 *) Format->Value.Pointer = FormatList->TotalTextSize;
				break;
			case FSTRING_FORMAT_TYPE_QUERY32:
				*(s32 *) Format->Value.Pointer = FormatList->TotalTextSize;
				break;
			case FSTRING_FORMAT_TYPE_QUERY64:
				*(s64 *) Format->Value.Pointer = FormatList->TotalTextSize;
				break;

			default:
				fstring_format_status Status =
					FString_WriteFormat(Format, Buffer);
				if (Status == FSTRING_FORMAT_BUFFER_TOO_SMALL) TooSmall = TRUE;
				else if (Status != FSTRING_FORMAT_VALID) return Status;
				String_BumpBytes(&Buffer, Format->ActualWidth);
				FormatList->TotalTextSize += Format->ActualWidth;
		}
	}

	// Add the remaining plaintext length
	Src.Length = (usize) FormatList->FormatString.Text
			   + FormatList->FormatString.Length
			   - (usize) Src.Text;
	while (Src.Length) {
		u32 Codepoint = String_NextCodepoint(&Src);
		if (Codepoint == '%') String_NextCodepoint(&Src);
		usize Delta = String_WriteCodepoint(Buffer, Codepoint);
		if (Buffer.Length < Delta) TooSmall = TRUE;
		String_BumpBytes(&Buffer, Delta);
		FormatList->TotalTextSize += Delta;
	}

	return TooSmall ? FSTRING_FORMAT_BUFFER_TOO_SMALL : FSTRING_FORMAT_VALID;
}

/// @brief Format the provided template string with the given args.
/// @param[in] Format A template string to insert the parameters into. See
/// `FString` for more details.
/// @param[in] Args A va_list of parameters to insert into the format
/// string. These must align with the patterns in `Format` or stack
/// corruption may occur.
/// @return A stack-backed formatted string. If any errors occurred during
/// parsing, this will be a copy of the format string, and an error will be
/// logged.
internal string
FVString(string Format, va_list Args)
{
	fstring_format_list	  FormatList;
	fstring_format_status Status;

	usize  TotalFormats	  = USIZE_MAX;
	string OriginalFormat = Format;
	string ErrorString	  = EString();

	// First we have to parse the format string
	Status = FString_ParseFormatString(&Format, &FormatList);
	if (Status != FSTRING_FORMAT_VALID) {
		ErrorString.Text   = Format.Text;
		ErrorString.Length = 1;
		goto failed;
	}
	TotalFormats = FormatList.FormatCount;

	// Now we populate its params with Args
	Status = FVString_UpdateParamReferences(&FormatList, Args);
	if (Status != FSTRING_FORMAT_VALID) {
		ErrorString = FormatList.FormatString;
		goto failed;
	}

	string Buffer	= EString();
	Buffer.Encoding = Format.Encoding;

	// Need to compute the total size to know how large the buffer should
	// be.
	Status = FString_WriteFormats(&FormatList, Buffer);

	// This shouldn't happen, but...
	if (Status == FSTRING_FORMAT_VALID) return Buffer;
	if (Status != FSTRING_FORMAT_BUFFER_TOO_SMALL) {
		ErrorString = FormatList.FormatString;
		goto failed;
	}

	// Allocate the buffer on the stack. We might support other allocators
	// later.
	Buffer.Length = FormatList.TotalTextSize;
	Buffer.Text	  = Stack_Allocate(Buffer.Length);

	// Write into the buffer and return it.
	Status = FString_WriteFormats(&FormatList, Buffer);

	// This shouldn't happen, but...
	if (Status != FSTRING_FORMAT_VALID) {
		ErrorString = FormatList.FormatString;
		goto failed;
	}

	return Buffer;

// The parsing failed, so we'll return a copy of the original format string
// and log an error
failed:
	Stack_Push();
	string ErrorPointer = LString(OriginalFormat.Length + 1);
	Mem_Set(ErrorPointer.Text, ' ', ErrorPointer.Length);
	Mem_Set(
		ErrorPointer.Text
			+ (usize) ErrorString.Text
			- (usize) OriginalFormat.Text,
		'^',
		ErrorString.Length
	);
	string ErrorFormat =
		FormatList.FormatCount < TotalFormats
			? CStringL(
				  "Error parsing format string, in specifier %d: "
				  "%s.\n\n%s\n%s\n\n"
			  )
			: CStringL("Error parsing format string: %_d%s.\n\n%s\n%s\n\n");
	Platform_WriteError(
		FString(
			ErrorFormat,
			FormatList.FormatCount + 1,
			FStringFormatStatusDescriptions[Status],
			OriginalFormat,
			ErrorPointer
		),
		FALSE
	);
	Stack_Pop();

	string Result = OriginalFormat;
	Result.Text	  = Stack_Allocate(Result.Length);
	Mem_Cpy(Result.Text, OriginalFormat.Text, Result.Length);
	return Result;
}

/// @brief Format the provided template string with the given args.
/// @param[in] Format A template string to insert the parameters into. It
/// can be a string of any encoding, but special character sequences
/// beginning with '%' are treated as format specifiers. To print a plain
/// percent sign, use '%%'.
///
/// Format specifiers can be in one of two forms. All formats in the string
/// must use the same form:
///  - '%' [ flags ] [ width | '*' ] [ '.' [ precision | '*' ] ] [ size ]
///    type
///
///  - '%' index '$' [ flags ] [ width | '*' index '$' ]
///    [ '.' [ precision | '*' index '$' ] ] [ size ] type
///
/// Width is an integer that specifies the minimum number of codepoints that
/// the result should contain. If not specified, it defaults to zero. You
/// cannot specify zero directly since that would be treated as a flag. If
/// the result is smaller, it suffixes with spaces until it reaches the
/// specified width. If the result is larger, it does nothing. You can also
/// specify '*' instead of a number to read the next param as an s32 and use
/// its value as width. Negative values will be treated as positive with
/// Left Justify. Width cannot be larger than S32_MAX or less than -S32_MAX.
///
/// Precision generally specifies the minimum number of digits to print. If
/// a precision would cause the full value to be cut off, the next smallest
/// digit is rounded away from zero. You can also specify '*' instead of a
/// number to read the next param as an s32 and use its value as precision.
/// Negative values will be treated as default. Precision cannot be larger
/// than S32_MAX. If you specify '.' but no precision, it will be treated as
/// 0.
///  - For %d, %i, %x, %o, and %b, it defaults to 1 and dictates the minimum
///    number of digits to print, prefixing with zero if necessary.
///
///  - For %f and %e, it defaults to 6 and dictates the number of fractional
///    digits to print, suffixing with zero if necessary. If zero, no decimal
///    point will be printed.
///
///  - For %a, it dictates the number of fractional digits to print,
///    suffixing with zero if necessary. If zero, no radix point will be
///    printed. If default, the full precision will be printed with trailing
///    zeroes trimmed off.
///
///  - For %g, it defaults to 1 and dictates the number of significant
///    digits to print. Zero is also treated as 1. If 1, no decimal point will
///    be printed.
///
/// Flags control various formatting styles. They can be repeated as many
/// times as you'd like. The following flags are supported:
///  - '-': Left Justify. If the format's width is greater than its content,
///    padding will be suffixed instead of prefixed, effectively aligning
///    the result to the left.
///
///  - '+': Prefix Sign. For %d, %i, %f, %e, %g, and %a, this forces '+' to
///    be prefixed for positive values.
///
///  - ' ': Prefix Space. For %d, %i, %f, %e, %g, and %a, this forces ' ' to
///    be prefixed for positive values. This does nothing if Prefix Sign is
///    specified as well.
///
///  - '#': Specify Radix. This usually prints radix-specific information.
///    For %x, prefix with '0x'. For %o, prefix with '0'. For %b, prefix with
///    '0b'. For %a, %f, and %e, force '.' even with no fractional digits.
///    For %g, force '.' and don't trim trailing zeroes.
///
///  - ''': Separate Groups. This delimits integer digits with separator
///    characters, usually ',' every three digits for decimal numbers, '_'
///    every four for hex, '_' every eight for binary, and '_' every three
///    for octal.
///
///  - '0': Pad With Zero. This extends integer values with leading zeroes
///    up to the format's width, rather than padding with spaces. These zeroes
///    are delimited if Separate Groups is flagged as well. A group delimiter
///    may cause the result to exceed the format's width by at most one
///    digit. This does nothing if Left Justify or a precision is specified.
///
///  - '_': No Write. This reads a param as the provided type but does not
///    print it, which can be useful to skip params or indexes.
///
/// Indexes specify which parameter to use for the type, width, or precision.
/// For example, `%2$*1$.*3$` would specify width via the first parameter, the
/// value via the second, and precision via the third. Indexes must be at least
/// 1 and no greater than S32_MAX. You cannot skip any indexes due to
/// limitations with C varargs, but you can use the '_' flag to achieve the same
/// effect. You cannot redefine a parameter to be different types-- for example,
/// `%1$*1$f` is invalid because you're defining parameter 1 as both an r64 (the
/// value) and an s32 (width). Note that `%1$*1$d` is valid since both specify
/// s32.
///
/// Type specifies what kind of data a parameter should be printed as. Some
/// types allow uppercase variants-- those will capitalize any relevant
/// alphabetical characters in their outputs. Note that '%C' and '%S' will not
/// capitalize their contents. The following parameters are supported:
///  - %d/%i : Signed decimal integer.
///  - %u    : Unsigned decimal integer.
///  - %o    : Unsigned octal integer. Uses the digits [0-7].
///  - %x/%X : Unsigned hexadecimal integer. Uses the digits [0-9a-f].
///  - %b/%B : Unsigned binary integer. Uses the digits [01].
///  - %f/%F : Standard float. Prints as 'nan', 'inf', or `[-]ddd.ddd`.
///  - %e/%E : Exponential float. Prints as 'nan', 'inf', or `[-]d.ddde[+-]dd`.
///  - %g/%G : Fit float. Prints as exponential if the exponent would be >=
///            precision or < -4. Otherwise, prints as standard.
///  - %a/%A : Hex float. Prints as 'nan', 'inf', or `[-]0xh.hhhp[+-]dd`. The
///            whole digit will always be 1 unless the number is denormal or
///            zero, in which case it will be zero.
///  - %T    : Boolean value. Prints 'true' or 'false'.
///  - %c/%C : Character. Wide characters (%C and %lc) are unicode codepoints.
///  - %s/%S : String. It's transcoded into the output encoding.
///  - %p    : Pointer. Will be treated as %#x regardless of flags or precision.
///  - %n    : Query. The integer it points to will be updated with the current
///            number of characters output at that point.
///
/// Size refers to a character sequence that denotes, in combination with type,
/// what kind of data to interpret each parameter as. The following sizes are
/// supported:
///
/// |      | %d/%i | %u    | %f/%F | %T  | %c  | %C  | %s/%S  |  %p  |  %n  |
/// |      |       | %o    | %e/%E |     |     |     |        |      |      |
/// |      |       | %x/%X | %g/%G |     |     |     |        |      |      |
/// |      |       | %b/%B | %a/%A |     |     |     |        |      |      |
/// |------|-------|-------|-------|-----|-----|-----|--------|------|------|
/// |      |  s32  |  u32  |  r64  | b08 | c08 | c32 | string | vptr | s32* |
/// | hh   |  s08  |  u08  |       |     |     |     |        |      |      |
/// | h    |  s16  |  u16  |       |     |     |     |        |      | s16* |
/// | l    |  s64  |  u64  |       |     | c32 |     | string |      | s64* |
/// | ll   |  s64  |  u64  |       |     |     |     |        |      |      |
/// | q    |  s64  |  u64  |       |     |     |     |        |      |      |
/// | L    |       |       |  r64  |     |     |     |        |      |      |
/// | j    | ssize | usize |       |     |     |     |        |      |      |
/// | t    | ssize | usize |       |     |     |     |        |      |      |
/// | z    | ssize | usize |       |     |     |     |        |      |      |
/// | Z    | ssize | usize |       |     |     |     |        |      |      |
/// | w8   |  s08  |  u08  |       |     |     |     |        |      |      |
/// | w16  |  s16  |  u16  |       |     |     |     |        |      |      |
/// | w32  |  s32  |  u32  |       |     |     |     |        |      |      |
/// | w64  |  s64  |  u64  |       |     |     |     |        |      |      |
/// | wf8  |  s08  |  u08  |       |     |     |     |        |      |      |
/// | wf16 |  s16  |  u16  |       |     |     |     |        |      |      |
/// | wf32 |  s32  |  u32  |       |     |     |     |        |      |      |
/// | wf64 |  s64  |  u64  |       |     |     |     |        |      |      |
///
/// @param[in] ... A vaiadic list of parameters to insert into the format
/// string. These must align with the patterns in `Format` or stack
/// corruption may occur.
/// @return A stack-backed formatted string. If any errors occurred during
/// parsing, this will be a copy of the format string, and an error will be
/// logged.
internal string
FString(string Format, ...)
{
	va_list Args;
	VA_Start(Args, Format);

	string Result = FVString(Format, Args);

	VA_End(Args);
	return Result;
}

#endif	// SECTION_STRING_FORMATTING

/***************************************************************************\
|  SECTION: Tests |
|---------------------------------------------------------------------------|
|  Tests all functionality within this file. |
\***************************************************************************/

#ifndef SECTION_STRING_TESTS

#define STRING_TESTS                                                                                        \
	TEST(FString_ParseFormatInt, ReportsNotPresentOnNonDigit, (                                             \
	    fstring_format_status Result = FString_ParseFormatInt(NULL, NULL);                                  \
		Assert(Result == FSTRING_FORMAT_INT_NOT_PRESENT);                                                   \
		s32 Int = -92;                                                                                      \
	    Result = FString_ParseFormatInt(NULL, &Int);                                                        \
		Assert(Result == FSTRING_FORMAT_INT_NOT_PRESENT);                                                   \
		Assert(Int == -92);                                                                                 \
		string Cursor = CStringL("-23");                                                                    \
	    Result = FString_ParseFormatInt(&Cursor, &Int);                                                     \
		Assert(Result == FSTRING_FORMAT_INT_NOT_PRESENT);                                                   \
		Assert(String_Cmp(Cursor, CStringL("-23")) == 0);                                                   \
		Assert(Int == -92);                                                                                 \
		Cursor = CStringL("-23");                                                                           \
	    Result = FString_ParseFormatInt(&Cursor, NULL);                                                     \
		Assert(Result == FSTRING_FORMAT_INT_NOT_PRESENT);                                                   \
		Assert(String_Cmp(Cursor, CStringL("-23")) == 0);                                                   \
	))                                                                                                      \
	TEST(FString_ParseFormatInt, ReportsNotPresentOnZero, (                                                 \
		s32 Int = -92;                                                                                      \
		string Cursor = CStringL("02");                                                                     \
	    fstring_format_status Result = FString_ParseFormatInt(&Cursor, &Int);                               \
		Assert(Result == FSTRING_FORMAT_INT_NOT_PRESENT);                                                   \
		Assert(String_Cmp(Cursor, CStringL("02")) == 0);                                                    \
		Assert(Int == -92);                                                                                 \
		Cursor = CStringL("02");                                                                            \
	    Result = FString_ParseFormatInt(&Cursor, NULL);                                                     \
		Assert(Result == FSTRING_FORMAT_INT_NOT_PRESENT);                                                   \
		Assert(String_Cmp(Cursor, CStringL("02")) == 0);                                                    \
	))                                                                                                      \
	TEST(FString_ParseFormatInt, ReportsIntOverflowOnTooLarge, (                                            \
		s32 Int = -92;                                                                                      \
		string Cursor = CStringL("2147483648");                                                             \
	    fstring_format_status Result = FString_ParseFormatInt(&Cursor, &Int);                               \
		Assert(Result == FSTRING_FORMAT_INT_OVERFLOW);                                                      \
		Assert(String_Cmp(Cursor, CStringL("8")) == 0);                                                     \
		Assert(Int == -92);                                                                                 \
		Cursor = CStringL("2147483648");                                                                    \
	    Result = FString_ParseFormatInt(&Cursor, NULL);                                                     \
		Assert(Result == FSTRING_FORMAT_INT_OVERFLOW);                                                      \
		Assert(String_Cmp(Cursor, CStringL("8")) == 0);                                                     \
	))                                                                                                      \
	TEST(FString_ParseFormatInt, ReturnsValidOnSuccesssfulParse, (                                          \
		s32 Int = -92;                                                                                      \
		string Cursor = CStringL("2147483647");                                                             \
	    fstring_format_status Result = FString_ParseFormatInt(&Cursor, &Int);                               \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Cursor, CStringL("")) == 0);                                                      \
		Assert(Int == 2147483647);                                                                          \
		Cursor = CStringL("1");                                                                             \
	    Result = FString_ParseFormatInt(&Cursor, &Int);                                                     \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Cursor, CStringL("")) == 0);                                                      \
		Assert(Int == 1);                                                                                   \
		Cursor = CStringL("1");                                                                             \
	    Result = FString_ParseFormatInt(&Cursor, NULL);                                                     \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Cursor, CStringL("")) == 0);                                                      \
	))                                                                                                      \
	TEST(FString_ParseFormatIndex, PropagatesIndexOverflow, (                                               \
		s32 Index = -92;                                                                                    \
		b08 UseIndexes = 2;                                                                                 \
		string Cursor = CStringL("2147483648");                                                             \
	    fstring_format_status Result = FString_ParseFormatIndex(&Cursor, &Index, &UseIndexes, TRUE);        \
		Assert(Result == FSTRING_FORMAT_INT_OVERFLOW);                                                      \
		Assert(String_Cmp(Cursor, CStringL("8")) == 0);                                                     \
		Assert(UseIndexes == 2);                                                                            \
		Assert(Index == -92);                                                                               \
		Cursor = CStringL("2147483648");                                                                    \
	    Result = FString_ParseFormatIndex(&Cursor, NULL, &UseIndexes, TRUE);                                \
		Assert(Result == FSTRING_FORMAT_INT_OVERFLOW);                                                      \
		Assert(String_Cmp(Cursor, CStringL("8")) == 0);                                                     \
	))                                                                                                      \
	TEST(FString_ParseFormatIndex, ReportsInconsistentIfIndexNotPresentAndExpected, (                       \
		b08 UseIndexes = TRUE;                                                                              \
		s32 Index = -92;                                                                                    \
		string Cursor = CStringL("-2$");                                                                    \
	    fstring_format_status Result = FString_ParseFormatIndex(&Cursor, &Index, &UseIndexes, FALSE);       \
		Assert(Result == FSTRING_FORMAT_INDEX_INCONSISTENT);                                                \
		Assert(String_Cmp(Cursor, CStringL("-2$")) == 0);                                                   \
		Assert(UseIndexes == TRUE);                                                                         \
		Assert(Index == -92);                                                                               \
	    Result = FString_ParseFormatIndex(NULL, NULL, &UseIndexes, FALSE);                                  \
		Assert(Result == FSTRING_FORMAT_INDEX_INCONSISTENT);                                                \
		Assert(UseIndexes == TRUE);                                                                         \
	))                                                                                                      \
	TEST(FString_ParseFormatIndex, ReturnsValidIfIndexNotPresentAndProhibited, (                            \
		b08 UseIndexes = FALSE;                                                                             \
		s32 Index = -92;                                                                                    \
		string Cursor = CStringL("-2$");                                                                    \
	    fstring_format_status Result = FString_ParseFormatIndex(&Cursor, &Index, &UseIndexes, FALSE);       \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Cursor, CStringL("-2$")) == 0);                                                   \
		Assert(UseIndexes == FALSE);                                                                        \
		Assert(Index == -92);                                                                               \
	    Result = FString_ParseFormatIndex(NULL, NULL, &UseIndexes, FALSE);                                  \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Cursor, CStringL("-2$")) == 0);                                                   \
		Assert(UseIndexes == FALSE);                                                                        \
	))                                                                                                      \
	TEST(FString_ParseFormatIndex, UpdatesUseIndexesIfIndexNotPresentAndUseIndexesUndefined, (              \
		b08 UseIndexes = 2;                                                                                 \
		s32 Index = -92;                                                                                    \
		string Cursor = CStringL("-2$");                                                                    \
	    fstring_format_status Result = FString_ParseFormatIndex(&Cursor, &Index, &UseIndexes, TRUE);        \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Cursor, CStringL("-2$")) == 0);                                                   \
		Assert(UseIndexes == FALSE);                                                                        \
		Assert(Index == -92);                                                                               \
	))                                                                                                      \
	TEST(FString_ParseFormatIndex, ReportsInconsistentIfIndexPresentAndProhibited, (                        \
		b08 UseIndexes = FALSE;                                                                             \
		s32 Index = -92;                                                                                    \
		string Cursor = CStringL("2$");                                                                     \
	    fstring_format_status Result = FString_ParseFormatIndex(&Cursor, &Index, &UseIndexes, FALSE);       \
		Assert(Result == FSTRING_FORMAT_INDEX_INCONSISTENT);                                                \
		Assert(String_Cmp(Cursor, CStringL("$")) == 0);                                                     \
		Assert(UseIndexes == FALSE);                                                                        \
		Assert(Index == 2);                                                                                 \
	))                                                                                                      \
	TEST(FString_ParseFormatIndex, ReturnsValidIfIndexIsValid, (                                            \
		b08 UseIndexes = TRUE;                                                                              \
		s32 Index = -92;                                                                                    \
		string Cursor = CStringL("2$");                                                                     \
	    fstring_format_status Result = FString_ParseFormatIndex(&Cursor, &Index, &UseIndexes, FALSE);       \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Cursor, CStringL("")) == 0);                                                      \
		Assert(UseIndexes == TRUE);                                                                         \
		Assert(Index == 2);                                                                                 \
	))                                                                                                      \
	TEST(FString_ParseFormatIndex, UpdatesUseIndexesIfIndexValidAndUseIndexesUndefined, (                   \
		b08 UseIndexes = FALSE;                                                                             \
		s32 Index = -92;                                                                                    \
		string Cursor = CStringL("2$");                                                                     \
	    fstring_format_status Result = FString_ParseFormatIndex(&Cursor, &Index, &UseIndexes, TRUE);        \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Cursor, CStringL("")) == 0);                                                      \
		Assert(UseIndexes == TRUE);                                                                         \
		Assert(Index == 2);                                                                                 \
	))                                                                                                      \
	TEST(FString_ParseFormatIndex, ReportsNotPresentIfNoDollarSignFollows, (                                \
		b08 UseIndexes = 2;                                                                                 \
		s32 Index = -92;                                                                                    \
		string Cursor = CStringL("2");                                                                      \
	    fstring_format_status Result = FString_ParseFormatIndex(&Cursor, &Index, &UseIndexes, TRUE);        \
		Assert(Result == FSTRING_FORMAT_INDEX_NOT_PRESENT);                                                 \
		Assert(String_Cmp(Cursor, CStringL("")) == 0);                                                      \
		Assert(UseIndexes == 2);                                                                            \
		Assert(Index == 2);                                                                                 \
	))                                                                                                      \
	TEST(FString_ParseFormatFlags, ReturnsValidAndKeepsExistingDataInOutput, (                              \
		fstring_format_type Flags = 29;                                                                     \
		string Cursor = CStringL("...");                                                                    \
	    fstring_format_status Result = FString_ParseFormatFlags(&Cursor, &Flags);                           \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Cursor, CStringL("...")) == 0);                                                   \
		Assert(Flags == 29);                                                                                \
	))                                                                                                      \
	TEST(FString_ParseFormatFlags, HyphenIsLeftJustify, (                                                   \
		fstring_format_type Flags = 0;                                                                      \
		string Cursor = CStringL("-");                                                                      \
	    FString_ParseFormatFlags(&Cursor, &Flags);                                                          \
		Assert(String_Cmp(Cursor, CStringL("")) == 0);                                                      \
		Assert(Flags == FSTRING_FORMAT_FLAG_LEFT_JUSTIFY);                                                  \
	))                                                                                                      \
	TEST(FString_ParseFormatFlags, PlusIsPrefixSign, (                                                      \
		fstring_format_type Flags = 0;                                                                      \
		string Cursor = CStringL("+");                                                                      \
	    FString_ParseFormatFlags(&Cursor, &Flags);                                                          \
		Assert(String_Cmp(Cursor, CStringL("")) == 0);                                                      \
		Assert(Flags == FSTRING_FORMAT_FLAG_PREFIX_SIGN);                                                   \
	))                                                                                                      \
	TEST(FString_ParseFormatFlags, SpaceIsPrefixSpace, (                                                    \
		fstring_format_type Flags = 0;                                                                      \
		string Cursor = CStringL(" ");                                                                      \
	    FString_ParseFormatFlags(&Cursor, &Flags);                                                          \
		Assert(String_Cmp(Cursor, CStringL("")) == 0);                                                      \
		Assert(Flags == FSTRING_FORMAT_FLAG_PREFIX_SPACE);                                                  \
	))                                                                                                      \
	TEST(FString_ParseFormatFlags, HashIsSpecifyRadix, (                                                    \
		fstring_format_type Flags = 0;                                                                      \
		string Cursor = CStringL("#");                                                                      \
	    FString_ParseFormatFlags(&Cursor, &Flags);                                                          \
		Assert(String_Cmp(Cursor, CStringL("")) == 0);                                                      \
		Assert(Flags == FSTRING_FORMAT_FLAG_SPECIFY_RADIX);                                                 \
	))                                                                                                      \
	TEST(FString_ParseFormatFlags, ApostropheIsSeparateGroups, (                                            \
		fstring_format_type Flags = 0;                                                                      \
		string Cursor = CStringL("'");                                                                      \
	    FString_ParseFormatFlags(&Cursor, &Flags);                                                          \
		Assert(String_Cmp(Cursor, CStringL("")) == 0);                                                      \
		Assert(Flags == FSTRING_FORMAT_FLAG_SEPARATE_GROUPS);                                               \
	))                                                                                                      \
	TEST(FString_ParseFormatFlags, ZeroIsPadWithZero, (                                                     \
		fstring_format_type Flags = 0;                                                                      \
		string Cursor = CStringL("0");                                                                      \
	    FString_ParseFormatFlags(&Cursor, &Flags);                                                          \
		Assert(String_Cmp(Cursor, CStringL("")) == 0);                                                      \
		Assert(Flags == FSTRING_FORMAT_FLAG_PAD_WITH_ZERO);                                                 \
	))                                                                                                      \
	TEST(FString_ParseFormatFlags, UnderscoreIsNoWrite, (                                                   \
		fstring_format_type Flags = 0;                                                                      \
		string Cursor = CStringL("_");                                                                      \
	    FString_ParseFormatFlags(&Cursor, &Flags);                                                          \
		Assert(String_Cmp(Cursor, CStringL("")) == 0);                                                      \
		Assert(Flags == FSTRING_FORMAT_FLAG_NO_WRITE);                                                      \
	))                                                                                                      \
	TEST(FString_ParseFormatFlags, IgnoresDuplicates, (                                                     \
		fstring_format_type Flags = FSTRING_FORMAT_TYPE_MASK;                                               \
		string Cursor = CStringL("-+++00-");                                                                \
	    fstring_format_status Result = FString_ParseFormatFlags(&Cursor, &Flags);                           \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Cursor, CStringL("")) == 0);                                                      \
		Assert(Flags == (FSTRING_FORMAT_TYPE_MASK | FSTRING_FORMAT_FLAG_LEFT_JUSTIFY                        \
			| FSTRING_FORMAT_FLAG_PREFIX_SIGN | FSTRING_FORMAT_FLAG_PAD_WITH_ZERO));                        \
	    Result = FString_ParseFormatFlags(NULL, NULL);                                                      \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
	))                                                                                                      \
	TEST(FString_ParseFormatType, ReportsInvalidOnInvalidChars, (                                           \
		fstring_format_type Type = -92;                                                                     \
		string Cursor = CStringL("hhc");                                                                    \
	    fstring_format_status Result = FString_ParseFormatType(&Cursor, &Type);                             \
		Assert(Result == FSTRING_FORMAT_TYPE_INVALID);                                                      \
		Assert(String_Cmp(Cursor, CStringL("c")) == 0);                                                     \
		Assert(Type == -92);                                                                                \
	    Result = FString_ParseFormatType(NULL, NULL);                                                       \
		Assert(Result == FSTRING_FORMAT_TYPE_INVALID);                                                      \
	))                                                                                                      \
	TEST(FString_ParseFormatType, KeepsExistingDataInType, (                                                \
		fstring_format_type Type = ~FSTRING_FORMAT_TYPE_MASK;                                               \
		string Cursor = CStringL("d");                                                                      \
	    fstring_format_status Result = FString_ParseFormatType(&Cursor, &Type);                             \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Cursor, CStringL("")) == 0);                                                      \
		Assert(Type == (~FSTRING_FORMAT_TYPE_MASK | FSTRING_FORMAT_TYPE_S32));                              \
		Cursor = CStringL("d");                                                                             \
	    Result = FString_ParseFormatType(&Cursor, NULL);                                                    \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
	))                                                                                                      \
	TEST(FString_ParseFormatType, ParsesFullType, (                                                         \
		fstring_format_type Type = 0;                                                                       \
		string Cursor = CStringL("wf64x");                                                                  \
	    fstring_format_status Result = FString_ParseFormatType(&Cursor, &Type);                             \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Cursor, CStringL("")) == 0);                                                      \
		Assert(Type == (FSTRING_FORMAT_TYPE_U64 | FSTRING_FORMAT_FLAG_INT_HEX));                            \
	))                                                                                                      \
	TEST(FString_ParseFormatType, FlagsUppercaseForRespectiveTypes, (                                       \
		fstring_format_type Type = 0;                                                                       \
		string Cursor = CStringL("BXFEGACSTZdLd");                                                          \
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
		Assert(Type == (FSTRING_FORMAT_TYPE_STR));                                                          \
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
		string Cursor = CStringL("%d");                                                                     \
		fstring_format Format;                                                                              \
	    fstring_format_status Result = FString_ParseFormat(&Cursor, &Format, &UseIndexes, TRUE);            \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Cursor, CStringL("")) == 0);                                                      \
		Assert(UseIndexes == FALSE);                                                                        \
		Assert(Format.Type == FSTRING_FORMAT_TYPE_S32);                                                     \
		Assert(Format.ValueIndex == 0);                                                                     \
		Assert(Format.Width == 0);                                                                          \
		Assert(Format.Precision == -1);                                                                     \
		Assert(String_Cmp(Format.SpecifierString, CStringL("%d")) == 0);                                    \
		Assert(Format.ActualWidth == 0);                                                                    \
		Cursor = CStringL("%d");                                                                            \
	    Result = FString_ParseFormat(&Cursor, NULL, &UseIndexes, FALSE);                                    \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
	))                                                                                                      \
	TEST(FString_ParseFormat, ReportsIndexInconsistentWhenIndexPresentAndProhibited, (                      \
		b08 UseIndexes = FALSE;                                                                             \
		string Cursor = CStringL("%1$d");                                                                   \
		fstring_format Format;                                                                              \
	    fstring_format_status Result = FString_ParseFormat(&Cursor, &Format, &UseIndexes, FALSE);           \
		Assert(Result == FSTRING_FORMAT_INDEX_INCONSISTENT);                                                \
		Assert(String_Cmp(Cursor, CStringL("$d")) == 0);                                                    \
		Assert(UseIndexes == FALSE);                                                                        \
	))                                                                                                      \
	TEST(FString_ParseFormat, ReportsIndexInconsistentWhenIndexNotPresentAndExpected, (                     \
		b08 UseIndexes = TRUE;                                                                              \
		string Cursor = CStringL("%d");                                                                     \
		fstring_format Format;                                                                              \
	    fstring_format_status Result = FString_ParseFormat(&Cursor, &Format, &UseIndexes, FALSE);           \
		Assert(Result == FSTRING_FORMAT_INDEX_INCONSISTENT);                                                \
		Assert(String_Cmp(Cursor, CStringL("d")) == 0);                                                     \
		Assert(UseIndexes == TRUE);                                                                         \
	))                                                                                                      \
	TEST(FString_ParseFormat, MaximalCaseWithExplicitNumbers, (                                             \
		b08 UseIndexes = TRUE;                                                                              \
		string FormatStr = CStringL("%--++  ''##002147483647.2147483647wf64d");                             \
		string Cursor = FormatStr;                                                                          \
		fstring_format Format;                                                                              \
	    fstring_format_status Result = FString_ParseFormat(&Cursor, &Format, &UseIndexes, TRUE);            \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Cursor, CStringL("")) == 0);                                                      \
		Assert(UseIndexes == FALSE);                                                                        \
		Assert(Format.Type == (FSTRING_FORMAT_TYPE_S64 | FSTRING_FORMAT_FLAG_LEFT_JUSTIFY                   \
			| FSTRING_FORMAT_FLAG_PREFIX_SIGN | FSTRING_FORMAT_FLAG_PREFIX_SPACE                            \
			| FSTRING_FORMAT_FLAG_SPECIFY_RADIX | FSTRING_FORMAT_FLAG_SEPARATE_GROUPS                       \
			| FSTRING_FORMAT_FLAG_PAD_WITH_ZERO));                                                          \
		Assert(Format.ValueIndex == 0);                                                                     \
		Assert(Format.Width == 2147483647);                                                                 \
		Assert(Format.Precision == 2147483647);                                                             \
		Assert(String_Cmp(Format.SpecifierString, FormatStr) == 0);                                         \
		Assert(Format.ActualWidth == 0);                                                                    \
	))                                                                                                      \
	TEST(FString_ParseFormat, MaximalCaseWithParamNumbers, (                                                \
		b08 UseIndexes = TRUE;                                                                              \
		string FormatStr = CStringL("%--++  ''##00*.*wf64d");                                               \
		string Cursor = FormatStr;                                                                          \
		fstring_format Format;                                                                              \
	    fstring_format_status Result = FString_ParseFormat(&Cursor, &Format, &UseIndexes, TRUE);            \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Cursor, CStringL("")) == 0);                                                      \
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
		string FormatStr = CStringL("%2147483647$--++  ''##00*2147483647$.*2147483647$wf64d");              \
		string Cursor = FormatStr;                                                                          \
		fstring_format Format;                                                                              \
	    fstring_format_status Result = FString_ParseFormat(&Cursor, &Format, &UseIndexes, TRUE);            \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Cursor, CStringL("")) == 0);                                                      \
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
		string FormatStr = CStringL("%.d");                                                                 \
		string Cursor = FormatStr;                                                                          \
		fstring_format Format;                                                                              \
	    fstring_format_status Result = FString_ParseFormat(&Cursor, &Format, &UseIndexes, FALSE);           \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(Format.Precision == 0);                                                                      \
	))                                                                                                      \
	TEST(FString_ParseFormat, PropagatesIntOverflow, (                                                      \
		b08 UseIndexes = TRUE;                                                                              \
		string Cursor = CStringL("%2147483648$d");                                                          \
		fstring_format Format;                                                                              \
	    fstring_format_status Result = FString_ParseFormat(&Cursor, &Format, &UseIndexes, TRUE);            \
		Assert(Result == FSTRING_FORMAT_INT_OVERFLOW);                                                      \
		Assert(String_Cmp(Cursor, CStringL("8$d")) == 0);                                                   \
		Cursor = CStringL("%2147483648d");                                                                  \
	    Result = FString_ParseFormat(&Cursor, &Format, &UseIndexes, TRUE);                                  \
		Assert(Result == FSTRING_FORMAT_INT_OVERFLOW);                                                      \
		Assert(String_Cmp(Cursor, CStringL("8d")) == 0);                                                    \
		Cursor = CStringL("%*2147483648$d");                                                                \
	    Result = FString_ParseFormat(&Cursor, &Format, &UseIndexes, TRUE);                                  \
		Assert(Result == FSTRING_FORMAT_INT_OVERFLOW);                                                      \
		Assert(String_Cmp(Cursor, CStringL("8$d")) == 0);                                                   \
		Cursor = CStringL("%.2147483648d");                                                                 \
	    Result = FString_ParseFormat(&Cursor, &Format, &UseIndexes, TRUE);                                  \
		Assert(Result == FSTRING_FORMAT_INT_OVERFLOW);                                                      \
		Assert(String_Cmp(Cursor, CStringL("8d")) == 0);                                                    \
		Cursor = CStringL("%.*2147483648$d");                                                               \
	    Result = FString_ParseFormat(&Cursor, &Format, &UseIndexes, TRUE);                                  \
		Assert(Result == FSTRING_FORMAT_INT_OVERFLOW);                                                      \
		Assert(String_Cmp(Cursor, CStringL("8$d")) == 0);                                                   \
	))                                                                                                      \
	TEST(FString_ParseFormat, PropagatesInvalidType, (                                                      \
		b08 UseIndexes = FALSE;                                                                             \
		string Cursor = CStringL("%hhc");                                                                   \
		fstring_format Format;                                                                              \
	    fstring_format_status Result = FString_ParseFormat(&Cursor, &Format, &UseIndexes, TRUE);            \
		Assert(Result == FSTRING_FORMAT_TYPE_INVALID);                                                      \
		Assert(String_Cmp(Cursor, CStringL("c")) == 0);                                                     \
	))                                                                                                      \
	TEST(FString_ParseFormatString, AllowsNullArgumentsButWhy, (                                            \
	    fstring_format_status Result = FString_ParseFormatString(NULL, NULL);                               \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		string Cursor = CStringL("...%d...");                                                               \
	    Result = FString_ParseFormatString(&Cursor, NULL);                                                  \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
	))                                                                                                      \
	TEST(FString_ParseFormatString, IgnoresEscapedPercentSigns, (                                           \
		usize StackCursor = (usize) Stack_GetCursor();                                                      \
		string FormatStr = CStringL("%d%%%s");                                                              \
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
		Assert(FormatList.TotalTextSize == 0);                                                              \
		Assert(FormatList.Formats[0].Type == FSTRING_FORMAT_TYPE_S32);                                      \
		Assert(FormatList.Formats[0].ValueIndex == 1);                                                      \
		Assert(FormatList.Formats[1].Type == FSTRING_FORMAT_TYPE_STR);                                      \
		Assert(FormatList.Formats[1].ValueIndex == 2);                                                      \
		Assert((usize) Stack_GetCursor() == StackCursor + 2 * sizeof(fstring_format) + sizeof(string));     \
	))                                                                                                      \
	TEST(FString_ParseFormatString, NoAllocationIfNoFormats, (                                              \
		usize StackCursor = (usize) Stack_GetCursor();                                                      \
		string FormatStr = CStringL("%%d%%%%s");                                                            \
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
		Assert((usize) Stack_GetCursor() == StackCursor);                                                   \
	))                                                                                                      \
	TEST(FString_ParseFormatString, ExplicitParamIndexesAreCorrect, (                                       \
		string Cursor = CStringL("%2$d%1$s%4$*3$.*3$f");                                                    \
		fstring_format_list FormatList;                                                                     \
	    fstring_format_status Result = FString_ParseFormatString(&Cursor, &FormatList);                     \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(FormatList.FormatCount == 3);                                                                \
		Assert(FormatList.ParamCount == 4);                                                                 \
		Assert(FormatList.Formats[0].ValueIndex == 2);                                                      \
		Assert(FormatList.Formats[1].ValueIndex == 1);                                                      \
		Assert(FormatList.Formats[2].ValueIndex == 4);                                                      \
		Assert(FormatList.Formats[2].WidthIndex == 3);                                                      \
		Assert(FormatList.Formats[2].PrecisionIndex == 3);                                                  \
		Assert(String_Cmp(FormatList.Formats[0].SpecifierString, CStringL("%2$d")) == 0);                   \
		Assert(String_Cmp(FormatList.Formats[1].SpecifierString, CStringL("%1$s")) == 0);                   \
		Assert(String_Cmp(FormatList.Formats[2].SpecifierString, CStringL("%4$*3$.*3$f")) == 0);            \
	))                                                                                                      \
	TEST(FString_ParseFormatString, ImplicitParamIndexesAreCorrect, (                                       \
		string Cursor = CStringL("%d %*s %.*f %*.*x");                                                      \
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
		Assert(String_Cmp(FormatList.Formats[0].SpecifierString, CStringL("%d")) == 0);                     \
		Assert(String_Cmp(FormatList.Formats[1].SpecifierString, CStringL("%*s")) == 0);                    \
		Assert(String_Cmp(FormatList.Formats[2].SpecifierString, CStringL("%.*f")) == 0);                   \
		Assert(String_Cmp(FormatList.Formats[3].SpecifierString, CStringL("%*.*x")) == 0);                  \
	))                                                                                                      \
	TEST(FString_ParseFormatString, ReportsInconsistentIndexes, (                                           \
		string FormatStr = CStringL("%2$d %s %3$f");                                                        \
		string Cursor = FormatStr;                                                                          \
		fstring_format_list FormatList;                                                                     \
	    fstring_format_status Result = FString_ParseFormatString(&Cursor, &FormatList);                     \
		Assert(Result == FSTRING_FORMAT_INDEX_INCONSISTENT);                                                \
		Assert(String_Cmp(Cursor, CStringL("s %3$f")) == 0);                                                \
		Cursor = CStringL("%d %1$s %f");                                                                    \
	    Result = FString_ParseFormatString(&Cursor, &FormatList);                                           \
		Assert(Result == FSTRING_FORMAT_INDEX_INCONSISTENT);                                                \
		Assert(String_Cmp(Cursor, CStringL("$s %f")) == 0);                                                 \
	))                                                                                                      \
	TEST(FString_ParseFormatString, PropagatesIntOverflow, (                                                \
		string Cursor = CStringL("%d %3000000000s %f");                                                     \
		fstring_format_list FormatList;                                                                     \
	    fstring_format_status Result = FString_ParseFormatString(&Cursor, &FormatList);                     \
		Assert(Result == FSTRING_FORMAT_INT_OVERFLOW);                                                      \
		Assert(String_Cmp(Cursor, CStringL("0s %f")) == 0);                                                 \
	))                                                                                                      \
	TEST(FString_ParseFormatString, PropagatesInvalidType, (                                                \
		string Cursor = CStringL("%d %y %f");                                                               \
		fstring_format_list FormatList;                                                                     \
	    fstring_format_status Result = FString_ParseFormatString(&Cursor, &FormatList);                     \
		Assert(Result == FSTRING_FORMAT_TYPE_INVALID);                                                      \
		Assert(String_Cmp(Cursor, CStringL("y %f")) == 0);                                                  \
	))                                                                                                      \
	TEST(FString_UpdateParamReferences, ReportsRedefinedParamsWithinSpecifier, (                            \
		string FormatStr = CStringL("%2$d %1$.*1$f %%");                                                    \
		fstring_format_list FormatList;                                                                     \
	    fstring_format_status Result = FString_ParseFormatString(&FormatStr, &FormatList);                  \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		usize Cursor = (usize) Stack_GetCursor();                                                           \
	    Result = FString_UpdateParamReferences(&FormatList, 1.2f, 23);                                      \
		Assert(Result == FSTRING_FORMAT_PARAM_REDEFINED);                                                   \
		Assert(String_Cmp(FormatList.FormatString, CStringL("%1$.*1$f")) == 0);                             \
		Assert(FormatList.FormatCount == 1);                                                                \
		Assert((usize) Stack_GetCursor() == Cursor);                                                        \
	))                                                                                                      \
	TEST(FString_UpdateParamReferences, ReportsRedefinedParamsBetweenSpecifiers, (                          \
		string FormatStr = CStringL("%2$d %2$.*1$f %%");                                                    \
		fstring_format_list FormatList;                                                                     \
	    fstring_format_status Result = FString_ParseFormatString(&FormatStr, &FormatList);                  \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
	    Result = FString_UpdateParamReferences(&FormatList, 23, 1.2f);                                      \
		Assert(Result == FSTRING_FORMAT_PARAM_REDEFINED);                                                   \
		Assert(String_Cmp(FormatList.FormatString, CStringL("%2$.*1$f")) == 0);                             \
		Assert(FormatList.FormatCount == 1);                                                                \
	))                                                                                                      \
	TEST(FString_UpdateParamReferences, ReportsSkippedParams, (                                             \
		string FormatStr = CStringL("%3$d %1$f");                                                           \
		fstring_format_list FormatList;                                                                     \
	    fstring_format_status Result = FString_ParseFormatString(&FormatStr, &FormatList);                  \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
	    usize Cursor = (usize) Stack_GetCursor();                                                           \
	    Result = FString_UpdateParamReferences(&FormatList, 23, 1.2f);                                      \
		Assert(Result == FSTRING_FORMAT_INDEX_NOT_PRESENT);                                                 \
		Assert(String_Cmp(FormatList.FormatString, FormatStr) == 0);                                        \
		Assert(FormatList.FormatCount == 2);                                                                \
		Assert((usize) Stack_GetCursor() == Cursor);                                                        \
	))                                                                                                      \
	TEST(FString_UpdateParamReferences, UpdatesValuesAndReturnsValid, (                                     \
		string FormatStr = CStringL("%*d %s");                                                              \
		fstring_format_list FormatList;                                                                     \
	    fstring_format_status Result = FString_ParseFormatString(&FormatStr, &FormatList);                  \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
	    usize Cursor = (usize) Stack_GetCursor();                                                           \
	    Result = FString_UpdateParamReferences(&FormatList, 23, 2, CStringL("Hi!"));                        \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(FormatList.FormatString, FormatStr) == 0);                                        \
		Assert(FormatList.Formats[0].Value.Signed == 23);                                                   \
		Assert(FormatList.Formats[0].Width == 2);                                                           \
		Assert(FormatList.Formats[1].Value.String == FormatList.ExtraData);                                 \
		Assert(FormatList.ExtraData = FormatList.Formats[1].Value.String + 1);                              \
		Assert(String_Cmp(*FormatList.Formats[1].Value.String, CStringL("Hi!")) == 0);                      \
		Assert(FormatList.FormatCount == 2);                                                                \
		Assert((usize) Stack_GetCursor() == Cursor);                                                        \
	))                                                                                                      \
	TEST(FString_UpdateParamReferences, ReportsOverflowedWidthParam, (                                      \
		string FormatStr = CStringL("%*d %s");                                                              \
		fstring_format_list FormatList;                                                                     \
	    fstring_format_status Result = FString_ParseFormatString(&FormatStr, &FormatList);                  \
		Assert(Result == FSTRING_FORMAT_VALID);                                                             \
	    usize Cursor = (usize) Stack_GetCursor();                                                           \
	    Result = FString_UpdateParamReferences(&FormatList, 23, S32_MIN, CStringL("Hi!"));                  \
		Assert(Result == FSTRING_FORMAT_INT_OVERFLOW);                                                      \
		Assert(String_Cmp(FormatList.FormatString, CStringL("%*d")) == 0);                                  \
		Assert(FormatList.Formats[0].Value.Signed == 23);                                                   \
		Assert(FormatList.Formats[0].Width == S32_MIN);                                                     \
		Assert(FormatList.FormatCount == 0);                                                                \
		Assert((usize) Stack_GetCursor() == Cursor);                                                        \
	))                                                                                                      \
	TEST(FString_WriteString, UpdatesLengthAndReturnsWithEmptyBuffer, (                                     \
		string String = CStringL("Hello!");                                                                 \
		fstring_format Format = { .Width = 8, .Value = { .String = &String } };                             \
		fstring_format_status Status = FString_WriteString(&Format, EString());                             \
		Assert(Status == FSTRING_FORMAT_BUFFER_TOO_SMALL);                                                  \
		Assert(Format.ActualWidth == 8);                                                                    \
		Format = (fstring_format){ .Width = 4, .Value = { .String = &String } };                            \
		Status = FString_WriteString(&Format, EString());                                                   \
		Assert(Status == FSTRING_FORMAT_BUFFER_TOO_SMALL);                                                  \
		Assert(Format.ActualWidth == 6);                                                                    \
	))                                                                                                      \
	TEST(FString_WriteString, RespectsSourceEncoding, (                                                     \
		string String = CStringL_UTF32("Hi!");                                                              \
		string Buffer = LString(8);                                                                         \
		Buffer.Encoding = STRING_ENCODING_UTF8;                                                             \
		fstring_format Format = { .Width = 8, .Value = { .String = &String } };                             \
		fstring_format_status Status = FString_WriteString(&Format, Buffer);                                \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("     Hi!")) == 0);                                              \
		String = CStringL_UTF16("Hi!");                                                                     \
		Format = (fstring_format){ .Width = 8, .Value = { .String = &String } };                            \
		Status = FString_WriteString(&Format, Buffer);                                                      \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("     Hi!")) == 0);                                              \
	))                                                                                                      \
	TEST(FString_WriteString, RespectsDestEncoding, (                                                       \
		string String = CStringL_UTF8("Hi!");                                                               \
		string Buffer = LString(16);                                                                        \
		Buffer.Encoding = STRING_ENCODING_UTF32;                                                            \
		fstring_format Format = { .Width = 4, .Value = { .String = &String } };                             \
		fstring_format_status Status = FString_WriteString(&Format, Buffer);                                \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL_UTF32(" Hi!")) == 0);                                            \
		Buffer.Encoding = STRING_ENCODING_UTF16;                                                            \
		Buffer.Length = 8;                                                                                  \
		Format = (fstring_format){ .Width = 4, .Value = { .String = &String } };                            \
		Status = FString_WriteString(&Format, Buffer);                                                      \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL_UTF16(" Hi!")) == 0);                                            \
	))                                                                                                      \
	TEST(FString_WriteString, HandlesLeftAlign, (                                                           \
		string String = CStringL("Hi!");                                                                    \
		string Buffer = LString(8);                                                                         \
		fstring_format Format = { .Width = 8, .Value = { .String = &String } };                             \
		Format.Type = FSTRING_FORMAT_FLAG_LEFT_JUSTIFY;                                                     \
		fstring_format_status Status = FString_WriteString(&Format, Buffer);                                \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("Hi!     ")) == 0);                                              \
		String = CStringL("");                                                                              \
		Format = (fstring_format){ .Width = 8, .Value = { .String = &String } };                            \
		Format.Type = FSTRING_FORMAT_FLAG_LEFT_JUSTIFY;                                                     \
		Status = FString_WriteString(&Format, Buffer);                                                      \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("        ")) == 0);                                              \
	))                                                                                                      \
	TEST(FString_WriteChar, RespectsWideFlag, (                                                             \
		string Buffer = LString(4);                                                                         \
		Buffer.Encoding = STRING_ENCODING_UTF8;                                                             \
		fstring_format Format = { .Type = FSTRING_FORMAT_FLAG_CHR_WIDE,                                     \
			.Value = { .Char = U'\U0010FFFF' } };                                                           \
		fstring_format_status Status = FString_WriteChar(&Format, Buffer);                                  \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL_UTF8("\U0010FFFF")) == 0);                                       \
		Format = (fstring_format){ .Value = { .Char = U'\U0010FFFF' } };                                    \
		Mem_Set(Buffer.Text, 0, Buffer.Length);                                                             \
		Status = FString_WriteChar(&Format, Buffer);                                                        \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(Buffer.Text[0] == 0xC3);                                                                     \
		Assert(Buffer.Text[1] == 0xBF);                                                                     \
		Assert(Buffer.Text[2] == 0);                                                                        \
		Assert(Buffer.Text[3] == 0);                                                                        \
	))                                                                                                      \
	TEST(FString_WriteChar, HandlesLeftAlignAndPadding, (                                                   \
		string Buffer = LString(8);                                                                         \
		fstring_format Format = { .Width = 8, .Value = { .Char = 'T' } };                                   \
		Format.Type = FSTRING_FORMAT_FLAG_LEFT_JUSTIFY;                                                     \
		fstring_format_status Status = FString_WriteChar(&Format, Buffer);                                  \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("T       ")) == 0);                                              \
		Buffer.Length = 3;                                                                                  \
		Format = (fstring_format){ .Width = 3, .Value = { .Char = '?' } };                                  \
		Status = FString_WriteChar(&Format, Buffer);                                                        \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("  ?")) == 0);                                                   \
	))                                                                                                      \
	TEST(FString_WriteBool, WorksAndHandlesLeftAlignAndPadding, (                                           \
		string Buffer = LString(8);                                                                         \
		fstring_format Format = { .Width = 8, .Value = { .Bool = TRUE } };                                  \
		Format.Type = FSTRING_FORMAT_FLAG_LEFT_JUSTIFY;                                                     \
		fstring_format_status Status = FString_WriteBool(&Format, Buffer);                                  \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("true    ")) == 0);                                              \
		Buffer.Length = 6;                                                                                  \
		Format = (fstring_format){ .Width = 6, .Value = { .Bool = FALSE } };                                \
		Status = FString_WriteBool(&Format, Buffer);                                                        \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL(" false")) == 0);                                                \
	))                                                                                                      \
	TEST(FString_WriteUnsigned, PrintsDecimal, (                                                            \
		string Buffer = LString(1);                                                                         \
		fstring_format Format = { .Precision = -1, .Value = { .Unsigned = 0 } };                            \
		fstring_format_status Status = FString_WriteUnsigned(&Format, Buffer);                              \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("0")) == 0);                                                     \
		Format = (fstring_format){ .Precision = -1, .Value = { .Unsigned = U64_MAX } };                     \
		Buffer = LString(20);                                                                               \
		Status = FString_WriteUnsigned(&Format, Buffer);                                                    \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("18446744073709551615")) == 0);                                  \
	))                                                                                                      \
	TEST(FString_WriteUnsigned, PrintsOctal, (                                                              \
		string Buffer = LString(1);                                                                         \
		fstring_format Format = { .Precision = -1, .Value = { .Unsigned = 0 } };                            \
		Format.Type = FSTRING_FORMAT_FLAG_INT_OCT;                                                          \
		fstring_format_status Status = FString_WriteUnsigned(&Format, Buffer);                              \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("0")) == 0);                                                     \
		Format = (fstring_format){ .Precision = -1, .Value = { .Unsigned = U64_MAX } };                     \
		Format.Type = FSTRING_FORMAT_FLAG_INT_OCT;                                                          \
		Buffer = LString(22);                                                                               \
		Status = FString_WriteUnsigned(&Format, Buffer);                                                    \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("1777777777777777777777")) == 0);                                \
	))                                                                                                      \
	TEST(FString_WriteUnsigned, PrintsHexadecimal, (                                                        \
		string Buffer = LString(1);                                                                         \
		fstring_format Format = { .Precision = -1, .Value = { .Unsigned = 0 } };                            \
		Format.Type = FSTRING_FORMAT_FLAG_INT_HEX;                                                          \
		fstring_format_status Status = FString_WriteUnsigned(&Format, Buffer);                              \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("0")) == 0);                                                     \
		Format = (fstring_format){ .Precision = -1, .Value = { .Unsigned = U64_MAX } };                     \
		Format.Type = FSTRING_FORMAT_FLAG_INT_HEX;                                                          \
		Buffer = LString(16);                                                                               \
		Status = FString_WriteUnsigned(&Format, Buffer);                                                    \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("ffffffffffffffff")) == 0);                                      \
	))                                                                                                      \
	TEST(FString_WriteUnsigned, PrintsBinary, (                                                             \
		string Buffer = LString(1);                                                                         \
		fstring_format Format = { .Precision = -1, .Value = { .Unsigned = 0 } };                            \
		Format.Type = FSTRING_FORMAT_FLAG_INT_BIN;                                                          \
		fstring_format_status Status = FString_WriteUnsigned(&Format, Buffer);                              \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("0")) == 0);                                                     \
		Format = (fstring_format){ .Precision = -1, .Value = { .Unsigned = U64_MAX } };                     \
		Format.Type = FSTRING_FORMAT_FLAG_INT_BIN;                                                          \
		Buffer = LString(64);                                                                               \
		Status = FString_WriteUnsigned(&Format, Buffer);                                                    \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		string TestStr = CStringL("1111111111111111111111111111111111111111111111111111111111111111");      \
		Assert(String_Cmp(Buffer, TestStr) == 0);                                                           \
	))                                                                                                      \
	TEST(FString_WriteUnsigned, PrintsDecimalGroups, (                                                      \
		string Buffer = LString(20);                                                                        \
		fstring_format Format = { .Value = { .Unsigned = 0x1DEADBEEF } };                                   \
		Format.Type = FSTRING_FORMAT_FLAG_INT_DEC | FSTRING_FORMAT_FLAG_SEPARATE_GROUPS;                    \
		fstring_format_status Status = FString_WriteUnsigned(&Format, Buffer);                              \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("8,030,895,855")) == 0);                                         \
	))                                                                                                      \
	TEST(FString_WriteUnsigned, PrintsOctalGroups, (                                                        \
		string Buffer = LString(20);                                                                        \
		fstring_format Format = { .Value = { .Unsigned = 0x1DEADBEEF } };                                   \
		Format.Type = FSTRING_FORMAT_FLAG_INT_OCT | FSTRING_FORMAT_FLAG_SEPARATE_GROUPS;                    \
		fstring_format_status Status = FString_WriteUnsigned(&Format, Buffer);                              \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("73_653_337_357")) == 0);                                        \
	))                                                                                                      \
	TEST(FString_WriteUnsigned, PrintsHexadecimalGroups, (                                                  \
		string Buffer = LString(20);                                                                        \
		fstring_format Format = { .Value = { .Unsigned = 0x1DEADBEEF } };                                   \
		Format.Type = FSTRING_FORMAT_FLAG_INT_HEX | FSTRING_FORMAT_FLAG_SEPARATE_GROUPS;                    \
		fstring_format_status Status = FString_WriteUnsigned(&Format, Buffer);                              \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("1_dead_beef")) == 0);                                           \
	))                                                                                                      \
	TEST(FString_WriteUnsigned, PrintsBinaryGroups, (                                                       \
		string Buffer = LString(40);                                                                        \
		fstring_format Format = { .Value = { .Unsigned = 0x1DEADBEEF } };                                   \
		Format.Type = FSTRING_FORMAT_FLAG_INT_BIN | FSTRING_FORMAT_FLAG_SEPARATE_GROUPS;                    \
		fstring_format_status Status = FString_WriteUnsigned(&Format, Buffer);                              \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("1_11011110_10101101_10111110_11101111")) == 0);                 \
	))                                                                                                      \
	TEST(FString_WriteUnsigned, PrintsOctalPrefix, (                                                        \
		string Buffer = LString(4);                                                                         \
		fstring_format Format = { .Precision = -1, .Value = { .Unsigned = 255 } };                          \
		Format.Type = FSTRING_FORMAT_FLAG_INT_OCT;                                                          \
		Format.Type |= FSTRING_FORMAT_FLAG_SPECIFY_RADIX;                                                   \
		fstring_format_status Status = FString_WriteUnsigned(&Format, Buffer);                              \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("0377")) == 0);                                                  \
	))                                                                                                      \
	TEST(FString_WriteUnsigned, PrintsBinaryPrefix, (                                                       \
		string Buffer = LString(8);                                                                         \
		fstring_format Format = { .Precision = -1, .Value = { .Unsigned = 42 } };                           \
		Format.Type = FSTRING_FORMAT_FLAG_INT_BIN;                                                          \
		Format.Type |= FSTRING_FORMAT_FLAG_SPECIFY_RADIX;                                                   \
		fstring_format_status Status = FString_WriteUnsigned(&Format, Buffer);                              \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("0b101010")) == 0);                                              \
		Format.ActualWidth = 0;                                                                             \
		Format.Type |= FSTRING_FORMAT_FLAG_UPPERCASE;                                                       \
		Status = FString_WriteUnsigned(&Format, Buffer);                                                    \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("0B101010")) == 0);                                              \
	))                                                                                                      \
	TEST(FString_WriteUnsigned, PrintsHexadecimalPrefix, (                                                  \
		string Buffer = LString(8);                                                                         \
		fstring_format Format = { .Precision = -1, .Value = { .Unsigned = 42 } };                           \
		Format.Type = FSTRING_FORMAT_FLAG_INT_HEX;                                                          \
		Format.Type |= FSTRING_FORMAT_FLAG_SPECIFY_RADIX;                                                   \
		fstring_format_status Status = FString_WriteUnsigned(&Format, Buffer);                              \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("0x2a")) == 0);                                                  \
		Format.ActualWidth = 0;                                                                             \
		Format.Type |= FSTRING_FORMAT_FLAG_UPPERCASE;                                                       \
		Status = FString_WriteUnsigned(&Format, Buffer);                                                    \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("0X2A")) == 0);                                                  \
	))                                                                                                      \
	TEST(FString_WriteUnsigned, HandlesAlignmentAndPadding, (                                               \
		string Buffer = LString(13);                                                                        \
		fstring_format Format = { .Width = 12, .Precision = -1, .Value = { .Unsigned = 0x2cdeba } };        \
		Format.Type = FSTRING_FORMAT_FLAG_INT_HEX;                                                          \
		Format.Type |= FSTRING_FORMAT_FLAG_SPECIFY_RADIX;                                                   \
		Format.Type |= FSTRING_FORMAT_FLAG_SEPARATE_GROUPS;                                                 \
		Format.Type |= FSTRING_FORMAT_FLAG_LEFT_JUSTIFY;                                                    \
		Format.Type |= FSTRING_FORMAT_FLAG_PAD_WITH_ZERO;                                                   \
		fstring_format_status Status = FString_WriteUnsigned(&Format, Buffer);                              \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("0x2c_deba   ")) == 0);                                          \
		Format.ActualWidth = 0;                                                                             \
		Format.Type &= ~FSTRING_FORMAT_FLAG_LEFT_JUSTIFY;                                                   \
		Buffer.Length = 13;                                                                                 \
		Status = FString_WriteUnsigned(&Format, Buffer);                                                    \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("0x0_002c_deba")) == 0);                                         \
	))                                                                                                      \
	TEST(FString_WriteUnsigned, HandlesPrecision, (                                                         \
		string Buffer = LString(12);                                                                        \
		fstring_format Format = { .Width = 12, .Precision = 8, .Value = { .Unsigned = 192832 } };           \
		Format.Type = FSTRING_FORMAT_FLAG_INT_DEC;                                                          \
		Format.Type |= FSTRING_FORMAT_FLAG_SEPARATE_GROUPS;                                                 \
		fstring_format_status Status = FString_WriteUnsigned(&Format, Buffer);                              \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("  00,192,832")) == 0);                                          \
		Format.Value.Unsigned = 0;                                                                          \
		Format.Precision = 0;                                                                               \
		Format.ActualWidth = 0;                                                                             \
		Format.Type &= ~FSTRING_FORMAT_FLAG_LEFT_JUSTIFY;                                                   \
		Status = FString_WriteUnsigned(&Format, Buffer);                                                    \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("            ")) == 0);                                          \
	))                                                                                                      \
	TEST(FString_WriteUnsigned, PrintsMax, (                                                                \
		string Buffer = LString(30);                                                                        \
		fstring_format Format = { .Value = { .Unsigned = U64_MAX } };                                       \
		fstring_format_status Status = FString_WriteUnsigned(&Format, Buffer);                              \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("18446744073709551615")) == 0);                                  \
	))                                                                                                      \
	TEST(FString_WritePointer, OnlyAcceptsLeftJustify, (                                                    \
		string Buffer = LString(10);                                                                        \
		fstring_format Format = { .Width = 10, .Precision = 8, .Value = { .Pointer = (vptr) 0xABCD } };     \
		Format.Type = FSTRING_FORMAT_FLAG_INT_BIN;                                                          \
		Format.Type |= FSTRING_FORMAT_FLAG_SPECIFY_RADIX;                                                   \
		Format.Type |= FSTRING_FORMAT_FLAG_SEPARATE_GROUPS;                                                 \
		Format.Type |= FSTRING_FORMAT_FLAG_UPPERCASE;                                                       \
		Format.Type |= FSTRING_FORMAT_FLAG_PAD_WITH_ZERO;                                                   \
		fstring_format_status Status = FString_WritePointer(&Format, Buffer);                               \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("    0xabcd")) == 0);                                            \
		Format.Precision = 0;                                                                               \
		Format.ActualWidth = 0;                                                                             \
		Format.Type |= FSTRING_FORMAT_FLAG_LEFT_JUSTIFY;                                                    \
		Status = FString_WritePointer(&Format, Buffer);                                                     \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("0xabcd    ")) == 0);                                            \
	))                                                                                                      \
	TEST(FString_WritePointer, PrintsNilForNull, (                                                          \
		string Buffer = LString(10);                                                                        \
		fstring_format Format = { .Value = { .Pointer = NULL } };                                           \
		fstring_format_status Status = FString_WritePointer(&Format, Buffer);                               \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("(nil)")) == 0);                                                 \
	))                                                                                                      \
	TEST(FString_WriteSigned, HandlesAlignmentAndPadding, (                                                 \
		string Buffer = LString(10);                                                                        \
		fstring_format Format = { .Width = 9, .Precision = -1, .Value = { .Signed = -92831 } };             \
		Format.Type = FSTRING_FORMAT_FLAG_PREFIX_SIGN;                                                      \
		Format.Type |= FSTRING_FORMAT_FLAG_SEPARATE_GROUPS;                                                 \
		Format.Type |= FSTRING_FORMAT_FLAG_LEFT_JUSTIFY;                                                    \
		Format.Type |= FSTRING_FORMAT_FLAG_PAD_WITH_ZERO;                                                   \
		fstring_format_status Status = FString_WriteSigned(&Format, Buffer);                                \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("-92,831  ")) == 0);                                             \
		Format.ActualWidth = 0;                                                                             \
		Format.Type &= ~FSTRING_FORMAT_FLAG_LEFT_JUSTIFY;                                                   \
		Buffer.Length = 12;                                                                                 \
		Status = FString_WriteSigned(&Format, Buffer);                                                      \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("-0,092,831")) == 0);                                            \
	))                                                                                                      \
	TEST(FString_WriteSigned, HandlesPrecision, (                                                           \
		string Buffer = LString(12);                                                                        \
		fstring_format Format = { .Width = 12, .Precision = 8, .Value = { .Signed = 192832 } };             \
		Format.Type = FSTRING_FORMAT_FLAG_SEPARATE_GROUPS;                                                  \
		fstring_format_status Status = FString_WriteSigned(&Format, Buffer);                                \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("  00,192,832")) == 0);                                          \
		Format.Value.Signed = 0;                                                                            \
		Format.Precision = 0;                                                                               \
		Format.ActualWidth = 0;                                                                             \
		Format.Type &= ~FSTRING_FORMAT_FLAG_LEFT_JUSTIFY;                                                   \
		Status = FString_WriteSigned(&Format, Buffer);                                                      \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("            ")) == 0);                                          \
	))                                                                                                      \
	TEST(FString_WriteSigned, HandlesPrefixSpace, (                                                         \
		string Buffer = LString(12);                                                                        \
		fstring_format Format = { .Value = { .Signed = 192832 } };                                          \
		Format.Type = FSTRING_FORMAT_FLAG_PREFIX_SPACE;                                                     \
		fstring_format_status Status = FString_WriteSigned(&Format, Buffer);                                \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL(" 192832")) == 0);                                               \
		Format.ActualWidth = 0;                                                                             \
		Format.Type |= FSTRING_FORMAT_FLAG_PREFIX_SIGN;                                                     \
		Buffer.Length = 12;                                                                                 \
		Status = FString_WriteSigned(&Format, Buffer);                                                      \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("+192832")) == 0);                                               \
	))                                                                                                      \
	TEST(FString_WriteSigned, PrintsMax, (                                                                  \
		string Buffer = LString(30);                                                                        \
		fstring_format Format = { .Value = { .Signed = S64_MAX } };                                         \
		fstring_format_status Status = FString_WriteSigned(&Format, Buffer);                                \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("9223372036854775807")) == 0);                                   \
	))                                                                                                      \
	TEST(FString_WriteSigned, PrintsMin, (                                                                  \
		string Buffer = LString(30);                                                                        \
		fstring_format Format = { .Value = { .Signed = S64_MIN } };                                         \
		fstring_format_status Status = FString_WriteSigned(&Format, Buffer);                                \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("-9223372036854775808")) == 0);                                  \
	))                                                                                                      \
	TEST(FString_WriteHexFloat, PrintsSignExpAndMantissa, (                                                 \
		string		   Buffer = LString(30);                                                                \
		fstring_format Format = { .Value = { .Float = R64_CREATE(1, 1023, 0x8F1D29138CD2Full) } };          \
		Format.Precision = -1;                                                                              \
		fstring_format_status Status = FString_WriteHexFloat(&Format, Buffer);                              \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("-0x1.8f1d29138cd2fp+1023")) == 0);                              \
	))                                                                                                      \
	TEST(FString_WriteHexFloat, HandlesUppercase, (                                                         \
		string		   Buffer = LString(30);                                                                \
		fstring_format Format = { .Value = { .Float = R64_CREATE(1, 0, 0x123456789abcdull) } };             \
		Format.Type = FSTRING_FORMAT_FLAG_UPPERCASE;                                                        \
		Format.Precision = -1;                                                                              \
		fstring_format_status Status = FString_WriteHexFloat(&Format, Buffer);                              \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("-0X1.123456789ABCDP+0")) == 0);                                 \
	))                                                                                                      \
	TEST(FString_WriteHexFloat, HandlesPrefixSpace, (                                                       \
		string		   Buffer = LString(30);                                                                \
		fstring_format Format = { .Value = { .Float = R64_CREATE(0, 0, 0x123456789abcdull) } };             \
		Format.Type = FSTRING_FORMAT_FLAG_PREFIX_SPACE;                                                     \
		Format.Precision = -1;                                                                              \
		fstring_format_status Status = FString_WriteHexFloat(&Format, Buffer);                              \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL(" 0x1.123456789abcdp+0")) == 0);                                 \
	))                                                                                                      \
	TEST(FString_WriteHexFloat, HandlesPrefixSign, (                                                        \
		string		   Buffer = LString(30);                                                                \
		fstring_format Format = { .Value = { .Float = R64_CREATE(0, 0, 0x123456789abcdull) } };             \
		Format.Type = FSTRING_FORMAT_FLAG_PREFIX_SIGN | FSTRING_FORMAT_FLAG_PREFIX_SPACE;                   \
		Format.Precision = -1;                                                                              \
		fstring_format_status Status = FString_WriteHexFloat(&Format, Buffer);                              \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("+0x1.123456789abcdp+0")) == 0);                                 \
	))                                                                                                      \
	TEST(FString_WriteHexFloat, HandlesDenormal, (                                                          \
		string		   Buffer = LString(30);                                                                \
		fstring_format Format = { .Value = { .Float = R64_CREATE(0, -1023, 0x8F1D29138CD00ull) } };         \
		Format.Precision = -1;                                                                              \
		fstring_format_status Status = FString_WriteHexFloat(&Format, Buffer);                              \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("0x0.8f1d29138cdp-1022")) == 0);                                 \
	))                                                                                                      \
	TEST(FString_WriteHexFloat, PrecisionExtends, (                                                         \
		string		   Buffer = LString(30);                                                                \
		fstring_format Format = { .Precision = 4, .Value = { .Float = R64_CREATE(0, 2, 0) } };              \
		fstring_format_status Status = FString_WriteHexFloat(&Format, Buffer);                              \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("0x1.0000p+2")) == 0);                                           \
	))                                                                                                      \
	TEST(FString_WriteHexFloat, PrecisionTruncates, (                                                       \
		string		   Buffer = LString(30);                                                                \
		fstring_format Format = { .Value = { .Float = R64_CREATE(0, 2, 0x123456789ABCD) } };                \
		Format.Precision = 4;                                                                               \
		fstring_format_status Status = FString_WriteHexFloat(&Format, Buffer);                              \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("0x1.1234p+2")) == 0);                                           \
	))                                                                                                      \
	TEST(FString_WriteHexFloat, PrecisionCanRoundUp, (                                                      \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Precision = 1, .Value = { .Float = 1.96875 } };                          \
		fstring_format_status Status = FString_WriteHexFloat(&Format, Buffer);                              \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("0x2.0p+0")) == 0);                                              \
	))                                                                                                      \
	TEST(FString_WriteHexFloat, PrecisionCanRoundDown, (                                                    \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Precision = 1, .Value = { .Float = 1.46875 } };                          \
		fstring_format_status Status = FString_WriteHexFloat(&Format, Buffer);                              \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("0x1.8p+0")) == 0);                                              \
	))                                                                                                      \
	TEST(FString_WriteHexFloat, NoDecimalIfPrecisionZero, (                                                 \
		string		   Buffer = LString(30);                                                                \
		fstring_format Format = { .Precision = 0, .Value = { .Float = R64_CREATE(0, 2, 1) } };              \
		fstring_format_status Status = FString_WriteHexFloat(&Format, Buffer);                              \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("0x1p+2")) == 0);                                                \
	))                                                                                                      \
	TEST(FString_WriteHexFloat, NoDecimalIfPrecisionDefaultAndMantissaEmpty, (                              \
		string		   Buffer = LString(30);                                                                \
		fstring_format Format = { .Precision = -1, .Value = { .Float = R64_CREATE(0, 2, 0) } };             \
		fstring_format_status Status = FString_WriteHexFloat(&Format, Buffer);                              \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("0x1p+2")) == 0);                                                \
	))                                                                                                      \
	TEST(FString_WriteHexFloat, HandlesSpecifyRadix, (                                                      \
		string		   Buffer = LString(30);                                                                \
		fstring_format Format = { .Precision = -1, .Value = { .Float = R64_CREATE(0, 2, 0) } };             \
		Format.Type = FSTRING_FORMAT_FLAG_SPECIFY_RADIX;                                                    \
		fstring_format_status Status = FString_WriteHexFloat(&Format, Buffer);                              \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("0x1.p+2")) == 0);                                               \
	))                                                                                                      \
	TEST(FString_WriteHexFloat, HandlesWidth, (                                                             \
		string		   Buffer = LString(30);                                                                \
		fstring_format Format = { .Value = { .Float = R64_CREATE(1, 1023, 0x123456789ABCDull) } };          \
		Format.Width = 26;                                                                                  \
		Format.Precision = -1;                                                                              \
		fstring_format_status Status = FString_WriteHexFloat(&Format, Buffer);                              \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("  -0x1.123456789abcdp+1023")) == 0);                            \
	))                                                                                                      \
	TEST(FString_WriteHexFloat, HandlesPadZero, (                                                           \
		string		   Buffer = LString(30);                                                                \
		fstring_format Format = { .Value = { .Float = R64_CREATE(0, 1023, 0x123456789ABCDull) } };          \
		Format.Type = FSTRING_FORMAT_FLAG_PAD_WITH_ZERO;                                                    \
		Format.Width = 26;                                                                                  \
		Format.Precision = -1;                                                                              \
		fstring_format_status Status = FString_WriteHexFloat(&Format, Buffer);                              \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("0x0001.123456789abcdp+1023")) == 0);                            \
	))                                                                                                      \
	TEST(FString_WriteHexFloat, HandlesLeftJustify, (                                                       \
		string		   Buffer = LString(30);                                                                \
		fstring_format Format = { .Value = { .Float = R64_CREATE(0, 1023, 0x123456789ABCDull) } };          \
		Format.Type = FSTRING_FORMAT_FLAG_LEFT_JUSTIFY | FSTRING_FORMAT_FLAG_PAD_WITH_ZERO;                 \
		Format.Width = 26;                                                                                  \
		Format.Precision = -1;                                                                              \
		fstring_format_status Status = FString_WriteHexFloat(&Format, Buffer);                              \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("0x1.123456789abcdp+1023   ")) == 0);                            \
	))                                                                                                      \
	TEST(FString_WriteHexFloat, HandlesSpecifyGroups, (                                                     \
		string		   Buffer = LString(30);                                                                \
		fstring_format Format = { .Precision = -1, .Width = 12, .Value = { .Float = 1.5  } };               \
		Format.Type = FSTRING_FORMAT_FLAG_SEPARATE_GROUPS | FSTRING_FORMAT_FLAG_PAD_WITH_ZERO;              \
		fstring_format_status Status = FString_WriteHexFloat(&Format, Buffer);                              \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("0x0_0001.8p+0")) == 0);                                         \
	))                                                                                                      \
	TEST(FString_WriteFloat, HandlesInfinity, (                                                             \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Width = 10, .Value = { .Float = R64_INF } };                             \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("       inf")) == 0);                                            \
		Format = (fstring_format){ .Width = 10, .Value = { .Float = R64_NINF } };                           \
		Status = FString_WriteFloat(&Format, Buffer);                                                       \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("      -inf")) == 0);                                            \
		Format = (fstring_format){ .Width = 10, .Value = { .Float = R64_INF } };                            \
		Format.Type = FSTRING_FORMAT_FLAG_PREFIX_SIGN | FSTRING_FORMAT_FLAG_PREFIX_SPACE;                   \
		Status = FString_WriteFloat(&Format, Buffer);                                                       \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("      +inf")) == 0);                                            \
		Format = (fstring_format){ .Width = 10, .Value = { .Float = R64_INF } };                            \
		Format.Type = FSTRING_FORMAT_FLAG_PREFIX_SPACE | FSTRING_FORMAT_FLAG_UPPERCASE                      \
			| FSTRING_FORMAT_FLAG_LEFT_JUSTIFY;                                                             \
		Status = FString_WriteFloat(&Format, Buffer);                                                       \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL(" INF      ")) == 0);                                            \
	))                                                                                                      \
	TEST(FString_WriteFloat, SpecialHandlesNan, (                                                           \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Width = 6, .Value = { .Float = R64_NAN(1) } };                           \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("   nan")) == 0);                                                \
	))                                                                                                      \
	TEST(FString_WriteFloat, SpecialHandlesInf, (                                                           \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Width = 6, .Value = { .Float = R64_NAN(0) } };                           \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("   inf")) == 0);                                                \
	))                                                                                                      \
	TEST(FString_WriteFloat, SpecialHandlesUppercaseNan, (                                                  \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Width = 6, .Value = { .Float = R64_NAN(1) } };                           \
		Format.Type = FSTRING_FORMAT_FLAG_UPPERCASE;                                                        \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("   NAN")) == 0);                                                \
	))                                                                                                      \
	TEST(FString_WriteFloat, SpecialHandlesUppercaseInf, (                                                  \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Width = 6, .Value = { .Float = R64_NAN(0) } };                           \
		Format.Type = FSTRING_FORMAT_FLAG_UPPERCASE;                                                        \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("   INF")) == 0);                                                \
	))                                                                                                      \
	TEST(FString_WriteFloat, SpecialHandlesNegative, (                                                      \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Width = 6, .Value = { .Float = R64_NNAN(1) } };                          \
		Format.Type = FSTRING_FORMAT_FLAG_PREFIX_SIGN | FSTRING_FORMAT_FLAG_PREFIX_SPACE;                   \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("  -nan")) == 0);                                                \
	))                                                                                                      \
	TEST(FString_WriteFloat, SpecialHandlesPrefixSign, (                                                    \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Width = 6, .Value = { .Float = R64_NAN(0) } };                           \
		Format.Type = FSTRING_FORMAT_FLAG_PREFIX_SIGN | FSTRING_FORMAT_FLAG_PREFIX_SPACE;                   \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("  +inf")) == 0);                                                \
	))                                                                                                      \
	TEST(FString_WriteFloat, SpecialHandlesPrefixSpace, (                                                   \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Value = { .Float = R64_NAN(1) } };                                       \
		Format.Type = FSTRING_FORMAT_FLAG_PREFIX_SPACE;                                                     \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL(" nan")) == 0);                                                  \
	))                                                                                                      \
	TEST(FString_WriteFloat, SpecialHandlesLeftJustify, (                                                   \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Width = 6, .Value = { .Float = R64_NAN(0) } };                           \
		Format.Type = FSTRING_FORMAT_FLAG_PREFIX_SPACE | FSTRING_FORMAT_FLAG_LEFT_JUSTIFY;                  \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL(" inf  ")) == 0);                                                \
	))                                                                                                      \
	TEST(FString_WriteFloat, CallsPrintHexFloat, (                                                          \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Precision = -1, .Value = { .Float = 2.5 } };                             \
		Format.Type = FSTRING_FORMAT_FLAG_FLOAT_HEX;                                                        \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("0x1.4p+1")) == 0);                                              \
	))                                                                                                      \
	TEST(FString_WriteFloat, StdHandlesZeroAndDefaultPrecision, (                                           \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Width = 10, .Precision = -1, .Value = { .Float = 0 } };                  \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CString("  0.000000")) == 0);                                             \
	))                                                                                                      \
	TEST(FString_WriteFloat, StdHandlesNoDecimalPointIfPrecisionZero, (                                     \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Width = 4, .Precision = 0, .Value = { .Float = 0 } };                    \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CString("   0")) == 0);                                                   \
	))                                                                                                      \
	TEST(FString_WriteFloat, StdHandlesWholeNumbers, (                                                      \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Precision = 2, .Value = { .Float = 123 } };                              \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CString("123.00")) == 0);                                                 \
	))                                                                                                      \
	TEST(FString_WriteFloat, StdHandlesNumbersWithoutWholePart, (                                           \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Precision = 4, .Value = { .Float = 0.625 } };                            \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CString("0.6250")) == 0);                                                 \
	))                                                                                                      \
	TEST(FString_WriteFloat, StdHandlesNumbersWithBothParts, (                                              \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Precision = 4, .Value = { .Float = 123.625 } };                          \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CString("123.6250")) == 0);                                               \
	))                                                                                                      \
	TEST(FString_WriteFloat, StdHandlesRoundingFractionalPart, (                                            \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Precision = 2, .Value = { .Float = 123.6875 } };                         \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CString("123.69")) == 0);                                                 \
	))                                                                                                      \
	TEST(FString_WriteFloat, StdHandlesRoundingAcrossDecimalPoint, (                                        \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Precision = 2, .Value = { .Float = 123.999 } };                          \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CString("124.00")) == 0);                                                 \
	))                                                                                                      \
	TEST(FString_WriteFloat, StdHandlesRoundedExtraDigit, (                                                 \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Precision = 2, .Value = { .Float = 999.999 } };                          \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CString("1000.00")) == 0);                                                \
	))                                                                                                      \
	TEST(FString_WriteFloat, StdHandlesLargeExponent, (                                                     \
		string		   Buffer = LString(320);                                                               \
		r64 Float = R64_CREATE(1, R64_EXPONENT_MAX-1, R64_MANTISSA_MASK);                                   \
		fstring_format Format = { .Width = 319, .Precision = -1, .Value = { .Float = Float } };             \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		c08 *Expected = "  -179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.000000"; \
		Assert(String_Cmp(Buffer, CString(Expected)) == 0);                                                 \
	))                                                                                                      \
	TEST(FString_WriteFloat, StdHandlesSmallExponent, (                                                     \
		string		   Buffer = LString(1080);                                                              \
		r64 Float = R64_CREATE(0, R64_EXPONENT_MIN, 1);                                                     \
		fstring_format Format = { .Width = 1080, .Precision = 1076, .Value = { .Float = Float } };          \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		c08 *Expected = "  0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000494065645841246544176568792868221372365059802614324764425585682500675507270208751865299836361635992379796564695445717730926656710355939796398774796010781878126300713190311404527845817167848982103688718636056998730723050006387409153564984387312473397273169615140031715385398074126238565591171026658556686768187039560310624931945271591492455329305456544401127480129709999541931989409080416563324524757147869014726780159355238611550134803526493472019379026810710749170333222684475333572083243193609238289345836806010601150616980975307834227731832924790498252473077637592724787465608477820373446969953364701797267771758512566055119913150489110145103786273816725095583738973359899366480994116420570263709027924276754456522908753868250641971826553344726562500"; \
		Assert(String_Cmp(Buffer, CString(Expected)) == 0);                                                 \
	))                                                                                                      \
	TEST(FString_WriteFloat, StdHandlesSpecifyRadix, (                                                      \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Precision = 0, .Value = { .Float = 99.5 } };                             \
		Format.Type = FSTRING_FORMAT_FLAG_SPECIFY_RADIX;                                                    \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CString("100.")) == 0);                                                   \
	))                                                                                                      \
	TEST(FString_WriteFloat, StdHandlesPrefixSign, (                                                        \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Precision = 1, .Value = { .Float = 123.5 } };                            \
		Format.Type = FSTRING_FORMAT_FLAG_PREFIX_SIGN | FSTRING_FORMAT_FLAG_PREFIX_SPACE;                   \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CString("+123.5")) == 0);                                                 \
	))                                                                                                      \
	TEST(FString_WriteFloat, StdHandlesPrefixSpace, (                                                       \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Precision = 1, .Value = { .Float = 123.5 } };                            \
		Format.Type = FSTRING_FORMAT_FLAG_PREFIX_SPACE;                                                     \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CString(" 123.5")) == 0);                                                 \
	))                                                                                                      \
	TEST(FString_WriteFloat, StdHandlesGroups, (                                                            \
		string		   Buffer = LString(12);                                                                \
		fstring_format Format = { .Precision = 1, .Value = { .Float = 1234567 } };                          \
		Format.Type = FSTRING_FORMAT_FLAG_SEPARATE_GROUPS;                                                  \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CString("1,234,567.0")) == 0);                                            \
	))                                                                                                      \
	TEST(FString_WriteFloat, StdHandlesPadZero, (                                                           \
		string		   Buffer = LString(12);                                                                \
		fstring_format Format = { .Width = 11, .Precision = 1, .Value = { .Float = -123.5 } };              \
		Format.Type = FSTRING_FORMAT_FLAG_SEPARATE_GROUPS | FSTRING_FORMAT_FLAG_PAD_WITH_ZERO;              \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CString("-0,000,123.5")) == 0);                                           \
	))                                                                                                      \
	TEST(FString_WriteFloat, StdHandlesLeftJustify, (                                                       \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Width = 10, .Precision = 1, .Value = { .Float = 123 } };                 \
		Format.Type = FSTRING_FORMAT_FLAG_LEFT_JUSTIFY | FSTRING_FORMAT_FLAG_PAD_WITH_ZERO;                 \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CString("123.0     ")) == 0);                                             \
	))                                                                                                      \
	TEST(FString_WriteFloat, ReportsBufferTooSmall, (                                                       \
		string		   Buffer = LString(9);                                                                 \
		fstring_format Format = { .Width = 10, .Precision = 1, .Value = { .Float = 123 } };                 \
		Format.Type = FSTRING_FORMAT_FLAG_LEFT_JUSTIFY | FSTRING_FORMAT_FLAG_PAD_WITH_ZERO;                 \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_BUFFER_TOO_SMALL);                                                  \
	))                                                                                                      \
	TEST(FString_WriteFloat, ExpHandlesZero, (                                                              \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Precision = 1, .Value = { .Float = 0 } };                                \
		Format.Type = FSTRING_FORMAT_FLAG_FLOAT_EXP;                                                        \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CString("0.0e+00")) == 0);                                                \
	))                                                                                                      \
	TEST(FString_WriteFloat, ExpHandlesDefaultPrecision, (                                                  \
		string		   Buffer = LString(12);                                                                \
		fstring_format Format = { .Precision = -1, .Value = { .Float = 0.000732421875 } };                  \
		Format.Type = FSTRING_FORMAT_FLAG_FLOAT_EXP;                                                        \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CString("7.324219e-04")) == 0);                                           \
	))                                                                                                      \
	TEST(FString_WriteFloat, ExpHandlesPrecisionZero, (                                                     \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Precision = 0, .Value = { .Float = 100 } };                              \
		Format.Type = FSTRING_FORMAT_FLAG_FLOAT_EXP;                                                        \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CString("1e+02")) == 0);                                                  \
	))                                                                                                      \
	TEST(FString_WriteFloat, ExpHandlesUppercase, (                                                         \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Precision = 2, .Value = { .Float = 123.5 } };                            \
		Format.Type = FSTRING_FORMAT_FLAG_FLOAT_EXP | FSTRING_FORMAT_FLAG_UPPERCASE;                        \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CString("1.24E+02")) == 0);                                               \
	))                                                                                                      \
	TEST(FString_WriteFloat, ExpHandlesRoundedExtraDigit, (                                                 \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Precision = 4, .Value = { .Float = 999.999 } };                          \
		Format.Type = FSTRING_FORMAT_FLAG_FLOAT_EXP;                                                        \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CString("1.0000e+03")) == 0);                                             \
	))                                                                                                      \
	TEST(FString_WriteFloat, ExpHandlesLargeExponent, (                                                     \
		string		   Buffer = LString(330);                                                               \
		fstring_format Format = { .Precision = 308, .Value = { .Float = -1.7976931348623157e308 } };        \
		Format.Width = 319;                                                                                 \
		Format.Type = FSTRING_FORMAT_FLAG_FLOAT_EXP;                                                        \
		Format.Type |= FSTRING_FORMAT_FLAG_SEPARATE_GROUPS | FSTRING_FORMAT_FLAG_PAD_WITH_ZERO;             \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		c08 *Expected = "-0,001.79769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368e+308"; \
		Assert(String_Cmp(Buffer, CString(Expected)) == 0);                                                 \
	))                                                                                                      \
	TEST(FString_WriteFloat, ExpHandlesSmallExponent, (                                                     \
		string		   Buffer = LString(770);                                                               \
		fstring_format Format = { .Value = { .Float = R64_CREATE(1, R64_EXPONENT_MIN, 1) } };               \
		Format.Precision = 752;                                                                             \
		Format.Width = 763;                                                                                 \
		Format.Type = FSTRING_FORMAT_FLAG_FLOAT_EXP;                                                        \
		Format.Type |= FSTRING_FORMAT_FLAG_SEPARATE_GROUPS | FSTRING_FORMAT_FLAG_PAD_WITH_ZERO;             \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		c08 *Expected = "-0,004.94065645841246544176568792868221372365059802614324764425585682500675507270208751865299836361635992379796564695445717730926656710355939796398774796010781878126300713190311404527845817167848982103688718636056998730723050006387409153564984387312473397273169615140031715385398074126238565591171026658556686768187039560310624931945271591492455329305456544401127480129709999541931989409080416563324524757147869014726780159355238611550134803526493472019379026810710749170333222684475333572083243193609238289345836806010601150616980975307834227731832924790498252473077637592724787465608477820373446969953364701797267771758512566055119913150489110145103786273816725095583738973359899366480994116420570263709027924276754456522908753868250641971826553344726562500e-324"; \
		Assert(String_Cmp(Buffer, CString(Expected)) == 0);                                                 \
	))                                                                                                      \
	TEST(FString_WriteFloat, FitSelectsExpPastPrecisionAndRounds, (                                         \
		string		   Buffer = LString(20);                                                                \
		fstring_format Format = { .Precision = 6, .Value = { .Float = 1234567.5 } };                        \
		Format.Type = FSTRING_FORMAT_FLAG_FLOAT_FIT;                                                        \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CString("1.23457e+06")) == 0);                                            \
	))                                                                                                      \
	TEST(FString_WriteFloat, FitHandlesZero, (                                                              \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Precision = 1, .Value = { .Float = 0 } };                                \
		Format.Type = FSTRING_FORMAT_FLAG_FLOAT_FIT;                                                        \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CString("0")) == 0);                                                      \
	))                                                                                                      \
	TEST(FString_WriteFloat, FitDefaultsPrecisionToOne, (                                                   \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Precision = -1, .Value = { .Float = 10 } };                              \
		Format.Type = FSTRING_FORMAT_FLAG_FLOAT_FIT;                                                        \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CString("1e+01")) == 0);                                                  \
	))                                                                                                      \
	TEST(FString_WriteFloat, FitTreatsZeroPrecisionAsOne, (                                                 \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Precision = 0, .Value = { .Float = 10 } };                               \
		Format.Type = FSTRING_FORMAT_FLAG_FLOAT_FIT;                                                        \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CString("1e+01")) == 0);                                                  \
	))                                                                                                      \
	TEST(FString_WriteFloat, FitSelectsExpPastPrecisionAndTrimsTrailingZeroes, (                            \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Precision = 6, .Value = { .Float = 1000000 } };                          \
		Format.Type = FSTRING_FORMAT_FLAG_FLOAT_FIT;                                                        \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CString("1e+06")) == 0);                                                  \
	))                                                                                                      \
	TEST(FString_WriteFloat, FitSelectsStdUnderPrecision, (                                                 \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Precision = 6, .Value = { .Float = 123450 } };                           \
		Format.Type = FSTRING_FORMAT_FLAG_FLOAT_FIT;                                                        \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CString("123450")) == 0);                                                 \
	))                                                                                                      \
	TEST(FString_WriteFloat, FitSelectsStdUnderPrecisionAndTrimsTrailingZeroes, (                           \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Precision = 6, .Value = { .Float = 123.5 } };                            \
		Format.Type = FSTRING_FORMAT_FLAG_FLOAT_FIT;                                                        \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CString("123.5")) == 0);                                                  \
	))                                                                                                      \
	TEST(FString_WriteFloat, FitSelectsExpUnderENegativeFourAndTrimsTrailingZeroes, (                       \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Value = { .Float = 0.00009 } };                                          \
		Format.Type = FSTRING_FORMAT_FLAG_FLOAT_FIT;                                                        \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CString("9e-05")) == 0);                                                  \
	))                                                                                                      \
	TEST(FString_WriteFloat, FitSelectsFitOverENegativeFourAndTrimsTrailingZeroes, (                        \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Value = { .Float = 0.0001 } };                                           \
		Format.Type = FSTRING_FORMAT_FLAG_FLOAT_FIT;                                                        \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CString("0.0001")) == 0);                                                 \
	))                                                                                                      \
	TEST(FString_WriteFloat, FitSelectsStdUnderENegativeFourIfItRoundsUp, (                                 \
		string		   Buffer = LString(10);                                                                \
		fstring_format Format = { .Value = { .Float = 0.000099999 } };                                      \
		Format.Type = FSTRING_FORMAT_FLAG_FLOAT_FIT;                                                        \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CString("0.0001")) == 0);                                                 \
	))                                                                                                      \
	TEST(FString_WriteFloat, FitHandlesSpecifyRadixForStd, (                                                \
		string		   Buffer = LString(20);                                                                \
		fstring_format Format = { .Precision = 6, .Value = { .Float = 123.5 } };                            \
		Format.Type = FSTRING_FORMAT_FLAG_FLOAT_FIT | FSTRING_FORMAT_FLAG_SPECIFY_RADIX;                    \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CString("123.500")) == 0);                                                \
	))                                                                                                      \
	TEST(FString_WriteFloat, FitHandlesSpecifyRadixForExp, (                                                \
		string		   Buffer = LString(20);                                                                \
		fstring_format Format = { .Precision = 3, .Value = { .Float = 10000 } };                            \
		Format.Type = FSTRING_FORMAT_FLAG_FLOAT_FIT | FSTRING_FORMAT_FLAG_SPECIFY_RADIX;                    \
		fstring_format_status Status = FString_WriteFloat(&Format, Buffer);                                 \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CString("1.00e+04")) == 0);                                               \
	))                                                                                                      \
	TEST(FString_WriteFormat, DelegatesByType, (                                                            \
		string Buffer = LString(10);                                                                        \
		fstring_format Format = { .Value = { .Unsigned = 0xABCD } };                                        \
		Format.Type = FSTRING_FORMAT_TYPE_U16;                                                              \
		fstring_format_status Status = FString_WriteFormat(&Format, Buffer);                                \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("43981")) == 0);                                                 \
		Buffer.Length = 10;                                                                                 \
		Format.ActualWidth = 0;                                                                             \
		Format.Type = FSTRING_FORMAT_TYPE_PTR;                                                              \
		Status = FString_WriteFormat(&Format, Buffer);                                                      \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("0xabcd")) == 0);                                                \
		Buffer.Length = 10;                                                                                 \
		Format.ActualWidth = 0;                                                                             \
		Format.Type = FSTRING_FORMAT_TYPE_CHR;                                                              \
		Buffer.Encoding = STRING_ENCODING_UTF8;                                                             \
		Status = FString_WriteFormat(&Format, Buffer);                                                      \
		Buffer.Length = Format.ActualWidth;                                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(String_Cmp(Buffer, CStringL("\xc3\x8d")) == 0);                                              \
	))                                                                                                      \
	TEST(FString_WriteFormat, HandlesNoWrite, (                                                             \
		string Buffer = EString();                                                                          \
		fstring_format Format = { .Value = { .Unsigned = 0xABCD } };                                        \
		Format.Type = FSTRING_FORMAT_TYPE_U16 | FSTRING_FORMAT_FLAG_NO_WRITE;                               \
		fstring_format_status Status = FString_WriteFormat(&Format, Buffer);                                \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
		Assert(Format.ActualWidth == 0);                                                                    \
	))                                                                                                      \
	TEST(FString_WriteFormat, PropagatesError, (                                                            \
		string Buffer = LString(0);                                                                         \
		fstring_format Format = { .Value = { .Unsigned = 0xABCD } };                                        \
		Format.Type = FSTRING_FORMAT_TYPE_U16;                                                              \
		fstring_format_status Status = FString_WriteFormat(&Format, Buffer);                                \
		Assert(Status == FSTRING_FORMAT_BUFFER_TOO_SMALL);                                                  \
	))                                                                                                      \
	TEST(FString_WriteFormat, ReportsInvalidType, (                                                         \
		string Buffer = LString(60);                                                                        \
		fstring_format Format = { 0 };                                                                      \
		fstring_format_status Status = FString_WriteFormat(&Format, Buffer);                                \
		Assert(Status == FSTRING_FORMAT_TYPE_INVALID);                                                      \
	))                                                                                                      \
	TEST(FString_WriteFormats, WritesMultipleFormats, (                                                     \
		s32 Num = -1;                                                                                       \
		string Str = CStringL("Jimmy");                                                                     \
		string Format = CStringL("Hello, %s! Your lucky number today%n is %d%%!");                          \
		fstring_format Formats[3] = {                                                                       \
			{ .Value = { .String = &Str }, .SpecifierString = { .Text = Format.Text + 7, .Length = 2 } },   \
			{ .Value = { .Pointer = &Num }, .SpecifierString = { .Text = Format.Text + 34, .Length = 2 } }, \
			{ .Value = { .Signed = -192 }, .SpecifierString = { .Text = Format.Text + 40, .Length = 2 } }   \
		};                                                                                                  \
		Formats[0].Type = FSTRING_FORMAT_TYPE_STR;                                                          \
		Formats[1].Type = FSTRING_FORMAT_TYPE_QUERY32;                                                      \
		Formats[2].Type = FSTRING_FORMAT_TYPE_S32;                                                          \
		string Buffer = EString();                                                                          \
		fstring_format_list FormatList = { .FormatString = Format, .FormatCount = 3, .Formats = Formats };  \
		fstring_format_status Status = FString_WriteFormats(&FormatList, Buffer);                           \
		Assert(Status == FSTRING_FORMAT_BUFFER_TOO_SMALL);                                                  \
		Assert(Num == 37);                                                                                  \
		Assert(FormatList.TotalTextSize == 47);                                                             \
		Buffer = LString(60);                                                                               \
		Status = FString_WriteFormats(&FormatList, Buffer);                                                 \
		Assert(Status == FSTRING_FORMAT_VALID);                                                             \
	))                                                                                                      \
	TEST(FString_WriteFormats, PropagatesTypeError, (                                                       \
		string Format = CStringL("Hello, %s!");                                                             \
		fstring_format Formats[1] = { { .SpecifierString = { .Text = Format.Text + 7, .Length = 2 } } };    \
		string Buffer = LString(10);                                                                        \
		fstring_format_list FormatList = { .FormatString = Format, .FormatCount = 1, .Formats = Formats };  \
		fstring_format_status Status = FString_WriteFormats(&FormatList, Buffer);                           \
		Assert(Status == FSTRING_FORMAT_TYPE_INVALID);                                                      \
		Assert(FormatList.TotalTextSize == 7);                                                              \
	))                                                                                                      \
	TEST(FString, TiesEverythingTogether, (                                                                 \
		s32 Query = -1, Num = -192;                                                                         \
		string Name = CStringL("Jimmy");                                                                    \
		string Format = CStringL("Hello, %s! Your lucky number today%n is %d%%!");                          \
		string Result = FString(Format, Name, &Query, Num);                                                 \
		Assert(String_Cmp(Result, CStringL("Hello, Jimmy! Your lucky number today is -192%!")) == 0);       \
		Assert(Query == 37);                                                                                \
	))                                                                                                      \
	TEST(FString, HandlesEncodedFormatString, (                                                             \
		s32 Query = -1, Num = -192;                                                                         \
		string Name = CStringL("Jimmy");                                                                    \
		string Format = CStringL_UTF32("Hello, %s! Your lucky number today%n is %d%%!");                    \
		string Expected = CStringL_UTF32("Hello, Jimmy! Your lucky number today is -192%!");                \
		string Result = FString(Format, Name, &Query, Num);                                                 \
		Assert(Result.Encoding == STRING_ENCODING_UTF32);                                                   \
		Assert(String_Cmp(Result, Expected) == 0);                                                          \
		Assert(Query == 148);                                                                               \
	))                                                                                                      \
	TEST(FString, HandlesNoFormats, (                                                                       \
		string Format = CStringL("Hello! Your have no lucky number today. :(");                             \
		string Result = FString(Format);                                                                    \
		Assert(String_Cmp(Result, Format) == 0);                                                            \
		Assert(Format.Text != Result.Text);                                                                 \
	))                                                                                                      \
	TEST(FString, HandlesEmptyString, (                                                                     \
		string Result = FString(EString());                                                                 \
		Assert(String_Cmp(Result, EString()) == 0);                                                         \
	))                                                                                                      \
	/*TEST(FString, HandlesInvalidStrings, (                                                                  \
		string Format = CStringL("Hello, %-y! Today's temperature is %d.");                                 \
		string Result = FString(Format, CStringL("Jimmy"), 35);                                             \
		Assert(String_Cmp(Result, Format) == 0);                                                            \
		Format = CStringL("Hello, %");                                                                      \
		Result = FString(Format, CStringL("Jimmy"), 35);                                                    \
		Assert(String_Cmp(Result, Format) == 0);                                                            \
		Format = CStringL("Hello, %-s! Today's temperature is %2147483648$d.");                             \
		Result = FString(Format, CStringL("Jimmy"), 35);                                                    \
		Assert(String_Cmp(Result, Format) == 0);                                                            \
		Format = CStringL("Hello, %1$s! Today's temperature is %#d.");                                      \
		Result = FString(Format, CStringL("Jimmy"), 35);                                                    \
		Assert(String_Cmp(Result, Format) == 0);                                                            \
		Format = CStringL("Hello, %1$s! Today's temperature is %1$d.");                                     \
		Result = FString(Format, CStringL("Jimmy"), 35);                                                    \
		Assert(String_Cmp(Result, Format) == 0);                                                            \
		Format = CStringL("Hello, %1$s! Today's temperature is %3$d %4$c.");                                \
		Result = FString(Format, CStringL("Jimmy"), 35, 'c');                                               \
		Assert(String_Cmp(Result, Format) == 0);                                                            \
		Format = CStringL("Hello, %ls!");                                                                   \
		Result = FString(Format, CStringL("Jimmy"));                                                        \
		Assert(String_Cmp(Result, Format) == 0);                                                            \
	))*/                                                                                                      \
	//

#endif	// SECTION_STRING_TESTS

#endif	// INCLUDE_SOURCE
