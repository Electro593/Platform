/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                             *
*  Author: Aria Seiler                                                        *
*                                                                             *
*  This program is in the public domain. There is no implied warranty, so use *
*  it at your own risk.                                                       *
*                                                                             *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#pragma once

#include "protocol.h"

wayland_prototype Wayland_Display_Prototype = {
	.Name		  = "wl_display",
	.Version	  = 1,
	.RequestCount = 2,
	.EventCount	  = 2,
	.Requests =
		M{
		  { "sync", "on", P{ &Wayland_Callback_Prototype } },
		  { "get_registry", "on", P{ &Wayland_Registry_Prototype } },
		  },
	.Events =
		M{
		  { "error", "oous", NULL },
		  { "delete_id", "ou", NULL },
		  },
};

enum wayland_display_error {
	WAYLAND_DISPLAY_ERROR_INVALID_OBJECT = 0,
	WAYLAND_DISPLAY_ERROR_INVALID_METHOD = 1,
	WAYLAND_DISPLAY_ERROR_NO_MEMORY		 = 2,
	WAYLAND_DISPLAY_ERROR_IMPLEMENTATION = 3,
};

wayland_prototype Wayland_Registry_Prototype = {
	.Name		  = "wl_registry",
	.Version	  = 1,
	.RequestCount = 1,
	.EventCount	  = 2,
	.Requests =
		M{
		  { "bind", "oun", P{ NULL } },
		  },
	.Events =
		M{
		  { "global", "ousu", NULL },
		  { "global_remove", "ou", NULL },
		  },
};

wayland_prototype Wayland_Callback_Prototype = {
	.Name		= "wl_callback",
	.EventCount = 1,
	.Events		= M{ { "done", "ou", NULL } },
};

wayland_prototype Wayland_Compositor_Prototype = {
	.Name		  = "wl_compositor",
	.Version	  = 7,
	.RequestCount = 3,
	.Requests =
		M{
		  { "create_surface", "on", P{ &Wayland_Surface_Prototype } },
		  { "create_region", "on", P{ &Wayland_Region_Prototype } },
		  { "release", "7d", NULL },
		  }
};

wayland_prototype Wayland_ShmPool_Prototype = {
	.Name		  = "wl_shm_pool",
	.Version	  = 3,
	.RequestCount = 3,
	.Requests =
		M{
		  { "create_buffer", "oniiiiu", P{ &Wayland_Buffer_Prototype } },
		  { "destroy", "d", NULL },
		  { "resize", "oi", NULL },
		  },
};

enum wayland_shm_pool_error {
	WAYLAND_SHM_POOL_ERROR_INVALID_FORMAT = 0,	// v3
	WAYLAND_SHM_POOL_ERROR_INVALID_STRIDE = 1,	// v3
};

wayland_prototype Wayland_Shm_Prototype = {
	.Name		  = "wl_shm",
	.Version	  = 3,
	.RequestCount = 2,
	.EventCount	  = 1,
	.Requests =
		M{
		  { "create_pool", "onfi", P{ &Wayland_ShmPool_Prototype } },
		  { "release", "2d", NULL },
		  },
	.Events =
		M{
		  { "format", "ou", NULL },
		  },
};

enum wayland_shm_error {
	WAYLAND_SHM_ERROR_INVALID_FORMAT		  = 0,	// v1-v2
	WAYLAND_SHM_ERROR_INVALID_STRIDE		  = 1,
	WAYLAND_SHM_ERROR_INVALID_FILE_DESCRIPTOR = 2,
};

