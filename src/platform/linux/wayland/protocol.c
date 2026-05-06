/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                            *
*  Author: Aria Seiler                                                       *
*                                                                            *
*  This program is in the public domain. There is no implied warranty, so    *
*  use it at your own risk.                                                  *
*                                                                            *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef INCLUDE_HEADER

#ifndef SECTION_WAYLAND

typedef enum wayland_display_error	   wayland_display_error;
typedef enum wayland_shm_error		   wayland_shm_error;
typedef enum wayland_shm_format		   wayland_shm_format;
typedef enum wayland_data_offer_error  wayland_data_offer_error;
typedef enum wayland_data_source_error wayland_data_source_error;
typedef enum wayland_data_device_error wayland_data_device_error;
typedef enum wayland_data_device_manager_drag_and_drop_action
	wayland_data_device_manager_drag_and_drop_action;
typedef enum wayland_shell_error			 wayland_shell_error;
typedef enum wayland_shell_surface_resize	 wayland_shell_surface_resize;
typedef enum wayland_shell_surface_transient wayland_shell_surface_transient;
typedef enum wayland_shell_surface_fullscreen_method
									 wayland_shell_surface_fullscreen_method;
typedef enum wayland_surface_error	 wayland_surface_error;
typedef enum wayland_seat_capability wayland_seat_capability;
typedef enum wayland_seat_error		 wayland_seat_error;
typedef enum wayland_pointer_error	 wayland_pointer_error;
typedef enum wayland_pointer_button_state wayland_pointer_button_state;
typedef enum wayland_pointer_axis		  wayland_pointer_axis;
typedef enum wayland_pointer_axis_source  wayland_pointer_axis_source;
typedef enum wayland_pointer_axis_relative_direction
	wayland_pointer_axis_relative_direction;
typedef enum wayland_keyboard_keymap_format wayland_keyboard_keymap_format;
typedef enum wayland_keyboard_key_state		wayland_keyboard_key_state;
typedef enum wayland_output_subpixel		wayland_output_subpixel;
typedef enum wayland_output_transform		wayland_output_transform;
typedef enum wayland_output_mode			wayland_output_mode;
typedef enum wayland_subcompositor_error	wayland_subcompositor_error;
typedef enum wayland_subsurface_error		wayland_subsurface_error;

typedef struct wayland_display			   wayland_display;
typedef struct wayland_registry			   wayland_registry;
typedef struct wayland_callback			   wayland_callback;
typedef struct wayland_compositor		   wayland_compositor;
typedef struct wayland_shm_pool			   wayland_shm_pool;
typedef struct wayland_shm				   wayland_shm;
typedef struct wayland_buffer			   wayland_buffer;
typedef struct wayland_data_offer		   wayland_data_offer;
typedef struct wayland_data_source		   wayland_data_source;
typedef struct wayland_data_device		   wayland_data_device;
typedef struct wayland_data_device_manager wayland_data_device_manager;
typedef struct wayland_shell			   wayland_shell;
typedef struct wayland_shell_surface	   wayland_shell_surface;
typedef struct wayland_surface			   wayland_surface;
typedef struct wayland_seat				   wayland_seat;
typedef struct wayland_pointer			   wayland_pointer;
typedef struct wayland_keyboard			   wayland_keyboard;
typedef struct wayland_touch			   wayland_touch;
typedef struct wayland_output			   wayland_output;
typedef struct wayland_region			   wayland_region;
typedef struct wayland_subcompositor	   wayland_subcompositor;
typedef struct wayland_subsurface		   wayland_subsurface;
typedef struct wayland_fixes			   wayland_fixes;

enum wayland_display_error {
	WAYLAND_DISPLAY_ERROR_INVALID_OBJECT = 0,
	WAYLAND_DISPLAY_ERROR_INVALID_METHOD = 1,
	WAYLAND_DISPLAY_ERROR_NO_MEMORY		 = 2,
	WAYLAND_DISPLAY_ERROR_IMPLEMENTATION = 3,
};

struct wayland_display {
	wayland_interface Interface;

	void (*Error)(
		wayland_display	  *This,
		wayland_interface *Object,
		u32				   Code,
		c08				  *Message
	);
	void (*DeleteId)(wayland_display *This, u32 Id);
};

struct wayland_registry {
	wayland_interface Interface;

	void (*Global)(
		wayland_registry *This,
		u32				  Name,
		c08				 *Interface,
		u32				  Version
	);
	void (*GlobalRemove)(wayland_registry *This, u32 Name);
};

struct wayland_callback {
	wayland_interface Interface;

	void (*Done)(wayland_callback *This, u32 CallbackData);
};

struct wayland_compositor {
	wayland_interface Interface;
};

struct wayland_shm_pool {
	wayland_interface Interface;
};

enum wayland_shm_error {
	WAYLAND_SHM_ERROR_INVALID_FORMAT		  = 0,
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

struct wayland_shm {
	wayland_interface Interface;

	void (*Format)(wayland_shm *This, wayland_shm_format Format);
};

struct wayland_buffer {
	wayland_interface Interface;

	void (*Release)(wayland_buffer *This);
};

enum wayland_data_offer_error {
	WAYLAND_DATA_OFFER_ERROR_INVALID_FINISH		 = 0,
	WAYLAND_DATA_OFFER_ERROR_INVALID_ACTION_MASK = 1,
	WAYLAND_DATA_OFFER_ERROR_INVALID_ACTION		 = 2,
	WAYLAND_DATA_OFFER_ERROR_INVALID_OFFER		 = 3,
};

struct wayland_data_offer {
	wayland_interface Interface;

	void (*Offer)(wayland_data_offer *This, c08 *MimeType);
	void (*SourceActions)(
		wayland_data_offer								*This,
		wayland_data_device_manager_drag_and_drop_action SourceActions
	);
	void (*Action)(
		wayland_data_offer								*This,
		wayland_data_device_manager_drag_and_drop_action DragAndDropAction
	);
};

enum wayland_data_source_error {
	WAYLAND_DATA_SOURCE_ERROR_INVALID_ACTION_MASK = 0,
	WAYLAND_DATA_SOURCE_ERROR_INVALID_SOURCE	  = 1,
};

struct wayland_data_source {
	wayland_interface Interface;

	void (*Target)(wayland_data_source *This, c08 *MimeType);
	void (*Send)(wayland_data_source *This, c08 *MimeType, s32 FileDescriptor);
	void (*Cancelled)(wayland_data_source *This);
	void (*DragAndDropPerformed)(wayland_data_source *This);
	void (*DragAndDropFinished)(wayland_data_source *This);
	void (*Action)(
		wayland_data_source								*This,
		wayland_data_device_manager_drag_and_drop_action DragAndDropAction
	);
};

enum wayland_data_device_error {
	WAYLAND_DATA_DEVICE_ERROR_ROLE		  = 0,
	WAYLAND_DATA_DEVICE_ERROR_USED_SOURCE = 1,
};

struct wayland_data_device {
	wayland_interface Interface;

	void (*DataOffer)(wayland_data_device *This, u32 NewDataOfferId);
	void (*Enter)(
		wayland_data_device *This,
		u32					 Serial,
		wayland_surface		*Surface,
		wayland_fixed		 X,
		wayland_fixed		 Y,
		wayland_data_offer	*DataOffer
	);
	void (*Leave)(wayland_data_device *This);
	void (*Motion)(
		wayland_data_device *This,
		u32					 Time,
		wayland_fixed		 X,
		wayland_fixed		 Y
	);
	void (*Drop)(wayland_data_device *This);
	void (*Selection)(wayland_data_device *This, wayland_data_offer *DataOffer);
};

enum wayland_data_device_manager_drag_and_drop_action {
	WAYLAND_DATA_DEVICE_MANAGER_DRAG_AND_DROP_ACTION_NONE = 0x0,
	WAYLAND_DATA_DEVICE_MANAGER_DRAG_AND_DROP_ACTION_COPY = 0x1,
	WAYLAND_DATA_DEVICE_MANAGER_DRAG_AND_DROP_ACTION_MOVE = 0x2,
	WAYLAND_DATA_DEVICE_MANAGER_DRAG_AND_DROP_ACTION_ASK  = 0x4,
};

struct wayland_data_device_manager {
	wayland_interface Interface;
};

enum wayland_shell_error {
	WAYLAND_SHELL_ERROR_ROLE = 0,
};

struct wayland_shell {
	wayland_interface Interface;
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

struct wayland_shell_surface {
	wayland_interface Interface;

	void (*Ping)(wayland_shell_surface *This, u32 Serial);
	void (*Configure)(
		wayland_shell_surface		*This,
		wayland_shell_surface_resize Edges,
		s32							 Width,
		s32							 Height
	);
	void (*PopupDone)(wayland_shell_surface *This);
};

enum wayland_surface_error {
	WAYLAND_SURFACE_ERROR_INVALID_SCALE		  = 0,
	WAYLAND_SURFACE_ERROR_INVALID_TRANSFORM	  = 1,
	WAYLAND_SURFACE_ERROR_INVALID_SIZE		  = 2,
	WAYLAND_SURFACE_ERROR_INVALID_OFFSET	  = 3,
	WAYLAND_SURFACE_ERROR_DEFUNCT_ROLE_OBJECT = 4,
};

struct wayland_surface {
	wayland_interface Interface;

	void (*Enter)(wayland_surface *This, wayland_output *Output);
	void (*Leave)(wayland_surface *This, wayland_output *Output);
	void (*PreferredBufferScale)(wayland_surface *This, s32 Factor);
	void (*PreferredBufferTransform)(
		wayland_surface			*This,
		wayland_output_transform Transform
	);
};

enum wayland_seat_capability {
	WAYLAND_SEAT_CAPABILITY_POINTER	 = 0x1,
	WAYLAND_SEAT_CAPABILITY_KEYBOARD = 0x2,
	WAYLAND_SEAT_CAPABILITY_TOUCH	 = 0x4,
};

enum wayland_seat_error {
	WAYLAND_SEAT_ERROR_MISSING_CAPABILITY = 0,
};

struct wayland_seat {
	wayland_interface Interface;

