/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                            *
*  Author: Aria Seiler                                                       *
*                                                                            *
*  This program is in the public domain. There is no implied warranty, so    *
*  use it at your own risk.                                                  *
*                                                                            *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef INCLUDE_HEADER

#define DRM_DEV_MAJOR 226

typedef enum drm_node_type {
	DRM_NODE_TYPE_PRIMARY = 0,	// Display & graphics control
	DRM_NODE_TYPE_CONTROL = 1,
	DRM_NODE_TYPE_RENDER  = 2,	// Non-display compute
	DRM_NODE_TYPE_COUNT
} drm_node_type;

#define DRM_IOC_READ 2U
#define DRM_IOC_SIZEBITS 14

#define DRM_IOC(MODE, BASE, ID, SIZE) \
	((MODE << (16 + DRM_IOC_SIZEBITS)) | (SIZE << 16) | (BASE << 8) | ID)

#define DRM_IOR(ID, TYPE) DRM_IOC(DRM_IOC_READ, DRM_IOCTL_BASE, ID, sizeof(TYPE))

#define DRM_IOCTL_BASE 'd'
#define DRM_IOCTL_GET_MAGIC DRM_IOR(0x02, u32)

#endif

#ifdef INCLUDE_SOURCE

internal s32
Drm_IoCtl(s32 Fd, usize Op, vptr Data)
{
	s32 Result;
	do {
		Result = Sys_IoCtl(Fd, Op, Data);
	} while (Result == SYS_EINTR || Result == SYS_EAGAIN);

	return Result;
}

internal s32
Drm_OpenDriver(string Name)
{
	return Sys_Open(Name.Text, SYS_OPEN_READWRITE, 0);
}

internal s32
Drm_CloseDriver(s32 Fd)
{
	return Sys_Close(Fd);
}

internal drm_node_type
Drm_GetNodeType(s32 Fd)
{
	sys_stat Stat;
	s32		 Result = Sys_FileStat(Fd, &Stat);
	if (Result < 0) return Result;

	s32 Major = SYS_DEV_MAJOR(Stat.SpecialDeviceId);
	s32 Minor = SYS_DEV_MINOR(Stat.SpecialDeviceId);

	if (Major != DRM_DEV_MAJOR) return SYS_EINVAL;

	drm_node_type NodeType = Minor >> 6;
	if (NodeType >= 0 && NodeType < DRM_NODE_TYPE_COUNT) return NodeType;

	return SYS_ENODEV;
}

internal s32
Drm_GetMagicNumber(s32 Fd, u32 *MagicOut)
{
	return Drm_IoCtl(Fd, DRM_IOCTL_GET_MAGIC, MagicOut);
}

#endif
