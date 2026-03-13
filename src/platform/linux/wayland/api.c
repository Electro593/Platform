/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                             *
*  Author: Aria Seiler                                                        *
*                                                                             *
*  This program is in the public domain. There is no implied warranty, so     *
*  use it at your own risk.                                                   *
*                                                                             *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef INCLUDE_HEADER

#define WAYLAND_DISPLAY_ID 1

#ifndef SECTION_TYPES

typedef u32 wayland_fixed;

typedef struct wayland_array		   wayland_array;
typedef struct wayland_param		   wayland_param;
typedef struct wayland_interface	   wayland_interface;
typedef struct wayland_message		   wayland_message;
typedef struct wayland_method		   wayland_method;
typedef struct wayland_prepared_method wayland_prepared_method;
typedef struct wayland_event		   wayland_event;
typedef struct wayland_prototype	   wayland_prototype;

typedef enum wayland_param_type : u08 {
	WAYLAND_PARAM_SINT,
	WAYLAND_PARAM_UINT,
	WAYLAND_PARAM_FIXED,
	WAYLAND_PARAM_STRING,
	WAYLAND_PARAM_ARRAY,
	WAYLAND_PARAM_OBJECT,
	WAYLAND_PARAM_NEWOBJECT,
	WAYLAND_PARAM_FD,
} wayland_param_type;

struct wayland_array {
	u32	 Size;
	u32 *Data;
};

struct wayland_param {
	wayland_param_type Type;
	u08				   Size;
	b08				   Nullable;

	union {
		s32			  Sint;
		u32			  Uint;
		wayland_fixed Fixed;

		c08			 *String;
		wayland_array Array;

		struct {
			wayland_interface *Data;
			b08				   Destructor;
		} Object;

		struct {
			wayland_interface *Data;
			wayland_prototype *Prototype;
		} NewObject;

		s32 Fd;

		u08 Data[];
	};
};

struct wayland_interface {
	u32 Id;
	u32 Version;
	u32 Size;

	wayland_prototype *Prototype;

	vptr Events[];
};

struct wayland_message {
	u32 MessageSize;
	u32 ControlSize;

	u32			*MessageData;
	sys_cmsghdr *ControlData;
};

struct wayland_method {
	c08				   *Name;
	c08				   *Format;
	wayland_prototype **Prototypes;
};

struct wayland_prepared_method {
	wayland_method Method;

	wayland_interface *Object;
	u16				   Opcode;
	u16				   FdCount;
	u16				   NewCount;
	u16				   ParamCount;

	u32	 VersionSince;
	u32	 DeprecatedSince;
	c08 *Format;

	wayland_param *Params;
};

struct wayland_event {
	wayland_prepared_method Method;
	wayland_message			Message;
};

struct wayland_prototype {
	c08 *Name;

	u32 Version;
	u16 RequestCount;
	u16 EventCount;

	wayland_method *Requests;
	wayland_method *Events;
};

typedef struct wayland_api_id_entry {
	struct wayland_api_id_entry *Next;

	u32 BaseId;
	u32 Count;
} wayland_api_id_entry;

typedef struct wayland_message_queue_entry {
	struct wayland_message_queue_entry *Next;

	wayland_prepared_method PreparedMethod;
	u32					   *Data;
} wayland_message_queue_entry;

typedef struct wayland_fd_queue_entry {
	struct wayland_fd_queue_entry *Next;

	s32 Fd;
} wayland_fd_queue_entry;

typedef struct wayland_message_queue {
	usize Count;

	wayland_message_queue_entry *Front;
	wayland_message_queue_entry *Back;
} wayland_message_queue;

typedef struct wayland_fd_queue {
	usize Count;

	wayland_fd_queue_entry *Front;
	wayland_fd_queue_entry *Back;
} wayland_fd_queue;

typedef struct wayland_api_state {
	heap *Heap;
	usize HeapSize;

	s32 Socket;
	u32 Lock;

	b08 Attempted;
	b08 Connected;

	wayland_api_id_entry *NextIdEntry;

	hashmap IdTable;

	wayland_message_queue MessageQueue;
	wayland_fd_queue	  FdQueue;
} wayland_api_state;

#endif

#ifndef SECTION_FUNCS

#define WAYLAND_API_FUNCS \
	INTERN(void,             Wayland_FreeObjectId, u32 ObjectId) \
	INTERN(wayland_display*, Wayland_GetDisplay,   void) \
	\
	INTERN(b08,  Wayland_IsConnected, void) \
	INTERN(void, Wayland_Disconnect,  void) \
	INTERN(b08,  Wayland_Connect,     void) \
	\
	INTERN(b08,  Wayland_PollEventQueue, s32 Timeout, wayland_event *Event) \
	INTERN(void, Wayland_DispatchEvent,  wayland_event Event) \
	INTERN(void, Wayland_DestroyEvent,   wayland_event Event) \
	//

#endif

#endif

#ifdef _LINUX
#ifdef INCLUDE_SOURCE

#ifndef SECTION_OBJECT_MANAGEMENT

extern wayland_prototype WaylandDisplayPrototype;
extern wayland_prototype WaylandRegistryPrototype;
extern wayland_prototype WaylandCallbackPrototype;
extern wayland_prototype WaylandCompositorPrototype;
extern wayland_prototype WaylandShmPoolPrototype;
extern wayland_prototype WaylandShmPrototype;
extern wayland_prototype WaylandBufferPrototype;
extern wayland_prototype WaylandDataOfferPrototype;
extern wayland_prototype WaylandDataSourcePrototype;
extern wayland_prototype WaylandDataDevicePrototype;
extern wayland_prototype WaylandDataDeviceManagerPrototype;
extern wayland_prototype WaylandShellPrototype;
extern wayland_prototype WaylandShellSurfacePrototype;
extern wayland_prototype WaylandSurfacePrototype;
extern wayland_prototype WaylandSeatPrototype;
extern wayland_prototype WaylandPointerPrototype;
extern wayland_prototype WaylandKeyboardPrototype;
extern wayland_prototype WaylandTouchPrototype;
extern wayland_prototype WaylandOutputPrototype;
extern wayland_prototype WaylandRegionPrototype;
extern wayland_prototype WaylandSubcompositorPrototype;
extern wayland_prototype WaylandSubsurfacePrototype;
extern wayland_prototype WaylandFixesPrototype;
extern wayland_prototype WaylandXdgWmBasePrototype;
extern wayland_prototype WaylandXdgPositionerPrototype;
extern wayland_prototype WaylandXdgSurfacePrototype;
extern wayland_prototype WaylandXdgToplevelPrototype;
extern wayland_prototype WaylandXdgPopupPrototype;
extern wayland_prototype WaylandDrmPrototype;
extern wayland_prototype WaylandZwpLinuxDmabufV1Prototype;
extern wayland_prototype WaylandZwpLinuxBufferParamsV1Prototype;
extern wayland_prototype WaylandZwpLinuxDmabufFeedbackV1Prototype;

