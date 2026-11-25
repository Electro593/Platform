/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                             *
*  Author: Aria Seiler                                                        *
*                                                                             *
*  This program is in the public domain. There is no implied warranty, so     *
*  use it at your own risk.                                                   *
*                                                                             *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef INCLUDE_HEADER

typedef struct wayland_dmabuf_tranche_entry {
	wayland_array									   Device;
	wayland_array									   FormatIndices;
	wayland_zwp_linux_dmabuf_feedback_v1_tranche_flags Flags;
} wayland_dmabuf_tranche_entry;

typedef struct wayland_seat_entry {
	wayland_seat		   *Seat;
	u32						GlobalName;
	string					Name;
	wayland_seat_capability Capabilities;

	wayland_pointer					*Pointer;
	v2r32							 PointerPos;
	wayland_zwp_relative_pointer_v1 *ZwpRelativePointerV1;

	wayland_keyboard			  *Keyboard;
	wayland_keyboard_keymap_format KeymapFormat;
	vptr						   KeymapData;
	usize						   KeymapSize;
} wayland_seat_entry;

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

	u32				  BackBufferIndex;
	wayland_buffer	 *Buffers[2];
	wayland_callback *FrameCallback;
} wayland_window_state;

typedef struct wayland_state {
	wayland_event_queue EventQueue;

	thread_handle EventThread;
	_Atomic b08	  ThreadRunning;

	wayland_display	 *Display;
	wayland_registry *Registry;

	wayland_compositor						*Compositor;
	u32										 CompositorName;
	wayland_fixes							*Fixes;
	u32										 FixesName;
	wayland_xdg_wm_base						*XdgWmBase;
	u32										 XdgWmBaseName;
	wayland_zwp_linux_dmabuf_v1				*ZwpLinuxDmabufV1;
	u32										 ZwpLinuxDmabufV1Name;
	wayland_zwp_relative_pointer_manager_v1 *ZwpRelativePointerManagerV1;
	u32										 ZwpRelativePointerManagerV1Name;

	s32					DrmFd;
	drm_format			DrmFormat;
	drm_format_modifier DrmFormatModifier;

	gbm Gbm;
	egl Egl;

	mutex_handle SyncLock;
	b08			 Syncing;

	usize						 DrmFormatsSize;
	wayland_dmabuf_format_entry *DrmFormats;
	ARRAY_LIST(wayland_dmabuf_tranche_entry) DrmTranches;

	wayland_window_state Window;

	s32						 PreferredBufferScale;
	wayland_output_transform PreferredBufferTransform;
	v2s32					 MaxWindowSize;
	v2s32					 PreferredWindowSize;

	wayland_seat_entry Seat;
} wayland_state;

#define WAYLAND_USER_FUNCS \
	EXPORT(b08,               Wayland_TryInit,        void) \
	EXPORT(void,              Wayland_Deinit,         void) \
	EXPORT(wayland_surface *, Wayland_CreateGLWindow, c08 *Title, usize Width, usize Height) \
	EXPORT(void,              Wayland_SwapBuffers,    void) \
	\
	INTERN(void, Wayland_Pointer_Motion, wayland_pointer *This, u32 Time, wayland_fixed SurfaceX, wayland_fixed SurfaceY) \
	INTERN(void, Wayland_Pointer_Frame,  wayland_pointer *This) \
	//

#endif

#ifdef _LINUX
#ifdef INCLUDE_SOURCE

internal void Wayland_ModifySeatEntryCapabilities(
	wayland_seat_entry	   *Entry,
	wayland_seat_capability Capabilities
);

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
			Wayland_GetDisplayName(Object),
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
		Wayland_GetDisplayName(Object),
		Code,
		CString(Message)
	);
	Wayland_Disconnect();
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
Wayland_Callback_Done_Sync(wayland_callback *This, u32 CallbackData)
{
	Wayland_DebugLog(This, "Event queue sync sentinel hit\n");

	Assert(_G.Wayland.Syncing);
	_G.Wayland.Syncing = FALSE;
}

