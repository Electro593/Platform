/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                            *
*  Author: Aria Seiler                                                       *
*                                                                            *
*  This program is in the public domain. There is no implied warranty, so    *
*  use it at your own risk.                                                  *
*                                                                            *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef INCLUDE_HEADER

typedef s64	 egl_attrib;
typedef u32	 egl_boolean;
typedef vptr egl_client_buffer;
typedef vptr egl_config;
typedef vptr egl_context;
typedef vptr egl_display;
typedef vptr egl_image;
typedef s32	 egl_int;
typedef vptr egl_native_display_type;
typedef vptr egl_native_window_type;
typedef vptr egl_surface;
typedef vptr egl_sync;

#define EGL_DONT_CARE ((egl_int)(-1))  // v1.0
#define EGL_UNKNOWN   ((egl_int)(-1))  // v1.2
#define EGL_FALSE     0                // v1.0
#define EGL_TRUE      1                // v1.0
#define EGL_NONE      0x3038           // v1.0
#define EGL_HEIGHT    0x3056 // v1.0
#define EGL_WIDTH     0x3057 // v1.0

#define EGL_DISPLAY_SCALING 10000                 // v1.2
#define EGL_FOREVER         0xFFFFFFFFFFFFFFFFull // v1.5

#define EGL_NO_CONTEXT      ((egl_context)(0))           // v1.0
#define EGL_NO_DISPLAY      ((egl_display)(0))           // v1.0
#define EGL_NO_SURFACE      ((egl_surface)(0))           // v1.0
#define EGL_DEFAULT_DISPLAY ((egl_native_display_type)(0))  // v1.4
#define EGL_NO_SYNC         ((egl_sync)(0))               // v1.5
#define EGL_NO_IMAGE        ((egl_image)(0))              // v1.5

#define EGL_LARGEST_PBUFFER                0x3058 // v1.0
#define EGL_DRAW                           0x3059 // v1.0
#define EGL_READ                           0x305A // v1.0
#define EGL_CORE_NATIVE_ENGINE             0x305B // v1.0
#define EGL_NO_TEXTURE                     0x305C // v1.1
#define EGL_TEXTURE_RGB                    0x305D // v1.1
#define EGL_TEXTURE_RGBA                   0x305E // v1.1
#define EGL_TEXTURE_2D                     0x305F // v1.1
#define EGL_TEXTURE_FORMAT                 0x3080 // v1.1
#define EGL_TEXTURE_TARGET                 0x3081 // v1.1
#define EGL_MIPMAP_TEXTURE                 0x3082 // v1.1
#define EGL_MIPMAP_LEVEL                   0x3083 // v1.1
#define EGL_BACK_BUFFER                    0x3084 // v1.1
#define EGL_SINGLE_BUFFER                  0x3085 // v1.2
#define EGL_RENDER_BUFFER                  0x3086 // v1.2
#define EGL_COLORSPACE                     0x3087 // v1.2
#define EGL_VG_COLORSPACE                  0x3087 // v1.3
#define EGL_ALPHA_FORMAT                   0x3088 // v1.2
#define EGL_VG_ALPHA_FORMAT                0x3088 // v1.3
#define EGL_COLORSPACE_sRGB                0x3089 // v1.2
#define EGL_GL_COLORSPACE_SRGB             0x3089 // v1.5
#define EGL_VG_COLORSPACE_sRGB             0x3089 // v1.3
#define EGL_COLORSPACE_LINEAR              0x308A // v1.2
#define EGL_GL_COLORSPACE_LINEAR           0x308A // v1.5
#define EGL_VG_COLORSPACE_LINEAR           0x308A // v1.3
#define EGL_ALPHA_FORMAT_NONPRE            0x308B // v1.2
#define EGL_VG_ALPHA_FORMAT_NONPRE         0x308B // v1.3
#define EGL_ALPHA_FORMAT_PRE               0x308C // v1.2
#define EGL_VG_ALPHA_FORMAT_PRE            0x308C // v1.3
#define EGL_HORIZONTAL_RESOLUTION          0x3090 // v1.2
#define EGL_VERTICAL_RESOLUTION            0x3091 // v1.2
#define EGL_PIXEL_ASPECT_RATIO             0x3092 // v1.2
#define EGL_SWAP_BEHAVIOR                  0x3093 // v1.2
#define EGL_BUFFER_PRESERVED               0x3094 // v1.2
#define EGL_BUFFER_DESTROYED               0x3095 // v1.2
#define EGL_OPENVG_IMAGE                   0x3096 // v1.2
#define EGL_CONTEXT_CLIENT_TYPE            0x3097 // v1.2
#define EGL_MULTISAMPLE_RESOLVE            0x3099 // v1.4
#define EGL_MULTISAMPLE_RESOLVE_DEFAULT    0x309A // v1.4
#define EGL_MULTISAMPLE_RESOLVE_BOX        0x309B // v1.4
#define EGL_CL_EVENT_HANDLE                0x309C // v1.5
#define EGL_GL_COLORSPACE                  0x309D // v1.5
#define EGL_SYNC_PRIOR_COMMANDS_COMPLETE   0x30F0 // v1.5
#define EGL_SYNC_STATUS                    0x30F1 // v1.5
#define EGL_SIGNALED                       0x30F2 // v1.5
#define EGL_UNSIGNALED                     0x30F3 // v1.5
#define EGL_TIMEOUT_EXPIRED                0x30F5 // v1.5
#define EGL_CONDITION_SATISFIED            0x30F6 // v1.5
#define EGL_SYNC_TYPE                      0x30F7 // v1.5
#define EGL_SYNC_CONDITION                 0x30F8 // v1.5
#define EGL_SYNC_FENCE                     0x30F9 // v1.5
#define EGL_SYNC_CL_EVENT                  0x30FE // v1.5
#define EGL_SYNC_CL_EVENT_COMPLETE         0x30FF // v1.5