wayland_prototype *WaylandPrototypes[] = {
	&WaylandBufferPrototype,
	&WaylandCallbackPrototype,
	&WaylandCompositorPrototype,
	&WaylandDataDevicePrototype,
	&WaylandDataDeviceManagerPrototype,
	&WaylandDataOfferPrototype,
	&WaylandDataSourcePrototype,
	&WaylandDisplayPrototype,
	&WaylandDrmPrototype,
	&WaylandFixesPrototype,
	&WaylandKeyboardPrototype,
	&WaylandOutputPrototype,
	&WaylandPointerPrototype,
	&WaylandRegionPrototype,
	&WaylandRegistryPrototype,
	&WaylandSeatPrototype,
	&WaylandShellPrototype,
	&WaylandShellSurfacePrototype,
	&WaylandShmPrototype,
	&WaylandShmPoolPrototype,
	&WaylandSubcompositorPrototype,
	&WaylandSubsurfacePrototype,
	&WaylandSurfacePrototype,
	&WaylandTouchPrototype,
	&WaylandXdgPopupPrototype,
	&WaylandXdgPositionerPrototype,
	&WaylandXdgSurfacePrototype,
	&WaylandXdgToplevelPrototype,
	&WaylandXdgWmBasePrototype,
	&WaylandZwpLinuxBufferParamsV1Prototype,
	&WaylandZwpLinuxDmabufFeedbackV1Prototype,
	&WaylandZwpLinuxDmabufV1Prototype,
};

internal wayland_prototype *
Wayland_FindPrototype(c08 *Name)
{
	usize  Start  = 0;
	usize  End	  = sizeof(WaylandPrototypes) / sizeof(wayland_prototype *);
	usize  Index  = 0;
	string Target = CString(Name);

	wayland_prototype *Curr = NULL;

	while (Start != End) {
		Index = Start + (End - Start) / 2;
		Curr  = WaylandPrototypes[Index];

		s32 Cmp = String_Cmp(Target, CString(Curr->Name));

		if (Cmp > 0) Start = Index + 1;
		else if (Cmp < 0) End = Index;
		else break;
	}

	return Start == End ? NULL : Curr;
}

internal u32
Wayland_AllocateObjectId(void)
{
	wayland_api_id_entry *Entry = _G.WaylandApi.NextIdEntry;
	if (!Entry || !Entry->Count) return 0;

	u32 Id		   = Entry->BaseId;
	Entry->BaseId += 1;
	Entry->Count  -= 1;

	if (Entry->Count == 0) {
		_G.WaylandApi.NextIdEntry = Entry->Next;
		Heap_FreeA(Entry);
	}

	return Id;
}

internal void
Wayland_FreeObjectId(u32 ObjectId)
{
	Platform_LockMutex(&_G.WaylandApi.Lock);

	// Store a pointer to the entry pointer to allow inserting a new record
	// after the previous.
	wayland_api_id_entry **Entry = &_G.WaylandApi.NextIdEntry;

	// Find the appropriate location to insert
	while (*Entry && (*Entry)->BaseId + (*Entry)->Count < ObjectId)
		Entry = &(*Entry)->Next;

	// Add a new record after prev
	if (!*Entry || ObjectId < (*Entry)->BaseId - 1) {
		wayland_api_id_entry *NewEntry =
			Heap_AllocateA(_G.WaylandApi.Heap, sizeof(wayland_api_id_entry));

		NewEntry->BaseId = ObjectId;
		NewEntry->Count	 = 1;
		NewEntry->Next	 = *Entry;

		*Entry = NewEntry;

		Platform_UnlockMutex(&_G.WaylandApi.Lock);
		return;
	}

	// Expand this record right and potentially merge the next
	if ((*Entry)->BaseId + (*Entry)->Count == ObjectId) {
		(*Entry)->Count += 1;

		wayland_api_id_entry *Next = (*Entry)->Next;
		if (Next && Next->BaseId == ObjectId + 1) {
			(*Entry)->Count += Next->Count;
			(*Entry)->Next	 = Next->Next;
			Heap_FreeA(Next);
		}

		Platform_UnlockMutex(&_G.WaylandApi.Lock);
		return;
	}

	// Expand this record left. We don't need to try to merge, since if it was
	// possible, we already would've above.
	if ((*Entry)->BaseId == ObjectId + 1) {
		(*Entry)->BaseId -= 1;
		(*Entry)->Count	 += 1;

		Platform_UnlockMutex(&_G.WaylandApi.Lock);
		return;
	}

	// Shouldn't get here
	Assert(FALSE);
}

internal vptr
Wayland_GetObject(u32 ObjectId)
{
	wayland_interface *Object = NULL;
	HashMap_Get(&_G.WaylandApi.IdTable, &ObjectId, &Object);
	return Object;
}

internal vptr
Wayland_CreateObject(wayland_prototype *Prototype, u32 ObjectId, u32 Version)
{
	usize InterfaceSize =
		sizeof(wayland_interface) + sizeof(vptr) * Prototype->EventCount;
	wayland_interface *Object =
		Heap_AllocateA(_G.WaylandApi.Heap, InterfaceSize);

	if (Prototype->Version < Version) {
		FPrintL(
			"Warning: Attempted to create %s-v%d, but its maximum version is "
			"v%d. Defaulting to v%d\n",
			Prototype->Name,
			Version,
			Prototype->Version,
			Prototype->Version
		);
		Version = Prototype->Version;
	}

	Mem_Set(Object, 0, InterfaceSize);
	Object->Id		  = ObjectId;
	Object->Version	  = Version;
	Object->Size	  = InterfaceSize;
	Object->Prototype = Prototype;

	HashMap_Add(&_G.WaylandApi.IdTable, &ObjectId, &Object);
	return Object;
}

