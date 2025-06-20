/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
**                                                                         **
**  Author: Aria Seiler                                                    **
**                                                                         **
**  This program is in the public domain. There is no implied warranty,    **
**  so use it at your own risk.                                            **
**                                                                         **
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

//CREDIT: https://refspecs.linuxfoundation.org/elf/gabi4+/contents.html

#if defined(_X64)
#define ELF_CLASS 64
#define ELF_ENCODING LITTLE_ENDIAN
#define ELF_VERSION CURRENT
#define ELF_MACHINE X86_64
#endif

#define _ELF_CAT2(A, B) A ## B
#define ELF_CAT2(A, B) _ELF_CAT2(A, B)

#define _ELF_CAT3(A, B, C) A ## B ## C
#define ELF_CAT3(A, B, C) _ELF_CAT3(A, B, C)

#define ELF_EXPECTED_CLASS    ELF_CAT2(ELF_CLASS_,    ELF_CLASS)
#define ELF_EXPECTED_ENCODING ELF_CAT2(ELF_ENCODING_, ELF_ENCODING)
#define ELF_EXPECTED_VERSION  ELF_CAT2(ELF_VERSION_,  ELF_VERSION)
#define ELF_EXPECTED_MACHINE  ELF_CAT2(ELF_MACHINE_,  ELF_MACHINE)

/* ========== ELF FILE HEADER ========== */

#define ELF_MAGIC_NUMBER (u08[4]){127, 'E', 'L', 'F'} // All elf files must start with this

#define ELF_CLASS_NONE 0 // Invalid
#define ELF_CLASS_32   1 // elf32 structs
#define ELF_CLASS_64   2 // elf64 structs

#define ELF_ENCODING_NONE          0 // Invalid
#define ELF_ENCODING_LITTLE_ENDIAN 1 // Least significant byte at the lowest memory address
#define ELF_ENCODING_BIG_ENDIAN    2 // Most significant byte at the lowest memory address

#define ELF_VERSION_NONE    0 // Invalid
#define ELF_VERSION_CURRENT 1 // Current version

#define ELF_OSABI_NONE     0 // No extensions or unspecified
#define ELF_OSABI_HPUX     1 // Hewlett-Packard HP-UX
#define ELF_OSABI_NETBSD   2 // NetBSD
#define ELF_OSABI_LINUX    3 // Linux
#define ELF_OSABI_SOLARIS  6 // Sun Solaris
#define ELF_OSABI_AIX      7 // AIX
#define ELF_OSABI_IRIX     8 // IRIX
#define ELF_OSABI_FREEBSD  9 // FreeBSD
#define ELF_OSABI_TRU64   10 // Compaq TRU64 UNIX
#define ELF_OSABI_MODESTO 11 // Novell Modesto
#define ELF_OSABI_OPENBSD 12 // Open BSD
#define ELF_OSABI_OPENVMS 13 // Open VMS
#define ELF_OSABI_NSK     14 // Hewlett-Packard Non-Stop Kernel

#define ELF_ABI_VERSION_NONE 0 // Unspecified

#define ELF_TYPE_NONE        0 // No file type
#define ELF_TYPE_RELOCATABLE 1 // Relocatable file
#define ELF_TYPE_EXECUTABLE  2 // Executable file
#define ELF_TYPE_DYNAMIC     3 // Shared object file
#define ELF_TYPE_CORE        4 // Core file

