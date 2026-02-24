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
typedef struct gbm		   gbm;

typedef enum gbm_bo_flags gbm_bo_flags;

enum gbm_bo_flags {
	GBM_BO_USE_SCANOUT	 = (1 << 0),
	GBM_BO_USE_CURSOR	 = (1 << 1),
	GBM_BO_USE_RENDERING = (1 << 2),
	GBM_BO_USE_WRITE	 = (1 << 3),
	GBM_BO_USE_LINEAR	 = (1 << 4),
};

struct gbm {
	gbm_device	*Device;
	gbm_surface *Surface;

	u32		   Width;
	u32		   Height;
	drm_format Format;
};

#define GBM_FUNCS \
	INTERN(b08, Gbm_Init, s32 DrmFd, u32 Width, u32 Height, drm_format Format, gbm *GbmOut) \
	\
	IMPORT(gbm_device*, Gbm, gbm_create_device, Gbm_CreateDevice, s32 DrmFd) \
	IMPORT(gbm_surface*, Gbm, gbm_surface_create, Gbm_SurfaceCreate, gbm_device *GbmDevice, u32 Width, u32 Height, drm_format Format, u32 Flags) \
	//

#endif

#ifdef INCLUDE_SOURCE

internal b08
Gbm_Init(s32 DrmFd, u32 Width, u32 Height, drm_format Format, gbm *GbmOut)
{
	if (!GbmOut || !DrmFd) return FALSE;

	GbmOut->Device = Gbm_CreateDevice(DrmFd);
	if (!GbmOut->Device) {
		FPrintL("Failed to create gbm device\n");
		return FALSE;
	}

	GbmOut->Width  = Width;
	GbmOut->Height = Height;
	GbmOut->Format = Format;

	GbmOut->Surface = Gbm_SurfaceCreate(
		GbmOut->Device,
		Width,
		Height,
		Format,
		GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING
	);
	if (!GbmOut->Surface) {
		FPrintL("Failed to create gbm surface\n");
		return FALSE;
	}

	FPrintL("Successfully initialized gbm!\n");
	return TRUE;
}

#endif
