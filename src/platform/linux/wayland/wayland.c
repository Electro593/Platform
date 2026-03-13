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
	s32					   DrmFd;
	b08					   DrmAuthenticated;
	wayland_drm_capability DrmCapabilities;
	drm_format			   DrmFormat;

	gbm Gbm;
	egl Egl;

	thread_handle EventThread;

	u32 SyncLock;

	wayland_display	 *Display;
	wayland_registry *Registry;

	wayland_compositor	*Compositor;
	u32					 CompositorName;
	wayland_shm			*Shm;
	u32					 ShmName;
	wayland_xdg_wm_base *XdgWmBase;
	u32					 XdgWmBaseName;
	wayland_drm			*Drm;
	u32					 DrmName;

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
	EXPORT(wayland_surface *, Wayland_CreateGLWindow, c08 *Title, usize Width, usize Height) \
	//

#endif

#ifdef _LINUX
#ifdef INCLUDE_SOURCE

internal void
Wayland_DebugLog(vptr Object, c08 *Format, ...)
{
	va_list Args;
	VA_Start(Args, Format);
	string Message = FVString(CString(Format), Args);
	VA_End(Args);

	if (Object) {
		wayland_interface *Interface = Object;
		FPrintL(
			"[%d] [Wayland] [%s#%d] %s",
			Sys_GetTid(),
			CString(Interface->Prototype->Name),
			Interface->Id,
			Message
		);
	} else {
		FPrintL("[%d] [Wayland] %s", Sys_GetTid(), Message);
	}
}

internal void
Wayland_Display_Error(
	wayland_display	  *This,
	wayland_interface *Object,
	u32				   Code,
	c08				  *Message
)
{
	Wayland_DebugLog(
		This,
		"An error occurred in %s: Error %d: %s\n",
		Wayland_GetObjectName(Object),
		Code,
		CString(Message)
	);
	Wayland_Disconnect();
}

internal void
Wayland_Display_DeleteId(wayland_display *This, u32 Id)
{
	Wayland_DebugLog(This, "Requested to delete id %u\n", Id);

	Wayland_FreeObjectId(Id);
}

internal void
Wayland_Shm_Format(wayland_shm *This, wayland_shm_format Format)
{
	Wayland_DebugLog(This, "Offered pixel format %d\n", Format);
}

internal void
Wayland_Buffer_Release(wayland_buffer *This)
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
Wayland_Surface_Enter(wayland_surface *This, wayland_output *Output)
{
	Wayland_DebugLog(
		This,
		"Entering output object with id %d\n",
		Output->Interface.Id
	);
}

internal void
Wayland_Surface_Leave(wayland_surface *This, wayland_output *Output)
{
	Wayland_DebugLog(
		This,
		"Leaving output object with id %d\n",
		Output->Interface.Id
	);
}

internal void
Wayland_Surface_PreferredBufferScale(wayland_surface *This, s32 Factor)
{
	Wayland_DebugLog(This, "Requested scaling factor of %d\n", Factor);

	_G.Wayland.PreferredBufferScale = Factor;
}

internal void
Wayland_Surface_PreferredBufferTransform(wayland_surface *This, u32 Transform)
{
	Wayland_DebugLog(This, "Requested buffer transform of %u\n", Transform);

	_G.Wayland.PreferredBufferTransform = Transform;
}

internal void
Wayland_XdgWmBase_Ping(wayland_xdg_wm_base *This, u32 Serial)
{
	Wayland_DebugLog(This, "Pinged! Ponging with %u\n", Serial);

	Wayland_XdgWmBase_Pong(This, Serial);
}

internal void
Wayland_XdgSurface_Configure(wayland_xdg_surface *This, u32 Serial)
{
	Wayland_DebugLog(
		This,
		"Requested configure. Acknowledging with serial "
		"%u\n",
		Serial
	);

	Wayland_XdgSurface_AckConfigure(This, Serial);
}