#define ELF_MACHINE_NONE          0 // No machine
#define ELF_MACHINE_M32           1 // AT&T WE 32100
#define ELF_MACHINE_SPARC         2 // SPARC
#define ELF_MACHINE_386           3 // Intel 80386
#define ELF_MACHINE_68K           4 // Motorola 68000
#define ELF_MACHINE_88K           5 // Motorola 88000
#define ELF_MACHINE_860           7 // Intel 80860
#define ELF_MACHINE_MIPS          8 // MIPS I Architecture
#define ELF_MACHINE_S370          9 // IBM System/370 Processor
#define ELF_MACHINE_MIPS_RS3_LE  10 // MIPS RS3000 Little-endian
#define ELF_MACHINE_PARISC       15 // Hewlett-Packard PA-RISC
#define ELF_MACHINE_VPP500       17 // Fujitsu VPP500
#define ELF_MACHINE_SPARC32PLUS  18 // Enhanced instruction set SPARC
#define ELF_MACHINE_960          19 // Intel 80960
#define ELF_MACHINE_PPC          20 // PowerPC
#define ELF_MACHINE_PPC64        21 // 64-bit PowerPC
#define ELF_MACHINE_S390         22 // IBM System/390 Processor
#define ELF_MACHINE_V800         36 // NEC V800
#define ELF_MACHINE_FR20         37 // Fujitsu FR20
#define ELF_MACHINE_RH32         38 // TRW RH-32
#define ELF_MACHINE_RCE          39 // Motorola RCE
#define ELF_MACHINE_ARM          40 // Advanced RISC Machines ARM
#define ELF_MACHINE_ALPHA        41 // Digital Alpha
#define ELF_MACHINE_SH           42 // Hitachi SH
#define ELF_MACHINE_SPARCV9      43 // SPARC Version 9
#define ELF_MACHINE_TRICORE      44 // Siemens TriCore embedded processor
#define ELF_MACHINE_ARC          45 // Argonaut RISC Core, Argonaut Technologies Inc.
#define ELF_MACHINE_H8_300       46 // Hitachi H8/300
#define ELF_MACHINE_H8_300H      47 // Hitachi H8/300H
#define ELF_MACHINE_H8S          48 // Hitachi H8S
#define ELF_MACHINE_H8_500       49 // Hitachi H8/500
#define ELF_MACHINE_IA_64        50 // Intel IA-64 processor architecture
#define ELF_MACHINE_MIPS_X       51 // Stanford MIPS-X
#define ELF_MACHINE_COLDFIRE     52 // Motorola ColdFire
#define ELF_MACHINE_68HC12       53 // Motorola M68HC12
#define ELF_MACHINE_MMA          54 // Fujitsu MMA Multimedia Accelerator
#define ELF_MACHINE_PCP          55 // Siemens PCP
#define ELF_MACHINE_NCPU         56 // Sony nCPU embedded RISC processor
#define ELF_MACHINE_NDR1         57 // Denso NDR1 microprocessor
#define ELF_MACHINE_STARCORE     58 // Motorola Star*Core processor
#define ELF_MACHINE_ME16         59 // Toyota ME16 processor
#define ELF_MACHINE_ST100        60 // STMicroelectronics ST100 processor
#define ELF_MACHINE_TINYJ        61 // Advanced Logic Corp. TinyJ embedded processor family
#define ELF_MACHINE_X86_64       62 // AMD x86-64 architecture
#define ELF_MACHINE_PDSP         63 // Sony DSP Processor
#define ELF_MACHINE_PDP10        64 // Digital Equipment Corp. PDP-10
#define ELF_MACHINE_PDP11        65 // Digital Equipment Corp. PDP-11
#define ELF_MACHINE_FX66         66 // Siemens FX66 microcontroller
#define ELF_MACHINE_ST9PLUS      67 // STMicroelectronics ST9+ 8/16 bit microcontroller
#define ELF_MACHINE_ST7          68 // STMicroelectronics ST7 8-bit microcontroller
#define ELF_MACHINE_68HC16       69 // Motorola MC68HC16 Microcontroller
#define ELF_MACHINE_68HC11       70 // Motorola MC68HC11 Microcontroller
#define ELF_MACHINE_68HC08       71 // Motorola MC68HC08 Microcontroller
#define ELF_MACHINE_68HC05       72 // Motorola MC68HC05 Microcontroller
#define ELF_MACHINE_SVX          73 // Silicon Graphics SVx
#define ELF_MACHINE_ST19         74 // STMicroelectronics ST19 8-bit microcontroller
#define ELF_MACHINE_VAX          75 // Digital VAX
#define ELF_MACHINE_CRIS         76 // Axis Communications 32-bit embedded processor
#define ELF_MACHINE_JAVELIN      77 // Infineon Technologies 32-bit embedded processor
#define ELF_MACHINE_FIREPATH     78 // Element 14 64-bit DSP Processor
#define ELF_MACHINE_ZSP          79 // LSI Logic 16-bit DSP Processor
#define ELF_MACHINE_MMIX         80 // Donald Knuth's educational 64-bit processor
#define ELF_MACHINE_HUANY        81 // Harvard University machine-independent object files
#define ELF_MACHINE_PRISM        82 // SiTera Prism
#define ELF_MACHINE_AVR          83 // Atmel AVR 8-bit microcontroller
#define ELF_MACHINE_FR30         84 // Fujitsu FR30
#define ELF_MACHINE_D10V         85 // Mitsubishi D10V
#define ELF_MACHINE_D30V         86 // Mitsubishi D30V
#define ELF_MACHINE_V850         87 // NEC v850
#define ELF_MACHINE_M32R         88 // Mitsubishi M32R
#define ELF_MACHINE_MN10300      89 // Matsushita MN10300
#define ELF_MACHINE_MN10200      90 // Matsushita MN10200
#define ELF_MACHINE_PJ           91 // picoJava
#define ELF_MACHINE_OPENRISC     92 // OpenRISC 32-bit embedded processor
#define ELF_MACHINE_ARC_A5       93 // ARC Cores Tangent-A5
#define ELF_MACHINE_XTENSA       94 // Tensilica Xtensa Architecture
#define ELF_MACHINE_VIDEOCORE    95 // Alphamosaic VideoCore processor
#define ELF_MACHINE_TMM_GPP      96 // Thompson Multimedia General Purpose Processor
#define ELF_MACHINE_NS32K        97 // National Semiconductor 32000 series
#define ELF_MACHINE_TPC          98 // Tenor Network TPC processor
#define ELF_MACHINE_SNP1K        99 // Trebia SNP 1000 processor
#define ELF_MACHINE_ST200       100 // STMicroelectronics (www.st.com) ST200 microcontroller

