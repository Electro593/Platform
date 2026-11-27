/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                             *
*  Author: Aria Seiler                                                        *
*                                                                             *
*  This program is in the public domain. There is no implied warranty, so use *
*  it at your own risk.                                                       *
*                                                                             *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#pragma once

#include "wli.h"
#include "wire.h"

#define WL_DISPLAY                                    \
	((WlDisplay, wl_display, 1),                      \
		R((Sync), N(WL_CALLBACK)),                    \
		R((GetRegistry), N(WL_REGISTRY)),             \
		E((Error), O(Object, WLI_INTERFACE), U(Code), S(Message)), \
		E((DeleteId), U(Id)))

typedef enum wl_display_error : u32 {
	WL_DISPLAY_ERROR_INVALID_OBJECT = 0,
	WL_DISPLAY_ERROR_INVALID_METHOD = 1,
	WL_DISPLAY_ERROR_NO_MEMORY		= 2,
	WL_DISPLAY_ERROR_IMPLEMENTATION = 3,
} wl_display_error;

#define WL_REGISTRY                                     \
	((WlRegistry, wl_registry, 1),                      \
		R((Bind), U(Name), N()),                        \
		E((Global), U(Name), S(Interface), U(Version)), \
		E((GlobalRemove), U(Name)))

#define WL_CALLBACK ((WlCallback, wl_callback, 0), C((Done), U(CallbackData)))

#define WL_COMPOSITOR                      \
	((WlCompositor, wl_compostitor, 7),    \
		R((CreateSurface), N(WL_SURFACE)), \
		R((CreateRegion), N(WL_REGION)),   \
		D((Release, 7)))

#define WL_SHM_POOL                    \
	((WlShmPool, wl_shm_pool, 3),      \
		R((CreateBuffer),              \
			N(WL_BUFFER),              \
			I(Offset),                 \
			I(Width),                  \
			I(Height),                 \
			I(Stride),                 \
			U(Format, wl_shm_format)), \
		D((Destroy)),                  \
		R((Resize), I(Size)))

typedef enum wl_shm_pool_error : u32 {
	WL_SHM_POOL_ERROR_INVALID_FORMAT = 0,  // v3
	WL_SHM_POOL_ERROR_INVALID_STRIDE = 1,  // v3
} wl_shm_pool_error;

#define WL_SHM                                           \
	((WlShm, wl_shm, 3),                                 \
		R((CreatePool), N(WL_SHM_POOL), F(Fd), I(Size)), \
		D((Release, 2)),                                 \
		E((Format), U(Format, wl_shm_format)))

typedef enum wl_shm_error : u32 {
	WL_SHM_ERROR_INVALID_FORMAT			 = 0,  // v1-v2
	WL_SHM_ERROR_INVALID_STRIDE			 = 1,
	WL_SHM_ERROR_INVALID_FILE_DESCRIPTOR = 2,
} wl_shm_error;

