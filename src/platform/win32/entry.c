/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                             *
*  Author: Aria Seiler                                                        *
*                                                                             *
*  This program is in the public domain. There is no implied warranty, so     *
*  use it at your own risk.                                                   *
*                                                                             *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <platform/main.c>

global win32_window			PrimaryWindow;
global win32_device_context DeviceContext;
global s64					CounterFrequency = 0;

internal void
Platform_LoadWin32(void)
{
	win32_teb		 *TEB	= (win32_teb *) Intrin_ReadGSQWord(48);
	win32_list_entry *Entry = TEB->PEB->Ldr->MemoryOrderList.Next;
	u32 Offset = OFFSET_OF(win32_ldr_data_table_entry, MemoryOrderLinks);
	Entry	   = Entry->Next->Next;
	win32_ldr_data_table_entry *TableEntry =
		(win32_ldr_data_table_entry *) ((u08 *) Entry
										- OFFSET_OF(
											win32_ldr_data_table_entry,
											MemoryOrderLinks
										));
	win32_module Kernel32 = TableEntry->DllBase;

	win32_image_dos_header *DOSHeader = (win32_image_dos_header *) Kernel32;
	win32_image_nt_headers *NTHeaders =
		(win32_image_nt_headers *) ((u08 *) DOSHeader + DOSHeader->e_lfanew);
	win32_image_export_directory *ExportDirectory =
		(win32_image_export_directory *) ((u08 *) DOSHeader
										  + NTHeaders->OptionalHeader
												.ExportTable.VirtualAddress);
	u32 *ExportNameTable =
		(u32 *) ((u08 *) DOSHeader + ExportDirectory->AddressOfNames);
	u16 *ExportOrdinalTable =
		(u16 *) ((u08 *) DOSHeader + ExportDirectory->AddressOfNameOrdinals);
	u32 *ExportAddressTable =
		(u32 *) ((u08 *) DOSHeader + ExportDirectory->AddressOfFunctions);

	u32 Index;
	u32 Low	 = 0;
	u32 High = ExportDirectory->NumberOfNames - 1;
	while (TRUE) {
		Index	  = (High + Low) / 2;
		c08 *Goal = "GetProcAddress";
		c08 *Curr = (c08 *) ((u08 *) DOSHeader + ExportNameTable[Index]);

		while (*Goal && *Curr && *Goal == *Curr) {
			Goal++;
			Curr++;
		}

		if (*Goal == *Curr) break;
		if (*Goal > *Curr) Low = Index;
		else High = Index;
	}

	u16 GetProcAddressOrdinal = ExportOrdinalTable[Index];
	u32 GetProcAddressRVA	  = ExportAddressTable[GetProcAddressOrdinal];
	Win32_GetProcAddress =
		(func_Win32_GetProcAddress *) ((u08 *) DOSHeader + GetProcAddressRVA);

	Win32_LoadLibraryA = (func_Win32_LoadLibraryA *)
		Win32_GetProcAddress(Kernel32, "LoadLibraryA");
	win32_module Gdi32	 = Win32_LoadLibraryA("gdi32.dll");
	win32_module User32	 = Win32_LoadLibraryA("user32.dll");
	win32_module Shell32 = Win32_LoadLibraryA("shell32.dll");

#define IMPORT(Module, ReturnType, Name, ...) \
      Win32_##Name = (func_Win32_##Name*)Win32_GetProcAddress(Module, #Name); \
      Assert(Win32_##Name);
#define X WIN32_FUNCS
#include <x.h>
}

internal void
Platform_LoadWGL(void)
{
	win32_module OpenGL32 = Win32_LoadLibraryA("opengl32.dll");
#define IMPORT(ReturnType, Name, ...) \
      WGL_##Name = (func_WGL_##Name*)Win32_GetProcAddress(OpenGL32, "wgl" #Name); \
      Assert(WGL_##Name);
#define X WGL_FUNCS_TYPE_1
#include <x.h>

	win32_window_class_a DummyWindowClass = { 0 };
	DummyWindowClass.Callback =
		(func_Win32_WindowCallback) Win32_DefWindowProcA;
	DummyWindowClass.Instance  = Win32_GetModuleHandleA(NULL);
	DummyWindowClass.ClassName = "VoxarcDummyWindowClass";

	Win32_RegisterClassA(&DummyWindowClass);
	win32_window DummyWindow = Win32_CreateWindowExA(
		0,
		DummyWindowClass.ClassName,
		"VoxarcDummyWindow",
		0,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		DummyWindowClass.Instance,
		NULL
	);

	vptr DummyDeviceContext = Win32_GetDC(DummyWindow);

	win32_pixel_format_descriptor PixelFormatDescriptor = { 0 };
	PixelFormatDescriptor.Size		= sizeof(win32_pixel_format_descriptor);
	PixelFormatDescriptor.Version	= 1;
	PixelFormatDescriptor.PixelType = PFD_TYPE_RGBA;
	PixelFormatDescriptor.Flags =
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLE_BUFFER;
	PixelFormatDescriptor.ColorBits	  = 32;
	PixelFormatDescriptor.AlphaBits	  = 8;
	PixelFormatDescriptor.LayerType	  = PFD_MAIN_PLANE;
	PixelFormatDescriptor.DepthBits	  = 24;
	PixelFormatDescriptor.StencilBits = 8;

	s32 PixelFormat =
		Win32_ChoosePixelFormat(DummyDeviceContext, &PixelFormatDescriptor);
	Win32_SetPixelFormat(
		DummyDeviceContext,
		PixelFormat,
		&PixelFormatDescriptor
	);
	vptr DummyRenderContext = WGL_CreateContext(DummyDeviceContext);
	WGL_MakeCurrent(DummyDeviceContext, DummyRenderContext);

#define IMPORT(ReturnType, Name, ...) \
      WGL_##Name = (func_WGL_##Name*)WGL_GetProcAddress("wgl" #Name); \
      Assert(WGL_##Name);
#define X WGL_FUNCS_TYPE_2
#include <x.h>

	WGL_MakeCurrent(DummyDeviceContext, 0);
	WGL_DeleteContext(DummyRenderContext);
	Win32_ReleaseDC(DummyWindow, DummyDeviceContext);
	Win32_DestroyWindow(DummyWindow);
}

opengl_funcs OpenGLFuncs;

internal opengl_funcs *
Platform_LoadOpenGL(void)
{
	if (OpenGLFuncs.Initialized) return &OpenGLFuncs;

	s32 PixelFormatAttribs[] = { WGL_DRAW_TO_WINDOW_ARB,
								 TRUE,
								 WGL_SUPPORT_OPENGL_ARB,
								 TRUE,
								 WGL_DOUBLE_BUFFER_ARB,
								 TRUE,
								 WGL_ACCELERATION_ARB,
								 WGL_FULL_ACCELERATION_ARB,
								 WGL_PIXEL_TYPE_ARB,
								 WGL_TYPE_RGBA_ARB,
								 WGL_COLOR_BITS_ARB,
								 32,
								 WGL_DEPTH_BITS_ARB,
								 24,
								 WGL_STENCIL_BITS_ARB,
								 8,
								 WGL_SAMPLE_BUFFERS_ARB,
								 1,
								 WGL_SAMPLES_ARB,
								 4,
								 0 };

	win32_pixel_format_descriptor PixelFormatDescriptor;
	s32							  PixelFormat;
	u32							  FormatCount;
	WGL_ChoosePixelFormatARB(
		DeviceContext,
		PixelFormatAttribs,
		0,
		1,
		&PixelFormat,
		&FormatCount
	);
	Win32_DescribePixelFormat(
		DeviceContext,
		PixelFormat,
		sizeof(win32_pixel_format_descriptor),
		&PixelFormatDescriptor
	);

#if defined(_DEBUG)
	u32 DebugBit = WGL_CONTEXT_DEBUG_BIT_ARB;
#else
	u32 DebugBit = 0;
#endif

	Win32_SetPixelFormat(DeviceContext, PixelFormat, &PixelFormatDescriptor);
	s32 AttribList[] = { WGL_CONTEXT_MAJOR_VERSION_ARB,
						 4,
						 WGL_CONTEXT_MINOR_VERSION_ARB,
						 6,
						 WGL_CONTEXT_FLAGS_ARB,
						 DebugBit | WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
						 WGL_CONTEXT_PROFILE_MASK_ARB,
						 WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
						 0 };

	win32_opengl_render_context RenderContext =
		WGL_CreateContextAttribsARB(DeviceContext, 0, AttribList);
	WGL_MakeCurrent(DeviceContext, RenderContext);

	win32_handle OpenGL32 = Win32_GetModuleHandleA("opengl32.dll");
	Assert(OpenGL32);

#define IMPORT(ReturnType, Name, ...) \
      OpenGLFuncs.OpenGL_##Name = (func_OpenGL_##Name*)Win32_GetProcAddress(OpenGL32, "gl" #Name); \
      Assert(OpenGLFuncs.OpenGL_##Name);
#define X OPENGL_FUNCS_TYPE_1
#include <x.h>

#define IMPORT(ReturnType, Name, ...) \
      OpenGLFuncs.OpenGL_##Name = (func_OpenGL_##Name*)WGL_GetProcAddress("gl" #Name); \
      Assert(OpenGLFuncs.OpenGL_##Name);
#define X OPENGL_FUNCS_TYPE_2
#include <x.h>

	OpenGLFuncs.Initialized = TRUE;

	return &OpenGLFuncs;
}

internal s64 Platform_WindowCallback(
	win32_window Window,
	u32			 Message,
	s64			 WParam,
	s64			 LParam
);

internal void
Platform_CreateWindow(void)
{
	Platform_LoadWGL();

	win32_window_class_a WindowClass = { 0 };
	WindowClass.Callback			 = Platform_WindowCallback;
	WindowClass.Instance			 = Win32_GetModuleHandleA(NULL);
	WindowClass.Icon				 = Win32_LoadIconA(NULL, IDI_APPLICATION);
	WindowClass.Cursor				 = Win32_LoadCursorA(NULL, IDC_ARROW);
	WindowClass.Background = (win32_brush) Win32_GetStockObject(BRUSH_BLACK);
	WindowClass.ClassName  = "VoxarcWindowClass";
	Win32_RegisterClassA(&WindowClass);

	PrimaryWindow = Win32_CreateWindowExA(
		0,
		WindowClass.ClassName,
		"Voxarc",
		WS_OVERLAPPED
			| WS_SYSMENU
			| WS_CAPTION
			| WS_VISIBLE
			| WS_THICKFRAME
			| WS_MINIMIZEBOX
			| WS_MAXIMIZEBOX,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		WindowClass.Instance,
		NULL
	);
	Assert(PrimaryWindow);

	DeviceContext = Win32_GetDC(PrimaryWindow);

	win32_raw_input_device RawInputDevice;
	RawInputDevice.UsagePage = HID_USAGE_PAGE_GENERIC;
	RawInputDevice.Usage	 = HID_USAGE_GENERIC_MOUSE;
	RawInputDevice.Flags	 = 0;
	RawInputDevice.Target	 = PrimaryWindow;
	b32 Res					 = Win32_RegisterRawInputDevices(
		 &RawInputDevice,
		 1,
		 sizeof(win32_raw_input_device)
	 );
	Assert(Res == TRUE);

	_G.WindowedApp = TRUE;
}

internal vptr
Platform_AllocateMemory(u64 Size)
{
	vptr MemoryBlock =
		Win32_VirtualAlloc(0, Size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	Assert(MemoryBlock);
	return MemoryBlock;
}

internal void
Platform_FreeMemory(vptr Base, u64 Size)
{
	Win32_VirtualFree(Base, 0, MEM_RELEASE);
}

internal u64
Platform_GetFileLength(file_handle FileHandle)
{
	win32_large_integer Size;
	Size.QuadPart = 0;

	b08 Success = Win32_GetFileSizeEx(FileHandle.Handle, &Size);
	Assert(Success);

	return Size.QuadPart;
}

internal b08
Platform_OpenFile(file_handle *FileHandle, c08 *FileName, file_mode OpenMode)
{
	string Name;
	if (_G.UtilIsLoaded) {
		Stack_Push();
		Name = FNStringL("%s", FileName);
	} else {
		Name.Length = _Mem_BytesUntil(FileName, 0);
		Name.Text	= Platform_AllocateMemory(Name.Length);
		_Mem_Cpy(Name.Text, FileName, Name.Length + 1);
	}
	for (usize I = 0; I < Name.Length; I++)
		if (Name.Text[I] == '/') Name.Text[I] = '\\';

	u32 DesiredAccess		= 0;
	u32 CreationDisposition = 0;

	switch (OpenMode) {
		case FILE_READ: {
			DesiredAccess		= GENERIC_READ;
			CreationDisposition = OPEN_EXISTING;
		} break;

		case FILE_WRITE: {
			DesiredAccess		= GENERIC_WRITE;
			CreationDisposition = CREATE_ALWAYS;
		} break;

		case FILE_APPEND: {
			DesiredAccess		= GENERIC_WRITE;
			CreationDisposition = CREATE_NEW;
		} break;

		default: {
			STOP;
		}
	}

	win32_handle GivenHandle = Win32_CreateFileA(
		Name.Text,
		DesiredAccess,
		FILE_SHARE_READ,
		NULL,
		CreationDisposition,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (_G.UtilIsLoaded) Stack_Pop();
	else Platform_FreeMemory(Name.Text, Name.Length);

	if (GivenHandle != INVALID_HANDLE_VALUE) {
		FileHandle->Handle = GivenHandle;
		return TRUE;
	} else {
		FileHandle->Handle = NULL;
	}

	return FALSE;
}

internal u64
Platform_ReadFile(file_handle FileHandle, vptr Dest, u64 Length, u64 Offset)
{
	win32_overlapped Overlapped		  = { 0 };
	u64				*OverlappedOffset = (u64 *) &Overlapped.Offset;
	*OverlappedOffset				  = Offset;

	u64 TotalBytesRead = 0;
	while (Length) {
		u32 BytesRead;
		u32 BytesToRead = Length % (U32_MAX + 1ULL);
		b08 Success		= Win32_ReadFile(
			FileHandle.Handle,
			Dest,
			BytesToRead,
			&BytesRead,
			&Overlapped
		);

		if (!Success) {
			u32 DEBUG_Err = Win32_GetLastError();
			Assert(Success, "Failed to read data from file");
			return 0;
		}

		TotalBytesRead	  += BytesRead;
		Length			  -= BytesRead;
		*OverlappedOffset += BytesRead;
	}

	return TotalBytesRead;
}

internal u64
Platform_WriteFile(file_handle FileHandle, vptr Src, u64 Length, u64 Offset)
{
	win32_overlapped Overlapped		  = { 0 };
	u64				*OverlappedOffset = (u64 *) &Overlapped.Offset;
	*OverlappedOffset				  = Offset;

	u32 TotalBytesWritten = 0;
	while (Length) {
		u32 BytesWritten;
		u32 BytesToWrite = Length % (U32_MAX + 1ULL);
		b08 Success		 = Win32_WriteFile(
			 FileHandle.Handle,
			 Src,
			 BytesToWrite,
			 &BytesWritten,
			 &Overlapped
		 );

		if (!Success) {
			u32 DEBUG_Err = Win32_GetLastError();
			Assert(Success, "Failed to write data to file");
			return 0;
		}

		TotalBytesWritten += BytesWritten;
		Length			  -= BytesWritten;
		*OverlappedOffset += BytesWritten;
	}

	return TotalBytesWritten;
}

internal void
Platform_WriteConsole(string Message)
{
	win32_handle Handle = Win32_GetStdHandle(STD_OUTPUT_HANDLE);
	Win32_WriteConsoleA(Handle, Message.Text, Message.Length, NULL, NULL);

	c08 *StringCopy = Platform_AllocateMemory(Message.Length + 1);
	Mem_Cpy(StringCopy, Message.Text, Message.Length);
	StringCopy[Message.Length] = 0;
	Win32_OutputDebugStringA(StringCopy);
	Platform_FreeMemory(StringCopy, Message.Length + 1);
}

internal void
Platform_WriteError(string Message, u32 Exit)
{
	win32_handle Handle = Win32_GetStdHandle(STD_ERROR_HANDLE);
	Win32_WriteConsoleA(Handle, Message.Text, Message.Length, NULL, NULL);

	c08 *StringCopy = Platform_AllocateMemory(Message.Length + 1);
	Mem_Cpy(StringCopy, Message.Text, Message.Length);
	StringCopy[Message.Length] = 0;
	Win32_OutputDebugStringA(StringCopy);
	Platform_FreeMemory(StringCopy, Message.Length + 1);

	if (Exit) Platform_Exit(Exit);
}

internal void
Platform_CloseFile(file_handle FileHandle)
{
	Win32_CloseHandle(FileHandle.Handle);
}

internal void
Platform_GetFileTime(
	c08		 *FileName,
	datetime *CreationTime,
	datetime *LastAccessTime,
	datetime *LastWriteTime
)
{
	file_handle		FileHandle;
	win32_file_time _Times[3];
	datetime	   *Times[3] = { CreationTime, LastAccessTime, LastWriteTime };
	Platform_OpenFile(&FileHandle, FileName, FILE_READ);
	Win32_GetFileTime(FileHandle.Handle, _Times + 0, _Times + 1, _Times + 2);
	Platform_CloseFile(FileHandle);

	win32_system_time SystemTime;
	for (u32 I = 0; I < 3; I++) {
		if (!Times[I]) continue;
		Win32_FileTimeToSystemTime(_Times + I, &SystemTime);
		Times[I]->Year		  = SystemTime.Year;
		Times[I]->Month		  = SystemTime.Month;
		Times[I]->Day		  = SystemTime.Day;
		Times[I]->DayOfWeek	  = SystemTime.DayOfWeek;
		Times[I]->Hour		  = SystemTime.Hour;
		Times[I]->Minute	  = SystemTime.Minute;
		Times[I]->Second	  = SystemTime.Second;
		Times[I]->Millisecond = SystemTime.Milliseconds;
	}
}

internal r64
Platform_GetTime(void)
{
	s64 Counts = 0;
	Win32_QueryPerformanceCounter(&Counts);
	return (r64) Counts / CounterFrequency;
}

internal s08
Platform_CmpFileTime(datetime A, datetime B)
{
	u16 AVals[] = { A.Year,	  A.Month,	A.Day,		  A.Hour,
					A.Minute, A.Second, A.Millisecond };
	u16 BVals[] = { B.Year,	  B.Month,	B.Day,		  B.Hour,
					B.Minute, B.Second, B.Millisecond };
	u32 Count	= sizeof(AVals) / sizeof(AVals[0]);
	for (u32 I = 0; I < Count; I++) {
		if (AVals[I] < BVals[I]) return LESS;
		if (AVals[I] > BVals[I]) return GREATER;
	}
	return EQUAL;
}

internal b08
Platform_CreateThread(
	thread_handle *ThreadHandle,
	s32 (*Callback)(vptr UserData),
	vptr UserData
)
{
	return FALSE;
}

internal b08
Platform_JoinThread(thread_handle ThreadHandle)
{
	return FALSE;
}

internal void
Platform_LockMutex(u32 *Mutex)
{ }

internal void
Platform_UnlockMutex(u32 *Mutex)
{ }

internal void
Platform_GetProcAddress(platform_module *Module, c08 *Name, vptr *ProcOut)
{
	vptr Proc = (vptr) Win32_GetProcAddress(Module->DLL, Name);
	if (!Proc) *ProcOut = (vptr) Platform_Stub;
	else *ProcOut = Proc;
}

internal b08
Platform_IsModuleBackendOpened(platform_module *Module)
{
	return !!Module->DLL;
}

internal void
Platform_OpenModuleBackend(platform_module *Module)
{
	Module->DLL = Win32_LoadLibraryA(Module->FileName);
}

internal void
Platform_CloseModuleBackend(platform_module *Module)
{
	Win32_FreeLibrary(Module->DLL);
	Module->DLL = NULL;
}

internal void
Platform_HideCursor(win32_window Window)
{
	Win32_SetCursor(NULL);

	Win32_GetCursorPos(&_G.RestoreCursorPos);
	Win32_ScreenToClient(Window, &_G.RestoreCursorPos);

	win32_rect ClipRect;
	Win32_GetClientRect(Window, &ClipRect);
	Win32_ClientToScreen(Window, (v2s32 *) &ClipRect.Left);
	Win32_ClientToScreen(Window, (v2s32 *) &ClipRect.Right);
	Win32_ClipCursor(&ClipRect);
}

internal void
Platform_ShowCursor(win32_window Window)
{
	Win32_ClipCursor(NULL);

	Win32_ClientToScreen(Window, &_G.RestoreCursorPos);
	Win32_SetCursorPos(_G.RestoreCursorPos.X, _G.RestoreCursorPos.Y);

	Win32_SetCursor(Win32_LoadCursorA(NULL, IDC_ARROW));
}

internal s64
Platform_WindowCallback(
	win32_window Window,
	u32			 Message,
	s64			 WParam,
	s64			 LParam
)
{
	switch (Message) {
		case WM_DESTROY:
		case WM_CLOSE  : {
			_G.ExecutionState = EXECUTION_ENDED;
		}
			return 0;

		case WM_SIZE: {
			_G.WindowSize.X	 = (s16) LParam;
			_G.WindowSize.Y	 = (s16) (LParam >> 16);
			_G.Updates		|= WINDOW_RESIZED;
		}
			return 0;

		case WM_SETFOCUS: {
			if (_G.FocusState == FOCUS_NONE) {
				if (_G.CursorIsDisabled) Platform_HideCursor(Window);

				Mem_Set(_G.Keys, 0, sizeof(_G.Keys));
				Mem_Set(_G.Buttons, 0, sizeof(_G.Buttons));

				_G.FocusState = FOCUS_CLIENT;
			}
		}
			return 0;

		case WM_KILLFOCUS: {
			// if(_G.CursorIsDisabled)
			//     Platform_ShowCursor(Window);

			_G.FocusState = FOCUS_NONE;
		}
			return 0;

		case WM_MOUSEACTIVATE: {
			if (((LParam >> 16) & 0xFFFF) == WM_LBUTTONDOWN) {
				if ((LParam & 0xFFFF) != HTCLIENT) _G.FocusState = FOCUS_FRAME;
			}
		}
			return MA_ACTIVATEANDEAT;

		case WM_INPUT: {
			Stack_Push();

			u32					   Size;
			win32_raw_input_handle Handle = (vptr) LParam;
			u32					   Res	  = Win32_GetRawInputData(
				  Handle,
				  RID_INPUT,
				  NULL,
				  &Size,
				  sizeof(win32_raw_input_header)
			  );
			Assert(Res == 0);
			win32_raw_input *Data = Stack_Allocate(sizeof(win32_raw_mouse));
			Res					  = Win32_GetRawInputData(
				  Handle,
				  RID_INPUT,
				  Data,
				  &Size,
				  sizeof(win32_raw_input_header)
			  );
			Assert(Res == sizeof(win32_raw_input));

			if (Data->Mouse.Flags & MOUSE_MOVE_ABSOLUTE) {
				Assert(FALSE, "Absolute mouse movement not supported.");
			} else {
				_G.CursorPos = (v2s32) { _G.CursorPos.X + Data->Mouse.LastX,
										 _G.CursorPos.Y - Data->Mouse.LastY };
			}

			// Automatically goes through RI_MOUSE_BUTTON_1_DOWN/UP,
			// RI_MOUSE_BUTTON_2_DOWN/UP, etc.
			for (u32 I = 0; I < 6; I++)
				if (Data->Mouse.ButtonFlags & (1 << (2 * I)))
					_G.Buttons[I] = PRESSED;
				else if (Data->Mouse.ButtonFlags & (2 << (2 * I)))
					_G.Buttons[I] = RELEASED;
				else if (_G.Buttons[I] == PRESSED) _G.Buttons[I] = HELD;

			Stack_Pop();
		}
			return 0;

		case WM_KEYUP:
		case WM_KEYDOWN:
		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN: {
			u08 ScanCode   = (LParam >> 16) & 0xFF;
			b08 IsExtended = (LParam >> 24) & 0x01;
			b08 WasDown	   = (LParam >> 30) & 0x01;
			b08 IsUp	   = (LParam >> 31) & 0x01;

			key_state KeyState;
			if (IsUp == TRUE) KeyState = RELEASED;
			else if (WasDown == FALSE) KeyState = PRESSED;
			else KeyState = HELD;

			if (IsExtended) {
				// Exclude NumLock
				if (ScanCode != 0x45) ScanCode |= 0x80;
			} else {
				// Pause key
				if (ScanCode == 0x45) ScanCode = 0xFF;
				// Alt + PrintScreen
				else if (ScanCode == 0x54) ScanCode = 0x80 | 0x37;
			}

			if (IsUp == TRUE) KeyState = RELEASED;
			else if (_G.Keys[ScanCode] == RELEASED) KeyState = PRESSED;
			// else if(_G.Keys) KeyState = PRESSED;

			_G.Keys[ScanCode] = KeyState;
		}
			return 0;
	}

	return Win32_DefWindowProcA(Window, Message, WParam, LParam);
}

internal void
Platform_ParseCommandLine(void)
{
	// Get and split the CLI arguments
	c16	 *CmdLine = Win32_GetCommandLineW();
	c16 **Args	  = Win32_CommandLineToArgvW(CmdLine, (s32 *) &_G.ArgCount);
	u32	 *Sizes	  = Platform_AllocateMemory(_G.ArgCount * sizeof(u32));

	// Find and allocate the total size after converting to utf-8
	u32 Size = _G.ArgCount * sizeof(string);
	for (u32 I = 0; I < _G.ArgCount; I++) {
		Sizes[I] = Win32_WideCharToMultiByte(
			CP_UTF8,
			0,
			Args[I],
			-1,
			NULL,
			0,
			NULL,
			NULL
		);
		Size += Sizes[I];
	}
	_G.Args = Platform_AllocateMemory(Size);

	c08 *Cursor = (c08 *) (_G.Args + _G.ArgCount);
	for (u32 I = 0; I < _G.ArgCount; I++) {
		_G.Args[I] = CLEString(Cursor, Sizes[I], STRING_ENCODING_ASCII);
		u32 S	   = Win32_WideCharToMultiByte(
			 CP_UTF8,
			 0,
			 Args[I],
			 -1,
			 _G.Args[I].Text,
			 Sizes[I],
			 NULL,
			 NULL
		 );
		Assert(S == Sizes[I]);
		Cursor += Sizes[I];
	}

	Win32_LocalFree(Args);
	Platform_FreeMemory(Sizes, _G.ArgCount * sizeof(u32));
}

internal void
Platform_Exit(u32 ExitCode)
{
	Win32_ExitProcess(ExitCode);
	UNREACHABLE;
}

external void
Platform_Entry(void)
{
	_F = (platform_funcs) {
#define EXPORT(R, N, ...) N,
#define X PLATFORM_FUNCS
#include <x.h>
	};
	_G.Funcs = &_F;

	Platform_LoadWin32();
	Platform_ParseCommandLine();
	Win32_QueryPerformanceFrequency(&CounterFrequency);

	platform_module *UtilModule = Platform_LoadModule(UTIL_MODULE_NAME);

	Platform_CreateWindow();

	Platform_LoadModule(CStringL("base"));

	HASHMAP_FOREACH (
		I,
		Hash,
		string,
		Key,
		platform_module *,
		Module,
		&_G.ModuleTable
	)
	{
		stack Stack;
		if (_G.UtilIsLoaded) Stack = Stack_Get();
		Module->Init(&_G);
		if (_G.UtilIsLoaded) Stack_Set(Stack);
	}

	s64 CountsPerSecond;
	Win32_QueryPerformanceFrequency(&CountsPerSecond);

	s64 StartTime;
	Win32_QueryPerformanceCounter(&StartTime);

	while (_G.ExecutionState == EXECUTION_RUNNING) {
		// Will reload the modules if necessary
		HASHMAP_FOREACH (
			I,
			Hash,
			string,
			Key,
			platform_module *,
			Module,
			&_G.ModuleTable
		)
		{
			stack Stack;
			if (_G.UtilIsLoaded) Stack = Stack_Get();
			Platform_ReloadModule(Module);
			if (_G.UtilIsLoaded) Stack_Set(Stack);
		}

		win32_message Message;
		while (Win32_PeekMessageA(&Message, PrimaryWindow, 0, 0, PM_REMOVE)) {
			if (Message.Message == WM_QUIT) {
				_G.ExecutionState = EXECUTION_ENDED;
				break;
			}

			Win32_TranslateMessage(&Message);
			Win32_DispatchMessageA(&Message);
		}

		if (_G.WindowedApp) {
			if (_G.CursorIsDisabled) {
				// HACK: Not sure why, but the cursor keeps reappearing
				// otherwise
				Win32_SetCursor(NULL);
			}

			if (_G.FocusState == FOCUS_CLIENT
				&& !_G.CursorIsDisabled
				&& _G.Buttons[Button_Left] == PRESSED)
			{
				Platform_HideCursor(PrimaryWindow);
				_G.Updates			|= CURSOR_DISABLED;
				_G.CursorIsDisabled	 = TRUE;
			}
			if (_G.CursorIsDisabled && _G.Keys[ScanCode_Escape] == PRESSED) {
				Platform_ShowCursor(PrimaryWindow);
				_G.CursorIsDisabled = FALSE;
			}
		}

		HASHMAP_FOREACH (
			I,
			Hash,
			string,
			Key,
			platform_module *,
			Module,
			&_G.ModuleTable
		)
		{
			stack Stack;
			if (_G.UtilIsLoaded) Stack = Stack_Get();
			Module->Update(&_G);
			if (_G.UtilIsLoaded) Stack_Set(Stack);
		}

		if (_G.WindowedApp) Win32_SwapBuffers(DeviceContext);

		s64 EndTime;
		Win32_QueryPerformanceCounter(&EndTime);

		s64 ElapsedTime = EndTime - StartTime;
		StartTime		= EndTime;

		_G.FPS = CountsPerSecond / (r64) ElapsedTime;
	}

	HASHMAP_FOREACH (
		I,
		Hash,
		string,
		Key,
		platform_module *,
		Module,
		&_G.ModuleTable
	)
	{
		if (Module->IsUtil) continue;

		stack Stack;
		if (_G.UtilIsLoaded) Stack = Stack_Get();
		Module->Deinit(&_G);
		if (_G.UtilIsLoaded) Stack_Set(Stack);
		Platform_UnloadModule(Module);
	}

	Stack_Pop();

	UtilModule->Deinit(&_G);
	Platform_UnloadModule(UtilModule);

	Platform_Exit(0);
}
