/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                             *
*  Author: Aria Seiler                                                        *
*                                                                             *
*  This program is in the public domain. There is no implied warranty, so use *
*  it at your own risk.                                                       *
*                                                                             *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "util/intrin.h"
#include "wayland/protocol/wire.h"
#include "wayland/protocol/wli.h"

typedef struct wayland_arg {
	c08 Type;
	union {
		wayland_int		   I;
		wayland_uint	   U;
		wayland_fixed	   R;
		wayland_fd		   F;
		wayland_string	   S;
		wayland_array	   A;
		wayland_prototype *N;
		wayland_object	  *O;
		wayland_object	  *D;
	};
} wayland_arg;

typedef struct wayland_method {
	wayland_object *Object;
	u16				Opcode;

	wayland_arg Args[WLI_MAX_ARGS];
} wayland_method;

wayland_method
MakeRequestMethodV(wayland_object *Object, u16 Opcode, va_list Args);

wayland_message SerializeMethod(wayland_method Method);

wayland_method DeserializeMethod(wayland_message Message);
