/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                             *
*  Author: Aria Seiler                                                        *
*                                                                             *
*  This program is in the public domain. There is no implied warranty, so     *
*  use it at your own risk.                                                   *
*                                                                             *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef INCLUDE_HEADER

typedef struct wayland_state {
	usize HeapSize;
	heap *Heap;

	b08 Connected : 1;
	b08 Attempted : 1;

	wayland_display				*Display;
	wayland_registry			*Registry;
	wayland_compositor			*Compositor;
	wayland_shared_memory		*SharedMemory;
	wayland_data_device_manager *DataDeviceManager;
	wayland_data_device_manager *Subcompositor;
	wayland_fixes				*Fixes;

	hashmap Seats;
	hashmap Outputs;
} wayland_state;

#define WAYLAND_USER_FUNCS \
	EXPORT(wayland_surface *, Wayland_CreateOpenGLWindow, c08 *Title, usize Width, usize Height)

#endif

#ifdef _LINUX
#ifdef INCLUDE_SOURCE

#define CALL(Object, Func, ...) (Object)->Func((Object) __VA_OPT__(,) __VA_ARGS__)

internal void
Wayland_Display_HandleError(
	wayland_display	  *This,
	wayland_interface *Object,
	u32				   Code,
	string			   Message
)
{
	STOP;
	Platform_WriteError(
		FStringL(
			"[WAYLAND] An error occurred in a %s v%d with id %d: Error %d: "
			"%s\n",
			WaylandNames[Object->Type],
			Object->Version,
			Object->Id,
			Code,
			Message
		),
		0
	);
}

internal void
Wayland_Display_HandleDeleteId(wayland_display *This, u32 Id)
{
	STOP;
}

internal void
Wayland_Registry_HandleGlobal(
	wayland_registry *This,
	u32				  Name,
	string			  Interface,
	u32				  Version
)
{
	FPrintL("Notified of %s v%d\n", Interface, Version);

#define MAYBE_BIND(NAME, FIELD, TYPE)                                                        \
	if (String_Cmp(Interface, CStringL("wl_" #NAME)) == 0) {                                 \
		if (!_G.Wayland.FIELD)                                                               \
			_G.Wayland.FIELD = CALL(This, Bind, Name, WAYLAND_OBJECT_TYPE_##TYPE, Version);  \
	} else

	MAYBE_BIND(compositor, Compositor, COMPOSITOR) { }

#undef MAYBE_BIND
}

internal void
Wayland_Registry_HandleGlobalRemove(wayland_registry *This, u32 Name)
{
#define MAYBE_DELETE(FIELD)                                         \
	if (_G.Wayland.FIELD && Name == _G.Wayland.FIELD->Header.Name)  \
		Wayland_DeleteObject(_G.Wayland.FIELD);                     \
	else

	MAYBE_DELETE(Compositor)
	MAYBE_DELETE(SharedMemory)
	MAYBE_DELETE(DataDeviceManager)
	MAYBE_DELETE(Fixes) { }

#undef MAYBE_DELETE
}

internal void
Wayland_SyncAndHandleEvents(void)
{
	wayland_callback *SyncCallback = CALL(_G.Wayland.Display, Sync);

	while (TRUE) {
		vptr Object = Wayland_HandleNextEvent();
		if (Object == SyncCallback) break;
	}

	Wayland_DeleteObject(SyncCallback);
}

internal b08
Wayland_TryInit(void)
{
	if (_G.Wayland.Connected) return TRUE;
	if (_G.Wayland.Attempted) return FALSE;

	_G.Wayland.HeapSize = 16 * 1024 * 1024;
	_G.Wayland.Heap		= Heap_Init(
		Platform_AllocateMemory(_G.Wayland.HeapSize),
		_G.Wayland.HeapSize
	);

	_G.Wayland.Attempted = TRUE;
	_G.Wayland.Connected = Wayland_InitApi(_G.Wayland.Heap);

	if (_G.Wayland.Connected) {
		_G.Wayland.Display				   = Wayland_GetDisplay();
		_G.Wayland.Display->HandleError	   = Wayland_Display_HandleError;
		_G.Wayland.Display->HandleDeleteId = Wayland_Display_HandleDeleteId;

		_G.Wayland.Registry = CALL(_G.Wayland.Display, GetRegistry);
		_G.Wayland.Registry->HandleGlobal = Wayland_Registry_HandleGlobal;
		_G.Wayland.Registry->HandleGlobalRemove =
			Wayland_Registry_HandleGlobalRemove;

		Wayland_SyncAndHandleEvents();
	} else {
		Platform_FreeMemory(_G.Wayland.Heap, _G.Wayland.HeapSize);
	}

	return _G.Wayland.Connected;
}

#define INIT if (!Wayland_TryInit()) return FALSE;

internal wayland_surface *
Wayland_CreateOpenGLWindow(c08 *Title, usize Width, usize Height)
{
	INIT;

	wayland_surface *Surface = CALL(_G.Wayland.Compositor, CreateSurface);

	return Surface;
}

#undef INIT
#undef CALL

#endif
#endif