internal void
Wayland_Keyboard_Keymap(
	wayland_keyboard			  *This,
	wayland_keyboard_keymap_format Format,
	s32							   Fd,
	u32							   Size
)
{
	Wayland_DebugLog(This, "Sent keymap table (fd %d, %d bytes)\n", Fd, Size);

	wayland_seat_entry *Entry = &_G.Wayland.Seat;
	Assert(Entry->Seat && Entry->Keyboard == This);

	Entry->KeymapFormat = Format;
	Entry->KeymapSize	= Size;
	Entry->KeymapData =
		Sys_MemMap(NULL, Size, SYS_PROT_READ, SYS_MAP_PRIVATE, Fd, 0);
	Assert(Entry->KeymapData && (usize) Entry->KeymapData <= (usize) -4096);
	Sys_Close(Fd);

	FPrintL("%s\n", CString(Entry->KeymapData));

	// TODO
	// _G.Wayland.Keymap = KeymapData;
}

internal void
Wayland_Keyboard_Enter(
	wayland_keyboard *This,
	u32				  Serial,
	wayland_surface	 *Surface,
	wayland_array	  Keys
)
{
	Wayland_DebugLog(
		This,
		"Keyboard focused on %s with serial %u and %d keys pressed\n",
		Wayland_GetDisplayName(Surface),
		Serial,
		Keys.Size / sizeof(u32)
	);

	if (_G.FocusState == FOCUS_NONE) {
		Mem_Set(_G.Keys, 0, sizeof(_G.Keys));
		Mem_Set(_G.Buttons, 0, sizeof(_G.Buttons));

		_G.FocusState		= FOCUS_CLIENT;
		_G.CursorIsDisabled = TRUE;	 // TODO: Properly capture cursor
	}
}

internal void
Wayland_Keyboard_Leave(
	wayland_keyboard *This,
	u32				  Serial,
	wayland_surface	 *Surface
)
{
	Wayland_DebugLog(
		This,
		"Keyboard defocused from %s with serial %u\n",
		Wayland_GetDisplayName(Surface),
		Serial
	);

	_G.FocusState = FOCUS_NONE;
}

internal void
Wayland_Pointer_Motion(
	wayland_pointer *This,
	u32				 Time,
	wayland_fixed	 SurfaceX,
	wayland_fixed	 SurfaceY
)
{
	wayland_seat_entry *Entry = &_G.Wayland.Seat;
	Assert(Entry->Seat && Entry->Pointer);

	Entry->PointerPos = (v2r32){
		.X = Wayland_FixedToR32(SurfaceX),
		.Y = Wayland_FixedToR32(SurfaceY),
	};

	Wayland_DebugLog(
		This,
		"Pointer motion at %ums: (%f, %f)\n",
		Entry->PointerPos.X,
		Entry->PointerPos.Y
	);
}

internal void
Wayland_Pointer_Frame(wayland_pointer *This)
{
	Wayland_DebugLog(This, "Pointer frame\n");

	wayland_seat_entry *Entry = &_G.Wayland.Seat;
	Assert(Entry->Seat && Entry->Pointer);

	_G.CursorPos = (v2s32){
		(s32) Entry->PointerPos.X,
		(s32) Entry->PointerPos.Y,
	};
}

internal void
Wayland_Seat_Capabilities(
	wayland_seat		   *This,
	wayland_seat_capability Capabilities
)
{
	Wayland_DebugLog(This, "Advertized seat capabilities %x\n", Capabilities);

	wayland_seat_entry *Entry = &_G.Wayland.Seat;
	Assert(Entry->Seat);

	Wayland_ModifySeatEntryCapabilities(Entry, Capabilities);
}

internal void
Wayland_Seat_Name(wayland_seat *This, c08 *Name)
{
	Wayland_DebugLog(This, "Advertized seat name %s\n", CString(Name));

	wayland_seat_entry *Entry = &_G.Wayland.Seat;
	Assert(Entry->Seat);

	Entry->Name = HString(_G.Heap, Name);
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

	_G.ExecutionState		 = EXECUTION_ENDED;
	_G.Wayland.ThreadRunning = FALSE;
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
		Wayland_GetDisplayName(Buffer)
	);

	usize I = _G.Wayland.Window.Buffers[0] != NULL;

	Buffer->Release = Wayland_Buffer_Release;
	Wayland_LinkEventQueue((vptr) Buffer, &_G.Wayland.EventQueue);

	_G.Wayland.Window.Buffers[I] = Buffer;
}