typedef u32 elf32_addr;
typedef u32 elf32_off;
typedef u16 elf32_half;
typedef u32 elf32_word;
typedef s32 elf32_sword;

typedef u64 elf64_addr;
typedef u64 elf64_off;
typedef u16 elf64_half;
typedef u32 elf64_word;
typedef s32 elf64_sword;
typedef u64 elf64_xword;
typedef s64 elf64_sxword;

typedef struct elf_ident {
	u08 MagicNumber[4];
	u08 FileClass;
	u08 DataEncoding;
	u08 FileVersion;
	u08 OSABI;
	u08 ABIVersion;
	u08 _Padding[7];
} elf_ident;

typedef struct elf32_header {
	elf_ident  Ident;
	elf32_half Type;
	elf32_half Machine;
	elf32_word Version;
	elf32_addr Entry;
	elf32_off  ProgramHeaderTableOffset;
	elf32_off  SectionHeaderTableOffset;
	elf32_word Flags;
	elf32_half ElfHeaderSize;
	elf32_half ProgramHeaderSize;
	elf32_half ProgramHeaderCount;
	elf32_half SectionHeaderSize;
	elf32_half SectionHeaderCount;
	elf32_half SectionNameTableIndex;
} elf32_header;

typedef struct elf64_header {
	elf_ident  Ident;
	elf64_half Type;
	elf64_half Machine;
	elf64_word Version;
	elf64_addr Entry;
	elf64_off  ProgramHeaderTableOffset;
	elf64_off  SectionHeaderTableOffset;
	elf64_word Flags;
	elf64_half ElfHeaderSize;
	elf64_half ProgramHeaderSize;
	elf64_half ProgramHeaderCount;
	elf64_half SectionHeaderSize;
	elf64_half SectionHeaderCount;
	elf64_half SectionNameTableIndex;
} elf64_header;

#define elf_header ELF_CAT3(elf, ELF_CLASS, _header)

/* ========== ELF SECTION HEADER ========== */

#define ELF_SECTION_INDEX_UNDEF     0x0000
#define ELF_SECTION_INDEX_LORESERVE 0xFF00
#define ELF_SECTION_INDEX_ABS       0xFFF1
#define ELF_SECTION_INDEX_COMMON    0xFFF2
#define ELF_SECTION_INDEX_EXTENDED  0xFFFF

#define ELF_SECTION_TYPE_NULL           0
#define ELF_SECTION_TYPE_PROGBITS       1
#define ELF_SECTION_TYPE_SYMTAB         2
#define ELF_SECTION_TYPE_STRTAB         3
#define ELF_SECTION_TYPE_RELA           4
#define ELF_SECTION_TYPE_HASH           5
#define ELF_SECTION_TYPE_DYNAMIC        6
#define ELF_SECTION_TYPE_NOTE           7
#define ELF_SECTION_TYPE_NOBITS         8
#define ELF_SECTION_TYPE_REL            9
#define ELF_SECTION_TYPE_SHLIB         10
#define ELF_SECTION_TYPE_DYNSYM        11
#define ELF_SECTION_TYPE_INIT_ARRAY    14
#define ELF_SECTION_TYPE_FINI_ARRAY    15
#define ELF_SECTION_TYPE_PREINIT_ARRAY 16
#define ELF_SECTION_TYPE_GROUP         17
#define ELF_SECTION_TYPE_SYMTAB_SHNDX  18

