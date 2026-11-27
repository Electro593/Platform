/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                             *
*  Author: Aria Seiler                                                        *
*                                                                             *
*  This program is in the public domain. There is no implied warranty, so use *
*  it at your own risk.                                                       *
*                                                                             *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "wayland/protocol/wire.h"

struct wayland_object {
	wayland_object_header Header;

	void (*Events[])(wayland_object *This, ...);
};

wayland_prototype *ObjectPrototype(wayland_object *Object);

wayland_prototype_method *RequestPrototype(wayland_object *Object, u16 Opcode);

wayland_prototype_method *EventPrototype(wayland_object *Object, u16 Opcode);
