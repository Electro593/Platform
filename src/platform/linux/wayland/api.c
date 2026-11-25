/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                             *
*  Author: Aria Seiler                                                        *
*                                                                             *
*  This program is in the public domain. There is no implied warranty, so use *
*  it at your own risk.                                                       *
*                                                                             *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef INCLUDE_HEADER

#define WAYLAND_DISPLAY_ID 1

#define WAYLAND_INTERFACE vptr

#ifndef SECTION_TYPES

typedef u32 wayland_fixed;

typedef struct wayland_array		   wayland_array;
typedef struct wayland_param		   wayland_param;
typedef struct wayland_event_queue	   wayland_event_queue;
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

struct wayland_event_queue {
	array_deque	 Deque;
	mutex_handle Lock;
	usize		 RefCount;
};

struct wayland_interface {
	u32 Id;
	u16 Version;
	u16 Size;

	wayland_prototype *Prototype;

	mutex_handle		 LinkLock;
	wayland_event_queue *LinkedEventQueue;
	wayland_event_queue	 InternalEventQueue;

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

	u32	 SinceVersion;
	u32	 UntilVersion;
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

	mutex_handle SocketLock;
	s32			 Socket;
	b08			 Connected;

	mutex_handle		  IdLock;
	hashmap				  IdTable;
	wayland_api_id_entry *NextIdEntry;

	wayland_message_queue MessageQueue;
	wayland_fd_queue	  FdQueue;
} wayland_api_state;

#endif

#ifndef SECTION_FUNCS

#define WAYLAND_API_FUNCS \
	INTERN(r32,           Wayland_FixedToR32,     wayland_fixed Fixed) \
	INTERN(wayland_fixed, Wayland_R32ToFixed,     r32 Value) \
	INTERN(wayland_array, Wayland_CopyArray,      heap *Heap, wayland_array Array) \
	INTERN(void,          Wayland_FreeArray,      wayland_array Array) \
	INTERN(string,        Wayland_GetDisplayName, WAYLAND_INTERFACE Object) \
	INTERN(u16,           Wayland_GetVersion,     WAYLAND_INTERFACE Object) \
	\
	INTERN(void, Wayland_DestroyClientObject, wayland_interface *Interface) \
	\
	INTERN(b08,                Wayland_IsConnected, void) \
	INTERN(void,               Wayland_Disconnect,  void) \
	INTERN(b08,                Wayland_Connect,     void) \
	INTERN(wayland_display*,   Wayland_GetDisplay,  void) \
	\
	INTERN(wayland_event_queue, Wayland_CreateEventQueue, void) \
	INTERN(void,                Wayland_DestroyEventQueue, wayland_event_queue *EventQueue) \
	INTERN(b08,                 Wayland_DequeueEvent,     wayland_event_queue *EventQueue, wayland_event *EventOut) \
	INTERN(void,                Wayland_LinkEventQueue,   wayland_interface *Object, wayland_event_queue *EventQueue) \
	\
	INTERN(b08,  Wayland_PollConnection, s32 Timeout) \
	INTERN(void, Wayland_DispatchEvent,  wayland_event Event) \
	INTERN(void, Wayland_DestroyEvent,   wayland_event Event) \
	//

#endif

#endif

#ifdef _LINUX
#ifdef INCLUDE_SOURCE

#ifndef SECTION_UTILITIES

internal r32
Wayland_FixedToR32(wayland_fixed Fixed)
{ return (r32) Fixed / 256.0f; }

internal wayland_fixed
Wayland_R32ToFixed(r32 Value)
{ return (wayland_fixed) (Value * 256.0f); }

internal wayland_array
Wayland_CopyArray(heap *Heap, wayland_array Array)
{
	return (wayland_array){
		.Size = Array.Size,
		.Data = Heap_AllocateA(Heap, Array.Size),
	};
}

internal void
Wayland_FreeArray(wayland_array Array)
{ Heap_FreeA(Array.Data); }

internal string
Wayland_GetDisplayName(WAYLAND_INTERFACE Object)
{
	if (!Object) return CStringL("null");
	wayland_interface *Interface = Object;
	return FStringL(
		"%s-v%u#%#x",
		CString(Interface->Prototype->Name),
		Interface->Version,
		Interface->Id
	);
}