internal void
Wayland_DestroyObject(vptr Object)
{
	wayland_interface *Interface = Object;
	if (!Interface->Id) return;

	HashMap_Remove(&_G.WaylandApi.IdTable, &Interface->Id, NULL, NULL);

	Mem_Set(Interface, 0, Interface->Size);
	Heap_FreeA(Interface);
}

internal wayland_display *
Wayland_GetDisplay()
{
	Platform_LockMutex(&_G.WaylandApi.Lock);
	wayland_display *Display = Wayland_GetObject(WAYLAND_DISPLAY_ID);

	if (!Display)
		Display = Wayland_CreateObject(
			&WaylandDisplayPrototype,
			WAYLAND_DISPLAY_ID,
			1
		);

	Platform_UnlockMutex(&_G.WaylandApi.Lock);
	return Display;
}

#endif

#ifndef SECTION_SESSION_MANAGEMENT

internal b08
Wayland_IsConnected(void)
{
	Platform_LockMutex(&_G.WaylandApi.Lock);
	b08 IsConnected = _G.WaylandApi.Connected;
	Platform_UnlockMutex(&_G.WaylandApi.Lock);
	return IsConnected;
}

internal void
Wayland_Disconnect(void)
{
	Platform_LockMutex(&_G.WaylandApi.Lock);

	if (_G.WaylandApi.Connected) {
		Sys_Close(_G.WaylandApi.Socket);
		_G.WaylandApi.Connected = FALSE;
	}

	// TODO: Clean up file descriptors, queues, etc.
	Platform_UnlockMutex(&_G.WaylandApi.Lock);
}

internal b08
Wayland_ConnectToSocket(string Name)
{
	if (_G.WaylandApi.Connected) return TRUE;
	if (Name.Length >= sizeof((sys_sockaddr_unix) {}.Data)) return FALSE;

	s32 FileDescriptor =
		Sys_Socket(SYS_ADDRESS_FAMILY_UNIX, SYS_SOCKET_STREAM, 0);
	if (FileDescriptor < 0) return FALSE;

	sys_sockaddr_unix Address;
	Address.Family = SYS_ADDRESS_FAMILY_UNIX;
	Mem_Cpy(Address.Data, Name.Text, Name.Length);
	Address.Data[Name.Length] = 0;

	s32 Result = Sys_Connect(
		FileDescriptor,
		(sys_sockaddr *) &Address,
		sizeof(sys_sockaddr_unix)
	);
	if (Result < 0) {
		Sys_Close(FileDescriptor);
		return FALSE;
	}

	_G.WaylandApi.Socket	= FileDescriptor;
	_G.WaylandApi.Connected = TRUE;
	return TRUE;
}

internal b08
Wayland_Connect(void)
{
	Platform_LockMutex(&_G.WaylandApi.Lock);

	if (_G.WaylandApi.Attempted) goto end;
	_G.WaylandApi.Attempted = TRUE;

	usize HeapSize		   = 16 * 1024 * 1024;
	vptr  HeapBase		   = Platform_AllocateMemory(HeapSize);
	heap *Heap			   = Heap_Init(HeapBase, HeapSize);
	_G.WaylandApi.HeapSize = HeapSize;
	_G.WaylandApi.Heap	   = Heap;

	_G.WaylandApi.IdTable =
		HashMap_Init(Heap, sizeof(u32), sizeof(wayland_interface *));

	s32	   FileDescriptor;
	string WaylandSocket = Platform_GetEnvParam(CStringL("WAYLAND_SOCKET"));
	if (String_TryParseS32(WaylandSocket, &FileDescriptor)) {
		_G.WaylandApi.Socket	= FileDescriptor;
		_G.WaylandApi.Connected = TRUE;
	} else {
		string XdgRuntimeDir =
			Platform_GetEnvParam(CStringL("XDG_RUNTIME_DIR"));
		string WaylandDisplay =
			Platform_GetEnvParam(CStringL("WAYLAND_DISPLAY"));

		if (WaylandDisplay.Length) {
			string SocketPath =
				FStringL("%s/%s", XdgRuntimeDir, WaylandDisplay);

			if (!Wayland_ConnectToSocket(SocketPath)) {
				SocketPath = FStringL("%s/wayland-0", XdgRuntimeDir);
				Wayland_ConnectToSocket(SocketPath);
			}
		}
	}

	if (_G.WaylandApi.Connected) {
		_G.WaylandApi.NextIdEntry =
			Heap_AllocateA(Heap, sizeof(wayland_api_id_entry));
		_G.WaylandApi.NextIdEntry->BaseId = 2;
		_G.WaylandApi.NextIdEntry->Count  = 0xFEFFFFFF;
		_G.WaylandApi.NextIdEntry->Next	  = NULL;
	}

end:
	b08 Success = _G.WaylandApi.Connected;
	Platform_UnlockMutex(&_G.WaylandApi.Lock);
	return Success;
}

#endif

#ifndef SECTION_MESSAGE_SERIALIZATION

internal b08
Wayland_WaitUntilCanSend(s32 Timeout)
{
	if (!_G.WaylandApi.Connected) return FALSE;

	sys_pollfd PollFd = { .FileDescriptor  = _G.WaylandApi.Socket,
						  .RequestedEvents = SYS_POLLOUT,
						  .ReturnedEvents  = 0 };

	s32 Result = Sys_Poll(&PollFd, 1, Timeout);
	return Result == 1 && (PollFd.ReturnedEvents & SYS_POLLOUT);
}

internal b08
Wayland_SendMessage(wayland_message Message)
{
	sys_iovec IOVector = { 0 };
	IOVector.Base	   = Message.MessageData;
	IOVector.Length	   = Message.MessageSize;

	sys_msghdr MessageHeader	= { 0 };
	MessageHeader.IOVectors		= &IOVector;
	MessageHeader.IOVectorCount = 1;
	MessageHeader.Control		= Message.ControlData;
	MessageHeader.ControlSize	= Message.ControlSize;

	ssize BytesWritten =
		Sys_SendMsg(_G.WaylandApi.Socket, &MessageHeader, SYS_MSG_NOSIGNAL);

	if (BytesWritten < 0) return FALSE;
	Assert(BytesWritten == Message.MessageSize);
	return TRUE;
}