#define EGL_SYNC_FLUSH_COMMANDS_BIT 0x0001     // v1.5

typedef enum egl_api						 egl_api;
typedef enum egl_config_attrib				 egl_config_attrib;
typedef enum egl_context_attrib				 egl_context_attrib;
typedef enum egl_color_buffer_type			 egl_color_buffer_type;
typedef enum egl_config_caveat				 egl_config_caveat;
typedef enum egl_conformance_flags			 egl_conformance_flags;
typedef enum egl_error						 egl_error;
typedef enum egl_image_attrib				 egl_image_attrib;
typedef enum egl_image_target				 egl_image_target;
typedef enum egl_profile_flags				 egl_profile_flags;
typedef enum egl_query						 egl_query;
typedef enum egl_renderable_type_flags		 egl_renderable_type_flags;
typedef enum egl_reset_notification_strategy egl_reset_notification_strategy;
typedef enum egl_sample_range_hint			 egl_sample_range_hint;
typedef enum egl_surface_type_flags			 egl_surface_type_flags;
typedef enum egl_transparent_type			 egl_transparent_type;
typedef enum egl_window_attrib				 egl_window_attrib;
typedef enum egl_yuv_chroma_horizontal_siting_hint
									  egl_yuv_chroma_horizontal_siting_hint;
typedef enum egl_yuv_color_space_hint egl_yuv_color_space_hint;

typedef struct egl egl;

enum egl_api {
	EGL_API_NONE	  = EGL_NONE,  // v1.0
	EGL_API_OPENGL_ES = 0x30A0,	   // v1.2
	EGL_API_OPENVG	  = 0x30A1,	   // v1.2
	EGL_API_OPENGL	  = 0x30A2,	   // v1.4
};

