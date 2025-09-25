/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                             *
*  Author: Aria Seiler                                                        *
*                                                                             *
*  This program is in the public domain. There is no implied warranty, so     *
*  use it at your own risk.                                                   *
*                                                                             *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef INCLUDE_HEADER

typedef u32 wayland_fixed;

typedef enum wayland_display_error		  wayland_display_error;
typedef enum wayland_shared_memory_error  wayland_shared_memory_error;
typedef enum wayland_shared_memory_format wayland_shared_memory_format;
typedef enum wayland_data_offer_error	  wayland_data_offer_error;
typedef enum wayland_data_source_error	  wayland_data_source_error;
typedef enum wayland_data_device_error	  wayland_data_device_error;
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

typedef struct wayland_array	wayland_array;
typedef union wayland_primitive wayland_primitive;
typedef struct wayland_message	wayland_message;

typedef struct wayland_interface		   wayland_interface;
typedef struct wayland_display			   wayland_display;
typedef struct wayland_registry			   wayland_registry;
typedef struct wayland_callback			   wayland_callback;
typedef struct wayland_compositor		   wayland_compositor;
typedef struct wayland_shared_memory_pool  wayland_shared_memory_pool;
typedef struct wayland_shared_memory	   wayland_shared_memory;
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

typedef enum wayland_object_type : u16 {
	WAYLAND_OBJECT_TYPE_DISPLAY,
	WAYLAND_OBJECT_TYPE_REGISTRY,
	WAYLAND_OBJECT_TYPE_CALLBACK,
	WAYLAND_OBJECT_TYPE_COMPOSITOR,
	WAYLAND_OBJECT_TYPE_SHARED_MEMORY_POOL,
	WAYLAND_OBJECT_TYPE_SHARED_MEMORY,
	WAYLAND_OBJECT_TYPE_BUFFER,
	WAYLAND_OBJECT_TYPE_DATA_OFFER,
	WAYLAND_OBJECT_TYPE_DATA_SOURCE,
	WAYLAND_OBJECT_TYPE_DATA_DEVICE,
	WAYLAND_OBJECT_TYPE_DATA_DEVICE_MANAGER,
	WAYLAND_OBJECT_TYPE_SHELL,
	WAYLAND_OBJECT_TYPE_SHELL_SURFACE,
	WAYLAND_OBJECT_TYPE_SURFACE,
	WAYLAND_OBJECT_TYPE_SEAT,
	WAYLAND_OBJECT_TYPE_POINTER,
	WAYLAND_OBJECT_TYPE_KEYBOARD,
	WAYLAND_OBJECT_TYPE_TOUCH,
	WAYLAND_OBJECT_TYPE_OUTPUT,
	WAYLAND_OBJECT_TYPE_REGION,
	WAYLAND_OBJECT_TYPE_SUBCOMPOSITOR,
	WAYLAND_OBJECT_TYPE_SUBSURFACE,
	WAYLAND_OBJECT_TYPE_FIXES,
} wayland_object_type;

enum wayland_display_error {
	WAYLAND_DISPLAY_ERROR_INVALID_OBJECT = 0,
	WAYLAND_DISPLAY_ERROR_INVALID_METHOD = 1,
	WAYLAND_DISPLAY_ERROR_NO_MEMORY		 = 2,
	WAYLAND_DISPLAY_ERROR_IMPLEMENTATION = 3,
};

enum wayland_shared_memory_error {
	WAYLAND_SHARED_MEMORY_ERROR_INVALID_FORMAT			= 0,
	WAYLAND_SHARED_MEMORY_ERROR_INVALID_STRIDE			= 1,
	WAYLAND_SHARED_MEMORY_ERROR_INVALID_FILE_DESCRIPTOR = 2,
};

