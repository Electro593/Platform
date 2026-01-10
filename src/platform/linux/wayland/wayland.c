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
	thread_handle EventThread;

	u32 SyncLock;

	wayland_display		*Display;
	wayland_registry	*Registry;
	wayland_compositor	*Compositor;
	wayland_shm			*Shm;
	wayland_xdg_wm_base *XdgWmBase;

	s32						 PreferredBufferScale;
	wayland_output_transform PreferredBufferTransform;
	v2s32					 MaxWindowSize;
	v2s32					 PreferredWindowSize;

	hashmap Seats;
	hashmap Outputs;
} wayland_state;

#define WAYLAND_USER_FUNCS \
	EXPORT(b08,               Wayland_TryInit,            void) \
	EXPORT(wayland_surface *, Wayland_CreateOpenGLWindow, c08 *Title, usize Width, usize Height) \
	//

#endif

#ifdef _LINUX
#ifdef INCLUDE_SOURCE

#define CALL(Object, Func, ...) (Object)->Func((Object) __VA_OPT__(,) __VA_ARGS__)

internal void
Wayland_DebugLog(vptr Object, c08 *Format, ...)
{
	va_list Args;
	VA_Start(Args, Format);
	string Message = FVString(CString(Format), Args);
	VA_End(Args);

	wayland_interface *Interface = Object;
	FPrintL(
		"[Wayland][%s#%d] %s",
		WaylandNames[Interface->Type],
		Interface->Id,
		Message
	);
}

internal void
Wayland_Display_HandleError(
	wayland_display	  *This,
	wayland_interface *Object,
	u32				   Code,
	string			   Message
)
{
	Wayland_DebugLog(
		This,
		"An error occurred in a %s v%d with id %d: Error %d: %s\n",
		WaylandNames[Object->Type],
		Object->Version,
		Object->Id,
		Code,
		Message
	);
	Wayland_Disconnect();
}

internal void
Wayland_Display_HandleDeleteId(wayland_display *This, u32 Id)
{
	Wayland_DebugLog(This, "Requested to delete id %u\n", Id);
}

internal void
Wayland_Shm_HandleFormat(wayland_shm *This, wayland_shm_format Format)
{
	Wayland_DebugLog(This, "Offered pixel format %d\n", Format);
}

internal void
Wayland_Buffer_HandleRelease(wayland_buffer *This)
{
	Wayland_DebugLog(This, "Released by compositor\n");
}

internal void
Wayland_Surface_HandleEnter(wayland_surface *This, wayland_output *Output)
{
	Wayland_DebugLog(
		This,
		"Entering output object with id %d\n",
		Output->Header.Id
	);
}

internal void
Wayland_Surface_HandleLeave(wayland_surface *This, wayland_output *Output)
{
	Wayland_DebugLog(
		This,
		"Leaving output object with id %d\n",
		Output->Header.Id
	);
}

internal void
Wayland_Surface_HandlePreferredBufferScale(wayland_surface *This, s32 Factor)
{
	Wayland_DebugLog(This, "Requested scaling factor of %d\n", Factor);
	_G.Wayland.PreferredBufferScale = Factor;
}

internal void
Wayland_Surface_HandlePreferredBufferTransform(
	wayland_surface *This,
	u32				 Transform
)
{
	Wayland_DebugLog(This, "Requested buffer transform of %u\n", Transform);
	_G.Wayland.PreferredBufferTransform = Transform;
}

internal void
Wayland_XdgWmBase_HandlePing(wayland_xdg_wm_base *This, u32 Serial)
{
	Wayland_DebugLog(This, "Pinged! Ponging with %u\n", Serial);
	CALL(This, Pong, Serial);
}

internal void
Wayland_XdgSurface_HandleConfigure(wayland_xdg_surface *This, u32 Serial)
{
	Wayland_DebugLog(
		This,
		"Requested configure. Acknowledging with serial "
		"%u\n",
		Serial
	);
	CALL(This, AckConfigure, Serial);
}

internal void
Wayland_XdgToplevel_HandleConfigure(
	wayland_xdg_toplevel *This,
	s32					  Width,
	s32					  Height,
	wayland_array		  States
)
{
	Wayland_DebugLog(
		This,
		"Requested configure with dimensions %dx%d and "
		"states:",
		Width,
		Height
	);
	for (usize I = 0; I < States.Size / 4; I++)
		FPrintL("%s %d", CString(I ? "," : ""), States.Data[I]);
	FPrintL("\n");
}