#define ELF_SECTION_FLAG_WRITE            0x001
#define ELF_SECTION_FLAG_ALLOC            0x002
#define ELF_SECTION_FLAG_EXECINSTR        0x004
#define ELF_SECTION_FLAG_MERGE            0x010
#define ELF_SECTION_FLAG_STRINGS          0x020
#define ELF_SECTION_FLAG_INFO_LINK        0x040
#define ELF_SECTION_FLAG_LINK_ORDER       0x080
#define ELF_SECTION_FLAG_OS_NONCONFORMING 0x100
#define ELF_SECTION_FLAG_GROUP            0x200
#define ELF_SECTION_FLAG_TLS              0x400

#define ELF_GROUP_COMDAT 0x1

typedef struct elf32_section_header {
	elf32_word Name;
	elf32_word Type;
	elf32_word Flags;
	elf32_addr Address;
	elf32_off  Offset;
	elf32_word Size;
	elf32_word Link;
	elf32_word Info;
	elf32_word AddressAlignment;
	elf32_word EntrySize;
} elf32_section_header;

typedef struct elf64_section_header {
	elf64_word  Name;
	elf64_word  Type;
	elf64_xword Flags;
	elf64_addr  Address;
	elf64_off   Offset;
	elf64_xword Size;
	elf64_word  Link;
	elf64_word  Info;
	elf64_xword AddressAlignment;
	elf64_xword EntrySize;
} elf64_section_header;

#define elf_section_header ELF_CAT3(elf, ELF_CLASS, _section_header)

/* ========== ELF PROGRAM HEADER ========== */

#define ELF_SEGMENT_TYPE_NULL    0
#define ELF_SEGMENT_TYPE_LOAD    1
#define ELF_SEGMENT_TYPE_DYNAMIC 2
#define ELF_SEGMENT_TYPE_INTERP  3
#define ELF_SEGMENT_TYPE_NOTE    4
#define ELF_SEGMENT_TYPE_SHLIB   5
#define ELF_SEGMENT_TYPE_PHDR    6
#define ELF_SEGMENT_TYPE_TLS     7

#define ELF_SEGMENT_FLAG_EXEC  0x1
#define ELF_SEGMENT_FLAG_WRITE 0x2
#define ELF_SEGMENT_FLAG_READ  0x4

typedef struct elf32_program_header {
	elf32_word Type;
	elf32_off  Offset;
	elf32_addr VirtualAddress;
	elf32_addr PhysicalAddress;
	elf32_word FileSize;
	elf32_word MemSize;
	elf32_word Flags;
	elf32_word Alignment;
} elf32_program_header;

typedef struct elf64_program_header {
	elf64_word  Type;
	elf64_word  Flags;
	elf64_off   Offset;
	elf64_addr  VirtualAddress;
	elf64_addr  PhysicalAddress;
	elf64_xword FileSize;
	elf64_xword MemSize;
	elf64_xword Alignment;
} elf64_program_header;

#define elf_program_header ELF_CAT3(elf, ELF_CLASS, _program_header)

/* ========== ELF SYMBOL ========== */

#define ELF_SYMBOL_INDEX_UNDEFINED 0

#define ELF_SYMBOL_BINDING_LOCAL  0 // Only visible within a single object file
#define ELF_SYMBOL_BINDING_GLOBAL 1 // Visible to all object files
#define ELF_SYMBOL_BINDING_WEAK   2 // Global, but with lower precedence

#define ELF_SYMBOL_TYPE_NONE     0 // Unspecified
#define ELF_SYMBOL_TYPE_OBJECT   1 // Variable, array, struct, etc.
#define ELF_SYMBOL_TYPE_FUNCTION 2 // Executable code
#define ELF_SYMBOL_TYPE_SECTION  3 // Usually for relocations
#define ELF_SYMBOL_TYPE_FILE     4 // Source file name
#define ELF_SYMBOL_TYPE_COMMON   5 // Uninitialized common block
#define ELF_SYMBOL_TYPE_TLS      6 // Thread-local storage