enum wayland_shared_memory_format {
	// clang-format off
	WAYLAND_SHARED_MEMORY_FORMAT_ARGB8888             = 0x00000000, // - 32-bit ARGB format, [31:0] A:R:G:B 8:8:8:8 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_XRGB8888             = 0x00000001, // - 32-bit RGB format, [31:0] x:R:G:B 8:8:8:8 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_C8                   = 0x20203843, // - 8-bit color index format, [7:0] C
	WAYLAND_SHARED_MEMORY_FORMAT_RGB332               = 0x38424752, // - 8-bit RGB format, [7:0] R:G:B 3:3:2
	WAYLAND_SHARED_MEMORY_FORMAT_BGR233               = 0x38524742, // - 8-bit BGR format, [7:0] B:G:R 2:3:3
	WAYLAND_SHARED_MEMORY_FORMAT_XRGB4444             = 0x32315258, // - 16-bit xRGB format, [15:0] x:R:G:B 4:4:4:4 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_XBGR4444             = 0x32314258, // - 16-bit xBGR format, [15:0] x:B:G:R 4:4:4:4 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_RGBX4444             = 0x32315852, // - 16-bit RGBx format, [15:0] R:G:B:x 4:4:4:4 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_BGRX4444             = 0x32315842, // - 16-bit BGRx format, [15:0] B:G:R:x 4:4:4:4 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_ARGB4444             = 0x32315241, // - 16-bit ARGB format, [15:0] A:R:G:B 4:4:4:4 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_ABGR4444             = 0x32314241, // - 16-bit ABGR format, [15:0] A:B:G:R 4:4:4:4 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_RGBA4444             = 0x32314152, // - 16-bit RBGA format, [15:0] R:G:B:A 4:4:4:4 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_BGRA4444             = 0x32314142, // - 16-bit BGRA format, [15:0] B:G:R:A 4:4:4:4 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_XRGB1555             = 0x35315258, // - 16-bit xRGB format, [15:0] x:R:G:B 1:5:5:5 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_XBGR1555             = 0x35314258, // - 16-bit xBGR 1555 format, [15:0] x:B:G:R 1:5:5:5 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_RGBX5551             = 0x35315852, // - 16-bit RGBx 5551 format, [15:0] R:G:B:x 5:5:5:1 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_BGRX5551             = 0x35315842, // - 16-bit BGRx 5551 format, [15:0] B:G:R:x 5:5:5:1 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_ARGB1555             = 0x35315241, // - 16-bit ARGB 1555 format, [15:0] A:R:G:B 1:5:5:5 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_ABGR1555             = 0x35314241, // - 16-bit ABGR 1555 format, [15:0] A:B:G:R 1:5:5:5 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_RGBA5551             = 0x35314152, // - 16-bit RGBA 5551 format, [15:0] R:G:B:A 5:5:5:1 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_BGRA5551             = 0x35314142, // - 16-bit BGRA 5551 format, [15:0] B:G:R:A 5:5:5:1 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_RGB565               = 0x36314752, // - 16-bit RGB 565 format, [15:0] R:G:B 5:6:5 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_BGR565               = 0x36314742, // - 16-bit BGR 565 format, [15:0] B:G:R 5:6:5 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_RGB888               = 0x34324752, // - 24-bit RGB format, [23:0] R:G:B little endian
	WAYLAND_SHARED_MEMORY_FORMAT_BGR888               = 0x34324742, // - 24-bit BGR format, [23:0] B:G:R little endian
	WAYLAND_SHARED_MEMORY_FORMAT_XBGR8888             = 0x34324258, // - 32-bit xBGR format, [31:0] x:B:G:R 8:8:8:8 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_RGBX8888             = 0x34325852, // - 32-bit RGBx format, [31:0] R:G:B:x 8:8:8:8 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_BGRX8888             = 0x34325842, // - 32-bit BGRx format, [31:0] B:G:R:x 8:8:8:8 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_ABGR8888             = 0x34324241, // - 32-bit ABGR format, [31:0] A:B:G:R 8:8:8:8 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_RGBA8888             = 0x34324152, // - 32-bit RGBA format, [31:0] R:G:B:A 8:8:8:8 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_BGRA8888             = 0x34324142, // - 32-bit BGRA format, [31:0] B:G:R:A 8:8:8:8 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_XRGB2101010          = 0x30335258, // - 32-bit xRGB format, [31:0] x:R:G:B 2:10:10:10 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_XBGR2101010          = 0x30334258, // - 32-bit xBGR format, [31:0] x:B:G:R 2:10:10:10 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_RGBX1010102          = 0x30335852, // - 32-bit RGBx format, [31:0] R:G:B:x 10:10:10:2 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_BGRX1010102          = 0x30335842, // - 32-bit BGRx format, [31:0] B:G:R:x 10:10:10:2 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_ARGB2101010          = 0x30335241, // - 32-bit ARGB format, [31:0] A:R:G:B 2:10:10:10 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_ABGR2101010          = 0x30334241, // - 32-bit ABGR format, [31:0] A:B:G:R 2:10:10:10 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_RGBA1010102          = 0x30334152, // - 32-bit RGBA format, [31:0] R:G:B:A 10:10:10:2 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_BGRA1010102          = 0x30334142, // - 32-bit BGRA format, [31:0] B:G:R:A 10:10:10:2 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_YUYV                 = 0x56595559, // - packed YCbCr format, [31:0] Cr0:Y1:Cb0:Y0 8:8:8:8 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_YVYU                 = 0x55595659, // - packed YCbCr format, [31:0] Cb0:Y1:Cr0:Y0 8:8:8:8 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_UYVY                 = 0x59565955, // - packed YCbCr format, [31:0] Y1:Cr0:Y0:Cb0 8:8:8:8 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_VYUY                 = 0x59555956, // - packed YCbCr format, [31:0] Y1:Cb0:Y0:Cr0 8:8:8:8 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_AYUV                 = 0x56555941, // - packed AYCbCr format, [31:0] A:Y:Cb:Cr 8:8:8:8 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_NV12                 = 0x3231564e, // - 2 plane YCbCr Cr:Cb format, 2x2 subsampled Cr:Cb plane
	WAYLAND_SHARED_MEMORY_FORMAT_NV21                 = 0x3132564e, // - 2 plane YCbCr Cb:Cr format, 2x2 subsampled Cb:Cr plane
	WAYLAND_SHARED_MEMORY_FORMAT_NV16                 = 0x3631564e, // - 2 plane YCbCr Cr:Cb format, 2x1 subsampled Cr:Cb plane
	WAYLAND_SHARED_MEMORY_FORMAT_NV61                 = 0x3136564e, // - 2 plane YCbCr Cb:Cr format, 2x1 subsampled Cb:Cr plane
	WAYLAND_SHARED_MEMORY_FORMAT_YUV410               = 0x39565559, // - 3 plane YCbCr format, 4x4 subsampled Cb (1) and Cr (2) planes
	WAYLAND_SHARED_MEMORY_FORMAT_YVU410               = 0x39555659, // - 3 plane YCbCr format, 4x4 subsampled Cr (1) and Cb (2) planes
	WAYLAND_SHARED_MEMORY_FORMAT_YUV411               = 0x31315559, // - 3 plane YCbCr format, 4x1 subsampled Cb (1) and Cr (2) planes
	WAYLAND_SHARED_MEMORY_FORMAT_YVU411               = 0x31315659, // - 3 plane YCbCr format, 4x1 subsampled Cr (1) and Cb (2) planes
	WAYLAND_SHARED_MEMORY_FORMAT_YUV420               = 0x32315559, // - 3 plane YCbCr format, 2x2 subsampled Cb (1) and Cr (2) planes
	WAYLAND_SHARED_MEMORY_FORMAT_YVU420               = 0x32315659, // - 3 plane YCbCr format, 2x2 subsampled Cr (1) and Cb (2) planes
	WAYLAND_SHARED_MEMORY_FORMAT_YUV422               = 0x36315559, // - 3 plane YCbCr format, 2x1 subsampled Cb (1) and Cr (2) planes
	WAYLAND_SHARED_MEMORY_FORMAT_YVU422               = 0x36315659, // - 3 plane YCbCr format, 2x1 subsampled Cr (1) and Cb (2) planes
	WAYLAND_SHARED_MEMORY_FORMAT_YUV444               = 0x34325559, // - 3 plane YCbCr format, non-subsampled Cb (1) and Cr (2) planes
	WAYLAND_SHARED_MEMORY_FORMAT_YVU444               = 0x34325659, // - 3 plane YCbCr format, non-subsampled Cr (1) and Cb (2) planes
	WAYLAND_SHARED_MEMORY_FORMAT_R8                   = 0x20203852, // - [7:0] R
	WAYLAND_SHARED_MEMORY_FORMAT_R16                  = 0x20363152, // - [15:0] R little endian
	WAYLAND_SHARED_MEMORY_FORMAT_RG88                 = 0x38384752, // - [15:0] R:G 8:8 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_GR88                 = 0x38385247, // - [15:0] G:R 8:8 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_RG1616               = 0x32334752, // - [31:0] R:G 16:16 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_GR1616               = 0x32335247, // - [31:0] G:R 16:16 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_XRGB16161616F        = 0x48345258, // - [63:0] x:R:G:B 16:16:16:16 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_XBGR16161616F        = 0x48344258, // - [63:0] x:B:G:R 16:16:16:16 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_ARGB16161616F        = 0x48345241, // - [63:0] A:R:G:B 16:16:16:16 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_ABGR16161616F        = 0x48344241, // - [63:0] A:B:G:R 16:16:16:16 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_XYUV8888             = 0x56555958, // - [31:0] X:Y:Cb:Cr 8:8:8:8 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_VUY888               = 0x34325556, // - [23:0] Cr:Cb:Y 8:8:8 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_VUY101010            = 0x30335556, // - Y followed by U then V, 10:10:10. Non-linear modifier only
	WAYLAND_SHARED_MEMORY_FORMAT_Y210                 = 0x30313259, // - [63:0] Cr0:0:Y1:0:Cb0:0:Y0:0 10:6:10:6:10:6:10:6 little endian per 2 Y pixels
	WAYLAND_SHARED_MEMORY_FORMAT_Y212                 = 0x32313259, // - [63:0] Cr0:0:Y1:0:Cb0:0:Y0:0 12:4:12:4:12:4:12:4 little endian per 2 Y pixels
	WAYLAND_SHARED_MEMORY_FORMAT_Y216                 = 0x36313259, // - [63:0] Cr0:Y1:Cb0:Y0 16:16:16:16 little endian per 2 Y pixels
	WAYLAND_SHARED_MEMORY_FORMAT_Y410                 = 0x30313459, // - [31:0] A:Cr:Y:Cb 2:10:10:10 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_Y412                 = 0x32313459, // - [63:0] A:0:Cr:0:Y:0:Cb:0 12:4:12:4:12:4:12:4 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_Y416                 = 0x36313459, // - [63:0] A:Cr:Y:Cb 16:16:16:16 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_XVYU2101010          = 0x30335658, // - [31:0] X:Cr:Y:Cb 2:10:10:10 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_XVYU12_16161616      = 0x36335658, // - [63:0] X:0:Cr:0:Y:0:Cb:0 12:4:12:4:12:4:12:4 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_XVYU16161616         = 0x38345658, // - [63:0] X:Cr:Y:Cb 16:16:16:16 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_Y0L0                 = 0x304c3059, // - [63:0] A3:A2:Y3:0:Cr0:0:Y2:0:A1:A0:Y1:0:Cb0:0:Y0:0 1:1:8:2:8:2:8:2:1:1:8:2:8:2:8:2 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_X0L0                 = 0x304c3058, // - [63:0] X3:X2:Y3:0:Cr0:0:Y2:0:X1:X0:Y1:0:Cb0:0:Y0:0 1:1:8:2:8:2:8:2:1:1:8:2:8:2:8:2 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_Y0L2                 = 0x324c3059, // - [63:0] A3:A2:Y3:Cr0:Y2:A1:A0:Y1:Cb0:Y0 1:1:10:10:10:1:1:10:10:10 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_X0L2                 = 0x324c3058, // - [63:0] X3:X2:Y3:Cr0:Y2:X1:X0:Y1:Cb0:Y0 1:1:10:10:10:1:1:10:10:10 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_YUV420_8BIT          = 0x38305559, //
	WAYLAND_SHARED_MEMORY_FORMAT_YUV420_10BIT         = 0x30315559, //
	WAYLAND_SHARED_MEMORY_FORMAT_XRGB8888_A8          = 0x38415258, //
	WAYLAND_SHARED_MEMORY_FORMAT_XBGR8888_A8          = 0x38414258, //
	WAYLAND_SHARED_MEMORY_FORMAT_RGBX8888_A8          = 0x38415852, //
	WAYLAND_SHARED_MEMORY_FORMAT_BGRX8888_A8          = 0x38415842, //
	WAYLAND_SHARED_MEMORY_FORMAT_RGB888_A8            = 0x38413852, //
	WAYLAND_SHARED_MEMORY_FORMAT_BGR888_A8            = 0x38413842, //
	WAYLAND_SHARED_MEMORY_FORMAT_RGB565_A8            = 0x38413552, //
	WAYLAND_SHARED_MEMORY_FORMAT_BGR565_A8            = 0x38413542, //
	WAYLAND_SHARED_MEMORY_FORMAT_NV24                 = 0x3432564e, // - non-subsampled Cr:Cb plane
	WAYLAND_SHARED_MEMORY_FORMAT_NV42                 = 0x3234564e, // - non-subsampled Cb:Cr plane
	WAYLAND_SHARED_MEMORY_FORMAT_P210                 = 0x30313250, // - 2x1 subsampled Cr:Cb plane, 10 bit per channel
	WAYLAND_SHARED_MEMORY_FORMAT_P010                 = 0x30313050, // - 2x2 subsampled Cr:Cb plane 10 bits per channel
	WAYLAND_SHARED_MEMORY_FORMAT_P012                 = 0x32313050, // - 2x2 subsampled Cr:Cb plane 12 bits per channel
	WAYLAND_SHARED_MEMORY_FORMAT_P016                 = 0x36313050, // - 2x2 subsampled Cr:Cb plane 16 bits per channel
	WAYLAND_SHARED_MEMORY_FORMAT_AXBXGXRX106106106106 = 0x30314241, // - [63:0] A:x:B:x:G:x:R:x 10:6:10:6:10:6:10:6 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_NV15                 = 0x3531564e, // - 2x2 subsampled Cr:Cb plane
	WAYLAND_SHARED_MEMORY_FORMAT_Q410                 = 0x30313451, //
	WAYLAND_SHARED_MEMORY_FORMAT_Q401                 = 0x31303451, //
	WAYLAND_SHARED_MEMORY_FORMAT_XRGB16161616         = 0x38345258, // - [63:0] x:R:G:B 16:16:16:16 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_XBGR16161616         = 0x38344258, // - [63:0] x:B:G:R 16:16:16:16 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_ARGB16161616         = 0x38345241, // - [63:0] A:R:G:B 16:16:16:16 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_ABGR16161616         = 0x38344241, // - [63:0] A:B:G:R 16:16:16:16 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_C1                   = 0x20203143, // - [7:0] C0:C1:C2:C3:C4:C5:C6:C7 1:1:1:1:1:1:1:1 eight pixels/byte
	WAYLAND_SHARED_MEMORY_FORMAT_C2                   = 0x20203243, // - [7:0] C0:C1:C2:C3 2:2:2:2 four pixels/byte
	WAYLAND_SHARED_MEMORY_FORMAT_C4                   = 0x20203443, // - [7:0] C0:C1 4:4 two pixels/byte
	WAYLAND_SHARED_MEMORY_FORMAT_D1                   = 0x20203144, // - [7:0] D0:D1:D2:D3:D4:D5:D6:D7 1:1:1:1:1:1:1:1 eight pixels/byte
	WAYLAND_SHARED_MEMORY_FORMAT_D2                   = 0x20203244, // - [7:0] D0:D1:D2:D3 2:2:2:2 four pixels/byte
	WAYLAND_SHARED_MEMORY_FORMAT_D4                   = 0x20203444, // - [7:0] D0:D1 4:4 two pixels/byte
	WAYLAND_SHARED_MEMORY_FORMAT_D8                   = 0x20203844, // - [7:0] D
	WAYLAND_SHARED_MEMORY_FORMAT_R1                   = 0x20203152, // - [7:0] R0:R1:R2:R3:R4:R5:R6:R7 1:1:1:1:1:1:1:1 eight pixels/byte
	WAYLAND_SHARED_MEMORY_FORMAT_R2                   = 0x20203252, // - [7:0] R0:R1:R2:R3 2:2:2:2 four pixels/byte
	WAYLAND_SHARED_MEMORY_FORMAT_R4                   = 0x20203452, // - [7:0] R0:R1 4:4 two pixels/byte
	WAYLAND_SHARED_MEMORY_FORMAT_R10                  = 0x20303152, // - [15:0] x:R 6:10 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_R12                  = 0x20323152, // - [15:0] x:R 4:12 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_AVUY8888             = 0x59555641, // - [31:0] A:Cr:Cb:Y 8:8:8:8 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_XVUY8888             = 0x59555658, // - [31:0] X:Cr:Cb:Y 8:8:8:8 little endian
	WAYLAND_SHARED_MEMORY_FORMAT_P030                 = 0x30333050, // - 2x2 subsampled Cr:Cb plane 10 bits per channel packed
	// clang-format on
};

