/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                            *
*  Author: Aria Seiler                                                       *
*                                                                            *
*  This program is in the public domain. There is no implied warranty, so    *
*  use it at your own risk.                                                  *
*                                                                            *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef INCLUDE_HEADER

typedef struct gbm_device  gbm_device;
typedef struct gbm_surface gbm_surface;
typedef struct gbm_bo	   gbm_bo;

typedef enum gbm_bo_flags : u32 {
	GBM_BO_USE_SCANOUT	 = (1 << 0),
	GBM_BO_USE_CURSOR	 = (1 << 1),
	GBM_BO_USE_RENDERING = (1 << 2),
	GBM_BO_USE_WRITE	 = (1 << 3),
	GBM_BO_USE_LINEAR	 = (1 << 4),
} gbm_bo_flags;

typedef struct gbm {
	gbm_device	*Device;
	gbm_surface *Surface;

	u32		   Width;
	u32		   Height;
	drm_format Format;

	gbm_bo *BufferObjects[2];
} gbm;

#define GBM_FUNCS \
	INTERN(b08, Gbm_Init, s32 DrmFd, u32 Width, u32 Height, drm_format Format, drm_format_modifier Modifier, gbm *GbmOut) \
	\
	IMPORT(gbm_bo*,      Gbm, gbm_bo_create,                Gbm_BoCreate,              gbm_device *GbmDevice, u32 Width, u32 Height, drm_format Format, gbm_bo_flags Flags) \
	IMPORT(gbm_bo*,      Gbm, gbm_bo_create_with_modifiers, Gbm_BoCreateWithModifiers, gbm_device *GbmDevice, u32 Width, u32 Height, drm_format Format, drm_format_modifier *Modifiers, u32 ModifierCount) \
	IMPORT(s32,          Gbm, gbm_bo_get_fd,                Gbm_BoGetFd,               gbm_bo *BufferObject) \
	IMPORT(drm_format,   Gbm, gbm_bo_get_format,            Gbm_BoGetFormat,           gbm_bo *BufferObject) \
	IMPORT(u32,          Gbm, gbm_bo_get_height,            Gbm_BoGetHeight,           gbm_bo *BufferObject) \
	IMPORT(u64,          Gbm, gbm_bo_get_modifier,          Gbm_BoGetModifier,         gbm_bo *BufferObject) \
	IMPORT(u32,          Gbm, gbm_bo_get_stride,            Gbm_BoGetStride,           gbm_bo *BufferObject) \
	IMPORT(u32,          Gbm, gbm_bo_get_width,             Gbm_BoGetWidth,            gbm_bo *BufferObject) \
	IMPORT(gbm_device*,  Gbm, gbm_create_device,            Gbm_CreateDevice,          s32 DrmFd) \
	IMPORT(gbm_surface*, Gbm, gbm_surface_create,           Gbm_SurfaceCreate,         gbm_device *GbmDevice, u32 Width, u32 Height, drm_format Format, u32 Flags) \
	//

#endif

#ifdef INCLUDE_SOURCE

extern s32 *__errno_location(void);

internal sys_errno
Gbm_GetError(void)
{
	return *__errno_location();
}

internal b08
Gbm_Init(
	s32					DrmFd,
	u32					Width,
	u32					Height,
	drm_format			Format,
	drm_format_modifier Modifier,
	gbm				   *GbmOut
)
{
	if (!GbmOut || !DrmFd) return FALSE;

	GbmOut->Device = Gbm_CreateDevice(DrmFd);
	if (!GbmOut->Device) {
		FPrintL("Failed to create gbm device: code %m\n", Gbm_GetError());
		return FALSE;
	}

	GbmOut->Width  = Width;
	GbmOut->Height = Height;
	GbmOut->Format = Format;

	for (usize I = 0; I < 2; I++) {
		gbm_bo *BufferObject = NULL;

		if (Modifier != DRM_FORMAT_MOD_INVALID)
			BufferObject = Gbm_BoCreateWithModifiers(
				GbmOut->Device,
				Width,
				Height,
				Format,
				&Modifier,
				1
			);

		if (!BufferObject)
			BufferObject = Gbm_BoCreate(
				GbmOut->Device,
				Width,
				Height,
				Format,
				GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING
			);

		if (!BufferObject) {
			FPrintL(
				"Failed to create gbm buffer object: code %m\n",
				Gbm_GetError()
			);
			return FALSE;
		}
		GbmOut->BufferObjects[I] = BufferObject;
	}

	FPrintL("Successfully initialized gbm!\n");
	return TRUE;
}

#endif