internal b08
Wayland_WaitUntilCanReceive(s32 Timeout)
{
	if (!_G.WaylandApi.Connected) return FALSE;

	sys_pollfd PollFd = { .FileDescriptor  = _G.WaylandApi.Socket,
						  .RequestedEvents = SYS_POLLIN,
						  .ReturnedEvents  = 0 };

	s32 Result = Sys_Poll(&PollFd, 1, Timeout);
	return Result == 1 && (PollFd.ReturnedEvents & SYS_POLLIN);
}

internal void
Wayland_DestroyMessage(wayland_message Message)
{
	if (Message.MessageData) Heap_FreeA(Message.MessageData);
	if (Message.ControlData) Heap_FreeA(Message.ControlData);
}

internal string
Wayland_GetObjectName(wayland_interface *Object)
{
	if (!Object) return CStringL("null");
	return FStringL(
		"%s-v%d#%d",
		CString(Object->Prototype->Name),
		Object->Version,
		Object->Id
	);
}

internal void
Wayland_LogMessage(
	wayland_interface *Object,
	c08				  *MethodName,
	c08				  *Message,
	...
)
{
	string Descriptor = EString();
	if (Object) {
		string ObjectName = Wayland_GetObjectName(Object);

		Descriptor = FStringL(
			" [%s%s%s]",
			ObjectName,
			MethodName ? CStringL("::") : EString(),
			CString(MethodName)
		);
	}

	va_list Args;
	VA_Start(Args, Message);
	string FormattedMessage = FVString(CString(Message), Args);
	VA_End(Args);

	FPrintL("[WaylandApi]%s %s\n", Descriptor, FormattedMessage);
}

internal wayland_prepared_method
Wayland_PrepareMethod(wayland_interface *Object, u16 Opcode, b08 IsEvent)
{
	wayland_prepared_method PreparedMethod = { 0 };

	PreparedMethod.Object = Object;
	PreparedMethod.Opcode = Opcode;

	PreparedMethod.Method = IsEvent ? Object->Prototype->Events[Opcode]
									: Object->Prototype->Requests[Opcode];
	wayland_method Method = PreparedMethod.Method;

	c08 *C = Method.Format;

	while (*C >= '0' && *C <= '9') {
		PreparedMethod.VersionSince *= 10;
		PreparedMethod.VersionSince += *C - '0';
		C++;
	}
	if (!PreparedMethod.VersionSince) PreparedMethod.VersionSince = 1;

	if (*C == '_') {
		C++;
		while (*C >= '0' && *C <= '9') {
			PreparedMethod.DeprecatedSince *= 10;
			PreparedMethod.DeprecatedSince += *C - '0';
			C++;
		}
	}

	PreparedMethod.Format = C;
	Assert(
		*C == 'd' || *C == 'o',
		"The first format param must be a not-null object!"
	);

	usize PI = 0, CI = 0;
	for (C = PreparedMethod.Format; *C; C++) {
		if (*C == '?') C++;
		if (*C == 'f') PreparedMethod.FdCount++;
		else if (*C == 'n') PreparedMethod.NewCount++;
		PreparedMethod.ParamCount++;
	}

	usize ParamsSize	  = PreparedMethod.ParamCount * sizeof(wayland_param);
	PreparedMethod.Params = Heap_AllocateA(_G.WaylandApi.Heap, ParamsSize);
	wayland_param *Params = PreparedMethod.Params;
	Mem_Set(Params, 0, ParamsSize);

	PI = 0, CI = 0;
	for (c08 *C = PreparedMethod.Format; *C; C++) {
		if (*C == '?') {
			Params[PI].Nullable = TRUE;
			C++;
		}

		switch (*C) {
			case 'i':
				Params[PI].Type = WAYLAND_PARAM_SINT;
				Params[PI].Size = sizeof(s32);
				break;
			case 'u':
				Params[PI].Type = WAYLAND_PARAM_UINT;
				Params[PI].Size = sizeof(u32);
				break;
			case 'r':
				Params[PI].Type = WAYLAND_PARAM_FIXED;
				Params[PI].Size = sizeof(wayland_fixed);
				break;
			case 's':
				Params[PI].Type = WAYLAND_PARAM_STRING;
				Params[PI].Size = sizeof(c08 *);
				break;
			case 'a':
				Params[PI].Type = WAYLAND_PARAM_ARRAY;
				Params[PI].Size = sizeof(wayland_array);
				break;
			case 'd':
			case 'o':
				Params[PI].Type				 = WAYLAND_PARAM_OBJECT;
				Params[PI].Size				 = sizeof(wayland_interface *);
				Params[PI].Object.Destructor = *C == 'd';
				break;
			case 'n':
				Params[PI].Type				   = WAYLAND_PARAM_NEWOBJECT;
				Params[PI].Size				   = sizeof(wayland_interface *);
				Params[PI].NewObject.Prototype = Method.Prototypes[CI++];
				break;
			case 'f':
				Params[PI].Type = WAYLAND_PARAM_FD;
				Params[PI].Size = sizeof(s32);
				break;
			default:
				Wayland_LogMessage(
					Object,
					Method.Name,
					"Warning: Unknown param type %c (in format %s)",
					*C,
					CString(Method.Format)
				);
		}

		b08 CanBeNullable = Params[PI].Type == WAYLAND_PARAM_OBJECT
						 || Params[PI].Type == WAYLAND_PARAM_STRING;
		if (Params[PI].Nullable && !CanBeNullable)
			Wayland_LogMessage(
				Object,
				Method.Name,
				"Warning: Param type %c cannot be null (in format %s)",
				*C,
				CString(Method.Format)
			);

		PI++;
	}

	if (PreparedMethod.VersionSince > Object->Version)
		Wayland_LogMessage(
			Object,
			Method.Name,
			"Warning: Method is not introduced until v%d",
			PreparedMethod.VersionSince
		);

	b08 IsDeprecated = PreparedMethod.DeprecatedSince
					&& PreparedMethod.DeprecatedSince <= Object->Version;
	if (IsDeprecated)
		Wayland_LogMessage(
			Object,
			Method.Name,
			"Warning: Method has been deprecated since v%d",
			PreparedMethod.DeprecatedSince
		);

	return PreparedMethod;
}

