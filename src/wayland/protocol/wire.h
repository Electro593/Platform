/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                             *
*  Author: Aria Seiler                                                        *
*                                                                             *
*  This program is in the public domain. There is no implied warranty, so use *
*  it at your own risk.                                                       *
*                                                                             *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#pragma once

#include <util/string.h>
#include <util/set.h>

typedef s32 wayland_fd;
typedef u32 wayland_fixed;
typedef string wayland_string;

typedef struct wayland_array {
	u32	 Size;
	u32 *Data;
} wayland_array;

typedef struct wayland_prototype_method {
	u16	 Since;
	u16	 Until;
	c08 *Format;

	struct wayland_prototype **Prototypes;
} wayland_prototype_method;

typedef struct wayland_prototype {
	c08 *Name;
	u16	 MaxVersion;
	b08	 Frozen;
	b08	 Deprecated;

	u16 RequestCount;
	u16 EventCount;

	wayland_prototype_method *Requests;
	wayland_prototype_method *Events;
} wayland_prototype;

typedef struct wayland_event_queue {
	array_deque	 Deque;
	mutex_handle Lock;
	usize		 RefCount;
} wayland_event_queue;

typedef struct wayland_interface_header {
	u32 Id;
	u16 Version;
	u16 Size;

	wayland_prototype *Prototype;

	mutex_handle Lock;

	vptr UserData;

	struct wayland_event_queue *LinkedEventQueue;
	struct wayland_event_queue	InternalEventQueue;
} wayland_interface_header;

typedef struct wayland_interface {
	wayland_interface_header Header;

	void (*Events[])(struct wayland_interface *This, ...);
} wayland_interface;

void Wayland_HandleRequest(wayland_interface *This, u16 Opcode, ...);