enum wayland_data_offer_error {
	WAYLAND_DATA_OFFER_ERROR_INVALID_FINISH		 = 0,
	WAYLAND_DATA_OFFER_ERROR_INVALID_ACTION_MASK = 1,
	WAYLAND_DATA_OFFER_ERROR_INVALID_ACTION		 = 2,
	WAYLAND_DATA_OFFER_ERROR_INVALID_OFFER		 = 3,
};

enum wayland_data_source_error {
	WAYLAND_DATA_SOURCE_ERROR_INVALID_ACTION_MASK = 0,
	WAYLAND_DATA_SOURCE_ERROR_INVALID_SOURCE	  = 1,
};

enum wayland_data_device_error {
	WAYLAND_DATA_DEVICE_ERROR_ROLE		  = 0,
	WAYLAND_DATA_DEVICE_ERROR_USED_SOURCE = 1,
};

enum wayland_data_device_manager_drag_and_drop_action {
	WAYLAND_DATA_DEVICE_MANAGER_DRAG_AND_DROP_ACTION_NONE = 0x0,
	WAYLAND_DATA_DEVICE_MANAGER_DRAG_AND_DROP_ACTION_COPY = 0x1,
	WAYLAND_DATA_DEVICE_MANAGER_DRAG_AND_DROP_ACTION_MOVE = 0x2,
	WAYLAND_DATA_DEVICE_MANAGER_DRAG_AND_DROP_ACTION_ASK  = 0x4,
};

enum wayland_shell_error {
	WAYLAND_SHELL_ERROR_ROLE = 0,
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

enum wayland_surface_error {
	WAYLAND_SURFACE_ERROR_INVALID_SCALE		  = 0,
	WAYLAND_SURFACE_ERROR_INVALID_TRANSFORM	  = 1,
	WAYLAND_SURFACE_ERROR_INVALID_SIZE		  = 2,
	WAYLAND_SURFACE_ERROR_INVALID_OFFSET	  = 3,
	WAYLAND_SURFACE_ERROR_DEFUNCT_ROLE_OBJECT = 4,
};

enum wayland_seat_capability {
	WAYLAND_SEAT_CAPABILITY_POINTER	 = 0x1,
	WAYLAND_SEAT_CAPABILITY_KEYBOARD = 0x2,
	WAYLAND_SEAT_CAPABILITY_TOUCH	 = 0x4,
};

enum wayland_seat_error {
	WAYLAND_SEAT_ERROR_MISSING_CAPABILITY = 0,
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

enum wayland_keyboard_keymap_format {
	WAYLAND_KEYBOARD_KEYMAP_FORMAT_NO_KEYMAP = 0,
	WAYLAND_KEYBOARD_KEYMAP_FORMAT_XKB_V1	 = 1,
};

enum wayland_keyboard_key_state {
	WAYLAND_KEYBOARD_KEY_STATE_RELEASED = 0,
	WAYLAND_KEYBOARD_KEY_STATE_PRESSED	= 1,
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

enum wayland_subcompositor_error {
	WAYLAND_SUBCOMPOSITOR_ERROR_BAD_SURFACE = 0,
	WAYLAND_SUBCOMPOSITOR_ERROR_BAD_PARENT	= 1,
};

enum wayland_subsurface_error {
	WAYLAND_SUBSURFACE_ERROR_BAD_SURFACE = 0,
};

struct wayland_array {
	u32	 Size;
	u32 *Data;
};

union wayland_primitive {
	s32			  Sint;
	u32			  Uint;
	string		  String;
	vptr		  Object;
	wayland_fixed Fixed;
	wayland_array Array;
};

struct wayland_message {
	u32 ObjectId;
	u16 Opcode;
	u16 Size;
	u32 Data[];
};

struct wayland_interface {
	wayland_object_type Type;
	u16					Size;
	u32					Id;

