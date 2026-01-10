/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                             *
*  Author: Aria Seiler                                                        *
*                                                                             *
*  This program is in the public domain. There is no implied warranty, so     *
*  use it at your own risk.                                                   *
*                                                                             *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef INCLUDE_HEADER

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

#define SYS_RUNTIME_LOADER_LAZY     0x0001
#define SYS_RUNTIME_LOADER_NOW      0x0002
#define SYS_RUNTIME_LOADER_NOLOAD   0x0004
#define SYS_RUNTIME_LOADER_DEEPBIND 0x0008
#define SYS_RUNTIME_LOADER_GLOBAL   0x0100
#define SYS_RUNTIME_LOADER_NODELETE 0x1000

#define SYS_ADDRESS_FAMILY_UNIX 1

#define SYS_SOCKET_STREAM 1

#define SYS_CLOCK_REALTIME 0

typedef s32 sys_pid;

typedef struct sys_sockaddr_unix {
	u16 Family;
	c08 Data[108];
} sys_sockaddr_unix;

typedef struct sys_sockaddr {
	u16 Family;
	union {
		c08	 MinData[14];
		char Data[];
	};
} sys_sockaddr;

typedef enum sys_send_flags {
	SYS_MSG_NOSIGNAL = 0x4000,
} sys_send_flags;

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

typedef ssize sys_time;
typedef ssize sys_suseconds;

typedef struct sys_timeval {
	sys_time	  Seconds;
	sys_suseconds Microseconds;
} sys_timeval;

typedef struct sys_rusage {
	sys_timeval UserTime;
	sys_timeval SystemTime;
	usize		MaxResidentSetSize;
	usize		IntegralSharedMemorySize;
	usize		IntegralUnsharedDataSize;
	usize		IntegralUnsharedStackSize;
	usize		PageReclaimCount;
	usize		PageFaultCount;
	usize		SwapCount;
	usize		BlockInputOperationCount;
	usize		BlockOutputOperationCount;
	usize		SentIPCMessageCount;
	usize		ReceivedIPCMessageCount;
	usize		ReceivedSignalCount;
	usize		VoluntaryContextSwitchCount;
	usize		InvoluntaryContextSwitchCount;
} sys_rusage;

typedef struct sys_pollfd {
	s32 FileDescriptor;
	s16 RequestedEvents;
	s16 ReturnedEvents;
} sys_pollfd;

typedef enum sys_poll_event {
	SYS_POLLIN	 = 0x1,
	SYS_POLLPRI	 = 0x2,
	SYS_POLLOUT	 = 0x4,
	SYS_POLLERR	 = 0x8,
	SYS_POLLNVAL = 0x20,
} sys_poll_event;

typedef enum sys_wait_options {
	SYS_WAIT_OPTION_NO_HANG	 = 1,
	SYS_WAIT_OPTION_UNTRACED = 2,
} sys_wait_options;

typedef enum sys_clone_flags {
	SYS_CLONE_VM	  = 0x00000100,
	SYS_CLONE_FS	  = 0x00000200,
	SYS_CLONE_FILES	  = 0x00000400,
	SYS_CLONE_SIGHAND = 0x00000800,
	SYS_CLONE_THREAD  = 0x00010000,
	SYS_CLONE_IO	  = 0x80000000,
} sys_clone_flags;

#ifndef _USE_LOADER
extern vptr dlopen(c08 *Path, s32 Flags);
extern vptr dlsym(vptr Handle, c08 *Symbol);
extern s32	dlclose(vptr Handle);
#endif

#define LINUX_SYSCALLS \
	SYSCALL(0,   Read,         ssize,   u32 FileDescriptor, c08 *Buffer, u64 Count) \
	SYSCALL(1,   Write,        ssize,   u32 FileDescriptor, c08 *Buffer, u64 Count) \
	SYSCALL(2,   Open,         s32,     c08 *Filename, s32 Flags, u16 Mode) \
	SYSCALL(3,   Close,        s32,     u32 FileDescriptor) \
	SYSCALL(5,   FileStat,     s32,     u32 FileDescriptor, sys_stat *Stat) \
	SYSCALL(7,   Poll,         s32,     sys_pollfd *FileDescriptors, s32 FileDescriptorCount, s32 Timeout) \
	SYSCALL(8,   Seek,         ssize,   u32 FileDescriptor, ssize Offset, s32 Whence) \
	SYSCALL(9,   MemMap,       vptr,    vptr Address, usize Length, s32 Protection, s32 Flags, u32 FileDescriptor, s64 Offset) \
	SYSCALL(10,  MemProtect,   s32,     vptr Address, usize Length, s32 Protection) \
	SYSCALL(11,  MemUnmap,     s32,     vptr Address, usize Length) \
	SYSCALL(41,  Socket,       s32,     s32 Domain, s32 Type, s32 Protocol) \
	SYSCALL(42,  Connect,      s32,     s32 SocketFileDescriptor, sys_sockaddr *Address, u32 AddressLength) \
	SYSCALL(44,  SendTo,       ssize,   s32 SocketFileDescriptor, vptr Buffer, usize Size, sys_send_flags Flags, sys_sockaddr *Address, u32 AddressLength) \
	SYSCALL(56,  Clone,        sys_pid, sys_clone_flags Flags, vptr Stack, s32 *ParentTidOut, usize TLS, s32 *ChildTidOut) \
	SYSCALL(57,  Fork,         sys_pid, void) \
	SYSCALL(60,  Exit,         void,    s32 ErrorCode) \
	SYSCALL(61,  Wait4,        sys_pid, sys_pid ProcessId, s32 *StatusOut, s32 Options, sys_rusage *ResourceUsageOut) \
	SYSCALL(228, GetClockTime, s32,     s32 ClockID, sys_timespec *Timespec) \
	SYSCALL(229, GetClockRes,  s32,     s32 ClockID, sys_timespec *Timespec) \
	//

#endif // defined(_X64)

#endif // defined(INCLUDE_HEADER)

#ifdef INCLUDE_SOURCE

#ifdef _X64

// rdi, rsi, rdx, rcx, r8, r9 -> rdi, rsi, rdx, r10, r8, r9
#define SYSCALL(ID, Name, ReturnType, ...)     \
	internal ReturnType __attribute__((naked)) \
	Sys_##Name(__VA_ARGS__) {                  \
		__asm__ (                              \
			"mov $"#ID", %eax  \n"             \
			"mov %rcx, %r10    \n"             \
			"syscall           \n"             \
			"ret               \n"             \
		);                                     \
	}
LINUX_SYSCALLS
#undef SYSCALL

#endif // defined(_X64)

#endif // defined(INCLUDE_SOURCE)