internal void
Wayland_FillRequestParams(wayland_prepared_method PreparedMethod, va_list Args)
{
	wayland_interface *Object			 = PreparedMethod.Object;
	PreparedMethod.Params[0].Object.Data = Object;

	for (usize I = 1; I < PreparedMethod.ParamCount; I++) {
		wayland_param *Param = &PreparedMethod.Params[I];
		switch (Param->Type) {
			case WAYLAND_PARAM_SINT: Param->Sint = VA_Next(Args, s32); break;
			case WAYLAND_PARAM_UINT: Param->Uint = VA_Next(Args, u32); break;
			case WAYLAND_PARAM_FIXED:
				Param->Fixed = VA_Next(Args, wayland_fixed);
				break;
			case WAYLAND_PARAM_STRING:
				Param->String = VA_Next(Args, c08 *);
				if (!Param->String && !Param->Nullable)
					Wayland_LogMessage(
						Object,
						PreparedMethod.Method.Name,
						"Warning: Provided a null string for non-nullable "
						"param %d",
						I + 1
					);
				break;
			case WAYLAND_PARAM_ARRAY:
				Param->Array = VA_Next(Args, wayland_array);
				break;
			case WAYLAND_PARAM_OBJECT:
				Param->Object.Data = VA_Next(Args, wayland_interface *);
				if (!Param->Object.Data && !Param->Nullable)
					Wayland_LogMessage(
						Object,
						PreparedMethod.Method.Name,
						"Warning: Provided a null object for "
						"non-nullable param %d",
						I
					);
				break;
			case WAYLAND_PARAM_NEWOBJECT:
				u32 ObjectId = Wayland_AllocateObjectId();

				wayland_prototype *Prototype = Param->NewObject.Prototype;
				if (Prototype) {
					u32 Version = Object->Version;
					Param->NewObject.Data =
						Wayland_CreateObject(Prototype, ObjectId, Version);
				} else {
					c08 *Name	 = VA_Next(Args, c08 *);
					u32	 Version = VA_Next(Args, u32);
					Prototype	 = Wayland_FindPrototype(Name);
					Assert(
						Prototype,
						"Failed to find prototype for constructed object"
					);
					Assert(
						String_Cmp(CString(Prototype->Name), CString(Name))
							== 0,
						"Wayland prototypes must be in alphabetical order!"
					);
					Param->NewObject.Data =
						Wayland_CreateObject(Prototype, ObjectId, Version);
				}
				break;
			case WAYLAND_PARAM_FD: Param->Fd = VA_Next(Args, s32); break;
			default				 : Assert(FALSE);
		}
	}
}

internal void
Wayland_GetConstructedParams(
	wayland_prepared_method PreparedMethod,
	wayland_interface	  **ConstructedOut
)
{
	usize CI = 0;
	for (usize I = 0; I < PreparedMethod.ParamCount; I++) {
		wayland_param *Param = &PreparedMethod.Params[I];
		if (Param->Type == WAYLAND_PARAM_NEWOBJECT)
			ConstructedOut[CI++] = Param->NewObject.Data;
	}
}

internal void
Wayland_DestroyDestructorParams(wayland_prepared_method PreparedMethod)
{
	for (usize I = 0; I < PreparedMethod.ParamCount; I++) {
		wayland_param *Param = &PreparedMethod.Params[I];
		if (Param->Type == WAYLAND_PARAM_OBJECT && Param->Object.Destructor) {
			wayland_interface *Object = Param->Object.Data;
			if (Object) Wayland_DestroyObject(Object);
		}
	}
}

internal void
Wayland_DestroyPreparedMethod(wayland_prepared_method PreparedMethod)
{
	Heap_FreeA(PreparedMethod.Params);
}

internal void
Wayland_SerializeParam(
	wayland_message Message,
	usize		   *WordIndex,
	usize		   *FdIndex,
	wayland_param	Param
)
{
	u32	 *Words = (vptr) Message.MessageData;
	s32	 *Fds = Message.ControlData ? (vptr) &Message.ControlData->Data : NULL;
	usize WI  = *WordIndex;
	usize FI  = *FdIndex;

	switch (Param.Type) {
		case WAYLAND_PARAM_SINT:
			if (Words) *(s32 *) &Words[WI] = Param.Sint;
			WI++;
			break;
		case WAYLAND_PARAM_UINT:
			if (Words) Words[WI] = Param.Uint;
			WI++;
			break;
		case WAYLAND_PARAM_FIXED:
			if (Words) *(wayland_fixed *) &Words[WI] = Param.Fixed;
			WI++;
			break;
		case WAYLAND_PARAM_STRING:
			if (!Param.String) {
				if (Words) Words[WI] = 0;
				WI++;
			} else {
				string Str = CString(Param.String);
				if (Words) {
					Words[WI] = Str.Length + 1;
					Mem_Cpy(Words + WI + 1, Str.Text, Str.Length + 1);
				}
				WI += 1 + (Str.Length + 4) / 4;
			}
			break;
		case WAYLAND_PARAM_ARRAY:
			wayland_array Array = Param.Array;
			if (Words) {
				Words[WI] = Array.Size;
				Mem_Cpy(Words + WI + 1, Array.Data, Array.Size);
			}
			WI += 1 + (Array.Size + 3) / 4;
			break;
		case WAYLAND_PARAM_OBJECT:
			wayland_interface *Object = Param.Object.Data;
			if (Words) Words[WI] = Object ? Object->Id : 0;
			WI++;
			break;
		case WAYLAND_PARAM_NEWOBJECT:
			wayland_interface *NewObject = Param.NewObject.Data;
			if (!Param.NewObject.Prototype) {
				string Name		= CString(NewObject->Prototype->Name);
				u32	   Version	= NewObject->Version;
				usize  StrWords = 1 + (Name.Length + 4) / 4;
				if (Words) {
					Words[WI] = Name.Length + 1;
					Mem_Cpy(Words + WI + 1, Name.Text, Name.Length + 1);
					Words[WI + StrWords] = Version;
				}
				WI += StrWords + 1;
			}
			if (Words) Words[WI] = NewObject->Id;
			WI++;
			break;
		case WAYLAND_PARAM_FD:
			if (Fds) Fds[FI] = Param.Fd;
			FI++;
			break;
		default: Assert(FALSE);
	}

	*WordIndex = WI;
	*FdIndex   = FI;
}

