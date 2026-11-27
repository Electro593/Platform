/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                             *
*  Author: Aria Seiler                                                        *
*                                                                             *
*  This program is in the public domain. There is no implied warranty, so use *
*  it at your own risk.                                                       *
*                                                                             *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#pragma once

#include <platform/linux/wayland/drm.h>

#include "core.h"

#define ZWP_LINUX_DMABUF_V1                                          \
	((ZwpLinuxDmabufV1, zwp_linux_dmabuf_v1, 6),                     \
		D((Destroy)),                                                \
		R((CreateParams), N(ZWP_LINUX_BUFFER_PARAMS_V1)),            \
		R((GetDefaultFeedback, 4), N(ZWP_LINUX_DMABUF_FEEDBACK_V1)), \
		R((GetSurfaceFeedback, 4),                                   \
			N(ZWP_LINUX_DMABUF_FEEDBACK_V1),                         \
			O(Surface, WL_SURFACE)),                                 \
		E((Format, 1, 3), U(Format, drm_format)),                    \
		E((Modifier, 3, 3),                                          \
			U(Format, drm_format),                                   \
			U(ModifierHigh),                                         \
			U(ModifierLow)))

#define ZWP_LINUX_BUFFER_PARAMS_V1                            \
	((ZwpLinuxBufferParamsV1, zwp_linux_buffer_params_v1, 6), \
		D((Destroy)),                                         \
		R((Add),                                              \
			F(Fd),                                            \
			U(PlaneIndex),                                    \
			U(Offset),                                        \
			U(Stride),                                        \
			U(ModifierHigh),                                  \
			U(ModifierLow)),                                  \
		R((Create),                                           \
			I(Width),                                         \
			I(Height),                                        \
			U(Format, drm_format),                            \
			U(Flags, zwp_linux_buffer_params_v1_flags)),      \
		R((CreateImmed, 2),                                   \
			N(WL_BUFFER),                                     \
			I(Width),                                         \
			I(Height),                                        \
			U(Format, drm_format),                            \
			U(Flags, zwp_linux_buffer_params_v1_flags)),      \
		R((SetSamplingDevice, 6), A(Device)),                 \
		E((Created), N(Buffer, WL_BUFFER)),                   \
		E((Failed)))

typedef enum zwp_linux_buffer_params_v1_error : u32 {
	ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_ALREADY_USED		= 0,
	ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_PLANE_IDX			= 1,
	ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_PLANE_SET			= 2,
	ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_INCOMPLETE			= 3,
	ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_INVALID_FORMAT		= 4,
	ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_INVALID_DIMENSIONS = 5,
	ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_OUT_OF_BOUNDS		= 6,
	ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_INVALID_WL_BUFFER	= 7,
	ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_INVALID_DEV_T_SIZE = 8,  // v6
} zwp_linux_buffer_params_v1_error;

typedef enum zwp_linux_buffer_params_v1_flags : u32 {
	ZWP_LINUX_BUFFER_PARAMS_V1_FLAGS_Y_INVERT	  = 0x01,
	ZWP_LINUX_BUFFER_PARAMS_V1_FLAGS_INTERLACED	  = 0x02,
	ZWP_LINUX_BUFFER_PARAMS_V1_FLAGS_BOTTOM_FIRST = 0x04,
} zwp_linux_buffer_params_v1_flags;

#define ZWP_LINUX_DMABUF_FEEDBACK_V1                               \
	((ZwpLinuxDmabufFeedbackV1, zwp_linux_dmabuf_feedback_v1, 6),  \
		D((Destroy)),                                              \
		E((Done)),                                                 \
		E((Format_table), F(Fd), U(Size)),                         \
		E((Main_device, 1, 6), A(Device)),                         \
		E((Tranche_done)),                                         \
		E((Tranche_target_device), A(Device)),                     \
		E((Tranche_formats), A(Indices)),                          \
		E((Tranche_flags),                                         \
			U(Flags, zwp_linux_dmabuf_feedback_v1_tranche_flags)))

typedef enum zwp_linux_dmabuf_feedback_v1_tranche_flags : u32 {
	ZWP_LINUX_DMABUF_FEEDBACK_V1_TRANCHE_FLAGS_SCANOUT	= 0x01,	 // v4
	ZWP_LINUX_DMABUF_FEEDBACK_V1_TRANCHE_FLAGS_SAMPLING = 0x02,	 // v6
} zwp_linux_dmabuf_feedback_v1_tranche_flags;

WLI_DECLARE_STRUCT(ZWP_LINUX_DMABUF_V1)
WLI_DECLARE_STRUCT(ZWP_LINUX_BUFFER_PARAMS_V1)
WLI_DECLARE_STRUCT(ZWP_LINUX_DMABUF_FEEDBACK_V1)

WLI_DECLARE(ZWP_LINUX_DMABUF_V1)
WLI_DECLARE(ZWP_LINUX_BUFFER_PARAMS_V1)
WLI_DECLARE(ZWP_LINUX_DMABUF_FEEDBACK_V1)