internal void
Wayland_XdgToplevel_HandleClose(wayland_xdg_toplevel *This)
{
	Wayland_DebugLog(This, "Requested to close\n");
	Wayland_Disconnect();
}

internal void
Wayland_XdgToplevel_HandleConfigureBounds(
	wayland_xdg_toplevel *This,
	s32					  Width,
	s32					  Height
)
{
	Wayland_DebugLog(
		This,
		"Requested to constrain bounds to %dx%d\n",
		Width,
		Height
	);

	if (Width || Height) {
		_G.Wayland.MaxWindowSize.X = Width;
		_G.Wayland.MaxWindowSize.Y = Height;
	}
}

internal void
Wayland_XdgToplevel_HandleWmCapabilities(
	wayland_xdg_toplevel *This,
	wayland_array		  Capabilities
)
{
	Wayland_DebugLog(This, "Advertized window manager capabilities:");
	for (usize I = 0; I < Capabilities.Size / 4; I++)
		FPrintL("%s %d", CString(I ? "," : ""), Capabilities.Data[I]);
	FPrintL("\n");
}

internal void
Wayland_Registry_HandleGlobal(
	wayland_registry *This,
	u32				  Name,
	string			  Interface,
	u32				  Version
)
{
	Wayland_DebugLog(
		This,
		"Handling global %d, named %s v%d\n",
		Name,
		Interface,
		Version
	);

	if (!String_Cmp(Interface, WaylandNames[WAYLAND_OBJECT_TYPE_COMPOSITOR])) {
		_G.Wayland.Compositor =
			CALL(This, Bind, Name, WAYLAND_OBJECT_TYPE_COMPOSITOR, Version);
	} else if (!String_Cmp(Interface, WaylandNames[WAYLAND_OBJECT_TYPE_SHM])) {
		_G.Wayland.Shm =
			CALL(This, Bind, Name, WAYLAND_OBJECT_TYPE_SHM, Version);
		_G.Wayland.Shm->HandleFormat = Wayland_Shm_HandleFormat;
	} else if (!String_Cmp(
				   Interface,
				   WaylandNames[WAYLAND_OBJECT_TYPE_XDG_WM_BASE]
			   ))
	{
		_G.Wayland.XdgWmBase =
			CALL(This, Bind, Name, WAYLAND_OBJECT_TYPE_XDG_WM_BASE, Version);
		_G.Wayland.XdgWmBase->HandlePing = Wayland_XdgWmBase_HandlePing;
	}
}

internal void
Wayland_Registry_HandleGlobalRemove(wayland_registry *This, u32 Name)
{
	Wayland_DebugLog(This, "Requested to remove global %d\n", Name);

	if (_G.Wayland.Compositor && _G.Wayland.Compositor->Header.Name == Name)
		Wayland_DestroyObject(_G.Wayland.Compositor);
	else if (_G.Wayland.Shm && _G.Wayland.Shm->Header.Name == Name)
		Wayland_DestroyObject(_G.Wayland.Shm);
	else if (_G.Wayland.XdgWmBase && _G.Wayland.XdgWmBase->Header.Name == Name)
		Wayland_DestroyObject(_G.Wayland.XdgWmBase);
}

internal s32
Wayland_EventHandlerThread(vptr Data)
{
	while (Wayland_IsConnected()) {
		Platform_LockMutex(&_G.Wayland.SyncLock);
		wayland_message *Message = Wayland_ReadMessage(20);
		Wayland_HandleMessage(Message);
		Platform_UnlockMutex(&_G.Wayland.SyncLock);
	}
	return 0;
}

internal void
Wayland_SyncAndHandleEvents(void)
{
	s32 Tid = Sys_GetTid();
	FPrintL("[%d] Main thread\n", Tid);

	Platform_LockMutex(&_G.Wayland.SyncLock);

	wayland_callback *SyncCallback = CALL(_G.Wayland.Display, Sync);
	while (Wayland_IsConnected()) {
		wayland_message *Message = Wayland_ReadMessage(20);
		vptr			 Object	 = Wayland_HandleMessage(Message);
		if (Object == SyncCallback) break;
	}

	Platform_UnlockMutex(&_G.Wayland.SyncLock);
}

