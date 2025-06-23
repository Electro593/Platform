/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
**                                                                         **
**  Author: Aria Seiler                                                    **
**                                                                         **
**  This program is in the public domain. There is no implied warranty,    **
**  so use it at your own risk.                                            **
**                                                                         **
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef INCLUDE_HEADER

#define WAYLAND_USER_FUNCS \
	EXPORT(b08, Wayland_CreateWindow, c08 *Title, usize Width, usize Height)

#endif

#ifdef INCLUDE_SOURCE

#define CALL(Object, Func, ...) (Object)->Func((Object) __VA_OPT__(,) __VA_ARGS__)

internal void
Wayland_Display_HandleError(
	wayland_display		 *This,
	wayland_interface	 *Object,
	wayland_display_error Code,
	string				  Message
)
{
	STOP;
}

internal void
Wayland_Display_HandleDeleteId(wayland_display *This, u32 Id)
{
	STOP;
}

internal void
Wayland_Registry_HandleGlobal(wayland_registry *This, u32 Name, string Interface, u32 Version)
{ }

internal void
Wayland_Registry_HandleGlobalRemove(wayland_registry *This, u32 Name)
{
	STOP;
}

internal void
Wayland_SyncAndHandleEvents(void)
{
	wayland_callback *SyncCallback = CALL(_G.Display, Sync);

	while (TRUE) {
		vptr Object = Wayland_HandleNextEvent();
		if (Object == SyncCallback) break;
	}

	Wayland_DeleteObject(SyncCallback);
}

internal b08
Wayland_TryInit(void)
{
	if (_G.Connected) return TRUE;
	if (_G.Attempted) return FALSE;

	_G.Attempted = TRUE;
	_G.Connected = Wayland_ConnectToServerSocket(&_G.Socket);

	if (_G.Connected) {
		_G.HeapSize = 16 * 1024 * 1024;
		_G.Heap		= Heap_Init(Platform_AllocateMemory(_G.HeapSize), _G.HeapSize);

		_G.IdTable = HashMap_Init(_G.Heap, sizeof(u32), sizeof(wayland_interface *));

		_G.NextObjectId			   = 1;
		_G.Display				   = Wayland_CreateObject(WAYLAND_OBJECT_TYPE_DISPLAY);
		_G.Display->HandleError	   = Wayland_Display_HandleError;
		_G.Display->HandleDeleteId = Wayland_Display_HandleDeleteId;

		_G.Registry						= CALL(_G.Display, GetRegistry);
		_G.Registry->HandleGlobal		= Wayland_Registry_HandleGlobal;
		_G.Registry->HandleGlobalRemove = Wayland_Registry_HandleGlobalRemove;

		Wayland_SyncAndHandleEvents();
	}

	STOP;
	return _G.Connected;
}

internal void
Wayland_TryClose(void)
{
	if (!_G.Connected) return;

	Platform_FreeMemory((vptr) _G.Heap, _G.HeapSize);
	Platform_CloseFile(_G.Socket);
}

#define INIT if (!Wayland_TryInit()) return FALSE;

internal b08
Wayland_CreateWindow(c08 *Title, usize Width, usize Height)
{
	INIT;
	return FALSE;
}

#undef INIT
#undef CALL

#endif