enum egl_config_attrib {
	EGL_ATTRIB_BUFFER_SIZE			   = 0x3020,	// v1.0
	EGL_CONFIG_ALPHA_SIZE			   = 0x3021,	// v1.0
	EGL_CONFIG_BLUE_SIZE			   = 0x3022,	// v1.0
	EGL_CONFIG_GREEN_SIZE			   = 0x3023,	// v1.0
	EGL_CONFIG_RED_SIZE				   = 0x3024,	// v1.0
	EGL_CONFIG_DEPTH_SIZE			   = 0x3025,	// v1.0
	EGL_CONFIG_STENCIL_SIZE			   = 0x3026,	// v1.0
	EGL_CONFIG_CAVEAT				   = 0x3027,	// v1.0
	EGL_CONFIG_ID					   = 0x3028,	// v1.0
	EGL_CONFIG_LEVEL				   = 0x3029,	// v1.0
	EGL_CONFIG_MAX_PBUFFER_HEIGHT	   = 0x302A,	// v1.0 - get attrib only
	EGL_CONFIG_MAX_PBUFFER_PIXELS	   = 0x302B,	// v1.0 - get attrib only
	EGL_CONFIG_MAX_PBUFFER_WIDTH	   = 0x302C,	// v1.0 - get attrib only
	EGL_CONFIG_NATIVE_RENDERABLE	   = 0x302D,	// v1.0
	EGL_CONFIG_NATIVE_VISUAL_ID		   = 0x302E,	// v1.0 - get attrib only
	EGL_CONFIG_NATIVE_VISUAL_TYPE	   = 0x302F,	// v1.0 - get attrib only
	EGL_CONFIG_SAMPLES				   = 0x3031,	// v1.0
	EGL_CONFIG_SAMPLE_BUFFERS		   = 0x3032,	// v1.0
	EGL_CONFIG_SURFACE_TYPE			   = 0x3033,	// v1.0
	EGL_CONFIG_TRANSPARENT_TYPE		   = 0x3034,	// v1.0
	EGL_CONFIG_TRANSPARENT_BLUE_VALUE  = 0x3035,	// v1.0
	EGL_CONFIG_TRANSPARENT_GREEN_VALUE = 0x3036,	// v1.0
	EGL_CONFIG_TRANSPARENT_RED_VALUE   = 0x3037,	// v1.0
	EGL_CONFIG_NONE					   = EGL_NONE,	// v1.0
	EGL_CONFIG_BIND_TO_TEXTURE_RGB	   = 0x3039,	// v1.1
	EGL_CONFIG_BIND_TO_TEXTURE_RGBA	   = 0x303A,	// v1.1
	EGL_CONFIG_MIN_SWAP_INTERVAL	   = 0x303B,	// v1.1
	EGL_CONFIG_MAX_SWAP_INTERVAL	   = 0x303C,	// v1.1
	EGL_CONFIG_LUMINANCE_SIZE		   = 0x303D,	// v1.2
	EGL_CONFIG_ALPHA_MASK_SIZE		   = 0x303E,	// v1.2
	EGL_CONFIG_COLOR_BUFFER_TYPE	   = 0x303F,	// v1.2
	EGL_CONFIG_RENDERABLE_TYPE		   = 0x3040,	// v1.2
	EGL_CONFIG_MATCH_NATIVE_PIXMAP	   = 0x3041,	// v1.3
	EGL_CONFIG_CONFORMANT			   = 0x3042,	// v1.3
};

enum egl_context_attrib {
	EGL_CONTEXT_NONE							   = EGL_NONE,	// v1.0
	EGL_CONTEXT_CLIENT_VERSION					   = 0x3098,	// v1.3
	EGL_CONTEXT_MAJOR_VERSION					   = 0x3098,	// v1.5
	EGL_CONTEXT_MINOR_VERSION					   = 0x30FB,	// v1.5
	EGL_CONTEXT_OPENGL_PROFILE_MASK				   = 0x30FD,	// v1.5
	EGL_CONTEXT_OPENGL_DEBUG					   = 0x31B0,	// v1.5
	EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE		   = 0x31B1,	// v1.5
	EGL_CONTEXT_OPENGL_ROBUST_ACCESS			   = 0x31B2,	// v1.5
	EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY = 0x31BD,	// v1.5
};

enum egl_color_buffer_type {
	EGL_COLOR_BUFFER_RGB	   = 0x308E,  // v1.2
	EGL_COLOR_BUFFER_LUMINANCE = 0x308F,  // v1.2
};

enum egl_config_caveat {
	EGL_CONFIG_CAVEAT_DONT_CARE		 = EGL_DONT_CARE,  // v1.0
	EGL_CONFIG_CAVEAT_NONE			 = EGL_NONE,	   // v1.0
	EGL_CONFIG_CAVEAT_SLOW			 = 0x3050,		   // v1.0
	EGL_CONFIG_CAVEAT_NON_CONFORMANT = 0x3051,		   // v1.0 - v1.2
};

enum egl_conformance_flags {
	EGL_CONFORMANCE_OPENGL_ES  = 0x0001,  // v1.2
	EGL_CONFORMANCE_OPENVG	   = 0x0002,  // v1.2
	EGL_CONFORMANCE_OPENGL_ES2 = 0x0004,  // v1.3
	EGL_CONFORMANCE_OPENGL	   = 0x0008,  // v1.4
	EGL_CONFORMANCE_OPENGL_ES3 = 0x0040,  // v1.5
};

enum egl_error {
	EGL_ERROR_SUCCESS			  = 0x3000,	 // v1.0
	EGL_ERROR_NOT_INITIALIZED	  = 0x3001,	 // v1.0
	EGL_ERROR_BAD_ACCESS		  = 0x3002,	 // v1.0
	EGL_ERROR_BAD_ALLOC			  = 0x3003,	 // v1.0
	EGL_ERROR_BAD_ATTRIBUTE		  = 0x3004,	 // v1.0
	EGL_ERROR_BAD_CONFIG		  = 0x3005,	 // v1.0
	EGL_ERROR_BAD_CONTEXT		  = 0x3006,	 // v1.0
	EGL_ERROR_BAD_CURRENT_SURFACE = 0x3007,	 // v1.0
	EGL_ERROR_BAD_DISPLAY		  = 0x3008,	 // v1.0
	EGL_ERROR_BAD_MATCH			  = 0x3009,	 // v1.0
	EGL_ERROR_BAD_NATIVE_PIXMAP	  = 0x300A,	 // v1.0
	EGL_ERROR_BAD_NATIVE_WINDOW	  = 0x300B,	 // v1.0
	EGL_ERROR_BAD_PARAMETER		  = 0x300C,	 // v1.0
	EGL_ERROR_BAD_SURFACE		  = 0x300D,	 // v1.0
	EGL_ERROR_CONTEXT_LOST		  = 0x300E,	 // v1.1
};