enum wayland_shm_format {
	// clang-format off
	WAYLAND_SHM_FORMAT_ARGB8888             = 0x00000000, // - 32-bit ARGB format, [31:0] A:R:G:B 8:8:8:8 little endian
	WAYLAND_SHM_FORMAT_XRGB8888             = 0x00000001, // - 32-bit RGB format, [31:0] x:R:G:B 8:8:8:8 little endian
	WAYLAND_SHM_FORMAT_C8                   = 0x20203843, // - 8-bit color index format, [7:0] C
	WAYLAND_SHM_FORMAT_RGB332               = 0x38424752, // - 8-bit RGB format, [7:0] R:G:B 3:3:2
	WAYLAND_SHM_FORMAT_BGR233               = 0x38524742, // - 8-bit BGR format, [7:0] B:G:R 2:3:3
	WAYLAND_SHM_FORMAT_XRGB4444             = 0x32315258, // - 16-bit xRGB format, [15:0] x:R:G:B 4:4:4:4 little endian
	WAYLAND_SHM_FORMAT_XBGR4444             = 0x32314258, // - 16-bit xBGR format, [15:0] x:B:G:R 4:4:4:4 little endian
	WAYLAND_SHM_FORMAT_RGBX4444             = 0x32315852, // - 16-bit RGBx format, [15:0] R:G:B:x 4:4:4:4 little endian
	WAYLAND_SHM_FORMAT_BGRX4444             = 0x32315842, // - 16-bit BGRx format, [15:0] B:G:R:x 4:4:4:4 little endian
	WAYLAND_SHM_FORMAT_ARGB4444             = 0x32315241, // - 16-bit ARGB format, [15:0] A:R:G:B 4:4:4:4 little endian
	WAYLAND_SHM_FORMAT_ABGR4444             = 0x32314241, // - 16-bit ABGR format, [15:0] A:B:G:R 4:4:4:4 little endian
	WAYLAND_SHM_FORMAT_RGBA4444             = 0x32314152, // - 16-bit RBGA format, [15:0] R:G:B:A 4:4:4:4 little endian
	WAYLAND_SHM_FORMAT_BGRA4444             = 0x32314142, // - 16-bit BGRA format, [15:0] B:G:R:A 4:4:4:4 little endian
	WAYLAND_SHM_FORMAT_XRGB1555             = 0x35315258, // - 16-bit xRGB format, [15:0] x:R:G:B 1:5:5:5 little endian
	WAYLAND_SHM_FORMAT_XBGR1555             = 0x35314258, // - 16-bit xBGR 1555 format, [15:0] x:B:G:R 1:5:5:5 little endian
	WAYLAND_SHM_FORMAT_RGBX5551             = 0x35315852, // - 16-bit RGBx 5551 format, [15:0] R:G:B:x 5:5:5:1 little endian
	WAYLAND_SHM_FORMAT_BGRX5551             = 0x35315842, // - 16-bit BGRx 5551 format, [15:0] B:G:R:x 5:5:5:1 little endian
	WAYLAND_SHM_FORMAT_ARGB1555             = 0x35315241, // - 16-bit ARGB 1555 format, [15:0] A:R:G:B 1:5:5:5 little endian
	WAYLAND_SHM_FORMAT_ABGR1555             = 0x35314241, // - 16-bit ABGR 1555 format, [15:0] A:B:G:R 1:5:5:5 little endian
	WAYLAND_SHM_FORMAT_RGBA5551             = 0x35314152, // - 16-bit RGBA 5551 format, [15:0] R:G:B:A 5:5:5:1 little endian
	WAYLAND_SHM_FORMAT_BGRA5551             = 0x35314142, // - 16-bit BGRA 5551 format, [15:0] B:G:R:A 5:5:5:1 little endian
	WAYLAND_SHM_FORMAT_RGB565               = 0x36314752, // - 16-bit RGB 565 format, [15:0] R:G:B 5:6:5 little endian
	WAYLAND_SHM_FORMAT_BGR565               = 0x36314742, // - 16-bit BGR 565 format, [15:0] B:G:R 5:6:5 little endian
	WAYLAND_SHM_FORMAT_RGB888               = 0x34324752, // - 24-bit RGB format, [23:0] R:G:B little endian
	WAYLAND_SHM_FORMAT_BGR888               = 0x34324742, // - 24-bit BGR format, [23:0] B:G:R little endian
	WAYLAND_SHM_FORMAT_XBGR8888             = 0x34324258, // - 32-bit xBGR format, [31:0] x:B:G:R 8:8:8:8 little endian
	WAYLAND_SHM_FORMAT_RGBX8888             = 0x34325852, // - 32-bit RGBx format, [31:0] R:G:B:x 8:8:8:8 little endian
	WAYLAND_SHM_FORMAT_BGRX8888             = 0x34325842, // - 32-bit BGRx format, [31:0] B:G:R:x 8:8:8:8 little endian
	WAYLAND_SHM_FORMAT_ABGR8888             = 0x34324241, // - 32-bit ABGR format, [31:0] A:B:G:R 8:8:8:8 little endian
	WAYLAND_SHM_FORMAT_RGBA8888             = 0x34324152, // - 32-bit RGBA format, [31:0] R:G:B:A 8:8:8:8 little endian
	WAYLAND_SHM_FORMAT_BGRA8888             = 0x34324142, // - 32-bit BGRA format, [31:0] B:G:R:A 8:8:8:8 little endian
	WAYLAND_SHM_FORMAT_XRGB2101010          = 0x30335258, // - 32-bit xRGB format, [31:0] x:R:G:B 2:10:10:10 little endian
	WAYLAND_SHM_FORMAT_XBGR2101010          = 0x30334258, // - 32-bit xBGR format, [31:0] x:B:G:R 2:10:10:10 little endian
	WAYLAND_SHM_FORMAT_RGBX1010102          = 0x30335852, // - 32-bit RGBx format, [31:0] R:G:B:x 10:10:10:2 little endian
	WAYLAND_SHM_FORMAT_BGRX1010102          = 0x30335842, // - 32-bit BGRx format, [31:0] B:G:R:x 10:10:10:2 little endian
	WAYLAND_SHM_FORMAT_ARGB2101010          = 0x30335241, // - 32-bit ARGB format, [31:0] A:R:G:B 2:10:10:10 little endian
	WAYLAND_SHM_FORMAT_ABGR2101010          = 0x30334241, // - 32-bit ABGR format, [31:0] A:B:G:R 2:10:10:10 little endian
	WAYLAND_SHM_FORMAT_RGBA1010102          = 0x30334152, // - 32-bit RGBA format, [31:0] R:G:B:A 10:10:10:2 little endian
	WAYLAND_SHM_FORMAT_BGRA1010102          = 0x30334142, // - 32-bit BGRA format, [31:0] B:G:R:A 10:10:10:2 little endian
	WAYLAND_SHM_FORMAT_YUYV                 = 0x56595559, // - packed YCbCr format, [31:0] Cr0:Y1:Cb0:Y0 8:8:8:8 little endian
	WAYLAND_SHM_FORMAT_YVYU                 = 0x55595659, // - packed YCbCr format, [31:0] Cb0:Y1:Cr0:Y0 8:8:8:8 little endian
	WAYLAND_SHM_FORMAT_UYVY                 = 0x59565955, // - packed YCbCr format, [31:0] Y1:Cr0:Y0:Cb0 8:8:8:8 little endian
	WAYLAND_SHM_FORMAT_VYUY                 = 0x59555956, // - packed YCbCr format, [31:0] Y1:Cb0:Y0:Cr0 8:8:8:8 little endian
	WAYLAND_SHM_FORMAT_AYUV                 = 0x56555941, // - packed AYCbCr format, [31:0] A:Y:Cb:Cr 8:8:8:8 little endian
	WAYLAND_SHM_FORMAT_NV12                 = 0x3231564e, // - 2 plane YCbCr Cr:Cb format, 2x2 subsampled Cr:Cb plane
	WAYLAND_SHM_FORMAT_NV21                 = 0x3132564e, // - 2 plane YCbCr Cb:Cr format, 2x2 subsampled Cb:Cr plane
	WAYLAND_SHM_FORMAT_NV16                 = 0x3631564e, // - 2 plane YCbCr Cr:Cb format, 2x1 subsampled Cr:Cb plane
	WAYLAND_SHM_FORMAT_NV61                 = 0x3136564e, // - 2 plane YCbCr Cb:Cr format, 2x1 subsampled Cb:Cr plane
	WAYLAND_SHM_FORMAT_YUV410               = 0x39565559, // - 3 plane YCbCr format, 4x4 subsampled Cb (1) and Cr (2) planes
	WAYLAND_SHM_FORMAT_YVU410               = 0x39555659, // - 3 plane YCbCr format, 4x4 subsampled Cr (1) and Cb (2) planes
	WAYLAND_SHM_FORMAT_YUV411               = 0x31315559, // - 3 plane YCbCr format, 4x1 subsampled Cb (1) and Cr (2) planes
	WAYLAND_SHM_FORMAT_YVU411               = 0x31315659, // - 3 plane YCbCr format, 4x1 subsampled Cr (1) and Cb (2) planes
	WAYLAND_SHM_FORMAT_YUV420               = 0x32315559, // - 3 plane YCbCr format, 2x2 subsampled Cb (1) and Cr (2) planes
	WAYLAND_SHM_FORMAT_YVU420               = 0x32315659, // - 3 plane YCbCr format, 2x2 subsampled Cr (1) and Cb (2) planes
	WAYLAND_SHM_FORMAT_YUV422               = 0x36315559, // - 3 plane YCbCr format, 2x1 subsampled Cb (1) and Cr (2) planes
	WAYLAND_SHM_FORMAT_YVU422               = 0x36315659, // - 3 plane YCbCr format, 2x1 subsampled Cr (1) and Cb (2) planes
	WAYLAND_SHM_FORMAT_YUV444               = 0x34325559, // - 3 plane YCbCr format, non-subsampled Cb (1) and Cr (2) planes
	WAYLAND_SHM_FORMAT_YVU444               = 0x34325659, // - 3 plane YCbCr format, non-subsampled Cr (1) and Cb (2) planes
	WAYLAND_SHM_FORMAT_R8                   = 0x20203852, // - [7:0] R
	WAYLAND_SHM_FORMAT_R16                  = 0x20363152, // - [15:0] R little endian
	WAYLAND_SHM_FORMAT_RG88                 = 0x38384752, // - [15:0] R:G 8:8 little endian
	WAYLAND_SHM_FORMAT_GR88                 = 0x38385247, // - [15:0] G:R 8:8 little endian
	WAYLAND_SHM_FORMAT_RG1616               = 0x32334752, // - [31:0] R:G 16:16 little endian
	WAYLAND_SHM_FORMAT_GR1616               = 0x32335247, // - [31:0] G:R 16:16 little endian
	WAYLAND_SHM_FORMAT_XRGB16161616F        = 0x48345258, // - [63:0] x:R:G:B 16:16:16:16 little endian
	WAYLAND_SHM_FORMAT_XBGR16161616F        = 0x48344258, // - [63:0] x:B:G:R 16:16:16:16 little endian
	WAYLAND_SHM_FORMAT_ARGB16161616F        = 0x48345241, // - [63:0] A:R:G:B 16:16:16:16 little endian
	WAYLAND_SHM_FORMAT_ABGR16161616F        = 0x48344241, // - [63:0] A:B:G:R 16:16:16:16 little endian
	WAYLAND_SHM_FORMAT_XYUV8888             = 0x56555958, // - [31:0] X:Y:Cb:Cr 8:8:8:8 little endian
	WAYLAND_SHM_FORMAT_VUY888               = 0x34325556, // - [23:0] Cr:Cb:Y 8:8:8 little endian
	WAYLAND_SHM_FORMAT_VUY101010            = 0x30335556, // - Y followed by U then V, 10:10:10. Non-linear modifier only
	WAYLAND_SHM_FORMAT_Y210                 = 0x30313259, // - [63:0] Cr0:0:Y1:0:Cb0:0:Y0:0 10:6:10:6:10:6:10:6 little endian per 2 Y pixels
	WAYLAND_SHM_FORMAT_Y212                 = 0x32313259, // - [63:0] Cr0:0:Y1:0:Cb0:0:Y0:0 12:4:12:4:12:4:12:4 little endian per 2 Y pixels
	WAYLAND_SHM_FORMAT_Y216                 = 0x36313259, // - [63:0] Cr0:Y1:Cb0:Y0 16:16:16:16 little endian per 2 Y pixels
	WAYLAND_SHM_FORMAT_Y410                 = 0x30313459, // - [31:0] A:Cr:Y:Cb 2:10:10:10 little endian
	WAYLAND_SHM_FORMAT_Y412                 = 0x32313459, // - [63:0] A:0:Cr:0:Y:0:Cb:0 12:4:12:4:12:4:12:4 little endian
	WAYLAND_SHM_FORMAT_Y416                 = 0x36313459, // - [63:0] A:Cr:Y:Cb 16:16:16:16 little endian
	WAYLAND_SHM_FORMAT_XVYU2101010          = 0x30335658, // - [31:0] X:Cr:Y:Cb 2:10:10:10 little endian
	WAYLAND_SHM_FORMAT_XVYU12_16161616      = 0x36335658, // - [63:0] X:0:Cr:0:Y:0:Cb:0 12:4:12:4:12:4:12:4 little endian
	WAYLAND_SHM_FORMAT_XVYU16161616         = 0x38345658, // - [63:0] X:Cr:Y:Cb 16:16:16:16 little endian
	WAYLAND_SHM_FORMAT_Y0L0                 = 0x304c3059, // - [63:0] A3:A2:Y3:0:Cr0:0:Y2:0:A1:A0:Y1:0:Cb0:0:Y0:0 1:1:8:2:8:2:8:2:1:1:8:2:8:2:8:2 little endian
	WAYLAND_SHM_FORMAT_X0L0                 = 0x304c3058, // - [63:0] X3:X2:Y3:0:Cr0:0:Y2:0:X1:X0:Y1:0:Cb0:0:Y0:0 1:1:8:2:8:2:8:2:1:1:8:2:8:2:8:2 little endian
	WAYLAND_SHM_FORMAT_Y0L2                 = 0x324c3059, // - [63:0] A3:A2:Y3:Cr0:Y2:A1:A0:Y1:Cb0:Y0 1:1:10:10:10:1:1:10:10:10 little endian
	WAYLAND_SHM_FORMAT_X0L2                 = 0x324c3058, // - [63:0] X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0 1:1:10:10:10:1:1:10:10:10 little endian
	WAYLAND_SHM_FORMAT_YUV420_8BIT          = 0x38305559, //
	WAYLAND_SHM_FORMAT_YUV420_10BIT         = 0x30315559, //
	WAYLAND_SHM_FORMAT_XRGB8888_A8          = 0x38415258, //
	WAYLAND_SHM_FORMAT_XBGR8888_A8          = 0x38414258, //
	WAYLAND_SHM_FORMAT_RGBX8888_A8          = 0x38415852, //
	WAYLAND_SHM_FORMAT_BGRX8888_A8          = 0x38415842, //
	WAYLAND_SHM_FORMAT_RGB888_A8            = 0x38413852, //
	WAYLAND_SHM_FORMAT_BGR888_A8            = 0x38413842, //
	WAYLAND_SHM_FORMAT_RGB565_A8            = 0x38413552, //
	WAYLAND_SHM_FORMAT_BGR565_A8            = 0x38413542, //
	WAYLAND_SHM_FORMAT_NV24                 = 0x3432564e, // - non-subsampled Cr:Cb plane
	WAYLAND_SHM_FORMAT_NV42                 = 0x3234564e, // - non-subsampled Cb:Cr plane
	WAYLAND_SHM_FORMAT_P210                 = 0x30313250, // - 2x1 subsampled Cr:Cb plane, 10 bit per channel
	WAYLAND_SHM_FORMAT_P010                 = 0x30313050, // - 2x2 subsampled Cr:Cb plane 10 bits per channel
	WAYLAND_SHM_FORMAT_P012                 = 0x32313050, // - 2x2 subsampled Cr:Cb plane 12 bits per channel
	WAYLAND_SHM_FORMAT_P016                 = 0x36313050, // - 2x2 subsampled Cr:Cb plane 16 bits per channel
	WAYLAND_SHM_FORMAT_AXBXGXRX106106106106 = 0x30314241, // - [63:0] A:x:B:x:G:x:R:x 10:6:10:6:10:6:10:6 little endian
	WAYLAND_SHM_FORMAT_NV15                 = 0x3531564e, // - 2x2 subsampled Cr:Cb plane
	WAYLAND_SHM_FORMAT_Q410                 = 0x30313451, //
	WAYLAND_SHM_FORMAT_Q401                 = 0x31303451, //
	WAYLAND_SHM_FORMAT_XRGB16161616         = 0x38345258, // - [63:0] x:R:G:B 16:16:16:16 little endian
	WAYLAND_SHM_FORMAT_XBGR16161616         = 0x38344258, // - [63:0] x:B:G:R 16:16:16:16 little endian
	WAYLAND_SHM_FORMAT_ARGB16161616         = 0x38345241, // - [63:0] A:R:G:B 16:16:16:16 little endian
	WAYLAND_SHM_FORMAT_ABGR16161616         = 0x38344241, // - [63:0] A:B:G:R 16:16:16:16 little endian
	WAYLAND_SHM_FORMAT_C1                   = 0x20203143, // - [7:0] C0:C1:C2:C3:C4:C5:C6:C7 1:1:1:1:1:1:1:1 eight pixels/byte
	WAYLAND_SHM_FORMAT_C2                   = 0x20203243, // - [7:0] C0:C1:C2:C3 2:2:2:2 four pixels/byte
	WAYLAND_SHM_FORMAT_C4                   = 0x20203443, // - [7:0] C0:C1 4:4 two pixels/byte
	WAYLAND_SHM_FORMAT_D1                   = 0x20203144, // - [7:0] D0:D1:D2:D3:D4:D5:D6:D7 1:1:1:1:1:1:1:1 eight pixels/byte
	WAYLAND_SHM_FORMAT_D2                   = 0x20203244, // - [7:0] D0:D1:D2:D3 2:2:2:2 four pixels/byte
	WAYLAND_SHM_FORMAT_D4                   = 0x20203444, // - [7:0] D0:D1 4:4 two pixels/byte
	WAYLAND_SHM_FORMAT_D8                   = 0x20203844, // - [7:0] D
	WAYLAND_SHM_FORMAT_R1                   = 0x20203152, // - [7:0] R0:R1:R2:R3:R4:R5:R6:R7 1:1:1:1:1:1:1:1 eight pixels/byte
	WAYLAND_SHM_FORMAT_R2                   = 0x20203252, // - [7:0] R0:R1:R2:R3 2:2:2:2 four pixels/byte
	WAYLAND_SHM_FORMAT_R4                   = 0x20203452, // - [7:0] R0:R1 4:4 two pixels/byte
	WAYLAND_SHM_FORMAT_R10                  = 0x20303152, // - [15:0] x:R 6:10 little endian
	WAYLAND_SHM_FORMAT_R12                  = 0x20323152, // - [15:0] x:R 4:12 little endian
	WAYLAND_SHM_FORMAT_AVUY8888             = 0x59555641, // - [31:0] A:Cr:Cb:Y 8:8:8:8 little endian
	WAYLAND_SHM_FORMAT_XVUY8888             = 0x59555658, // - [31:0] X:Cr:Cb:Y 8:8:8:8 little endian
	WAYLAND_SHM_FORMAT_P030                 = 0x30333050, // - 2x2 subsampled Cr:Cb plane 10 bits per channel packed
	// clang-format on
};