	void (*Capabilities)(
		wayland_seat		   *This,
		wayland_seat_capability Capabilities
	);
	void (*Name)(wayland_seat *This, c08 *Name);
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

struct wayland_pointer {
	wayland_interface Interface;

	void (*Enter)(
		wayland_pointer *This,
		u32				 Serial,
		wayland_surface *Surface,
		wayland_fixed	 SurfaceX,
		wayland_fixed	 SurfaceY
	);
	void (*Leave)(wayland_pointer *This, u32 Serial, wayland_surface *Surface);
	void (*Motion)(
		wayland_pointer *This,
		u32				 Time,
		wayland_fixed	 SurfaceX,
		wayland_fixed	 SurfaceY
	);
	void (*Button)(
		wayland_pointer				*This,
		u32							 Serial,
		u32							 Time,
		u32							 Button,
		wayland_pointer_button_state State
	);
	void (*Axis)(
		wayland_pointer		*This,
		u32					 Time,
		wayland_pointer_axis Axis,
		wayland_fixed		 Value
	);
	void (*Frame)(wayland_pointer *This);
	void (*AxisSource)(
		wayland_pointer			   *This,
		wayland_pointer_axis_source AxisSource
	);
	void (*AxisStop)(
		wayland_pointer		*This,
		u32					 Time,
		wayland_pointer_axis AxisStop
	);
	void (*AxisDiscrete)(
		wayland_pointer		*This,
		wayland_pointer_axis Axis,
		s32					 Discrete
	);
	void (*AxisValue120)(
		wayland_pointer		*This,
		wayland_pointer_axis Axis,
		s32					 Value120
	);
	void (*AxisRelativeDirection)(
		wayland_pointer						   *This,
		wayland_pointer_axis					Axis,
		wayland_pointer_axis_relative_direction Direction
	);
};

enum wayland_keyboard_keymap_format {
	WAYLAND_KEYBOARD_KEYMAP_FORMAT_NO_KEYMAP = 0,
	WAYLAND_KEYBOARD_KEYMAP_FORMAT_XKB_V1	 = 1,
};

enum wayland_keyboard_key_state {
	WAYLAND_KEYBOARD_KEY_STATE_RELEASED = 0,
	WAYLAND_KEYBOARD_KEY_STATE_PRESSED	= 1,
};

struct wayland_keyboard {
	wayland_interface Interface;

	void (*Keymap)(wayland_keyboard *This, s32 FileDescriptor, u32 Size);
	void (*Enter)(
		wayland_keyboard *This,
		u32				  Serial,
		wayland_surface	 *Surface,
		wayland_array	  Keys
	);
	void (*Leave)(wayland_keyboard *This, u32 Serial, wayland_surface *Surface);
	void (*Key)(
		wayland_keyboard		  *This,
		u32						   Serial,
		u32						   Time,
		u32						   Key,
		wayland_keyboard_key_state State
	);
	void (*Modifiers)(
		wayland_keyboard *This,
		u32				  Serial,
		u32				  Depressed,
		u32				  Latched,
		u32				  Locked,
		u32				  Group
	);
	void (*RepeatInfo)(wayland_keyboard *This, s32 Rate, s32 Delay);
};

struct wayland_touch {
	wayland_interface Interface;

	void (*Down)(
		wayland_touch	*This,
		u32				 Serial,
		u32				 Time,
		wayland_surface *Surface,
		s32				 Id,
		wayland_fixed	 X,
		wayland_fixed	 Y
	);
	void (*Up)(wayland_touch *This, u32 Serial, u32 Time, s32 Id);
	void (*Motion)(
		wayland_touch *This,
		u32			   Motion,
		s32			   Id,
		wayland_fixed  X,
		wayland_fixed  Y
	);
	void (*Frame)(wayland_touch *This);
	void (*Cancel)(wayland_touch *This);
	void (*Shape)(
		wayland_touch *This,
		s32			   Id,
		wayland_fixed  Major,
		wayland_fixed  Minor
	);
	void (*Orientation)(wayland_touch *This, s32 Id, wayland_fixed Orientation);
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

struct wayland_output {
	wayland_interface Interface;

	void (*Geometry)(
		wayland_output			*This,
		s32						 X,
		s32						 Y,
		s32						 PhysicalWidth,
		s32						 PhysicalHeight,
		wayland_output_subpixel	 Subpixel,
		c08						*Make,
		c08						*Model,
		wayland_output_transform Transform
	);
	void (*Mode)(
		wayland_output	   *This,
		wayland_output_mode Flags,
		s32					Width,
		s32					Height,
		s32					Refresh
	);
	void (*Done)(wayland_output *This);
	void (*Scale)(wayland_output *This, s32 Factor);
	void (*Name)(wayland_output *This, c08 *Name);
	void (*Description)(wayland_output *This, c08 *Description);
};

struct wayland_region {
	wayland_interface Interface;
};

enum wayland_subcompositor_error {
	WAYLAND_SUBCOMPOSITOR_ERROR_BAD_SURFACE = 0,
	WAYLAND_SUBCOMPOSITOR_ERROR_BAD_PARENT	= 1,
};

struct wayland_subcompositor {
	wayland_interface Interface;
};

enum wayland_subsurface_error {
	WAYLAND_SUBSURFACE_ERROR_BAD_SURFACE = 0,
};

struct wayland_subsurface {
	wayland_interface Interface;
};

struct wayland_fixes {
	wayland_interface Interface;
};

#endif

#ifndef SECTION_XDG_SHELL

typedef enum wayland_xdg_wm_base_error		wayland_xdg_wm_base_error;
typedef enum wayland_xdg_positioner_error	wayland_xdg_positioner_error;
typedef enum wayland_xdg_positioner_anchor	wayland_xdg_positioner_anchor;
typedef enum wayland_xdg_positioner_gravity wayland_xdg_positioner_gravity;
typedef enum wayland_xdg_positioner_constraint_adjustment
	wayland_xdg_positioner_constraint_adjustment;
typedef enum wayland_xdg_surface_error		  wayland_xdg_surface_error;
typedef enum wayland_xdg_toplevel_error		  wayland_xdg_toplevel_error;
typedef enum wayland_xdg_toplevel_resize_edge wayland_xdg_toplevel_resize_edge;
typedef enum wayland_xdg_toplevel_state		  wayland_xdg_toplevel_state;
typedef enum wayland_xdg_toplevel_wm_capabilities
									 wayland_xdg_toplevel_wm_capabilities;
typedef enum wayland_xdg_popup_error wayland_xdg_popup_error;

typedef struct wayland_xdg_wm_base	  wayland_xdg_wm_base;
typedef struct wayland_xdg_positioner wayland_xdg_positioner;
typedef struct wayland_xdg_surface	  wayland_xdg_surface;
typedef struct wayland_xdg_toplevel	  wayland_xdg_toplevel;
typedef struct wayland_xdg_popup	  wayland_xdg_popup;

enum wayland_xdg_wm_base_error {
	WAYLAND_XDG_WM_BASE_ERROR_ROLE					= 0,
	WAYLAND_XDG_WM_BASE_ERROR_DEFUNCT_SURFACES		= 1,
	WAYLAND_XDG_WM_BASE_ERROR_NOT_THE_TOPMOST_POPUP = 2,
	WAYLAND_XDG_WM_BASE_ERROR_INVALID_POPUP_PARENT	= 3,
	WAYLAND_XDG_WM_BASE_ERROR_INVALID_SURFACE_STATE = 4,
	WAYLAND_XDG_WM_BASE_ERROR_INVALID_POSITIONER	= 5,
	WAYLAND_XDG_WM_BASE_ERROR_UNRESPONSIVE			= 6,
};

struct wayland_xdg_wm_base {
	wayland_interface Interface;

	void (*Ping)(wayland_xdg_wm_base *This, u32 Serial);
};

enum wayland_xdg_positioner_error {
	WAYLAND_XDG_POSITIONER_ERROR_INVALID_INPUT = 0
};

enum wayland_xdg_positioner_anchor {
	WAYLAND_XDG_POSITIONER_ANCHOR_NONE		   = 0,
	WAYLAND_XDG_POSITIONER_ANCHOR_TOP		   = 1,
	WAYLAND_XDG_POSITIONER_ANCHOR_BOTTOM	   = 2,
	WAYLAND_XDG_POSITIONER_ANCHOR_LEFT		   = 3,
	WAYLAND_XDG_POSITIONER_ANCHOR_RIGHT		   = 4,
	WAYLAND_XDG_POSITIONER_ANCHOR_TOP_LEFT	   = 5,
	WAYLAND_XDG_POSITIONER_ANCHOR_BOTTOM_LEFT  = 6,
	WAYLAND_XDG_POSITIONER_ANCHOR_TOP_RIGHT	   = 7,
	WAYLAND_XDG_POSITIONER_ANCHOR_BOTTOM_RIGHT = 8,
};

enum wayland_xdg_positioner_gravity {
	WAYLAND_XDG_POSITIONER_GRAVITY_NONE			= 0,
	WAYLAND_XDG_POSITIONER_GRAVITY_TOP			= 1,
	WAYLAND_XDG_POSITIONER_GRAVITY_BOTTOM		= 2,
	WAYLAND_XDG_POSITIONER_GRAVITY_LEFT			= 3,
	WAYLAND_XDG_POSITIONER_GRAVITY_RIGHT		= 4,
	WAYLAND_XDG_POSITIONER_GRAVITY_TOP_LEFT		= 5,
	WAYLAND_XDG_POSITIONER_GRAVITY_BOTTOM_LEFT	= 6,
	WAYLAND_XDG_POSITIONER_GRAVITY_TOP_RIGHT	= 7,
	WAYLAND_XDG_POSITIONER_GRAVITY_BOTTOM_RIGHT = 8,
};

enum wayland_xdg_positioner_constraint_adjustment {
	WAYLAND_XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_NONE	  = 0,
	WAYLAND_XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_SLIDE_X  = 1,
	WAYLAND_XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_SLIDE_Y  = 2,
	WAYLAND_XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_FLIP_X	  = 4,
	WAYLAND_XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_FLIP_Y	  = 8,
	WAYLAND_XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_RESIZE_X = 16,
	WAYLAND_XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_RESIZE_Y = 32,
};

struct wayland_xdg_positioner {
	wayland_interface Interface;
};

enum wayland_xdg_surface_error {
	WAYLAND_XDG_SURFACE_ERROR_NOT_CONSTRUCTED	  = 1,
	WAYLAND_XDG_SURFACE_ERROR_ALREADY_CONSTRUCTED = 2,
	WAYLAND_XDG_SURFACE_ERROR_UNCONFIGURED_BUFFER = 3,
	WAYLAND_XDG_SURFACE_ERROR_INVALID_SERIAL	  = 4,
	WAYLAND_XDG_SURFACE_ERROR_INVALID_SIZE		  = 5,
	WAYLAND_XDG_SURFACE_ERROR_DEFUNCT_ROLE_OBJECT = 6,
};

struct wayland_xdg_surface {
	wayland_interface Interface;

