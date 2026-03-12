/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                             *
*  Author: Aria Seiler                                                        *
*                                                                             *
*  This program is in the public domain. There is no implied warranty, so     *
*  use it at your own risk.                                                   *
*                                                                             *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef INCLUDE_HEADER

#define WAYLAND_DISPLAY_ID 1

#ifndef SECTION_TYPES

typedef u32 wayland_fixed;

typedef struct wayland_array		   wayland_array;
typedef struct wayland_param		   wayland_param;
typedef struct wayland_interface	   wayland_interface;
typedef struct wayland_message		   wayland_message;
typedef struct wayland_method		   wayland_method;
typedef struct wayland_prepared_method wayland_prepared_method;
typedef struct wayland_event		   wayland_event;
typedef struct wayland_prototype	   wayland_prototype;

typedef enum wayland_param_type : u08 {
	WAYLAND_PARAM_SINT,
	WAYLAND_PARAM_UINT,
	WAYLAND_PARAM_FIXED,
	WAYLAND_PARAM_STRING,
	WAYLAND_PARAM_ARRAY,
	WAYLAND_PARAM_OBJECT,
	WAYLAND_PARAM_NEWOBJECT,
	WAYLAND_PARAM_FD,
} wayland_param_type;

struct wayland_array {
	u32	 Size;
	u32 *Data;
};

struct wayland_param {
	wayland_param_type Type;
	u08				   Size;
	b08				   Nullable;

	union {
		s32			  Sint;
		u32			  Uint;
		wayland_fixed Fixed;

		c08			 *String;
		wayland_array Array;

		struct {
			wayland_interface *Data;
			b08				   Destructor;
		} Object;

		struct {
			wayland_interface *Data;
			wayland_prototype *Prototype;
		} NewObject;

		s32 Fd;

		u08 Data[];
	};
};

struct wayland_interface {
	u32 Id;
	u32 Version;
	u32 Size;

	wayland_prototype *Prototype;

	vptr Events[];
};

struct wayland_message {
	u32 MessageSize;
	u32 ControlSize;

	u32			*MessageData;
	sys_cmsghdr *ControlData;
};

struct wayland_method {
	c08				   *Name;
	c08				   *Format;
	wayland_prototype **Prototypes;
};

struct wayland_prepared_method {
	wayland_method Method;

	wayland_interface *Object;
	u16				   Opcode;
	u16				   FdCount;
	u16				   NewCount;
	u16				   ParamCount;

	u32	 VersionSince;
	u32	 DeprecatedSince;
	c08 *Format;

	wayland_param *Params;
};

struct wayland_event {
	wayland_prepared_method Method;
	wayland_message			Message;
};

struct wayland_prototype {
	c08 *Name;

	u32 Version;
	u16 RequestCount;
	u16 EventCount;

	wayland_method *Requests;
	wayland_method *Events;
};

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

typedef struct wayland_api_id_entry {
	struct wayland_api_id_entry *Next;

	u32 BaseId;
	u32 Count;
} wayland_api_id_entry;

typedef struct wayland_message_queue_entry {
	struct wayland_message_queue_entry *Next;

	wayland_prepared_method PreparedMethod;
	u32					   *Data;
} wayland_message_queue_entry;

typedef struct wayland_fd_queue_entry {
	struct wayland_fd_queue_entry *Next;

	s32 Fd;
} wayland_fd_queue_entry;

typedef struct wayland_message_queue {
	usize Count;

	wayland_message_queue_entry *Front;
	wayland_message_queue_entry *Back;
} wayland_message_queue;

typedef struct wayland_fd_queue {
	usize Count;

	wayland_fd_queue_entry *Front;
	wayland_fd_queue_entry *Back;
} wayland_fd_queue;

typedef struct wayland_api_state {
	heap *Heap;
	usize HeapSize;

	s32 Socket;

	u32 ReadLock;
	u32 WriteLock;

	b08 Attempted;
	b08 Connected;

	u32					  IdLock;
	wayland_api_id_entry *NextIdEntry;

	u32		IdTableLock;
	hashmap IdTable;

	wayland_message_queue MessageQueue;
	wayland_fd_queue	  FdQueue;
} wayland_api_state;

#endif

#ifndef SECTION_FUNCS

#define WAYLAND_API_FUNCS \
	INTERN(b08,                Wayland_IsType,           wayland_interface *Object, c08 *Name) \
	INTERN(wayland_prototype*, Wayland_FindPrototype,    c08 *Name) \
	INTERN(vptr,               Wayland_GetObject,        u32 ObjectId) \
	INTERN(vptr,               Wayland_CreateObject,     wayland_prototype *Prototype, u32 ObjectId, u32 Version) \
	INTERN(void,               Wayland_DestroyObject,    vptr Object) \
	INTERN(wayland_display*,   Wayland_GetDisplay,       void) \
	\
	INTERN(b08,  Wayland_IsConnected, void) \
	INTERN(void, Wayland_Disconnect,  void) \
	INTERN(b08,  Wayland_Connect,     void) \
	\
	INTERN(b08,  Wayland_PollEventQueue, s32 Timeout, wayland_event *Event) \
	INTERN(void, Wayland_DispatchEvent,  wayland_event Event) \
	INTERN(void, Wayland_DestroyEvent,   wayland_event Event) \
	\
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

#ifdef _LINUX
#ifdef INCLUDE_SOURCE

/**
 * Key:
 * - _: Deprecated
 * - 1-9: Version
 * - ?: nullable
 * - i: int
 * - u: uint
 * - r: Fixed
 * - s: String
 * - a: Array
 * - f: File Descroptor
 * - o: Object
 * - d: Object to Destroy
 * - n: New Object (pass prototype afterward, or NULL)
 *
 * Request Format: `\d*(_\d*)[od](\?[sod]|[iurafnsod])*`
 * Event Format: `\d*(_\d*)o(\?[sod]|[iurafnsod])*`
 */

#ifndef SECTION_PROTOTYPES

#ifndef SECTION_WAYLAND

extern wayland_prototype WaylandDisplayPrototype;
extern wayland_prototype WaylandRegistryPrototype;
extern wayland_prototype WaylandCallbackPrototype;
extern wayland_prototype WaylandCompositorPrototype;
extern wayland_prototype WaylandShmPoolPrototype;
extern wayland_prototype WaylandShmPrototype;
extern wayland_prototype WaylandBufferPrototype;
extern wayland_prototype WaylandDataOfferPrototype;
extern wayland_prototype WaylandDataSourcePrototype;
extern wayland_prototype WaylandDataDevicePrototype;
extern wayland_prototype WaylandDataDeviceManagerPrototype;
extern wayland_prototype WaylandShellPrototype;
extern wayland_prototype WaylandShellSurfacePrototype;
extern wayland_prototype WaylandSurfacePrototype;
extern wayland_prototype WaylandSeatPrototype;
extern wayland_prototype WaylandPointerPrototype;
extern wayland_prototype WaylandKeyboardPrototype;
extern wayland_prototype WaylandTouchPrototype;
extern wayland_prototype WaylandOutputPrototype;
extern wayland_prototype WaylandRegionPrototype;
extern wayland_prototype WaylandSubcompositorPrototype;
extern wayland_prototype WaylandSubsurfacePrototype;
extern wayland_prototype WaylandFixesPrototype;

// clang-format off
wayland_prototype WaylandDisplayPrototype = {
	"wl_display", 1,
	2, 2,
	(wayland_method[]){
		{ "sync", "on", (wayland_prototype*[]){ &WaylandCallbackPrototype } },
		{ "get_registry", "on", (wayland_prototype*[]){ &WaylandRegistryPrototype } },
	},
	(wayland_method[]){
		{ "error", "oous", NULL },
		{ "delete_id", "ou", NULL },
	}
};

wayland_prototype WaylandRegistryPrototype = {
	"wl_registry", 1,
	1, 2,
	(wayland_method[]){
		{ "bind", "oun", (wayland_prototype*[]){ NULL } },
	},
	(wayland_method[]){
		{ "global", "ousu", NULL },
		{ "global_remove", "ou", NULL },
	}
};

wayland_prototype WaylandCallbackPrototype = {
	"wl_callback", 1,
	0, 1,
	NULL,
	(wayland_method[]){
		{ "done", "ou", NULL },
	}
};

wayland_prototype WaylandCompositorPrototype = {
	"wl_compositor", 6,
	2, 0,
	(wayland_method[]){
		{ "create_surface", "on", (wayland_prototype*[]){ &WaylandSurfacePrototype } },
		{ "create_region", "on", (wayland_prototype*[]){ &WaylandRegionPrototype } },
	},
	NULL
};

wayland_prototype WaylandShmPoolPrototype = {
	"wl_shm_pool", 2,
	3, 0,
	(wayland_method[]){
		{ "create_buffer", "oniiiiu", (wayland_prototype*[]){ &WaylandBufferPrototype } },
		{ "destroy", "d", NULL },
		{ "resize", "oi", NULL },
	},
	NULL
};

wayland_prototype WaylandShmPrototype = {
	"wl_shm", 2,
	2, 1,
	(wayland_method[]){
		{ "create_pool", "onfi", (wayland_prototype*[]){ &WaylandShmPoolPrototype } },
		{ "release", "2d", NULL },
	},
	(wayland_method[]){
		{ "format", "ou", NULL },
	}
};

wayland_prototype WaylandBufferPrototype = {
	"wl_buffer", 1,
	1, 1,
	(wayland_method[]){
		{ "destroy", "d", NULL },
	},
	(wayland_method[]){
		{ "release", "o", NULL },
	}
};

wayland_prototype WaylandDataOfferPrototype = {
	"wl_data_offer", 3,
	5, 3,
	(wayland_method[]){
		{ "accept", "ou?s", NULL },
		{ "receive", "osf", NULL },
		{ "destroy", "d", NULL },
		{ "finish", "3o", NULL },
		{ "set_actions", "3ouu", NULL },
	},
	(wayland_method[]){
		{ "offer", "os", NULL },
		{ "source_actions", "3ou", NULL },
		{ "action", "3o", NULL },
	}
};