#define WL_BUFFER ((WlBuffer, wl_buffer, 0), D((Destroy)), E((Release)))

wayland_prototype Wayland_DataOffer_Prototype = {
	.Name		  = "wl_data_offer",
	.Version	  = 4,
	.RequestCount = 5,
	.EventCount	  = 3,
	.Requests =
		M{
		  { "accept", "ous?", NULL },
		  { "receive", "osf", NULL },
		  { "destroy", "d", NULL },
		  { "finish", "3o", NULL },
		  { "set_actions", "3ouu", NULL },
		  },
	.Events =
		M{
		  { "offer", "os", NULL },
		  { "source_actions", "3ou", NULL },
		  { "action", "3o", NULL },
		  },
};

enum wayland_data_offer_error {
	WAYLAND_DATA_OFFER_ERROR_INVALID_FINISH		 = 0,
	WAYLAND_DATA_OFFER_ERROR_INVALID_ACTION_MASK = 1,
	WAYLAND_DATA_OFFER_ERROR_INVALID_ACTION		 = 2,
	WAYLAND_DATA_OFFER_ERROR_INVALID_OFFER		 = 3,
};

wayland_prototype Wayland_DataSource_Prototype = {
	.Name		  = "wl_data_source",
	.Version	  = 4,
	.RequestCount = 3,
	.EventCount	  = 6,
	.Requests =
		M{
		  { "offer", "os", NULL },
		  { "destroy", "d", NULL },
		  { "set_actions", "3ou", NULL },
		  },
	.Events =
		M{
		  { "target", "os?", NULL },
		  { "send", "osf", NULL },
		  { "cancelled", "3o", NULL },
		  { "dnd_drop_performed", "3o", NULL },
		  { "dnd_finished", "3o", NULL },
		  { "action", "3ou", NULL },
		  },
};

