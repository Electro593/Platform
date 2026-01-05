/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                            *
*  Author: Aria Seiler                                                       *
*                                                                            *
*  This program is in the public domain. There is no implied warranty, so    *
*  use it at your own risk.                                                  *
*                                                                            *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef INCLUDE_HEADER

typedef unsigned int egl_boolean;
typedef void		*egl_display;
typedef void		*egl_surface;

typedef enum egl_platform { EGL_PLATFORM_WAYLAND_KHR = 0x31D8 } egl_platform;

#define EGL_FUNCS \
	IMPORT(egl_display, egl, EGL_, GetDisplay,  vptr NativeDisplay, egl_attrib *AttribList) \
	IMPORT(egl_boolean, egl, EGL_, SwapBuffers, egl_display Display, egl_surface Surface)

#define IMPORT(R, N, ...) global R (*EGL_##N)(__VA_ARGS__);
#define X EGL_FUNCS
#include <x.h>

#endif

#ifdef INCLUDE_SOURCE

internal void
EGL_LoadFuncs()
{
	if (!_G.EGLHandle)
		_G.EGLHandle = dlopen("/usr/lib/libEGL.so", SYS_RUNTIME_LOADER_LAZY);

#define IMPORT(R, LP, P, N, ...) P##N = dlsym(_G.EGLHandle, #LP #N);
#define X EGL_FUNCS
#include <x.h>
}

#endif
