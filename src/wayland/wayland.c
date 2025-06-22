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

internal b08
Wayland_TryInit(void)
{
	if (_G.Connected) return TRUE;
	if (_G.Attempted) return FALSE;

	_G.Attempted = TRUE;
	_G.Connected = Wayland_ConnectToServerSocket(&_G.Socket);

	if (_G.Connected) {
		usize HeapSize	= 16 * 1024 * 1024;
		_G.Heap			= Heap_Init(Platform_AllocateMemory(HeapSize), HeapSize);
		_G.NextObjectId = 2;

		_G.Registry = Wayland_Display_GetRegistry(&WaylandDisplay);

		wayland_callback SyncCallback = Wayland_Display_Sync(&WaylandDisplay);

		while (TRUE) {
			wayland_message *Message = Wayland_ReadMessage();

			if (Message->ObjectId == SyncCallback.Header.Id) {
				STOP;
				break;
			}

			Wayland_FreeMessage(Message);
		}
	}

	return _G.Connected;
}

internal void
Wayland_TryClose(void)
{
	if (!_G.Connected) return;

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

#endif