	void (*Configure)(wayland_xdg_surface *This, u32 Serial);
};

enum wayland_xdg_toplevel_error {
	WAYLAND_XDG_TOPLEVEL_ERROR_INVALID_RESIZE_EDGE = 0,
	WAYLAND_XDG_TOPLEVEL_ERROR_INVALID_PARENT	   = 1,
	WAYLAND_XDG_TOPLEVEL_ERROR_INVALID_SIZE		   = 2,
};

enum wayland_xdg_toplevel_resize_edge {
	WAYLAND_XDG_TOPLEVEL_RESIZE_EDGE_NONE		  = 0,
	WAYLAND_XDG_TOPLEVEL_RESIZE_EDGE_TOP		  = 1,
	WAYLAND_XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM		  = 2,
	WAYLAND_XDG_TOPLEVEL_RESIZE_EDGE_LEFT		  = 4,
	WAYLAND_XDG_TOPLEVEL_RESIZE_EDGE_TOP_LEFT	  = 5,
	WAYLAND_XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_LEFT  = 6,
	WAYLAND_XDG_TOPLEVEL_RESIZE_EDGE_RIGHT		  = 8,
	WAYLAND_XDG_TOPLEVEL_RESIZE_EDGE_TOP_RIGHT	  = 9,
	WAYLAND_XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_RIGHT = 10,
};

enum wayland_xdg_toplevel_state {
	WAYLAND_XDG_TOPLEVEL_STATE_MAXIMIZED			   = 1,
	WAYLAND_XDG_TOPLEVEL_STATE_FULLSCREEN			   = 2,
	WAYLAND_XDG_TOPLEVEL_STATE_RESIZING				   = 3,
	WAYLAND_XDG_TOPLEVEL_STATE_ACTIVATED			   = 4,
	WAYLAND_XDG_TOPLEVEL_STATE_TILED_LEFTSINCE		   = 5,	  // version 2
	WAYLAND_XDG_TOPLEVEL_STATE_TILED_RIGHTSINCE		   = 6,	  // version 2
	WAYLAND_XDG_TOPLEVEL_STATE_TILED_TOPSINCE		   = 7,	  // version 2
	WAYLAND_XDG_TOPLEVEL_STATE_TILED_BOTTOMSINCE	   = 8,	  // version 2
	WAYLAND_XDG_TOPLEVEL_STATE_SUSPENDEDSINCE		   = 9,	  // version 6
	WAYLAND_XDG_TOPLEVEL_STATE_CONSTRAINED_LEFTSINCE   = 10,  // version 7
	WAYLAND_XDG_TOPLEVEL_STATE_CONSTRAINED_RIGHTSINCE  = 11,  // version 7
	WAYLAND_XDG_TOPLEVEL_STATE_CONSTRAINED_TOPSINCE	   = 12,  // version 7
	WAYLAND_XDG_TOPLEVEL_STATE_CONSTRAINED_BOTTOMSINCE = 13,  // version 7
};

enum wayland_xdg_toplevel_wm_capabilities {
	WAYLAND_XDG_TOPLEVEL_WM_CAPABILITIES_WINDOW_MENU = 1,
	WAYLAND_XDG_TOPLEVEL_WM_CAPABILITIES_MAXIMIZE	 = 2,
	WAYLAND_XDG_TOPLEVEL_WM_CAPABILITIES_FULLSCREEN	 = 3,
	WAYLAND_XDG_TOPLEVEL_WM_CAPABILITIES_MINIMIZE	 = 4,
};

struct wayland_xdg_toplevel {
	wayland_interface Interface;

	void (*Configure)(
		wayland_xdg_toplevel *This,
		s32					  Width,
		s32					  Height,
		wayland_array		  Array
	);
	void (*Close)(wayland_xdg_toplevel *This);
	void (*ConfigureBounds)(wayland_xdg_toplevel *This, s32 Width, s32 Height);
	void (*WmCapabilities)(
		wayland_xdg_toplevel *This,
		wayland_array		  Capabilities
	);
};

enum wayland_xdg_popup_error { WAYLAND_XDG_POPUP_ERROR_INVALID_GRAB = 0 };

struct wayland_xdg_popup {
	wayland_interface Interface;

	void (*Configure)(
		wayland_xdg_popup *This,
		s32				   X,
		s32				   Y,
		s32				   Width,
		s32				   Height
	);
	void (*PopupDone)(wayland_xdg_popup *This);
	void (*Repositioned)(wayland_xdg_popup *This, u32 Token);
};

#endif

#ifndef SECTION_DRM

typedef enum wayland_drm_error		wayland_drm_error;
typedef enum wayland_drm_capability wayland_drm_capability;

typedef struct wayland_drm wayland_drm;

enum wayland_drm_error {
	WAYLAND_DRM_ERROR_AUTHENTICATE_FAIL = 0,
	WAYLAND_DRM_ERROR_INVALID_FORMAT	= 1,
	WAYLAND_DRM_ERROR_INVALID_NAME		= 2,
};

enum wayland_drm_capability { WAYLAND_DRM_CAPABILITY_PRIME = 1 };

struct wayland_drm {
	wayland_interface Interface;

	void (*Device)(wayland_drm *This, c08 *Name);
	void (*Format)(wayland_drm *This, u32 Format);
	void (*Authenticated)(wayland_drm *This);
	void (*Capabilities)(
		wayland_drm			  *This,
		wayland_drm_capability Capabilities
	);
};

#endif

#ifndef SECTION_LINUX_DMABUF

typedef enum wayland_zwp_linux_buffer_params_v1_error
	wayland_zwp_linux_buffer_params_v1_error;
typedef enum wayland_zwp_linux_buffer_params_v1_flags
	wayland_zwp_linux_buffer_params_v1_flags;
typedef enum wayland_zwp_linux_dmabuf_feedback_v1_tranche_flags
	wayland_zwp_linux_dmabuf_feedback_v1_tranche_flags;

typedef struct wayland_zwp_linux_dmabuf_v1 wayland_zwp_linux_dmabuf_v1;
typedef struct wayland_zwp_linux_buffer_params_v1
	wayland_zwp_linux_buffer_params_v1;
typedef struct wayland_zwp_linux_dmabuf_feedback_v1
	wayland_zwp_linux_dmabuf_feedback_v1;

struct wayland_zwp_linux_dmabuf_v1 {
	wayland_interface Interface;

	void (*Format)(wayland_zwp_linux_dmabuf_v1 *This, u32 Format);
	void (*Modifier)(
		wayland_zwp_linux_dmabuf_v1 *This,
		u32							 Format,
		u32							 ModifierHigh,
		u32							 ModifierLow
	);
};

enum wayland_zwp_linux_buffer_params_v1_error {
	WAYLAND_ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_ALREADY_USED		= 0,
	WAYLAND_ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_PLANE_IDX			= 1,
	WAYLAND_ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_PLANE_SET			= 2,
	WAYLAND_ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_INCOMPLETE			= 3,
	WAYLAND_ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_INVALID_FORMAT		= 4,
	WAYLAND_ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_INVALID_DIMENSIONS = 5,
	WAYLAND_ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_OUT_OF_BOUNDS		= 6,
	WAYLAND_ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_INVALID_WL_BUFFER	= 7,
};

enum wayland_zwp_linux_buffer_params_v1_flags {
	WAYLAND_ZWP_LINUX_BUFFER_PARAMS_V1_FLAGS_Y_INVERT	  = 0x01,
	WAYLAND_ZWP_LINUX_BUFFER_PARAMS_V1_FLAGS_INTERLACED	  = 0x02,
	WAYLAND_ZWP_LINUX_BUFFER_PARAMS_V1_FLAGS_BOTTOM_FIRST = 0x04,
};

struct wayland_zwp_linux_buffer_params_v1 {
	wayland_interface Interface;

	void (*Created)(
		wayland_zwp_linux_buffer_params_v1 *This,
		wayland_buffer					   *Buffer
	);
	void (*Failed)(wayland_zwp_linux_buffer_params_v1 *This);
};

enum wayland_zwp_linux_dmabuf_feedback_v1_tranche_flags {
	WAYLAND_ZWP_LINUX_DMABUF_FEEDBACK_V1_TRANCHE_FLAGS_SCANOUT = 0x01
};

struct wayland_zwp_linux_dmabuf_feedback_v1 {
	wayland_interface Interface;

