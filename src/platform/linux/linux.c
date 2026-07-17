/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                             *
*  Author: Aria Seiler                                                        *
*                                                                             *
*  This program is in the public domain. There is no implied warranty, so     *
*  use it at your own risk.                                                   *
*                                                                             *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

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

#endif	// defined(_X64)