internal void
Wayland_ZwpLinuxBufferParamsV1_Failed(wayland_zwp_linux_buffer_params_v1 *This)
{ Wayland_DebugLog(This, "Failed to create dmabuf buffer\n"); }

internal void
Wayland_ZwpLinuxDmabufFeedbackV1_Done(
	wayland_zwp_linux_dmabuf_feedback_v1 *This
)
{
	Wayland_DebugLog(This, "Feedback is complete\n");

	if (_G.Wayland.DrmFormat) {
		c08 *C = (c08 *) &_G.Wayland.DrmFormat;
		Wayland_DebugLog(
			This,
			"- Selected Format: %#x ('%c%c%c%c')\n",
			_G.Wayland.DrmFormat,
			C[0],
			C[1],
			C[2],
			C[3]
		);
		Wayland_DebugLog(
			This,
			"- Selected Modifier: %0#18llx\n",
			_G.Wayland.DrmFormatModifier
		);
	}

	Sys_MemUnmap(_G.Wayland.DrmFormats, _G.Wayland.DrmFormatsSize);
	_G.Wayland.DrmFormats	  = NULL;
	_G.Wayland.DrmFormatsSize = 0;
}

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
	Sys_Close(Fd);

	_G.Wayland.DrmFormatsSize = Size;
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

	Assert(Device.Size == sizeof(sys_dev));
	sys_dev DeviceId = Device.Data[0];
	if (Device.Size == 8) DeviceId |= (sys_dev) Device.Data[1] << 32;

	s32 Fd = Drm_OpenRenderNodeFromDevice(DeviceId);
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
			"- Format %#x ('%c%c%c%c'), Modifier %0#18llx\n",
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
Wayland_ZwpRelativePointerV1_RelativeMotion(
	wayland_zwp_relative_pointer_v1 *This,
	u32								 MicroTimeHigh,
	u32								 MicroTimeLow,
	wayland_fixed					 DeltaX,
	wayland_fixed					 DeltaY,
	wayland_fixed					 DeltaXUnaccel,
	wayland_fixed					 DeltaYUnaccel
)
{
	Wayland_DebugLog(
		This,
		"Notified of relative motion at %lluus: %f, %f (%f, %f "
		"unaccelerated)\n",
		(u64) MicroTimeHigh << 32 | MicroTimeLow,
		Wayland_FixedToR32(DeltaX),
		Wayland_FixedToR32(DeltaY),
		Wayland_FixedToR32(DeltaYUnaccel),
		Wayland_FixedToR32(DeltaYUnaccel)
	);
}

