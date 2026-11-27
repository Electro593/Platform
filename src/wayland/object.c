/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                             *
*  Author: Aria Seiler                                                        *
*                                                                             *
*  This program is in the public domain. There is no implied warranty, so use *
*  it at your own risk.                                                       *
*                                                                             *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "object.h"

#include "shared.h"

wayland_prototype *
ObjectPrototype(wayland_object *Object)
{
	Assert(Object);
	Assert(Object->Header.Prototype);
	return Object->Header.Prototype;
}

wayland_prototype_method *
RequestPrototype(wayland_object *Object, u16 Opcode)
{
	wayland_prototype *Prototype = ObjectPrototype(Object);
	Assert(Opcode < Prototype->RequestCount);
	return &Prototype->Requests[Opcode];
}

wayland_prototype_method *
EventPrototype(wayland_object *Object, u16 Opcode)
{
	wayland_prototype *Prototype = ObjectPrototype(Object);
	Assert(Opcode < Prototype->EventCount);
	return &Prototype->Events[Opcode];
}