enum wayland_data_source_error {
	WAYLAND_DATA_SOURCE_ERROR_INVALID_ACTION_MASK = 0,
	WAYLAND_DATA_SOURCE_ERROR_INVALID_SOURCE	  = 1,
};

wayland_prototype Wayland_DataDevice_Prototype = {
	.Name		  = "wl_data_device",
	.Version	  = 4,
	.RequestCount = 3,
	.EventCount	  = 6,
	.Requests =
		M{
		  { "start_drag", "oo?oo?u", NULL },
		  { "set_selection", "oo?u", NULL },
		  { "release", "2d", NULL },
		  },
	.Events =
		M{
		  { "data_offer", "on", P{ &Wayland_DataOffer_Prototype } },
		  { "enter", "ouorro?", NULL },
		  { "leave", "o", NULL },
		  { "motion", "ourr", NULL },
		  { "drop", "o", NULL },
		  { "selection", "oo?", NULL },
		  },
};

enum wayland_data_device_error {
	WAYLAND_DATA_DEVICE_ERROR_ROLE		  = 0,
	WAYLAND_DATA_DEVICE_ERROR_USED_SOURCE = 1,
};

wayland_prototype Wayland_DataDeviceManager_Prototype = {
	.Name		  = "wl_data_device_manager",
	.Version	  = 4,
	.RequestCount = 3,
	.Requests =
		M{
		  { "create_data_source", "on", P{ &Wayland_DataSource_Prototype } },
		  { "get_data_device", "ono", P{ &Wayland_DataDevice_Prototype } },
		  { "release", "4d", NULL },
		  },
};

