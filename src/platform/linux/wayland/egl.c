/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                            *
*  Author: Aria Seiler                                                       *
*                                                                            *
*  This program is in the public domain. There is no implied warranty, so    *
*  use it at your own risk.                                                  *
*                                                                            *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef INCLUDE_HEADER

typedef s32	 egl_int;
typedef u32	 egl_boolean;
typedef vptr egl_display;

typedef wl_display *egl_native_display_type;

typedef struct wl_proxy wl_proxy;
typedef struct wl_display wl_display;

struct wl_proxy {
	struct wl_object object;
	wl_display *display;
	void *user_data;
};

struct wl_display {
	wl_proxy proxy;
	struct wl_connection *connection;
	int fd;
	uint32_t mask;
	struct wl_map objects;
	struct wl_list global_listener_list;
	struct wl_list global_list;

	wl_display_update_func_t update;
	void *update_data;

	wl_display_global_func_t global_handler;
	void *global_handler_data;
};

#define EGL_FUNCS \
	INTERN(egl_display, Egl_Initialize, wayland_display *Display) \
	IMPORT(egl_display, egl, Egl_, GetDisplay, egl_native_display_type Display) \
	IMPORT(egl_boolean, egl, Egl_, Initialize, egl_display Display, egl_int *Major, egl_int *Minor) \
	//

#define INTERN(R, N, ...) internal R (*N)(__VA_ARGS__);
#define X EGL_FUNCS
#include <x.h>

#endif

#ifdef INCLUDE_SOURCE

#define IMPORT(R, LP, P, N, ...) global R (*P##N)(__VA_ARGS__);
#define X EGL_FUNCS
#include <x.h>

internal wl_display
Egl_DuckTypeDisplay(wayland_display *Display)
{

}

internal egl_display
Egl_Initialize(wl_display *Display)
{
	vptr Module = dlopen("/usr/lib/libEGL.so", SYS_RUNTIME_LOADER_LAZY);

#define IMPORT(R, LP, P, N, ...) Platform_GetProcAddress(Module, #LP #N, &P##N);
#define X EGL_FUNCS
#include <x.h>

	egl_display EglDisplay = Egl_GetDisplay(Display);

	egl_int Major, Minor;
	if (!Egl_Initialize(EglDisplay, &Major, &Minor)) {
		FPrintL("Failed to initialize egl\n");
		return NULL;
	}
	FPrintL("Initialized egl version %d.%d\n");

	return EglDisplay;
}

#endif