internal u16
Wayland_GetVersion(WAYLAND_INTERFACE Object)
{
	if (!Object) return 0;
	return ((wayland_interface *) Object)->Version;
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
		string ObjectName = Wayland_GetDisplayName(Object);

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

	FPrintL(
		"[%d] [WaylandApi]%s %s\n",
		Sys_GetTid(),
		Descriptor,
		FormattedMessage
	);
}

#endif

#ifndef SECTION_OBJECT_MANAGEMENT

internal wayland_interface *
Wayland_GetObject(u32 ObjectId)
{
	Platform_LockMutex(&_G.WaylandApi.IdLock);
	wayland_interface *Object = NULL;
	HashMap_Get(&_G.WaylandApi.IdTable, &ObjectId, &Object);
	Platform_UnlockMutex(&_G.WaylandApi.IdLock);
	return Object;
}

internal vptr
Wayland_CreateObject(wayland_prototype *Prototype, u32 ObjectId, u32 Version)
{
	usize InterfaceSize =
		sizeof(wayland_interface) + sizeof(vptr) * Prototype->EventCount;
	wayland_interface *Object =
		Heap_AllocateA(_G.WaylandApi.Heap, InterfaceSize);

	Mem_Set(Object, 0, InterfaceSize);
	Object->Id		  = ObjectId;
	Object->Version	  = Version;
	Object->Size	  = InterfaceSize;
	Object->Prototype = Prototype;

	Object->LinkedEventQueue   = NULL;
	Object->InternalEventQueue = Wayland_CreateEventQueue();
	Object->InternalEventQueue.RefCount++;

	if (Prototype->Deprecated)
		Wayland_LogMessage(
			NULL,
			NULL,
			"Warning: Creating deprecated object %s.\n",
			Wayland_GetDisplayName(Object)
		);

	if (Prototype->Version && Prototype->Version < Version) {
		Wayland_LogMessage(
			NULL,
			NULL,
			"Warning: Creating object %1$s, but its maximum version is v%2$d. "
			"Defaulting to v%2$d\n",
			Wayland_GetDisplayName(Object),
			Prototype->Version
		);
		Version = Prototype->Version;
	}

	Platform_LockMutex(&_G.WaylandApi.IdLock);
	HashMap_Add(&_G.WaylandApi.IdTable, &ObjectId, &Object);
	Platform_LockMutex(&_G.WaylandApi.IdLock);
	return Object;
}

internal void
Wayland_DestroyClientObject(wayland_interface *Object)
{
	if (!Object) return;
	Assert(Object->Id);

	Platform_LockMutex(&_G.WaylandApi.IdLock);
	b08 Removed =
		HashMap_Remove(&_G.WaylandApi.IdTable, &Object->Id, NULL, NULL);
	Assert(Removed);
	Platform_UnlockMutex(&_G.WaylandApi.IdLock);

	Wayland_LinkEventQueue(Object, NULL);
	Object->InternalEventQueue.RefCount--;
	Wayland_DestroyEventQueue(&Object->InternalEventQueue);

#if defined(_DEBUG)
	Mem_Set(Object, 0, Object->Size);
#endif
	Heap_FreeA(Object);
}

internal u32
Wayland_AllocateObjectId(void)
{
	Platform_LockMutex(&_G.WaylandApi.IdLock);
	wayland_api_id_entry *Entry = _G.WaylandApi.NextIdEntry;

	if (Entry) {
		Assert(Entry->Count > 0);

		u32 Id		   = Entry->BaseId;
		Entry->BaseId += 1;
		Entry->Count  -= 1;

		if (Entry->Count == 0) {
			_G.WaylandApi.NextIdEntry = Entry->Next;
			Heap_FreeA(Entry);
		}
	}

	Platform_UnlockMutex(&_G.WaylandApi.IdLock);
	return Id;
}