typedef enum wl_shm_format : u32 {
	// clang-format off
	WL_SHM_FORMAT_ARGB8888             = 0,           // 32-bit ARGB format, [31:0] A:R:G:B 8:8:8:8 little endian
	WL_SHM_FORMAT_XRGB8888             = 1,           // 32-bit RGB format, [31:0] x:R:G:B 8:8:8:8 little endian
	WL_SHM_FORMAT_C8                   = 0x20203843,  // 8-bit color index format, [7:0] C
	WL_SHM_FORMAT_RGB332               = 0x38424752,  // 8-bit RGB format, [7:0] R:G:B 3:3:2
	WL_SHM_FORMAT_BGR233               = 0x38524742,  // 8-bit BGR format, [7:0] B:G:R 2:3:3
	WL_SHM_FORMAT_XRGB4444             = 0x32315258,  // 16-bit xRGB format, [15:0] x:R:G:B 4:4:4:4 little endian
	WL_SHM_FORMAT_XBGR4444             = 0x32314258,  // 16-bit xBGR format, [15:0] x:B:G:R 4:4:4:4 little endian
	WL_SHM_FORMAT_RGBX4444             = 0x32315852,  // 16-bit RGBx format, [15:0] R:G:B:x 4:4:4:4 little endian
	WL_SHM_FORMAT_BGRX4444             = 0x32315842,  // 16-bit BGRx format, [15:0] B:G:R:x 4:4:4:4 little endian
	WL_SHM_FORMAT_ARGB4444             = 0x32315241,  // 16-bit ARGB format, [15:0] A:R:G:B 4:4:4:4 little endian
	WL_SHM_FORMAT_ABGR4444             = 0x32314241,  // 16-bit ABGR format, [15:0] A:B:G:R 4:4:4:4 little endian
	WL_SHM_FORMAT_RGBA4444             = 0x32314152,  // 16-bit RBGA format, [15:0] R:G:B:A 4:4:4:4 little endian
	WL_SHM_FORMAT_BGRA4444             = 0x32314142,  // 16-bit BGRA format, [15:0] B:G:R:A 4:4:4:4 little endian
	WL_SHM_FORMAT_XRGB1555             = 0x35315258,  // 16-bit xRGB format, [15:0] x:R:G:B 1:5:5:5 little endian
	WL_SHM_FORMAT_XBGR1555             = 0x35314258,  // 16-bit xBGR 1555 format, [15:0] x:B:G:R 1:5:5:5 little endian
	WL_SHM_FORMAT_RGBX5551             = 0x35315852,  // 16-bit RGBx 5551 format, [15:0] R:G:B:x 5:5:5:1 little endian
	WL_SHM_FORMAT_BGRX5551             = 0x35315842,  // 16-bit BGRx 5551 format, [15:0] B:G:R:x 5:5:5:1 little endian
	WL_SHM_FORMAT_ARGB1555             = 0x35315241,  // 16-bit ARGB 1555 format, [15:0] A:R:G:B 1:5:5:5 little endian
	WL_SHM_FORMAT_ABGR1555             = 0x35314241,  // 16-bit ABGR 1555 format, [15:0] A:B:G:R 1:5:5:5 little endian
	WL_SHM_FORMAT_RGBA5551             = 0x35314152,  // 16-bit RGBA 5551 format, [15:0] R:G:B:A 5:5:5:1 little endian
	WL_SHM_FORMAT_BGRA5551             = 0x35314142,  // 16-bit BGRA 5551 format, [15:0] B:G:R:A 5:5:5:1 little endian
	WL_SHM_FORMAT_RGB565               = 0x36314752,  // 16-bit RGB 565 format, [15:0] R:G:B 5:6:5 little endian
	WL_SHM_FORMAT_BGR565               = 0x36314742,  // 16-bit BGR 565 format, [15:0] B:G:R 5:6:5 little endian
	WL_SHM_FORMAT_RGB888               = 0x34324752,  // 24-bit RGB format, [23:0] R:G:B little endian
	WL_SHM_FORMAT_BGR888               = 0x34324742,  // 24-bit BGR format, [23:0] B:G:R little endian
	WL_SHM_FORMAT_XBGR8888             = 0x34324258,  // 32-bit xBGR format, [31:0] x:B:G:R 8:8:8:8 little endian
	WL_SHM_FORMAT_RGBX8888             = 0x34325852,  // 32-bit RGBx format, [31:0] R:G:B:x 8:8:8:8 little endian
	WL_SHM_FORMAT_BGRX8888             = 0x34325842,  // 32-bit BGRx format, [31:0] B:G:R:x 8:8:8:8 little endian
	WL_SHM_FORMAT_ABGR8888             = 0x34324241,  // 32-bit ABGR format, [31:0] A:B:G:R 8:8:8:8 little endian
	WL_SHM_FORMAT_RGBA8888             = 0x34324152,  // 32-bit RGBA format, [31:0] R:G:B:A 8:8:8:8 little endian
	WL_SHM_FORMAT_BGRA8888             = 0x34324142,  // 32-bit BGRA format, [31:0] B:G:R:A 8:8:8:8 little endian
	WL_SHM_FORMAT_XRGB2101010          = 0x30335258,  // 32-bit xRGB format, [31:0] x:R:G:B 2:10:10:10 little endian
	WL_SHM_FORMAT_XBGR2101010          = 0x30334258,  // 32-bit xBGR format, [31:0] x:B:G:R 2:10:10:10 little endian
	WL_SHM_FORMAT_RGBX1010102          = 0x30335852,  // 32-bit RGBx format, [31:0] R:G:B:x 10:10:10:2 little endian
	WL_SHM_FORMAT_BGRX1010102          = 0x30335842,  // 32-bit BGRx format, [31:0] B:G:R:x 10:10:10:2 little endian
	WL_SHM_FORMAT_ARGB2101010          = 0x30335241,  // 32-bit ARGB format, [31:0] A:R:G:B 2:10:10:10 little endian
	WL_SHM_FORMAT_ABGR2101010          = 0x30334241,  // 32-bit ABGR format, [31:0] A:B:G:R 2:10:10:10 little endian
	WL_SHM_FORMAT_RGBA1010102          = 0x30334152,  // 32-bit RGBA format, [31:0] R:G:B:A 10:10:10:2 little endian
	WL_SHM_FORMAT_BGRA1010102          = 0x30334142,  // 32-bit BGRA format, [31:0] B:G:R:A 10:10:10:2 little endian
	WL_SHM_FORMAT_YUYV                 = 0x56595559,  // packed YCbCr format, [31:0] Cr0:Y1:Cb0:Y0 8:8:8:8 little endian
	WL_SHM_FORMAT_YVYU                 = 0x55595659,  // packed YCbCr format, [31:0] Cb0:Y1:Cr0:Y0 8:8:8:8 little endian
	WL_SHM_FORMAT_UYVY                 = 0x59565955,  // packed YCbCr format, [31:0] Y1:Cr0:Y0:Cb0 8:8:8:8 little endian
	WL_SHM_FORMAT_VYUY                 = 0x59555956,  // packed YCbCr format, [31:0] Y1:Cb0:Y0:Cr0 8:8:8:8 little endian
	WL_SHM_FORMAT_AYUV                 = 0x56555941,  // packed AYCbCr format, [31:0] A:Y:Cb:Cr 8:8:8:8 little endian
	WL_SHM_FORMAT_NV12                 = 0x3231564e,  // 2 plane YCbCr Cr:Cb format, 2x2 subsampled Cr:Cb plane
	WL_SHM_FORMAT_NV21                 = 0x3132564e,  // 2 plane YCbCr Cb:Cr format, 2x2 subsampled Cb:Cr plane
	WL_SHM_FORMAT_NV16                 = 0x3631564e,  // 2 plane YCbCr Cr:Cb format, 2x1 subsampled Cr:Cb plane
	WL_SHM_FORMAT_NV61                 = 0x3136564e,  // 2 plane YCbCr Cb:Cr format, 2x1 subsampled Cb:Cr plane
	WL_SHM_FORMAT_YUV410               = 0x39565559,  // 3 plane YCbCr format, 4x4 subsampled Cb (1) and Cr (2) planes
	WL_SHM_FORMAT_YVU410               = 0x39555659,  // 3 plane YCbCr format, 4x4 subsampled Cr (1) and Cb (2) planes
	WL_SHM_FORMAT_YUV411               = 0x31315559,  // 3 plane YCbCr format, 4x1 subsampled Cb (1) and Cr (2) planes
	WL_SHM_FORMAT_YVU411               = 0x31315659,  // 3 plane YCbCr format, 4x1 subsampled Cr (1) and Cb (2) planes
	WL_SHM_FORMAT_YUV420               = 0x32315559,  // 3 plane YCbCr format, 2x2 subsampled Cb (1) and Cr (2) planes
	WL_SHM_FORMAT_YVU420               = 0x32315659,  // 3 plane YCbCr format, 2x2 subsampled Cr (1) and Cb (2) planes
	WL_SHM_FORMAT_YUV422               = 0x36315559,  // 3 plane YCbCr format, 2x1 subsampled Cb (1) and Cr (2) planes
	WL_SHM_FORMAT_YVU422               = 0x36315659,  // 3 plane YCbCr format, 2x1 subsampled Cr (1) and Cb (2) planes
	WL_SHM_FORMAT_YUV444               = 0x34325559,  // 3 plane YCbCr format, non-subsampled Cb (1) and Cr (2) planes
	WL_SHM_FORMAT_YVU444               = 0x34325659,  // 3 plane YCbCr format, non-subsampled Cr (1) and Cb (2) planes
	WL_SHM_FORMAT_R8                   = 0x20203852,  // [7:0] R
	WL_SHM_FORMAT_R16                  = 0x20363152,  // [15:0] R little endian
	WL_SHM_FORMAT_RG88                 = 0x38384752,  // [15:0] R:G 8:8 little endian
	WL_SHM_FORMAT_GR88                 = 0x38385247,  // [15:0] G:R 8:8 little endian
	WL_SHM_FORMAT_RG1616               = 0x32334752,  // [31:0] R:G 16:16 little endian
	WL_SHM_FORMAT_GR1616               = 0x32335247,  // [31:0] G:R 16:16 little endian
	WL_SHM_FORMAT_XRGB16161616F        = 0x48345258,  // [63:0] x:R:G:B 16:16:16:16 little endian
	WL_SHM_FORMAT_XBGR16161616F        = 0x48344258,  // [63:0] x:B:G:R 16:16:16:16 little endian
	WL_SHM_FORMAT_ARGB16161616F        = 0x48345241,  // [63:0] A:R:G:B 16:16:16:16 little endian
	WL_SHM_FORMAT_ABGR16161616F        = 0x48344241,  // [63:0] A:B:G:R 16:16:16:16 little endian
	WL_SHM_FORMAT_XYUV8888             = 0x56555958,  // [31:0] X:Y:Cb:Cr 8:8:8:8 little endian
	WL_SHM_FORMAT_VUY888               = 0x34325556,  // [23:0] Cr:Cb:Y 8:8:8 little endian
	WL_SHM_FORMAT_VUY101010            = 0x30335556,  // Y followed by U then V, 10:10:10. Non-linear modifier only
	WL_SHM_FORMAT_Y210                 = 0x30313259,  // [63:0] Cr0:0:Y1:0:Cb0:0:Y0:0 10:6:10:6:10:6:10:6 little endian per 2 Y pixels
	WL_SHM_FORMAT_Y212                 = 0x32313259,  // [63:0] Cr0:0:Y1:0:Cb0:0:Y0:0 12:4:12:4:12:4:12:4 little endian per 2 Y pixels
	WL_SHM_FORMAT_Y216                 = 0x36313259,  // [63:0] Cr0:Y1:Cb0:Y0 16:16:16:16 little endian per 2 Y pixels
	WL_SHM_FORMAT_Y410                 = 0x30313459,  // [31:0] A:Cr:Y:Cb 2:10:10:10 little endian
	WL_SHM_FORMAT_Y412                 = 0x32313459,  // [63:0] A:0:Cr:0:Y:0:Cb:0 12:4:12:4:12:4:12:4 little endian
	WL_SHM_FORMAT_Y416                 = 0x36313459,  // [63:0] A:Cr:Y:Cb 16:16:16:16 little endian
	WL_SHM_FORMAT_XVYU2101010          = 0x30335658,  // [31:0] X:Cr:Y:Cb 2:10:10:10 little endian
	WL_SHM_FORMAT_XVYU12_16161616      = 0x36335658,  // [63:0] X:0:Cr:0:Y:0:Cb:0 12:4:12:4:12:4:12:4 little endian
	WL_SHM_FORMAT_XVYU16161616         = 0x38345658,  // [63:0] X:Cr:Y:Cb 16:16:16:16 little endian
	WL_SHM_FORMAT_Y0L0                 = 0x304c3059,  // [63:0] A3:A2:Y3:0:Cr0:0:Y2:0:A1:A0:Y1:0:Cb0:0:Y0:0 1:1:8:2:8:2:8:2:1:1:8:2:8:2:8:2 little endian
	WL_SHM_FORMAT_X0L0                 = 0x304c3058,  // [63:0] X3:X2:Y3:0:Cr0:0:Y2:0:X1:X0:Y1:0:Cb0:0:Y0:0 1:1:8:2:8:2:8:2:1:1:8:2:8:2:8:2 little endian
	WL_SHM_FORMAT_Y0L2                 = 0x324c3059,  // [63:0] A3:A2:Y3:Cr0:Y2:A1:A0:Y1:Cb0:Y0 1:1:10:10:10:1:1:10:10:10 little endian
	WL_SHM_FORMAT_X0L2                 = 0x324c3058,  // [63:0] X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0 1:1:10:10:10:1:1:10:10:10 little endian
	WL_SHM_FORMAT_YUV420_8BIT          = 0x38305559,
	WL_SHM_FORMAT_YUV420_10BIT         = 0x30315559,
	WL_SHM_FORMAT_XRGB8888_A8          = 0x38415258,
	WL_SHM_FORMAT_XBGR8888_A8          = 0x38414258,
	WL_SHM_FORMAT_RGBX8888_A8          = 0x38415852,
	WL_SHM_FORMAT_BGRX8888_A8          = 0x38415842,
	WL_SHM_FORMAT_RGB888_A8            = 0x38413852,
	WL_SHM_FORMAT_BGR888_A8            = 0x38413842,
	WL_SHM_FORMAT_RGB565_A8            = 0x38413552,
	WL_SHM_FORMAT_BGR565_A8            = 0x38413542,
	WL_SHM_FORMAT_NV24                 = 0x3432564e,  // non-subsampled Cr:Cb plane
	WL_SHM_FORMAT_NV42                 = 0x3234564e,  // non-subsampled Cb:Cr plane
	WL_SHM_FORMAT_P210                 = 0x30313250,  // 2x1 subsampled Cr:Cb plane, 10 bit per channel
	WL_SHM_FORMAT_P010                 = 0x30313050,  // 2x2 subsampled Cr:Cb plane 10 bits per channel
	WL_SHM_FORMAT_P012                 = 0x32313050,  // 2x2 subsampled Cr:Cb plane 12 bits per channel
	WL_SHM_FORMAT_P016                 = 0x36313050,  // 2x2 subsampled Cr:Cb plane 16 bits per channel
	WL_SHM_FORMAT_AXBXGXRX106106106106 = 0x30314241,  // [63:0] A:x:B:x:G:x:R:x 10:6:10:6:10:6:10:6 little endian
	WL_SHM_FORMAT_NV15                 = 0x3531564e,  // 2x2 subsampled Cr:Cb plane
	WL_SHM_FORMAT_Q410                 = 0x30313451,
	WL_SHM_FORMAT_Q401                 = 0x31303451,
	WL_SHM_FORMAT_XRGB16161616         = 0x38345258,  // [63:0] x:R:G:B 16:16:16:16 little endian
	WL_SHM_FORMAT_XBGR16161616         = 0x38344258,  // [63:0] x:B:G:R 16:16:16:16 little endian
	WL_SHM_FORMAT_ARGB16161616         = 0x38345241,  // [63:0] A:R:G:B 16:16:16:16 little endian
	WL_SHM_FORMAT_ABGR16161616         = 0x38344241,  // [63:0] A:B:G:R 16:16:16:16 little endian
	WL_SHM_FORMAT_C1                   = 0x20203143,  // [7:0] C0:C1:C2:C3:C4:C5:C6:C7 1:1:1:1:1:1:1:1 eight pixels/byte
	WL_SHM_FORMAT_C2                   = 0x20203243,  // [7:0] C0:C1:C2:C3 2:2:2:2 four pixels/byte
	WL_SHM_FORMAT_C4                   = 0x20203443,  // [7:0] C0:C1 4:4 two pixels/byte
	WL_SHM_FORMAT_D1                   = 0x20203144,  // [7:0] D0:D1:D2:D3:D4:D5:D6:D7 1:1:1:1:1:1:1:1 eight pixels/byte
	WL_SHM_FORMAT_D2                   = 0x20203244,  // [7:0] D0:D1:D2:D3 2:2:2:2 four pixels/byte
	WL_SHM_FORMAT_D4                   = 0x20203444,  // [7:0] D0:D1 4:4 two pixels/byte
	WL_SHM_FORMAT_D8                   = 0x20203844,  // [7:0] D
	WL_SHM_FORMAT_R1                   = 0x20203152,  // [7:0] R0:R1:R2:R3:R4:R5:R6:R7 1:1:1:1:1:1:1:1 eight pixels/byte
	WL_SHM_FORMAT_R2                   = 0x20203252,  // [7:0] R0:R1:R2:R3 2:2:2:2 four pixels/byte
	WL_SHM_FORMAT_R4                   = 0x20203452,  // [7:0] R0:R1 4:4 two pixels/byte
	WL_SHM_FORMAT_R10                  = 0x20303152,  // [15:0] x:R 6:10 little endian
	WL_SHM_FORMAT_R12                  = 0x20323152,  // [15:0] x:R 4:12 little endian
	WL_SHM_FORMAT_AVUY8888             = 0x59555641,  // [31:0] A:Cr:Cb:Y 8:8:8:8 little endian
	WL_SHM_FORMAT_XVUY8888             = 0x59555658,  // [31:0] X:Cr:Cb:Y 8:8:8:8 little endian
	WL_SHM_FORMAT_P030                 = 0x30333050,  // 2x2 subsampled Cr:Cb plane 10 bits per channel packed
	WL_SHM_FORMAT_RGB161616            = 0x38344752,  // [47:0] R:G:B 16:16:16 little endian
	WL_SHM_FORMAT_BGR161616            = 0x38344742,  // [47:0] B:G:R 16:16:16 little endian
	WL_SHM_FORMAT_R16F                 = 0x48202052,  // [15:0] R 16 little endian
	WL_SHM_FORMAT_GR1616F              = 0x48205247,  // [31:0] G:R 16:16 little endian
	WL_SHM_FORMAT_BGR161616F           = 0x48524742,  // [47:0] B:G:R 16:16:16 little endian
	WL_SHM_FORMAT_R32F                 = 0x46202052,  // [31:0] R 32 little endian
	WL_SHM_FORMAT_GR3232F              = 0x46205247,  // [63:0] G:R 32:32 little endian
	WL_SHM_FORMAT_BGR323232F           = 0x46524742,  // [95:0] B:G:R 32:32:32 little endian
	WL_SHM_FORMAT_ABGR32323232F        = 0x46384241,  // [127:0] A:B:G:R 32:32:32:32 little endian
	WL_SHM_FORMAT_NV20                 = 0x3032564e,  // 2x1 subsampled Cr:Cb plane
	WL_SHM_FORMAT_NV30                 = 0x3033564e,  // non-subsampled Cr:Cb plane
	WL_SHM_FORMAT_S010                 = 0x30313053,  // 2x2 subsampled Cb (1) and Cr (2) planes 10 bits per channel
	WL_SHM_FORMAT_S210                 = 0x30313253,  // 2x1 subsampled Cb (1) and Cr (2) planes 10 bits per channel
	WL_SHM_FORMAT_S410                 = 0x30313453,  // non-subsampled Cb (1) and Cr (2) planes 10 bits per channel
	WL_SHM_FORMAT_S012                 = 0x32313053,  // 2x2 subsampled Cb (1) and Cr (2) planes 12 bits per channel
	WL_SHM_FORMAT_S212                 = 0x32313253,  // 2x1 subsampled Cb (1) and Cr (2) planes 12 bits per channel
	WL_SHM_FORMAT_S412                 = 0x32313453,  // non-subsampled Cb (1) and Cr (2) planes 12 bits per channel
	WL_SHM_FORMAT_S016                 = 0x36313053,  // 2x2 subsampled Cb (1) and Cr (2) planes 16 bits per channel
	WL_SHM_FORMAT_S216                 = 0x36313253,  // 2x1 subsampled Cb (1) and Cr (2) planes 16 bits per channel
	WL_SHM_FORMAT_S416                 = 0x36313453,  // non-subsampled Cb (1) and Cr (2) planes 16 bits per channel
	WL_SHM_FORMAT_XVUY2101010          = 0x30335958,  // [31:0] x:Cr:Cb:Y 2:10:10:10 little endian
	WL_SHM_FORMAT_P230                 = 0x30333250,  // 2x1 subsampled Cr:Cb plane 10 bits per channel packed
	WL_SHM_FORMAT_T430                 = 0x30333454,
	WL_SHM_FORMAT_Y8                   = 0x59455247,  // 8-bit Y-only
	WL_SHM_FORMAT_XYYY2101010          = 0x34415059,  // [31:0] x:Y2:Y1:Y0 2:10:10:10 little endian
	// clang-format on
} wl_shm_format;