enum wayland_data_device_manager_drag_and_drop_action {
	WAYLAND_DATA_DEVICE_MANAGER_DRAG_AND_DROP_ACTION_NONE = 0x0,
	WAYLAND_DATA_DEVICE_MANAGER_DRAG_AND_DROP_ACTION_COPY = 0x1,
	WAYLAND_DATA_DEVICE_MANAGER_DRAG_AND_DROP_ACTION_MOVE = 0x2,
	WAYLAND_DATA_DEVICE_MANAGER_DRAG_AND_DROP_ACTION_ASK  = 0x4,
};

wayland_prototype Wayland_Shell_Prototype = {
	.Name		  = "wl_shell",
	.Version	  = 1,
	.Deprecated	  = TRUE,
	.RequestCount = 1,
	.Requests =
		M{
		  { "get_shell_surface",
				"-1ono",
				P{ &Wayland_ShellSurface_Prototype } },
		  },
};

enum wayland_shell_error {
	WAYLAND_SHELL_ERROR_ROLE = 0,
};

wayland_prototype Wayland_ShellSurface_Prototype = {
	.Name		  = "wl_shell_surface",
	.Version	  = 1,
	.Deprecated	  = TRUE,
	.RequestCount = 10,
	.EventCount	  = 3,
	.Requests =
		M{
		  { "pong", "-1ou", NULL },
		  { "move", "-1oou", NULL },
		  { "resize", "-1oouu", NULL },
		  { "set_toplevel", "-1o", NULL },
		  { "set_transient", "-1ooiiu", NULL },
		  { "set_fullscreen", "-1ouuo?", NULL },
		  { "set_popup", "-1oouoiiu", NULL },
		  { "set_maximized", "-1oo?", NULL },
		  { "set_title", "-1os", NULL },
		  { "set_class", "-1os", NULL },
		  },
	.Events =
		M{
		  { "ping", "-1ou", NULL },
		  { "configure", "-1ouii", NULL },
		  { "popup_done", "-1o", NULL },
		  },
};