internal wayland_message
Wayland_SerializeMethod(wayland_prepared_method PreparedMethod)
{
	wayland_message Message = { 0 };

	usize WI = 2, FI = 0;
	for (usize I = 1; I < PreparedMethod.ParamCount; I++)
		Wayland_SerializeParam(Message, &WI, &FI, PreparedMethod.Params[I]);

	Message.MessageSize = WI * sizeof(u32);
	Message.MessageData =
		Heap_AllocateA(_G.WaylandApi.Heap, Message.MessageSize);
	Message.MessageData[0] = PreparedMethod.Object->Id;
	Message.MessageData[1] =
		(Message.MessageSize << 16) | PreparedMethod.Opcode;

	if (FI) {
		Message.ControlSize = sizeof(sys_cmsghdr) + FI * sizeof(s32);
		Message.ControlData =
			Heap_AllocateA(_G.WaylandApi.Heap, Message.ControlSize);

		Message.ControlData->Length = Message.ControlSize;
		Message.ControlData->Level	= SYS_SOL_SOCKET;
		Message.ControlData->Type	= SYS_SCM_RIGHTS;
	}

	WI = 2, FI = 0;
	for (usize I = 1; I < PreparedMethod.ParamCount; I++)
		Wayland_SerializeParam(Message, &WI, &FI, PreparedMethod.Params[I]);

	return Message;
}

internal void
Wayland_DeserializeParam(
	u32					   *Words,
	usize				   *WordIndex,
	wayland_prepared_method PreparedMethod,
	wayland_param		   *Param,
	usize					ParamIndex
)
{
	wayland_interface *Object = PreparedMethod.Object;
	usize			   WI	  = *WordIndex;

	switch (Param->Type) {
		case WAYLAND_PARAM_SINT: Param->Sint = *(s32 *) &Words[WI++]; break;
		case WAYLAND_PARAM_UINT: Param->Uint = Words[WI++]; break;
		case WAYLAND_PARAM_FIXED:
			Param->Fixed = *(wayland_fixed *) &Words[WI++];
			break;
		case WAYLAND_PARAM_STRING:
			u32 Size	  = Words[WI++];
			Param->String = NULL;
			if (Size) {
				Param->String  = (c08 *) &Words[WI];
				WI			  += (Size + 3) / 4;
			}
			if (!Param->String && !Param->Nullable)
				Wayland_LogMessage(
					Object,
					PreparedMethod.Method.Name,
					"Warning: Received a null string for non-nullable param %d",
					ParamIndex
				);
			break;
		case WAYLAND_PARAM_ARRAY:
			Param->Array.Size  = Words[WI++];
			Param->Array.Data  = &Words[WI];
			WI				  += (Param->Array.Size + 3) / 4;
			break;
		case WAYLAND_PARAM_OBJECT:
			u32 ObjectId	   = Words[WI++];
			Param->Object.Data = ObjectId ? Wayland_GetObject(ObjectId) : NULL;
			if (!ObjectId && !Param->Nullable)
				Wayland_LogMessage(
					Object,
					PreparedMethod.Method.Name,
					"Warning: Received a null object for non-nullable param %d",
					ParamIndex
				);
			if (ObjectId && !Param->Object.Data)
				Wayland_LogMessage(
					Object,
					PreparedMethod.Method.Name,
					"Warning: Failed to lookup object #%d",
					ObjectId
				);
			break;
		case WAYLAND_PARAM_NEWOBJECT:
			u32 Version = PreparedMethod.Object->Version;

			wayland_prototype *Prototype = Param->NewObject.Prototype;
			if (!Prototype) {
				u32 NameSize = Words[WI++];
				Assert(NameSize);
				c08 *Name = (c08 *) &Words[WI];
				Prototype = Wayland_FindPrototype(Name);
				Assert(Prototype);
				WI		+= (NameSize + 3) / 4;
				Version	 = Words[WI++];
			}

			u32 NewObjectId = Words[WI++];
			Param->NewObject.Data =
				Wayland_CreateObject(Prototype, NewObjectId, Version);
			break;
		case WAYLAND_PARAM_FD: Param->Fd = -1; break;
		default				 : Assert(FALSE);
	}

	*WordIndex = WI;
}

internal wayland_prepared_method
Wayland_DeserializeMethod(u32 *Words)
{
	u32 ObjectId	= Words[0];
	u16 Opcode		= Words[1] & 0xFFFF;
	u16 MessageSize = Words[1] >> 16;

	wayland_interface *Object = Wayland_GetObject(ObjectId);
	Assert(Object);

	wayland_prepared_method PreparedMethod =
		Wayland_PrepareMethod(Object, Opcode, TRUE);

	wayland_param *Params = PreparedMethod.Params;
	Params[0].Object.Data = Object;

	usize WI = 2;
	for (usize I = 1; I < PreparedMethod.ParamCount; I++) {
		Wayland_DeserializeParam(
			Words,
			&WI,
			PreparedMethod,
			&PreparedMethod.Params[I],
			I
		);
	}

	return PreparedMethod;
}

internal void
Wayland_EnqueueMessage(
	heap				  *Heap,
	wayland_message_queue *MessageQueue,
	u32					  *Data
)
{
	u16 MessageSize = Data[1] >> 16;
	Assert(MessageSize % sizeof(u32) == 0);

	wayland_message_queue_entry *Entry =
		Heap_AllocateA(Heap, sizeof(wayland_message_queue_entry));

	Entry->Next = NULL;
	Entry->Data = Heap_AllocateA(Heap, MessageSize);
	Mem_Cpy(Entry->Data, Data, MessageSize);

	Entry->PreparedMethod = Wayland_DeserializeMethod(Entry->Data);

	if (MessageQueue->Back) {
		MessageQueue->Back->Next = Entry;
	} else {
		Assert(!MessageQueue->Front);
		MessageQueue->Front = Entry;
	}
	MessageQueue->Back = Entry;
	MessageQueue->Count++;
}