#define ELF_SYMBOL_VISIBILITY_DEFAULT   0 // Inherits from binding, preemptable
#define ELF_SYMBOL_VISIBILITY_INTERNAL  1 // Effectively hidden
#define ELF_SYMBOL_VISIBILITY_HIDDEN    2 // Invisible to other components
#define ELF_SYMBOL_VISIBILITY_PROTECTED 3 // Visible but non-preemptable

typedef struct elf32_symbol {
	elf32_word Name;
	elf32_addr Value;
	elf32_word Size;
	u08        Info;
	u08        Other;
	elf32_half SectionIndex;
} elf32_symbol;

typedef struct elf64_symbol {
	elf64_word  Name;
	u08         Info;
	u08         Other;
	elf64_half  SectionIndex;
	elf64_addr  Value;
	elf64_xword Size;
} elf64_symbol;

#define elf_symbol ELF_CAT3(elf, ELF_CLASS, _symbol)

/* ========== ELF RELOCATION ========== */

typedef struct elf32_relocation {
	elf32_addr Offset;
	elf32_word Info;
} elf32_relocation;

typedef struct elf32_relocation_a {
	elf32_addr  Offset;
	elf32_word  Info;
	elf32_sword Addend;
} elf32_relocation_a;

typedef struct elf64_relocation {
	elf64_addr  Offset;
	elf64_xword Info;
} elf64_relocation;

typedef struct elf64_relocation_a {
	elf64_addr   Offset;
	elf64_xword  Info;
	elf64_sxword Addend;
} elf64_relocation_a;

#define elf_relocation ELF_CAT3(elf, ELF_CLASS, _relocation)
#define elf_relocation_a ELF_CAT3(elf, ELF_CLASS, _relocation_a)

/* ========== ELF DYNAMIC ========== */

#define ELF_DYNAMIC_TAG_NULL             0
#define ELF_DYNAMIC_TAG_NEEDED           1
#define ELF_DYNAMIC_TAG_PLTRELSZ         2
#define ELF_DYNAMIC_TAG_PLTGOT           3
#define ELF_DYNAMIC_TAG_HASH             4
#define ELF_DYNAMIC_TAG_STRTAB           5
#define ELF_DYNAMIC_TAG_SYMTAB           6
#define ELF_DYNAMIC_TAG_RELA             7
#define ELF_DYNAMIC_TAG_RELASZ           8
#define ELF_DYNAMIC_TAG_RELAENT          9
#define ELF_DYNAMIC_TAG_STRSZ           10
#define ELF_DYNAMIC_TAG_SYMENT          11
#define ELF_DYNAMIC_TAG_INIT            12
#define ELF_DYNAMIC_TAG_FINI            13
#define ELF_DYNAMIC_TAG_SONAME          14
#define ELF_DYNAMIC_TAG_RPATH           15
#define ELF_DYNAMIC_TAG_SYMBOLIC        16
#define ELF_DYNAMIC_TAG_REL             17
#define ELF_DYNAMIC_TAG_RELSZ           18
#define ELF_DYNAMIC_TAG_RELENT          19
#define ELF_DYNAMIC_TAG_PLTREL          20
#define ELF_DYNAMIC_TAG_DEBUG           21
#define ELF_DYNAMIC_TAG_TEXTREL         22
#define ELF_DYNAMIC_TAG_JMPREL          23
#define ELF_DYNAMIC_TAG_BIND_NOW        24
#define ELF_DYNAMIC_TAG_INIT_ARRAY      25
#define ELF_DYNAMIC_TAG_FINI_ARRAY      26
#define ELF_DYNAMIC_TAG_INIT_ARRAYSZ    27
#define ELF_DYNAMIC_TAG_FINI_ARRAYSZ    28
#define ELF_DYNAMIC_TAG_RUNPATH         29
#define ELF_DYNAMIC_TAG_FLAGS           30
#define ELF_DYNAMIC_TAG_ENCODING        32
#define ELF_DYNAMIC_TAG_PREINIT_ARRAY   32
#define ELF_DYNAMIC_TAG_PREINIT_ARRAYSZ 33

#define ELF_DYNAMIC_FLAG_ORIGIN     0x01
#define ELF_DYNAMIC_FLAG_SYMBOLIC   0x02
#define ELF_DYNAMIC_FLAG_TEXTREL    0x04
#define ELF_DYNAMIC_FLAG_BIND_NOW   0x08
#define ELF_DYNAMIC_FLAG_STATIC_TLS 0x10

typedef struct elf32_dynamic {
	elf32_sword Tag;
   	union {
   		elf32_word Value;
   		elf32_addr Pointer;
	};
} elf32_dynamic;