enum wayland_shell_surface_resize {
	WAYLAND_SHELL_SURFACE_RESIZE_NONE		  = 0,
	WAYLAND_SHELL_SURFACE_RESIZE_TOP		  = 1,
	WAYLAND_SHELL_SURFACE_RESIZE_BOTTOM		  = 2,
	WAYLAND_SHELL_SURFACE_RESIZE_LEFT		  = 4,
	WAYLAND_SHELL_SURFACE_RESIZE_TOP_LEFT	  = 5,
	WAYLAND_SHELL_SURFACE_RESIZE_BOTTOM_LEFT  = 6,
	WAYLAND_SHELL_SURFACE_RESIZE_RIGHT		  = 8,
	WAYLAND_SHELL_SURFACE_RESIZE_TOP_RIGHT	  = 9,
	WAYLAND_SHELL_SURFACE_RESIZE_BOTTOM_RIGHT = 10,
};

enum wayland_shell_surface_transient {
	WAYLAND_SHELL_SURFACE_TRANSIENT_INACTIVE = 0x1,
};

enum wayland_shell_surface_fullscreen_method {
	WAYLAND_SHELL_SURFACE_FULLSCREEN_METHOD_DEFAULT = 0,
	WAYLAND_SHELL_SURFACE_FULLSCREEN_METHOD_SCALE	= 1,
	WAYLAND_SHELL_SURFACE_FULLSCREEN_METHOD_DRIVER	= 2,
	WAYLAND_SHELL_SURFACE_FULLSCREEN_METHOD_FILL	= 3,
};

wayland_prototype Wayland_Surface_Prototype = {
	.Name		  = "wl_surface",
	.Version	  = 7,
	.RequestCount = 12,
	.EventCount	  = 4,
	.Requests =
		M{
		  { "destroy", "d", NULL },
		  { "attach", "oo?ii", NULL },
		  { "damage", "oiiii", NULL },
		  { "frame", "on", P{ &Wayland_Callback_Prototype } },
		  { "set_opaque_region", "oo?", NULL },
		  { "set_input_region", "oo?", NULL },
		  { "commit", "o", NULL },
		  { "set_buffer_transform", "2oi", NULL },
		  { "set_buffer_scale", "3oi", NULL },
		  { "damage_buffer", "4oiiii", NULL },
		  { "offset", "5oii", NULL },
		  { "get_release", "7on", P{ &Wayland_Callback_Prototype } },
		  },
	.Events =
		M{
		  { "enter", "oo", NULL },
		  { "leave", "oo", NULL },
		  { "preferred_buffer_scale", "6oi", NULL },
		  { "preferred_buffer_transform", "6ou", NULL },
		  },
};

