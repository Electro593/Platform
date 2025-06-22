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
	return (wayland_message_header) {
		.ObjectId = ObjectId,
		.Info = ((ContentSize + sizeof(wayland_message_header)) << 16) | Opcode
	};
}

internal b08
Wayland_CreateWindow(c08 *Title, usize Width, usize Height)
{
	return FALSE;
}

#endif
