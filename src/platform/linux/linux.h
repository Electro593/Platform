/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
**                                                                         **
**  Author: Aria Seiler                                                    **
**                                                                         **
**  This program is in the public domain. There is no implied warranty,    **
**  so use it at your own risk.                                            **
**                                                                         **
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if _X64

#define LINUX_SYSCALLS \
	SYSCALL(60, Exit, s32 ErrorCode) \

// rdi, rsi, rdx, rcx, r8, r9 -> rdi, rsi, rdx, r10, r8, r9
#define SYSCALL(ID, Name, ...) \
	s32 __attribute__((naked)) \
	Sys_##Name(__VA_ARGS__) { \
		__asm__ ( \
			"mov $"#ID", %eax\n" \
			"mov %rcx, %r10\n" \
			"syscall\n" \
			"ret" \
		); \
	}
LINUX_SYSCALLS
#undef SYSCALL

#endif