	void (*HandleEvent)(wayland_interface *This, wayland_message *Message);
};

struct wayland_display {
	wayland_interface Header;

	wayland_callback *(*Sync)(wayland_display *This);
	wayland_registry *(*GetRegistry)(wayland_display *This);

	void (*HandleError)(
		wayland_display		 *This,
		wayland_interface	 *Object,
		wayland_display_error Code,
		string				  Message
	);
	void (*HandleDeleteId)(wayland_display *This, u32 Id);
};

struct wayland_registry {
	wayland_interface Header;

	u32 (*Bind)(wayland_registry *This, u32 Name);

	void (*HandleGlobal)(
		wayland_registry *This,
		u32				  Name,
		string			  Interface,
		u32				  Version
	);
	void (*HandleGlobalRemove)(wayland_registry *This, u32 Name);
};

struct wayland_callback {
	wayland_interface Header;

	void (*HandleDone)(wayland_callback *This, u32 CallbackData);
};

struct wayland_compositor {
	wayland_interface Header;

	wayland_surface *(*CreateSurface)(wayland_compositor *This);
	wayland_region *(*CreateRegion)(wayland_compositor *This);
};

struct wayland_shared_memory_pool {
	wayland_interface Header;

	wayland_buffer *(*CreateBuffer)(
		wayland_shared_memory_pool	*This,
		s32							 Offset,
		s32							 Width,
		s32							 Height,
		s32							 Stride,
		wayland_shared_memory_format Format
	);
	void (*Destroy)(wayland_shared_memory_pool *This);
	void (*Resize)(wayland_shared_memory_pool *This, s32 Size);
};

struct wayland_shared_memory {
	wayland_interface Header;

	wayland_shared_memory_pool *(*CreatePool)(
		wayland_shared_memory *This,
		s32					   FileDescriptor,
		s32					   Size
	);
	void (*Release)(wayland_shared_memory *This);

	void (*HandleFormat)(
		wayland_shared_memory		*This,
		wayland_shared_memory_format Format
	);
};

struct wayland_buffer {
	wayland_interface Header;

	void (*Destroy)(wayland_buffer *This);

	void (*HandleRelease)(wayland_buffer *This);
};

struct wayland_data_offer {
	wayland_interface Header;

	void (*Accept)(wayland_data_offer *This, u32 Serial, string MimeType);
	void (*Receive)(
		wayland_data_offer *This,
		string				MimeType,
		s32					FileDescriptor
	);
	void (*Destroy)(wayland_data_offer *This);
	void (*Finish)(wayland_data_offer *This);
	void (*SetActions)(
		wayland_data_offer								*This,
		wayland_data_device_manager_drag_and_drop_action Supported,
		wayland_data_device_manager_drag_and_drop_action Preferred
	);

	void (*HandleOffer)(wayland_data_offer *This, string MimeType);
	void (*HandleSourceActions)(
		wayland_data_offer								*This,
		wayland_data_device_manager_drag_and_drop_action SourceActions
	);
	void (*HandleAction)(
		wayland_data_offer								*This,
		wayland_data_device_manager_drag_and_drop_action DragAndDropAction
	);
};

struct wayland_data_source {
	wayland_interface Header;

	void (*Offer)(wayland_data_source *This, string MimeType);
	void (*Destroy)(wayland_data_source *This);
	void (*SetActions)(
		wayland_data_source								*This,
		wayland_data_device_manager_drag_and_drop_action DragAndDropActions
	);

	void (*HandleTarget)(wayland_data_source *This, string MimeType);
	void (*HandleSend)(
		wayland_data_source *This,
		string				 MimeType,
		s32					 FileDescriptor
	);
	void (*HandleCancelled)(wayland_data_source *This);
	void (*HandleDragAndDropPerformed)(wayland_data_source *This);
	void (*HandleDragAndDropFinished)(wayland_data_source *This);
	void (*HandleAction)(
		wayland_data_source								*This,
		wayland_data_device_manager_drag_and_drop_action DragAndDropAction
	);
};

struct wayland_data_device {
	wayland_interface Header;

	void (*StartDrag)(
		wayland_data_device *This,
		wayland_data_source *Source,
		wayland_surface		*Origin,
		wayland_surface		*Icon,
		u32					 Serial
	);
	void (*SetSelection)(
		wayland_data_device *This,
		wayland_data_source *Source,
		u32					 Serial
	);
	void (*Release)(wayland_data_device *This);

	void (*HandleDataOffer)(wayland_data_device *This, u32 NewDataOfferId);
	void (*HandleEnter)(
		wayland_data_device *This,
		u32					 Serial,
		wayland_surface		*Surface,
		wayland_fixed		 X,
		wayland_fixed		 Y,
		wayland_data_offer	*DataOffer
	);
	void (*HandleLeave)(wayland_data_device *This);
	void (*HandleMotion)(
		wayland_data_device *This,
		u32					 Time,
		wayland_fixed		 X,
		wayland_fixed		 Y
	);
	void (*HandleDrop)(wayland_data_device *This);
	void (*HandleSelection)(
		wayland_data_device *This,
		wayland_data_offer	*DataOffer
	);
};

struct wayland_data_device_manager {
	wayland_interface Header;

	wayland_data_source *(*CreateDataSource)(wayland_data_device_manager *This);
	wayland_data_device *(*GetDataDevice)(
		wayland_data_device_manager *This,
		wayland_seat				*Seat
	);
};

struct wayland_shell {
	wayland_interface Header;

	wayland_shell_surface
		*(*GetShellSurface)(wayland_shell *This, wayland_surface *Surface);
};

struct wayland_shell_surface {
	wayland_interface Header;

	void (*Pong)(wayland_shell_surface *This, u32 Serial);
	void (*Move)(wayland_shell_surface *This, wayland_seat *Seat, u32 Serial);
	void (*Resize)(
		wayland_shell_surface		*This,
		wayland_seat				*Seat,
		u32							 Serial,
		wayland_shell_surface_resize Edges
	);
	void (*SetToplevel)(wayland_shell_surface *This);
	void (*SetTransient)(
		wayland_shell_surface		   *This,
		wayland_surface				   *Parent,
		s32								X,
		s32								Y,
		wayland_shell_surface_transient Flags
	);
	void (*SetFullscreen)(
		wayland_shell_surface				   *This,
		wayland_shell_surface_fullscreen_method Method,
		u32										Framerate,
		wayland_output						   *Output
	);
	void (*SetPopup)(
		wayland_shell_surface			*This,
		wayland_seat					*Seat,
		u32								 Serial,
		wayland_surface					*Parent,
		s32								 X,
		s32								 Y,
		wayland_shell_surface_transient *Flags
	);
	void (*SetMaximized)(wayland_shell_surface *This, wayland_output *Output);
	void (*SetTitle)(wayland_shell_surface *This, string Title);
	void (*SetClass)(wayland_shell_surface *This, string Class);

