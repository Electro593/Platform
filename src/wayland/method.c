/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                             *
*  Author: Aria Seiler                                                        *
*                                                                             *
*  This program is in the public domain. There is no implied warranty, so use *
*  it at your own risk.                                                       *
*                                                                             *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "method.h"

#include "object.h"

wayland_method
MakeRequestMethodV(wayland_object *Object, u16 Opcode, va_list Args)
{
	wayland_prototype		 *Prototype = ObjectPrototype(Object);
	wayland_prototype_method *Method	= RequestPrototype(Object, Opcode);
}
