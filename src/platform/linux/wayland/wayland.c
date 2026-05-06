/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                             *
*  Author: Aria Seiler                                                        *
*                                                                             *
*  This program is in the public domain. There is no implied warranty, so     *
*  use it at your own risk.                                                   *
*                                                                             *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef INCLUDE_HEADER

typedef struct wayland_dmabuf_format_entry {
	drm_format			Format;
	drm_format_modifier Modifier;
} wayland_dmabuf_format_entry;

typedef struct wayland_window_state {
	b08 Initialized;

	wayland_surface		 *Surface;
	wayland_xdg_surface	 *XdgSurface;
	wayland_xdg_toplevel *XdgToplevel;

	s32 Width;
	s32 Height;

	u32				  FrontBufferIndex;
	wayland_buffer	 *Buffers[2];
	wayland_callback *FrameCallback;
} wayland_window_state;

typedef struct wayland_state {
	s32					DrmFd;
	drm_format			DrmFormat;
	drm_format_modifier DrmFormatModifier;

	gbm Gbm;
	egl Egl;

	thread_handle EventThread;

	u32 SyncLock;

	wayland_display	 *Display;
	wayland_registry *Registry;

	wayland_compositor			*Compositor;
	u32							 CompositorName;
	wayland_xdg_wm_base			*XdgWmBase;
	u32							 XdgWmBaseName;
	wayland_zwp_linux_dmabuf_v1 *ZwpLinuxDmabufV1;
	u32							 ZwpLinuxDmabufV1Name;

	s32							 DrmFormatsFd;
	usize						 DrmFormatCount;
	wayland_dmabuf_format_entry *DrmFormats;

	wayland_window_state Window;

	s32						 PreferredBufferScale;
	wayland_output_transform PreferredBufferTransform;
	v2s32					 MaxWindowSize;
	v2s32					 PreferredWindowSize;

	hashmap Seats;
	hashmap Outputs;
} wayland_state;