internal void
Wayland_ModifySeatEntryCapabilities(
	wayland_seat_entry	   *Entry,
	wayland_seat_capability Capabilities
)
{
	b08 HasPointer = Capabilities & WAYLAND_SEAT_CAPABILITY_POINTER;
	if (HasPointer && !Entry->Pointer) {
		Entry->Pointer		   = Wayland_Seat_GetPointer(Entry->Seat);
		Entry->Pointer->Motion = Wayland_Pointer_Motion;
		Entry->Pointer->Frame  = Wayland_Pointer_Frame;
		Wayland_LinkEventQueue((vptr) Entry->Pointer, &_G.Wayland.EventQueue);

		Entry->ZwpRelativePointerV1 =
			Wayland_ZwpRelativePointerManagerV1_GetRelativePointer(
				_G.Wayland.ZwpRelativePointerManagerV1,
				Entry->Pointer
			);
		Entry->ZwpRelativePointerV1->RelativeMotion =
			Wayland_ZwpRelativePointerV1_RelativeMotion;
		Wayland_LinkEventQueue(
			(vptr) Entry->ZwpRelativePointerV1,
			&_G.Wayland.EventQueue
		);
	} else if (!HasPointer && Entry->Pointer) {
		Wayland_ZwpRelativePointerV1_Destroy(Entry->ZwpRelativePointerV1);
		Entry->ZwpRelativePointerV1 = NULL;

		Wayland_Pointer_Release(Entry->Pointer);
		Entry->Pointer = NULL;
	}

	b08 HasKeyboard = Capabilities & WAYLAND_SEAT_CAPABILITY_KEYBOARD;
	if (HasKeyboard && !Entry->Keyboard) {
		Entry->Keyboard			= Wayland_Seat_GetKeyboard(Entry->Seat);
		Entry->Keyboard->Keymap = Wayland_Keyboard_Keymap;
		Entry->Keyboard->Enter	= Wayland_Keyboard_Enter;
		Entry->Keyboard->Leave	= Wayland_Keyboard_Leave;
		Wayland_LinkEventQueue((vptr) Entry->Keyboard, &_G.Wayland.EventQueue);
	} else if (!HasKeyboard && Entry->Keyboard) {
		if (Entry->KeymapData) {
			Sys_MemUnmap(Entry->KeymapData, Entry->KeymapSize);
			Entry->KeymapFormat = 0;
			Entry->KeymapData	= NULL;
			Entry->KeymapSize	= 0;
		}
		Wayland_Keyboard_Release(Entry->Keyboard);
		Entry->Keyboard = NULL;
	}

	Entry->Capabilities = Capabilities;
}

internal void
Wayland_Registry_Global(
	wayland_registry *This,
	u32				  Name,
	c08				 *Interface,
	u32				  Version
)
{
#define MATCHES(ShortName) (String_Cmp(Str, CString(Wayland_##ShortName##_Prototype.Name)) == 0)
#define BIND() (Bound = (vptr) Wayland_Registry_Bind(This, Name, Interface, Version))

	Wayland_DebugLog(
		This,
		"Offered global #%d, named %s-v%d\n",
		Name,
		CString(Interface),
		Version
	);

	vptr   Bound = NULL;
	string Str	 = CString(Interface);

	// TODO: Global table by name

	if (MATCHES(Compositor)) {
		_G.Wayland.CompositorName = Name;
		_G.Wayland.Compositor	  = BIND();
	} else if (MATCHES(Fixes)) {
		_G.Wayland.FixesName = Name;
		_G.Wayland.Fixes	 = BIND();
	} else if (MATCHES(Seat)) {
		if (!_G.Wayland.Seat.Seat) {
			wayland_seat *Seat		   = BIND();
			Seat->Capabilities		   = Wayland_Seat_Capabilities;
			Seat->Name				   = Wayland_Seat_Name;
			_G.Wayland.Seat.Seat	   = Seat;
			_G.Wayland.Seat.GlobalName = Name;
		}
	} else if (MATCHES(XdgWmBase)) {
		_G.Wayland.XdgWmBaseName   = Name;
		_G.Wayland.XdgWmBase	   = BIND();
		_G.Wayland.XdgWmBase->Ping = Wayland_XdgWmBase_Ping;
	} else if (MATCHES(ZwpLinuxDmabufV1)) {
		if (Version >= 4) {
			_G.Wayland.ZwpLinuxDmabufV1Name = Name;
			_G.Wayland.ZwpLinuxDmabufV1		= BIND();
		}
	} else if (MATCHES(ZwpRelativePointerManagerV1)) {
		_G.Wayland.ZwpRelativePointerManagerV1Name = Name;
		_G.Wayland.ZwpRelativePointerManagerV1	   = BIND();
	}

	if (Bound) {
		Wayland_DebugLog(This, "Bound %s\n", Wayland_GetDisplayName(Bound));
		Wayland_LinkEventQueue(Bound, &_G.Wayland.EventQueue);
	}

#undef MATCHES
#undef BIND
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
			Wayland_GetDisplayName(_G.Wayland.Compositor)
		);
		if (Wayland_GetVersion(_G.Wayland.Compositor) >= 7)
			Wayland_Compositor_Release(_G.Wayland.Compositor);
		else Wayland_DestroyClientObject((vptr) _G.Wayland.Compositor);
	} else if (_G.Wayland.FixesName == Name) {
		_G.Wayland.FixesName = 0;
		Wayland_DebugLog(
			This,
			"Destroying %s\n",
			Wayland_GetDisplayName(_G.Wayland.Fixes)
		);
		Wayland_Fixes_Destroy(_G.Wayland.Fixes);
	} else if (_G.Wayland.Seat.GlobalName == Name) {
		_G.Wayland.Seat.GlobalName = 0;
		Wayland_DebugLog(
			This,
			"Destroying %s\n",
			Wayland_GetDisplayName(_G.Wayland.Seat.Seat)
		);
		Wayland_ModifySeatEntryCapabilities(&_G.Wayland.Seat, 0);
		if (Wayland_GetVersion(_G.Wayland.Seat.Seat) >= 5)
			Wayland_Seat_Release(_G.Wayland.Seat.Seat);
		else Wayland_DestroyClientObject((vptr) _G.Wayland.Seat.Seat);
		_G.Wayland.Seat.Seat = NULL;
	} else if (_G.Wayland.XdgWmBaseName == Name) {
		_G.Wayland.XdgWmBaseName = 0;
		Wayland_DebugLog(
			This,
			"Destroying %s\n",
			Wayland_GetDisplayName(_G.Wayland.XdgWmBase)
		);
		Wayland_XdgWmBase_Destroy(_G.Wayland.XdgWmBase);
	} else if (_G.Wayland.ZwpLinuxDmabufV1Name == Name) {
		_G.Wayland.ZwpLinuxDmabufV1Name = 0;
		Wayland_DebugLog(
			This,
			"Destroying %s\n",
			Wayland_GetDisplayName(_G.Wayland.ZwpLinuxDmabufV1)
		);
		Wayland_ZwpLinuxDmabufV1_Destroy(_G.Wayland.ZwpLinuxDmabufV1);
	}

	Wayland_Fixes_AckGlobalRemove(_G.Wayland.Fixes, _G.Wayland.Registry, Name);
}