#define WL_BUFFER ((WlBuffer, wl_buffer, 0), D((Destroy)), E((Release)))

#define WL_DATA_OFFER                                                    \
	((WlDataOffer, wl_data_offer, 4),                                    \
		R((Accept), U(Serial), S(MimeType, TRUE)),                       \
		R((Receive), S(MimeType), F(Fd)),                                \
		D((Destroy)),                                                    \
		R((Finish, 3)),                                                  \
		R((SetActions, 3),                                               \
			U(DndActions, wl_data_device_manager_dnd_action),            \
			U(PreferredAction, wl_data_device_manager_dnd_action)),      \
		E((Offer), S(MimeType)),                                         \
		E((SourceActions, 3),                                            \
			U(SourceActions, wl_data_device_manager_dnd_action)),        \
		E((Action, 3), U(DndAction, wl_data_device_manager_dnd_action)))

typedef enum wl_data_offer_error : u32 {
	WL_DATA_OFFER_ERROR_INVALID_FINISH		= 0,
	WL_DATA_OFFER_ERROR_INVALID_ACTION_MASK = 1,
	WL_DATA_OFFER_ERROR_INVALID_ACTION		= 2,
	WL_DATA_OFFER_ERROR_INVALID_OFFER		= 3,
} wl_data_offer_error;