internal void
Wayland_EnqueueFd(heap *Heap, wayland_fd_queue *FdQueue, s32 Fd)
{
	wayland_fd_queue_entry *FdEntry =
		Heap_AllocateA(Heap, sizeof(wayland_fd_queue_entry));

	FdEntry->Next = NULL;
	FdEntry->Fd	  = Fd;

	if (FdQueue->Back) {
		FdQueue->Back->Next = FdEntry;
	} else {
		Assert(!FdQueue->Front);
		FdQueue->Front = FdEntry;
	}
	FdQueue->Back = FdEntry;
	FdQueue->Count++;
}

internal b08
Wayland_DequeueMessage(
	wayland_message_queue		*MessageQueue,
	wayland_message_queue_entry *EntryOut
)
{
	wayland_message_queue_entry *Entry = MessageQueue->Front;
	if (!Entry) return FALSE;

	MessageQueue->Front = Entry->Next;
	MessageQueue->Count--;
	if (!MessageQueue->Front) MessageQueue->Back = NULL;

	if (EntryOut) *EntryOut = *Entry;
	Heap_FreeA(Entry);
	return TRUE;
}

internal s32
Wayland_DequeueFd(wayland_fd_queue *FdQueue)
{
	wayland_fd_queue_entry *Entry = FdQueue->Front;
	if (!Entry) return -1;

	FdQueue->Front = Entry->Next;
	FdQueue->Count--;
	if (!FdQueue->Front) FdQueue->Back = NULL;

	s32 Fd = Entry->Fd;
	Heap_FreeA(Entry);
	return Fd;
}

internal b08
Wayland_PollSocket(
	heap				  *Heap,
	s32					   Socket,
	wayland_message_queue *MessageQueue,
	wayland_fd_queue	  *FdQueue,
	s32					   Timeout
)
{
	if (!Wayland_WaitUntilCanReceive(Timeout)) return FALSE;

	u32		  Header[2];
	sys_iovec IOVector = { 0 };
	IOVector.Base	   = (vptr) Header;
	IOVector.Length	   = sizeof(Header);

	sys_msghdr MessageHeader	= { 0 };
	MessageHeader.IOVectors		= &IOVector;
	MessageHeader.IOVectorCount = 1;
	ssize MessageSize = Sys_RecvMsg(Socket, &MessageHeader, SYS_MSG_PEEK);

	if (MessageSize < 0) return FALSE;
	Assert(MessageSize);

	usize ControlSize = 0;
	while (MessageHeader.Flags & SYS_MSG_CTRUNC) {
		if (ControlSize) {
			ControlSize *= 2;
			Heap_ResizeA(&MessageHeader.Control, ControlSize);
		} else {
			ControlSize			  = sizeof(sys_cmsghdr) + sizeof(s32) * 2;
			MessageHeader.Control = Heap_AllocateA(Heap, ControlSize);
		}
		MessageHeader.ControlSize = ControlSize;

		MessageSize = Sys_RecvMsg(Socket, &MessageHeader, SYS_MSG_PEEK);
		if (MessageSize < 0) {
			Heap_FreeA(MessageHeader.Control);
			return FALSE;
		}
		Assert(MessageSize);
	}

	IOVector.Length = Header[1] >> 16;
	IOVector.Base	= Heap_AllocateA(Heap, IOVector.Length);

	MessageSize = Sys_RecvMsg(Socket, &MessageHeader, 0);
	if (MessageSize < 0) {
		if (MessageHeader.Control) Heap_FreeA(MessageHeader.Control);
		Heap_FreeA(IOVector.Base);
		return FALSE;
	}
	Assert(MessageSize);
	Assert(!(MessageHeader.Flags & SYS_MSG_CTRUNC));

	Wayland_EnqueueMessage(Heap, MessageQueue, IOVector.Base);
	Heap_FreeA(IOVector.Base);
	if (!ControlSize) return TRUE;

	usize FdDataSize  = 0;
	vptr  ControlData = MessageHeader.Control;
	vptr  ControlEnd  = (vptr) ControlData + MessageHeader.ControlSize;
	usize Align		  = sizeof(usize) - 1;
	while (ControlData + sizeof(sys_cmsghdr) <= ControlEnd) {
		sys_cmsghdr *Control  = ControlData;
		ControlData			 += (Control->Length + Align) & ~Align;
		FdDataSize			 += Control->Length - sizeof(sys_cmsghdr);
	}

	Assert(FdDataSize % sizeof(s32) == 0);
	s32 *Fds = Heap_AllocateA(Heap, FdDataSize);

	ControlData	  = MessageHeader.Control;
	vptr FdCursor = Fds;
	while (ControlData + sizeof(sys_cmsghdr) <= ControlEnd) {
		sys_cmsghdr *Control = ControlData;
		usize		 Delta	 = Control->Length - sizeof(sys_cmsghdr);
		Mem_Cpy(FdCursor, &Control->Data, Delta);
		ControlData += (Control->Length + Align) & ~Align;
		FdCursor	+= Delta;
	}

	usize FdCount = FdDataSize / sizeof(s32);
	for (usize I = 0; I < FdCount; I++)
		Wayland_EnqueueFd(Heap, FdQueue, Fds[I]);

	Heap_FreeA(Fds);
	if (MessageHeader.Control) Heap_FreeA(MessageHeader.Control);
	return TRUE;
}

internal b08
Wayland_TryDequeueEvent(
	heap				  *Heap,
	wayland_message_queue *MessageQueue,
	wayland_fd_queue	  *FdQueue,
	wayland_event		  *EventOut
)
{
	if (!_G.WaylandApi.MessageQueue.Count) return FALSE;

	wayland_message_queue_entry *Entry = _G.WaylandApi.MessageQueue.Front;

	if (Entry->PreparedMethod.FdCount > FdQueue->Count) return FALSE;

	wayland_event Event		  = { 0 };
	Event.Method			  = Entry->PreparedMethod;
	Event.Message.MessageData = Entry->Data;
	Event.Message.MessageSize = Entry->Data[1] >> 16;
	Event.Message.ControlSize =
		sizeof(sys_cmsghdr) + Event.Method.FdCount * sizeof(s32);
	Wayland_DequeueMessage(&_G.WaylandApi.MessageQueue, NULL);

	if (Event.Message.ControlSize) {
		Event.Message.ControlData =
			Heap_AllocateA(Heap, Event.Message.ControlSize);
		Event.Message.ControlData->Length = Event.Message.ControlSize;

		s32 *Fds = (vptr) &Event.Message.ControlData->Data;

		usize FI = 0;
		for (usize I = 0; I < Event.Method.ParamCount; I++) {
			wayland_param *Param = &Event.Method.Params[I];
			if (Param->Type == WAYLAND_PARAM_FD) {
				s32 Fd	  = Wayland_DequeueFd(&_G.WaylandApi.FdQueue);
				Fds[FI++] = Fd;
				Param->Fd = Fd;
			}
		}
	}

	*EventOut = Event;
	return TRUE;
}