internal void
Wayland_Callback_Done_Frame(wayland_callback *This, u32 CallbackData)
{
	Wayland_DebugLog(This, "Notified of frame at time %d\n", CallbackData);

	wayland_window_state *Window = &_G.Wayland.Window;

	wayland_callback *FrameCallback = Wayland_Surface_Frame(Window->Surface);
	FrameCallback->Done				= Wayland_Callback_Done_Frame;
	Wayland_LinkEventQueue((vptr) FrameCallback, &_G.Wayland.EventQueue);

	Window->FrameCallback = FrameCallback;
	Wayland_SwapBuffers();
}

internal s32
Wayland_EventHandlerThread(vptr Data)
{
	usize DebugCounter = 0;

	usize PollsSinceLastEvent = 0;
	while (_G.Wayland.ThreadRunning && Wayland_IsConnected()) {
		Platform_LockMutex(&_G.Wayland.SyncLock);

		wayland_event Event;
		b08 Polled = Wayland_PollConnection(20)
				  && Wayland_DequeueEvent(&_G.Wayland.EventQueue, &Event);
		if (Polled) {
			Wayland_DispatchEvent(Event);
			Wayland_DestroyEvent(Event);
			PollsSinceLastEvent = 0;
			DebugCounter++;
		} else {
			PollsSinceLastEvent++;
		}

		if (DebugCounter > 100) {
			DebugCounter = 0;
			HASHMAP_FOREACH (
				I,
				Hash,
				u32,
				Id,
				wayland_interface *,
				Object,
				&_G.WaylandApi.IdTable
			)
			{
				u32 EventCount = Object->InternalEventQueue.Deque.Count;
				if (!Object->LinkedEventQueue && EventCount > 0) {
					Wayland_LogMessage(
						Object,
						NULL,
						"[Debug] Has %d outstanding events\n",
						EventCount
					);
				}
			}
		}

		Platform_UnlockMutex(&_G.Wayland.SyncLock);

		if (PollsSinceLastEvent) {
			usize BackoffExponent = MIN(PollsSinceLastEvent, 10);
			usize NanosecondDelay = 1000 << BackoffExponent;

			sys_timespec Delay = { .Seconds = 0, .Nano = NanosecondDelay };
			Sys_NanoSleep(&Delay, NULL);
		}
	}

	return 0;
}