enum egl_image_attrib {
	EGL_IMAGE_ATTRIB_NONE			 = EGL_NONE,  // v1.0
	EGL_IMAGE_ATTRIB_TEXTURE_LEVEL	 = 0x30BC,	  // v1.5
	EGL_IMAGE_ATTRIB_TEXTURE_ZOFFSET = 0x30BD,	  // v1.5
	EGL_IMAGE_ATTRIB_PRESERVED		 = 0x30D2,	  // v1.5

	// EGL_EXT_image_dma_buf_import
	EGL_IMAGE_ATTRIB_WIDTH							   = EGL_WIDTH,
	EGL_IMAGE_ATTRIB_HEIGHT							   = EGL_HEIGHT,
	EGL_IMAGE_ATTRIB_LINUX_DRM_FOURCC				   = 0x3271,
	EGL_IMAGE_ATTRIB_DMA_BUF_PLANE0_FD				   = 0x3272,
	EGL_IMAGE_ATTRIB_DMA_BUF_PLANE0_OFFSET			   = 0x3273,
	EGL_IMAGE_ATTRIB_DMA_BUF_PLANE0_PITCH			   = 0x3274,
	EGL_IMAGE_ATTRIB_DMA_BUF_PLANE1_FD				   = 0x3275,
	EGL_IMAGE_ATTRIB_DMA_BUF_PLANE1_OFFSET			   = 0x3276,
	EGL_IMAGE_ATTRIB_DMA_BUF_PLANE1_PITCH			   = 0x3277,
	EGL_IMAGE_ATTRIB_DMA_BUF_PLANE2_FD				   = 0x3278,
	EGL_IMAGE_ATTRIB_DMA_BUF_PLANE2_OFFSET			   = 0x3279,
	EGL_IMAGE_ATTRIB_DMA_BUF_PLANE2_PITCH			   = 0x327A,
	EGL_IMAGE_ATTRIB_YUV_COLOR_SPACE_HINT			   = 0x327B,
	EGL_IMAGE_ATTRIB_SAMPLE_RANGE_HINT				   = 0x327C,
	EGL_IMAGE_ATTRIB_YUV_CHROMA_HORIZONTAL_SITING_HINT = 0x327D,
	EGL_IMAGE_ATTRIB_YUV_CHROMA_VERTICAL_SITING_HINT   = 0x327E,

	// EGL_EXT_image_dma_buf_import_modifiers
	EGL_IMAGE_ATTRIB_DMA_BUF_PLANE0_MODIFIER_LO = 0x3443,
	EGL_IMAGE_ATTRIB_DMA_BUF_PLANE0_MODIFIER_HI = 0x3444,
	EGL_IMAGE_ATTRIB_DMA_BUF_PLANE1_MODIFIER_LO = 0x3445,
	EGL_IMAGE_ATTRIB_DMA_BUF_PLANE1_MODIFIER_HI = 0x3446,
	EGL_IMAGE_ATTRIB_DMA_BUF_PLANE2_MODIFIER_LO = 0x3447,
	EGL_IMAGE_ATTRIB_DMA_BUF_PLANE2_MODIFIER_HI = 0x3448,
	EGL_IMAGE_ATTRIB_DMA_BUF_PLANE3_FD			= 0x3440,
	EGL_IMAGE_ATTRIB_DMA_BUF_PLANE3_OFFSET		= 0x3441,
	EGL_IMAGE_ATTRIB_DMA_BUF_PLANE3_PITCH		= 0x3442,
	EGL_IMAGE_ATTRIB_DMA_BUF_PLANE3_MODIFIER_LO = 0x3449,
	EGL_IMAGE_ATTRIB_DMA_BUF_PLANE3_MODIFIER_HI = 0x344A,
};

