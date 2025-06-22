/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
**                                                                         **
**  Author: Aria Seiler                                                    **
**                                                                         **
**  This program is in the public domain. There is no implied warranty,    **
**  so use it at your own risk.                                            **
**                                                                         **
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef INCLUDE_HEADER

typedef enum wayland_display_error wayland_display_error;

typedef union wayland_primitive wayland_primitive;

typedef struct wayland_message	 wayland_message;
typedef struct wayland_interface wayland_interface;
typedef struct wayland_display	 wayland_display;
typedef struct wayland_registry	 wayland_registry;
typedef struct wayland_callback	 wayland_callback;

enum wayland_display_error {
	WAYLAND_DISPLAY_ERROR_INVALID_OBJECT = 0,
	WAYLAND_DISPLAY_ERROR_INVALID_METHOD = 1,
	WAYLAND_DISPLAY_ERROR_NO_MEMORY		 = 2,
	WAYLAND_DISPLAY_ERROR_IMPLEMENTATION = 3,
};

union wayland_primitive {
	s32	   Sint;
	u32	   Uint;
	string String;
};

struct wayland_message {
	u32 ObjectId;
	u16 Opcode;
	u16 Size;
	u32 Data[];
};

struct wayland_interface {
	u32 Id;

	void (*HandleEvent)(wayland_interface *This, wayland_message *Message);
};

struct wayland_display {
	wayland_interface Header;

	void (*HandleError)(
		wayland_display		 *This,
		u32					  ObjectId,
		wayland_display_error Code,
		string				  Message
	);
	void (*HandleDeleteId)(wayland_display *This, u32 Id);

	wayland_callback (*Sync)(wayland_display *This);
	wayland_registry (*GetRegistry)(wayland_display *This);
};

struct wayland_registry {
	wayland_interface Header;

	void (*HandleGlobal)(wayland_registry *This, u32 Name, string Interface, u32 Version);
	void (*HandleGlobalRemove)(wayland_registry *This, u32 Name);

	u32 (*Bind)(wayland_registry *This, u32 Name);
};

struct wayland_callback {
	wayland_interface Header;

	void (*HandleDone)(wayland_callback *This, u32 CallbackData);
};

#define WAYLAND_FUNCS \
	EXPORT(b08, Wayland_CreateWindow, c08 *Title, usize Width, usize Height) \
	\
	INTERN(void,             Wayland_Display_HandleEvent,  wayland_display *This, wayland_message *Message) \
	INTERN(wayland_callback, Wayland_Display_Sync,         wayland_display *This) \
	INTERN(wayland_registry, Wayland_Display_GetRegistry,  wayland_display *This) \
	INTERN(void,             Wayland_Registry_HandleEvent, wayland_registry *This, wayland_message *Message) \
	INTERN(u32,              Wayland_Registry_Bind,        wayland_registry *This, u32 Name) \
	INTERN(void,             Wayland_Callback_HandleEvent, wayland_callback *This, wayland_message *Message)

#endif

#ifdef INCLUDE_SOURCE

wayland_display WaylandDisplay = {
	.Header			= {.Id = 1, .HandleEvent = (vptr) Wayland_Display_HandleEvent},
	.HandleError	= NULL,
	.HandleDeleteId = NULL,
	.Sync			= Wayland_Display_Sync,
	.GetRegistry	= Wayland_Display_GetRegistry,
};

wayland_registry WaylandRegistryPrototype = {
	.Header				= {.Id = 0, .HandleEvent = (vptr) Wayland_Registry_HandleEvent},
	.HandleGlobal		= NULL,
	.HandleGlobalRemove = NULL,
	.Bind				= Wayland_Registry_Bind,
};

wayland_callback WaylandCallbackPrototype = {
	.Header		= {.Id = 0, .HandleEvent = (vptr) Wayland_Callback_HandleEvent},
	.HandleDone = NULL,
};

internal u32
Wayland_AllocateObjectId()
{
	return _G.NextObjectId++;
}

internal b08
Wayland_ConnectToServerSocket(file_handle *Socket)
{
	s32	   FileDescriptor;
	string WaylandSocket = Platform_GetEnvParam(CStringL("WAYLAND_SOCKET"));
	if (String_TryParseS32(WaylandSocket, &FileDescriptor)) {
#ifdef _LINUX
		Socket->FileDescriptor = FileDescriptor;
#endif
		return TRUE;
	}

	string XdgRuntimeDir = Platform_GetEnvParam(CStringL("XDG_RUNTIME_DIR"));
	Stack_Push();
	XdgRuntimeDir = String_Cat(XdgRuntimeDir, CStringL("/"));

	string WaylandDisplay = Platform_GetEnvParam(CStringL("WAYLAND_DISPLAY"));

	if (WaylandDisplay.Length) {
		string SocketPath = String_Cat(XdgRuntimeDir, WaylandDisplay);

		if (!Platform_ConnectToLocalSocket(SocketPath, Socket)) {
			WaylandDisplay = CNStringL("wayland-0");
			SocketPath	   = String_Cat(XdgRuntimeDir, WaylandDisplay);

			if (!Platform_ConnectToLocalSocket(SocketPath, Socket)) {
				Stack_Pop();
				return FALSE;
			}
		}
	}

	Stack_Pop();
	return TRUE;
}

