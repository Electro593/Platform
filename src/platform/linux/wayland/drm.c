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

#define DRM_FOURCC_CODE(A, B, C, D) ((u32)(A) | ((u32)(B) << 8) | ((u32)(C) << 16) | ((u32)(D) << 24))

typedef enum drm_format {
	DRM_FORMAT_XRGB8888	   = DRM_FOURCC_CODE('X', 'R', '2', '4'),
	DRM_FORMAT_XBGR8888	   = DRM_FOURCC_CODE('X', 'B', '2', '4'),
	DRM_FORMAT_ARGB8888	   = DRM_FOURCC_CODE('A', 'R', '2', '4'),
	DRM_FORMAT_RGBX1010102 = DRM_FOURCC_CODE('R', 'X', '3', '0'),
} drm_format;

typedef enum drm_format_modifier : u64 {
	DRM_FORMAT_MOD_LINEAR  = 0,
	DRM_FORMAT_MOD_INVALID = 0x00FFFFFFFFFFFFFFull,
} drm_format_modifier;

typedef enum drm_node_type {
	DRM_NODE_TYPE_PRIMARY = 0,	// Display & graphics control
	DRM_NODE_TYPE_CONTROL = 1,
	DRM_NODE_TYPE_RENDER  = 2,	// Non-display compute
	DRM_NODE_TYPE_COUNT
} drm_node_type;

#define DRM_IOC_READ 2u
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
	} while (Result == -SYS_EINTR || Result == -SYS_EAGAIN);

	return Result;
}

internal s32
Drm_OpenDriver(string Name)
{
	s32 Fd = Sys_Open(Name.Text, SYS_OPEN_READWRITE | SYS_OPEN_CLOEXEC, 0);
	if (Fd < 0) return Fd;

	sys_stat Stat;
	s32		 Result = Sys_FileStat(Fd, &Stat);
	if (Result < 0 || !SYS_STAT_MODE_IS_CHARACTER(Stat.Mode)) {
		Sys_Close(Fd);
		return -SYS_ENODEV;
	}

	return Fd;
}

internal s32
Drm_CloseDriver(s32 Fd)
{ return Sys_Close(Fd); }

internal drm_node_type
Drm_GetNodeType(s32 Fd)
{
	sys_stat Stat;
	s32		 Result = Sys_FileStat(Fd, &Stat);
	if (Result < 0) return Result;

	s32 Major = SYS_DEV_MAJOR(Stat.SpecialDeviceId);
	s32 Minor = SYS_DEV_MINOR(Stat.SpecialDeviceId);

	if (Major != DRM_DEV_MAJOR) return -SYS_EINVAL;

	drm_node_type NodeType = Minor >> 6;
	if (NodeType >= 0 && NodeType < DRM_NODE_TYPE_COUNT) return NodeType;

	return -SYS_ENODEV;
}

internal s32
Drm_GetMagicNumber(s32 Fd, u32 *MagicOut)
{ return Drm_IoCtl(Fd, DRM_IOCTL_GET_MAGIC, MagicOut); }

internal s32
Drm_OpenRenderNodeFromDevice(sys_dev DeviceId)
{
	u32 Major = SYS_DEV_MAJOR(DeviceId);
	u32 Minor = SYS_DEV_MINOR(DeviceId);

	string PathName = FNStringL("/sys/dev/char/%d:%d/device/drm", Major, Minor);
	s32	   PathFd	= Sys_Open(
		PathName.Text,
		SYS_OPEN_READONLY | SYS_OPEN_DIRECTORY | SYS_OPEN_CLOEXEC,
		0
	);
	if (PathFd < 0) return PathFd;
	ssize FdResult = 0;

	sys_dirent64  DirEntries[16];
	sys_dirent64 *Entry;
	while (TRUE) {
		FdResult =
			Sys_GetDirEnts64(PathFd, DirEntries, ARRAY_COUNT(DirEntries));
		if (FdResult < 0) break;
		if (FdResult == 0) {
			FdResult = -SYS_ENODEV;
			break;
		}

		Entry = DirEntries;
		while (FdResult > 0) {
			string EntryName = CString(Entry->Name);
			if (String_BeginsWith(EntryName, CStringL("renderD"))) {
				string Path = FNStringL("/dev/dri/%s", EntryName);
				FdResult	= Drm_OpenDriver(Path);
				if (FdResult >= 0) break;
			}

			FdResult -= Entry->EntrySize;
			Entry	  = (vptr) ((usize) Entry + Entry->EntrySize);
		}
	}

	Sys_Close(PathFd);
	return FdResult;
}

#endif