enum egl_image_target {
	EGL_IMAGE_TARGET_TEXTURE_2D					 = 0x30B1,	// v1.5
	EGL_IMAGE_TARGET_TEXTURE_3D					 = 0x30B2,	// v1.5
	EGL_IMAGE_TARGET_TEXTURE_CUBE_MAP_POSITIVE_X = 0x30B3,	// v1.5
	EGL_IMAGE_TARGET_TEXTURE_CUBE_MAP_NEGATIVE_X = 0x30B4,	// v1.5
	EGL_IMAGE_TARGET_TEXTURE_CUBE_MAP_POSITIVE_Y = 0x30B5,	// v1.5
	EGL_IMAGE_TARGET_TEXTURE_CUBE_MAP_NEGATIVE_Y = 0x30B6,	// v1.5
	EGL_IMAGE_TARGET_TEXTURE_CUBE_MAP_POSITIVE_Z = 0x30B7,	// v1.5
	EGL_IMAGE_TARGET_TEXTURE_CUBE_MAP_NEGATIVE_Z = 0x30B8,	// v1.5
	EGL_IMAGE_TARGET_RENDERBUFFER				 = 0x30B9,	// v1.5

	// EGL_EXT_image_dma_buf_import
	EGL_IMAGE_TARGET_LINUX_DMA_BUF = 0x3270,
};

enum egl_profile_flags {
	EGL_PROFILE_OPENGL_CORE			 = 0x00000001,	// v1.5
	EGL_PROFILE_OPENGL_COMPATIBILITY = 0x00000002,	// v1.5
};

enum egl_query {
	EGL_QUERY_VENDOR	  = 0x3053,	 // v1.0
	EGL_QUERY_VERSION	  = 0x3054,	 // v1.0
	EGL_QUERY_EXTENSIONS  = 0x3055,	 // v1.0
	EGL_QUERY_CLIENT_APIS = 0x308D,	 // v1.2
};

enum egl_renderable_type_flags {
	EGL_RENDERABLE_TYPE_OPENGL_ES  = 0x0001,  // v1.2
	EGL_RENDERABLE_TYPE_OPENVG	   = 0x0002,  // v1.2
	EGL_RENDERABLE_TYPE_OPENGL_ES2 = 0x0004,  // v1.3
	EGL_RENDERABLE_TYPE_OPENGL	   = 0x0008,  // v1.4
	EGL_RENDERABLE_TYPE_OPENGL_ES3 = 0x0040,  // v1.5
};

enum egl_reset_notification_strategy {
	EGL_NO_RESET_NOTIFICATION = 0x31BE,	 // v1.5
	EGL_LOSE_CONTEXT_ON_RESET = 0x31BF,	 // v1.5
};

enum egl_sample_range_hint {
	// EGL_EXT_image_dma_buf_import
	EGL_SAMPLE_RANGE_YUV_FULL	= 0x3282,
	EGL_SAMPLE_RANGE_YUV_NARROW = 0x3283,
};

enum egl_surface_type_flags {
	EGL_SURFACE_TYPE_PBUFFER				 = 0x0001,	// v1.0
	EGL_SURFACE_TYPE_PIXMAP					 = 0x0002,	// v1.0
	EGL_SURFACE_TYPE_WINDOW					 = 0x0004,	// v1.0
	EGL_SURFACE_TYPE_VG_COLORSPACE_LINEAR	 = 0x0020,	// v1.3
	EGL_SURFACE_TYPE_VG_ALPHA_FORMAT_PRE	 = 0x0040,	// v1.3
	EGL_SURFACE_TYPE_MULTISAMPLE_RESOLVE_BOX = 0x0200,	// v1.4
	EGL_SURFACE_TYPE_SWAP_BEHAVIOR_PRESERVED = 0x0400,	// v1.4
};

enum egl_transparent_type {
	EGL_TRANSPARENT_NONE = EGL_NONE,  // v1.0
	EGL_TRANSPARENT_RGB	 = 0x3052,	  // v1.0
};

enum egl_window_attrib {
	EGL_WINDOW_NONE = EGL_NONE	// v1.0
};

enum egl_yuv_chroma_horizontal_siting_hint {
	// EGL_EXT_image_dma_buf_import
	EGL_YUV_CHROMA_HORIZONTAL_SITING_0	 = 0x3284,
	EGL_YUV_CHROMA_HORIZONTAL_SITING_0_5 = 0x3285,
};

enum egl_yuv_color_space_hint {
	// EGL_EXT_image_dma_buf_import
	EGL_YUV_COLOR_SPACE_ITU_REC601	= 0x327F,
	EGL_YUV_COLOR_SPACE_ITU_REC709	= 0x3280,
	EGL_YUV_COLOR_SPACE_ITU_REC2020 = 0x3281,
};

struct egl {
	gbm *Gbm;

	egl_image EglImages[2];
	u32		  FramebufferIds[2];
	u32		  TextureIds[2];

	egl_display Display;
	egl_context Context;
	egl_config	Config;
};

