/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
**                                                                         **
**  Author: Aria Seiler                                                    **
**                                                                         **
**  This program is in the public domain. There is no implied warranty,    **
**  so use it at your own risk.                                            **
**                                                                         **
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef INCLUDE_HEADER

typedef struct wayland_message_header {
	u32 ObjectId;
	u32 Info;
} wayland_message_header;

#define WAYLAND_FUNCS \
	EXPORT(b08, Wayland_CreateWindow, c08 *Title, usize Width, usize Height)

#endif

#ifdef INCLUDE_SOURCE

internal u32
Wayland_AllocateObjectId()
{
	return _G.NextObjectId++;
}

internal wayland_message_header
Wayland_MakeMessageHeader(u32 ObjectId, u16 ContentSize, u16 Opcode)
{
	return (wayland_message_header) {.ObjectId = ObjectId,
									 .Info = ((ContentSize + sizeof(wayland_message_header)) << 16) | Opcode};
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

internal b08
Wayland_CreateWindow(c08 *Title, usize Width, usize Height)
{
	file_handle Socket;
	if (!Wayland_ConnectToServerSocket(&Socket)) return FALSE;

	return TRUE;
}

#endif