internal void
Wayland_SyncAndHandleEvents(void)
{
	Platform_LockMutex(&_G.Wayland.SyncLock);
	_G.Wayland.Syncing = TRUE;

	wayland_callback *SyncCallback = Wayland_Display_Sync(_G.Wayland.Display);
	SyncCallback->Done			   = Wayland_Callback_Done_Sync;
	Wayland_LinkEventQueue((vptr) SyncCallback, &_G.Wayland.EventQueue);

	while (_G.Wayland.Syncing && Wayland_PollConnection(20)) {
		wayland_event Event;
		if (Wayland_DequeueEvent(&_G.Wayland.EventQueue, &Event)) {
			Wayland_DispatchEvent(Event);
			Wayland_DestroyEvent(Event);
		}
	}

	Platform_UnlockMutex(&_G.Wayland.SyncLock);
}

internal b08
Wayland_TryInit(void)
{
	if (Wayland_IsConnected()) return TRUE;

	if (Wayland_Connect()) {
		_G.Wayland.EventQueue = Wayland_CreateEventQueue();

		wayland_display *Display = Wayland_GetDisplay();
		Display->Error			 = Wayland_Display_Error;
		_G.Wayland.Display		 = Display;
		Wayland_LinkEventQueue((vptr) Display, &_G.Wayland.EventQueue);

		wayland_registry *Registry =
			Wayland_Display_GetRegistry(_G.Wayland.Display);
		Registry->Global	   = Wayland_Registry_Global;
		Registry->GlobalRemove = Wayland_Registry_GlobalRemove;
		_G.Wayland.Registry	   = Registry;
		Wayland_LinkEventQueue((vptr) Registry, &_G.Wayland.EventQueue);

		_G.Wayland.ThreadRunning = TRUE;
		b08 Success				 = Platform_CreateThread(
			&_G.Wayland.EventThread,
			Wayland_EventHandlerThread,
			NULL
		);
		if (!Success) goto error;

		Wayland_DebugLog(NULL, "Syncing for global events...\n");
		Wayland_SyncAndHandleEvents();

		if (!_G.Wayland.ZwpLinuxDmabufV1
			|| !_G.Wayland.ZwpRelativePointerManagerV1)
			goto error;

		_G.Wayland.DrmTranches =
			ArrayList_Init(_G.Heap, sizeof(wayland_dmabuf_tranche_entry), 8);

		Wayland_DebugLog(NULL, "Connected!\n");
	}

	return Wayland_IsConnected();

error:
	Wayland_Deinit();
	return FALSE;
}

