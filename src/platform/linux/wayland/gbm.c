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

typedef enum gbm_bo_transfer_flags : u32 {
	GBM_BO_TRANSFER_READ	   = (1 << 0),
	GBM_BO_TRANSFER_WRITE	   = (1 << 1),
	GBM_BO_TRANSFER_READ_WRITE = (GBM_BO_TRANSFER_READ | GBM_BO_TRANSFER_WRITE),
} gbm_bo_transfer_flags;

typedef struct gbm {
	gbm_device *Device;

	u32		   Width;
	u32		   Height;
	drm_format Format;

	gbm_bo *BufferObjects[2];
} gbm;

#define GBM_FUNCS \
	INTERN(b08,  Gbm_Init,   s32 DrmFd, u32 Width, u32 Height, drm_format Format, drm_format_modifier Modifier, gbm *GbmOut) \
	INTERN(void, Gbm_Deinit, gbm *Gbm) \
	\
	IMPORT(gbm_bo*,      Gbm, gbm_bo_create,                 Gbm_BoCreate,               gbm_device *GbmDevice, u32 Width, u32 Height, drm_format Format, gbm_bo_flags Flags) \
	IMPORT(gbm_bo*,      Gbm, gbm_bo_create_with_modifiers,  Gbm_BoCreateWithModifiers,  gbm_device *GbmDevice, u32 Width, u32 Height, drm_format Format, drm_format_modifier *Modifiers, u32 ModifierCount) \
	IMPORT(gbm_bo*,      Gbm, gbm_bo_create_with_modifiers2, Gbm_BoCreateWithModifiers2, gbm_device *GbmDevice, u32 Width, u32 Height, drm_format Format, drm_format_modifier *Modifiers, u32 ModifierCount, gbm_bo_flags Flags) \
	IMPORT(void,         Gbm, gbm_bo_destroy,                Gbm_BoDestroy,              gbm_bo *BufferObject) \
	IMPORT(s32,          Gbm, gbm_bo_get_fd,                 Gbm_BoGetFd,                gbm_bo *BufferObject) \
	IMPORT(s32,          Gbm, gbm_bo_get_fd_for_plane,       Gbm_BoGetFdForPlane,        gbm_bo *BufferObject, s32 Plane) \
	IMPORT(drm_format,   Gbm, gbm_bo_get_format,             Gbm_BoGetFormat,            gbm_bo *BufferObject) \
	IMPORT(u32,          Gbm, gbm_bo_get_height,             Gbm_BoGetHeight,            gbm_bo *BufferObject) \
	IMPORT(u64,          Gbm, gbm_bo_get_modifier,           Gbm_BoGetModifier,          gbm_bo *BufferObject) \
	IMPORT(u32,          Gbm, gbm_bo_get_offset,             Gbm_BoGetOffset,            gbm_bo *BufferObject, s32 Plane) \
	IMPORT(s32,          Gbm, gbm_bo_get_plane_count,        Gbm_BoGetPlaneCount,        gbm_bo *BufferObject) \
	IMPORT(u32,          Gbm, gbm_bo_get_stride,             Gbm_BoGetStride,            gbm_bo *BufferObject) \
	IMPORT(u32,          Gbm, gbm_bo_get_stride_for_plane,   Gbm_BoGetStrideForPlane,    gbm_bo *BufferObject, s32 Plane) \
	IMPORT(u32,          Gbm, gbm_bo_get_width,              Gbm_BoGetWidth,             gbm_bo *BufferObject) \
	IMPORT(vptr,         Gbm, gbm_bo_map,                    Gbm_BoMap,                  gbm_bo *BufferObject, u32 X, u32 Y, u32 Width, u32 Height, gbm_bo_transfer_flags Flags, u32 *Stride, vptr *MapData) \
	IMPORT(gbm_device*,  Gbm, gbm_create_device,             Gbm_CreateDevice,           s32 DrmFd) \
	IMPORT(void,         Gbm, gbm_device_destroy,            Gbm_DeviceDestroy,          gbm_device *Device) \
	IMPORT(gbm_surface*, Gbm, gbm_surface_create,            Gbm_SurfaceCreate,          gbm_device *Device, u32 Width, u32 Height, drm_format Format, u32 Flags) \
	IMPORT(void,         Gbm, gbm_surface_destroy,           Gbm_SurfaceDestroy,         gbm_surface *Surface) \
	//

#endif

#ifdef INCLUDE_SOURCE

extern s32 *__errno_location(void);

internal sys_errno
Gbm_GetError(void)
{ return *__errno_location(); }

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
	if (!GbmOut || DrmFd < 0) return FALSE;

	gbm Gbm = {
		.Width	= Width,
		.Height = Height,
		.Format = Format,
	};

	Gbm.Device = Gbm_CreateDevice(DrmFd);
	if (!Gbm.Device) {
		FPrintL("Failed to create gbm device: code %m\n", Gbm_GetError());
		goto error;
	}

	ARRAY_FOREACH_PTR (Gbm.BufferObjects) {
		if (Modifier != DRM_FORMAT_MOD_INVALID) {
			if (Gbm_BoCreateWithModifiers2)
				*It = Gbm_BoCreateWithModifiers2(
					Gbm.Device,
					Width,
					Height,
					Format,
					&Modifier,
					1,
					GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING
				);

			if (!*It)
				*It = Gbm_BoCreateWithModifiers(
					Gbm.Device,
					Width,
					Height,
					Format,
					&Modifier,
					1
				);
		} else {
			*It = Gbm_BoCreate(
				Gbm.Device,
				Width,
				Height,
				Format,
				GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING
			);
		}

		if (!*It) {
			FPrintL(
				"Failed to create gbm buffer object: code %m\n",
				Gbm_GetError()
			);
			goto error;
		}
	}

	FPrintL("Successfully initialized gbm!\n");
	*GbmOut = Gbm;
	return TRUE;

error:
	Gbm_Deinit(&Gbm);
	return FALSE;
}

internal void
Gbm_Deinit(gbm *Gbm)
{
	if (!Gbm) return;

	ARRAY_FOREACH (Gbm->BufferObjects)
		if (It) Gbm_BoDestroy(It);

	if (Gbm->Device) Gbm_DeviceDestroy(Gbm->Device);

	Mem_Set(Gbm, 0, sizeof(gbm));
}

#endif
