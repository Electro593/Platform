/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                             *
*  Author: Aria Seiler                                                        *
*                                                                             *
*  This program is in the public domain. There is no implied warranty, so use *
*  it at your own risk.                                                       *
*                                                                             *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "core.h"

#define ZWP_RELATIVE_POINTER_MANAGER_V1                                 \
	((ZwpRelativePointerManagerV1, zwp_relative_pointer_manager_v1, 1), \
		D((Destroy)),                                                   \
		R((GetRelativePointer),                                         \
			N(ZWP_RELATIVE_POINTER_V1),                                 \
			O(Pointer, WL_POINTER)))

#define ZWP_RELATIVE_POINTER_V1                          \
	((ZwpRelativePointerV1, zwp_relative_pointer_v1, 1), \
		D((Destroy)),                                    \
		E((RelativeMotion),                              \
			U(MicroTimeHigh),                            \
			U(MicroTimeLow),                             \
			R(DeltaX),                                   \
			R(DeltaY),                                   \
			R(UnacceleratedDeltaX),                      \
			R(UnacceleratedDeltaY)))

WLI_DECLARE_STRUCT(ZWP_RELATIVE_POINTER_MANAGER_V1)
WLI_DECLARE_STRUCT(ZWP_RELATIVE_POINTER_V1)

WLI_DECLARE(ZWP_RELATIVE_POINTER_MANAGER_V1)
WLI_DECLARE(ZWP_RELATIVE_POINTER_V1)