#define EGL_FUNCS \
	INTERN(b08, Egl_Init, gbm *Gbm, heap *Heap, egl *EglOut) \
	\
	IMPORT(egl_boolean, Egl, eglBindAPI,             Egl_BindApi,             egl_api Api) \
	IMPORT(egl_boolean, Egl, eglChooseConfig,        Egl_ChooseConfig,        egl_display Display, egl_int *Attribs, egl_config *ConfigsOut, egl_int ConfigSize, egl_int *ConfigCountOut) \
	IMPORT(egl_context, Egl, eglCreateContext,       Egl_CreateContext,       egl_display Display, egl_config Config, egl_context ShareContext, egl_int *Attribs) \
	IMPORT(egl_image,   Egl, eglCreateImage,         Egl_CreateImage,         egl_display Display, egl_context Context, egl_image_target Target, egl_client_buffer Buffer, egl_attrib *Attribs) \
	IMPORT(egl_surface, Egl, eglCreateWindowSurface, Egl_CreateWindowSurface, egl_display Display, egl_config Config, egl_native_window_type NativeWindow, egl_int *Attribs) \
	IMPORT(egl_boolean, Egl, eglDestroyContext,      Egl_DestroyContext,      egl_display Display, egl_context Context) \
	IMPORT(egl_boolean, Egl, eglDestroyImage,        Egl_DestroyImage,        egl_display Display, egl_image Image) \
	IMPORT(egl_boolean, Egl, eglDestroySurface,      Egl_DestroySurface,      egl_display Display, egl_surface Surface) \
	IMPORT(egl_boolean, Egl, eglGetConfigAttrib,     Egl_GetConfigAttrib,     egl_display Display, egl_config Config, egl_config_attrib Attrib, egl_int *ValueOut) \
	IMPORT(egl_boolean, Egl, eglGetConfigs,          Egl_GetConfigs,          egl_display Display, egl_config *ConfigsOut, egl_int ConfigSize, egl_int *ConfigCountOut) \
	IMPORT(egl_display, Egl, eglGetDisplay,          Egl_GetDisplay,          egl_native_display_type Display) \
	IMPORT(egl_int,     Egl, eglGetError,            Egl_GetError,            void) \
	IMPORT(egl_boolean, Egl, eglInitialize,          Egl_Initialize,          egl_display Display, egl_int *Major, egl_int *Minor) \
	IMPORT(egl_boolean, Egl, eglMakeCurrent,         Egl_MakeCurrent,         egl_display Display, egl_surface Draw, egl_surface Read, egl_context Context) \
	IMPORT(egl_api,     Egl, eglQueryAPI,            Egl_QueryApi,            void) \
	IMPORT(egl_display, Egl, eglQueryString,         Egl_QueryString,         egl_display Display, egl_int Name) \
	IMPORT(egl_boolean, Egl, eglSwapBuffers,         Egl_SwapBuffers,         egl_display Display, egl_surface Surface) \
	IMPORT(egl_boolean, Egl, eglTerminate,           Egl_Terminate,           egl_display Display) \
	//

#endif

#ifdef INCLUDE_SOURCE