enum wayland_surface_error {
	WAYLAND_SURFACE_ERROR_INVALID_SCALE		  = 0,
	WAYLAND_SURFACE_ERROR_INVALID_TRANSFORM	  = 1,
	WAYLAND_SURFACE_ERROR_INVALID_SIZE		  = 2,
	WAYLAND_SURFACE_ERROR_INVALID_OFFSET	  = 3,
	WAYLAND_SURFACE_ERROR_DEFUNCT_ROLE_OBJECT = 4,
	WAYLAND_SURFACE_ERROR_NO_BUFFER			  = 5,
};

wayland_prototype Wayland_Seat_Prototype = {
	.Name		  = "wl_seat",
	.Version	  = 11,
	.RequestCount = 4,
	.EventCount	  = 2,
	.Requests =
		M{
		  { "get_pointer", "on", P{ &Wayland_Pointer_Prototype } },
		  { "get_keyboard", "on", P{ &Wayland_Keyboard_Prototype } },
		  { "get_touch", "on", P{ &Wayland_Touch_Prototype } },
		  { "release", "5d", NULL },
		  },
	.Events =
		M{
		  { "capabilities", "ou", NULL },
		  { "name", "2os", NULL },
		  },
};

enum wayland_seat_capability {
	WAYLAND_SEAT_CAPABILITY_POINTER	 = 0x1,
	WAYLAND_SEAT_CAPABILITY_KEYBOARD = 0x2,
	WAYLAND_SEAT_CAPABILITY_TOUCH	 = 0x4,
};

enum wayland_seat_error {
	WAYLAND_SEAT_ERROR_MISSING_CAPABILITY = 0,
};

wayland_prototype Wayland_Pointer_Prototype = {
	.Name		  = "wl_pointer",
	.Version	  = 11,
	.RequestCount = 2,
	.EventCount	  = 12,
	.Requests =
		M{
		  { "set_cursor", "ouo?ii", NULL },
		  { "release", "3d", NULL },
		  },
	.Events =
		M{
		  { "enter", "ouorr", NULL },
		  { "leave", "ouo", NULL },
		  { "motion", "ourr", NULL },
		  { "button", "ouuuu", NULL },
		  { "axis", "ouur", NULL },
		  { "frame", "5o", NULL },
		  { "axis_source", "5ou", NULL },
		  { "axis_stop", "5ouu", NULL },
		  { "axis_discrete", "5-7oui", NULL },
		  { "axis_value120", "8oui", NULL },
		  { "axis_relative_direction", "9ouu", NULL },
		  { "warp", "11orr", NULL },
		  },
};

enum wayland_pointer_error {
	WAYLAND_POINTER_ERROR_ROLE = 0,
};

enum wayland_pointer_button_state {
	WAYLAND_POINTER_BUTTON_STATE_RELEASED = 0,
	WAYLAND_POINTER_BUTTON_STATE_PRESSED  = 1,
};

enum wayland_pointer_axis {
	WAYLAND_POINTER_AXIS_VERTICAL_SCROLL   = 0,
	WAYLAND_POINTER_AXIS_HORIZONTAL_SCROLL = 1,
};

enum wayland_pointer_axis_source {
	WAYLAND_POINTER_AXIS_SOURCE_WHEEL	   = 0,
	WAYLAND_POINTER_AXIS_SOURCE_FINGER	   = 1,
	WAYLAND_POINTER_AXIS_SOURCE_CONTINUOUS = 2,
	WAYLAND_POINTER_AXIS_SOURCE_WHEEL_TILT = 3,
};

enum wayland_pointer_axis_relative_direction {
	WAYLAND_POINTER_AXIS_RELATIVE_DIRECTION_IDENTICAL = 0,
	WAYLAND_POINTER_AXIS_RELATIVE_DIRECTION_INVERTED  = 1,
};

wayland_prototype Wayland_Keyboard_Prototype = {
	.Name		  = "wl_keyboard",
	.Version	  = 11,
	.RequestCount = 1,
	.EventCount	  = 6,
	.Requests =
		M{
		  { "release", "3d", NULL },
		  },
	.Events =
		M{
		  { "keymap", "oufu", NULL },
		  { "enter", "ouoa", NULL },
		  { "leave", "ouo", NULL },
		  { "key", "ouuuu", NULL },
		  { "modifiers", "ouuuuu", NULL },
		  { "repeat_info", "4oii", NULL },
		  },
};

enum wayland_keyboard_keymap_format {
	WAYLAND_KEYBOARD_KEYMAP_FORMAT_NO_KEYMAP = 0,
	WAYLAND_KEYBOARD_KEYMAP_FORMAT_XKB_V1	 = 1,
};

enum wayland_keyboard_key_state {
	WAYLAND_KEYBOARD_KEY_STATE_RELEASED = 0,
	WAYLAND_KEYBOARD_KEY_STATE_PRESSED	= 1,
};

wayland_prototype Wayland_Touch_Prototype = {
	.Name		  = "wl_touch",
	.Version	  = 11,
	.RequestCount = 1,
	.EventCount	  = 7,
	.Requests =
		M{
		  { "release", "3d", NULL },
		  },
	.Events =
		M{
		  { "down", "ouuoirr", NULL },
		  { "up", "ouui", NULL },
		  { "motion", "ouirr", NULL },
		  { "frame", "o", NULL },
		  { "cancel", "o", NULL },
		  { "shape", "6oirr", NULL },
		  { "orientation", "6oir", NULL },
		  },
};