#define WL_DATA_SOURCE                                                        \
	((WlDataSource, wl_data_source, 4),                                       \
		R((Offer), S(MimeType)),                                              \
		D((Destroy)),                                                         \
		R((SetActions, 3), U(DndActions, wl_data_device_manager_dnd_action)), \
		E((Target), S(MimeType, TRUE)),                                       \
		E((Send), S(MimeType), F(Fd)),                                        \
		E((Cancelled, 3)),                                                    \
		E((DndDropPerformed, 3)),                                             \
		E((DndFinished, 3)),                                                  \
		E((Action, 3), U(DndAction, wl_data_device_manager_dnd_action)))

typedef enum wl_data_source_error : u32 {
	WL_DATA_SOURCE_ERROR_INVALID_ACTION_MASK = 0,
	WL_DATA_SOURCE_ERROR_INVALID_SOURCE		 = 1,
} wl_data_source_error;

#define WL_DATA_DEVICE                                                 \
	((WlDataDevice, wl_data_device, 4),                                \
		R((StartDrag),                                                 \
			O(Source, WL_DATA_SOURCE, TRUE),                           \
			O(Origin, WL_SURFACE),                                     \
			O(Icon, WL_SURFACE, TRUE),                                 \
			U(Serial)),                                                \
		R((SetSelection), O(Source, WL_DATA_SOURCE, TRUE), U(Serial)), \
		D((Release, 2)),                                               \
		E((DataOffer), N(Id, WL_DATA_OFFER)),                          \
		E((Enter),                                                     \
			U(Serial),                                                 \
			O(Surface, WL_SURFACE),                                    \
			R(X),                                                      \
			R(Y),                                                      \
			O(Id, WL_DATA_OFFER, TRUE)),                               \
		E((Leave)),                                                    \
		E((Motion), U(Time), R(X), R(Y)),                              \
		E((Drop)),                                                     \
		E((Selection), O(Id, WL_DATA_OFFER, TRUE)))