internal b08
Egl_Init(gbm *Gbm, heap *Heap, egl *EglOut)
{
	egl_config *Configs	   = NULL;
	egl_surface EglSurface = EGL_NO_SURFACE;
	egl_display EglDisplay = Egl_GetDisplay(Gbm->Device);

	// Initialize
	egl_int Major, Minor;
	if (!Egl_Initialize(EglDisplay, &Major, &Minor)) {
		FPrintL("Failed to initialize egl: code %#x\n", Egl_GetError());
		goto error;
	}

	FPrintL("Initialized egl v%d.%d\n", Major, Minor);
	FPrintL(
		"- Vendor: %s\n",
		CString(Egl_QueryString(EglDisplay, EGL_QUERY_VENDOR))
	);
	FPrintL(
		"- Full Version: %s\n",
		CString(Egl_QueryString(EglDisplay, EGL_QUERY_VERSION))
	);

	if (Major == 1 && Minor < 4) {
		FPrintL("Egl version is too old! Must be at least v1.4\n");
		goto error;
	}

	// Select Client API
	egl_api Api = Egl_QueryApi();
	string	ClientApis =
		CString(Egl_QueryString(EglDisplay, EGL_QUERY_CLIENT_APIS));
	FPrintL("- Client APIs:\n");
	string Cursor = ClientApis;
	while (Cursor.Length) {
		string ClientApi = String_SplitLeftByCodepoint(&Cursor, ' ');
		FPrintL("  - %s\n", ClientApi);
		if (String_Cmp(ClientApi, CStringL("OpenGL")) == 0) {
			if (Egl_BindApi(EGL_API_OPENGL)) Api = EGL_API_OPENGL;
		}
	}

	// Find extensions
	b08	   HasEglImageDmaBufImport = FALSE;
	b08	   HasEglImageStorage	   = FALSE;
	string Extensions =
		CString(Egl_QueryString(EglDisplay, EGL_QUERY_EXTENSIONS));
	FPrintL("- Extensions:\n");
	Cursor = Extensions;
	while (Cursor.Length) {
		string Extension = String_SplitLeftByCodepoint(&Cursor, ' ');
		FPrintL("  - %s\n", Extension);

		if (String_Cmp(Extension, CStringL("EGL_EXT_image_dma_buf_import"))
			== 0)
			HasEglImageDmaBufImport = TRUE;

		else if (String_Cmp(Extension, CStringL("EXT_EGL_image_storage")) == 0)
			HasEglImageStorage = TRUE;
	}

	if (Api != EGL_API_OPENGL) {
		FPrintL("Failed to select OpenGL as EGL's client API\n");
		goto error;
	}
	FPrintL("Selected EGL OpenGL client api\n");

	if (!HasEglImageDmaBufImport) {
		FPrintL(
			"Required extension EGL_EXT_image_dma_buf_import is not present\n"
		);
		goto error;
	}

	if (!HasEglImageStorage) {
		FPrintL("Required extension EGL_EXT_image_storage is not present\n");
		goto error;
	}

	// Select Config
	egl_int ConfigCount = 0;
	if (!Egl_GetConfigs(EglDisplay, NULL, 0, &ConfigCount)) {
		FPrintL("Failed to get egl config count: code %#x\n", Egl_GetError());
		goto error;
	}
	if (ConfigCount < 1) {
		FPrintL("No egl configs were found\n");
		goto error;
	}

	Configs = Heap_AllocateA(Heap, ConfigCount * sizeof(egl_config));
	if (!Configs) {
		FPrintL("Failed to allocate space for egl configs\n");
		goto error;
	}

	// clang-format off
	egl_int ConfigAttribs[] = {
		EGL_CONFIG_RED_SIZE,          1,
		EGL_CONFIG_GREEN_SIZE,        1,
		EGL_CONFIG_BLUE_SIZE,         1,
		EGL_CONFIG_ALPHA_SIZE,        0,
		EGL_CONFIG_COLOR_BUFFER_TYPE, EGL_COLOR_BUFFER_RGB,

		EGL_CONFIG_DEPTH_SIZE,        24,
		EGL_CONFIG_STENCIL_SIZE,      8,

		EGL_CONFIG_SAMPLE_BUFFERS,    1,
		EGL_CONFIG_SAMPLES,           4,

		EGL_CONFIG_SURFACE_TYPE,      EGL_SURFACE_TYPE_WINDOW,
		EGL_CONFIG_CONFORMANT,        EGL_CONFORMANCE_OPENGL,
		EGL_CONFIG_RENDERABLE_TYPE,   EGL_RENDERABLE_TYPE_OPENGL,
		EGL_CONFIG_NONE
	};
	// clang-format on

	egl_int		MatchCount = 0;
	egl_boolean Success	   = Egl_ChooseConfig(
		EglDisplay,
		ConfigAttribs,
		Configs,
		ConfigCount,
		&MatchCount
	);
	if (!Success) {
		FPrintL(
			"Failed to find applicable egl configs: code %#x\n",
			Egl_GetError()
		);
		goto error;
	}

	egl_int SelectedIndex = -1;
	for (egl_int I = 0; I < MatchCount; I++) {
		egl_int Value;
		Success = Egl_GetConfigAttrib(
			EglDisplay,
			Configs[I],
			EGL_CONFIG_NATIVE_VISUAL_ID,
			&Value
		);
		if (!Success) {
			FPrintL(
				"Failed to query egl config attrib: code %#x\n",
				Egl_GetError()
			);
			goto error;
		}

		if (Value == Gbm->Format) {
			SelectedIndex = I;
			break;
		}
	}
	if (SelectedIndex == -1) {
		FPrintL("No applicable egl configs were found\n");
		goto error;
	}

	egl_config EglConfig = Configs[SelectedIndex];
	Heap_FreeA(Configs);

#if defined(_DEBUG)
	b08 IsDebug = TRUE;
#else
	b08 IsDebug = FALSE;
#endif

	// Create context
	// clang-format off
	egl_int ContextAttribs[] = {
		EGL_CONTEXT_MAJOR_VERSION,             4,
		EGL_CONTEXT_MINOR_VERSION,             6,
		EGL_CONTEXT_OPENGL_PROFILE_MASK,       EGL_PROFILE_OPENGL_CORE,
		EGL_CONTEXT_OPENGL_DEBUG,              IsDebug,
		EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE, TRUE,
		EGL_CONTEXT_NONE,
	};
	// clang-format on
	egl_context EglContext = Egl_CreateContext(
		EglDisplay,
		EglConfig,
		EGL_NO_CONTEXT,
		ContextAttribs
	);
	if (EglContext == EGL_NO_CONTEXT) {
		FPrintL("Failed to create egl context: code %#x\n", Egl_GetError());
		goto error;
	}

	if (!Egl_MakeCurrent(
			EglDisplay,
			EGL_NO_SURFACE,
			EGL_NO_SURFACE,
			EglContext
		))
	{
		FPrintL(
			"Failed to make egl context current: code %#x\n",
			Egl_GetError()
		);
		goto error;
	}

	OpenGL_GenFramebuffers(2, EglOut->FramebufferIds);
	OpenGL_GenTextures(2, EglOut->TextureIds);

	egl_image EglImages[2] = { 0 };
	for (usize I = 0; I < 2; I++) {
		gbm_bo *Bo = Gbm->BufferObjects[I];

		u64 Modifier = Gbm_BoGetModifier(Bo);

		// clang-format off
		egl_attrib ImageAttribs[17] = {
			EGL_IMAGE_ATTRIB_WIDTH,                 Gbm_BoGetWidth(Bo),
			EGL_IMAGE_ATTRIB_HEIGHT,                Gbm_BoGetHeight(Bo),
			EGL_IMAGE_ATTRIB_LINUX_DRM_FOURCC,      Gbm_BoGetFormat(Bo),
			EGL_IMAGE_ATTRIB_DMA_BUF_PLANE0_FD,     Gbm_BoGetFd(Bo),
			EGL_IMAGE_ATTRIB_DMA_BUF_PLANE0_OFFSET, 0,
			EGL_IMAGE_ATTRIB_DMA_BUF_PLANE0_PITCH,  Gbm_BoGetStride(Bo),
			EGL_IMAGE_ATTRIB_NONE,                  EGL_NONE,
			EGL_IMAGE_ATTRIB_NONE,                  EGL_NONE,
			EGL_IMAGE_ATTRIB_NONE
		};
		// clang-format on

		if (Modifier != DRM_FORMAT_MOD_INVALID) {
			ImageAttribs[12] = EGL_IMAGE_ATTRIB_DMA_BUF_PLANE0_MODIFIER_LO;
			ImageAttribs[13] = Modifier & 0xFFFFFFFF;
			ImageAttribs[14] = EGL_IMAGE_ATTRIB_DMA_BUF_PLANE0_MODIFIER_HI;
			ImageAttribs[15] = Modifier >> 32;
		}

		EglOut->EglImages[I] = Egl_CreateImage(
			EglDisplay,
			EGL_NO_CONTEXT,
			EGL_IMAGE_TARGET_LINUX_DMA_BUF,
			NULL,
			ImageAttribs
		);

		if (!EglOut->EglImages[I]) {
			FPrintL(
				"Failed to create egl image %d: code %#x\n",
				I,
				Egl_GetError()
			);
			goto error;
		}

		OpenGL_BindFramebuffer(GL_FRAMEBUFFER, EglOut->FramebufferIds[I]);
		OpenGL_BindTexture(GL_TEXTURE_2D, EglOut->TextureIds[I]);
		OpenGL_EGLImageTargetTexStorageEXT(
			GL_TEXTURE_2D,
			EglOut->EglImages[I],
			NULL
		);
		OpenGL_FramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D,
			EglOut->TextureIds[I],
			0
		);

		u32 Status = OpenGL_CheckFramebufferStatus(GL_FRAMEBUFFER);
		if (Status != GL_FRAMEBUFFER_COMPLETE) {
			FPrintL(
				"Failed to initialize framebuffer %d: code %#x\n",
				I,
				Status
			);
			goto error;
		}
	}

	FPrintL("Successfully initialized egl!\n");
	EglOut->Gbm		= Gbm;
	EglOut->Display = EglDisplay;
	EglOut->Context = EglContext;
	EglOut->Config	= EglConfig;
	Mem_Cpy(EglOut->EglImages, EglImages, sizeof(EglImages));
	return TRUE;

error:
	if (EglImages[1]) Egl_DestroyImage(EglDisplay, EglImages[1]);
	if (EglImages[0]) Egl_DestroyImage(EglDisplay, EglImages[0]);
	if (EglContext != EGL_NO_CONTEXT)
		Egl_DestroyContext(EglDisplay, EglContext);
	if (Configs) Heap_FreeA(Configs);
	Egl_Terminate(EglDisplay);
	return FALSE;
}

#endif
