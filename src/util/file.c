/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                             *
*  Author: Aria Seiler                                                        *
*                                                                             *
*  This program is in the public domain. There is no implied warranty, so     *
*  use it at your own risk.                                                   *
*                                                                             *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef INCLUDE_HEADER

#define FILE_FUNCS \
   EXPORT(string, File_Read, c08 *FileName, u64 Length, u64 Offset)

#endif

#ifdef INCLUDE_SOURCE

internal string
File_Read(c08 *FileName, u64 Length, u64 Offset)
{
	file_handle FileHandle;
	b08			Opened = Platform_OpenFile(&FileHandle, FileName, FILE_READ);
	Assert(Opened, "Invalid file name!");

	if (Length == 0) Length = Platform_GetFileLength(FileHandle);

	// TODO: We should auto-detect the encoding
	c08 *Text		= Stack_Allocate(Length + 1);
	u64	 BytesRead	= Platform_ReadFile(FileHandle, Text, Length, Offset);
	Text[BytesRead] = 0;
	string String	= { .Length	  = BytesRead,
						.Encoding = STRING_ENCODING_ASCII,
						.Text	  = Text };

	Platform_CloseFile(FileHandle);

	return String;
}

#endif