internal b08
Wayland_TryInit(void)
{
	if (Wayland_Connect()) {
		wayland_display *Display = Wayland_GetDisplay();
		Display->HandleError	 = Wayland_Display_HandleError;
		Display->HandleDeleteId	 = Wayland_Display_HandleDeleteId;
		_G.Wayland.Display		 = Display;

		wayland_registry *Registry	 = CALL(_G.Wayland.Display, GetRegistry);
		Registry->HandleGlobal		 = Wayland_Registry_HandleGlobal;
		Registry->HandleGlobalRemove = Wayland_Registry_HandleGlobalRemove;
		_G.Wayland.Registry			 = Registry;

		b08 Success = Platform_CreateThread(
			&_G.Wayland.EventThread,
			Wayland_EventHandlerThread,
			NULL
		);
		if (!Success) {
			Wayland_Disconnect();
			return FALSE;
		}

		Wayland_SyncAndHandleEvents();
	}

	return Wayland_IsConnected();
}

internal wayland_surface *
Wayland_CreateOpenGLWindow(c08 *Title, usize Width, usize Height)
{
	if (!Wayland_IsConnected()) return NULL;

	wayland_shm_pool	 *ShmPool	  = NULL;
	wayland_buffer		 *Buffer	  = NULL;
	wayland_surface		 *Surface	  = NULL;
	wayland_xdg_surface	 *XdgSurface  = NULL;
	wayland_xdg_toplevel *XdgToplevel = NULL;

	Surface = CALL(_G.Wayland.Compositor, CreateSurface);
	if (!Surface) goto failed;
	Surface->HandleEnter = Wayland_Surface_HandleEnter;
	Surface->HandleLeave = Wayland_Surface_HandleLeave;
	Surface->HandlePreferredBufferScale =
		Wayland_Surface_HandlePreferredBufferScale;
	Surface->HandlePreferredBufferTransform =
		Wayland_Surface_HandlePreferredBufferTransform;

	XdgSurface = CALL(_G.Wayland.XdgWmBase, GetXdgSurface, Surface);
	if (!XdgSurface) goto failed;
	XdgSurface->HandleConfigure = Wayland_XdgSurface_HandleConfigure;

	XdgToplevel = CALL(XdgSurface, GetToplevel);
	if (!XdgToplevel) goto failed;
	XdgToplevel->HandleConfigure = Wayland_XdgToplevel_HandleConfigure;
	XdgToplevel->HandleClose	 = Wayland_XdgToplevel_HandleClose;
	XdgToplevel->HandleConfigureBounds =
		Wayland_XdgToplevel_HandleConfigureBounds;
	XdgToplevel->HandleWmCapabilities =
		Wayland_XdgToplevel_HandleWmCapabilities;

	s32 ShmSize = 2 * 4 * Width * Height;
	s32 ShmFd	= Sys_MemfdCreate("wayland-shm", 0);
	if (ShmFd < 0) goto failed;
	if (Sys_FTruncate(ShmFd, ShmSize) < 0) goto failed;

	ShmPool = CALL(_G.Wayland.Shm, CreatePool, ShmFd, ShmSize);
	if (!ShmPool) goto failed;

	Buffer = CALL(
		ShmPool,
		CreateBuffer,
		0,
		Width,
		Height,
		Width * 4,
		WAYLAND_SHM_FORMAT_ARGB8888
	);
	if (!Buffer) goto failed;
	Buffer->HandleRelease = Wayland_Buffer_HandleRelease;

	CALL(XdgToplevel, SetTitle, CStringL("Voxarc"));
	CALL(XdgToplevel, SetAppId, CStringL("voxarc"));
	if (!CALL(Surface, Commit)) goto failed;

	Wayland_SyncAndHandleEvents();

	while (1);

	return Surface;

failed:
	if (Buffer->Header.Id) CALL(Buffer, Destroy);
	if (ShmPool->Header.Id) CALL(ShmPool, Destroy);
	if (XdgToplevel->Header.Id) CALL(XdgToplevel, Destroy);
	if (XdgSurface->Header.Id) CALL(XdgSurface, Destroy);
	if (Surface->Header.Id) CALL(Surface, Destroy);
	Sys_Close(ShmFd);
	return NULL;
}

#undef CALL

#endif
#endif