wayland_prototype Wayland_Output_Prototype = {
	.Name		  = "wl_output",
	.Version	  = 4,
	.RequestCount = 1,
	.EventCount	  = 6,
	.Requests =
		M{
		  { "release", "3d", NULL },
		  },
	.Events =
		M{
		  { "geometry", "oiiiiissi", NULL },
		  { "mode", "ouiii", NULL },
		  { "done", "2o", NULL },
		  { "scale", "2oi", NULL },
		  { "name", "4os", NULL },
		  { "description", "4os", NULL },
		  },
};

enum wayland_output_subpixel {
	WAYLAND_OUTPUT_SUBPIXEL_UNKNOWN		   = 0,
	WAYLAND_OUTPUT_SUBPIXEL_NONE		   = 1,
	WAYLAND_OUTPUT_SUBPIXEL_HORIZONTAL_RGB = 2,
	WAYLAND_OUTPUT_SUBPIXEL_HORIZONTAL_BGR = 3,
	WAYLAND_OUTPUT_SUBPIXEL_VERTICAL_RGB   = 4,
	WAYLAND_OUTPUT_SUBPIXEL_VERTICAL_BGR   = 5,
};

enum wayland_output_transform {
	WAYLAND_OUTPUT_TRANSFORM_NORMAL		 = 0,
	WAYLAND_OUTPUT_TRANSFORM_90			 = 1,
	WAYLAND_OUTPUT_TRANSFORM_180		 = 2,
	WAYLAND_OUTPUT_TRANSFORM_270		 = 3,
	WAYLAND_OUTPUT_TRANSFORM_FLIPPED	 = 4,
	WAYLAND_OUTPUT_TRANSFORM_FLIPPED_90	 = 5,
	WAYLAND_OUTPUT_TRANSFORM_FLIPPED_180 = 6,
	WAYLAND_OUTPUT_TRANSFORM_FLIPPED_270 = 7,
};

enum wayland_output_mode {
	WAYLAND_OUTPUT_MODE_CURRENT	  = 1,
	WAYLAND_OUTPUT_MODE_PREFERRED = 2,
};

wayland_prototype Wayland_Region_Prototype = {
	.Name		  = "wl_region",
	.Version	  = 7,
	.RequestCount = 3,
	.Requests =
		M{
		  { "destroy", "d", NULL },
		  { "add", "oiiii", NULL },
		  { "subtract", "oiiii", NULL },
		  },
};

wayland_prototype Wayland_Subcompositor_Prototype = {
	.Name		  = "wl_subcompositor",
	.Version	  = 1,
	.RequestCount = 2,
	.Requests =
		M{
		  { "destroy", "d", NULL },
		  { "get_subsurface", "onoo", P{ &Wayland_Subsurface_Prototype } },
		  },
};

enum wayland_subcompositor_error {
	WAYLAND_SUBCOMPOSITOR_ERROR_BAD_SURFACE = 0,
	WAYLAND_SUBCOMPOSITOR_ERROR_BAD_PARENT	= 1,
};

wayland_prototype Wayland_Subsurface_Prototype = {
	.Name		  = "wl_subsurface",
	.Version	  = 1,
	.RequestCount = 6,
	.Requests =
		M{
		  { "destroy", "d", NULL },
		  { "set_position", "oii", NULL },
		  { "place_above", "oo", NULL },
		  { "place_below", "oo", NULL },
		  { "set_sync", "o", NULL },
		  { "set_desync", "o", NULL },
		  },
};

enum wayland_subsurface_error {
	WAYLAND_SUBSURFACE_ERROR_BAD_SURFACE = 0,
};

wayland_prototype Wayland_Fixes_Prototype = {
	.Name		  = "wl_fixes",
	.Version	  = 2,
	.RequestCount = 3,
	.Requests =
		M{
		  { "destroy", "d", NULL },
		  { "destroy_registry", "od", NULL },
		  { "ack_global_remove", "2oou", NULL },
		  },
};

enum wayland_fixes_error {
	WAYLAND_FIXES_ERROR_INVALID_ACK_REMOVE = 0,
};

WLI_DECLARE(WL_DISPLAY)
WLI_DECLARE(WL_REGISTRY)
WLI_DECLARE(WL_CALLBACK)
WLI_DECLARE(WL_COMPOSITOR)
WLI_DECLARE(WL_SHM_POOL)
WLI_DECLARE(WL_SHM)
WLI_DECLARE(WL_BUFFER)
WLI_DECLARE(WL_DATA_OFFER)
WLI_DECLARE(WL_DATA_SOURCE)
WLI_DECLARE(WL_DATA_DEVICE)
WLI_DECLARE(WL_DATA_DEVICE_MANAGER)
WLI_DECLARE(WL_SHELL)
WLI_DECLARE(WL_SHELL_SURFACE)
WLI_DECLARE(WL_SURFACE)
WLI_DECLARE(WL_SEAT)
WLI_DECLARE(WL_POINTER)
WLI_DECLARE(WL_KEYBOARD)
WLI_DECLARE(WL_TOUCH)
WLI_DECLARE(WL_OUTPUT)
WLI_DECLARE(WL_REGION)
WLI_DECLARE(WL_SUBCOMPOSITOR)
WLI_DECLARE(WL_SUBSURFACE)
WLI_DECLARE(WL_FIXES)
