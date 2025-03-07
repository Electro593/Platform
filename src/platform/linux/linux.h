/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
**                                                                         **
**  Author: Aria Seiler                                                    **
**                                                                         **
**  This program is in the public domain. There is no implied warranty,    **
**  so use it at your own risk.                                            **
**                                                                         **
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define SYS_FILE_NONE -1
#define SYS_FILE_IN    0
#define SYS_FILE_OUT   1
#define SYS_FILE_ERR   2

#define SYS_PROT_NONE  0x0
#define SYS_PROT_READ  0x1
#define SYS_PROT_WRITE 0x2
#define SYS_PROT_EXEC  0x4

#define SYS_MAP_ANONYMOUS 0x20

typedef u16 sys_umode_t;
typedef s64 sys_off_t;

#if _X64

#define LINUX_SYSCALLS \
	SYSCALL(0,  Read,      s64,  u32 FileDescriptor, c08 *Buffer, u64 Count) \
	SYSCALL(1,  Write,     s64,  u32 FileDescriptor, c08 *Buffer, u64 Count) \
	SYSCALL(2,  Open,      s32,  c08 *Filename, s32 Flags, sys_umode_t Mode) \
	SYSCALL(3,  Close,     s32,  u32 FileDescriptor) \
	SYSCALL(9,  MemMap,    vptr, vptr Address, u64 Length, s32 Protection, s32 Flags, s32 FileDescriptor, sys_off_t Offset) \
	SYSCALL(11, MemUnmap,  s32,  vptr Address, u64 Length) \
	SYSCALL(60, Exit,      void, s32 ErrorCode) \

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