wayland_prototype WaylandDataSourcePrototype = {
	"wl_data_source", 3,
	3, 6,
	(wayland_method[]){
		{ "offer", "os", NULL },
		{ "destroy", "d", NULL },
		{ "set_actions", "3ou", NULL },
	},
	(wayland_method[]){
		{ "target", "o?s", NULL },
		{ "send", "osf", NULL },
		{ "cancelled", "3o", NULL },
		{ "dnd_drop_performed", "3o", NULL },
		{ "dnd_finished", "3o", NULL },
		{ "action", "3ou", NULL },
	}
};

wayland_prototype WaylandDataDevicePrototype = {
	"wl_data_device", 3,
	3, 6,
	(wayland_method[]){
		{ "start_drag", "o?oo?ou", NULL },
		{ "set_selection", "o?ou", NULL },
		{ "release", "2d", NULL },
	},
	(wayland_method[]){
		{ "data_offer", "on", (wayland_prototype*[]){ &WaylandDataOfferPrototype } },
		{ "enter", "ouorr?o", NULL },
		{ "leave", "o", NULL },
		{ "motion", "ourr", NULL },
		{ "drop", "o", NULL },
		{ "selection", "o?o", NULL },
	}
};

wayland_prototype WaylandDataDeviceManagerPrototype = {
	"wl_data_device_manager", 3,
	2, 0,
	(wayland_method[]){
		{ "create_data_source", "on", (wayland_prototype*[]){ &WaylandDataSourcePrototype } },
		{ "get_data_device", "ono", (wayland_prototype*[]){ &WaylandDataDevicePrototype } },
	},
	NULL
};

wayland_prototype WaylandShellPrototype = {
	"wl_shell", 1,
	1, 0,
	(wayland_method[]){
		{ "get_shell_surface", "_ono", (wayland_prototype*[]){ &WaylandShellSurfacePrototype } },
	},
	NULL
};