internal void
Wayland_Display_DeleteId(wayland_display *This, u32 Id)
{
	Platform_LockMutex(&_G.WaylandApi.IdLock);
	wayland_interface *Object = NULL;
	HashMap_Get(&_G.WaylandApi.IdTable, &Id, &Object);
	Platform_UnlockMutex(&_G.WaylandApi.IdLock);

	if (Object) {
		Wayland_LogMessage(
			This,
			"delete_id",
			"Warning: %s was not destroyed client-side. Destroying it now.\n",
			Wayland_GetDisplayName(Object)
		);
		Wayland_DestroyClientObject(Object);
	}

	Platform_LockMutex(&_G.WaylandApi.IdLock);

	// Store a pointer to the entry pointer to allow inserting a new record
	// after the previous.
	wayland_api_id_entry **Entry = &_G.WaylandApi.NextIdEntry;

	// Find the appropriate location to insert
	while (*Entry && (*Entry)->BaseId + (*Entry)->Count < Id)
		Entry = &(*Entry)->Next;

	// Add a new record after prev
	if (!*Entry || Id < (*Entry)->BaseId - 1) {
		wayland_api_id_entry *NewEntry =
			Heap_AllocateA(_G.WaylandApi.Heap, sizeof(wayland_api_id_entry));

		NewEntry->BaseId = Id;
		NewEntry->Count	 = 1;
		NewEntry->Next	 = *Entry;

		*Entry = NewEntry;
	} else if ((*Entry)->BaseId + (*Entry)->Count == Id) {
		// Expand this record right and potentially merge the next
		(*Entry)->Count += 1;

		wayland_api_id_entry *Next = (*Entry)->Next;
		if (Next && Next->BaseId == Id + 1) {
			(*Entry)->Count += Next->Count;
			(*Entry)->Next	 = Next->Next;
			Heap_FreeA(Next);
		}
	} else if ((*Entry)->BaseId == Id + 1) {
		// Expand this record left. We don't need to try to merge, since if it
		// was possible, we already would've above.
		(*Entry)->BaseId -= 1;
		(*Entry)->Count	 += 1;
	}

	Platform_UnlockMutex(&_G.WaylandApi.IdLock);
}

#endif

#ifndef SECTION_SESSION_MANAGEMENT

internal b08
Wayland_IsConnected(void)
{ return _G.WaylandApi.Connected; }

internal void
Wayland_Disconnect(void)
{
	Assert(_G.WaylandApi.Connected);
	_G.WaylandApi.Connected = FALSE;

	Sys_Close(_G.WaylandApi.Socket);
	Platform_FreeMemory(_G.WaylandApi.Heap, _G.WaylandApi.HeapSize);
}

internal b08
Wayland_ConnectToSocket(string Name)
{
	if (_G.WaylandApi.Connected) return TRUE;
	if (Name.Length >= sizeof((sys_sockaddr_unix){}.Data)) return FALSE;

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

	return _G.WaylandApi.Connected;
}

internal wayland_display *
Wayland_GetDisplay()
{
	wayland_display *Display = Wayland_GetObject(WAYLAND_DISPLAY_ID);

	if (!Display) {
		Display = Wayland_CreateObject(
			&Wayland_Display_Prototype,
			WAYLAND_DISPLAY_ID,
			1
		);
		Display->DeleteId = Wayland_Display_DeleteId;
	}

	return Display;
}

#endif

#ifndef SECTION_EVENT_QUEUE

internal wayland_event_queue
Wayland_CreateEventQueue(void)
{
	wayland_event_queue EventQueue = {
		.Deque = ArrayDeque_Init(_G.WaylandApi.Heap, sizeof(wayland_event), 16),
	};
	return EventQueue;
}

internal void
Wayland_DestroyEventQueue(wayland_event_queue *EventQueue)
{
	if (!EventQueue || !EventQueue->Deque.Data) return;
	Assert(
		EventQueue->RefCount == 0,
		"Cannot destroy an event queue while objects still reference it!"
	);

	ArrayDeque_Free(&EventQueue->Deque);
	Mem_Set(EventQueue, 0, sizeof(wayland_event_queue));
}

internal void
Wayland_EnqueueObjectEvent(wayland_interface *Object, wayland_event Event)
{
	Assert(Object);

	wayland_event_queue *EventQueue = Object->LinkedEventQueue;
	if (!EventQueue) EventQueue = &Object->InternalEventQueue;

	Platform_LockMutex(&EventQueue->Lock);
	Assert(
		EventQueue->Deque.Count < 64,
		"Large growth in wayland object event queue"
	);
	ArrayDeque_PushBack(&EventQueue->Deque, &Event);
	Platform_UnlockMutex(&EventQueue->Lock);
}