typedef enum wl_data_device_error : u32 {
	WL_DATA_DEVICE_ERROR_ROLE		 = 0,
	WL_DATA_DEVICE_ERROR_USED_SOURCE = 1,
} wl_data_device_error;

#define WL_DATA_DEVICE_MANAGER                                   \
	((WlDataDeviceManager, wl_data_device_manager, 4),           \
		R((CreateDataSource), N(WL_DATA_SOURCE)),                \
		R((GetDataDevice), N(WL_DATA_DEVICE), O(Seat, WL_SEAT)), \
		D((Release, 4)))

typedef enum wl_data_device_manager_dnd_action : u32 {
	WL_DATA_DEVICE_MANAGER_DND_ACTION_NONE = 0x0,  // v3
	WL_DATA_DEVICE_MANAGER_DND_ACTION_COPY = 0x1,  // v3
	WL_DATA_DEVICE_MANAGER_DND_ACTION_MOVE = 0x2,  // v3
	WL_DATA_DEVICE_MANAGER_DND_ACTION_ASK  = 0x4,  // v3
} wl_data_device_manager_dnd_action;

#define WL_SHELL                     \
	((WlShell, wl_shell, 1, TRUE),   \
		R((GetShellSurface, 1, 1),   \
			N(WL_SHELL_SURFACE),     \
			O(Surface, WL_SURFACE)))