	void (*HandlePing)(wayland_shell_surface *This, u32 Serial);
	void (*HandleConfigure)(
		wayland_shell_surface		*This,
		wayland_shell_surface_resize Edges,
		s32							 Width,
		s32							 Height
	);
	void (*HandlePopupDone)(wayland_shell_surface *This);
};

struct wayland_surface {
	wayland_interface Header;

	void (*Destroy)(wayland_surface *This);
	void (*Attach)(wayland_surface *This, wayland_buffer *Buffer, s32 X, s32 Y);
	void (*Damage)(wayland_surface *This, s32 X, s32 Y, s32 Width, s32 Height);
	wayland_callback *(*Frame)(wayland_surface *This);
	void (*SetOpaqueRegion)(wayland_surface *This, wayland_region *Region);
	void (*SetInputRegion)(wayland_surface *This, wayland_region *Region);
	void (*Commit)(wayland_surface *This);
	void (*SetBufferTransform)(
		wayland_surface			*This,
		wayland_output_transform Transform
	);
	void (*SetBufferScale)(wayland_surface *This, s32 Scale);
	void (*DamageBuffer)(
		wayland_surface *This,
		s32				 X,
		s32				 Y,
		s32				 Width,
		s32				 Height
	);
	void (*Offset)(wayland_surface *This, s32 X, s32 Y);

	void (*HandleEnter)(wayland_surface *This, wayland_output *Output);
	void (*HandleLeave)(wayland_surface *This, wayland_output *Output);
	void (*HandlePreferredBufferScale)(wayland_surface *This, s32 Factor);
	void (*HandlePreferredBufferTransform)(
		wayland_surface			*This,
		wayland_output_transform Transform
	);
};

struct wayland_seat {
	wayland_interface Header;

	wayland_pointer *(*GetPointer)(wayland_seat *This);
	wayland_keyboard *(*GetKeyboard)(wayland_seat *This);
	wayland_touch *(*GetTouch)(wayland_seat *This);
	void (*Release)(wayland_seat *This);

	void (*HandleCapabilities)(
		wayland_seat		   *This,
		wayland_seat_capability Capabilities
	);
	void (*HandleName)(wayland_seat *This, string Name);
};

struct wayland_pointer {
	wayland_interface Header;

	void (*SetCursor)(
		wayland_pointer *This,
		u32				 Serial,
		wayland_surface *Surface,
		s32				 HotspotX,
		s32				 HotspotY
	);
	void (*Release)(wayland_pointer *This);

	void (*HandleEnter)(
		wayland_pointer *This,
		u32				 Serial,
		wayland_surface *Surface,
		wayland_fixed	 SurfaceX,
		wayland_fixed	 SurfaceY
	);
	void (*HandleLeave)(
		wayland_pointer *This,
		u32				 Serial,
		wayland_surface *Surface
	);
	void (*HandleMotion)(
		wayland_pointer *This,
		u32				 Time,
		wayland_fixed	 SurfaceX,
		wayland_fixed	 SurfaceY
	);
	void (*HandleButton)(
		wayland_pointer				*This,
		u32							 Serial,
		u32							 Time,
		u32							 Button,
		wayland_pointer_button_state State
	);
	void (*HandleAxis)(
		wayland_pointer		*This,
		u32					 Time,
		wayland_pointer_axis Axis,
		wayland_fixed		 Value
	);
	void (*HandleFrame)(wayland_pointer *This);
	void (*HandleAxisSource)(
		wayland_pointer			   *This,
		wayland_pointer_axis_source AxisSource
	);
	void (*HandleAxisStop)(
		wayland_pointer		*This,
		u32					 Time,
		wayland_pointer_axis AxisStop
	);
	void (*HandleAxisDiscrete)(
		wayland_pointer		*This,
		wayland_pointer_axis Axis,
		s32					 Discrete
	);
	void (*HandleAxisValue120)(
		wayland_pointer		*This,
		wayland_pointer_axis Axis,
		s32					 Value120
	);
	void (*HandleAxisRelativeDirection)(
		wayland_pointer						   *This,
		wayland_pointer_axis					Axis,
		wayland_pointer_axis_relative_direction Direction
	);
};

struct wayland_keyboard {
	wayland_interface Header;

	void (*Release)(wayland_keyboard *This);

	void (*HandleKeymap)(wayland_keyboard *This, s32 FileDescriptor, u32 Size);
	void (*HandleEnter)(
		wayland_keyboard *This,
		u32				  Serial,
		wayland_surface	 *Surface,
		wayland_array	  Keys
	);
	void (*HandleLeave)(
		wayland_keyboard *This,
		u32				  Serial,
		wayland_surface	 *Surface
	);
	void (*HandleKey)(
		wayland_keyboard		  *This,
		u32						   Serial,
		u32						   Time,
		u32						   Key,
		wayland_keyboard_key_state State
	);
	void (*HandleModifiers)(
		wayland_keyboard *This,
		u32				  Serial,
		u32				  Depressed,
		u32				  Latched,
		u32				  Locked,
		u32				  Group
	);
	void (*HandleRepeatInfo)(wayland_keyboard *This, s32 Rate, s32 Delay);
};

struct wayland_touch {
	wayland_interface Header;

	void (*Release)(wayland_touch *This);

	void (*HandleDown)(
		wayland_touch	*This,
		u32				 Serial,
		u32				 Time,
		wayland_surface *Surface,
		s32				 Id,
		wayland_fixed	 X,
		wayland_fixed	 Y
	);
	void (*HandleUp)(wayland_touch *This, u32 Serial, u32 Time, s32 Id);
	void (*HandleMotion)(
		wayland_touch *This,
		u32			   Motion,
		s32			   Id,
		wayland_fixed  X,
		wayland_fixed  Y
	);
	void (*HandleFrame)(wayland_touch *This);
	void (*HandleCancel)(wayland_touch *This);
	void (*HandleShape)(
		wayland_touch *This,
		s32			   Id,
		wayland_fixed  Major,
		wayland_fixed  Minor
	);
	void (*HandleOrientation)(
		wayland_touch *This,
		s32			   Id,
		wayland_fixed  Orientation
	);
};

struct wayland_output {
	wayland_interface Header;

	void (*Release)(wayland_output *This);

	void (*HandleGeometry)(
		wayland_output			*This,
		s32						 X,
		s32						 Y,
		s32						 PhysicalWidth,
		s32						 PhysicalHeight,
		wayland_output_subpixel	 Subpixel,
		string					 Make,
		string					 Model,
		wayland_output_transform Transform
	);
	void (*HandleMode)(
		wayland_output	   *This,
		wayland_output_mode Flags,
		s32					Width,
		s32					Height,
		s32					Refresh
	);
	void (*HandleDone)(wayland_output *This);
	void (*HandleScale)(wayland_output *This, s32 Factor);
	void (*HandleName)(wayland_output *This, string Name);
	void (*HandleDescription)(wayland_output *This, string Description);
};

struct wayland_region {
	wayland_interface Header;

	void (*Destroy)(wayland_region *This);
	void (*Add)(wayland_region *This, s32 X, s32 Y, s32 Width, s32 Height);
	void (*Subtract)(wayland_region *This, s32 X, s32 Y, s32 Width, s32 Height);
};

struct wayland_subcompositor {
	wayland_interface Header;

	void (*Destroy)(wayland_subcompositor *This);
	wayland_subsurface *(*GetSubsurface)(
		wayland_subcompositor *This,
		wayland_surface		  *Surface,
		wayland_surface		  *Parent
	);
};

struct wayland_subsurface {
	wayland_interface Header;

	void (*Destroy)(wayland_subsurface *This);
	void (*SetPosition)(wayland_subsurface *This, s32 X, s32 Y);
	void (*PlaceAbove)(wayland_subsurface *This, wayland_surface *Sibling);
	void (*PlaceBelow)(wayland_subsurface *This, wayland_surface *Sibling);
	void (*SetSync)(wayland_subsurface *This);
	void (*SetDesync)(wayland_subsurface *This);
};

struct wayland_fixes {
	wayland_interface Header;

