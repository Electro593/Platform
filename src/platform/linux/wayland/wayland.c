/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                             *
*  Author: Aria Seiler                                                        *
*                                                                             *
*  This program is in the public domain. There is no implied warranty, so     *
*  use it at your own risk.                                                   *
*                                                                             *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef INCLUDE_HEADER

typedef struct wayland_window_state {
	b08 Initialized;

	wayland_surface		 *Surface;
	wayland_xdg_surface	 *XdgSurface;
	wayland_xdg_toplevel *XdgToplevel;

	s32 Width;
	s32 Height;

	s32	  ShmFd;
	usize BufferSize;
	vptr  BufferData[2];

	wayland_shm_pool *ShmPool;
	wayland_buffer	 *Buffers[2];

	wayland_callback *FrameCallback;
} wayland_window_state;

typedef struct wayland_state {
	// s32					   DrmFd;
	// b08					   DrmAuthenticated;
	// wayland_drm_capability DrmCapabilities;

	thread_handle EventThread;

	u32 SyncLock;

	wayland_display		*Display;
	wayland_registry	*Registry;
	wayland_compositor	*Compositor;
	wayland_shm			*Shm;
	wayland_xdg_wm_base *XdgWmBase;
	wayland_drm			*Drm;

	wayland_window_state Window;

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
		"[%d] [Wayland] [%s#%d] %s",
		Sys_GetTid(),
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

	Wayland_FreeObjectId(Id);
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

	wayland_window_state *Window = &_G.Wayland.Window;

	if (Window->Buffers[1] == This) {
		Window->Buffers[1] = Window->Buffers[0];
		Window->Buffers[0] = This;

		vptr BufferData		  = Window->BufferData[1];
		Window->BufferData[1] = Window->BufferData[0];
		Window->BufferData[0] = BufferData;
	}
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
Wayland_Drm_HandleDevice(wayland_drm *This, string Name)
{
	Wayland_DebugLog(This, "Using device name %s\n", Name);

	s32 Fd = Drm_OpenDriver(Name);
	if (Fd < 0) goto error;

	s32 NodeType = Drm_GetNodeType(Fd);
	if (NodeType < 0) goto error;

	if (NodeType == DRM_NODE_TYPE_RENDER) {
		_G.Wayland.DrmAuthenticated = TRUE;
	} else {
		u32 MagicNumber;
		s32 Result = Drm_GetMagicNumber(Fd, &MagicNumber);
		if (Result < 0) goto error;
		CALL(This, Authenticate, MagicNumber);
	}

	_G.Wayland.DrmFd = Fd;
	return;

error:
	if (Fd >= 0) Drm_CloseDriver(Fd);
}

internal void
Wayland_Drm_HandleFormat(wayland_drm *This, u32 Format)
{
	Wayland_DebugLog(This, "Can handle format %d\n", Format);
}

internal void
Wayland_Drm_HandleAuthenticated(wayland_drm *This)
{
	Wayland_DebugLog(This, "Authenticated!\n");

	_G.Wayland.DrmAuthenticated = TRUE;
}

internal void
Wayland_Drm_HandleCapabilities(
	wayland_drm			  *This,
	wayland_drm_capability Capabilities
)
{
	Wayland_DebugLog(This, "Capabilities: %d\n", Capabilities);

	_G.Wayland.DrmCapabilities = Capabilities;
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
	// } else if (!String_Cmp(Interface, WaylandNames[WAYLAND_OBJECT_TYPE_DRM])) {
	// 	_G.Wayland.Drm =
	// 		CALL(This, Bind, Name, WAYLAND_OBJECT_TYPE_DRM, Version);
	// 	_G.Wayland.Drm->HandleDevice		= Wayland_Drm_HandleDevice;
	// 	_G.Wayland.Drm->HandleFormat		= Wayland_Drm_HandleFormat;
	// 	_G.Wayland.Drm->HandleAuthenticated = Wayland_Drm_HandleAuthenticated;
	// 	_G.Wayland.Drm->HandleCapabilities	= Wayland_Drm_HandleCapabilities;
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
	// else if (_G.Wayland.Drm && _G.Wayland.Drm->Header.Name == Name)
	// 	Wayland_DestroyObject(_G.Wayland.Drm);
}

internal void
Wayland_HandleFrame(wayland_callback *This, u32 CallbackData)
{
	Wayland_DebugLog(This, "Notified of frame at time %d\n", CallbackData);

	wayland_window_state *Window = &_G.Wayland.Window;

	wayland_callback *FrameCallback = CALL(Window->Surface, Frame);
	FrameCallback->HandleDone		= Wayland_HandleFrame;
	Window->FrameCallback			= FrameCallback;

	u32 Stage = (CallbackData >> 12) & 7;

	u32 Value	 = (CallbackData >> 4) & 0xFF;
	u32 InvValue = 255 - Value;
	u32 Red = 0, Green = 0, Blue = 0;

	switch (Stage) {
		case 0:
			Red	  = Value;
			Green = 0;
			Blue  = 0;
			break;
		case 1:
			Red	  = 255;
			Green = Value;
			Blue  = 0;
			break;
		case 2:
			Red	  = InvValue;
			Green = 255;
			Blue  = 0;
			break;
		case 3:
			Red	  = 0;
			Green = 255;
			Blue  = Value;
			break;
		case 4:
			Red	  = 0;
			Green = InvValue;
			Blue  = 255;
			break;
		case 5:
			Red	  = Value;
			Green = 0;
			Blue  = 255;
			break;
		case 6:
			Red	  = 255;
			Green = Value;
			Blue  = 255;
			break;
		case 7:
			Red	  = InvValue;
			Green = InvValue;
			Blue  = InvValue;
			break;
	}

	u32 Color = (255 << 24) | (Red << 16) | (Green << 8) | Blue;

	u32 *Colors = Window->BufferData[0];
	for (usize Y = 0; Y < Window->Height; Y++)
		for (usize X = 0; X < Window->Width; X++)
			Colors[X + Y * Window->Width] = Color;

	CALL(Window->Surface, Attach, Window->Buffers[0], 0, 0);
	CALL(Window->Surface, DamageBuffer, 0, 0, Window->Width, Window->Height);

	CALL(Window->Surface, Commit);
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

	s32	  ShmFd		 = 0;
	usize BufferSize = 4 * Width * Height;
	vptr  BufferData = NULL;

	wayland_shm_pool	 *ShmPool	  = NULL;
	wayland_buffer		 *Buffer1	  = NULL;
	wayland_buffer		 *Buffer2	  = NULL;
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

	s32 ShmSize = 2 * BufferSize;
	ShmFd		= Sys_MemfdCreate("wayland-shm", 0);
	if (ShmFd < 0) goto failed;
	if (Sys_FTruncate(ShmFd, ShmSize) < 0) goto failed;
	BufferData = Sys_MemMap(
		NULL,
		ShmSize,
		SYS_PROT_READ | SYS_PROT_WRITE,
		SYS_MAP_SHARED | SYS_MAP_POPULATE,
		ShmFd,
		0
	);
	if ((ssize) BufferData < 0 && (ssize) BufferData > -4096) goto failed;

	ShmPool = CALL(_G.Wayland.Shm, CreatePool, ShmFd, ShmSize);
	if (!ShmPool) goto failed;

	Buffer1 = CALL(
		ShmPool,
		CreateBuffer,
		0,
		Width,
		Height,
		Width * 4,
		WAYLAND_SHM_FORMAT_ARGB8888
	);
	if (!Buffer1) goto failed;
	Buffer1->HandleRelease = Wayland_Buffer_HandleRelease;

	Buffer2 = CALL(
		ShmPool,
		CreateBuffer,
		BufferSize,
		Width,
		Height,
		Width * 4,
		WAYLAND_SHM_FORMAT_ARGB8888
	);
	if (!Buffer2) goto failed;
	Buffer2->HandleRelease = Wayland_Buffer_HandleRelease;

	CALL(XdgToplevel, SetTitle, CStringL("Voxarc"));
	CALL(XdgToplevel, SetAppId, CStringL("voxarc"));
	CALL(Surface, Commit);

	Mem_Set(BufferData, -1, ShmSize);

	wayland_window_state *Window = &_G.Wayland.Window;

	Window->Initialized	  = TRUE;
	Window->Surface		  = Surface;
	Window->XdgSurface	  = XdgSurface;
	Window->XdgToplevel	  = XdgToplevel;
	Window->ShmPool		  = ShmPool;
	Window->ShmFd		  = ShmFd;
	Window->Width		  = Width;
	Window->Height		  = Height;
	Window->BufferSize	  = BufferSize;
	Window->BufferData[0] = BufferData;
	Window->BufferData[1] = BufferData + BufferSize;
	Window->Buffers[0]	  = Buffer1;
	Window->Buffers[1]	  = Buffer2;

	// Wait for the configure events to pass
	Wayland_SyncAndHandleEvents();

	CALL(Surface, Attach, Buffer2, 0, 0);
	CALL(Surface, DamageBuffer, 0, 0, Width, Height);

	wayland_callback *FrameCallback = CALL(Surface, Frame);
	FrameCallback->HandleDone		= Wayland_HandleFrame;
	Window->FrameCallback			= FrameCallback;

	CALL(Surface, Commit);

	return Surface;

failed:
	if (Buffer1->Header.Id) CALL(Buffer1, Destroy);
	if (Buffer2->Header.Id) CALL(Buffer2, Destroy);
	if (ShmPool->Header.Id) CALL(ShmPool, Destroy);
	if (XdgToplevel->Header.Id) CALL(XdgToplevel, Destroy);
	if (XdgSurface->Header.Id) CALL(XdgSurface, Destroy);
	if (Surface->Header.Id) CALL(Surface, Destroy);
	if (BufferData) Sys_MemUnmap(BufferData, ShmSize);
	if (ShmFd) Sys_Close(ShmFd);
	return NULL;
}

#undef CALL

#endif
#endif