typedef enum wl_shell_error : u32 {
	WL_SHELL_ERROR_ROLE = 0,
} wl_shell_error;

#define WL_SHELL_SURFACE                                     \
	((WlShellSurface, wl_shell_surface, 1, TRUE),            \
		R((Pong, 1, 1), U(Serial)),                          \
		R((Move, 1, 1), O(Seat, WL_SEAT), U(Serial)),        \
		R((Resize, 1, 1),                                    \
			O(Seat, WL_SEAT),                                \
			U(Serial),                                       \
			U(Edges, wl_shell_surface_resize)),              \
		R((SetToplevel, 1, 1)),                              \
		R((SetTransient, 1, 1),                              \
			O(Parent, WL_SURFACE),                           \
			I(X),                                            \
			I(Y),                                            \
			U(Flags, wl_shell_surface_transient)),           \
		R((SetFullscreen, 1, 1),                             \
			U(Method, wl_shell_surface_fullscreen_method),   \
			U(Framerate),                                    \
			O(Output, WL_OUTPUT, TRUE)),                     \
		R((SetPopup, 1, 1),                                  \
			O(Seat, WL_SEAT),                                \
			U(Serial),                                       \
			O(Parent, WL_SURFACE),                           \
			I(X),                                            \
			I(Y),                                            \
			U(Flags, wl_shell_surface_transient)),           \
		R((SetMaximized, 1, 1), O(Output, WL_OUTPUT, TRUE)), \
		R((SetTitle, 1, 1), S(Title)),                       \
		R((SetClass, 1, 1), S(Class)),                       \
		E((Ping, 1, 1), U(Serial)),                          \
		E((Configure, 1, 1),                                 \
			U(Edges, wl_shell_surface_resize),               \
			I(Width),                                        \
			I(Height)),                                      \
		E((PopupDone, 1, 1)))

typedef enum wl_shell_surface_resize : u32 {
	WL_SHELL_SURFACE_RESIZE_NONE		 = 0,
	WL_SHELL_SURFACE_RESIZE_TOP			 = 1,
	WL_SHELL_SURFACE_RESIZE_BOTTOM		 = 2,
	WL_SHELL_SURFACE_RESIZE_LEFT		 = 4,
	WL_SHELL_SURFACE_RESIZE_TOP_LEFT	 = 5,
	WL_SHELL_SURFACE_RESIZE_BOTTOM_LEFT	 = 6,
	WL_SHELL_SURFACE_RESIZE_RIGHT		 = 8,
	WL_SHELL_SURFACE_RESIZE_TOP_RIGHT	 = 9,
	WL_SHELL_SURFACE_RESIZE_BOTTOM_RIGHT = 10,
} wl_shell_surface_resize;

typedef enum wl_shell_surface_transient : u32 {
	WL_SHELL_SURFACE_TRANSIENT_INACTIVE = 0x1,
} wl_shell_surface_transient;

typedef enum wl_shell_surface_fullscreen_method : u32 {
	WL_SHELL_SURFACE_FULLSCREEN_METHOD_DEFAULT = 0,
	WL_SHELL_SURFACE_FULLSCREEN_METHOD_SCALE   = 1,
	WL_SHELL_SURFACE_FULLSCREEN_METHOD_DRIVER  = 2,
	WL_SHELL_SURFACE_FULLSCREEN_METHOD_FILL	   = 3,
} wl_shell_surface_fullscreen_method;

#define WL_SURFACE                                                           \
	((WlSurface, wl_surface, 7),                                             \
		D((Destroy)),                                                        \
		R((Attach), O(Buffer, WL_BUFFER, TRUE), I(X), I(Y)),                 \
		R((Damage), I(X), I(Y), I(Width), I(Height)),                        \
		R((Frame), N(WL_CALLBACK)),                                          \
		R((SetOpaqueRegion), O(Region, WL_REGION, TRUE)),                    \
		R((SetInputRegion), O(Region, WL_REGION, TRUE)),                     \
		R((Commit)),                                                         \
		R((SetBufferTransform, 2), I(Transform, wl_output_transform)),       \
		R((SetBufferScale, 3), I(Scale)),                                    \
		R((DamageBuffer, 4), I(X), I(Y), I(Width), I(Height)),               \
		R((Offset, 5), I(X), I(Y)),                                          \
		R((GetRelease, 7), N(WL_CALLBACK)),                                  \
		E((Enter), O(Output, WL_OUTPUT)),                                    \
		E((Leave), O(Output, WL_OUTPUT)),                                    \
		E((PreferredBufferScale, 6), I(Factor)),                             \
		E((PreferredBufferTransform, 6), U(Transform, wl_output_transform)))

typedef enum wl_surface_error : u32 {
	WL_SURFACE_ERROR_INVALID_SCALE		 = 0,
	WL_SURFACE_ERROR_INVALID_TRANSFORM	 = 1,
	WL_SURFACE_ERROR_INVALID_SIZE		 = 2,
	WL_SURFACE_ERROR_INVALID_OFFSET		 = 3,
	WL_SURFACE_ERROR_DEFUNCT_ROLE_OBJECT = 4,
	WL_SURFACE_ERROR_NO_BUFFER			 = 5,
} wl_surface_error;

