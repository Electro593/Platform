/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
**                                                                         **
**  Author: Aria Seiler                                                    **
**                                                                         **
**  This program is in the public domain. There is no implied warranty,    **
**  so use it at your own risk.                                            **
**                                                                         **
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <platform/win32/win32.h>
#include <platform/platform.c>

typedef win32_handle file_handle;

global win32_window PrimaryWindow;
global platform_state *Platform;
global win32_device_context DeviceContext;
global s64 CounterFrequency = 0;

internal void Platform_Stub(void) { };

internal void
Platform_LoadWin32(void)
{
   win32_teb *TEB = (win32_teb*)Intrin_ReadGSQWord(48);
   win32_list_entry *Entry = TEB->PEB->Ldr->MemoryOrderList.Next;
   u32 Offset = OFFSET_OF(win32_ldr_data_table_entry, MemoryOrderLinks);
   Entry = Entry->Next->Next;
   win32_ldr_data_table_entry *TableEntry = (win32_ldr_data_table_entry*)((u08*)Entry - OFFSET_OF(win32_ldr_data_table_entry, MemoryOrderLinks));
   win32_module Kernel32 = TableEntry->DllBase;
   
   win32_image_dos_header *DOSHeader = (win32_image_dos_header*)Kernel32;
   win32_image_nt_headers *NTHeaders = (win32_image_nt_headers*)((u08*)DOSHeader + DOSHeader->e_lfanew);
   win32_image_export_directory *ExportDirectory = (win32_image_export_directory*)((u08*)DOSHeader + NTHeaders->OptionalHeader.ExportTable.VirtualAddress);
   u32 *ExportNameTable    = (u32*)((u08*)DOSHeader + ExportDirectory->AddressOfNames);
   u16 *ExportOrdinalTable = (u16*)((u08*)DOSHeader + ExportDirectory->AddressOfNameOrdinals);
   u32 *ExportAddressTable = (u32*)((u08*)DOSHeader + ExportDirectory->AddressOfFunctions);
   
   u32 Index;
   u32 Low = 0;
   u32 High = ExportDirectory->NumberOfNames - 1;
   while(TRUE) {
      Index = (High + Low) / 2;
      c08 *Goal = "GetProcAddress";
      c08 *Curr = (c08*)((u08*)DOSHeader + ExportNameTable[Index]);
      
      while(*Goal && *Curr && *Goal == *Curr) {
         Goal++;
         Curr++;
      }
      
      if(*Goal == *Curr) break;
      if(*Goal > *Curr) Low = Index;
      else High = Index;
   }
   
   u16 GetProcAddressOrdinal = ExportOrdinalTable[Index];
   u32 GetProcAddressRVA = ExportAddressTable[GetProcAddressOrdinal];
   Win32_GetProcAddress = (func_Win32_GetProcAddress*)((u08*)DOSHeader + GetProcAddressRVA);
   
   Win32_LoadLibraryA = (func_Win32_LoadLibraryA*)Win32_GetProcAddress(Kernel32, "LoadLibraryA");
   win32_module Gdi32 = Win32_LoadLibraryA("gdi32.dll");
   win32_module User32 = Win32_LoadLibraryA("user32.dll");
   win32_module Shell32 = Win32_LoadLibraryA("shell32.dll");
   
   #define IMPORT(Module, ReturnType, Name, ...) \
      Win32_##Name = (func_Win32_##Name*)Win32_GetProcAddress(Module, #Name); \
      Assert(Win32_##Name);
   #define X WIN32_FUNCS
   #include <x.h>
}

#ifdef _OPENGL
internal void
Platform_LoadWGL(void)
{
   win32_module OpenGL32 = Win32_LoadLibraryA("opengl32.dll");
   #define IMPORT(ReturnType, Name, ...) \
      WGL_##Name = (func_WGL_##Name*)Win32_GetProcAddress(OpenGL32, "wgl" #Name); \
      Assert(WGL_##Name);
   #define X WGL_FUNCS_TYPE_1
   #include <x.h>
   
   win32_window_class_a DummyWindowClass = {0};
   DummyWindowClass.Callback = (func_Win32_WindowCallback)Win32_DefWindowProcA;
   DummyWindowClass.Instance = Win32_GetModuleHandleA(NULL);
   DummyWindowClass.ClassName = "VoxarcDummyWindowClass";
   
   Win32_RegisterClassA(&DummyWindowClass);
   win32_window DummyWindow = Win32_CreateWindowExA(
      0, DummyWindowClass.ClassName, "VoxarcDummyWindow", 0,
      CW_USEDEFAULT, CW_USEDEFAULT,
      CW_USEDEFAULT, CW_USEDEFAULT,
      NULL, NULL, DummyWindowClass.Instance, NULL
   );
   
   vptr DummyDeviceContext = Win32_GetDC(DummyWindow);
   
   win32_pixel_format_descriptor PixelFormatDescriptor = {0};
   PixelFormatDescriptor.Size = sizeof(win32_pixel_format_descriptor);
   PixelFormatDescriptor.Version = 1;
   PixelFormatDescriptor.PixelType = PFD_TYPE_RGBA;
   PixelFormatDescriptor.Flags = PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLE_BUFFER;
   PixelFormatDescriptor.ColorBits = 32;
   PixelFormatDescriptor.AlphaBits = 8;
   PixelFormatDescriptor.LayerType = PFD_MAIN_PLANE;
   PixelFormatDescriptor.DepthBits = 24;
   PixelFormatDescriptor.StencilBits = 8;
   
   s32 PixelFormat = Win32_ChoosePixelFormat(DummyDeviceContext, &PixelFormatDescriptor);
   Win32_SetPixelFormat(DummyDeviceContext, PixelFormat, &PixelFormatDescriptor);
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
   
   s32 PixelFormatAttribs[] = {
      WGL_DRAW_TO_WINDOW_ARB, TRUE,
      WGL_SUPPORT_OPENGL_ARB, TRUE,
      WGL_DOUBLE_BUFFER_ARB,  TRUE,
      WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
      WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
      WGL_COLOR_BITS_ARB,     32,
      WGL_DEPTH_BITS_ARB,     24,
      WGL_STENCIL_BITS_ARB,   8,
      WGL_SAMPLE_BUFFERS_ARB, 1,
      WGL_SAMPLES_ARB,        4,
      0
   };
   
   win32_pixel_format_descriptor PixelFormatDescriptor;
   s32 PixelFormat;
   u32 FormatCount;
   WGL_ChoosePixelFormatARB(DeviceContext, PixelFormatAttribs, 0, 1, &PixelFormat, &FormatCount);
   Win32_DescribePixelFormat(DeviceContext, PixelFormat, sizeof(win32_pixel_format_descriptor), &PixelFormatDescriptor);
   
   #if defined(_DEBUG)
      u32 DebugBit = WGL_CONTEXT_DEBUG_BIT_ARB;
   #else
      u32 DebugBit = 0;
   #endif
   
   Win32_SetPixelFormat(DeviceContext, PixelFormat, &PixelFormatDescriptor);
   s32 AttribList[] = {
      WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
      WGL_CONTEXT_MINOR_VERSION_ARB, 6,
      WGL_CONTEXT_FLAGS_ARB,         DebugBit | WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
      WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
      0
   };
   
   win32_opengl_render_context RenderContext = WGL_CreateContextAttribsARB(DeviceContext, 0, AttribList);
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
#endif

internal void
Platform_CreateWindow(void) {
   #ifdef _OPENGL
   Platform_LoadWGL();
   #endif
   
   win32_window_class_a WindowClass = {0};
   WindowClass.Callback = Platform_WindowCallback;
   WindowClass.Instance = Win32_GetModuleHandleA(NULL);
   WindowClass.Icon = Win32_LoadIconA(NULL, IDI_APPLICATION);
   WindowClass.Cursor = Win32_LoadCursorA(NULL, IDC_ARROW);
   WindowClass.Background = (win32_brush)Win32_GetStockObject(BRUSH_BLACK);
   WindowClass.ClassName = "VoxarcWindowClass";
   Win32_RegisterClassA(&WindowClass);
   
   PrimaryWindow = Win32_CreateWindowExA(
      0, WindowClass.ClassName, "Voxarc",
      WS_OVERLAPPED|WS_SYSMENU|WS_CAPTION|WS_VISIBLE|WS_THICKFRAME|WS_MINIMIZEBOX|WS_MAXIMIZEBOX,
      CW_USEDEFAULT, CW_USEDEFAULT,
      CW_USEDEFAULT, CW_USEDEFAULT,
      NULL, NULL, WindowClass.Instance, NULL
   );
   Assert(PrimaryWindow);
   
   DeviceContext = Win32_GetDC(PrimaryWindow);
   
   win32_raw_input_device RawInputDevice;
   RawInputDevice.UsagePage = HID_USAGE_PAGE_GENERIC;
   RawInputDevice.Usage     = HID_USAGE_GENERIC_MOUSE;
   RawInputDevice.Flags     = 0;
   RawInputDevice.Target    = PrimaryWindow;
   b32 Res = Win32_RegisterRawInputDevices(&RawInputDevice, 1, sizeof(win32_raw_input_device));
   Assert(Res == TRUE);
   
   Platform->WindowedApp = TRUE;
}

internal void
Platform_Assert(c08 *File,
                u32 Line,
                c08 *Expression,
                c08 *Message)
{
   c08 LineStr[11];
   u32 Index = sizeof(LineStr);
   LineStr[--Index] = 0;
   do {
      LineStr[--Index] = (Line % 10) + '0';
      Line /= 10;
   } while(Line);
   
   Win32_OutputDebugStringA(File);
   Win32_OutputDebugStringA(": ");
   Win32_OutputDebugStringA(LineStr + Index);
   if(Expression[0] != 0) {
      Win32_OutputDebugStringA(": Assertion hit!\n\t(");
      Win32_OutputDebugStringA(Expression);
      Win32_OutputDebugStringA(") was FALSE\n");
   } else {
      Win32_OutputDebugStringA(": \n");
   }
   if(Message[0] != 0) {
      Win32_OutputDebugStringA("\t");
      Win32_OutputDebugStringA(Message);
   }
   Win32_OutputDebugStringA("\n");
}

internal vptr
Platform_AllocateMemory(u64 Size)
{
   vptr MemoryBlock = Win32_VirtualAlloc(0, Size, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
   Assert(MemoryBlock);
   return MemoryBlock;
}

internal void
Platform_FreeMemory(vptr Base)
{
   Win32_VirtualFree(Base, 0, MEM_RELEASE);
}

internal u64
Platform_GetFileLength(file_handle *FileHandle)
{
   win32_large_integer Size;
   Size.QuadPart = 0;
   
   Assert(Win32_GetFileSizeEx(*FileHandle, &Size));
   
   return Size.QuadPart;
}

internal b08
Platform_OpenFile(file_handle **FileHandle,
                  c08 *FileName,
                  file_mode OpenMode)
{
   u32 DesiredAccess = 0;
   u32 CreationDisposition = 0;
   
   switch(OpenMode) {
      case FILE_READ: {
         DesiredAccess = GENERIC_READ;
         CreationDisposition = OPEN_EXISTING;
      } break;
      
      case FILE_WRITE: {
         DesiredAccess = GENERIC_WRITE;
         CreationDisposition = CREATE_ALWAYS;
      } break;
      
      case FILE_APPEND: {
         DesiredAccess = GENERIC_WRITE;
         CreationDisposition = CREATE_NEW;
      } break;
      
      default: {
         STOP;
      }
   }
   
   win32_handle GivenHandle = Win32_CreateFileA(FileName, DesiredAccess, FILE_SHARE_READ,
                                                NULL, CreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
   
   if(GivenHandle != INVALID_HANDLE_VALUE) {
      **(win32_handle**)FileHandle = GivenHandle;
      return TRUE;
   } else {
      **(win32_handle**)FileHandle = NULL;
   }
   
   return FALSE;
}

internal u64
Platform_ReadFile(file_handle *FileHandle,
                  vptr Dest,
                  u64 Length,
                  u64 Offset)
{
   win32_overlapped Overlapped = {0};
   u64 *OverlappedOffset = (u64*)&Overlapped.Offset;
   *OverlappedOffset = Offset;
   
   u64 TotalBytesRead = 0;
   while(Length) {
      u32 BytesRead;
      u32 BytesToRead = Length % (U32_MAX+1ULL);
      b08 Success = Win32_ReadFile(*FileHandle, Dest, BytesToRead, &BytesRead, &Overlapped);
      
      if(!Success) {
         u32 DEBUG_Err = Win32_GetLastError();
         Assert(Success, "Failed to read data from file");
         return 0;
      }
      
      TotalBytesRead += BytesRead;
      Length -= BytesRead;
      *OverlappedOffset += BytesRead;
   }
   
   return TotalBytesRead;
}

internal u64
Platform_WriteFile(file_handle *FileHandle,
                   vptr Src,
                   u64 Length,
                   u64 Offset)
{
   win32_overlapped Overlapped = {0};
   u64 *OverlappedOffset = (u64*)&Overlapped.Offset;
   *OverlappedOffset = Offset;
   
   u32 TotalBytesWritten = 0;
   while(Length) {
      u32 BytesWritten;
      u32 BytesToWrite = Length % (U32_MAX+1ULL);
      b08 Success = Win32_WriteFile(*FileHandle, Src, BytesToWrite, &BytesWritten, &Overlapped);
      
      if(!Success) {
         u32 DEBUG_Err = Win32_GetLastError();
         Assert(Success, "Failed to write data to file");
         return 0;
      }
      
      TotalBytesWritten += BytesWritten;
      Length -= BytesWritten;
      *OverlappedOffset += BytesWritten;
   }
   
   return TotalBytesWritten;
}

internal void
Platform_WriteConsole(string Message)
{
   win32_handle Handle = Win32_GetStdHandle(STD_OUTPUT_HANDLE);
   Win32_WriteConsoleA(Handle, Message.Text, Message.Length, NULL, NULL);
   
   c08 *StringCopy = Platform_AllocateMemory(Message.Length+1);
   Mem_Cpy(StringCopy, Message.Text, Message.Length);
   StringCopy[Message.Length] = 0;
   Win32_OutputDebugStringA(StringCopy);
   Platform_FreeMemory(StringCopy);
}

internal void
Platform_WriteError(string Message, u32 Exit)
{
   win32_handle Handle = Win32_GetStdHandle(STD_ERROR_HANDLE);
   Win32_WriteConsoleA(Handle, Message.Text, Message.Length, NULL, NULL);
   
   c08 *StringCopy = Platform_AllocateMemory(Message.Length+1);
   Mem_Cpy(StringCopy, Message.Text, Message.Length);
   StringCopy[Message.Length] = 0;
   Win32_OutputDebugStringA(StringCopy);
   Platform_FreeMemory(StringCopy);
   
   if(Exit) Platform_Exit(Exit);
}

internal void
Platform_CloseFile(file_handle *FileHandle)
{
   Win32_CloseHandle(*FileHandle);
}

internal void
Platform_GetFileTime(c08 *FileName,
                     datetime *CreationTime,
                     datetime *LastAccessTime,
                     datetime *LastWriteTime)
{
   file_handle FileHandle;
   win32_file_time _Times[3];
   datetime *Times[3] = {CreationTime, LastAccessTime, LastWriteTime};
   Platform_OpenFile(&FileHandle, FileName, FILE_READ);
   Win32_GetFileTime(FileHandle, _Times+0, _Times+1, _Times+2);
   Platform_CloseFile(FileHandle);
   
   win32_system_time SystemTime;
   for(u32 I = 0; I < 3; I++) {
      if(!Times[I]) continue;
      Win32_FileTimeToSystemTime(_Times+I, &SystemTime);
      Times[I]->Year        = SystemTime.Year;
      Times[I]->Month       = SystemTime.Month;
      Times[I]->Day         = SystemTime.Day;
      Times[I]->DayOfWeek   = SystemTime.DayOfWeek;
      Times[I]->Hour        = SystemTime.Hour;
      Times[I]->Minute      = SystemTime.Minute;
      Times[I]->Second      = SystemTime.Second;
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
Platform_CmpFileTime(datetime A,
                     datetime B)
{
   u16 AVals[] = {A.Year, A.Month, A.Day, A.Hour, A.Minute, A.Second, A.Millisecond};
   u16 BVals[] = {B.Year, B.Month, B.Day, B.Hour, B.Minute, B.Second, B.Millisecond};
   u32 Count = sizeof(AVals)/sizeof(AVals[0]);
   for(u32 I = 0; I < Count; I++) {
      if(AVals[I] < BVals[I]) return LESS;
      if(AVals[I] > BVals[I]) return GREATER;
   }
   return EQUAL;
}

internal b08
Platform_ReloadModule(platform_module *Module)
{
   datetime LastWriteTime;
   Platform_GetFileTime(Module->FileName, 0, 0, &LastWriteTime);
   if(Module->DLL) {
      if(Platform_CmpFileTime(Module->LastWriteTime, LastWriteTime) != LESS)
         return FALSE;
      
      Module->Unload(Platform);
      Win32_FreeLibrary(Module->DLL);
   }
   Module->LastWriteTime = LastWriteTime;
   
   Module->DLL = Win32_LoadLibraryA(Module->FileName);
   
   Module->Load   = (func_Module_Load  *)Win32_GetProcAddress(Module->DLL, "Load");
   Module->Init   = (func_Module_Init  *)Win32_GetProcAddress(Module->DLL, "Init");
   Module->Update = (func_Module_Update*)Win32_GetProcAddress(Module->DLL, "Update");
   Module->Unload = (func_Module_Unload*)Win32_GetProcAddress(Module->DLL, "Unload");
   
   if(!Module->Load)   Module->Load   = (vptr)Platform_Stub;
   if(!Module->Init)   Module->Init   = (vptr)Platform_Stub;
   if(!Module->Update) Module->Update = (vptr)Platform_Stub;
   if(!Module->Unload) Module->Unload = (vptr)Platform_Stub;
   
   Module->Load(Platform, Module);
   
   return TRUE;
}

internal platform_module *
Platform_GetModule(c08 *Name)
{
   u32 Len = _Mem_BytesUntil(Name, 0);
   u32 S = 0;
   u32 E = Platform->ModuleCount;
   platform_module *Module;
   while(S < E) {
      u32 I = S + (E-S)/2;
      
      Module = Platform->Modules+I;
      s08 Cmp = _Mem_Cmp(Module->Name, Name, Len+1);
      
      if(Cmp == EQUAL) return Module;
      if(Cmp == LESS) S = I+1;
      else E = I;
   }
   
   return NULL;
}

internal platform_module *
Platform_LoadModule(c08 *Name)
{
   u32 NameLen = _Mem_BytesUntil(Name, 0);
   platform_module *Module = Platform_GetModule(Name);
   if (Module) return Module;
   
   u32 DesiredSize = (Platform->ModuleCount+1) * sizeof(platform_module);
   
   if(Platform->ModulesSize < DesiredSize) {
      vptr NewModules = Platform_AllocateMemory(DesiredSize*2);
      _Mem_Cpy(NewModules, (vptr)Platform->Modules, Platform->ModulesSize);
      Platform_FreeMemory(Platform->Modules);
      Platform->ModulesSize = DesiredSize*2;
      Platform->Modules = NewModules;
   }
   
   //TODO: Use the binary search to get an index
   platform_module *Next = Platform->Modules;
   u32 I;
   for(I = 0; I < Platform->ModuleCount; I++) {
      s08 Cmp = _Mem_Cmp(Next->Name, Name, NameLen+1);
      if(Cmp == LESS) {
         Next++;
         continue;
      }
      Assert(Cmp == GREATER, "Encountered a module that wasn't returned by Platform_GetModule");
      break;
   }
   
   u32 Delta = (Platform->ModuleCount-I)*sizeof(platform_module);
   _Mem_CpyRev((vptr)(Next+1), (vptr)Next, Delta);
   
   Module = Next;
   Platform->ModuleCount++;
   
   c08 *Path = Platform_AllocateMemory(NameLen+5);
   _Mem_Cpy(Path, Name, NameLen);
   Path[NameLen+0] = '.';
   Path[NameLen+1] = 'd';
   Path[NameLen+2] = 'l';
   Path[NameLen+3] = 'l';
   Path[NameLen+4] = '\0';
   
   _Mem_Set((vptr)Module, 0, sizeof(platform_module));
   Module->Name = Name;
   Module->FileName = Path;
   
   Platform_ReloadModule(Module);
   
   return Module;
}

internal void
Platform_LoadUtilFuncs(platform_module *UtilModule)
{
   util_funcs Funcs = *(util_funcs*)UtilModule->Funcs;
   
   #define EXPORT(R, N, ...) N = Funcs.N;
   #define X UTIL_FUNCS
   #include <x.h>
}

internal void
Platform_HideCursor(win32_window Window)
{
   Win32_SetCursor(NULL);
   
   Win32_GetCursorPos(&Platform->RestoreCursorPos);
   Win32_ScreenToClient(Window, &Platform->RestoreCursorPos);
   
   win32_rect ClipRect;
   Win32_GetClientRect(Window, &ClipRect);
   Win32_ClientToScreen(Window, (v2s32*)&ClipRect.Left);
   Win32_ClientToScreen(Window, (v2s32*)&ClipRect.Right);
   Win32_ClipCursor(&ClipRect);
}

internal void
Platform_ShowCursor(win32_window Window)
{
   Win32_ClipCursor(NULL);
   
   Win32_ClientToScreen(Window, &Platform->RestoreCursorPos);
   Win32_SetCursorPos(Platform->RestoreCursorPos.X, Platform->RestoreCursorPos.Y);
   
   Win32_SetCursor(Win32_LoadCursorA(NULL, IDC_ARROW));
}

internal s64
Platform_WindowCallback(win32_window Window,
                        u32 Message,
                        s64 WParam,
                        s64 LParam)
{
   switch(Message) {
      case WM_DESTROY:
      case WM_CLOSE: {
         Platform->ExecutionState = EXECUTION_ENDED;
      } return 0;
      
      case WM_SIZE: {
         Platform->WindowSize.X = (s16)LParam;
         Platform->WindowSize.Y = (s16)(LParam >> 16);
         Platform->Updates |= WINDOW_RESIZED;
      } return 0;
      
      case WM_SETFOCUS: {
         if(Platform->FocusState == FOCUS_NONE) {
            if(Platform->CursorIsDisabled)
               Platform_HideCursor(Window);
            
            Mem_Set(Platform->Keys, 0, sizeof(Platform->Keys));
            Mem_Set(Platform->Buttons, 0, sizeof(Platform->Buttons));
            
            Platform->FocusState = FOCUS_CLIENT;
         }
      } return 0;
      
      case WM_KILLFOCUS: {
         // if(Platform->CursorIsDisabled)
         //     Platform_ShowCursor(Window);
         
         Platform->FocusState = FOCUS_NONE;
      } return 0;
      
      case WM_MOUSEACTIVATE: {
         if(((LParam>>16)&0xFFFF) == WM_LBUTTONDOWN) {
            if((LParam&0xFFFF) != HTCLIENT) {
               Platform->FocusState = FOCUS_FRAME;
            }
         }
      } return MA_ACTIVATEANDEAT;
      
      case WM_INPUT: {
         Stack_Push();
         
         u32 Size;
         win32_raw_input_handle Handle = (vptr)LParam;
         u32 Res = Win32_GetRawInputData(Handle, RID_INPUT, NULL, &Size, sizeof(win32_raw_input_header));
         Assert(Res == 0);
         win32_raw_input *Data = Stack_Allocate(sizeof(win32_raw_mouse));
         Res = Win32_GetRawInputData(Handle, RID_INPUT, Data, &Size, sizeof(win32_raw_input_header));
         Assert(Res == sizeof(win32_raw_input));
         
         if(Data->Mouse.Flags & MOUSE_MOVE_ABSOLUTE) {
            Assert(FALSE, "Absolute mouse movement not supported.");
         } else {
            Platform->CursorPos = (v2s32){
               Platform->CursorPos.X + Data->Mouse.LastX,
               Platform->CursorPos.Y - Data->Mouse.LastY
            };
         }
         
         // Automatically goes through RI_MOUSE_BUTTON_1_DOWN/UP,
         // RI_MOUSE_BUTTON_2_DOWN/UP, etc.
         for(u32 I = 0; I < 6; I++) {
            if(Data->Mouse.ButtonFlags & (1<<(2*I)))
               Platform->Buttons[I] = PRESSED;
            else if(Data->Mouse.ButtonFlags & (2<<(2*I)))
               Platform->Buttons[I] = RELEASED;
            else if(Platform->Buttons[I] == PRESSED)
               Platform->Buttons[I] = HELD;
         }
         
         Stack_Pop();
      } return 0;
      
      case WM_KEYUP:
      case WM_KEYDOWN:
      case WM_SYSKEYUP:
      case WM_SYSKEYDOWN: {
         u08 ScanCode    = (LParam >> 16) & 0xFF;
         b08 IsExtended  = (LParam >> 24) & 0x01;
         b08 WasDown     = (LParam >> 30) & 0x01;
         b08 IsUp        = (LParam >> 31) & 0x01;
         
         key_state KeyState;
         if(IsUp == TRUE) KeyState = RELEASED;
         else if(WasDown == FALSE) KeyState = PRESSED;
         else KeyState = HELD;
         
         if(IsExtended) {
            // Exclude NumLock
            if(ScanCode != 0x45)
               ScanCode |= 0x80;
         } else {
            // Pause key
            if(ScanCode == 0x45)
               ScanCode = 0xFF;
            // Alt + PrintScreen
            else if(ScanCode == 0x54)
               ScanCode = 0x80 | 0x37;
         }
         
         if(IsUp == TRUE) KeyState = RELEASED;
         else if(Platform->Keys[ScanCode] == RELEASED) KeyState = PRESSED;
         // else if(Platform->Keys) KeyState = PRESSED;
         
         Platform->Keys[ScanCode] = KeyState;
      } return 0;
   }
   
   return Win32_DefWindowProcA(Window, Message, WParam, LParam);
}

internal void
Platform_ParseCommandLine(void)
{
   c16 *CmdLine = Win32_GetCommandLineW();
   c16 **Args = Win32_CommandLineToArgvW(CmdLine, &Platform->ArgCount);
   u32 *Sizes = Platform_AllocateMemory(Platform->ArgCount * sizeof(u32));
   u32 Size = Platform->ArgCount * sizeof(string*);
   for(u32 I = 0; I < Platform->ArgCount; I++) {
      Sizes[I] = Win32_WideCharToMultiByte(CP_UTF8, 0, Args[I], -1, 0, 0, 0, 0);
      Size += Sizes[I];
   }
   Platform->Args = Platform_AllocateMemory(Size);
   c08 *Strings = (c08*)(Platform->Args+Platform->ArgCount);
   for(u32 I = 0; I < Platform->ArgCount; I++) {
      Platform->Args[I] = Strings;
      u32 S = Win32_WideCharToMultiByte(
         CP_UTF8, 0, Args[I], -1, Platform->Args[I], Sizes[I], 0, 0);
      Assert(S == Sizes[I], "Buffer overflow while converting command line arguments");
      Strings += Sizes[I];
   }
   
   Win32_LocalFree(Args);
   Platform_FreeMemory(Sizes);
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
   platform_state _P = {0};
   Platform = &_P;
   
   #define EXPORT(ReturnType, Namespace, Name, ...) \
      Platform->Functions.Namespace##_##Name = Namespace##_##Name;
   #define X PLATFORM_FUNCS
   #include <x.h>
   
   Platform_LoadWin32();
	Platform_ParseCommandLine();
	Win32_QueryPerformanceFrequency(&CounterFrequency);
   Platform_LoadUtilFuncs(Platform_LoadModule("util"));
   
   u32 Size = 16*1024*1024;
   vptr Mem = Platform_AllocateMemory(Size);
   Stack_Init(Mem, Size);
   Stack_Push();
   
   Platform_LoadModule("base");
   
   for(u32 I = 0; I < Platform->ModuleCount; I++) {
      stack Stack = Stack_Get();
      Platform->Modules[I].Init(Platform);
      Stack_Set(Stack);
   }
   
   s64 CountsPerSecond;
   Win32_QueryPerformanceFrequency(&CountsPerSecond);
   
   s64 StartTime;
   Win32_QueryPerformanceCounter(&StartTime);
   
   while(Platform->ExecutionState == EXECUTION_RUNNING) {
      // Will reload the modules if necessary
      for(u32 I = 0; I < Platform->ModuleCount; I++) {
         platform_module *Module = Platform->Modules+I;
         b08 Reloaded = Platform_ReloadModule(Module);
         
         if(Reloaded && _Mem_Cmp(Module->Name, "util", 5) == EQUAL)
            Platform_LoadUtilFuncs(Module);
      }
      
      win32_message Message;
      while(Win32_PeekMessageA(&Message, PrimaryWindow, 0, 0, PM_REMOVE)) {
         if(Message.Message == WM_QUIT) {
            Platform->ExecutionState = EXECUTION_ENDED;
            break;
         }
         
         Win32_TranslateMessage(&Message);
         Win32_DispatchMessageA(&Message);
      }
      
      if(Platform->WindowedApp) {
         if(Platform->CursorIsDisabled) {
            // HACK: Not sure why, but the cursor keeps reappearing
            // otherwise
            Win32_SetCursor(NULL);
         }
         
         if(Platform->FocusState == FOCUS_CLIENT && !Platform->CursorIsDisabled && Platform->Buttons[Button_Left] == PRESSED) {
            Platform_HideCursor(PrimaryWindow);
            Platform->Updates |= CURSOR_DISABLED;
            Platform->CursorIsDisabled = TRUE;
         }
         if(Platform->CursorIsDisabled && Platform->Keys[ScanCode_Escape] == PRESSED) {
            Platform_ShowCursor(PrimaryWindow);
            Platform->CursorIsDisabled = FALSE;
         }
      }
      
      for(u32 I = 0; I < Platform->ModuleCount; I++) {
         stack Stack = Stack_Get();
         Platform->Modules[I].Update(Platform);
         Stack_Set(Stack);
      }
      
      if(Platform->WindowedApp)
         Win32_SwapBuffers(DeviceContext);
      
      s64 EndTime;
      Win32_QueryPerformanceCounter(&EndTime);
      
      s64 ElapsedTime = EndTime - StartTime;
      StartTime = EndTime;
      
      Platform->FPS = CountsPerSecond / (r64)ElapsedTime;
   }
   
   for(u32 I = 0; I < Platform->ModuleCount; I++) {
      vptr Cursor = Stack_GetCursor();
      Platform->Modules[I].Unload(Platform);
      Stack_SetCursor(Cursor);
   }
   
   Stack_Pop();
   
   Platform_Exit(0);
}