/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
**                                                                         **
**  Author: Aria Seiler                                                    **
**                                                                         **
**  This program is in the public domain. There is no implied warranty,    **
**  so use it at your own risk.                                            **
**                                                                         **
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if _X64

#define SYS_FILE_NONE (u32)(-1)
#define SYS_FILE_IN   (u32)( 0)
#define SYS_FILE_OUT  (u32)( 1)
#define SYS_FILE_ERR  (u32)( 2)

#define SYS_PROT_NONE  0x0
#define SYS_PROT_READ  0x1
#define SYS_PROT_WRITE 0x2
#define SYS_PROT_EXEC  0x4

#define SYS_SEEK_BEGINNING 0
#define SYS_SEEK_CURRENT   1
#define SYS_SEEK_END       2

#define SYS_MAP_FAILED        ((void*) -1)
#define SYS_MAP_SHARED        0x0000001
#define SYS_MAP_PRIVATE       0x0000002
#define SYS_MAP_FIXED         0x0000010
#define SYS_MAP_ANONYMOUS     0x0000020
#define SYS_MAP_32BIT         0x0000040
#define SYS_MAP_GROWSDOWN     0x0000100
#define SYS_MAP_DENYWRITE     0x0000800
#define SYS_MAP_EXECUTABLE    0x0001000
#define SYS_MAP_LOCKED        0x0002000
#define SYS_MAP_NORESERVE     0x0004000
#define SYS_MAP_POPULATE      0x0008000
#define SYS_MAP_NONBLOCK      0x0010000
#define SYS_MAP_STACK         0x0020000
#define SYS_MAP_UNINITIALIZED 0x4000000

#define SYS_OPEN_READONLY  0x0000
#define SYS_OPEN_WRITEONLY 0x0001
#define SYS_OPEN_READWRITE 0x0002
#define SYS_OPEN_CREATE    0x0100
#define SYS_OPEN_TRUNCATE  0x1000
#define SYS_OPEN_APPEND    0x2000

#define SYS_CREATE_OTHERS_EXEC  0x00001
#define SYS_CREATE_OTHERS_WRITE 0x00002
#define SYS_CREATE_OTHERS_READ  0x00004
#define SYS_CREATE_GROUP_EXEC   0x00010
#define SYS_CREATE_GROUP_WRITE  0x00020
#define SYS_CREATE_GROUP_READ   0x00040
#define SYS_CREATE_USER_EXEC    0x00100
#define SYS_CREATE_USER_WRITE   0x00200
#define SYS_CREATE_USER_READ    0x00400

#define SYS_CLOCK_REALTIME 0

typedef struct sys_stat {
	u64 DeviceID;
	u64 Inode;
	u64 Protection;

	u32 HardLinkCount;
	u32 UserID;
	u32 GroupID;
	u32 _Padding0;

	u64 SpecialDeviceID;
	s64 Size;
	s64 BlockSize;
	s64 BlockCount;

	u64 LastAccess;
	u64 LastAccessNano;
	u64 LastContentChange;
	u64 LastContentChangeNano;
	u64 LastStatusChange;
	u64 LastStatusChangeNano;
	s64 _Padding1[3];
} sys_stat;

typedef struct sys_timespec {
	s64 Seconds;
	s64 Nano;
} sys_timespec;

#define LINUX_SYSCALLS \
	SYSCALL(0,   Read,         s64,  u32 FileDescriptor, c08 *Buffer, u64 Count) \
	SYSCALL(1,   Write,        s64,  u32 FileDescriptor, c08 *Buffer, u64 Count) \
	SYSCALL(2,   Open,         s32,  c08 *Filename, s32 Flags, u16 Mode) \
	SYSCALL(3,   Close,        s32,  u32 FileDescriptor) \
	SYSCALL(5,   FileStat,     s32,  u32 FileDescriptor, sys_stat *Stat) \
	SYSCALL(8,   Seek,         s64,  u32 FileDescriptor, s64 Offset, s32 Whence) \
	SYSCALL(9,   MemMap,       vptr, vptr Address, u64 Length, s32 Protection, s32 Flags, u32 FileDescriptor, s64 Offset) \
	SYSCALL(11,  MemUnmap,     s32,  vptr Address, u64 Length) \
	SYSCALL(60,  Exit,         void, s32 ErrorCode) \
	SYSCALL(228, GetClockTime, s32,  s32 ClockID, sys_timespec *Timespec) \
	SYSCALL(229, GetClockRes,  s32,  s32 ClockID, sys_timespec *Timespec) \

// rdi, rsi, rdx, rcx, r8, r9 -> rdi, rsi, rdx, r10, r8, r9
#define SYSCALL(ID, Name, ReturnType, ...) \
	ReturnType __attribute__((naked)) \
	Sys_##Name(__VA_ARGS__) { \
		__asm__ ( \
			"mov $"#ID", %eax  \n" \
			"mov %rcx, %r10    \n" \
			"syscall           \n" \
			"ret               \n" \
		); \
	}
LINUX_SYSCALLS
#undef SYSCALL

#endif