internal void
Wayland_SwapBuffers(void)
{
	wayland_window_state *Window = &_G.Wayland.Window;

	u32 Index				= Window->BackBufferIndex;
	u32 NextIndex			= (Index + 1) % 2;
	Window->BackBufferIndex = NextIndex;

	OpenGL_Finish();

	Wayland_Surface_Attach(Window->Surface, Window->Buffers[Index], 0, 0);
	Wayland_Surface_DamageBuffer(
		Window->Surface,
		0,
		0,
		Window->Width,
		Window->Height
	);
	Wayland_Surface_SetBufferTransform(
		Window->Surface,
		WAYLAND_OUTPUT_TRANSFORM_FLIPPED_180
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
	Wayland_LinkEventQueue((vptr) Surface, &_G.Wayland.EventQueue);

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
	Wayland_LinkEventQueue((vptr) Feedback, &_G.Wayland.EventQueue);

	XdgSurface = Wayland_XdgWmBase_GetXdgSurface(_G.Wayland.XdgWmBase, Surface);
	if (!XdgSurface) goto error;
	XdgSurface->Configure = Wayland_XdgSurface_Configure;
	Wayland_LinkEventQueue((vptr) XdgSurface, &_G.Wayland.EventQueue);

	XdgToplevel = Wayland_XdgSurface_GetToplevel(XdgSurface);
	if (!XdgToplevel) goto error;
	XdgToplevel->Configure		 = Wayland_XdgToplevel_Configure;
	XdgToplevel->Close			 = Wayland_XdgToplevel_Close;
	XdgToplevel->ConfigureBounds = Wayland_XdgToplevel_ConfigureBounds;
	XdgToplevel->WmCapabilities	 = Wayland_XdgToplevel_WmCapabilities;
	Wayland_LinkEventQueue((vptr) XdgToplevel, &_G.Wayland.EventQueue);
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
		Wayland_LinkEventQueue((vptr) BufferParams[I], &_G.Wayland.EventQueue);

		gbm_bo *Bo = _G.Wayland.Gbm.BufferObjects[I];
		Wayland_ZwpLinuxBufferParamsV1_Add(
			BufferParams[I],
			Gbm_BoGetFd(Bo),
			0,
			0,
			Gbm_BoGetStride(Bo),
			Gbm_BoGetModifier(Bo) >> 32,
			Gbm_BoGetModifier(Bo) & 0xFFFFFFFF
		);
		Wayland_ZwpLinuxBufferParamsV1_Create(
			BufferParams[I],
			Gbm_BoGetWidth(Bo),
			Gbm_BoGetHeight(Bo),
			Gbm_BoGetFormat(Bo),
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

	Window->BackBufferIndex = 0;
	Wayland_SwapBuffers();

	_G.Updates |= WINDOW_RESIZED;

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

internal void
Wayland_Deinit(void)
{
	if (_G.Wayland.EventThread.ThreadId) {
		_G.Wayland.ThreadRunning = FALSE;
		Platform_JoinThread(_G.Wayland.EventThread);
	}

	ARRAY_LIST_FOREACH (&_G.Wayland.DrmTranches, wayland_dmabuf_tranche_entry) {
		Wayland_FreeArray(It.Device);
		Wayland_FreeArray(It.FormatIndices);
	}
	ArrayList_Free(&_G.Wayland.DrmTranches);

	Egl_Deinit(&_G.Wayland.Egl);
	Gbm_Deinit(&_G.Wayland.Gbm);
	if (_G.Wayland.DrmFd >= 0) Drm_CloseDriver(_G.Wayland.DrmFd);

	if (Wayland_IsConnected()) {
		if (_G.Wayland.Registry) {
			if (Wayland_GetVersion(_G.Wayland.Fixes) >= 2)
				Wayland_Fixes_DestroyRegistry(
					_G.Wayland.Fixes,
					_G.Wayland.Registry
				);
			else Wayland_DestroyClientObject((vptr) _G.Wayland.Registry);
		}

		if (_G.Wayland.Compositor) {
			if (Wayland_GetVersion(_G.Wayland.Compositor) >= 7)
				Wayland_Compositor_Release(_G.Wayland.Compositor);
			else Wayland_DestroyClientObject((vptr) _G.Wayland.Compositor);
		}
		if (_G.Wayland.Fixes) Wayland_Fixes_Destroy(_G.Wayland.Fixes);
		if (_G.Wayland.XdgWmBase)
			Wayland_XdgWmBase_Destroy(_G.Wayland.XdgWmBase);
		if (_G.Wayland.ZwpLinuxDmabufV1)
			Wayland_ZwpLinuxDmabufV1_Destroy(_G.Wayland.ZwpLinuxDmabufV1);
		if (_G.Wayland.ZwpRelativePointerManagerV1)
			Wayland_ZwpRelativePointerManagerV1_Destroy(
				_G.Wayland.ZwpRelativePointerManagerV1
			);

		Wayland_Disconnect();
	}

	Wayland_DestroyEventQueue(&_G.Wayland.EventQueue);

	Mem_Set(&_G.Wayland, 0, sizeof(wayland_state));
}

#undef CALL

#endif
#endif