	void (*Destroy)(wayland_fixes *This);
	void (*DestroyRegistry)(wayland_fixes *This, wayland_registry *Registry);
};

#define WAYLAND_API_FUNCS \
	INTERN(void, Wayland_Display_HandleEvent, wayland_display *This, wayland_message *Message) \
	INTERN(wayland_callback*, Wayland_Display_Sync, wayland_display *This) \
	INTERN(wayland_registry*, Wayland_Display_GetRegistry, wayland_display *This) \
	INTERN(void, Wayland_Registry_HandleEvent, wayland_registry *This, wayland_message *Message) \
	INTERN(u32, Wayland_Registry_Bind, wayland_registry *This, u32 Name) \
	INTERN(void, Wayland_Callback_HandleEvent, wayland_callback *This, wayland_message *Message)

#endif

#ifdef _LINUX
#ifdef INCLUDE_SOURCE

static wayland_display WaylandDisplayPrototype = {
	.Header			= { .Type		 = WAYLAND_OBJECT_TYPE_DISPLAY,
						.Size		 = sizeof(wayland_display),
						.Id			 = 0,
						.HandleEvent = (vptr) Wayland_Display_HandleEvent },
	.Sync			= Wayland_Display_Sync,
	.GetRegistry	= Wayland_Display_GetRegistry,
	.HandleError	= NULL,
	.HandleDeleteId = NULL,
};

static wayland_registry WaylandRegistryPrototype = {
	.Header				= { .Type		 = WAYLAND_OBJECT_TYPE_REGISTRY,
							.Size		 = sizeof(wayland_registry),
							.Id			 = 0,
							.HandleEvent = (vptr) Wayland_Registry_HandleEvent },
	.Bind				= Wayland_Registry_Bind,
	.HandleGlobal		= NULL,
	.HandleGlobalRemove = NULL,
};

static wayland_callback WaylandCallbackPrototype = {
	.Header		= { .Type		 = WAYLAND_OBJECT_TYPE_CALLBACK,
					.Size		 = sizeof(wayland_callback),
					.Id			 = 0,
					.HandleEvent = (vptr) Wayland_Callback_HandleEvent },
	.HandleDone = NULL,
};

static wayland_compositor WaylandCompositorPrototype = {
	.Header		   = { .Type		= WAYLAND_OBJECT_TYPE_COMPOSITOR,
					   .Size		= sizeof(wayland_compositor),
					   .Id			= 0,
					   .HandleEvent = NULL },
	.CreateSurface = NULL,
	.CreateRegion  = NULL,
};

static wayland_shared_memory_pool WaylandSharedMemoryPoolPrototype = {
	.Header		  = { .Type		   = WAYLAND_OBJECT_TYPE_SHARED_MEMORY_POOL,
					  .Size		   = sizeof(wayland_shared_memory_pool),
					  .Id		   = 0,
					  .HandleEvent = NULL },
	.CreateBuffer = NULL,
	.Destroy	  = NULL,
	.Resize		  = NULL,
};

static wayland_shared_memory WaylandSharedMemoryPrototype = {
	.Header		  = { .Type		   = WAYLAND_OBJECT_TYPE_SHARED_MEMORY,
					  .Size		   = sizeof(wayland_shared_memory),
					  .Id		   = 0,
					  .HandleEvent = NULL },
	.CreatePool	  = NULL,
	.Release	  = NULL,
	.HandleFormat = NULL,
};

static wayland_buffer WaylandBufferPrototype = {
	.Header		   = { .Type		= WAYLAND_OBJECT_TYPE_BUFFER,
					   .Size		= sizeof(wayland_buffer),
					   .Id			= 0,
					   .HandleEvent = NULL },
	.Destroy	   = NULL,
	.HandleRelease = NULL,
};

static wayland_data_offer WaylandDataOfferPrototype = {
	.Header				 = { .Type		  = WAYLAND_OBJECT_TYPE_DATA_OFFER,
							 .Size		  = sizeof(wayland_data_offer),
							 .Id		  = 0,
							 .HandleEvent = NULL },
	.Accept				 = NULL,
	.Receive			 = NULL,
	.Destroy			 = NULL,
	.Finish				 = NULL,
	.SetActions			 = NULL,
	.HandleOffer		 = NULL,
	.HandleSourceActions = NULL,
	.HandleAction		 = NULL,
};

static wayland_data_source WaylandDataSourcePrototype = {
	.Header						= { .Type		 = WAYLAND_OBJECT_TYPE_DATA_SOURCE,
									.Size		 = sizeof(wayland_data_source),
									.Id			 = 0,
									.HandleEvent = NULL },
	.Offer						= NULL,
	.Destroy					= NULL,
	.SetActions					= NULL,
	.HandleTarget				= NULL,
	.HandleSend					= NULL,
	.HandleCancelled			= NULL,
	.HandleDragAndDropPerformed = NULL,
	.HandleDragAndDropFinished	= NULL,
	.HandleAction				= NULL,
};

static wayland_data_device WaylandDataDevicePrototype = {
	.Header			 = { .Type		  = WAYLAND_OBJECT_TYPE_DATA_DEVICE,
						 .Size		  = sizeof(wayland_data_device),
						 .Id		  = 0,
						 .HandleEvent = NULL },
	.StartDrag		 = NULL,
	.SetSelection	 = NULL,
	.Release		 = NULL,
	.HandleDataOffer = NULL,
	.HandleEnter	 = NULL,
	.HandleLeave	 = NULL,
	.HandleMotion	 = NULL,
	.HandleDrop		 = NULL,
	.HandleSelection = NULL,
};

static wayland_data_device_manager WaylandDataDeviceManagerPrototype = {
	.Header			  = { .Type		   = WAYLAND_OBJECT_TYPE_DATA_DEVICE_MANAGER,
						  .Size		   = sizeof(wayland_data_device_manager),
						  .Id		   = 0,
						  .HandleEvent = NULL },
	.CreateDataSource = NULL,
	.GetDataDevice	  = NULL,
};

static wayland_shell WaylandShellPrototype = {
	.Header			 = { .Type		  = WAYLAND_OBJECT_TYPE_SHELL,
						 .Size		  = sizeof(wayland_shell),
						 .Id		  = 0,
						 .HandleEvent = NULL },
	.GetShellSurface = NULL,
};

static wayland_shell_surface WaylandShellSurfacePrototype = {
	.Header			 = { .Type		  = WAYLAND_OBJECT_TYPE_SHELL_SURFACE,
						 .Size		  = sizeof(wayland_shell_surface),
						 .Id		  = 0,
						 .HandleEvent = NULL },
	.Pong			 = NULL,
	.Move			 = NULL,
	.Resize			 = NULL,
	.SetToplevel	 = NULL,
	.SetTransient	 = NULL,
	.SetFullscreen	 = NULL,
	.SetPopup		 = NULL,
	.SetMaximized	 = NULL,
	.SetTitle		 = NULL,
	.SetClass		 = NULL,
	.HandlePing		 = NULL,
	.HandleConfigure = NULL,
	.HandlePopupDone = NULL,
};

static wayland_surface WaylandSurfacePrototype = {
	.Header							= { .Type		 = WAYLAND_OBJECT_TYPE_SURFACE,
										.Size		 = sizeof(wayland_surface),
										.Id			 = 0,
										.HandleEvent = NULL },
	.Destroy						= NULL,
	.Attach							= NULL,
	.Damage							= NULL,
	.Frame							= NULL,
	.SetOpaqueRegion				= NULL,
	.SetInputRegion					= NULL,
	.Commit							= NULL,
	.SetBufferTransform				= NULL,
	.SetBufferScale					= NULL,
	.DamageBuffer					= NULL,
	.Offset							= NULL,
	.HandleEnter					= NULL,
	.HandleLeave					= NULL,
	.HandlePreferredBufferScale		= NULL,
	.HandlePreferredBufferTransform = NULL,
};

static wayland_seat WaylandSeatPrototype = {
	.Header				= { .Type		 = WAYLAND_OBJECT_TYPE_SEAT,
							.Size		 = sizeof(wayland_seat),
							.Id			 = 0,
							.HandleEvent = NULL },
	.GetPointer			= NULL,
	.GetKeyboard		= NULL,
	.GetTouch			= NULL,
	.Release			= NULL,
	.HandleCapabilities = NULL,
	.HandleName			= NULL,
};

static wayland_pointer WaylandPointerPrototype = {
	.Header						 = { .Type		  = WAYLAND_OBJECT_TYPE_POINTER,
									 .Size		  = sizeof(wayland_pointer),
									 .Id		  = 0,
									 .HandleEvent = NULL },
	.SetCursor					 = NULL,
	.Release					 = NULL,
	.HandleEnter				 = NULL,
	.HandleLeave				 = NULL,
	.HandleMotion				 = NULL,
	.HandleButton				 = NULL,
	.HandleAxis					 = NULL,
	.HandleFrame				 = NULL,
	.HandleAxisSource			 = NULL,
	.HandleAxisStop				 = NULL,
	.HandleAxisDiscrete			 = NULL,
	.HandleAxisValue120			 = NULL,
	.HandleAxisRelativeDirection = NULL,
};

static wayland_keyboard WaylandKeyboardPrototype = {
	.Header			  = { .Type		   = WAYLAND_OBJECT_TYPE_KEYBOARD,
						  .Size		   = sizeof(wayland_keyboard),
						  .Id		   = 0,
						  .HandleEvent = NULL },
	.Release		  = NULL,
	.HandleKeymap	  = NULL,
	.HandleEnter	  = NULL,
	.HandleLeave	  = NULL,
	.HandleKey		  = NULL,
	.HandleModifiers  = NULL,
	.HandleRepeatInfo = NULL,
};

static wayland_touch WaylandTouchPrototype = {
	.Header			   = { .Type		= WAYLAND_OBJECT_TYPE_TOUCH,
						   .Size		= sizeof(wayland_touch),
						   .Id			= 0,
						   .HandleEvent = NULL },
	.Release		   = NULL,
	.HandleDown		   = NULL,
	.HandleUp		   = NULL,
	.HandleMotion	   = NULL,
	.HandleFrame	   = NULL,
	.HandleCancel	   = NULL,
	.HandleShape	   = NULL,
	.HandleOrientation = NULL,
};

static wayland_output WaylandOutputPrototype = {
	.Header			   = { .Type		= WAYLAND_OBJECT_TYPE_OUTPUT,
						   .Size		= sizeof(wayland_output),
						   .Id			= 0,
						   .HandleEvent = NULL },
	.Release		   = NULL,
	.HandleGeometry	   = NULL,
	.HandleMode		   = NULL,
	.HandleDone		   = NULL,
	.HandleScale	   = NULL,
	.HandleName		   = NULL,
	.HandleDescription = NULL,
};

static wayland_region WaylandRegionPrototype = {
	.Header	  = { .Type		   = WAYLAND_OBJECT_TYPE_REGION,
				  .Size		   = sizeof(wayland_region),
				  .Id		   = 0,
				  .HandleEvent = NULL },
	.Destroy  = NULL,
	.Add	  = NULL,
	.Subtract = NULL,
};

static wayland_subcompositor WaylandSubcompositorPrototype = {
	.Header		   = { .Type		= WAYLAND_OBJECT_TYPE_SUBCOMPOSITOR,
					   .Size		= sizeof(wayland_subcompositor),
					   .Id			= 0,
					   .HandleEvent = NULL },
	.Destroy	   = NULL,
	.GetSubsurface = NULL,
};

static wayland_subsurface WaylandSubsurfacePrototype = {
	.Header		 = { .Type		  = WAYLAND_OBJECT_TYPE_SUBSURFACE,
					 .Size		  = sizeof(wayland_subsurface),
					 .Id		  = 0,
					 .HandleEvent = NULL },
	.Destroy	 = NULL,
	.SetPosition = NULL,
	.PlaceAbove	 = NULL,
	.PlaceBelow	 = NULL,
	.SetSync	 = NULL,
	.SetDesync	 = NULL,
};

static wayland_fixes WaylandFixesPrototype = {
	.Header			 = { .Type		  = WAYLAND_OBJECT_TYPE_FIXES,
						 .Size		  = sizeof(wayland_fixes),
						 .Id		  = 0,
						 .HandleEvent = NULL },
	.Destroy		 = NULL,
	.DestroyRegistry = NULL,
};

static wayland_interface *WaylandPrototypes[] = {
	[WAYLAND_OBJECT_TYPE_DISPLAY] =
		(wayland_interface *) &WaylandDisplayPrototype,
	[WAYLAND_OBJECT_TYPE_REGISTRY] =
		(wayland_interface *) &WaylandRegistryPrototype,
	[WAYLAND_OBJECT_TYPE_CALLBACK] =
		(wayland_interface *) &WaylandCallbackPrototype,
	[WAYLAND_OBJECT_TYPE_COMPOSITOR] =
		(wayland_interface *) &WaylandCompositorPrototype,
	[WAYLAND_OBJECT_TYPE_SHARED_MEMORY_POOL] =
		(wayland_interface *) &WaylandSharedMemoryPoolPrototype,
	[WAYLAND_OBJECT_TYPE_SHARED_MEMORY] =
		(wayland_interface *) &WaylandSharedMemoryPrototype,
	[WAYLAND_OBJECT_TYPE_BUFFER] =
		(wayland_interface *) &WaylandBufferPrototype,
	[WAYLAND_OBJECT_TYPE_DATA_OFFER] =
		(wayland_interface *) &WaylandDataOfferPrototype,
	[WAYLAND_OBJECT_TYPE_DATA_SOURCE] =
		(wayland_interface *) &WaylandDataSourcePrototype,
	[WAYLAND_OBJECT_TYPE_DATA_DEVICE] =
		(wayland_interface *) &WaylandDataDevicePrototype,
	[WAYLAND_OBJECT_TYPE_DATA_DEVICE_MANAGER] =
		(wayland_interface *) &WaylandDataDeviceManagerPrototype,
	[WAYLAND_OBJECT_TYPE_SHELL] = (wayland_interface *) &WaylandShellPrototype,
	[WAYLAND_OBJECT_TYPE_SHELL_SURFACE] =
		(wayland_interface *) &WaylandShellSurfacePrototype,
	[WAYLAND_OBJECT_TYPE_SURFACE] =
		(wayland_interface *) &WaylandSurfacePrototype,
	[WAYLAND_OBJECT_TYPE_SEAT] = (wayland_interface *) &WaylandSeatPrototype,
	[WAYLAND_OBJECT_TYPE_POINTER] =
		(wayland_interface *) &WaylandPointerPrototype,
	[WAYLAND_OBJECT_TYPE_KEYBOARD] =
		(wayland_interface *) &WaylandKeyboardPrototype,
	[WAYLAND_OBJECT_TYPE_TOUCH] = (wayland_interface *) &WaylandTouchPrototype,
	[WAYLAND_OBJECT_TYPE_OUTPUT] =
		(wayland_interface *) &WaylandOutputPrototype,
	[WAYLAND_OBJECT_TYPE_REGION] =
		(wayland_interface *) &WaylandRegionPrototype,
	[WAYLAND_OBJECT_TYPE_SUBCOMPOSITOR] =
		(wayland_interface *) &WaylandSubcompositorPrototype,
	[WAYLAND_OBJECT_TYPE_SUBSURFACE] =
		(wayland_interface *) &WaylandSubsurfacePrototype,
	[WAYLAND_OBJECT_TYPE_FIXES] = (wayland_interface *) &WaylandFixesPrototype,
};

internal u32
Wayland_AllocateObjectId(void)
{
	return _G.NextObjectId++;
}

internal vptr
Wayland_CreateObject(wayland_object_type Type)
{
	wayland_interface *Proto  = WaylandPrototypes[Type];
	wayland_interface *Object = Heap_AllocateA(_G.Heap, Proto->Size);
	Mem_Cpy(Object, Proto, Proto->Size);
	Object->Id = Wayland_AllocateObjectId();
	HashMap_Add(&_G.IdTable, &Object->Id, &Object);
	return Object;
}

internal void
Wayland_DeleteObject(vptr Object)
{
	wayland_interface *Obj = Object;
	HashMap_Remove(&_G.IdTable, &Obj->Id, NULL);
	Mem_Set(Object, 0, Obj->Size);
	Heap_FreeA(Object);
}

internal b08
Wayland_ConnectToServerSocket(file_handle *Socket)
{
	s32	   FileDescriptor;
	string WaylandSocket = Platform_GetEnvParam(CStringL("WAYLAND_SOCKET"));
	if (String_TryParseS32(WaylandSocket, &FileDescriptor)) {
#ifdef _LINUX
		Socket->FileDescriptor = FileDescriptor;
#endif
		return TRUE;
	}

	string XdgRuntimeDir  = Platform_GetEnvParam(CStringL("XDG_RUNTIME_DIR"));
	string WaylandDisplay = Platform_GetEnvParam(CStringL("WAYLAND_DISPLAY"));

	Stack_Push();
	b08 Connected = TRUE;

	if (WaylandDisplay.Length) {
		string SocketPath = FStringL("%s/%s", XdgRuntimeDir, WaylandDisplay);

		if (!Platform_ConnectToLocalSocket(SocketPath, Socket)) {
			SocketPath = FStringL("%s/wayland-0", XdgRuntimeDir);
			if (!Platform_ConnectToLocalSocket(SocketPath, Socket))
				Connected = FALSE;
		}
	}

	Stack_Pop();
	return Connected;
}

internal void
Wayland_SendWord(u32 Word)
{
	usize BytesWritten =
		Platform_WriteFile(_G.Socket, &Word, sizeof(u32), (usize) -1);
	Assert(BytesWritten = sizeof(u32));
}

internal void
Wayland_SendMessage(u32 ObjectId, u16 Opcode, u16 WordCount, ...)
{
	Wayland_SendWord(ObjectId);
	Wayland_SendWord(((2 + WordCount) * sizeof(u32)) << 16 | Opcode);

	va_list Args;
	VA_Start(Args, WordCount);
	for (usize I = 0; I < WordCount; I++) Wayland_SendWord(VA_Next(Args, u32));
	VA_End(Args);
}

internal wayland_message *
Wayland_ReadMessage(void)
{
	u32	  Header[2];
	usize BytesRead = Platform_ReadFile(
		_G.Socket,
		(vptr) Header,
		sizeof(wayland_message),
		(usize) -1
	);
	Assert(BytesRead == sizeof(wayland_message));

	u32 ObjectId = Header[0];
	u16 Size	 = Header[1] >> 16;
	u16 Opcode	 = Header[1] & 0xFFFF;

	wayland_message *Message  = Stack_Allocate(Size);
	Message->ObjectId		  = ObjectId;
	Message->Opcode			  = Opcode;
	Message->Size			  = Size;
	BytesRead				 += Platform_ReadFile(
		   _G.Socket,
		   Message->Data,
		   Size - sizeof(wayland_message),
		   (usize) -1
	   );
	Assert(BytesRead == Size);

	return Message;
}

internal vptr
Wayland_HandleNextEvent(void)
{
	Stack_Push();
	wayland_message *Message = Wayland_ReadMessage();

	wayland_interface *Object;
	HashMap_Get(&_G.IdTable, &Message->ObjectId, &Object);
	if (!Object) return NULL;
	if (Object->HandleEvent) Object->HandleEvent(Object, Message);

	Stack_Pop();
	return Object;
}

#define MAC_FOR_FUNC_MAKE_PARSE_PRIM(NAME, ARG, ITER) \
	MAC_CONCAT(Arg, ITER) = MAC_CONCAT(Wayland_Parse, ARG)(NAME, &I)
#define MAC_FOR_FUNC_MAKE_PARSE_ARG(NAME, ARG, ITER) \
	MAC_CONCAT(Arg, ITER).ARG
#define TRYCALL(Handler, Name, Message, ...) \
	if (Handler->Name) { \
		usize I = 0; \
		__VA_OPT__(wayland_primitive MAC_FOREACH(Message, MAC_FOR_OP_SEQ, MAC_FOR_FUNC_MAKE_PARSE_PRIM, __VA_ARGS__);) \
		(Handler->Name)(Handler __VA_OPT__(, MAC_FOREACH(Message, MAC_FOR_OP_SEQ, MAC_FOR_FUNC_MAKE_PARSE_ARG, __VA_ARGS__))); \
	}

// internal wayland_primitive
// Wayland_ParseSint(wayland_message *Message, usize *I)
// {
// 	wayland_primitive Prim;
//
// 	Prim.Sint  = (s32) Message->Data[*I];
// 	*I		  += 1;
//
// 	return Prim;
// }

internal wayland_primitive
Wayland_ParseUint(wayland_message *Message, usize *I)
{
	wayland_primitive Prim;

	Prim.Uint  = Message->Data[*I];
	*I		  += 1;

	return Prim;
}

internal wayland_primitive
Wayland_ParseString(wayland_message *Message, usize *I)
{
	wayland_primitive Prim;

	u32 Length	= Message->Data[*I];
	*I		   += 1;

	Prim.String =
		CLEString((c08 *) &Message->Data[*I], Length, STRING_ENCODING_ASCII);
	*I += (Prim.String.Length + 3) / 4;

	return Prim;
}

// internal wayland_primitive
// Wayland_ParseArray(wayland_message *Message, usize *I)
// {
// 	wayland_primitive Prim;
//
// 	Prim.Array.Size	 = Message->Data[*I];
// 	*I				+= 1;
//
// 	Prim.Array.Data	 = &Message->Data[*I];
// 	*I				+= (Prim.Array.Size + 3) / 4;
//
// 	return Prim;
// }

internal wayland_primitive
Wayland_ParseObject(wayland_message *Message, usize *I)
{
	wayland_primitive Prim;

	u32 ObjectId  = Message->Data[*I];
	*I			 += 1;

	if (!HashMap_Get(&_G.IdTable, &ObjectId, &Prim.Object)) Prim.Object = NULL;

	return Prim;
}

internal void
Wayland_Display_HandleEvent(wayland_display *This, wayland_message *Message)
{
	switch (Message->Opcode) {
		case 0:
			TRYCALL(This, HandleError, Message, Object, Uint, String);
			break;
		case 1: TRYCALL(This, HandleDeleteId, Message, Uint); break;
	}
}

internal wayland_callback *
Wayland_Display_Sync(wayland_display *This)
{
	wayland_callback *Callback =
		Wayland_CreateObject(WAYLAND_OBJECT_TYPE_CALLBACK);
	Wayland_SendMessage(This->Header.Id, 0, 1, Callback->Header.Id);
	return Callback;
}

internal wayland_registry *
Wayland_Display_GetRegistry(wayland_display *This)
{
	wayland_registry *Registry =
		Wayland_CreateObject(WAYLAND_OBJECT_TYPE_REGISTRY);
	Wayland_SendMessage(This->Header.Id, 1, 1, Registry->Header.Id);
	return Registry;
}

internal void
Wayland_Registry_HandleEvent(wayland_registry *This, wayland_message *Message)
{
	switch (Message->Opcode) {
		case 0: TRYCALL(This, HandleGlobal, Message, Uint, String, Uint); break;
		case 1: TRYCALL(This, HandleGlobalRemove, Message, Uint); break;
	}
}

internal u32
Wayland_Registry_Bind(wayland_registry *This, u32 Name)
{
	u32 NewId = Wayland_AllocateObjectId();
	Wayland_SendMessage(This->Header.Id, 0, 2, Name, NewId);
	return NewId;
}

internal void
Wayland_Callback_HandleEvent(wayland_callback *This, wayland_message *Message)
{
	switch (Message->Opcode) {
		case 0: TRYCALL(This, HandleDone, Message, Uint); break;
	}
}

#undef TRYCALL

#endif
#endif