internal b08
Wayland_DequeueEvent(wayland_event_queue *EventQueue, wayland_event *EventOut)
{
	Assert(EventQueue);

	Platform_LockMutex(&EventQueue->Lock);
	b08 HasEvent = EventQueue->Deque.Count > 0;
	if (HasEvent) ArrayDeque_PopFront(&EventQueue->Deque, EventOut);
	Platform_UnlockMutex(&EventQueue->Lock);

	return HasEvent;
}

internal void
Wayland_LinkEventQueue(
	wayland_interface	*Object,
	wayland_event_queue *EventQueue
)
{
	Assert(Object);
	Platform_LockMutex(&Object->LinkLock);

	if (Object->LinkedEventQueue) {
		Platform_LockMutex(&Object->LinkedEventQueue->Lock);
		Object->LinkedEventQueue->RefCount--;
		Platform_UnlockMutex(&Object->LinkedEventQueue->Lock);
	} else if (EventQueue) {
		Platform_LockMutex(&EventQueue->Lock);
		EventQueue->RefCount++;
		Platform_UnlockMutex(&EventQueue->Lock);

		wayland_event Event;
		while (1) {
			b08 Popped = FALSE;

			wayland_event_queue *InternalQueue = &Object->InternalEventQueue;
			Platform_LockMutex(&InternalQueue->Lock);
			if (InternalQueue->Deque.Count > 0) {
				ArrayDeque_PopFront(&InternalQueue->Deque, &Event);
				Popped = TRUE;
			}
			Platform_UnlockMutex(&InternalQueue->Lock);
			if (!Popped) break;

			Platform_LockMutex(&EventQueue->Lock);
			ArrayDeque_PushBack(&EventQueue->Deque, &Event);
			Platform_UnlockMutex(&EventQueue->Lock);
		}
	}

	Object->LinkedEventQueue = EventQueue;
	Platform_UnlockMutex(&Object->LinkLock);
}

#endif

#ifndef SECTION_MESSAGE_SERIALIZATION

internal void
Wayland_DestroyMessage(wayland_message Message)
{
	if (Message.MessageData) Heap_FreeA(Message.MessageData);
	if (Message.ControlData) Heap_FreeA(Message.ControlData);
}

internal b08
Wayland_PrepareMethod(
	wayland_interface		*Object,
	u16						 Opcode,
	b08						 IsEvent,
	wayland_prepared_method *MethodOut
)
{
	wayland_prepared_method PreparedMethod = { 0 };

	PreparedMethod.Object = Object;
	PreparedMethod.Opcode = Opcode;

	u32 Limit = IsEvent ? Object->Prototype->EventCount
						: Object->Prototype->RequestCount;
	if (Opcode >= Limit) return FALSE;

	PreparedMethod.Method = IsEvent ? Object->Prototype->Events[Opcode]
									: Object->Prototype->Requests[Opcode];
	wayland_method Method = PreparedMethod.Method;

	c08 *C = Method.Format;

	u16 ObjectVersion = MAX(Object->Version, 1);

	while (*C >= '0' && *C <= '9') {
		PreparedMethod.SinceVersion *= 10;
		PreparedMethod.SinceVersion += *C - '0';
		C++;
	}
	if (!PreparedMethod.SinceVersion) PreparedMethod.SinceVersion = 1;

	if (*C == '-') {
		C++;
		while (*C >= '0' && *C <= '9') {
			PreparedMethod.UntilVersion *= 10;
			PreparedMethod.UntilVersion += *C - '0';
			C++;
		}
	}
	if (!PreparedMethod.UntilVersion)
		PreparedMethod.UntilVersion = ObjectVersion;

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

	if (PreparedMethod.SinceVersion > ObjectVersion)
		Wayland_LogMessage(
			Object,
			Method.Name,
			"Warning: Method is not introduced until v%d",
			PreparedMethod.SinceVersion
		);

	if (PreparedMethod.UntilVersion < ObjectVersion)
		Wayland_LogMessage(
			Object,
			Method.Name,
			"Warning: Method has been deprecated since v%d",
			PreparedMethod.UntilVersion + 1
		);

	*MethodOut = PreparedMethod;
	return TRUE;
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
				u32				   ObjectId	 = Wayland_AllocateObjectId();
				wayland_prototype *Prototype = Param->NewObject.Prototype;
				u16				   Version	 = Object->Version;
				if (!Prototype) {
					Prototype = VA_Next(Args, wayland_prototype *);
					Version	  = VA_Next(Args, u16);
				}
				Param->NewObject.Data =
					Wayland_CreateObject(Prototype, ObjectId, Version);
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
			if (Object) Wayland_DestroyClientObject(Object);
		}
	}
}