internal void
Wayland_SendWord(u32 Word)
{
	usize BytesWritten = Platform_WriteFile(_G.Socket, &Word, sizeof(u32), (usize) -1);
	Assert(BytesWritten = sizeof(u32));
}

internal void
Wayland_SendMessage(u32 ObjectId, u16 Opcode, u16 WordCount, ...)
{
	Wayland_SendWord(ObjectId);
	Wayland_SendWord(((2 + WordCount) * sizeof(u32)) << 16 | Opcode);

	va_list Args;
	VA_Start(Args, WordCount);
	for (usize I = 0; I < WordCount; I++) Wayland_SendWord(VA_Next(Args, u32));
	VA_End(Args);
}

internal wayland_message *
Wayland_ReadMessage(void)
{
	u32	  Header[2];
	usize BytesRead = Platform_ReadFile(
		_G.Socket,
		(vptr) Header,
		sizeof(wayland_message),
		(usize) -1
	);
	Assert(BytesRead == sizeof(wayland_message));

	u32 ObjectId = Header[0];
	u16 Size	 = Header[1] >> 16;
	u16 Opcode	 = Header[1] & 0xFFFF;

	wayland_message *Message  = Heap_AllocateA(_G.Heap, Size);
	Message->ObjectId		  = ObjectId;
	Message->Opcode			  = Opcode;
	Message->Size			  = Size;
	BytesRead				 += Platform_ReadFile(
		   _G.Socket,
		   Message->Data,
		   Size - sizeof(wayland_message),
		   (usize) -1
	   );
	Assert(BytesRead == Size);

	return Message;
}

internal void
Wayland_FreeMessage(wayland_message *Message)
{
	Heap_FreeA(Message);
}

#define MAC_FOR_FUNC_MAKE_PARSE_PRIM(NAME, ARG, ITER) \
	MAC_CONCAT(Arg, ITER) = MAC_CONCAT(Wayland_Parse, ARG)(NAME, &I)
#define MAC_FOR_FUNC_MAKE_PARSE_ARG(NAME, ARG, ITER) \
	MAC_CONCAT(Arg, ITER).ARG
#define TRYCALL(Handler, Name, Message, ...) \
	if (Handler->Name) { \
		usize I = 0; \
		__VA_OPT__(wayland_primitive MAC_FOREACH(Message, MAC_FOR_OP_SEQ, MAC_FOR_FUNC_MAKE_PARSE_PRIM, __VA_ARGS__);) \
		(Handler->Name)(Handler __VA_OPT__(, MAC_FOREACH(Message, MAC_FOR_OP_SEQ, MAC_FOR_FUNC_MAKE_PARSE_ARG, __VA_ARGS__))); \
	}

internal wayland_primitive
Wayland_ParseSint(wayland_message *Message, usize *I)
{
	wayland_primitive Prim;

	Prim.Sint = (s32) Message->Data[*I];

	*I += 1;
	return Prim;
}

internal wayland_primitive
Wayland_ParseUint(wayland_message *Message, usize *I)
{
	wayland_primitive Prim;

	Prim.Uint = Message->Data[*I];

	*I += 1;
	return Prim;
}

internal wayland_primitive
Wayland_ParseString(wayland_message *Message, usize *I)
{
	wayland_primitive Prim;

	c08 *Bytes	= (c08 *) (Message->Data + *I);
	Prim.String = CString(Bytes);

	*I += (Prim.String.Length + 4) / 4;
	return Prim;
}

internal void
Wayland_Display_HandleEvent(wayland_display *This, wayland_message *Message)
{
	switch (Message->Opcode) {
		case 0: TRYCALL(This, HandleError, Message, Uint, Uint, String); break;
		case 1: TRYCALL(This, HandleDeleteId, Message, Uint); break;
	}
}

internal wayland_callback
Wayland_Display_Sync(wayland_display *This)
{
	u32 CallbackId = Wayland_AllocateObjectId();
	Wayland_SendMessage(This->Header.Id, 0, 1, CallbackId);
	wayland_callback Callback = WaylandCallbackPrototype;
	Callback.Header.Id		  = CallbackId;
	return Callback;
}

internal wayland_registry
Wayland_Display_GetRegistry(wayland_display *This)
{
	u32 RegistryId = Wayland_AllocateObjectId();
	Wayland_SendMessage(This->Header.Id, 1, 1, RegistryId);
	wayland_registry Registry = WaylandRegistryPrototype;
	Registry.Header.Id		  = RegistryId;
	return Registry;
}

internal void
Wayland_Registry_HandleEvent(wayland_registry *This, wayland_message *Message)
{
	switch (Message->Opcode) {
		case 0: TRYCALL(This, HandleGlobal, Message, Uint, String, Uint); break;
		case 1: TRYCALL(This, HandleGlobalRemove, Message, Uint); break;
	}
}

internal u32
Wayland_Registry_Bind(wayland_registry *This, u32 Name)
{
	u32 NewId = Wayland_AllocateObjectId();
	Wayland_SendMessage(This->Header.Id, 0, 2, Name, NewId);
	return NewId;
}

internal void
Wayland_Callback_HandleEvent(wayland_callback *This, wayland_message *Message)
{
	switch (Message->Opcode) {
		case 0: TRYCALL(This, HandleDone, Message, Uint); break;
	}
}

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