typedef struct elf64_dynamic {
	elf64_sxword Tag;
   	union {
   		elf64_xword Value;
   		elf64_addr  Pointer;
	};
} elf64_dynamic;

#define elf_dynamic ELF_CAT3(elf, ELF_CLASS, _dynamic)

/* ========== AUXILIARY VECTORS ========== */

#define ELF_AT_NULL               0 /* end of vector */
#define ELF_AT_IGNORE             1 /* entry should be ignored */
#define ELF_AT_EXECFD             2 /* file descriptor of program */
#define ELF_AT_PHDR               3 /* program headers for program */
#define ELF_AT_PHENT              4 /* size of program header entry */
#define ELF_AT_PHNUM              5 /* number of program headers */
#define ELF_AT_PAGESZ             6 /* system page size */
#define ELF_AT_BASE               7 /* base address of interpreter */
#define ELF_AT_FLAGS              8 /* flags */
#define ELF_AT_ENTRY              9 /* entry point of program */
#define ELF_AT_NOTELF            10 /* program is not ELF */
#define ELF_AT_UID               11 /* real uid */
#define ELF_AT_EUID              12 /* effective uid */
#define ELF_AT_GID               13 /* real gid */
#define ELF_AT_EGID              14 /* effective gid */
#define ELF_AT_PLATFORM          15 /* string identifying CPU for optimizations */
#define ELF_AT_HWCAP             16 /* arch dependent hints at CPU capabilities */
#define ELF_AT_CLKTCK            17 /* frequency at which times() increments */
#define ELF_AT_SECURE            23 /* secure mode boolean */
#define ELF_AT_BASE_PLATFORM     24 /* string identifying real platform, may differ from AT_PLATFORM. */
#define ELF_AT_RANDOM            25 /* address of 16 random bytes */
#define ELF_AT_HWCAP2            26 /* extension of AT_HWCAP */
#define ELF_AT_RSEQ_FEATURE_SIZE 27 /* rseq supported feature size */
#define ELF_AT_RSEQ_ALIGN        28 /* rseq allocation alignment */
#define ELF_AT_HWCAP3            29 /* extension of AT_HWCAP */
#define ELF_AT_HWCAP4            30 /* extension of AT_HWCAP */
#define ELF_AT_EXECFN            31 /* filename of program */
#define ELF_AT_MINSIGSTKSZ       51 /* minimal stack size for signal delivery */

#define ELF_AT_SYSINFO      32
#define ELF_AT_SYSINFO_EHDR 33

typedef struct elf32_auxv {
	u32 Type;
	union {
		u32 Value;
		#if ELF_CLASS == 32
		void *Pointer;
		void (*Function)(void);
		#endif
	};
} elf32_auxv;

typedef struct elf64_auxv {
	u64 Type;
	union {
		u64 Value;
		#if ELF_CLASS == 64
		void *Pointer;
		void (*Function)(void);
		#endif
	};
} elf64_auxv;

#define elf_auxv ELF_CAT3(elf, ELF_CLASS, _auxv)

/* ========== IMPLEMENTATION ========== */

#define ELF_ERROR_SUCCESS           0
#define ELF_ERROR_UNKNOWN           1
#define ELF_ERROR_INVALID_OPERATION 2
#define ELF_ERROR_INVALID_ARGUMENT  3
#define ELF_ERROR_INVALID_FORMAT    4
#define ELF_ERROR_NOT_FOUND         5
#define ELF_ERROR_UNKNOWN_FORMAT    6
#define ELF_ERROR_NOT_SUPPORTED     7
#define ELF_ERROR_OUT_OF_MEMORY     8
#define ELF_ERROR_INVALID_MEM_MAP   9

#define ELF_STATE_CLOSED           0
#define ELF_STATE_OPENED           1
#define ELF_STATE_LOADED           2

typedef s32 elf_error;

typedef struct elf_state {
	// General
	u08 State;
	u64 PageSize;

	// Mapped file
	u32 FileDescriptor;
	u08 *File;
	u64 FileSize;

	// Quick access
	elf_header *Header;

	u64 SectionHeaderCount;
	u08 *SectionHeaderTable;
	u08 *ProgramHeaderTable;

	elf_section_header *NullSectionHeader;

	elf_section_header *SectionNameSectionHeader;
	u08 *SectionNameTable;

	// Loaded image
	vptr ImageAddress;
	u64 ImageSize;
} elf_state;