internal void
Wayland_DestroyPreparedMethod(wayland_prepared_method PreparedMethod)
{ Heap_FreeA(PreparedMethod.Params); }

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
	u32						*Words,
	usize					*WordIndex,
	wayland_prepared_method *PreparedMethod,
	usize					 ParamIndex
)
{
	wayland_interface *Object = PreparedMethod->Object;
	wayland_param	  *Param  = &PreparedMethod->Params[ParamIndex];
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
					PreparedMethod->Method.Name,
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
					PreparedMethod->Method.Name,
					"Warning: Received a null object for non-nullable param %d",
					ParamIndex
				);
			if (ObjectId && !Param->Object.Data)
				Wayland_LogMessage(
					Object,
					PreparedMethod->Method.Name,
					"Warning: Failed to lookup object #%d",
					ObjectId
				);
			break;
		case WAYLAND_PARAM_NEWOBJECT:
			Param->NewObject.Data = Wayland_CreateObject(
				Param->NewObject.Prototype,
				Words[WI++],
				PreparedMethod->Object->Version
			);
			break;
		case WAYLAND_PARAM_FD: Param->Fd = -1; break;
		default				 : Assert(FALSE);
	}

	*WordIndex = WI;
}

internal b08
Wayland_DeserializeMethod(u32 *Words, wayland_prepared_method *PreparedMethod)
{
	u32 ObjectId	= Words[0];
	u16 Opcode		= Words[1] & 0xFFFF;
	u16 MessageSize = Words[1] >> 16;

	wayland_interface *Object = Wayland_GetObject(ObjectId);
	Assert(Object);

	if (!Wayland_PrepareMethod(Object, Opcode, TRUE, PreparedMethod))
		return FALSE;

	wayland_param *Params = PreparedMethod->Params;
	Params[0].Object.Data = Object;

	usize WI = 2;
	for (usize I = 1; I < PreparedMethod->ParamCount; I++)
		Wayland_DeserializeParam(Words, &WI, PreparedMethod, I);

	return TRUE;
}

#endif

#ifndef SECTION_MESSAGE_QUEUE

internal b08
Wayland_WaitUntilCanReceive(s32 Timeout)
{
	if (!_G.WaylandApi.Connected) return FALSE;

	sys_pollfd PollFd = {
		.FileDescriptor	 = _G.WaylandApi.Socket,
		.RequestedEvents = SYS_POLLIN,
		.ReturnedEvents	 = 0,
	};

	s32 Result = Sys_Poll(&PollFd, 1, Timeout);
	return Result == 1 && (PollFd.ReturnedEvents & SYS_POLLIN);
}