internal b08
Wayland_PollEventQueue(s32 Timeout, wayland_event *Event)
{
	Platform_LockMutex(&_G.WaylandApi.Lock);

	b08 Dequeued = FALSE;
	if (_G.WaylandApi.Connected) {
		Wayland_PollSocket(
			_G.WaylandApi.Heap,
			_G.WaylandApi.Socket,
			&_G.WaylandApi.MessageQueue,
			&_G.WaylandApi.FdQueue,
			Timeout
		);

		Dequeued = Wayland_TryDequeueEvent(
			_G.WaylandApi.Heap,
			&_G.WaylandApi.MessageQueue,
			&_G.WaylandApi.FdQueue,
			Event
		);
	}

	Platform_UnlockMutex(&_G.WaylandApi.Lock);
	return Dequeued;
}

internal void
Wayland_DispatchEvent(wayland_event Event)
{
	Platform_LockMutex(&_G.WaylandApi.Lock);
	vptr Proc = Event.Method.Object->Events[Event.Method.Opcode];
	if (!Proc) {
		Platform_UnlockMutex(&_G.WaylandApi.Lock);
		return;
	}

#if defined(_X64)
	usize StackSize = 0;
	for (usize I = 0; I < Event.Method.ParamCount; I++) {
		wayland_param Param	 = Event.Method.Params[I];
		StackSize			+= (Param.Size + 7) & ~7;
	}
	usize StackWords = StackSize / 8;

	// Leave 8-byte extra to have 16-byte alignment later
	if (StackWords % 2 == 0) {
		StackSize += 8;
		StackWords++;
	}

	u64	  Regs[6];
	usize RI = 0;

	usize *Stack	   = Heap_AllocateA(_G.WaylandApi.Heap, StackSize);
	vptr   StackCursor = Stack;
	for (usize I = 0; I < Event.Method.ParamCount; I++) {
		wayland_param Param		= Event.Method.Params[I];
		usize		  ParamSize = (Param.Size + 7) & ~7;

		usize ParamWords = ParamSize / 8;
		if (ParamWords <= 6 - RI) {
			Mem_Cpy(Regs + RI, &Param.Data, ParamSize);
			RI		  += ParamWords;
			StackSize -= ParamSize;
		} else {
			Mem_Cpy(StackCursor, &Param.Data, Param.Size);
			StackCursor += ParamSize;
		}
	}

	Platform_UnlockMutex(&_G.WaylandApi.Lock);

	__asm__ volatile(
		"push %%rbx				\n"
		"mov %%rcx, %%rbx		\n"
		"sub %%rcx, %%rsp		\n"
		"shr $3, %%rcx			\n"
		"mov %%rsp, %%rdi		\n"
		"rep movsq				\n"
		"mov 0(%%rdx), %%rdi	\n"
		"mov 8(%%rdx), %%rsi	\n"
		"mov 24(%%rdx), %%rcx	\n"
		"mov 32(%%rdx), %%r8	\n"
		"mov 40(%%rdx), %%r9	\n"
		"mov 16(%%rdx), %%rdx	\n"
		"call *%%rax			\n"
		"add %%rbx, %%rsp		\n"
		"pop %%rbx				\n"
		:
		: "a"(Proc), "c"(StackSize), "d"(Regs), "S"(Stack)
	);

	Heap_FreeA(Stack);
#else
#error Unsupported Architecture
#endif
}

internal void
Wayland_DestroyEvent(wayland_event Event)
{
	Wayland_DestroyPreparedMethod(Event.Method);
	Wayland_DestroyMessage(Event.Message);
}

internal void
Wayland_HandleRequestV(
	wayland_interface  *Object,
	u16					Opcode,
	wayland_interface **Constructed,
	va_list				Args
)
{
	Platform_LockMutex(&_G.WaylandApi.Lock);

	wayland_prepared_method PreparedMethod =
		Wayland_PrepareMethod(Object, Opcode, FALSE);

	Wayland_FillRequestParams(PreparedMethod, Args);
	Wayland_GetConstructedParams(PreparedMethod, Constructed);

	wayland_message Message = Wayland_SerializeMethod(PreparedMethod);

	b08 CanSend = Wayland_WaitUntilCanSend(-1);
	b08 Sent	= FALSE;
	if (CanSend) Sent = Wayland_SendMessage(Message);

	Wayland_DestroyMessage(Message);

	if (Sent) Wayland_DestroyDestructorParams(PreparedMethod);
	Wayland_DestroyPreparedMethod(PreparedMethod);

	Platform_UnlockMutex(&_G.WaylandApi.Lock);
}

internal void
Wayland_HandleMultiConstructorRequest(
	wayland_interface  *Object,
	u16					Opcode,
	wayland_interface **Constructed,
	...
)
{
	va_list Args;
	VA_Start(Args, Opcode);
	Wayland_HandleRequestV(Object, Opcode, Constructed, Args);
	VA_End(Args);
}

internal vptr
Wayland_HandleConstructorRequest(wayland_interface *Object, u16 Opcode, ...)
{
	wayland_interface *Constructed;

	va_list Args;
	VA_Start(Args, Opcode);
	Wayland_HandleRequestV(Object, Opcode, &Constructed, Args);
	VA_End(Args);

	return Constructed;
}

internal void
Wayland_HandleRequest(wayland_interface *Object, u16 Opcode, ...)
{
	va_list Args;
	VA_Start(Args, Opcode);
	Wayland_HandleRequestV(Object, Opcode, NULL, Args);
	VA_End(Args);
}

#endif

#endif
#endif