wayland_prototype WaylandShellSurfacePrototype = {
	"wl_shell_surface", 1,
	10, 3,
	(wayland_method[]){
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
	(wayland_method[]){
		{ "ping", "_ou", NULL },
		{ "configure", "_ouii", NULL },
		{ "popup_done", "_o", NULL },
	}
};

wayland_prototype WaylandSurfacePrototype = {
	"wl_surface", 6,
	11, 4,
	(wayland_method[]){
		{ "destroy", "d", NULL },
		{ "attach", "o?oii", NULL },
		{ "damage", "oiiii", NULL },
		{ "frame", "on", (wayland_prototype*[]){ &WaylandCallbackPrototype } },
		{ "set_opaque_region", "o?o", NULL },
		{ "set_input_region", "o?o", NULL },
		{ "commit", "o", NULL },
		{ "set_buffer_transform", "2oi", NULL },
		{ "set_buffer_scale", "3oi", NULL },
		{ "damage_buffer", "4oiiii", NULL },
		{ "offset", "5oii", NULL },
	},
	(wayland_method[]){
		{ "enter", "oo", NULL },
		{ "leave", "oo", NULL },
		{ "preferred_buffer_scale", "6oi", NULL },
		{ "preferred_buffer_transform", "6ou", NULL },
	}
};

wayland_prototype WaylandSeatPrototype = {
	"wl_seat", 10,
	4, 2,
	(wayland_method[]){
		{ "get_pointer", "on", (wayland_prototype*[]){ &WaylandPointerPrototype } },
		{ "get_keyboard", "on", (wayland_prototype*[]){ &WaylandKeyboardPrototype } },
		{ "get_touch", "on", (wayland_prototype*[]){ &WaylandTouchPrototype } },
		{ "release", "5d", NULL },
	},
	(wayland_method[]){
		{ "capabilities", "ou", NULL },
		{ "name", "2os", NULL },
	}
};

wayland_prototype WaylandPointerPrototype = {
	"wl_pointer", 10,
	2, 11,
	(wayland_method[]){
		{ "set_cursor", "ou?oii", NULL },
		{ "release", "3d", NULL },
	},
	(wayland_method[]){
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
	}
};

wayland_prototype WaylandKeyboardPrototype = {
	"wl_keyboard", 10,
	1, 6,
	(wayland_method[]){
		{ "release", "3d", NULL },
	},
	(wayland_method[]){
		{ "keymap", "oufu", NULL },
		{ "enter", "ouoa", NULL },
		{ "leave", "ouo", NULL },
		{ "key", "ouuuu", NULL },
		{ "modifiers", "ouuuuu", NULL },
		{ "repeat_info", "4oii", NULL },
	}
};

wayland_prototype WaylandTouchPrototype = {
	"wl_touch", 10,
	1, 7,
	(wayland_method[]){
		{ "release", "3d", NULL },
	},
	(wayland_method[]){
		{ "down", "ouuoirr", NULL },
		{ "up", "ouui", NULL },
		{ "motion", "ouirr", NULL },
		{ "frame", "o", NULL },
		{ "cancel", "o", NULL },
		{ "shape", "6oirr", NULL },
		{ "orientation", "6oir", NULL },
	}
};

wayland_prototype WaylandOutputPrototype = {
	"wl_output", 4,
	1, 6,
	(wayland_method[]){
		{ "release", "3d", NULL },
	},
	(wayland_method[]){
		{ "geometry", "oiiiiissi", NULL },
		{ "mode", "ouiii", NULL },
		{ "done", "2o", NULL },
		{ "scale", "2oi", NULL },
		{ "name", "4os", NULL },
		{ "description", "4os", NULL },
	}
};

wayland_prototype WaylandRegionPrototype = {
	"wl_region", 1,
	3, 0,
	(wayland_method[]){
		{ "destroy", "d", NULL },
		{ "add", "oiiii", NULL },
		{ "subtract", "oiiii", NULL },
	},
	NULL
};

wayland_prototype WaylandSubcompositorPrototype = {
	"wl_subcompositor", 1,
	2, 0,
	(wayland_method[]){
		{ "destroy", "d", NULL },
		{ "get_subsurface", "onoo", (wayland_prototype*[]){ &WaylandSubsurfacePrototype } },
	},
	NULL
};

wayland_prototype WaylandSubsurfacePrototype = {
	"wl_subsurface", 1,
	6, 0,
	(wayland_method[]){
		{ "destroy", "d", NULL },
		{ "set_position", "oii", NULL },
		{ "place_above", "oo", NULL },
		{ "place_below", "oo", NULL },
		{ "set_sync", "o", NULL },
		{ "set_desync", "o", NULL },
	},
	NULL
};

wayland_prototype WaylandFixesPrototype = {
	"wl_fixes", 1,
	2, 0,
	(wayland_method[]){
		{ "destroy", "d", NULL },
		{ "destroy_registry", "od", NULL },
	},
	NULL
};

#endif

#ifndef SECTION_XDG_SHELL

extern wayland_prototype WaylandXdgWmBasePrototype;
extern wayland_prototype WaylandXdgPositionerPrototype;
extern wayland_prototype WaylandXdgSurfacePrototype;
extern wayland_prototype WaylandXdgToplevelPrototype;
extern wayland_prototype WaylandXdgPopupPrototype;

// clang-format off
wayland_prototype WaylandXdgWmBasePrototype = {
	"xdg_wm_base", 7,
	4, 1,
	(wayland_method[]){
		{ "destroy", "d", NULL },
		{ "create_positioner", "on", (wayland_prototype*[]){ &WaylandXdgPositionerPrototype } },
		{ "get_xdg_surface", "ono", (wayland_prototype*[]){ &WaylandXdgSurfacePrototype } },
		{ "pong", "ou", NULL },
	},
	(wayland_method[]){
		{ "ping", "ou", NULL },
	}
};

wayland_prototype WaylandXdgPositionerPrototype = {
	"xdg_positioner", 7,
	10, 0,
	(wayland_method[]){
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
	NULL
};

wayland_prototype WaylandXdgSurfacePrototype = {
	"xdg_surface", 7,
	5, 1,
	(wayland_method[]){
		{ "destroy", "d", NULL },
		{ "get_toplevel", "on", (wayland_prototype*[]){ &WaylandXdgToplevelPrototype } },
		{ "get_popup", "on?oo", (wayland_prototype*[]){ &WaylandXdgPopupPrototype } },
		{ "set_window_geometry", "oiiii", NULL },
		{ "ack_configure", "ou", NULL },
	},
	(wayland_method[]){
		{ "configure", "ou", NULL },
	}
};

wayland_prototype WaylandXdgToplevelPrototype = {
	"xdg_toplevel", 7,
	14, 4,
	(wayland_method[]){
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
	(wayland_method[]){
		{ "configure", "oiia", NULL },
		{ "close", "o", NULL },
		{ "configure_bounds", "4oii", NULL },
		{ "wm_capabilities", "5oa", NULL },
	}
};

wayland_prototype WaylandXdgPopupPrototype = {
	"xdg_popup", 7,
	3, 3,
	(wayland_method[]){
		{ "destroy", "d", NULL },
		{ "grab", "oou", NULL },
		{ "reposition", "3oou", NULL },
	},
	(wayland_method[]){
		{ "configure", "oiiii", NULL },
		{ "popup_done", "o", NULL },
		{ "repositioned", "3ou", NULL },
	}
};
// clang-format on

#endif

#ifndef SECTION_DRM

extern wayland_prototype WaylandDrmPrototype;

// clang-format off
wayland_prototype WaylandDrmPrototype = {
	"wl_drm", 2,
	4, 4,
	(wayland_method[]){
		{ "authenticate", "ou", NULL },
		{ "create_buffer", "onuiiuu", (wayland_prototype*[]){ &WaylandBufferPrototype } },
		{ "create_planar_buffer", "onuiiuiiiiii", (wayland_prototype*[]){ &WaylandBufferPrototype } },
		{ "create_prime_buffer", "onfiiuiiiiii", (wayland_prototype*[]){ &WaylandBufferPrototype } },
	},
	(wayland_method[]){
		{ "device", "os", NULL },
		{ "format", "ou", NULL },
		{ "authenticated", "o", NULL },
		{ "capabilities", "ou", NULL },
	}
};
// clang-format on

#endif

#ifndef SECTION_LINUX_DMABUF

extern wayland_prototype WaylandZwpLinuxDmabufV1Prototype;
extern wayland_prototype WaylandZwpLinuxBufferParamsV1Prototype;
extern wayland_prototype WaylandZwpLinuxDmabufFeedbackV1Prototype;

// clang-format off
wayland_prototype WaylandZwpLinuxDmabufV1Prototype = {
	"zwp_linux_dmabuf_v1", 5,
	4, 2,
	(wayland_method[]){
		{ "destroy", "d", NULL },
		{ "create_params", "on", (wayland_prototype*[]){ &WaylandZwpLinuxBufferParamsV1Prototype } },
		{ "get_default_feedback", "4on", (wayland_prototype*[]){ &WaylandZwpLinuxDmabufFeedbackV1Prototype } },
		{ "get_surface_feedback", "4ono", (wayland_prototype*[]){ &WaylandZwpLinuxDmabufFeedbackV1Prototype } },
	},
	(wayland_method[]){
		{ "format", "ou", NULL },
		{ "modifier", "3_4ouuu", NULL },
	}
};

wayland_prototype WaylandZwpLinuxBufferParamsV1Prototype = {
	"zwp_linux_buffer_params_v1", 5,
	4, 2,
	(wayland_method[]){
		{ "destroy", "d", NULL },
		{ "add", "ofuuuuu", NULL },
		{ "create", "oiiuu", NULL },
		{ "create_immed", "2oniiuu", (wayland_prototype*[]){ &WaylandBufferPrototype } },
	},
	(wayland_method[]){
		{ "created", "on", (wayland_prototype*[]){ &WaylandBufferPrototype } },
		{ "failed", "o", NULL },
	}
};

wayland_prototype WaylandZwpLinuxDmabufFeedbackV1Prototype = {
	"zwp_linux_dmabuf_feedback_v1", 5,
	1, 7,
	(wayland_method[]){
		{ "destroy", "d", NULL },
	},
	(wayland_method[]){
		{ "done", "o", NULL },
		{ "format_table", "ofu", NULL },
		{ "main_device", "oa", NULL },
		{ "tranche_done", "o", NULL },
		{ "tranche_target_device", "oa", NULL },
		{ "tranche_formats", "oa", NULL },
		{ "tranche_flags", "ou", NULL },
	}
};
// clang-format on

#endif

wayland_prototype *WaylandPrototypes[] = {
	&WaylandDisplayPrototype,
	&WaylandRegistryPrototype,
	&WaylandCallbackPrototype,
	&WaylandCompositorPrototype,
	&WaylandShmPoolPrototype,
	&WaylandShmPrototype,
	&WaylandBufferPrototype,
	&WaylandDataOfferPrototype,
	&WaylandDataSourcePrototype,
	&WaylandDataDevicePrototype,
	&WaylandDataDeviceManagerPrototype,
	&WaylandShellPrototype,
	&WaylandShellSurfacePrototype,
	&WaylandSurfacePrototype,
	&WaylandSeatPrototype,
	&WaylandPointerPrototype,
	&WaylandKeyboardPrototype,
	&WaylandTouchPrototype,
	&WaylandOutputPrototype,
	&WaylandRegionPrototype,
	&WaylandSubcompositorPrototype,
	&WaylandSubsurfacePrototype,
	&WaylandFixesPrototype,
	&WaylandXdgWmBasePrototype,
	&WaylandXdgPositionerPrototype,
	&WaylandXdgSurfacePrototype,
	&WaylandXdgToplevelPrototype,
	&WaylandXdgPopupPrototype,
	&WaylandDrmPrototype,
	&WaylandZwpLinuxDmabufV1Prototype,
	&WaylandZwpLinuxBufferParamsV1Prototype,
	&WaylandZwpLinuxDmabufFeedbackV1Prototype,
};

#endif

#ifndef SECTION_UTIL

#ifndef SECTION_OBJECT_MANAGEMENT

internal b08
Wayland_IsType(wayland_interface *Object, c08 *Name)
{
	if (!Object) return FALSE;
	return String_Cmp(CString(Object->Prototype->Name), CString(Name)) == 0;
}

internal wayland_prototype *
Wayland_FindPrototype(c08 *Name)
{
	usize  Start  = 0;
	usize  End	  = sizeof(WaylandPrototypes) / sizeof(wayland_prototype *);
	usize  Index  = 0;
	string Target = CString(Name);

	wayland_prototype *Curr = NULL;

	while (Start != End) {
		Index = Start + (End - Start) / 2;
		Curr  = WaylandPrototypes[Index];

		s32 Cmp = String_Cmp(Target, CString(Curr->Name));

		if (Cmp == EQUAL) break;
		if (Cmp == LESS) Start = Index + 1;
		else End = Index;
	}

	return Start == End ? NULL : Curr;
}

internal u32
Wayland_AllocateObjectId(void)
{
	Platform_LockMutex(&_G.WaylandApi.IdLock);

	wayland_api_id_entry *Entry = _G.WaylandApi.NextIdEntry;
	if (!Entry || !Entry->Count) return 0;

	u32 Id		   = Entry->BaseId;
	Entry->BaseId += 1;
	Entry->Count  -= 1;

	if (Entry->Count == 0) {
		_G.WaylandApi.NextIdEntry = Entry->Next;
		Heap_FreeA(Entry);
	}

	Platform_UnlockMutex(&_G.WaylandApi.IdLock);
	return Id;
}

internal void
Wayland_FreeObjectId(u32 ObjectId)
{
	Platform_LockMutex(&_G.WaylandApi.IdLock);

	// Store a pointer to the entry pointer to allow inserting a new record
	// after the previous.
	wayland_api_id_entry **Entry = &_G.WaylandApi.NextIdEntry;

	// Find the appropriate location to insert
	while (*Entry && (*Entry)->BaseId + (*Entry)->Count < ObjectId)
		Entry = &(*Entry)->Next;

	// Add a new record after prev
	if (!*Entry || ObjectId < (*Entry)->BaseId - 1) {
		wayland_api_id_entry *NewEntry =
			Heap_AllocateA(_G.WaylandApi.Heap, sizeof(wayland_api_id_entry));

		NewEntry->BaseId = ObjectId;
		NewEntry->Count	 = 1;
		NewEntry->Next	 = *Entry;

		*Entry = NewEntry;

		Platform_UnlockMutex(&_G.WaylandApi.IdLock);
		return;
	}

	// Expand this record right and potentially merge the next
	if ((*Entry)->BaseId + (*Entry)->Count == ObjectId) {
		(*Entry)->Count += 1;

		wayland_api_id_entry *Next = (*Entry)->Next;
		if (Next && Next->BaseId == ObjectId + 1) {
			(*Entry)->Count += Next->Count;
			(*Entry)->Next	 = Next->Next;
			Heap_FreeA(Next);
		}

		Platform_UnlockMutex(&_G.WaylandApi.IdLock);
		return;
	}

	// Expand this record left. We don't need to try to merge, since if it was
	// possible, we already would've above.
	if ((*Entry)->BaseId == ObjectId + 1) {
		(*Entry)->BaseId -= 1;
		(*Entry)->Count	 += 1;

		Platform_UnlockMutex(&_G.WaylandApi.IdLock);
		return;
	}

	// Shouldn't get here
	Assert(FALSE);
}

internal vptr
Wayland_GetObject(u32 ObjectId)
{
	wayland_interface *Object = NULL;

	Platform_LockMutex(&_G.WaylandApi.IdTableLock);
	HashMap_Get(&_G.WaylandApi.IdTable, &ObjectId, &Object);
	Platform_UnlockMutex(&_G.WaylandApi.IdTableLock);

	return Object;
}

internal vptr
Wayland_CreateObject(wayland_prototype *Prototype, u32 ObjectId, u32 Version)
{
	usize InterfaceSize =
		sizeof(wayland_interface) + sizeof(vptr) * Prototype->EventCount;
	wayland_interface *Object =
		Heap_AllocateA(_G.WaylandApi.Heap, InterfaceSize);

	if (Prototype->Version < Version) {
		FPrintL(
			"Warning: Attempted to create %s-v%d, but its maximum version is "
			"v%d. Defaulting to v%d\n",
			Prototype->Name,
			Version,
			Prototype->Version,
			Prototype->Version
		);
		Version = Prototype->Version;
	}

	Mem_Set(Object, 0, InterfaceSize);
	Object->Id		  = ObjectId;
	Object->Version	  = Version;
	Object->Size	  = InterfaceSize;
	Object->Prototype = Prototype;

	Platform_LockMutex(&_G.WaylandApi.IdTableLock);
	HashMap_Add(&_G.WaylandApi.IdTable, &ObjectId, &Object);
	Platform_UnlockMutex(&_G.WaylandApi.IdTableLock);

	return Object;
}

internal void
Wayland_DestroyObject(vptr Object)
{
	wayland_interface *Interface = Object;
	if (!Interface->Id) return;

	Platform_LockMutex(&_G.WaylandApi.IdTableLock);
	HashMap_Remove(&_G.WaylandApi.IdTable, &Interface->Id, NULL, NULL);
	Platform_UnlockMutex(&_G.WaylandApi.IdTableLock);

	Mem_Set(Interface, 0, Interface->Size);
	Heap_FreeA(Interface);
}

internal wayland_display *
Wayland_GetDisplay()
{
	wayland_display *Display = Wayland_GetObject(WAYLAND_DISPLAY_ID);
	if (Display) return Display;

	return Wayland_CreateObject(
		&WaylandDisplayPrototype,
		WAYLAND_DISPLAY_ID,
		1
	);
}

#endif

#ifndef SECTION_SESSION_MANAGEMENT

internal b08
Wayland_IsConnected(void)
{
	return _G.WaylandApi.Connected;
}

internal void
Wayland_Disconnect(void)
{
	if (_G.WaylandApi.Connected) {
		Sys_Close(_G.WaylandApi.Socket);
		_G.WaylandApi.Connected = FALSE;
	}
}

internal b08
Wayland_ConnectToSocket(string Name)
{
	if (_G.WaylandApi.Connected) return TRUE;
	if (Name.Length >= sizeof((sys_sockaddr_unix) {}.Data)) return FALSE;

	s32 FileDescriptor =
		Sys_Socket(SYS_ADDRESS_FAMILY_UNIX, SYS_SOCKET_STREAM, 0);
	if (FileDescriptor < 0) return FALSE;

	sys_sockaddr_unix Address;
	Address.Family = SYS_ADDRESS_FAMILY_UNIX;
	Mem_Cpy(Address.Data, Name.Text, Name.Length);
	Address.Data[Name.Length] = 0;

	s32 Result = Sys_Connect(
		FileDescriptor,
		(sys_sockaddr *) &Address,
		sizeof(sys_sockaddr_unix)
	);
	if (Result < 0) {
		Sys_Close(FileDescriptor);
		return FALSE;
	}

	_G.WaylandApi.Socket	= FileDescriptor;
	_G.WaylandApi.Connected = TRUE;
	return TRUE;
}

internal b08
Wayland_Connect(void)
{
	if (_G.WaylandApi.Attempted) return FALSE;
	_G.WaylandApi.Attempted = TRUE;

	usize HeapSize		   = 16 * 1024 * 1024;
	vptr  HeapBase		   = Platform_AllocateMemory(HeapSize);
	heap *Heap			   = Heap_Init(HeapBase, HeapSize);
	_G.WaylandApi.HeapSize = HeapSize;
	_G.WaylandApi.Heap	   = Heap;

	_G.WaylandApi.IdTable =
		HashMap_Init(Heap, sizeof(u32), sizeof(wayland_interface *));

	s32	   FileDescriptor;
	string WaylandSocket = Platform_GetEnvParam(CStringL("WAYLAND_SOCKET"));
	if (String_TryParseS32(WaylandSocket, &FileDescriptor)) {
#ifdef _LINUX
		_G.WaylandApi.Socket = FileDescriptor;
#endif
		_G.WaylandApi.Connected = TRUE;
	} else {
		string XdgRuntimeDir =
			Platform_GetEnvParam(CStringL("XDG_RUNTIME_DIR"));
		string WaylandDisplay =
			Platform_GetEnvParam(CStringL("WAYLAND_DISPLAY"));

		if (WaylandDisplay.Length) {
			string SocketPath =
				FStringL("%s/%s", XdgRuntimeDir, WaylandDisplay);

			if (!Wayland_ConnectToSocket(SocketPath)) {
				SocketPath = FStringL("%s/wayland-0", XdgRuntimeDir);
				Wayland_ConnectToSocket(SocketPath);
			}
		}
	}

	if (_G.WaylandApi.Connected) {
		_G.WaylandApi.NextIdEntry =
			Heap_AllocateA(Heap, sizeof(wayland_api_id_entry));
		_G.WaylandApi.NextIdEntry->BaseId = 2;
		_G.WaylandApi.NextIdEntry->Count  = 0xFEFFFFFF;
		_G.WaylandApi.NextIdEntry->Next	  = NULL;
	}

	return _G.WaylandApi.Connected;
}

#endif

#ifndef SECTION_MESSAGE_SERIALIZATION

internal b08
Wayland_WaitUntilCanSend(s32 Timeout)
{
	if (!_G.WaylandApi.Connected) return FALSE;

	sys_pollfd PollFd = { .FileDescriptor  = _G.WaylandApi.Socket,
						  .RequestedEvents = SYS_POLLOUT,
						  .ReturnedEvents  = 0 };

	s32 Result = Sys_Poll(&PollFd, 1, Timeout);
	return Result == 1 && (PollFd.ReturnedEvents & SYS_POLLOUT);
}

internal b08
Wayland_SendMessage(wayland_message Message)
{
	sys_iovec IOVector = { 0 };
	IOVector.Base	   = Message.MessageData;
	IOVector.Length	   = Message.MessageSize;

	sys_msghdr MessageHeader	= { 0 };
	MessageHeader.IOVectors		= &IOVector;
	MessageHeader.IOVectorCount = 1;
	MessageHeader.Control		= Message.ControlData;
	MessageHeader.ControlSize	= Message.ControlSize;

	ssize BytesWritten =
		Sys_SendMsg(_G.WaylandApi.Socket, &MessageHeader, SYS_MSG_NOSIGNAL);

	if (BytesWritten < 0) return FALSE;
	Assert(BytesWritten == Message.MessageSize);
	return TRUE;
}

internal b08
Wayland_WaitUntilCanReceive(s32 Timeout)
{
	if (!_G.WaylandApi.Connected) return FALSE;

	sys_pollfd PollFd = { .FileDescriptor  = _G.WaylandApi.Socket,
						  .RequestedEvents = SYS_POLLIN,
						  .ReturnedEvents  = 0 };

	s32 Result = Sys_Poll(&PollFd, 1, Timeout);
	return Result == 1 && (PollFd.ReturnedEvents & SYS_POLLIN);
}

internal void
Wayland_DestroyMessage(wayland_message Message)
{
	if (Message.MessageData) Heap_FreeA(Message.MessageData);
	if (Message.ControlData) Heap_FreeA(Message.ControlData);
}

internal string
Wayland_GetObjectName(wayland_interface *Object)
{
	if (!Object) return CStringL("null");
	return FStringL(
		"%s-v%d#%d",
		CString(Object->Prototype->Name),
		Object->Version,
		Object->Id
	);
}

internal void
Wayland_LogMessage(
	wayland_interface *Object,
	c08				  *MethodName,
	c08				  *Message,
	...
)
{
	string Descriptor = EString();
	if (Object) {
		string ObjectName = Wayland_GetObjectName(Object);

		Descriptor = FStringL(
			" [%s%s%s]",
			ObjectName,
			MethodName ? CStringL("::") : EString(),
			CString(MethodName)
		);
	}

	va_list Args;
	VA_Start(Args, Message);
	string FormattedMessage = FVString(CString(Message), Args);
	VA_End(Args);

	FPrintL("[WaylandApi]%s %s\n", Descriptor, FormattedMessage);
}

internal wayland_prepared_method
Wayland_PrepareMethod(wayland_interface *Object, u16 Opcode, b08 IsEvent)
{
	wayland_prepared_method PreparedMethod = { 0 };

	PreparedMethod.Object = Object;
	PreparedMethod.Opcode = Opcode;

	PreparedMethod.Method = IsEvent ? Object->Prototype->Events[Opcode]
									: Object->Prototype->Requests[Opcode];
	wayland_method Method = PreparedMethod.Method;

	c08 *C = Method.Format;

	while (*C >= '0' && *C <= '9') {
		PreparedMethod.VersionSince *= 10;
		PreparedMethod.VersionSince += *C - '0';
		C++;
	}
	if (!PreparedMethod.VersionSince) PreparedMethod.VersionSince = 1;

	if (*C == '_') {
		C++;
		while (*C >= '0' && *C <= '9') {
			PreparedMethod.DeprecatedSince *= 10;
			PreparedMethod.DeprecatedSince += *C - '0';
			C++;
		}
	}

	PreparedMethod.Format = C;
	Assert(
		*C == 'd' || *C == 'o',
		"The first format param must be a not-null object!"
	);

	usize PI = 0, CI = 0;
	for (C = PreparedMethod.Format; *C; C++) {
		if (*C == '?') C++;
		if (*C == 'f') PreparedMethod.FdCount++;
		else if (*C == 'n') {
			wayland_prototype *Prototype = Method.Prototypes[CI++];
			if (!Prototype) PreparedMethod.ParamCount += 2;
		}
		PreparedMethod.ParamCount++;
	}

	usize ParamsSize	  = PreparedMethod.ParamCount * sizeof(wayland_param);
	PreparedMethod.Params = Heap_AllocateA(_G.WaylandApi.Heap, ParamsSize);
	wayland_param *Params = PreparedMethod.Params;
	Mem_Set(Params, 0, ParamsSize);

	PI = 0, CI = 0;
	for (c08 *C = PreparedMethod.Format; *C; C++) {
		if (*C == '?') {
			Params[PI].Nullable = TRUE;
			C++;
		}

		switch (*C) {
			case 'i':
				Params[PI].Type = WAYLAND_PARAM_SINT;
				Params[PI].Size = sizeof(s32);
				break;
			case 'u':
				Params[PI].Type = WAYLAND_PARAM_UINT;
				Params[PI].Size = sizeof(u32);
				break;
			case 'r':
				Params[PI].Type = WAYLAND_PARAM_FIXED;
				Params[PI].Size = sizeof(wayland_fixed);
				break;
			case 's':
				Params[PI].Type = WAYLAND_PARAM_STRING;
				Params[PI].Size = sizeof(c08 *);
				break;
			case 'a':
				Params[PI].Type = WAYLAND_PARAM_ARRAY;
				Params[PI].Size = sizeof(wayland_array);
				break;
			case 'd':
			case 'o':
				Params[PI].Type				 = WAYLAND_PARAM_OBJECT;
				Params[PI].Size				 = sizeof(wayland_interface *);
				Params[PI].Object.Destructor = *C == 'd';
				break;
			case 'n':
				Params[PI].Type				   = WAYLAND_PARAM_NEWOBJECT;
				Params[PI].Size				   = sizeof(wayland_interface *);
				Params[PI].NewObject.Prototype = Method.Prototypes[CI++];
				break;
			case 'f':
				Params[PI].Type = WAYLAND_PARAM_FD;
				Params[PI].Size = sizeof(s32);
				break;
			default:
				Wayland_LogMessage(
					Object,
					Method.Name,
					"Warning: Unknown param type %c (in format %s)",
					*C,
					CString(Method.Format)
				);
		}

		b08 CanBeNullable = Params[PI].Type == WAYLAND_PARAM_OBJECT
						 || Params[PI].Type == WAYLAND_PARAM_STRING;
		if (Params[PI].Nullable && !CanBeNullable)
			Wayland_LogMessage(
				Object,
				Method.Name,
				"Warning: Param type %c cannot be null (in format %s)",
				*C,
				CString(Method.Format)
			);

		PI++;
	}

	if (PreparedMethod.VersionSince > Object->Version)
		Wayland_LogMessage(
			Object,
			Method.Name,
			"Warning: Method is not introduced until v%d",
			PreparedMethod.VersionSince
		);

	b08 IsDeprecated = PreparedMethod.DeprecatedSince
					&& PreparedMethod.DeprecatedSince <= Object->Version;
	if (IsDeprecated)
		Wayland_LogMessage(
			Object,
			Method.Name,
			"Warning: Method has been deprecated since v%d",
			PreparedMethod.DeprecatedSince
		);

	return PreparedMethod;
}

internal void
Wayland_FillRequestParams(wayland_prepared_method PreparedMethod, va_list Args)
{
	wayland_interface *Object			 = PreparedMethod.Object;
	PreparedMethod.Params[0].Object.Data = Object;

	for (usize I = 1; I < PreparedMethod.ParamCount; I++) {
		wayland_param *Param = &PreparedMethod.Params[I];
		switch (Param->Type) {
			case WAYLAND_PARAM_SINT: Param->Sint = VA_Next(Args, s32); break;
			case WAYLAND_PARAM_UINT: Param->Uint = VA_Next(Args, u32); break;
			case WAYLAND_PARAM_FIXED:
				Param->Fixed = VA_Next(Args, wayland_fixed);
				break;
			case WAYLAND_PARAM_STRING:
				Param->String = VA_Next(Args, c08 *);
				if (!Param->String && !Param->Nullable)
					Wayland_LogMessage(
						Object,
						PreparedMethod.Method.Name,
						"Warning: Provided a null string for non-nullable "
						"param %d",
						I + 1
					);
				break;
			case WAYLAND_PARAM_ARRAY:
				Param->Array = VA_Next(Args, wayland_array);
				break;
			case WAYLAND_PARAM_OBJECT:
				Param->Object.Data = VA_Next(Args, wayland_interface *);
				if (!Param->Object.Data && !Param->Nullable)
					Wayland_LogMessage(
						Object,
						PreparedMethod.Method.Name,
						"Warning: Provided a null object for "
						"non-nullable param %d",
						I
					);
				break;
			case WAYLAND_PARAM_NEWOBJECT:
				u32 ObjectId = Wayland_AllocateObjectId();

				wayland_prototype *Prototype = Param->NewObject.Prototype;
				if (Prototype) {
					u32 Version = Object->Version;
					Param->NewObject.Data =
						Wayland_CreateObject(Prototype, ObjectId, Version);
				} else {
					c08 *Name	 = VA_Next(Args, c08 *);
					u32	 Version = VA_Next(Args, u32);
					Prototype	 = Wayland_FindPrototype(Name);
					Assert(
						Prototype,
						"Failed to find prototype for constructed object"
					);
					Param->NewObject.Data =
						Wayland_CreateObject(Prototype, ObjectId, Version);
				}
				break;
			case WAYLAND_PARAM_FD: Param->Fd = VA_Next(Args, s32); break;
			default				 : Assert(FALSE);
		}
	}
}

internal void
Wayland_GetConstructedParams(
	wayland_prepared_method PreparedMethod,
	wayland_interface	  **ConstructedOut
)
{
	usize CI = 0;
	for (usize I = 0; I < PreparedMethod.ParamCount; I++) {
		wayland_param *Param = &PreparedMethod.Params[I];
		if (Param->Type == WAYLAND_PARAM_NEWOBJECT)
			ConstructedOut[CI++] = Param->NewObject.Data;
	}
}

internal void
Wayland_DestroyDestructorParams(wayland_prepared_method PreparedMethod)
{
	for (usize I = 0; I < PreparedMethod.ParamCount; I++) {
		wayland_param *Param = &PreparedMethod.Params[I];
		if (Param->Type == WAYLAND_PARAM_OBJECT && Param->Object.Destructor) {
			wayland_interface *Object = Param->Object.Data;
			if (Object) Wayland_DestroyObject(Object);
		}
	}
}

internal void
Wayland_DestroyPreparedMethod(wayland_prepared_method PreparedMethod)
{
	Heap_FreeA(PreparedMethod.Params);
}

internal void
Wayland_SerializeParam(
	wayland_message Message,
	usize		   *WordIndex,
	usize		   *FdIndex,
	wayland_param	Param
)
{
	u32	 *Words = (vptr) Message.MessageData;
	s32	 *Fds = Message.ControlData ? (vptr) &Message.ControlData->Data : NULL;
	usize WI  = *WordIndex;
	usize FI  = *FdIndex;

	switch (Param.Type) {
		case WAYLAND_PARAM_SINT:
			if (Words) *(s32 *) &Words[WI] = Param.Sint;
			WI++;
			break;
		case WAYLAND_PARAM_UINT:
			if (Words) Words[WI] = Param.Uint;
			WI++;
			break;
		case WAYLAND_PARAM_FIXED:
			if (Words) *(wayland_fixed *) &Words[WI] = Param.Fixed;
			WI++;
			break;
		case WAYLAND_PARAM_STRING:
			if (!Param.String) {
				if (Words) Words[WI] = 0;
				WI++;
			} else {
				string Str = CString(Param.String);
				if (Words) {
					Words[WI] = Str.Length + 1;
					Mem_Cpy(Words + WI + 1, Str.Text, Str.Length + 1);
				}
				WI += 1 + (Str.Length + 4) / 4;
			}
			break;
		case WAYLAND_PARAM_ARRAY:
			wayland_array Array = Param.Array;
			if (Words) {
				Words[WI] = Array.Size;
				Mem_Cpy(Words + WI + 1, Array.Data, Array.Size);
			}
			WI += 1 + (Array.Size + 3) / 4;
			break;
		case WAYLAND_PARAM_OBJECT:
			wayland_interface *Object = Param.Object.Data;
			if (Words) Words[WI] = Object ? Object->Id : 0;
			WI++;
			break;
		case WAYLAND_PARAM_NEWOBJECT:
			wayland_interface *NewObject = Param.NewObject.Data;
			if (!Param.NewObject.Prototype) {
				string Name		= CString(NewObject->Prototype->Name);
				u32	   Version	= NewObject->Version;
				usize  StrWords = 1 + (Name.Length + 4) / 4;
				if (Words) {
					Words[WI] = Name.Length + 1;
					Mem_Cpy(Words + WI + 1, Name.Text, Name.Length + 1);
					Words[StrWords] = Version;
				}
				WI += StrWords + 1;
			}
			if (Words) Words[WI] = NewObject->Id;
			WI++;
			break;
		case WAYLAND_PARAM_FD:
			if (Fds) Fds[FI] = Param.Fd;
			FI++;
			break;
		default: Assert(FALSE);
	}

	*WordIndex = WI;
	*FdIndex   = FI;
}

internal wayland_message
Wayland_SerializeMethod(wayland_prepared_method PreparedMethod)
{
	usize MessageHeaderSize = 2 * sizeof(u32);
	usize ControlHeaderSize = sizeof(sys_cmsghdr);

	wayland_message Message = { 0 };

	usize WI = 2, FI = 0;
	for (usize I = 1; I < PreparedMethod.ParamCount; I++)
		Wayland_SerializeParam(Message, &WI, &FI, PreparedMethod.Params[I]);

	Message.MessageSize = (2 + WI) * sizeof(u32);
	Message.MessageData =
		Heap_AllocateA(_G.WaylandApi.Heap, Message.MessageSize);
	Message.MessageData[0] = PreparedMethod.Object->Id;
	Message.MessageData[1] =
		(Message.MessageSize << 16) | PreparedMethod.Opcode;

	if (FI) {
		Message.ControlSize = sizeof(sys_cmsghdr) + FI * sizeof(s32);
		Message.ControlData =
			Heap_AllocateA(_G.WaylandApi.Heap, Message.ControlSize);

		Message.ControlData->Length = Message.ControlSize;
		Message.ControlData->Level	= SYS_SOL_SOCKET;
		Message.ControlData->Type	= SYS_SCM_RIGHTS;
	}

	WI = 2, FI = 0;
	for (usize I = 1; I < PreparedMethod.ParamCount; I++)
		Wayland_SerializeParam(Message, &WI, &FI, PreparedMethod.Params[I]);

	return Message;
}

internal void
Wayland_DeserializeParam(
	u32					   *Words,
	usize				   *WordIndex,
	wayland_prepared_method PreparedMethod,
	wayland_param		   *Param,
	usize					ParamIndex
)
{
	wayland_interface *Object = PreparedMethod.Object;
	usize			   WI	  = *WordIndex;

	switch (Param->Type) {
		case WAYLAND_PARAM_SINT: Param->Sint = *(s32 *) &Words[WI++]; break;
		case WAYLAND_PARAM_UINT: Param->Uint = Words[WI++]; break;
		case WAYLAND_PARAM_FIXED:
			Param->Fixed = *(wayland_fixed *) &Words[WI++];
			break;
		case WAYLAND_PARAM_STRING:
			u32 Size	  = Words[WI++];
			Param->String = NULL;
			if (Size) {
				Param->String  = (c08 *) &Words[WI];
				WI			  += (Size + 3) / 4;
			}
			if (!Param->String && !Param->Nullable)
				Wayland_LogMessage(
					Object,
					PreparedMethod.Method.Name,
					"Warning: Received a null string for non-nullable param %d",
					ParamIndex
				);
			break;
		case WAYLAND_PARAM_ARRAY:
			Param->Array.Size  = Words[WI++];
			Param->Array.Data  = &Words[WI];
			WI				  += (Param->Array.Size + 3) / 4;
			break;
		case WAYLAND_PARAM_OBJECT:
			u32 ObjectId	   = Words[WI++];
			Param->Object.Data = ObjectId ? Wayland_GetObject(ObjectId) : NULL;
			if (!ObjectId && !Param->Nullable)
				Wayland_LogMessage(
					Object,
					PreparedMethod.Method.Name,
					"Warning: Received a null object for non-nullable param %d",
					ParamIndex
				);
			if (ObjectId && !Param->Object.Data)
				Wayland_LogMessage(
					Object,
					PreparedMethod.Method.Name,
					"Warning: Failed to lookup object #%d",
					ObjectId
				);
			break;
		case WAYLAND_PARAM_NEWOBJECT:
			u32 Version = PreparedMethod.Object->Version;

			wayland_prototype *Prototype = Param->NewObject.Prototype;
			if (!Prototype) {
				u32 NameSize = Words[WI++];
				Assert(NameSize);
				c08 *Name = (c08 *) &Words[WI];
				Prototype = Wayland_FindPrototype(Name);
				Assert(Prototype);
				WI		+= (NameSize + 3) / 4;
				Version	 = Words[WI++];
			}

			u32 NewObjectId = Words[WI++];
			Param->NewObject.Data =
				Wayland_CreateObject(Prototype, NewObjectId, Version);
			break;
		case WAYLAND_PARAM_FD: Param->Fd = -1; break;
		default				 : Assert(FALSE);
	}

	*WordIndex = WI;
}

internal wayland_prepared_method
Wayland_DeserializeMethod(u32 *Words)
{
	u32 ObjectId	= Words[0];
	u16 Opcode		= Words[1] & 0xFFFF;
	u16 MessageSize = Words[1] >> 16;

	wayland_interface *Object = Wayland_GetObject(ObjectId);
	Assert(Object);

	wayland_prepared_method PreparedMethod =
		Wayland_PrepareMethod(Object, Opcode, TRUE);

	wayland_param *Params = PreparedMethod.Params;
	Params[0].Object.Data = Object;

	usize WI = 2;
	for (usize I = 1; I < PreparedMethod.ParamCount; I++) {
		Wayland_DeserializeParam(
			Words,
			&WI,
			PreparedMethod,
			&PreparedMethod.Params[I],
			I
		);
	}

	return PreparedMethod;
}

internal void
Wayland_EnqueueMessage(
	heap				  *Heap,
	wayland_message_queue *MessageQueue,
	u32					  *Data
)
{
	u16 MessageSize = Data[1] >> 16;
	Assert(MessageSize % sizeof(u32) == 0);

	wayland_message_queue_entry *Entry =
		Heap_AllocateA(Heap, sizeof(wayland_message_queue_entry));

	Entry->Next = NULL;
	Entry->Data = Heap_AllocateA(Heap, MessageSize);
	Mem_Cpy(Entry->Data, Data, MessageSize);

	Entry->PreparedMethod = Wayland_DeserializeMethod(Entry->Data);

	if (MessageQueue->Back) {
		MessageQueue->Back->Next = Entry;
	} else {
		Assert(!MessageQueue->Front);
		MessageQueue->Front = Entry;
	}
	MessageQueue->Back = Entry;
	MessageQueue->Count++;
}

internal void
Wayland_EnqueueFd(heap *Heap, wayland_fd_queue *FdQueue, s32 Fd)
{
	wayland_fd_queue_entry *FdEntry =
		Heap_AllocateA(Heap, sizeof(wayland_fd_queue_entry));

	FdEntry->Next = NULL;
	FdEntry->Fd	  = Fd;

	if (FdQueue->Back) {
		FdQueue->Back->Next = FdEntry;
	} else {
		Assert(!FdQueue->Front);
		FdQueue->Front = FdEntry;
	}
	FdQueue->Back = FdEntry;
	FdQueue->Count++;
}

internal b08
Wayland_DequeueMessage(
	wayland_message_queue		*MessageQueue,
	wayland_message_queue_entry *EntryOut
)
{
	wayland_message_queue_entry *Entry = MessageQueue->Front;
	if (!Entry) return FALSE;

	MessageQueue->Front = Entry->Next;
	MessageQueue->Count--;
	if (!MessageQueue->Front) MessageQueue->Back = NULL;

	if (EntryOut) *EntryOut = *Entry;
	Heap_FreeA(Entry);
	return TRUE;
}

internal s32
Wayland_DequeueFd(wayland_fd_queue *FdQueue)
{
	wayland_fd_queue_entry *Entry = FdQueue->Front;
	if (!Entry) return -1;

	FdQueue->Front = Entry->Next;
	FdQueue->Count--;
	if (!FdQueue->Front) FdQueue->Back = NULL;

	s32 Fd = Entry->Fd;
	Heap_FreeA(Entry);
	return Fd;
}

internal b08
Wayland_PollSocket(
	heap				  *Heap,
	s32					   Socket,
	wayland_message_queue *MessageQueue,
	wayland_fd_queue	  *FdQueue,
	s32					   Timeout
)
{
	if (!Wayland_WaitUntilCanReceive(Timeout)) return FALSE;

	u32		  Header[2];
	sys_iovec IOVector = { 0 };
	IOVector.Base	   = (vptr) Header;
	IOVector.Length	   = sizeof(Header);

	sys_msghdr MessageHeader	= { 0 };
	MessageHeader.IOVectors		= &IOVector;
	MessageHeader.IOVectorCount = 1;
	ssize MessageSize = Sys_RecvMsg(Socket, &MessageHeader, SYS_MSG_PEEK);

	if (MessageSize < 0) return FALSE;
	Assert(MessageSize);

	usize ControlSize = 0;
	while (MessageHeader.Flags & SYS_MSG_CTRUNC) {
		if (ControlSize) {
			ControlSize *= 2;
			Heap_ResizeA(&MessageHeader.Control, ControlSize);
		} else {
			ControlSize			  = sizeof(sys_cmsghdr) + sizeof(s32) * 2;
			MessageHeader.Control = Heap_AllocateA(Heap, ControlSize);
		}
		MessageHeader.ControlSize = ControlSize;

		MessageSize = Sys_RecvMsg(Socket, &MessageHeader, SYS_MSG_PEEK);
		if (MessageSize < 0) {
			Heap_FreeA(MessageHeader.Control);
			return FALSE;
		}
		Assert(MessageSize);
	}

	IOVector.Length = Header[1] >> 16;
	IOVector.Base	= Heap_AllocateA(Heap, IOVector.Length);

	MessageSize = Sys_RecvMsg(Socket, &MessageHeader, 0);
	if (MessageSize < 0) {
		if (MessageHeader.Control) Heap_FreeA(MessageHeader.Control);
		Heap_FreeA(IOVector.Base);
		return FALSE;
	}
	Assert(MessageSize);
	Assert(!(MessageHeader.Flags & SYS_MSG_CTRUNC));

	Wayland_EnqueueMessage(Heap, MessageQueue, IOVector.Base);
	Heap_FreeA(IOVector.Base);
	if (!ControlSize) return TRUE;

	usize FdDataSize  = 0;
	vptr  ControlData = MessageHeader.Control;
	vptr  ControlEnd  = (vptr) ControlData + MessageHeader.ControlSize;
	while (ControlData < ControlEnd) {
		sys_cmsghdr *Control  = ControlData;
		ControlData			 += Control->Length;
		FdDataSize			 += Control->Length;
	}

	Assert(FdDataSize % 4 == 0);
	vptr FdData = Heap_AllocateA(Heap, FdDataSize);

	ControlData = MessageHeader.Control;
	while (ControlData < ControlEnd) {
		sys_cmsghdr *Control  = ControlData;
		ControlData			 += Control->Length;
		Mem_Cpy(FdData, &Control->Data, Control->Length);
		FdData += Control->Length;
	}

	usize FdCount = FdDataSize / sizeof(s32);
	s32	 *Fds	  = FdData;
	for (usize I = 0; I < FdCount; I++)
		Wayland_EnqueueFd(Heap, FdQueue, Fds[I]);

	Heap_FreeA(FdData);
	if (MessageHeader.Control) Heap_FreeA(MessageHeader.Control);
	return TRUE;
}

internal b08
Wayland_TryDequeueEvent(
	heap				  *Heap,
	wayland_message_queue *MessageQueue,
	wayland_fd_queue	  *FdQueue,
	wayland_event		  *EventOut
)
{
	if (!_G.WaylandApi.MessageQueue.Count) return FALSE;

	wayland_message_queue_entry *Entry = _G.WaylandApi.MessageQueue.Front;

	if (Entry->PreparedMethod.FdCount > FdQueue->Count) return FALSE;

	wayland_event Event		  = { 0 };
	Event.Method			  = Entry->PreparedMethod;
	Event.Message.MessageData = Entry->Data;
	Event.Message.MessageSize = Entry->Data[1] >> 16;
	Event.Message.ControlSize =
		sizeof(sys_cmsghdr) + Event.Method.FdCount * sizeof(s32);
	Wayland_DequeueMessage(&_G.WaylandApi.MessageQueue, NULL);

	wayland_prepared_method PreparedMethod = Event.Method;
	if (Event.Message.ControlSize) {
		Event.Message.ControlData =
			Heap_AllocateA(Heap, Event.Message.ControlSize);
		Event.Message.ControlData->Length = Event.Message.ControlSize;

		s32 *Fds = (vptr) &Event.Message.ControlData->Data;

		usize PI = 0;
		usize CI = 0;
		usize FI = 0;
		for (c08 *C = PreparedMethod.Format; *C; C++) {
			if (*C == '?') C++;
			if (*C == 'n' && !PreparedMethod.Method.Prototypes[CI++]) PI += 2;
			else if (*C == 'f') {
				s32 Fd	  = Wayland_DequeueFd(&_G.WaylandApi.FdQueue);
				Fds[FI++] = Fd;
				PreparedMethod.Params[PI].Fd = Fd;
			}
			PI++;
		}
	}

	*EventOut = Event;
	return TRUE;
}

internal b08
Wayland_PollEventQueue(s32 Timeout, wayland_event *Event)
{
	if (!Wayland_IsConnected()) return FALSE;

	Platform_LockMutex(&_G.WaylandApi.ReadLock);
	Wayland_PollSocket(
		_G.WaylandApi.Heap,
		_G.WaylandApi.Socket,
		&_G.WaylandApi.MessageQueue,
		&_G.WaylandApi.FdQueue,
		Timeout
	);

	b08 Dequeued = Wayland_TryDequeueEvent(
		_G.WaylandApi.Heap,
		&_G.WaylandApi.MessageQueue,
		&_G.WaylandApi.FdQueue,
		Event
	);

	Platform_UnlockMutex(&_G.WaylandApi.ReadLock);
	return Dequeued;
}

internal void
Wayland_DispatchEvent(wayland_event Event)
{
	vptr Proc = Event.Method.Object->Events[Event.Method.Opcode];
	if (!Proc) return;

#if defined(_X64)
	usize StackSize = 0;
	for (usize I = 0; I < Event.Method.ParamCount; I++) {
		wayland_param Param	 = Event.Method.Params[I];
		StackSize			+= (Param.Size + 7) & ~7;
	}
	usize StackWords = StackSize / 8;

	// Leave 8-byte extra to have 16-byte alignment later
	if (StackWords % 2 == 0) {
		StackSize += 8;
		StackWords++;
	}

	u64	  Regs[6];
	usize RI = 0;

	usize *Stack	   = Heap_AllocateA(_G.WaylandApi.Heap, StackSize);
	vptr   StackCursor = Stack;
	for (usize I = 0; I < Event.Method.ParamCount; I++) {
		wayland_param Param		= Event.Method.Params[I];
		usize		  ParamSize = (Param.Size + 7) & ~7;

		usize ParamWords = ParamSize / 8;
		if (ParamWords <= 6 - RI) {
			Mem_Cpy(Regs + RI, &Param.Data, ParamSize);
			RI		  += ParamWords;
			StackSize -= ParamSize;
		} else {
			Mem_Cpy(StackCursor, &Param.Data, Param.Size);
			StackCursor += ParamSize;
		}
	}

	__asm__ volatile(
		"push %%rbx				\n"
		"mov %%rcx, %%rbx		\n"
		"sub %%rcx, %%rsp		\n"
		"shr $3, %%rcx			\n"
		"mov %%rsp, %%rdi		\n"
		"rep movsq				\n"
		"mov 0(%%rdx), %%rdi	\n"
		"mov 8(%%rdx), %%rsi	\n"
		"mov 24(%%rdx), %%rcx	\n"
		"mov 32(%%rdx), %%r8	\n"
		"mov 40(%%rdx), %%r9	\n"
		"mov 16(%%rdx), %%rdx	\n"
		"call *%%rax			\n"
		"add %%rbx, %%rsp		\n"
		"pop %%rbx				\n"
		:
		: "a"(Proc), "c"(StackSize), "d"(Regs), "S"(Stack)
	);

	Heap_FreeA(Stack);
#else
#error Unsupported Architecture
#endif
}

internal void
Wayland_DestroyEvent(wayland_event Event)
{
	Wayland_DestroyPreparedMethod(Event.Method);
	Wayland_DestroyMessage(Event.Message);
}

internal void
Wayland_HandleRequestV(
	wayland_interface  *Object,
	u16					Opcode,
	wayland_interface **Constructed,
	va_list				Args
)
{
	wayland_prepared_method PreparedMethod =
		Wayland_PrepareMethod(Object, Opcode, FALSE);

	Wayland_FillRequestParams(PreparedMethod, Args);
	Wayland_GetConstructedParams(PreparedMethod, Constructed);

	wayland_message Message = Wayland_SerializeMethod(PreparedMethod);

	Platform_LockMutex(&_G.WaylandApi.WriteLock);
	b08 CanSend = Wayland_WaitUntilCanSend(-1);
	b08 Sent	= FALSE;
	if (CanSend) Sent = Wayland_SendMessage(Message);
	Platform_UnlockMutex(&_G.WaylandApi.WriteLock);

	Wayland_DestroyMessage(Message);

	if (Sent) Wayland_DestroyDestructorParams(PreparedMethod);
	Wayland_DestroyPreparedMethod(PreparedMethod);
}

internal void
Wayland_HandleMultiConstructorRequest(
	wayland_interface  *Object,
	u16					Opcode,
	wayland_interface **Constructed,
	...
)
{
	va_list Args;
	VA_Start(Args, Opcode);
	Wayland_HandleRequestV(Object, Opcode, Constructed, Args);
	VA_End(Args);
}

internal vptr
Wayland_HandleConstructorRequest(wayland_interface *Object, u16 Opcode, ...)
{
	wayland_interface *Constructed;

	va_list Args;
	VA_Start(Args, Opcode);
	Wayland_HandleRequestV(Object, Opcode, &Constructed, Args);
	VA_End(Args);

	return Constructed;
}

internal void
Wayland_HandleRequest(wayland_interface *Object, u16 Opcode, ...)
{
	va_list Args;
	VA_Start(Args, Opcode);
	Wayland_HandleRequestV(Object, Opcode, NULL, Args);
	VA_End(Args);
}

#endif

#endif

#ifndef SECTION_PROTOCOL

#ifndef SECTION_WAYLAND

internal wayland_callback *
Wayland_Display_Sync(wayland_display *This)
{
	return Wayland_HandleConstructorRequest((wayland_interface *) This, 0);
}

internal wayland_registry *
Wayland_Display_GetRegistry(wayland_display *This)
{
	return Wayland_HandleConstructorRequest((wayland_interface *) This, 1);
}

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
		Name,
		Interface,
		Version
	);
}

internal wayland_surface *
Wayland_Compositor_CreateSurface(wayland_compositor *This)
{
	return Wayland_HandleConstructorRequest((wayland_interface *) This, 0);
}

internal wayland_region *
Wayland_Compositor_CreateRegion(wayland_compositor *This)
{
	return Wayland_HandleConstructorRequest((wayland_interface *) This, 1);
}

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
{
	Wayland_HandleRequest((wayland_interface *) This, 1);
}

internal void
Wayland_ShmPool_Resize(wayland_shm_pool *This, s32 Size)
{
	Wayland_HandleRequest((wayland_interface *) This, 2, Size);
}

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
{
	Wayland_HandleRequest((wayland_interface *) This, 1);
}

internal void
Wayland_Buffer_Destroy(wayland_buffer *This)
{
	Wayland_HandleRequest((wayland_interface *) This, 0);
}

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
{
	Wayland_HandleRequest((wayland_interface *) This, 0, Serial);
}

internal void
Wayland_ShellSurface_Move(
	wayland_shell_surface *This,
	wayland_seat		  *Seat,
	u32					   Serial
)
{
	Wayland_HandleRequest((wayland_interface *) This, 1, Seat, Serial);
}

internal void
Wayland_ShellSurface_Resize(
	wayland_shell_surface		*This,
	wayland_seat				*Seat,
	u32							 Serial,
	wayland_shell_surface_resize Edges
)
{
	Wayland_HandleRequest((wayland_interface *) This, 2, Seat, Serial, Edges);
}

internal void
Wayland_ShellSurface_SetToplevel(wayland_shell_surface *This)
{
	Wayland_HandleRequest((wayland_interface *) This, 3);
}

internal void
Wayland_ShellSurface_SetTransient(
	wayland_shell_surface		   *This,
	wayland_surface				   *Parent,
	s32								X,
	s32								Y,
	wayland_shell_surface_transient Flags
)
{
	Wayland_HandleRequest((wayland_interface *) This, 4, Parent, X, Y, Flags);
}

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
{
	Wayland_HandleRequest((wayland_interface *) This, 7, Output);
}

internal void
Wayland_ShellSurface_SetTitle(wayland_shell_surface *This, c08 *Title)
{
	Wayland_HandleRequest((wayland_interface *) This, 8, Title);
}

internal void
Wayland_ShellSurface_SetClass(wayland_shell_surface *This, c08 *Class)
{
	Wayland_HandleRequest((wayland_interface *) This, 9, Class);
}

internal void
Wayland_Surface_Destroy(wayland_surface *This)
{
	Wayland_HandleRequest((wayland_interface *) This, 0);
}

internal void
Wayland_Surface_Attach(
	wayland_surface *This,
	wayland_buffer	*Buffer,
	s32				 X,
	s32				 Y
)
{
	Wayland_HandleRequest((wayland_interface *) This, 1, Buffer, X, Y);
}

internal void
Wayland_Surface_Damage(
	wayland_surface *This,
	s32				 X,
	s32				 Y,
	s32				 Width,
	s32				 Height
)
{
	Wayland_HandleRequest((wayland_interface *) This, 2, X, Y, Width, Height);
}

internal wayland_callback *
Wayland_Surface_Frame(wayland_surface *This)
{
	return Wayland_HandleConstructorRequest((wayland_interface *) This, 3);
}

internal void
Wayland_Surface_SetOpaqueRegion(wayland_surface *This, wayland_region *Region)
{
	Wayland_HandleRequest((wayland_interface *) This, 4, Region);
}

internal void
Wayland_Surface_SetInputRegion(wayland_surface *This, wayland_region *Region)
{
	Wayland_HandleRequest((wayland_interface *) This, 5, Region);
}

internal void
Wayland_Surface_Commit(wayland_surface *This)
{
	Wayland_HandleRequest((wayland_interface *) This, 6);
}

internal void
Wayland_Surface_SetBufferTransform(
	wayland_surface			*This,
	wayland_output_transform Transform
)
{
	Wayland_HandleRequest((wayland_interface *) This, 7, Transform);
}

internal void
Wayland_Surface_SetBufferScale(wayland_surface *This, s32 Scale)
{
	Wayland_HandleRequest((wayland_interface *) This, 8, Scale);
}

internal void
Wayland_Surface_DamageBuffer(
	wayland_surface *This,
	s32				 X,
	s32				 Y,
	s32				 Width,
	s32				 Height
)
{
	Wayland_HandleRequest((wayland_interface *) This, 9, X, Y, Width, Height);
}

internal void
Wayland_Surface_Offset(wayland_surface *This, s32 X, s32 Y)
{
	Wayland_HandleRequest((wayland_interface *) This, 10, X, Y);
}

internal void
Wayland_Fixes_Destroy(wayland_fixes *This)
{
	Wayland_HandleRequest((wayland_interface *) This, 0);
}

internal void
Wayland_Fixes_DestroyRegistry(wayland_fixes *This, wayland_registry *Registry)
{
	Wayland_HandleRequest((wayland_interface *) This, 1, Registry);
}

#endif

#ifndef SECTION_XDG

internal void
Wayland_XdgWmBase_Destroy(wayland_xdg_wm_base *This)
{
	Wayland_HandleRequest((wayland_interface *) This, 0);
}

internal wayland_xdg_positioner *
Wayland_XdgWmBase_CreatePositioner(wayland_xdg_wm_base *This)
{
	return Wayland_HandleConstructorRequest((wayland_interface *) This, 1);
}

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
{
	Wayland_HandleRequest((wayland_interface *) This, 3, Serial);
}

internal void
Wayland_XdgPositioner_Destroy(wayland_xdg_positioner *This)
{
	Wayland_HandleRequest((wayland_interface *) This, 0);
}

internal void
Wayland_XdgPositioner_SetSize(
	wayland_xdg_positioner *This,
	s32						Width,
	s32						Height
)
{
	Wayland_HandleRequest((wayland_interface *) This, 1, Width, Height);
}

internal void
Wayland_XdgPositioner_SetAnchorRect(
	wayland_xdg_positioner *This,
	s32						X,
	s32						Y,
	s32						Width,
	s32						Height
)
{
	Wayland_HandleRequest((wayland_interface *) This, 2, X, Y, Width, Height);
}

internal void
Wayland_XdgPositioner_SetAnchor(
	wayland_xdg_positioner		 *This,
	wayland_xdg_positioner_anchor Anchor
)
{
	Wayland_HandleRequest((wayland_interface *) This, 3, Anchor);
}

internal void
Wayland_XdgPositioner_SetGravity(
	wayland_xdg_positioner		  *This,
	wayland_xdg_positioner_gravity Gravity
)
{
	Wayland_HandleRequest((wayland_interface *) This, 4, Gravity);
}

internal void
Wayland_XdgPositioner_SetConstraintAdjustment(
	wayland_xdg_positioner						*This,
	wayland_xdg_positioner_constraint_adjustment ConstraintAdjustment
)
{
	Wayland_HandleRequest((wayland_interface *) This, 5, ConstraintAdjustment);
}

internal void
Wayland_XdgPositioner_SetOffset(wayland_xdg_positioner *This, s32 X, s32 Y)
{
	Wayland_HandleRequest((wayland_interface *) This, 6, X, Y);
}

internal void
Wayland_XdgPositioner_SetReactive(wayland_xdg_positioner *This)
{
	Wayland_HandleRequest((wayland_interface *) This, 7);
}

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
{
	Wayland_HandleRequest((wayland_interface *) This, 9, Serial);
}

internal void
Wayland_XdgSurface_Destroy(wayland_xdg_surface *This)
{
	Wayland_HandleRequest((wayland_interface *) This, 0);
}

internal wayland_xdg_toplevel *
Wayland_XdgSurface_GetToplevel(wayland_xdg_surface *This)
{
	return Wayland_HandleConstructorRequest((wayland_interface *) This, 1);
}

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
{
	Wayland_HandleRequest((wayland_interface *) This, 3, X, Y, Width, Height);
}

internal void
Wayland_XdgSurface_AckConfigure(wayland_xdg_surface *This, u32 Serial)
{
	Wayland_HandleRequest((wayland_interface *) This, 4, Serial);
}

internal void
Wayland_XdgToplevel_Destroy(wayland_xdg_toplevel *This)
{
	Wayland_HandleRequest((wayland_interface *) This, 0);
}

internal void
Wayland_XdgToplevel_SetParent(
	wayland_xdg_toplevel *This,
	wayland_xdg_toplevel *Parent
)
{
	Wayland_HandleRequest((wayland_interface *) This, 1, Parent);
}

internal void
Wayland_XdgToplevel_SetTitle(wayland_xdg_toplevel *This, c08 *Title)
{
	Wayland_HandleRequest((wayland_interface *) This, 2, Title);
}

internal void
Wayland_XdgToplevel_SetAppId(wayland_xdg_toplevel *This, c08 *AppId)
{
	Wayland_HandleRequest((wayland_interface *) This, 3, AppId);
}

internal void
Wayland_XdgToplevel_ShowWindowMenu(
	wayland_xdg_toplevel *This,
	wayland_seat		 *Seat,
	u32					  Serial,
	s32					  X,
	s32					  Y
)
{
	Wayland_HandleRequest((wayland_interface *) This, 4, Seat, Serial, X, Y);
}

internal void
Wayland_XdgToplevel_Move(
	wayland_xdg_toplevel *This,
	wayland_seat		 *Seat,
	u32					  Serial
)
{
	Wayland_HandleRequest((wayland_interface *) This, 5, Seat, Serial);
}

internal void
Wayland_XdgToplevel_Resize(
	wayland_xdg_toplevel			*This,
	wayland_seat					*Seat,
	u32								 Serial,
	wayland_xdg_toplevel_resize_edge Edges
)
{
	Wayland_HandleRequest((wayland_interface *) This, 6, Seat, Serial, Edges);
}

internal void
Wayland_XdgToplevel_SetMaxSize(
	wayland_xdg_toplevel *This,
	s32					  Width,
	s32					  Height
)
{
	Wayland_HandleRequest((wayland_interface *) This, 7, Width, Height);
}

internal void
Wayland_XdgToplevel_SetMinSize(
	wayland_xdg_toplevel *This,
	s32					  Width,
	s32					  Height
)
{
	Wayland_HandleRequest((wayland_interface *) This, 8, Width, Height);
}

internal void
Wayland_XdgToplevel_SetMaximized(wayland_xdg_toplevel *This)
{
	Wayland_HandleRequest((wayland_interface *) This, 9);
}

internal void
Wayland_XdgToplevel_UnsetMaximized(wayland_xdg_toplevel *This)
{
	Wayland_HandleRequest((wayland_interface *) This, 10);
}

internal void
Wayland_XdgToplevel_SetFullscreen(wayland_xdg_toplevel *This)
{
	Wayland_HandleRequest((wayland_interface *) This, 11);
}

internal void
Wayland_XdgToplevel_UnsetFullscreen(wayland_xdg_toplevel *This)
{
	Wayland_HandleRequest((wayland_interface *) This, 12);
}

internal void
Wayland_XdgToplevel_SetMinimized(wayland_xdg_toplevel *This)
{
	Wayland_HandleRequest((wayland_interface *) This, 13);
}

internal void
Wayland_XdgPopup_Destroy(wayland_xdg_popup *This)
{
	Wayland_HandleRequest((wayland_interface *) This, 0);
}

internal void
Wayland_XdgPopup_Grab(wayland_xdg_popup *This, wayland_seat *Seat, u32 Serial)
{
	Wayland_HandleRequest((wayland_interface *) This, 1, Seat, Serial);
}

internal void
Wayland_XdgPopup_Reposition(
	wayland_xdg_popup	   *This,
	wayland_xdg_positioner *Positioner,
	u32						Token
)
{
	Wayland_HandleRequest((wayland_interface *) This, 2, Positioner, Token);
}

#endif

#ifndef SECTION_DRM

internal void
Wayland_Drm_Authenticate(wayland_drm *This, u32 Id)
{
	Wayland_HandleRequest((wayland_interface *) This, 0, Id);
}

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

#ifndef SECTION_DMABUF

internal void
Wayland_ZwpLinuxDmabufV1_Destroy(wayland_zwp_linux_dmabuf_v1 *This)
{
	Wayland_HandleRequest((wayland_interface *) This, 0);
}

internal wayland_zwp_linux_buffer_params_v1 *
Wayland_ZwpLinuxDmabufV1_CreateParams(wayland_zwp_linux_dmabuf_v1 *This)
{
	return Wayland_HandleConstructorRequest((wayland_interface *) This, 1);
}

internal wayland_zwp_linux_dmabuf_feedback_v1 *
Wayland_ZwpLinuxDmabufV1_GetDefaultFeedback(wayland_zwp_linux_dmabuf_v1 *This)
{
	return Wayland_HandleConstructorRequest((wayland_interface *) This, 2);
}

internal wayland_zwp_linux_dmabuf_feedback_v1 *
Wayland_ZwpLinuxDmabufV1_GetSurfaceFeedback(
	wayland_zwp_linux_dmabuf_v1 *This,
	wayland_surface				*Surface
)
{
	return Wayland_HandleConstructorRequest(
		(wayland_interface *) This,
		4,
		Surface
	);
}

internal void
Wayland_ZwpLinuxBufferParamsV1_Destroy(wayland_zwp_linux_buffer_params_v1 *This)
{
	Wayland_HandleRequest((wayland_interface *) This, 0);
}

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
{
	Wayland_HandleRequest((wayland_interface *) This, 0);
}

#endif

#undef VERSION
#undef DEPRECATED

#endif

#endif
#endif