internal b08
Wayland_WaitUntilCanSend(s32 Timeout)
{
	if (!_G.WaylandApi.Connected) return FALSE;

	sys_pollfd PollFd = {
		.FileDescriptor	 = _G.WaylandApi.Socket,
		.RequestedEvents = SYS_POLLOUT,
		.ReturnedEvents	 = 0,
	};

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

internal void
Wayland_EnqueueMessage(
	heap				  *Heap,
	wayland_message_queue *MessageQueue,
	u32					  *Data
)
{
	u16 MessageSize = Data[1] >> 16;
	Assert(MessageSize % sizeof(u32) == 0);

	// TODO: Can theset two allocations be merged?
	wayland_message_queue_entry *Entry =
		Heap_AllocateA(Heap, sizeof(wayland_message_queue_entry));

	Entry->Next = NULL;
	Entry->Data = Heap_AllocateA(Heap, MessageSize);
	Mem_Cpy(Entry->Data, Data, MessageSize);

	if (!Wayland_DeserializeMethod(Entry->Data, &Entry->PreparedMethod)) {
		Heap_FreeA(Entry->Data);
		Heap_FreeA(Entry);
	} else {
		if (MessageQueue->Back) {
			MessageQueue->Back->Next = Entry;
		} else {
			Assert(!MessageQueue->Front);
			MessageQueue->Front = Entry;
		}
		MessageQueue->Back = Entry;
		MessageQueue->Count++;
	}
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
Wayland_PollConnection(s32 Timeout)
{
	Platform_LockMutex(&_G.WaylandApi.SocketLock);

	b08 Dequeued = FALSE;
	if (_G.WaylandApi.Connected) {
		Wayland_PollSocket(
			_G.WaylandApi.Heap,
			_G.WaylandApi.Socket,
			&_G.WaylandApi.MessageQueue,
			&_G.WaylandApi.FdQueue,
			Timeout
		);

		wayland_event Event;
		Dequeued = Wayland_TryDequeueEvent(
			_G.WaylandApi.Heap,
			&_G.WaylandApi.MessageQueue,
			&_G.WaylandApi.FdQueue,
			&Event
		);

		if (Dequeued) Wayland_EnqueueObjectEvent(Event.Method.Object, Event);
	}

	Platform_UnlockMutex(&_G.WaylandApi.SocketLock);
	return Dequeued;
}

internal void
Wayland_DispatchEvent(wayland_event Event)
{
	vptr Proc = Event.Method.Object->Events[Event.Method.Opcode];
	if (!Proc) return;

#if defined(_X64)
	usize StackSize = 0;
	u64	  Regs[6];
	usize RI = 0;

	for (usize I = 0; I < Event.Method.ParamCount; I++) {
		wayland_param Param		 = Event.Method.Params[I];
		usize		  ParamSize	 = (Param.Size + 7) & ~7;
		usize		  ParamWords = ParamSize / 8;
		if (ParamWords <= 6 - RI) RI += ParamWords;
		else StackSize += ParamSize;
	}
	usize StackWords = StackSize / 8;

	// Leave 8-byte extra to have 16-byte alignment later
	if (StackWords % 2 == 0) {
		StackSize += 8;
		StackWords++;
	}

	RI				   = 0;
	usize *Stack	   = Heap_AllocateA(_G.WaylandApi.Heap, StackSize);
	vptr   StackCursor = Stack;
	for (usize I = 0; I < Event.Method.ParamCount; I++) {
		wayland_param Param		 = Event.Method.Params[I];
		usize		  ParamSize	 = (Param.Size + 7) & ~7;
		usize		  ParamWords = ParamSize / 8;
		if (ParamWords <= 6 - RI) {
			Mem_Cpy(Regs + RI, &Param.Data, ParamSize);
			RI += ParamWords;
		} else {
			Mem_Cpy(StackCursor, &Param.Data, Param.Size);
			StackCursor += ParamSize;
		}
	}

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
	Platform_LockMutex(&_G.WaylandApi.SocketLock);

	wayland_prepared_method PreparedMethod;
	if (!Wayland_PrepareMethod(Object, Opcode, FALSE, &PreparedMethod))
		Assert(FALSE, "Request not defined!");

	Wayland_FillRequestParams(PreparedMethod, Args);
	Wayland_GetConstructedParams(PreparedMethod, Constructed);

	wayland_message Message = Wayland_SerializeMethod(PreparedMethod);

	b08 CanSend = Wayland_WaitUntilCanSend(-1);
	b08 Sent	= FALSE;
	if (CanSend) Sent = Wayland_SendMessage(Message);

	Wayland_DestroyMessage(Message);

	if (Sent) Wayland_DestroyDestructorParams(PreparedMethod);
	Wayland_DestroyPreparedMethod(PreparedMethod);

	Platform_UnlockMutex(&_G.WaylandApi.SocketLock);
}

internal void
Wayland_HandleMultiConstructorRequest(
	WAYLAND_INTERFACE	Object,
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
Wayland_HandleConstructorRequest(WAYLAND_INTERFACE Object, u16 Opcode, ...)
{
	wayland_interface *Constructed;

	va_list Args;
	VA_Start(Args, Opcode);
	Wayland_HandleRequestV(Object, Opcode, &Constructed, Args);
	VA_End(Args);

	return Constructed;
}

internal void
Wayland_HandleRequest(WAYLAND_INTERFACE Object, u16 Opcode, ...)
{
	va_list Args;
	VA_Start(Args, Opcode);
	Wayland_HandleRequestV(Object, Opcode, NULL, Args);
	VA_End(Args);
}

#endif

#endif
#endif