internal void
Wayland_XdgToplevel_Configure(
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
Wayland_XdgToplevel_Close(wayland_xdg_toplevel *This)
{
	Wayland_DebugLog(This, "Requested to close\n");

	Wayland_Disconnect();
}

internal void
Wayland_XdgToplevel_ConfigureBounds(
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
Wayland_XdgToplevel_WmCapabilities(
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
Wayland_Drm_Device(wayland_drm *This, c08 *Name)
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
		Wayland_Drm_Authenticate(This, MagicNumber);
	}

	_G.Wayland.DrmFd = Fd;
	return;

error:
	if (Fd >= 0) Drm_CloseDriver(Fd);
}

internal void
Wayland_Drm_Format(wayland_drm *This, u32 Format)
{
	Wayland_DebugLog(This, "Can handle format %d\n", Format);

	if (Format == DRM_FORMAT_XRGB8888) {
		Wayland_DebugLog(This, "Selected pixel format!\n");
		_G.Wayland.DrmFormat = Format;
	}
}

internal void
Wayland_Drm_Authenticated(wayland_drm *This)
{
	Wayland_DebugLog(This, "Authenticated!\n");
	_G.Wayland.DrmAuthenticated = TRUE;
}

internal void
Wayland_Drm_Capabilities(wayland_drm *This, wayland_drm_capability Capabilities)
{
	Wayland_DebugLog(This, "Capabilities: %d\n", Capabilities);

	_G.Wayland.DrmCapabilities = Capabilities;
}

internal void
Wayland_Registry_Global(
	wayland_registry *This,
	u32				  Name,
	c08				 *Interface,
	u32				  Version
)
{
	Wayland_DebugLog(
		This,
		"Handling global %d, named %s v%d\n",
		Name,
		CString(Interface),
		Version
	);

	string Str = CString(Interface);
	if (String_Cmp(Str, CStringL("wl_compositor")) == 0) {
		_G.Wayland.CompositorName = Name;
		_G.Wayland.Compositor	  = (wayland_compositor *)
			Wayland_Registry_Bind(This, Name, Interface, Version);
	} else if (String_Cmp(Str, CStringL("wl_shm")) == 0) {
		_G.Wayland.ShmName = Name;
		_G.Wayland.Shm	   = (wayland_shm *)
			Wayland_Registry_Bind(This, Name, Interface, Version);
		_G.Wayland.Shm->Format = Wayland_Shm_Format;
	} else if (String_Cmp(Str, CStringL("xdg_wm_base")) == 0) {
		_G.Wayland.XdgWmBaseName = Name;
		_G.Wayland.XdgWmBase	 = (wayland_xdg_wm_base *)
			Wayland_Registry_Bind(This, Name, Interface, Version);
		_G.Wayland.XdgWmBase->Ping = Wayland_XdgWmBase_Ping;
	} else if (String_Cmp(Str, CStringL("wl_drm")) == 0) {
		_G.Wayland.DrmName = Name;
		_G.Wayland.Drm	   = (wayland_drm *)
			Wayland_Registry_Bind(This, Name, Interface, Version);
		_G.Wayland.Drm->Device		  = Wayland_Drm_Device;
		_G.Wayland.Drm->Format		  = Wayland_Drm_Format;
		_G.Wayland.Drm->Authenticated = Wayland_Drm_Authenticated;
		_G.Wayland.Drm->Capabilities  = Wayland_Drm_Capabilities;
	}
}

internal void
Wayland_Registry_GlobalRemove(wayland_registry *This, u32 Name)
{
	Wayland_DebugLog(This, "Requested to remove global %d\n", Name);

	if (_G.Wayland.CompositorName == Name) {
		_G.Wayland.CompositorName = 0;
		Wayland_DestroyObject(_G.Wayland.Compositor);
	} else if (_G.Wayland.ShmName == Name) {
		_G.Wayland.ShmName = 0;
		Wayland_DestroyObject(_G.Wayland.Shm);
	} else if (_G.Wayland.XdgWmBaseName == Name) {
		_G.Wayland.XdgWmBaseName = 0;
		Wayland_DestroyObject(_G.Wayland.XdgWmBase);
	} else if (_G.Wayland.DrmName == Name) {
		_G.Wayland.DrmName = 0;
		Wayland_DestroyObject(_G.Wayland.Drm);
	}
}

internal void
Wayland_Frame(wayland_callback *This, u32 CallbackData)
{
	Wayland_DebugLog(This, "Notified of frame at time %d\n", CallbackData);

	wayland_window_state *Window = &_G.Wayland.Window;

	wayland_callback *FrameCallback = Wayland_Surface_Frame(Window->Surface);
	FrameCallback->Done				= Wayland_Frame;
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

	Wayland_Surface_Attach(Window->Surface, Window->Buffers[0], 0, 0);
	Wayland_Surface_DamageBuffer(
		Window->Surface,
		0,
		0,
		Window->Width,
		Window->Height
	);

	Wayland_Surface_Commit(Window->Surface);

	// Egl_Swap(&_G.Wayland.Egl);
}

internal s32
Wayland_EventHandlerThread(vptr Data)
{
	wayland_event Event;

	while (Wayland_IsConnected()) {
		Platform_LockMutex(&_G.Wayland.SyncLock);
		if (Wayland_PollEventQueue(20, &Event)) {
			Wayland_DispatchEvent(Event);
			Wayland_DestroyEvent(Event);
		}
		Platform_UnlockMutex(&_G.Wayland.SyncLock);
	}

	return 0;
}

internal void
Wayland_SyncAndHandleEvents(void)
{
	wayland_event Event;
	Platform_LockMutex(&_G.Wayland.SyncLock);

	wayland_callback *SyncCallback = Wayland_Display_Sync(_G.Wayland.Display);
	while (Wayland_PollEventQueue(20, &Event)) {
		if (Event.Method.Object == (wayland_interface *) SyncCallback) {
			Wayland_DestroyEvent(Event);
			break;
		}
		Wayland_DispatchEvent(Event);
		Wayland_DestroyEvent(Event);
	}

	Platform_UnlockMutex(&_G.Wayland.SyncLock);
}

internal b08
Wayland_TryInit(void)
{
	if (Wayland_Connect()) {
		wayland_display *Display = Wayland_GetDisplay();
		Display->Error			 = Wayland_Display_Error;
		Display->DeleteId		 = Wayland_Display_DeleteId;
		_G.Wayland.Display		 = Display;

		wayland_registry *Registry =
			Wayland_Display_GetRegistry(_G.Wayland.Display);
		Registry->Global	   = Wayland_Registry_Global;
		Registry->GlobalRemove = Wayland_Registry_GlobalRemove;
		_G.Wayland.Registry	   = Registry;

		b08 Success = Platform_CreateThread(
			&_G.Wayland.EventThread,
			Wayland_EventHandlerThread,
			NULL
		);
		if (!Success) goto error;

		Wayland_DebugLog(NULL, "Syncing for global events...\n");
		Wayland_SyncAndHandleEvents();

		Wayland_DebugLog(NULL, "Syncing for DRM authentication...\n");
		Wayland_SyncAndHandleEvents();
		if (!_G.Wayland.DrmAuthenticated) goto error;

		Wayland_DebugLog(NULL, "Connected!\n");
	}

	return Wayland_IsConnected();

error:
	Wayland_Disconnect();
	return FALSE;
}

internal wayland_surface *
Wayland_CreateGLWindow(c08 *Title, usize Width, usize Height)
{
	if (!Wayland_IsConnected()) return NULL;
	if (_G.Wayland.Window.Surface) return _G.Wayland.Window.Surface;
	if (!Width || !Height) return NULL;

	Wayland_DebugLog(
		NULL,
		"Creating %dx%d window titled %s...\n",
		Width,
		Height,
		CString(Title)
	);
	_G.Wayland.Window.Width	 = Width;
	_G.Wayland.Window.Height = Height;
	_G.Wayland.DrmFormat	 = DRM_FORMAT_XRGB8888;

	b08 GbmStatus = Gbm_Init(
		_G.Wayland.DrmFd,
		Width,
		Height,
		_G.Wayland.DrmFormat,
		&_G.Wayland.Gbm
	);
	if (!GbmStatus) goto error;

	if (!Egl_Init(&_G.Wayland.Gbm, _G.Heap, &_G.Wayland.Egl)) goto error;

	s32	  ShmFd		 = 0;
	usize BufferSize = 4 * Width * Height;
	vptr  BufferData = NULL;

	wayland_shm_pool	 *ShmPool	  = NULL;
	wayland_buffer		 *Buffer1	  = NULL;
	wayland_buffer		 *Buffer2	  = NULL;
	wayland_surface		 *Surface	  = NULL;
	wayland_xdg_surface	 *XdgSurface  = NULL;
	wayland_xdg_toplevel *XdgToplevel = NULL;

	Surface = Wayland_Compositor_CreateSurface(_G.Wayland.Compositor);
	if (!Surface) goto error;
	Surface->Enter				  = Wayland_Surface_Enter;
	Surface->Leave				  = Wayland_Surface_Leave;
	Surface->PreferredBufferScale = Wayland_Surface_PreferredBufferScale;
	Surface->PreferredBufferTransform =
		Wayland_Surface_PreferredBufferTransform;

	XdgSurface = Wayland_XdgWmBase_GetXdgSurface(_G.Wayland.XdgWmBase, Surface);
	if (!XdgSurface) goto error;
	XdgSurface->Configure = Wayland_XdgSurface_Configure;

	XdgToplevel = Wayland_XdgSurface_GetToplevel(XdgSurface);
	if (!XdgToplevel) goto error;
	XdgToplevel->Configure		 = Wayland_XdgToplevel_Configure;
	XdgToplevel->Close			 = Wayland_XdgToplevel_Close;
	XdgToplevel->ConfigureBounds = Wayland_XdgToplevel_ConfigureBounds;
	XdgToplevel->WmCapabilities	 = Wayland_XdgToplevel_WmCapabilities;

	s32 ShmSize = 2 * BufferSize;
	ShmFd		= Sys_MemfdCreate("wayland-shm", 0);
	if (ShmFd < 0) goto error;
	if (Sys_FTruncate(ShmFd, ShmSize) < 0) goto error;
	BufferData = Sys_MemMap(
		NULL,
		ShmSize,
		SYS_PROT_READ | SYS_PROT_WRITE,
		SYS_MAP_SHARED | SYS_MAP_POPULATE,
		ShmFd,
		0
	);
	if ((ssize) BufferData < 0 && (ssize) BufferData > -4096) goto error;

	ShmPool = Wayland_Shm_CreatePool(_G.Wayland.Shm, ShmFd, ShmSize);
	if (!ShmPool) goto error;

	Buffer1 = Wayland_ShmPool_CreateBuffer(
		ShmPool,
		0,
		Width,
		Height,
		Width * 4,
		WAYLAND_SHM_FORMAT_ARGB8888
	);
	if (!Buffer1) goto error;
	Buffer1->Release = Wayland_Buffer_Release;

	Buffer2 = Wayland_ShmPool_CreateBuffer(
		ShmPool,
		BufferSize,
		Width,
		Height,
		Width * 4,
		WAYLAND_SHM_FORMAT_ARGB8888
	);
	if (!Buffer2) goto error;
	Buffer2->Release = Wayland_Buffer_Release;

	Wayland_XdgToplevel_SetTitle(XdgToplevel, "Voxarc");
	Wayland_XdgToplevel_SetAppId(XdgToplevel, "voxarc");
	Wayland_Surface_Commit(Surface);

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

	Wayland_Surface_Attach(Surface, Buffer2, 0, 0);
	Wayland_Surface_DamageBuffer(Surface, 0, 0, Width, Height);

	wayland_callback *FrameCallback = Wayland_Surface_Frame(Surface);
	FrameCallback->Done				= Wayland_Frame;
	Window->FrameCallback			= FrameCallback;

	Wayland_Surface_Commit(Surface);

	return Surface;

error:
	if (Buffer1->Interface.Id) Wayland_Buffer_Destroy(Buffer1);
	if (Buffer2->Interface.Id) Wayland_Buffer_Destroy(Buffer2);
	if (ShmPool->Interface.Id) Wayland_ShmPool_Destroy(ShmPool);
	if (XdgToplevel->Interface.Id) Wayland_XdgToplevel_Destroy(XdgToplevel);
	if (XdgSurface->Interface.Id) Wayland_XdgSurface_Destroy(XdgSurface);
	if (Surface->Interface.Id) Wayland_Surface_Destroy(Surface);
	if (BufferData) Sys_MemUnmap(BufferData, ShmSize);
	if (ShmFd) Sys_Close(ShmFd);
	Wayland_Disconnect();
	return NULL;
}

#undef CALL

#endif
#endif