#define WAYLAND_USER_FUNCS \
	EXPORT(b08,               Wayland_TryInit,        void) \
	EXPORT(wayland_surface *, Wayland_CreateGLWindow, c08 *Title, usize Width, usize Height) \
	EXPORT(void,              Wayland_SwapBuffers,    void) \
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
		FPrintL(
			"[%d] [Wayland] [%s] %s",
			Sys_GetTid(),
			Wayland_GetObjectName(Object),
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
Wayland_Buffer_Release(wayland_buffer *This)
{
	Wayland_DebugLog(This, "Released by compositor\n");

	wayland_window_state *Window = &_G.Wayland.Window;

	if (Window->Buffers[1] == This) {
		Window->Buffers[1] = Window->Buffers[0];
		Window->Buffers[0] = This;
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
Wayland_ZwpLinuxBufferParamsV1_Created(
	wayland_zwp_linux_buffer_params_v1 *This,
	wayland_buffer					   *Buffer
)
{
	Wayland_DebugLog(
		This,
		"Created dmabuf buffer %s\n",
		Wayland_GetObjectName((wayland_interface *) Buffer)
	);

	usize I = _G.Wayland.Window.Buffers[0] != NULL;

	Buffer->Release				 = Wayland_Buffer_Release;
	_G.Wayland.Window.Buffers[I] = Buffer;
}

internal void
Wayland_ZwpLinuxBufferParamsV1_Failed(wayland_zwp_linux_buffer_params_v1 *This)
{ Wayland_DebugLog(This, "Failed to create dmabuf buffer\n"); }

internal void
Wayland_ZwpLinuxDmabufFeedbackV1_Done(
	wayland_zwp_linux_dmabuf_feedback_v1 *This
)
{ Wayland_DebugLog(This, "Feedback is complete\n"); }

internal void
Wayland_ZwpLinuxDmabufFeedbackV1_FormatTable(
	wayland_zwp_linux_dmabuf_feedback_v1 *This,
	s32									  Fd,
	u32									  Size
)
{
	Wayland_DebugLog(This, "Sent format table (fd %d, %d bytes)\n", Fd, Size);

	vptr FormatData =
		Sys_MemMap(NULL, Size, SYS_PROT_READ, SYS_MAP_PRIVATE, Fd, 0);
	Assert(FormatData && (usize) FormatData <= (usize) -4096);

	_G.Wayland.DrmFormatsFd	  = Fd;
	_G.Wayland.DrmFormatCount = Size / sizeof(wayland_dmabuf_format_entry);
	_G.Wayland.DrmFormats	  = FormatData;
}

internal void
Wayland_ZwpLinuxDmabufFeedbackV1_MainDevice(
	wayland_zwp_linux_dmabuf_feedback_v1 *This,
	wayland_array						  Device
)
{
	Wayland_DebugLog(
		This,
		"Sent main device (%d bytes, %#llx)\n",
		Device.Size,
		*(u64 *) Device.Data
	);

	u64 SpecialDeviceId = *(u64 *) Device.Data;

	s32 Fd = Drm_OpenDriverFromVersion(
		SYS_DEV_MAJOR(SpecialDeviceId),
		SYS_DEV_MINOR(SpecialDeviceId)
	);
	if (Fd < 0) {
		Wayland_DebugLog(This, "Failed to open device node! code %d\n", -Fd);
		goto error;
	}

	s32 NodeType = Drm_GetNodeType(Fd);
	if (NodeType < 0) goto error;

	_G.Wayland.DrmFd = Fd;
	return;

error:
	if (Fd >= 0) Drm_CloseDriver(Fd);
}

internal void
Wayland_ZwpLinuxDmabufFeedbackV1_TrancheDone(
	wayland_zwp_linux_dmabuf_feedback_v1 *This
)
{ Wayland_DebugLog(This, "Tranche feedback is complete\n"); }

internal void
Wayland_ZwpLinuxDmabufFeedbackV1_TrancheTargetDevice(
	wayland_zwp_linux_dmabuf_feedback_v1 *This,
	wayland_array						  Device
)
{
	Wayland_DebugLog(
		This,
		"Sent tranche target device (%d bytes, %#llx)\n",
		Device.Size,
		*(u64 *) Device.Data
	);
}

internal void
Wayland_ZwpLinuxDmabufFeedbackV1_TrancheFormats(
	wayland_zwp_linux_dmabuf_feedback_v1 *This,
	wayland_array						  Indices
)
{
	Wayland_DebugLog(This, "Sent supported format indices for this tranche\n");

	if (_G.Wayland.DrmFormat) return;

	u16	 *IndicesData = (u16 *) Indices.Data;
	usize IndexCount  = Indices.Size / sizeof(u16);
	for (usize I = 0; I < IndexCount; I++) {
		u16 Index = IndicesData[I];

		wayland_dmabuf_format_entry Format = _G.Wayland.DrmFormats[Index];

		b08 FormatMatches	= Format.Format == DRM_FORMAT_XRGB8888
						   || Format.Format == DRM_FORMAT_XBGR8888
						   || Format.Format == DRM_FORMAT_ARGB8888;
		b08 ModifierMatches = Format.Modifier == DRM_FORMAT_MOD_INVALID;
		if (FormatMatches && ModifierMatches) {
			_G.Wayland.DrmFormat		 = Format.Format;
			_G.Wayland.DrmFormatModifier = Format.Modifier;
		}
		c08 *C = (c08 *) &Format.Format;
		Wayland_DebugLog(
			This,
			"- Format %#x ('%c%c%c%c'), Modifier %#llx\n",
			Format.Format,
			C[0],
			C[1],
			C[2],
			C[3],
			Format.Modifier
		);
	}
}

internal void
Wayland_ZwpLinuxDmabufFeedbackV1_TrancheFlags(
	wayland_zwp_linux_dmabuf_feedback_v1			  *This,
	wayland_zwp_linux_dmabuf_feedback_v1_tranche_flags Flags
)
{ Wayland_DebugLog(This, "Sent flags for this tranche: %#x\n", Flags); }

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
		"Offered global #%d, named %s-v%d\n",
		Name,
		CString(Interface),
		Version
	);

	string Str = CString(Interface);
	if (String_Cmp(Str, CStringL("wl_compositor")) == 0) {
		_G.Wayland.CompositorName = Name;
		_G.Wayland.Compositor	  = (wayland_compositor *)
			Wayland_Registry_Bind(This, Name, Interface, Version);
		Wayland_DebugLog(
			This,
			"Bound %s\n",
			Wayland_GetObjectName((wayland_interface *) _G.Wayland.Compositor)
		);
	} else if (String_Cmp(Str, CStringL("xdg_wm_base")) == 0) {
		_G.Wayland.XdgWmBaseName = Name;
		_G.Wayland.XdgWmBase	 = (wayland_xdg_wm_base *)
			Wayland_Registry_Bind(This, Name, Interface, Version);
		_G.Wayland.XdgWmBase->Ping = Wayland_XdgWmBase_Ping;
		Wayland_DebugLog(
			This,
			"Bound %s\n",
			Wayland_GetObjectName((wayland_interface *) _G.Wayland.XdgWmBase)
		);
	} else if (String_Cmp(Str, CStringL("zwp_linux_dmabuf_v1")) == 0) {
		if (Version < 4) return;
		_G.Wayland.ZwpLinuxDmabufV1Name = Name;
		_G.Wayland.ZwpLinuxDmabufV1		= (wayland_zwp_linux_dmabuf_v1 *)
			Wayland_Registry_Bind(This, Name, Interface, Version);
		Wayland_DebugLog(
			This,
			"Bound %s\n",
			Wayland_GetObjectName(
				(wayland_interface *) _G.Wayland.ZwpLinuxDmabufV1
			)
		);
	}
}

internal void
Wayland_Registry_GlobalRemove(wayland_registry *This, u32 Name)
{
	Wayland_DebugLog(This, "Requested to remove global %d\n", Name);

	if (_G.Wayland.CompositorName == Name) {
		_G.Wayland.CompositorName = 0;
		Wayland_DebugLog(
			This,
			"Destroying %s\n",
			Wayland_GetObjectName((wayland_interface *) _G.Wayland.Compositor)
		);
		Wayland_DestroyObject(_G.Wayland.Compositor);
	} else if (_G.Wayland.XdgWmBaseName == Name) {
		_G.Wayland.XdgWmBaseName = 0;
		Wayland_DebugLog(
			This,
			"Destroying %s\n",
			Wayland_GetObjectName((wayland_interface *) _G.Wayland.XdgWmBase)
		);
		Wayland_DestroyObject(_G.Wayland.XdgWmBase);
	} else if (_G.Wayland.ZwpLinuxDmabufV1Name == Name) {
		_G.Wayland.ZwpLinuxDmabufV1Name = 0;
		Wayland_DebugLog(
			This,
			"Destroying %s\n",
			Wayland_GetObjectName(
				(wayland_interface *) _G.Wayland.ZwpLinuxDmabufV1
			)
		);
		Wayland_DestroyObject(_G.Wayland.ZwpLinuxDmabufV1);
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

	u32 Index				 = Window->FrontBufferIndex;
	Window->FrontBufferIndex = (Index + 1) % 2;

	OpenGL_BindFramebuffer(
		GL_FRAMEBUFFER,
		_G.Wayland.Egl.FramebufferIds[Index]
	);
	OpenGL_Finish();

	Wayland_Surface_Attach(Window->Surface, Window->Buffers[Index], 0, 0);
	Wayland_Surface_DamageBuffer(
		Window->Surface,
		0,
		0,
		Window->Width,
		Window->Height
	);
	Wayland_Surface_Commit(Window->Surface);
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
	if (Wayland_IsConnected()) return TRUE;

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

		Wayland_DebugLog(NULL, "Connected!\n");
	}

	return Wayland_IsConnected();

error:
	Wayland_Disconnect();
	return FALSE;
}

internal void
Wayland_SwapBuffers(void)
{
	wayland_window_state *Window = &_G.Wayland.Window;

	u32 Index				 = Window->FrontBufferIndex;
	u32 NextIndex			 = (Index + 1) % 2;
	Window->FrontBufferIndex = NextIndex;

	OpenGL_Finish();

	Wayland_Surface_Attach(Window->Surface, Window->Buffers[Index], 0, 0);
	Wayland_Surface_DamageBuffer(
		Window->Surface,
		0,
		0,
		Window->Width,
		Window->Height
	);
	Wayland_Surface_Commit(Window->Surface);

	OpenGL_BindFramebuffer(
		GL_FRAMEBUFFER,
		_G.Wayland.Egl.FramebufferIds[NextIndex]
	);
}

internal wayland_surface *
Wayland_CreateGLWindow(c08 *Title, usize Width, usize Height)
{
	if (!Wayland_IsConnected()) return NULL;
	if (_G.Wayland.Window.Surface) return _G.Wayland.Window.Surface;
	if (!Width || !Height) return NULL;

	wayland_zwp_linux_dmabuf_feedback_v1 *Feedback = NULL;
	wayland_buffer						 *Buffer1 = NULL, *Buffer2 = NULL;
	wayland_surface						 *Surface	  = NULL;
	wayland_xdg_surface					 *XdgSurface  = NULL;
	wayland_xdg_toplevel				 *XdgToplevel = NULL;

	Wayland_DebugLog(
		NULL,
		"Creating %dx%d window titled %s...\n",
		Width,
		Height,
		CString(Title)
	);
	_G.Wayland.Window.Width	 = Width;
	_G.Wayland.Window.Height = Height;

	if (!_G.Wayland.ZwpLinuxDmabufV1) goto error;

	Surface = Wayland_Compositor_CreateSurface(_G.Wayland.Compositor);
	if (!Surface) goto error;
	Surface->Enter				  = Wayland_Surface_Enter;
	Surface->Leave				  = Wayland_Surface_Leave;
	Surface->PreferredBufferScale = Wayland_Surface_PreferredBufferScale;
	Surface->PreferredBufferTransform =
		Wayland_Surface_PreferredBufferTransform;

	Feedback = Wayland_ZwpLinuxDmabufV1_GetSurfaceFeedback(
		_G.Wayland.ZwpLinuxDmabufV1,
		Surface
	);
	Feedback->Done		  = Wayland_ZwpLinuxDmabufFeedbackV1_Done;
	Feedback->FormatTable = Wayland_ZwpLinuxDmabufFeedbackV1_FormatTable;
	Feedback->MainDevice  = Wayland_ZwpLinuxDmabufFeedbackV1_MainDevice;
	Feedback->TrancheDone = Wayland_ZwpLinuxDmabufFeedbackV1_TrancheDone;
	Feedback->TrancheTargetDevice =
		Wayland_ZwpLinuxDmabufFeedbackV1_TrancheTargetDevice;
	Feedback->TrancheFormats = Wayland_ZwpLinuxDmabufFeedbackV1_TrancheFormats;
	Feedback->TrancheFlags	 = Wayland_ZwpLinuxDmabufFeedbackV1_TrancheFlags;

	XdgSurface = Wayland_XdgWmBase_GetXdgSurface(_G.Wayland.XdgWmBase, Surface);
	if (!XdgSurface) goto error;
	XdgSurface->Configure = Wayland_XdgSurface_Configure;

	XdgToplevel = Wayland_XdgSurface_GetToplevel(XdgSurface);
	if (!XdgToplevel) goto error;
	XdgToplevel->Configure		 = Wayland_XdgToplevel_Configure;
	XdgToplevel->Close			 = Wayland_XdgToplevel_Close;
	XdgToplevel->ConfigureBounds = Wayland_XdgToplevel_ConfigureBounds;
	XdgToplevel->WmCapabilities	 = Wayland_XdgToplevel_WmCapabilities;
	Wayland_XdgToplevel_SetTitle(XdgToplevel, "Voxarc");
	Wayland_XdgToplevel_SetAppId(XdgToplevel, "voxarc");

	Wayland_DebugLog(NULL, "Syncing for dmabuf feedback...\n");
	Wayland_SyncAndHandleEvents();
	if (!_G.Wayland.DrmFormat) goto error;

	Wayland_ZwpLinuxDmabufFeedbackV1_Destroy(Feedback);
	Feedback = NULL;

	b08 GbmStatus = Gbm_Init(
		_G.Wayland.DrmFd,
		Width,
		Height,
		_G.Wayland.DrmFormat,
		_G.Wayland.DrmFormatModifier,
		&_G.Wayland.Gbm
	);
	if (!GbmStatus) goto error;

	if (!Egl_Init(&_G.Wayland.Gbm, _G.Heap, &_G.Wayland.Egl)) goto error;

	usize BufferSize = 4 * Width * Height;

	wayland_zwp_linux_buffer_params_v1 *BufferParams[2];
	for (usize I = 0; I < 2; I++) {
		BufferParams[I] =
			Wayland_ZwpLinuxDmabufV1_CreateParams(_G.Wayland.ZwpLinuxDmabufV1);
		BufferParams[I]->Created = Wayland_ZwpLinuxBufferParamsV1_Created;
		BufferParams[I]->Failed	 = Wayland_ZwpLinuxBufferParamsV1_Failed;
		Wayland_ZwpLinuxBufferParamsV1_Add(
			BufferParams[I],
			Gbm_BoGetFd(_G.Wayland.Gbm.BufferObjects[I]),
			0,
			0,
			Gbm_BoGetStride(_G.Wayland.Gbm.BufferObjects[I]),
			_G.Wayland.DrmFormatModifier >> 32,
			_G.Wayland.DrmFormatModifier & 0xFFFFFFFF
		);
		Wayland_ZwpLinuxBufferParamsV1_Create(
			BufferParams[I],
			Width,
			Height,
			_G.Wayland.DrmFormat,
			0
		);
	}

	Wayland_DebugLog(NULL, "Syncing for buffer creation...\n");
	Wayland_SyncAndHandleEvents();

	for (usize I = 0; I < 2; I++)
		Wayland_ZwpLinuxBufferParamsV1_Destroy(BufferParams[I]);

	Buffer1 = _G.Wayland.Window.Buffers[0];
	Buffer2 = _G.Wayland.Window.Buffers[1];
	if (!Buffer1 || !Buffer2) goto error;

	Wayland_Surface_Commit(Surface);

	wayland_window_state *Window = &_G.Wayland.Window;

	Window->Initialized = TRUE;
	Window->Surface		= Surface;
	Window->XdgSurface	= XdgSurface;
	Window->XdgToplevel = XdgToplevel;
	Window->Width		= Width;
	Window->Height		= Height;

	// Wait for the configure events to pass
	Wayland_SyncAndHandleEvents();

	Window->FrontBufferIndex = 0;
	Wayland_SwapBuffers();

	return Surface;

error:
	if (Buffer1) Wayland_Buffer_Destroy(Buffer1);
	if (Buffer2) Wayland_Buffer_Destroy(Buffer2);
	if (XdgToplevel) Wayland_XdgToplevel_Destroy(XdgToplevel);
	if (XdgSurface) Wayland_XdgSurface_Destroy(XdgSurface);
	if (Surface) Wayland_Surface_Destroy(Surface);
	if (Feedback) Wayland_ZwpLinuxDmabufFeedbackV1_Destroy(Feedback);
	Wayland_Disconnect();
	return NULL;
}

#undef CALL

#endif
#endif