#define WL_SEAT                                                \
	((WlSeat, wl_seat, 11),                                    \
		R((GetPointer), N(WL_POINTER)),                        \
		R((GetKeyboard), N(WL_KEYBOARD)),                      \
		R((GetTouch), N(WL_TOUCH)),                            \
		D((Release, 5)),                                       \
		E((Capabilities), U(Capabilites, wl_seat_capability)), \
		E((Name, 2), S(Name)))

typedef enum wl_seat_capability : u32 {
	WL_SEAT_CAPABILITY_POINTER	= 0x1,
	WL_SEAT_CAPABILITY_KEYBOARD = 0x2,
	WL_SEAT_CAPABILITY_TOUCH	= 0x4,
} wl_seat_capability;

typedef enum wl_seat_error : u32 {
	WL_SEAT_ERROR_MISSING_CAPABILITY = 0,
} wl_seat_error;

#define WL_POINTER                                                      \
	((WlPointer, wl_pointer, 11),                                       \
		R((SetCursor),                                                  \
			U(Serial),                                                  \
			O(Surface, WL_SURFACE, TRUE),                               \
			I(HotspotX),                                                \
			I(HotspotY)),                                               \
		D((Release, 3)),                                                \
		E((Enter),                                                      \
			U(Serial),                                                  \
			O(Surface, WL_SURFACE),                                     \
			R(SurfaceX),                                                \
			R(SurfaceY)),                                               \
		E((Leave), U(Serial), O(Surface, WL_SURFACE)),                  \
		E((Motion), U(Time), R(SurfaceX), R(SurfaceY)),                 \
		E((Button),                                                     \
			U(Serial),                                                  \
			U(Time),                                                    \
			U(Button),                                                  \
			U(State, wl_pointer_button_state)),                         \
		E((Axis), U(Time), U(Axis, wl_pointer_axis), R(Value)),         \
		E((Frame, 5)),                                                  \
		E((AxisSource, 5), U(AxisSource, wl_pointer_axis_source)),      \
		E((AxisStop, 5), U(Time), U(Axis, wl_pointer_axis)),            \
		E((AxisDiscrete, 5, 7), U(Axis, wl_pointer_axis), I(Discrete)), \
		E((AxisValue120, 8), U(Axis, wl_pointer_axis), I(Value120)),    \
		E((AxisRelativeDirection, 9),                                   \
			U(Axis, wl_pointer_axis),                                   \
			U(Direction, wl_pointer_axis_relative_direction)),          \
		E((Warp, 11), R(SurfaceX), R(SurfaceY)))

typedef enum wl_pointer_error : u32 {
	WL_POINTER_ERROR_ROLE = 0,
} wl_pointer_error;

typedef enum wl_pointer_button_state : u32 {
	WL_POINTER_BUTTON_STATE_RELEASED = 0,
	WL_POINTER_BUTTON_STATE_PRESSED	 = 1,
} wl_pointer_button_state;

typedef enum wl_pointer_axis : u32 {
	WL_POINTER_AXIS_VERTICAL_SCROLL	  = 0,
	WL_POINTER_AXIS_HORIZONTAL_SCROLL = 1,
} wl_pointer_axis;

typedef enum wl_pointer_axis_source : u32 {
	WL_POINTER_AXIS_SOURCE_WHEEL	  = 0,
	WL_POINTER_AXIS_SOURCE_FINGER	  = 1,
	WL_POINTER_AXIS_SOURCE_CONTINUOUS = 2,
	WL_POINTER_AXIS_SOURCE_WHEEL_TILT = 3,
} wl_pointer_axis_source;

typedef enum wl_pointer_axis_relative_direction : u32 {
	WL_POINTER_AXIS_RELATIVE_DIRECTION_IDENTICAL = 0,
	WL_POINTER_AXIS_RELATIVE_DIRECTION_INVERTED	 = 1,
} wl_pointer_axis_relative_direction;

#define WL_KEYBOARD                                                            \
	((WlKeyboard, wl_keyboard, 11),                                            \
		D((Release, 3)),                                                       \
		E((Keymap), U(Format, wl_keyboard_keymap_format), F(Fd), U(Size)),     \
		E((Enter), U(Serial), O(Surface, WL_SURFACE), A(Keys)),                \
		E((Leave), U(Serial), O(Surface, WL_SURFACE)),                         \
		E((Key), U(Serial), U(Time), U(Key), U(State, wl_keyboard_key_state)), \
		E((Modifiers),                                                         \
			U(Serial),                                                         \
			U(ModsDepressed),                                                  \
			U(ModsLatched),                                                    \
			U(ModsLocked),                                                     \
			U(Group)),                                                         \
		E((RepeatInfo, 4), I(Rate), I(Delay)))

typedef enum wl_keyboard_keymap_format : u32 {
	WL_KEYBOARD_KEYMAP_FORMAT_NO_KEYMAP = 0,
	WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1	= 1,
} wl_keyboard_keymap_format;

typedef enum wl_keyboard_key_state : u32 {
	WL_KEYBOARD_KEY_STATE_RELEASED = 0,
	WL_KEYBOARD_KEY_STATE_PRESSED  = 1,
	WL_KEYBOARD_KEY_STATE_REPEATED = 2,	 // v10
} wl_keyboard_key_state;

#define WL_TOUCH                                    \
	((WlTouch, wl_touch, 11),                       \
		D((Release, 3)),                            \
		E((Down),                                   \
			U(Serial),                              \
			U(Time),                                \
			O(Surface, WL_SURFACE),                 \
			I(Id),                                  \
			R(X),                                   \
			R(Y)),                                  \
		E((Up), U(Serial), U(Time), I(Id)),         \
		E((Motion), U(Time), I(Id), R(X), R(Y)),    \
		E((Frame)),                                 \
		E((Cancel)),                                \
		E((Shape, 6), I(Id), R(Major), R(Minor)),   \
		E((Orientation, 6), I(Id), R(Orientation)))