	void (*Done)(wayland_zwp_linux_dmabuf_feedback_v1 *This);
	void (*FormatTable)(
		wayland_zwp_linux_dmabuf_feedback_v1 *This,
		s32									  Fd,
		u32									  Size
	);
	void (*MainDevice)(
		wayland_zwp_linux_dmabuf_feedback_v1 *This,
		wayland_array						  Device
	);
	void (*TrancheDone)(wayland_zwp_linux_dmabuf_feedback_v1 *This);
	void (*TrancheTargetDevice)(
		wayland_zwp_linux_dmabuf_feedback_v1 *This,
		wayland_array						  Device
	);
	void (*TrancheFormats)(
		wayland_zwp_linux_dmabuf_feedback_v1 *This,
		wayland_array						  Indices
	);
	void (*TrancheFlags)(
		wayland_zwp_linux_dmabuf_feedback_v1			  *This,
		wayland_zwp_linux_dmabuf_feedback_v1_tranche_flags Flags
	);
};

#endif

#ifndef SECTION_FUNCS

#define WAYLAND_PROTOCOL_FUNCS \
	INTERN(wayland_callback*,      Wayland_Display_Sync,               wayland_display *This) \
	INTERN(wayland_registry*,      Wayland_Display_GetRegistry,        wayland_display *This) \
	INTERN(wayland_interface*,     Wayland_Registry_Bind,              wayland_registry *This, u32 Name, c08 *Interface, u32 Version) \
	INTERN(wayland_surface*,       Wayland_Compositor_CreateSurface,   wayland_compositor *This) \
	INTERN(wayland_region*,        Wayland_Compositor_CreateRegion,    wayland_compositor *This) \
	INTERN(wayland_buffer*,        Wayland_ShmPool_CreateBuffer,       wayland_shm_pool *This, s32 Offset, s32 Width, s32 Height, s32 Stride, wayland_shm_format Format) \
	INTERN(void,                   Wayland_ShmPool_Destroy,            wayland_shm_pool *This) \
	INTERN(void,                   Wayland_ShmPool_Resize,             wayland_shm_pool *This, s32 Size) \
	INTERN(wayland_shm_pool*,      Wayland_Shm_CreatePool,             wayland_shm *This, s32 Fd, s32 Size) \
	INTERN(void,                   Wayland_Shm_Release,                wayland_shm *This) \
	INTERN(void,                   Wayland_Buffer_Destroy,             wayland_buffer *This) \
	INTERN(wayland_shell_surface*, Wayland_Shell_GetShellSurface,      wayland_shell *This, wayland_surface *Surface) \
	INTERN(void,                   Wayland_ShellSurface_Pong,          wayland_shell_surface *This, u32 Serial) \
	INTERN(void,                   Wayland_ShellSurface_Move,          wayland_shell_surface *This, wayland_seat *Seat, u32 Serial) \
	INTERN(void,                   Wayland_ShellSurface_Resize,        wayland_shell_surface *This, wayland_seat *Seat, u32 Serial, wayland_shell_surface_resize Edges) \
	INTERN(void,                   Wayland_ShellSurface_SetToplevel,   wayland_shell_surface *This) \
	INTERN(void,                   Wayland_ShellSurface_SetTransient,  wayland_shell_surface *This, wayland_surface *Parent, s32 X, s32 Y, wayland_shell_surface_transient Flags) \
	INTERN(void,                   Wayland_ShellSurface_SetFullscreen, wayland_shell_surface *This, wayland_shell_surface_fullscreen_method Method, u32 Framerate, wayland_output *Output) \
	INTERN(void,                   Wayland_ShellSurface_SetPopup,      wayland_shell_surface *This, wayland_seat *Seat, u32 Serial, wayland_surface *Parent, s32 X, s32 Y, wayland_shell_surface_transient *Flags) \
	INTERN(void,                   Wayland_ShellSurface_SetMaximized,  wayland_shell_surface *This, wayland_output *Output) \
	INTERN(void,                   Wayland_ShellSurface_SetTitle,      wayland_shell_surface *This, c08 *Title) \
	INTERN(void,                   Wayland_ShellSurface_SetClass,      wayland_shell_surface *This, c08 *Class) \
	INTERN(void,                   Wayland_Surface_Destroy,            wayland_surface *This) \
	INTERN(void,                   Wayland_Surface_Attach,             wayland_surface *This, wayland_buffer *Buffer, s32 X, s32 Y) \
	INTERN(void,                   Wayland_Surface_Damage,             wayland_surface *This, s32 X, s32 Y, s32 Width, s32 Height) \
	INTERN(wayland_callback*,      Wayland_Surface_Frame,              wayland_surface *This) \
	INTERN(void,                   Wayland_Surface_SetOpaqueRegion,    wayland_surface *This, wayland_region *Region) \
	INTERN(void,                   Wayland_Surface_SetInputRegion,     wayland_surface *This, wayland_region *Region) \
	INTERN(void,                   Wayland_Surface_Commit,             wayland_surface *This) \
	INTERN(void,                   Wayland_Surface_SetBufferTransform, wayland_surface *This, wayland_output_transform Transform) \
	INTERN(void,                   Wayland_Surface_SetBufferScale,     wayland_surface *This, s32 Scale) \
	INTERN(void,                   Wayland_Surface_DamageBuffer,       wayland_surface *This, s32 X, s32 Y, s32 Width, s32 Height) \
	INTERN(void,                   Wayland_Surface_Offset,             wayland_surface *This, s32 X, s32 Y) \
	INTERN(void,                   Wayland_Fixes_Destroy,              wayland_fixes *This) \
	INTERN(void,                   Wayland_Fixes_DestroyRegistry,      wayland_fixes *This, wayland_registry *Registry) \
	\
	INTERN(void,                    Wayland_XdgWmBase_Destroy,                     wayland_xdg_wm_base *This) \
	INTERN(wayland_xdg_positioner*, Wayland_XdgWmBase_CreatePositioner,            wayland_xdg_wm_base *This) \
	INTERN(wayland_xdg_surface*,    Wayland_XdgWmBase_GetXdgSurface,               wayland_xdg_wm_base *This, wayland_surface *Surface) \
	INTERN(void,                    Wayland_XdgWmBase_Pong,                        wayland_xdg_wm_base *This, u32 Serial) \
	INTERN(void,                    Wayland_XdgPositioner_Destroy,                 wayland_xdg_positioner *This) \
	INTERN(void,                    Wayland_XdgPositioner_SetSize,                 wayland_xdg_positioner *This, s32 Width, s32 Height) \
	INTERN(void,                    Wayland_XdgPositioner_SetAnchorRect,           wayland_xdg_positioner *This, s32 X, s32 Y, s32 Width, s32 Height) \
	INTERN(void,                    Wayland_XdgPositioner_SetAnchor,               wayland_xdg_positioner *This, wayland_xdg_positioner_anchor Anchor) \
	INTERN(void,                    Wayland_XdgPositioner_SetGravity,              wayland_xdg_positioner *This, wayland_xdg_positioner_gravity Gravity) \
	INTERN(void,                    Wayland_XdgPositioner_SetConstraintAdjustment, wayland_xdg_positioner *This, wayland_xdg_positioner_constraint_adjustment ConstraintAdjustment) \
	INTERN(void,                    Wayland_XdgPositioner_SetOffset,               wayland_xdg_positioner *This, s32 X, s32 Y) \
	INTERN(void,                    Wayland_XdgPositioner_SetReactive,             wayland_xdg_positioner *This) \
	INTERN(void,                    Wayland_XdgPositioner_SetParentSize,           wayland_xdg_positioner *This, s32 ParentWidth, s32 ParentHeight) \
	INTERN(void,                    Wayland_XdgPositioner_SetParentConfigure,      wayland_xdg_positioner *This, u32 Serial) \
	INTERN(void,                    Wayland_XdgSurface_Destroy,                    wayland_xdg_surface *This) \
	INTERN(wayland_xdg_toplevel*,   Wayland_XdgSurface_GetToplevel,                wayland_xdg_surface *This) \
	INTERN(wayland_xdg_popup*,      Wayland_XdgSurface_GetPopup,                   wayland_xdg_surface *This, wayland_xdg_surface *Parent, wayland_xdg_positioner *Positioner) \
	INTERN(void,                    Wayland_XdgSurface_SetWindowGeometry,          wayland_xdg_surface *This, s32 X, s32 Y, s32 Width, s32 Height) \
	INTERN(void,                    Wayland_XdgSurface_AckConfigure,               wayland_xdg_surface *This, u32 Serial) \
	INTERN(void,                    Wayland_XdgPopup_Destroy,                      wayland_xdg_popup *This) \
	INTERN(void,                    Wayland_XdgPopup_Grab,                         wayland_xdg_popup *This, wayland_seat *Seat, u32 Serial) \
	INTERN(void,                    Wayland_XdgPopup_Reposition,                   wayland_xdg_popup *This, wayland_xdg_positioner *Positioner, u32 Token) \
	INTERN(void,                    Wayland_XdgToplevel_Destroy,                   wayland_xdg_toplevel *This) \
	INTERN(void,                    Wayland_XdgToplevel_SetParent,                 wayland_xdg_toplevel *This, wayland_xdg_toplevel *Parent) \
	INTERN(void,                    Wayland_XdgToplevel_SetTitle,                  wayland_xdg_toplevel *This, c08 *Title) \
	INTERN(void,                    Wayland_XdgToplevel_SetAppId,                  wayland_xdg_toplevel *This, c08 *AppId) \
	INTERN(void,                    Wayland_XdgToplevel_ShowWindowMenu,            wayland_xdg_toplevel *This, wayland_seat *Seat, u32 Serial, s32 X, s32 Y) \
	INTERN(void,                    Wayland_XdgToplevel_Move,                      wayland_xdg_toplevel *This, wayland_seat *Seat, u32 Serial) \
	INTERN(void,                    Wayland_XdgToplevel_Resize,                    wayland_xdg_toplevel *This, wayland_seat *Seat, u32 Serial, wayland_xdg_toplevel_resize_edge Edges) \
	INTERN(void,                    Wayland_XdgToplevel_SetMaxSize,                wayland_xdg_toplevel *This, s32 Width, s32 Height) \
	INTERN(void,                    Wayland_XdgToplevel_SetMinSize,                wayland_xdg_toplevel *This, s32 Width, s32 Height) \
	INTERN(void,                    Wayland_XdgToplevel_SetMaximized,              wayland_xdg_toplevel *This) \
	INTERN(void,                    Wayland_XdgToplevel_UnsetMaximized,            wayland_xdg_toplevel *This) \
	INTERN(void,                    Wayland_XdgToplevel_SetFullscreen,             wayland_xdg_toplevel *This) \
	INTERN(void,                    Wayland_XdgToplevel_UnsetFullscreen,           wayland_xdg_toplevel *This) \
	INTERN(void,                    Wayland_XdgToplevel_SetMinimized,              wayland_xdg_toplevel *This) \
	\
	INTERN(void,            Wayland_Drm_Authenticate,       wayland_drm *This, u32 Id) \
	INTERN(wayland_buffer*, Wayland_Drm_CreateBuffer,       wayland_drm *This, u32 Name, s32 Width, s32 Height, u32 Stride, wayland_shm_format Format) \
	INTERN(wayland_buffer*, Wayland_Drm_CreatePlanarBuffer, wayland_drm *This, u32 Name, s32 Width, s32 Height, wayland_shm_format Format, s32 Offset0, s32 Stride0, s32 Offset1, s32 Stride1, s32 Offset2, s32 Stride2) \
	INTERN(wayland_buffer*, Wayland_Drm_CreatePrimeBuffer,  wayland_drm *This, s32 Name, s32 Width, s32 Height, wayland_shm_format Format, s32 Offset0, s32 Stride0, s32 Offset1, s32 Stride1, s32 Offset2, s32 Stride2) \
	\
	INTERN(void,                                  Wayland_ZwpLinuxDmabufV1_Destroy,            wayland_zwp_linux_dmabuf_v1 *This) \
	INTERN(wayland_zwp_linux_buffer_params_v1*,   Wayland_ZwpLinuxDmabufV1_CreateParams,       wayland_zwp_linux_dmabuf_v1 *This) \
	INTERN(wayland_zwp_linux_dmabuf_feedback_v1*, Wayland_ZwpLinuxDmabufV1_GetDefaultFeedback, wayland_zwp_linux_dmabuf_v1 *This) \
	INTERN(wayland_zwp_linux_dmabuf_feedback_v1*, Wayland_ZwpLinuxDmabufV1_GetSurfaceFeedback, wayland_zwp_linux_dmabuf_v1 *This, wayland_surface *Surface) \
	INTERN(void,                                  Wayland_ZwpLinuxBufferParamsV1_Destroy,      wayland_zwp_linux_buffer_params_v1 *This) \
	INTERN(void,                                  Wayland_ZwpLinuxBufferParamsV1_Add,          wayland_zwp_linux_buffer_params_v1 *This, s32 Fd, u32 PlaneIndex, u32 Offset, u32 Stride, u32 ModifierHigh, u32 ModifierLow) \
	INTERN(void,                                  Wayland_ZwpLinuxBufferParamsV1_Create,       wayland_zwp_linux_buffer_params_v1 *This, s32 Width, s32 Height, u32 Format, wayland_zwp_linux_buffer_params_v1_flags Flags) \
	INTERN(wayland_buffer*,                       Wayland_ZwpLinuxBufferParamsV1_CreateImmed,  wayland_zwp_linux_buffer_params_v1 *This, s32 Width, s32 Height, u32 Format, wayland_zwp_linux_buffer_params_v1_flags Flags) \
	INTERN(void,                                  Wayland_ZwpLinuxDmabufFeedbackV1_Destroy,    wayland_zwp_linux_dmabuf_feedback_v1 *This) \
	//

#endif

#endif

#ifdef INCLUDE_SOURCE

#define M (wayland_method[])
#define P (wayland_prototype *[])

#ifndef SECTION_WAYLAND

wayland_prototype WaylandDisplayPrototype = {
	.Name		  = "wl_display",
	.Version	  = 1,
	.RequestCount = 2,
	.EventCount	  = 2,
	.Requests =
		M{
			{ "sync", "on", P{ &WaylandCallbackPrototype } },
			{ "get_registry", "on", P{ &WaylandRegistryPrototype } },
		  },
	.Events =
		M{
			{ "error", "oous", NULL },
			{ "delete_id", "ou", NULL },
		  },
};

wayland_prototype WaylandRegistryPrototype = {
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

wayland_prototype WaylandCallbackPrototype = {
	.Name		  = "wl_callback",
	.Version	  = 0,
	.RequestCount = 0,
	.EventCount	  = 1,
	.Requests	  = NULL,
	.Events		  = M{ { "done", "ou", NULL } },
};

wayland_prototype WaylandCompositorPrototype = {
	.Name		  = "wl_compositor",
	.Version	  = 6,
	.RequestCount = 2,
	.EventCount	  = 0,
	.Requests =
		M{
			{ "create_surface", "on", P{ &WaylandSurfacePrototype } },
			{ "create_region", "on", P{ &WaylandRegionPrototype } },
		  },
	.Events = NULL,
};

wayland_prototype WaylandShmPoolPrototype = {
	.Name		  = "wl_shm_pool",
	.Version	  = 2,
	.RequestCount = 3,
	.EventCount	  = 0,
	.Requests =
		M{
			{ "create_buffer", "oniiiiu", P{ &WaylandBufferPrototype } },
			{ "destroy", "d", NULL },
			{ "resize", "oi", NULL },
		  },
	.Events = NULL,
};

wayland_prototype WaylandShmPrototype = {
	.Name		  = "wl_shm",
	.Version	  = 2,
	.RequestCount = 2,
	.EventCount	  = 1,
	.Requests =
		M{
			{ "create_pool", "onfi", P{ &WaylandShmPoolPrototype } },
			{ "release", "2d", NULL },
		  },
	.Events =
		M{
			{ "format", "ou", NULL },
		  },
};

wayland_prototype WaylandBufferPrototype = {
	.Name		  = "wl_buffer",
	.Version	  = 0,
	.RequestCount = 1,
	.EventCount	  = 1,
	.Requests	  = M{ { "destroy", "d", NULL } },
	.Events		  = M{ { "release", "o", NULL } },
};

wayland_prototype WaylandDataOfferPrototype = {
	.Name		  = "wl_data_offer",
	.Version	  = 3,
	.RequestCount = 5,
	.EventCount	  = 3,
	.Requests =
		M{
			{ "accept", "ou?s", NULL },
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

wayland_prototype WaylandDataSourcePrototype = {
	.Name		  = "wl_data_source",
	.Version	  = 3,
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
			{ "target", "o?s", NULL },
			{ "send", "osf", NULL },
			{ "cancelled", "3o", NULL },
			{ "dnd_drop_performed", "3o", NULL },
			{ "dnd_finished", "3o", NULL },
			{ "action", "3ou", NULL },
		  },
};

wayland_prototype WaylandDataDevicePrototype = {
	.Name		  = "wl_data_device",
	.Version	  = 3,
	.RequestCount = 3,
	.EventCount	  = 6,
	.Requests =
		M{
			{ "start_drag", "o?oo?ou", NULL },
			{ "set_selection", "o?ou", NULL },
			{ "release", "2d", NULL },
		  },
	.Events =
		M{
			{ "data_offer", "on", P{ &WaylandDataOfferPrototype } },
			{ "enter", "ouorr?o", NULL },
			{ "leave", "o", NULL },
			{ "motion", "ourr", NULL },
			{ "drop", "o", NULL },
			{ "selection", "o?o", NULL },
		  },
};

wayland_prototype WaylandDataDeviceManagerPrototype = {
	.Name		  = "wl_data_device_manager",
	.Version	  = 3,
	.RequestCount = 2,
	.EventCount	  = 0,
	.Requests =
		M{
			{
				"create_data_source",
				"on",
				P{ &WaylandDataSourcePrototype },
			}, {
				"get_data_device",
				"ono",
				P{ &WaylandDataDevicePrototype },
			}, },
	.Events = NULL,
};

wayland_prototype WaylandShellPrototype = {
	.Name		  = "wl_shell",
	.Version	  = 1,
	.RequestCount = 1,
	.EventCount	  = 0,
	.Requests =
		M{
			{ "get_shell_surface", "_ono", P{ &WaylandShellSurfacePrototype } },
		  },
	.Events = NULL,
};

wayland_prototype WaylandShellSurfacePrototype = {
	.Name		  = "wl_shell_surface",
	.Version	  = 1,
	.RequestCount = 10,
	.EventCount	  = 3,
	.Requests =
		M{
			{ "pong", "_ou", NULL },
			{ "move", "_oou", NULL },
			{ "resize", "_oouu", NULL },
			{ "set_toplevel", "_o", NULL },
			{ "set_transient", "_ooiiu", NULL },
			{ "set_fullscreen", "_ouu?o", NULL },
			{ "set_popup", "_oouoiiu", NULL },
			{ "set_maximized", "_o?o", NULL },
			{ "set_title", "_os", NULL },
			{ "set_class", "_os", NULL },
		  },
	.Events =
		M{
			{ "ping", "_ou", NULL },
			{ "configure", "_ouii", NULL },
			{ "popup_done", "_o", NULL },
		  },
};

wayland_prototype WaylandSurfacePrototype = {
	.Name		  = "wl_surface",
	.Version	  = 6,
	.RequestCount = 11,
	.EventCount	  = 4,
	.Requests =
		M{
			{ "destroy", "d", NULL },
			{ "attach", "o?oii", NULL },
			{ "damage", "oiiii", NULL },
			{ "frame", "on", P{ &WaylandCallbackPrototype } },
			{ "set_opaque_region", "o?o", NULL },
			{ "set_input_region", "o?o", NULL },
			{ "commit", "o", NULL },
			{ "set_buffer_transform", "2oi", NULL },
			{ "set_buffer_scale", "3oi", NULL },
			{ "damage_buffer", "4oiiii", NULL },
			{ "offset", "5oii", NULL },
		  },
	.Events =
		M{
			{ "enter", "oo", NULL },
			{ "leave", "oo", NULL },
			{ "preferred_buffer_scale", "6oi", NULL },
			{ "preferred_buffer_transform", "6ou", NULL },
		  },
};

wayland_prototype WaylandSeatPrototype = {
	.Name		  = "wl_seat",
	.Version	  = 10,
	.RequestCount = 4,
	.EventCount	  = 2,
	.Requests =
		M{
			{ "get_pointer", "on", P{ &WaylandPointerPrototype } },
			{ "get_keyboard", "on", P{ &WaylandKeyboardPrototype } },
			{ "get_touch", "on", P{ &WaylandTouchPrototype } },
			{ "release", "5d", NULL },
		  },
	.Events =
		M{
			{ "capabilities", "ou", NULL },
			{ "name", "2os", NULL },
		  },
};

wayland_prototype WaylandPointerPrototype = {
	.Name		  = "wl_pointer",
	.Version	  = 10,
	.RequestCount = 2,
	.EventCount	  = 11,
	.Requests =
		M{
			{ "set_cursor", "ou?oii", NULL },
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
			{ "axis_discrete", "5_8oui", NULL },
			{ "axis_value120", "8oui", NULL },
			{ "axis_relative_direction", "9ouu", NULL },
		  },
};

wayland_prototype WaylandKeyboardPrototype = {
	.Name		  = "wl_keyboard",
	.Version	  = 10,
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

wayland_prototype WaylandTouchPrototype = {
	.Name		  = "wl_touch",
	.Version	  = 10,
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

wayland_prototype WaylandOutputPrototype = {
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

wayland_prototype WaylandRegionPrototype = {
	.Name		  = "wl_region",
	.Version	  = 1,
	.RequestCount = 3,
	.EventCount	  = 0,
	.Requests =
		M{
			{ "destroy", "d", NULL },
			{ "add", "oiiii", NULL },
			{ "subtract", "oiiii", NULL },
		  },
	.Events = NULL,
};

wayland_prototype WaylandSubcompositorPrototype = {
	.Name		  = "wl_subcompositor",
	.Version	  = 1,
	.RequestCount = 2,
	.EventCount	  = 0,
	.Requests =
		M{
			{ "destroy", "d", NULL },
			{ "get_subsurface", "onoo", P{ &WaylandSubsurfacePrototype } },
		  },
	.Events = NULL,
};

wayland_prototype WaylandSubsurfacePrototype = {
	.Name		  = "wl_subsurface",
	.Version	  = 1,
	.RequestCount = 6,
	.EventCount	  = 0,
	.Requests =
		M{
			{ "destroy", "d", NULL },
			{ "set_position", "oii", NULL },
			{ "place_above", "oo", NULL },
			{ "place_below", "oo", NULL },
			{ "set_sync", "o", NULL },
			{ "set_desync", "o", NULL },
		  },
	.Events = NULL,
};

wayland_prototype WaylandFixesPrototype = {
	.Name		  = "wl_fixes",
	.Version	  = 1,
	.RequestCount = 2,
	.EventCount	  = 0,
	.Requests =
		M{
			{ "destroy", "d", NULL },
			{ "destroy_registry", "od", NULL },
		  },
	.Events = NULL,
};

internal wayland_callback *
Wayland_Display_Sync(wayland_display *This)
{ return Wayland_HandleConstructorRequest((wayland_interface *) This, 0); }

internal wayland_registry *
Wayland_Display_GetRegistry(wayland_display *This)
{ return Wayland_HandleConstructorRequest((wayland_interface *) This, 1); }

internal wayland_interface *
Wayland_Registry_Bind(
	wayland_registry *This,
	u32				  Name,
	c08				 *Interface,
	u32				  Version
)
{
	return Wayland_HandleConstructorRequest(
		(wayland_interface *) This,
		0,
		Name,
		Interface,
		Version
	);
}

internal wayland_surface *
Wayland_Compositor_CreateSurface(wayland_compositor *This)
{ return Wayland_HandleConstructorRequest((wayland_interface *) This, 0); }

internal wayland_region *
Wayland_Compositor_CreateRegion(wayland_compositor *This)
{ return Wayland_HandleConstructorRequest((wayland_interface *) This, 1); }

internal wayland_buffer *
Wayland_ShmPool_CreateBuffer(
	wayland_shm_pool  *This,
	s32				   Offset,
	s32				   Width,
	s32				   Height,
	s32				   Stride,
	wayland_shm_format Format
)
{
	return Wayland_HandleConstructorRequest(
		(wayland_interface *) This,
		0,
		Offset,
		Width,
		Height,
		Stride,
		Format
	);
}

internal void
Wayland_ShmPool_Destroy(wayland_shm_pool *This)
{ Wayland_HandleRequest((wayland_interface *) This, 1); }

internal void
Wayland_ShmPool_Resize(wayland_shm_pool *This, s32 Size)
{ Wayland_HandleRequest((wayland_interface *) This, 2, Size); }

internal wayland_shm_pool *
Wayland_Shm_CreatePool(wayland_shm *This, s32 Fd, s32 Size)
{
	return Wayland_HandleConstructorRequest(
		(wayland_interface *) This,
		0,
		Fd,
		Size
	);
}

internal void
Wayland_Shm_Release(wayland_shm *This)
{ Wayland_HandleRequest((wayland_interface *) This, 1); }

internal void
Wayland_Buffer_Destroy(wayland_buffer *This)
{ Wayland_HandleRequest((wayland_interface *) This, 0); }

internal wayland_shell_surface *
Wayland_Shell_GetShellSurface(wayland_shell *This, wayland_surface *Surface)
{
	return Wayland_HandleConstructorRequest(
		(wayland_interface *) This,
		0,
		Surface
	);
}

internal void
Wayland_ShellSurface_Pong(wayland_shell_surface *This, u32 Serial)
{ Wayland_HandleRequest((wayland_interface *) This, 0, Serial); }

internal void
Wayland_ShellSurface_Move(
	wayland_shell_surface *This,
	wayland_seat		  *Seat,
	u32					   Serial
)
{ Wayland_HandleRequest((wayland_interface *) This, 1, Seat, Serial); }

internal void
Wayland_ShellSurface_Resize(
	wayland_shell_surface		*This,
	wayland_seat				*Seat,
	u32							 Serial,
	wayland_shell_surface_resize Edges
)
{ Wayland_HandleRequest((wayland_interface *) This, 2, Seat, Serial, Edges); }

internal void
Wayland_ShellSurface_SetToplevel(wayland_shell_surface *This)
{ Wayland_HandleRequest((wayland_interface *) This, 3); }

internal void
Wayland_ShellSurface_SetTransient(
	wayland_shell_surface		   *This,
	wayland_surface				   *Parent,
	s32								X,
	s32								Y,
	wayland_shell_surface_transient Flags
)
{ Wayland_HandleRequest((wayland_interface *) This, 4, Parent, X, Y, Flags); }

internal void
Wayland_ShellSurface_SetFullscreen(
	wayland_shell_surface				   *This,
	wayland_shell_surface_fullscreen_method Method,
	u32										Framerate,
	wayland_output						   *Output
)
{
	Wayland_HandleRequest(
		(wayland_interface *) This,
		5,
		Method,
		Framerate,
		Output
	);
}

internal void
Wayland_ShellSurface_SetPopup(
	wayland_shell_surface			*This,
	wayland_seat					*Seat,
	u32								 Serial,
	wayland_surface					*Parent,
	s32								 X,
	s32								 Y,
	wayland_shell_surface_transient *Flags
)
{
	Wayland_HandleRequest(
		(wayland_interface *) This,
		6,
		Seat,
		Serial,
		Parent,
		X,
		Y,
		Flags
	);
}

internal void
Wayland_ShellSurface_SetMaximized(
	wayland_shell_surface *This,
	wayland_output		  *Output
)
{ Wayland_HandleRequest((wayland_interface *) This, 7, Output); }

internal void
Wayland_ShellSurface_SetTitle(wayland_shell_surface *This, c08 *Title)
{ Wayland_HandleRequest((wayland_interface *) This, 8, Title); }

internal void
Wayland_ShellSurface_SetClass(wayland_shell_surface *This, c08 *Class)
{ Wayland_HandleRequest((wayland_interface *) This, 9, Class); }

internal void
Wayland_Surface_Destroy(wayland_surface *This)
{ Wayland_HandleRequest((wayland_interface *) This, 0); }

internal void
Wayland_Surface_Attach(
	wayland_surface *This,
	wayland_buffer	*Buffer,
	s32				 X,
	s32				 Y
)
{ Wayland_HandleRequest((wayland_interface *) This, 1, Buffer, X, Y); }

internal void
Wayland_Surface_Damage(
	wayland_surface *This,
	s32				 X,
	s32				 Y,
	s32				 Width,
	s32				 Height
)
{ Wayland_HandleRequest((wayland_interface *) This, 2, X, Y, Width, Height); }

internal wayland_callback *
Wayland_Surface_Frame(wayland_surface *This)
{ return Wayland_HandleConstructorRequest((wayland_interface *) This, 3); }

internal void
Wayland_Surface_SetOpaqueRegion(wayland_surface *This, wayland_region *Region)
{ Wayland_HandleRequest((wayland_interface *) This, 4, Region); }

internal void
Wayland_Surface_SetInputRegion(wayland_surface *This, wayland_region *Region)
{ Wayland_HandleRequest((wayland_interface *) This, 5, Region); }

internal void
Wayland_Surface_Commit(wayland_surface *This)
{ Wayland_HandleRequest((wayland_interface *) This, 6); }

internal void
Wayland_Surface_SetBufferTransform(
	wayland_surface			*This,
	wayland_output_transform Transform
)
{ Wayland_HandleRequest((wayland_interface *) This, 7, Transform); }

internal void
Wayland_Surface_SetBufferScale(wayland_surface *This, s32 Scale)
{ Wayland_HandleRequest((wayland_interface *) This, 8, Scale); }

internal void
Wayland_Surface_DamageBuffer(
	wayland_surface *This,
	s32				 X,
	s32				 Y,
	s32				 Width,
	s32				 Height
)
{ Wayland_HandleRequest((wayland_interface *) This, 9, X, Y, Width, Height); }

internal void
Wayland_Surface_Offset(wayland_surface *This, s32 X, s32 Y)
{ Wayland_HandleRequest((wayland_interface *) This, 10, X, Y); }

internal void
Wayland_Fixes_Destroy(wayland_fixes *This)
{ Wayland_HandleRequest((wayland_interface *) This, 0); }

internal void
Wayland_Fixes_DestroyRegistry(wayland_fixes *This, wayland_registry *Registry)
{ Wayland_HandleRequest((wayland_interface *) This, 1, Registry); }

#endif

#ifndef SECTION_XDG_SHELL

wayland_prototype WaylandXdgWmBasePrototype = {
	.Name		  = "xdg_wm_base",
	.Version	  = 7,
	.RequestCount = 4,
	.EventCount	  = 1,
	.Requests =
		M{
			{ "destroy", "d", NULL },
			{ "create_positioner", "on", P{ &WaylandXdgPositionerPrototype } },
			{ "get_xdg_surface", "ono", P{ &WaylandXdgSurfacePrototype } },
			{ "pong", "ou", NULL },
		  },
	.Events =
		M{
			{ "ping", "ou", NULL },
		  },
};

wayland_prototype WaylandXdgPositionerPrototype = {
	.Name		  = "xdg_positioner",
	.Version	  = 7,
	.RequestCount = 10,
	.EventCount	  = 0,
	.Requests =
		M{
			{ "destroy", "d", NULL },
			{ "set_size", "oii", NULL },
			{ "set_anchor_rect", "oiiii", NULL },
			{ "set_anchor", "ou", NULL },
			{ "set_gravity", "ou", NULL },
			{ "set_constraint_adjustment", "ou", NULL },
			{ "set_offset", "oii", NULL },
			{ "set_reactive", "3o", NULL },
			{ "set_parent_size", "3oii", NULL },
			{ "set_parent_configure", "3ou", NULL },
		  },
	.Events = NULL,
};

wayland_prototype WaylandXdgSurfacePrototype = {
	.Name		  = "xdg_surface",
	.Version	  = 7,
	.RequestCount = 5,
	.EventCount	  = 1,
	.Requests =
		M{
			{ "destroy", "d", NULL },
			{ "get_toplevel", "on", P{ &WaylandXdgToplevelPrototype } },
			{ "get_popup", "on?oo", P{ &WaylandXdgPopupPrototype } },
			{ "set_window_geometry", "oiiii", NULL },
			{ "ack_configure", "ou", NULL },
		  },
	.Events =
		M{
			{ "configure", "ou", NULL },
		  },
};

wayland_prototype WaylandXdgToplevelPrototype = {
	.Name		  = "xdg_toplevel",
	.Version	  = 7,
	.RequestCount = 14,
	.EventCount	  = 4,
	.Requests =
		M{
			{ "destroy", "d", NULL },
			{ "set_parent", "o?o", NULL },
			{ "set_title", "os", NULL },
			{ "set_app_id", "os", NULL },
			{ "show_window_menu", "oouii", NULL },
			{ "move", "oou", NULL },
			{ "resize", "oouu", NULL },
			{ "set_max_size", "oii", NULL },
			{ "set_min_size", "oii", NULL },
			{ "set_maximized", "o", NULL },
			{ "unset_maximized", "o", NULL },
			{ "set_fullscreen", "o?o", NULL },
			{ "unset_fullscreen", "o", NULL },
			{ "set_minimized", "o", NULL },
		  },
	.Events =
		M{
			{ "configure", "oiia", NULL },
			{ "close", "o", NULL },
			{ "configure_bounds", "4oii", NULL },
			{ "wm_capabilities", "5oa", NULL },
		  },
};

wayland_prototype WaylandXdgPopupPrototype = {
	.Name		  = "xdg_popup",
	.Version	  = 7,
	.RequestCount = 3,
	.EventCount	  = 3,
	.Requests =
		M{
			{ "destroy", "d", NULL },
			{ "grab", "oou", NULL },
			{ "reposition", "3oou", NULL },
		  },
	.Events =
		M{
			{ "configure", "oiiii", NULL },
			{ "popup_done", "o", NULL },
			{ "repositioned", "3ou", NULL },
		  },
};

internal void
Wayland_XdgWmBase_Destroy(wayland_xdg_wm_base *This)
{ Wayland_HandleRequest((wayland_interface *) This, 0); }

internal wayland_xdg_positioner *
Wayland_XdgWmBase_CreatePositioner(wayland_xdg_wm_base *This)
{ return Wayland_HandleConstructorRequest((wayland_interface *) This, 1); }

internal wayland_xdg_surface *
Wayland_XdgWmBase_GetXdgSurface(
	wayland_xdg_wm_base *This,
	wayland_surface		*Surface
)
{
	return Wayland_HandleConstructorRequest(
		(wayland_interface *) This,
		2,
		Surface
	);
}

internal void
Wayland_XdgWmBase_Pong(wayland_xdg_wm_base *This, u32 Serial)
{ Wayland_HandleRequest((wayland_interface *) This, 3, Serial); }

internal void
Wayland_XdgPositioner_Destroy(wayland_xdg_positioner *This)
{ Wayland_HandleRequest((wayland_interface *) This, 0); }

internal void
Wayland_XdgPositioner_SetSize(
	wayland_xdg_positioner *This,
	s32						Width,
	s32						Height
)
{ Wayland_HandleRequest((wayland_interface *) This, 1, Width, Height); }

internal void
Wayland_XdgPositioner_SetAnchorRect(
	wayland_xdg_positioner *This,
	s32						X,
	s32						Y,
	s32						Width,
	s32						Height
)
{ Wayland_HandleRequest((wayland_interface *) This, 2, X, Y, Width, Height); }

internal void
Wayland_XdgPositioner_SetAnchor(
	wayland_xdg_positioner		 *This,
	wayland_xdg_positioner_anchor Anchor
)
{ Wayland_HandleRequest((wayland_interface *) This, 3, Anchor); }

internal void
Wayland_XdgPositioner_SetGravity(
	wayland_xdg_positioner		  *This,
	wayland_xdg_positioner_gravity Gravity
)
{ Wayland_HandleRequest((wayland_interface *) This, 4, Gravity); }

internal void
Wayland_XdgPositioner_SetConstraintAdjustment(
	wayland_xdg_positioner						*This,
	wayland_xdg_positioner_constraint_adjustment ConstraintAdjustment
)
{ Wayland_HandleRequest((wayland_interface *) This, 5, ConstraintAdjustment); }

internal void
Wayland_XdgPositioner_SetOffset(wayland_xdg_positioner *This, s32 X, s32 Y)
{ Wayland_HandleRequest((wayland_interface *) This, 6, X, Y); }

internal void
Wayland_XdgPositioner_SetReactive(wayland_xdg_positioner *This)
{ Wayland_HandleRequest((wayland_interface *) This, 7); }

internal void
Wayland_XdgPositioner_SetParentSize(
	wayland_xdg_positioner *This,
	s32						ParentWidth,
	s32						ParentHeight
)
{
	Wayland_HandleRequest(
		(wayland_interface *) This,
		8,
		ParentWidth,
		ParentHeight
	);
}

internal void
Wayland_XdgPositioner_SetParentConfigure(
	wayland_xdg_positioner *This,
	u32						Serial
)
{ Wayland_HandleRequest((wayland_interface *) This, 9, Serial); }

internal void
Wayland_XdgSurface_Destroy(wayland_xdg_surface *This)
{ Wayland_HandleRequest((wayland_interface *) This, 0); }

internal wayland_xdg_toplevel *
Wayland_XdgSurface_GetToplevel(wayland_xdg_surface *This)
{ return Wayland_HandleConstructorRequest((wayland_interface *) This, 1); }

internal wayland_xdg_popup *
Wayland_XdgSurface_GetPopup(
	wayland_xdg_surface	   *This,
	wayland_xdg_surface	   *Parent,
	wayland_xdg_positioner *Positioner
)
{
	return Wayland_HandleConstructorRequest(
		(wayland_interface *) This,
		2,
		Parent,
		Positioner
	);
}

internal void
Wayland_XdgSurface_SetWindowGeometry(
	wayland_xdg_surface *This,
	s32					 X,
	s32					 Y,
	s32					 Width,
	s32					 Height
)
{ Wayland_HandleRequest((wayland_interface *) This, 3, X, Y, Width, Height); }

internal void
Wayland_XdgSurface_AckConfigure(wayland_xdg_surface *This, u32 Serial)
{ Wayland_HandleRequest((wayland_interface *) This, 4, Serial); }

internal void
Wayland_XdgToplevel_Destroy(wayland_xdg_toplevel *This)
{ Wayland_HandleRequest((wayland_interface *) This, 0); }

internal void
Wayland_XdgToplevel_SetParent(
	wayland_xdg_toplevel *This,
	wayland_xdg_toplevel *Parent
)
{ Wayland_HandleRequest((wayland_interface *) This, 1, Parent); }

internal void
Wayland_XdgToplevel_SetTitle(wayland_xdg_toplevel *This, c08 *Title)
{ Wayland_HandleRequest((wayland_interface *) This, 2, Title); }

internal void
Wayland_XdgToplevel_SetAppId(wayland_xdg_toplevel *This, c08 *AppId)
{ Wayland_HandleRequest((wayland_interface *) This, 3, AppId); }

internal void
Wayland_XdgToplevel_ShowWindowMenu(
	wayland_xdg_toplevel *This,
	wayland_seat		 *Seat,
	u32					  Serial,
	s32					  X,
	s32					  Y
)
{ Wayland_HandleRequest((wayland_interface *) This, 4, Seat, Serial, X, Y); }

internal void
Wayland_XdgToplevel_Move(
	wayland_xdg_toplevel *This,
	wayland_seat		 *Seat,
	u32					  Serial
)
{ Wayland_HandleRequest((wayland_interface *) This, 5, Seat, Serial); }

internal void
Wayland_XdgToplevel_Resize(
	wayland_xdg_toplevel			*This,
	wayland_seat					*Seat,
	u32								 Serial,
	wayland_xdg_toplevel_resize_edge Edges
)
{ Wayland_HandleRequest((wayland_interface *) This, 6, Seat, Serial, Edges); }

internal void
Wayland_XdgToplevel_SetMaxSize(
	wayland_xdg_toplevel *This,
	s32					  Width,
	s32					  Height
)
{ Wayland_HandleRequest((wayland_interface *) This, 7, Width, Height); }

internal void
Wayland_XdgToplevel_SetMinSize(
	wayland_xdg_toplevel *This,
	s32					  Width,
	s32					  Height
)
{ Wayland_HandleRequest((wayland_interface *) This, 8, Width, Height); }

internal void
Wayland_XdgToplevel_SetMaximized(wayland_xdg_toplevel *This)
{ Wayland_HandleRequest((wayland_interface *) This, 9); }

internal void
Wayland_XdgToplevel_UnsetMaximized(wayland_xdg_toplevel *This)
{ Wayland_HandleRequest((wayland_interface *) This, 10); }

internal void
Wayland_XdgToplevel_SetFullscreen(wayland_xdg_toplevel *This)
{ Wayland_HandleRequest((wayland_interface *) This, 11); }

internal void
Wayland_XdgToplevel_UnsetFullscreen(wayland_xdg_toplevel *This)
{ Wayland_HandleRequest((wayland_interface *) This, 12); }

internal void
Wayland_XdgToplevel_SetMinimized(wayland_xdg_toplevel *This)
{ Wayland_HandleRequest((wayland_interface *) This, 13); }

internal void
Wayland_XdgPopup_Destroy(wayland_xdg_popup *This)
{ Wayland_HandleRequest((wayland_interface *) This, 0); }

internal void
Wayland_XdgPopup_Grab(wayland_xdg_popup *This, wayland_seat *Seat, u32 Serial)
{ Wayland_HandleRequest((wayland_interface *) This, 1, Seat, Serial); }

internal void
Wayland_XdgPopup_Reposition(
	wayland_xdg_popup	   *This,
	wayland_xdg_positioner *Positioner,
	u32						Token
)
{ Wayland_HandleRequest((wayland_interface *) This, 2, Positioner, Token); }

#endif

#ifndef SECTION_DRM

wayland_prototype WaylandDrmPrototype = {
	.Name		  = "wl_drm",
	.Version	  = 2,
	.RequestCount = 4,
	.EventCount	  = 4,
	.Requests =
		M{
			{ "authenticate", "ou", NULL },
			{ "create_buffer", "onuiiuu", P{ &WaylandBufferPrototype } },
			{ "create_planar_buffer",
			"onuiiuiiiiii",
			P{ &WaylandBufferPrototype } },
			{ "create_prime_buffer",
			"onfiiuiiiiii",
			P{ &WaylandBufferPrototype } },
		  },
	.Events =
		M{
			{ "device", "os", NULL },
			{ "format", "ou", NULL },
			{ "authenticated", "o", NULL },
			{ "capabilities", "ou", NULL },
		  },
};

internal void
Wayland_Drm_Authenticate(wayland_drm *This, u32 Id)
{ Wayland_HandleRequest((wayland_interface *) This, 0, Id); }

internal wayland_buffer *
Wayland_Drm_CreateBuffer(
	wayland_drm		  *This,
	u32				   Name,
	s32				   Width,
	s32				   Height,
	u32				   Stride,
	wayland_shm_format Format
)
{
	return Wayland_HandleConstructorRequest(
		(wayland_interface *) This,
		1,
		Name,
		Width,
		Height,
		Stride,
		Format
	);
}

internal wayland_buffer *
Wayland_Drm_CreatePlanarBuffer(
	wayland_drm		  *This,
	u32				   Name,
	s32				   Width,
	s32				   Height,
	wayland_shm_format Format,
	s32				   Offset0,
	s32				   Stride0,
	s32				   Offset1,
	s32				   Stride1,
	s32				   Offset2,
	s32				   Stride2
)
{
	return Wayland_HandleConstructorRequest(
		(wayland_interface *) This,
		1,
		Name,
		Width,
		Height,
		Format,
		Offset0,
		Stride0,
		Offset1,
		Stride1,
		Offset2,
		Stride2
	);
}

internal wayland_buffer *
Wayland_Drm_CreatePrimeBuffer(
	wayland_drm		  *This,
	s32				   Name,
	s32				   Width,
	s32				   Height,
	wayland_shm_format Format,
	s32				   Offset0,
	s32				   Stride0,
	s32				   Offset1,
	s32				   Stride1,
	s32				   Offset2,
	s32				   Stride2
)
{
	return Wayland_HandleConstructorRequest(
		(wayland_interface *) This,
		2,
		Name,
		Width,
		Height,
		Format,
		Offset0,
		Stride0,
		Offset1,
		Stride1,
		Offset2,
		Stride2
	);
}

#endif

#ifndef SECTION_LINUX_DMABUF

wayland_prototype WaylandZwpLinuxDmabufV1Prototype = {
	.Name		  = "zwp_linux_dmabuf_v1",
	.Version	  = 5,
	.RequestCount = 4,
	.EventCount	  = 2,
	.Requests =
		M{
			{ "destroy", "d", NULL },
			{ "create_params",
				"on",
				P{ &WaylandZwpLinuxBufferParamsV1Prototype } },
			{ "get_default_feedback",
				"4on",
				P{ &WaylandZwpLinuxDmabufFeedbackV1Prototype } },
			{ "get_surface_feedback",
				"4ono",
				P{ &WaylandZwpLinuxDmabufFeedbackV1Prototype } },
		  },
	.Events =
		M{
			{ "format", "ou", NULL },
			{ "modifier", "3_4ouuu", NULL },
		  },
};

wayland_prototype WaylandZwpLinuxBufferParamsV1Prototype = {
	.Name		  = "zwp_linux_buffer_params_v1",
	.Version	  = 5,
	.RequestCount = 4,
	.EventCount	  = 2,
	.Requests =
		M{
			{ "destroy", "d", NULL },
			{ "add", "ofuuuuu", NULL },
			{ "create", "oiiuu", NULL },
			{ "create_immed", "2oniiuu", P{ &WaylandBufferPrototype } },
		  },
	.Events =
		M{
			{ "created", "on", P{ &WaylandBufferPrototype } },
			{ "failed", "o", NULL },
		  },
};

wayland_prototype WaylandZwpLinuxDmabufFeedbackV1Prototype = {
	.Name		  = "zwp_linux_dmabuf_feedback_v1",
	.Version	  = 5,
	.RequestCount = 1,
	.EventCount	  = 7,
	.Requests =
		M{
			{ "destroy", "d", NULL },
		  },
	.Events =
		M{
			{ "done", "o", NULL },
			{ "format_table", "ofu", NULL },
			{ "main_device", "oa", NULL },
			{ "tranche_done", "o", NULL },
			{ "tranche_target_device", "oa", NULL },
			{ "tranche_formats", "oa", NULL },
			{ "tranche_flags", "ou", NULL },
		  },
};

internal void
Wayland_ZwpLinuxDmabufV1_Destroy(wayland_zwp_linux_dmabuf_v1 *This)
{ Wayland_HandleRequest((wayland_interface *) This, 0); }

internal wayland_zwp_linux_buffer_params_v1 *
Wayland_ZwpLinuxDmabufV1_CreateParams(wayland_zwp_linux_dmabuf_v1 *This)
{ return Wayland_HandleConstructorRequest((wayland_interface *) This, 1); }

internal wayland_zwp_linux_dmabuf_feedback_v1 *
Wayland_ZwpLinuxDmabufV1_GetDefaultFeedback(wayland_zwp_linux_dmabuf_v1 *This)
{ return Wayland_HandleConstructorRequest((wayland_interface *) This, 2); }

internal wayland_zwp_linux_dmabuf_feedback_v1 *
Wayland_ZwpLinuxDmabufV1_GetSurfaceFeedback(
	wayland_zwp_linux_dmabuf_v1 *This,
	wayland_surface				*Surface
)
{
	return Wayland_HandleConstructorRequest(
		(wayland_interface *) This,
		3,
		Surface
	);
}

internal void
Wayland_ZwpLinuxBufferParamsV1_Destroy(wayland_zwp_linux_buffer_params_v1 *This)
{ Wayland_HandleRequest((wayland_interface *) This, 0); }

internal void
Wayland_ZwpLinuxBufferParamsV1_Add(
	wayland_zwp_linux_buffer_params_v1 *This,
	s32									Fd,
	u32									PlaneIndex,
	u32									Offset,
	u32									Stride,
	u32									ModifierHigh,
	u32									ModifierLow
)
{
	Wayland_HandleRequest(
		(wayland_interface *) This,
		1,
		Fd,
		PlaneIndex,
		Offset,
		Stride,
		ModifierHigh,
		ModifierLow
	);
}

internal void
Wayland_ZwpLinuxBufferParamsV1_Create(
	wayland_zwp_linux_buffer_params_v1		*This,
	s32										 Width,
	s32										 Height,
	u32										 Format,
	wayland_zwp_linux_buffer_params_v1_flags Flags
)
{
	Wayland_HandleRequest(
		(wayland_interface *) This,
		2,
		Width,
		Height,
		Format,
		Flags
	);
}

internal wayland_buffer *
Wayland_ZwpLinuxBufferParamsV1_CreateImmed(
	wayland_zwp_linux_buffer_params_v1		*This,
	s32										 Width,
	s32										 Height,
	u32										 Format,
	wayland_zwp_linux_buffer_params_v1_flags Flags
)
{
	return Wayland_HandleConstructorRequest(
		(wayland_interface *) This,
		3,
		Width,
		Height,
		Format,
		Flags
	);
}

internal void
Wayland_ZwpLinuxDmabufFeedbackV1_Destroy(
	wayland_zwp_linux_dmabuf_feedback_v1 *This
)
{ Wayland_HandleRequest((wayland_interface *) This, 0); }

#endif

#undef M
#undef P

#endif
