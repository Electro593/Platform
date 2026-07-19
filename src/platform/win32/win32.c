/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                             *
*  Author: Aria Seiler                                                        *
*                                                                             *
*  This program is in the public domain. There is no implied warranty, so     *
*  use it at your own risk.                                                   *
*                                                                             *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define IMPORT(Module, ReturnType, Name, ...) static func_Win32_##Name *Win32_##Name;
#define X WIN32_FUNCS
#include <x.h>

#define IMPORT(ReturnType, Name, ...) static func_WGL_##Name *WGL_##Name;
#define X WGL_FUNCS_TYPE_1 WGL_FUNCS_TYPE_2
#include <x.h>