#define WL_OUTPUT                                                             \
	((WlOutput, wl_output, 4),                                                \
		D((Release, 3)),                                                      \
		E((Geometry),                                                         \
			I(X),                                                             \
			I(Y),                                                             \
			I(PhysicalWidth),                                                 \
			I(PhysicalHeight),                                                \
			I(Subpixel, wl_output_subpixel),                                  \
			S(Make),                                                          \
			S(Model),                                                         \
			I(Transform, wl_output_transform)),                               \
		E((Mode), U(Flags, wl_output_mode), I(Width), I(Height), I(Refresh)), \
		E((Done, 2)),                                                         \
		E((Scale, 2), I(Factor)),                                             \
		E((Name, 4), S(Name)),                                                \
		E((Description, 4), S(Description)))

typedef enum wl_output_subpixel : u32 {
	WL_OUTPUT_SUBPIXEL_UNKNOWN		  = 0,
	WL_OUTPUT_SUBPIXEL_NONE			  = 1,
	WL_OUTPUT_SUBPIXEL_HORIZONTAL_RGB = 2,
	WL_OUTPUT_SUBPIXEL_HORIZONTAL_BGR = 3,
	WL_OUTPUT_SUBPIXEL_VERTICAL_RGB	  = 4,
	WL_OUTPUT_SUBPIXEL_VERTICAL_BGR	  = 5,
} wl_output_subpixel;

typedef enum wl_output_transform : u32 {
	WL_OUTPUT_TRANSFORM_NORMAL		= 0,
	WL_OUTPUT_TRANSFORM_90			= 1,
	WL_OUTPUT_TRANSFORM_180			= 2,
	WL_OUTPUT_TRANSFORM_270			= 3,
	WL_OUTPUT_TRANSFORM_FLIPPED		= 4,
	WL_OUTPUT_TRANSFORM_FLIPPED_90	= 5,
	WL_OUTPUT_TRANSFORM_FLIPPED_180 = 6,
	WL_OUTPUT_TRANSFORM_FLIPPED_270 = 7,
} wl_output_transform;

typedef enum wl_output_mode : u32 {
	WL_OUTPUT_MODE_CURRENT	 = 0x1,
	WL_OUTPUT_MODE_PREFERRED = 0x2,
} wl_output_mode;

#define WL_REGION                                       \
	((WlRegion, wl_region, 7),                          \
		D((Destroy)),                                   \
		R((Add), I(X), I(Y), I(Width), I(Height)),      \
		R((Subtract), I(X), I(Y), I(Width), I(Height)))

#define WL_SUBCOMPOSITOR                     \
	((WlSubcompositor, wl_subcompositor, 1), \
		D((Destroy)),                        \
		R((GetSubsurface),                   \
			N(WL_SUBSURFACE),                \
			O(Surface, WL_SURFACE),          \
			O(Parent, WL_SURFACE)))

typedef enum wl_subcompositor_error : u32 {
	WL_SUBCOMPOSITOR_ERROR_BAD_SURFACE = 0,
	WL_SUBCOMPOSITOR_ERROR_BAD_PARENT  = 1,
} wl_subcompositor_error;

#define WL_SUBSURFACE                            \
	((WlSubsurface, wl_subsurface, 1),           \
		D((Destroy)),                            \
		R((SetPosition), I(X), I(Y)),            \
		R((PlaceAbove), O(Sibling, WL_SURFACE)), \
		R((PlaceBelow), O(Sibling, WL_SURFACE)), \
		R((SetSync)),                            \
		R((SetDesync)))

typedef enum wl_subsurface_error : u32 {
	WL_SUBSURFACE_ERROR_BAD_SURFACE = 0,
} wl_subsurface_error;

#define WL_FIXES                                                    \
	((WlFixes, wl_fixes, 2),                                        \
		D((Destroy)),                                               \
		R((DestroyRegistry), D(Registry, WL_REGISTRY)),             \
		R((AckGlobalRemove, 2), O(Registry, WL_REGISTRY), U(Name)))

typedef enum wl_fixes_error : u32 {
	WL_FIXES_ERROR_INVALID_ACK_REMOVE = 0,	// v2
} wl_fixes_error;

WLI_DECLARE_STRUCT(WL_DISPLAY)
WLI_DECLARE_STRUCT(WL_REGISTRY)
WLI_DECLARE_STRUCT(WL_CALLBACK)
WLI_DECLARE_STRUCT(WL_COMPOSITOR)
WLI_DECLARE_STRUCT(WL_SHM_POOL)
WLI_DECLARE_STRUCT(WL_SHM)
WLI_DECLARE_STRUCT(WL_BUFFER)
WLI_DECLARE_STRUCT(WL_DATA_OFFER)
WLI_DECLARE_STRUCT(WL_DATA_SOURCE)
WLI_DECLARE_STRUCT(WL_DATA_DEVICE)
WLI_DECLARE_STRUCT(WL_DATA_DEVICE_MANAGER)
WLI_DECLARE_STRUCT(WL_SHELL)
WLI_DECLARE_STRUCT(WL_SHELL_SURFACE)
WLI_DECLARE_STRUCT(WL_SURFACE)
WLI_DECLARE_STRUCT(WL_SEAT)
WLI_DECLARE_STRUCT(WL_POINTER)
WLI_DECLARE_STRUCT(WL_KEYBOARD)
WLI_DECLARE_STRUCT(WL_TOUCH)
WLI_DECLARE_STRUCT(WL_OUTPUT)
WLI_DECLARE_STRUCT(WL_REGION)
WLI_DECLARE_STRUCT(WL_SUBCOMPOSITOR)
WLI_DECLARE_STRUCT(WL_SUBSURFACE)
WLI_DECLARE_STRUCT(WL_FIXES)

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
