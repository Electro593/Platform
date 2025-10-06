/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                             *
*  Author: Aria Seiler                                                        *
*                                                                             *
*  This program is in the public domain. There is no implied warranty, so     *
*  use it at your own risk.                                                   *
*                                                                             *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef INCLUDE_HEADER

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

#define elf_addr  ELF_CAT3(elf, ELF_CLASS, _addr)
#define elf_off   ELF_CAT3(elf, ELF_CLASS, _off)
#define elf_half  ELF_CAT3(elf, ELF_CLASS, _half)
#define elf_word  ELF_CAT3(elf, ELF_CLASS, _word)
#define elf_sword ELF_CAT3(elf, ELF_CLASS, _sword)

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
	elf64_word	Name;
	elf64_word	Type;
	elf64_xword Flags;
	elf64_addr	Address;
	elf64_off	Offset;
	elf64_xword Size;
	elf64_word	Link;
	elf64_word	Info;
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

#define ELF_SEGMENT_TYPE_GNU_EH_FRAME 0x6474e550
#define ELF_SEGMENT_TYPE_GNU_STACK    0x6474e551
#define ELF_SEGMENT_TYPE_GNU_RELRO    0x6474e552

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
	elf64_word	Type;
	elf64_word	Flags;
	elf64_off	Offset;
	elf64_addr	VirtualAddress;
	elf64_addr	PhysicalAddress;
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
	u08		   Info;
	u08		   Other;
	elf32_half SectionIndex;
} elf32_symbol;

typedef struct elf64_symbol {
	elf64_word	Name;
	u08			Info;
	u08			Other;
	elf64_half	SectionIndex;
	elf64_addr	Value;
	elf64_xword Size;
} elf64_symbol;

#define elf_symbol ELF_CAT3(elf, ELF_CLASS, _symbol)

/* ========== ELF RELOCATION ========== */
#define ELF_RELOCATION_X86_64_NONE             0 // none      none
#define ELF_RELOCATION_X86_64_64               1 // word64    S + A
#define ELF_RELOCATION_X86_64_PC32             2 // word32    S + A - P
#define ELF_RELOCATION_X86_64_GOT32            3 // word32    G + A
#define ELF_RELOCATION_X86_64_PLT32            4 // word32    L + A - P
#define ELF_RELOCATION_X86_64_COPY             5 // none      none
#define ELF_RELOCATION_X86_64_GLOB_DAT         6 // word64    S
#define ELF_RELOCATION_X86_64_JUMP_SLOT        7 // word64    S
#define ELF_RELOCATION_X86_64_RELATIVE         8 // word64    B + A
#define ELF_RELOCATION_X86_64_GOTPCREL         9 // word32    G + GOT + A - P
#define ELF_RELOCATION_X86_64_32              10 // word32    S + A
#define ELF_RELOCATION_X86_64_32S             11 // word32    S + A
#define ELF_RELOCATION_X86_64_16              12 // word16    S + A
#define ELF_RELOCATION_X86_64_PC16            13 // word16    S + A - P
#define ELF_RELOCATION_X86_64_8               14 // word8     S + A
#define ELF_RELOCATION_X86_64_PC8             15 // word8     S + A - P
#define ELF_RELOCATION_X86_64_DTPMOD64        16 // word64
#define ELF_RELOCATION_X86_64_DTPOFF64        17 // word64
#define ELF_RELOCATION_X86_64_TPOFF64         18 // word64
#define ELF_RELOCATION_X86_64_TLSGD           19 // word32
#define ELF_RELOCATION_X86_64_TLSLD           20 // word32
#define ELF_RELOCATION_X86_64_DTPOFF32        21 // word32
#define ELF_RELOCATION_X86_64_GOTTPOFF        22 // word32
#define ELF_RELOCATION_X86_64_TPOFF32         23 // word32
#define ELF_RELOCATION_X86_64_PC64            24 // word64    S + A - P
#define ELF_RELOCATION_X86_64_GOTOFF64        25 // word64    S + A - GOT
#define ELF_RELOCATION_X86_64_GOTPC32         26 // word32    GOT + A - P
#define ELF_RELOCATION_X86_64_SIZE32          32 // word32    Z + A
#define ELF_RELOCATION_X86_64_SIZE64          33 // word64    Z + A
#define ELF_RELOCATION_X86_64_GOTPC32_TLSDESC 34 // word32
#define ELF_RELOCATION_X86_64_TLSDESC_CALL    35 // none
#define ELF_RELOCATION_X86_64_TLSDESC         36 // word64×2
#define ELF_RELOCATION_X86_64_IRELATIVE       37 // word64    indirect (B + A)

typedef struct elf32_relocation {
	elf32_addr Offset;
	elf32_word Info;
} elf32_relocation;

typedef struct elf32_relocation_a {
	elf32_addr	Offset;
	elf32_word	Info;
	elf32_sword Addend;
} elf32_relocation_a;

typedef struct elf64_relocation {
	elf64_addr	Offset;
	elf64_xword Info;
} elf64_relocation;

typedef struct elf64_relocation_a {
	elf64_addr	 Offset;
	elf64_xword	 Info;
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
#define ELF_DYNAMIC_TAG_PREINIT_ARRAY   32
#define ELF_DYNAMIC_TAG_PREINIT_ARRAYSZ 33
#define ELF_DYNAMIC_TAG_SYMTAB_SHNDX    34
#define ELF_DYNAMIC_TAG_RELRSZ          35
#define ELF_DYNAMIC_TAG_RELR            36
#define ELF_DYNAMIC_TAG_RELRENT         37

#define ELF_DYNAMIC_TAG_GNU_HASH        0x6ffffef5

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
		elf64_addr	Pointer;
	};
} elf64_dynamic;

#define elf_dynamic ELF_CAT3(elf, ELF_CLASS, _dynamic)

/* ========== ELF NOTE ========== */

#define ELF_NOTE_GNU_ABI_TAG 1
#define ELF_NOTE_GNU_HWCAP 2
#define ELF_NOTE_GNU_BUILD_ID 3
#define ELF_NOTE_GNU_GOLD_VERSION 4
#define ELF_NOTE_GNU_PROPERTY_TYPE_0 5

#define ELF_NOTE_GNU_ABI_TAG_OS_LINUX    0
#define ELF_NOTE_GNU_ABI_TAG_OS_GNU      1
#define ELF_NOTE_GNU_ABI_TAG_OS_SOLARIS2 2
#define ELF_NOTE_GNU_ABI_TAG_OS_FREEBSD  3

#define ELF_GNU_PROPERTY_X86_FEATURE_1       0xC0000002
#define ELF_GNU_PROPERTY_X86_FEATURE_1_IBT   (1ull << 0)
#define ELF_GNU_PROPERTY_X86_FEATURE_1_SHSTK (1ull << 1)

#define ELF_GNU_PROPERTY_X86_ISA_1_USED     0xC0010002
#define ELF_GNU_PROPERTY_X86_ISA_1_NEEDED   0xC0008002
#define ELF_GNU_PROPERTY_X86_ISA_1_BASELINE (1ull << 0)
#define ELF_GNU_PROPERTY_X86_ISA_1_V2       (1ull << 1)
#define ELF_GNU_PROPERTY_X86_ISA_1_V3       (1ull << 2)
#define ELF_GNU_PROPERTY_X86_ISA_1_V4       (1ull << 3)

typedef struct elf_note {
	elf_word NameSize;
	elf_word DescriptorSize;
	elf_word Type;
	elf_word Words[];
} elf_note;

typedef struct elf_gnu_property {
	elf_word Type;
	elf_word DataSize;
	u08		 Data[];
} elf_gnu_property;

/* ========== AUXILIARY VECTORS ========== */

#define ELF_AUX_NULL               0 // End of vector
#define ELF_AUX_IGNORE             1 // Entry should be ignored
#define ELF_AUX_EXECFD             2 // File descriptor of program
#define ELF_AUX_PHDR               3 // Program headers for program
#define ELF_AUX_PHENT              4 // Size of program header entry
#define ELF_AUX_PHNUM              5 // Number of program headers
#define ELF_AUX_PAGESZ             6 // System page size
#define ELF_AUX_BASE               7 // Base address of interpreter
#define ELF_AUX_FLAGS              8 // Flags
#define ELF_AUX_ENTRY              9 // Entry point of program
#define ELF_AUX_NOTELF            10 // Program is not ELF
#define ELF_AUX_UID               11 // Real uid
#define ELF_AUX_EUID              12 // Effective uid
#define ELF_AUX_GID               13 // Real gid
#define ELF_AUX_EGID              14 // Effective gid
#define ELF_AUX_PLATFORM          15 // String identifying platform
#define ELF_AUX_HWCAP             16 // Machine-dependent hints about processor capabilities
#define ELF_AUX_CLKTCK            17 // Frequency of times()
#define ELF_AUX_FPUCW             18 // Used FPU control word
#define ELF_AUX_DCACHEBSIZE       19 // Data cache block size
#define ELF_AUX_ICACHEBSIZE       20 // Instruction cache block size
#define ELF_AUX_UCACHEBSIZE       21 // Unified cache block size
#define ELF_AUX_IGNOREPPC         22 // Entry should be ignored
#define ELF_AUX_SECURE            23 // Secure mode boolean
#define ELF_AUX_BASE_PLATFORM     24 // String identifying real platforms
#define ELF_AUX_RANDOM            25 // Address of 16 random bytes
#define ELF_AUX_HWCAP2            26 // More machine-dependent hints about processor capabilities
#define ELF_AUX_RSEQ_FEATURE_SIZE 27 // rseq supported feature size
#define ELF_AUX_RSEQ_ALIGN        28 // rseq allocation alignment
#define ELF_AUX_HWCAP3            29 // Extension of AT_HWCAP
#define ELF_AUX_HWCAP4            30 // Extension of AT_HWCAP
#define ELF_AUX_EXECFN            31 // Filename of executable
#define ELF_AUX_SYSINFO           32 // Pointer to the beginning of vsyscall
#define ELF_AUX_SYSINFO_EHDR      33 // Pointer to the beggining of the vDSO
#define ELF_AUX_L1I_CACHESHAPE    34 // Shape of L1 instruction cache (bits 0-3 associativity, 4-7 log2 line size)
#define ELF_AUX_L1D_CACHESHAPE    35 // Shape of L1 data cache
#define ELF_AUX_L2_CACHESHAPE     36 // Shape of L2 cache
#define ELF_AUX_L3_CACHESHAPE     37 // Shape of L3 cache
#define ELF_AUX_L1I_CACHESIZE     40 // Size of L1 instruction cache
#define ELF_AUX_L1I_CACHEGEOMETRY 41 // Geometry of L1 instruction cache (low 16 bits line size, high 16 associativity)
#define ELF_AUX_L1D_CACHESIZE     42 // Size of L1 data cache
#define ELF_AUX_L1D_CACHEGEOMETRY 43 // Geometry of L1 data cache
#define ELF_AUX_L2_CACHESIZE      44 // Size of L2 cache
#define ELF_AUX_L2_CACHEGEOMETRY  45 // Geometry of L2 cache
#define ELF_AUX_L3_CACHESIZE      46 // Size of L3 cache
#define ELF_AUX_L3_CACHEGEOMETRY  47 // Geometry of L3 cache
#define ELF_AUX_MINSIGSTKSZ       51 // Stack needed for signal delivery

typedef struct elf32_aux {
	u32 Type;
	union {
		u32	  Value;
		void *Pointer;
		void (*Function)(void);
	};
} elf32_aux;

typedef struct elf64_aux {
	u64 Type;
	union {
		u64	  Value;
		void *Pointer;
		void (*Function)(void);
	};
} elf64_aux;

#define elf_aux ELF_CAT3(elf, ELF_CLASS, _aux)

/* ========== DEBUG RENDEZVOUS ========== */

typedef enum sys_r_debug_state {
	SYS_R_DEBUG_STATE_CONSISTENT,
	SYS_R_DEBUG_STATE_ADD,
	SYS_R_DEBUG_STATE_DELETE
} sys_r_debug_state;

typedef struct sys_link_map {
	usize				 DeltaAddr;
	c08					*FileName;
	elf_dynamic			*Dynamics;
	struct sys_link_map *Next;
	struct sys_link_map *Prev;
} sys_link_map;

typedef struct sys_r_debug {
	s32			  Version;
	sys_link_map *LinkMap;
	void (*Breakpoint)(void);
	sys_r_debug_state State;
	vptr			  LoaderBase;
} sys_r_debug;

/* ========== HASH TABLE ========== */

#define ELF_LOOKUP_LINEAR   0
#define ELF_LOOKUP_HASH     1
#define ELF_LOOKUP_GNU_HASH 2

typedef struct elf_hash_table {
	elf_word BucketCount;
	elf_word ChainCount;
	elf_word Data[];
} elf_hash_table;

typedef struct elf_gnu_hash_table {
	elf_word BucketCount;
	elf_word SymbolOffset;
	elf_word BloomSize;
	elf_word BloomShift;
	elf_word Data[];
} elf_gnu_hash_table;

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

typedef struct elf_dynamic_state {
	elf_dynamic *Dynamics;

	usize PltRelSize;
	vptr  PltGot;
	vptr  Hash;
	c08	 *StrTab;
	vptr  SymTab;
	vptr  Rela;
	usize RelaSize;
	usize RelaEntrySize;
	usize StrTabSize;
	usize SymTabEntrySize;
	void (*Init)(void);
	void (*Fini)(void);
	c08	 *SoName;
	c08	 *RPath;
	b08	  Symbolic;
	vptr  Rel;
	usize RelSize;
	usize RelEntrySize;
	usize PltRel;
	vptr  Debug;
	b08	  TextRel;
	vptr  JmpRel;
	b08	  BindNow;
	void (**InitArray)(void);
	void (**FiniArray)(void);
	usize InitArraySize;
	usize FiniArraySize;
	c08	 *RunPath;
	usize Flags;
	void (**PreinitArray)(void);
	usize PreinitArraySize;
	vptr  SymTabShndx;
	usize RelrSize;
	vptr  Relr;
	usize RelrEntrySize;
	vptr  GnuHash;
} elf_dynamic_state;

typedef struct elf_state {
	// General
	u08	  State;
	usize PageSize;

	// Mapped file
	c08	 *FileName;
	u32	  FileDescriptor;
	u08	 *File;
	usize FileSize;

	// Quick access
	elf_header Header;

	usize SectionHeaderCount;
	u08	 *SectionHeaderTable;
	u08	 *ProgramHeaderTable;

	elf_section_header *NullSectionHeader;
	elf_section_header *SectionNameSectionHeader;
	c08				   *SectionNameTable;

	elf_dynamic_state Dynamic;

	usize LookupType;
	c08	 *HashStrTab;
	vptr  HashSymTab;
	usize HashSymTabEntrySize;
	union {
		elf_hash_table	   *HashTable;
		elf_gnu_hash_table *GnuHashTable;
	};

	// Loaded image
	usize TextVAddr;
	usize VAddrOffset;
	vptr  ImageAddress;
	usize ImageSize;
} elf_state;

#endif	// INCLUDE_HEADER

#ifdef INCLUDE_SOURCE

internal elf_section_header *
Elf_GetSectionHeader(elf_state *State, u64 Index)
{
	if (!State->SectionHeaderTable) return 0;
	return (vptr) (State->SectionHeaderTable
				   + State->Header.SectionHeaderSize * Index);
}

internal u08 *
Elf_GetSection(elf_state *State, elf_section_header *Header)
{
	if (!Header) return 0;
	if (Header->Type == ELF_SECTION_TYPE_NULL) return 0;
	if (Header->Type == ELF_SECTION_TYPE_NOBITS) return 0;
	return State->File + Header->Offset;
}

internal elf_program_header *
Elf_GetProgramHeader(elf_state *State, u64 Index)
{
	if (!State->ProgramHeaderTable) return 0;
	return (vptr) (State->ProgramHeaderTable
				   + State->Header.ProgramHeaderSize * Index);
}

internal b08
Elf_CheckName(c08 *Name, c08 *Expected)
{
	c08 *A, *B;
	for (A = Name, B = Expected; *A && *B && *A == *B; A++, B++);
	return (!*A || *A == '.') && !*B;
}

internal u32
Elf_Hash(c08 *Name)
{
	u32 Hash = 0;
	while (*Name) {
		Hash  = (Hash << 4) + *Name++;
		u32 G = Hash & 0xF0000000;
		if (G) Hash ^= G >> 24;
		Hash &= ~G;
	}
	return Hash;
}

internal u32
Elf_GnuHash(c08 *Name)
{
	u32 Hash = 5381;
	while (*Name) Hash = (Hash << 5) + Hash + *Name++;
	return Hash;
}

internal vptr
Elf_LookupSymbol_Linear(elf_state *State, c08 *Name)
{
	return NULL;
}

internal vptr
Elf_LookupSymbol_Hash(elf_state *State, c08 *Name)
{
	vptr  StrTab		  = State->HashStrTab;
	vptr  SymTab		  = State->HashSymTab;
	usize SymTabEntrySize = State->HashSymTabEntrySize;

	elf_hash_table *Table	= State->HashTable;
	elf_word	   *Buckets = Table->Data;
	elf_word	   *Chains	= Buckets + Table->BucketCount;

	u32 Hash = Elf_GnuHash(Name);

	u32 SymIndex = Buckets[Hash % Table->BucketCount];
	while (SymIndex != ELF_SYMBOL_INDEX_UNDEFINED) {
		elf_symbol *Symbol	= SymTab + SymIndex * SymTabEntrySize;
		c08		   *SymName = StrTab + Symbol->Name;
		if (Elf_CheckName(SymName, Name))
			return State->ImageAddress - State->VAddrOffset + Symbol->Value;

		SymIndex = Chains[SymIndex];
	}

	return NULL;
}

internal vptr
Elf_LookupSymbol_GnuHash(elf_state *State, c08 *Name)
{
	vptr  StrTab		  = State->HashStrTab;
	vptr  SymTab		  = State->HashSymTab;
	usize SymTabEntrySize = State->HashSymTabEntrySize;

	elf_gnu_hash_table *Table = State->GnuHashTable;

	usize BloomWords =
		Table->BloomSize
		* ((State->Header.Ident.FileClass == ELF_CLASS_32) ? 1 : 2);

	elf32_word	*Bloom32 = (vptr) Table->Data;
	elf64_xword *Bloom64 = (vptr) Table->Data;

	elf_word *Buckets = Table->Data + BloomWords;
	elf_word *Chain	  = Buckets + Table->BucketCount;

	u32 Hash  = Elf_GnuHash(Name);
	u32 Hash2 = Hash >> Table->BloomShift;

	if (State->Header.Ident.FileClass == 32) {
		u32 N = Bloom32[(Hash >> 5) & (Table->BloomSize - 1)];
		u32 B = (1 << (Hash & 31)) | (1 << (Hash2 & 31));
		if ((N & B) != B) return NULL;
	} else {
		u64 N = Bloom64[(Hash >> 6) & (Table->BloomSize - 1)];
		u64 B = (1ull << (Hash & 63)) | (1ull << (Hash2 & 63));
		if ((N & B) != B) return NULL;
	}

	u32	 SymIndex	= Buckets[Hash % Table->BucketCount];
	u32 *ChainEntry = Chain + SymIndex - Table->SymbolOffset;

	while (1) {
		if ((Hash >> 1) == (*ChainEntry >> 1)) {
			elf_symbol *Symbol	= SymTab + SymIndex * SymTabEntrySize;
			c08		   *SymName = StrTab + Symbol->Name;
			if (Elf_CheckName(SymName, Name))
				return State->ImageAddress - State->VAddrOffset + Symbol->Value;
		}

		if (*ChainEntry & 1) break;
		ChainEntry++, SymIndex++;
	}

	return NULL;
}

internal usize
Elf_ExtractAddend(elf_state *State, usize Type)
{
	switch (State->Header.Machine) {
		case ELF_MACHINE_X86_64: return 0;	// X86_64 uses only Rela
		default				   : return 0;
	}
}

internal usize
Elf_ComputeRelocation(elf_state *State, usize Type, usize A, usize S, usize B)
{
	usize V = 0;
	switch (State->Header.Machine) {
		case ELF_MACHINE_X86_64: {
			switch (Type) {
				case ELF_RELOCATION_X86_64_64		: V = S + A; break;
				case ELF_RELOCATION_X86_64_GLOB_DAT : V = S; break;
				case ELF_RELOCATION_X86_64_JUMP_SLOT: V = S; break;
				case ELF_RELOCATION_X86_64_RELATIVE : V = B + A; break;
			}
		} break;
	}
	return V;
}

internal void
Elf_ApplyRelocation(elf_state *State, vptr Target, usize Type, usize Value)
{
	u64 *V64 = Target;
	switch (State->Header.Machine) {
		case ELF_MACHINE_X86_64: {
			switch (Type) {
				case ELF_RELOCATION_X86_64_64		: *V64 = Value; break;
				case ELF_RELOCATION_X86_64_GLOB_DAT : *V64 = Value; break;
				case ELF_RELOCATION_X86_64_JUMP_SLOT: *V64 = Value; break;
				case ELF_RELOCATION_X86_64_RELATIVE : *V64 = Value; break;
			}
		} break;
	}
}

internal void
Elf_HandleRelocations(
	elf_state *State,
	vptr	   Relocs,
	usize	   RelocEntrySize,
	usize	   RelocsSize,
	b08		   IsRela,
	c08		  *StrTab,
	vptr	   SymTab,
	usize	   SymTabEntrySize
)
{
	vptr		PrevTarget = NULL;
	usize		Value	   = 0;
	usize		RelocType  = 0;
	elf_symbol *Symbol	   = NULL;

	vptr Cursor = Relocs;
	while (Cursor < Relocs + RelocsSize) {
		elf_relocation	 *Rel  = Cursor;
		elf_relocation_a *Rela = Cursor;

		vptr Target = State->ImageAddress - State->VAddrOffset + Rel->Offset;
		if (PrevTarget != Target) {
			if (PrevTarget) {
				c08 *Name = (c08 *) StrTab + Symbol->Name;
				Elf_ApplyRelocation(State, PrevTarget, RelocType, Value);
			}

			if (IsRela) Value = Rela->Addend;
			else Value = Elf_ExtractAddend(State, RelocType);
		}

		usize SymbolIndex;
		if (State->Header.Ident.FileClass == ELF_CLASS_32) {
			SymbolIndex = Rela->Info >> 8;
			RelocType	= Rela->Info & 0xFF;
		} else {
			SymbolIndex = Rela->Info >> 32;
			RelocType	= Rela->Info & 0xFFFFFFFF;
		}
		Symbol = (elf_symbol *) (SymTab + SymTabEntrySize * SymbolIndex);

		Value = Elf_ComputeRelocation(
			State,
			RelocType,
			Value,
			(usize) State->ImageAddress - State->VAddrOffset + Symbol->Value,
			(usize) State->ImageAddress
		);

		PrevTarget	= Target;
		Cursor	   += RelocEntrySize;
	}

	if (PrevTarget) Elf_ApplyRelocation(State, PrevTarget, RelocType, Value);
}

internal elf_error
Elf_HandleSectionRelocations(elf_state *State)
{
	for (usize I = 0; I < State->SectionHeaderCount; I++) {
		elf_section_header *Header = Elf_GetSectionHeader(State, I);
		vptr				Data   = State->File + Header->Offset;
		char *Name = (char *) (State->SectionNameTable + Header->Name);

		b08 IsRela = Elf_CheckName(Name, ".rela");
		b08 IsRel  = !IsRela && Elf_CheckName(Name, ".rel");

		if (!IsRela && !IsRel) continue;

		elf_section_header *SymTabHeader =
			Elf_GetSectionHeader(State, Header->Link);
		vptr SymTab = State->File + SymTabHeader->Offset;

		elf_section_header *TargetHeader =
			Elf_GetSectionHeader(State, Header->Info);
		vptr TargetBase = State->File + TargetHeader->Offset;

		elf_section_header *StrTabHeader =
			Elf_GetSectionHeader(State, SymTabHeader->Link);
		vptr StrTab = State->File + StrTabHeader->Offset;

		Elf_HandleRelocations(
			State,
			Data,
			Header->EntrySize,
			Header->Size,
			IsRela,
			StrTab,
			SymTab,
			SymTabHeader->EntrySize
		);
	}

	return ELF_ERROR_SUCCESS;
}

internal void
Elf_ReadDynamics(elf_state *State)
{
	vptr			   Base		= State->ImageAddress + State->VAddrOffset;
	elf_dynamic_state *DynState = &State->Dynamic;

	for (usize I = 0; I < State->Header.ProgramHeaderCount; I++) {
		elf_program_header *Header = Elf_GetProgramHeader(State, I);
		if (Header->Type != ELF_SEGMENT_TYPE_DYNAMIC) continue;

		DynState->Dynamics =
			State->ImageAddress - State->VAddrOffset + Header->VirtualAddress;
		break;
	}

	usize SoName = 0, RPath = 0, RunPath = 0;

	elf_dynamic *Dynamic = DynState->Dynamics;
	while (Dynamic->Tag != ELF_DYNAMIC_TAG_NULL) {
		switch (Dynamic->Tag) {
			case ELF_DYNAMIC_TAG_PLTRELSZ:
				DynState->PltRelSize = Dynamic->Value;
				break;
			case ELF_DYNAMIC_TAG_PLTGOT:
				DynState->PltGot = Base + Dynamic->Pointer;
				break;
			case ELF_DYNAMIC_TAG_HASH:
				DynState->Hash = Base + Dynamic->Pointer;
				break;
			case ELF_DYNAMIC_TAG_STRTAB:
				DynState->StrTab = Base + Dynamic->Pointer;
				break;
			case ELF_DYNAMIC_TAG_SYMTAB:
				DynState->SymTab = Base + Dynamic->Pointer;
				break;
			case ELF_DYNAMIC_TAG_RELA:
				DynState->Rela = Base + Dynamic->Pointer;
				break;
			case ELF_DYNAMIC_TAG_RELASZ:
				DynState->RelaSize = Dynamic->Value;
				break;
			case ELF_DYNAMIC_TAG_RELAENT:
				DynState->RelaEntrySize = Dynamic->Value;
				break;
			case ELF_DYNAMIC_TAG_STRSZ:
				DynState->StrTabSize = Dynamic->Value;
				break;
			case ELF_DYNAMIC_TAG_SYMENT:
				DynState->SymTabEntrySize = Dynamic->Value;
				break;
			case ELF_DYNAMIC_TAG_INIT:
				DynState->Init = Base + Dynamic->Pointer;
				break;
			case ELF_DYNAMIC_TAG_FINI:
				DynState->Fini = Base + Dynamic->Pointer;
				break;
			case ELF_DYNAMIC_TAG_SONAME	 : SoName = Dynamic->Value; break;
			case ELF_DYNAMIC_TAG_RPATH	 : RPath = Dynamic->Value; break;
			case ELF_DYNAMIC_TAG_SYMBOLIC: DynState->Symbolic = TRUE; break;
			case ELF_DYNAMIC_TAG_REL:
				DynState->Rel = Base + Dynamic->Pointer;
				break;
			case ELF_DYNAMIC_TAG_RELSZ:
				DynState->RelSize = Dynamic->Value;
				break;
			case ELF_DYNAMIC_TAG_RELENT:
				DynState->RelEntrySize = Dynamic->Value;
				break;
			case ELF_DYNAMIC_TAG_PLTREL:
				DynState->PltRel = Dynamic->Value;
				break;
			case ELF_DYNAMIC_TAG_DEBUG:
				DynState->Debug = Base + Dynamic->Pointer;
				break;
			case ELF_DYNAMIC_TAG_JMPREL:
				DynState->JmpRel = Base + Dynamic->Pointer;
				break;
			case ELF_DYNAMIC_TAG_BIND_NOW: DynState->BindNow = TRUE; break;
			case ELF_DYNAMIC_TAG_INIT_ARRAY:
				DynState->InitArray = Base + Dynamic->Pointer;
				break;
			case ELF_DYNAMIC_TAG_FINI_ARRAY:
				DynState->FiniArray = Base + Dynamic->Pointer;
				break;
			case ELF_DYNAMIC_TAG_INIT_ARRAYSZ:
				DynState->InitArraySize = Dynamic->Value;
				break;
			case ELF_DYNAMIC_TAG_FINI_ARRAYSZ:
				DynState->FiniArraySize = Dynamic->Value;
				break;
			case ELF_DYNAMIC_TAG_RUNPATH: RunPath = Dynamic->Value; break;
			case ELF_DYNAMIC_TAG_FLAGS	: DynState->Flags = Dynamic->Value; break;
			case ELF_DYNAMIC_TAG_PREINIT_ARRAY:
				DynState->PreinitArray = Base + Dynamic->Pointer;
				break;
			case ELF_DYNAMIC_TAG_PREINIT_ARRAYSZ:
				DynState->PreinitArraySize = Dynamic->Value;
				break;
			case ELF_DYNAMIC_TAG_SYMTAB_SHNDX:
				DynState->SymTabShndx = Base + Dynamic->Pointer;
				break;
			case ELF_DYNAMIC_TAG_RELRSZ:
				DynState->RelrSize = Dynamic->Value;
				break;
			case ELF_DYNAMIC_TAG_RELR:
				DynState->Relr = Base + Dynamic->Pointer;
				break;
			case ELF_DYNAMIC_TAG_RELRENT:
				DynState->RelrEntrySize = Dynamic->Value;
				break;
			case ELF_DYNAMIC_TAG_GNU_HASH:
				DynState->GnuHash = Base + Dynamic->Pointer;
				break;
		}
		Dynamic++;
	}

	DynState->SoName  = DynState->StrTab + SoName;
	DynState->RPath	  = DynState->StrTab + RPath;
	DynState->RunPath = DynState->StrTab + RunPath;
}

internal void
Elf_HandleDynamicRelocations(elf_state *State)
{
	Elf_ReadDynamics(State);

	if (State->Dynamic.Rel) {
		Elf_HandleRelocations(
			State,
			State->Dynamic.Rel,
			State->Dynamic.RelEntrySize,
			State->Dynamic.RelSize,
			FALSE,
			State->Dynamic.StrTab,
			State->Dynamic.SymTab,
			State->Dynamic.SymTabEntrySize
		);
	}

	if (State->Dynamic.Rela) {
		Elf_HandleRelocations(
			State,
			State->Dynamic.Rela,
			State->Dynamic.RelaEntrySize,
			State->Dynamic.RelaSize,
			TRUE,
			State->Dynamic.StrTab,
			State->Dynamic.SymTab,
			State->Dynamic.SymTabEntrySize
		);
	}

	if (State->Dynamic.GnuHash) {
		State->LookupType = ELF_LOOKUP_GNU_HASH;
		State->HashTable  = State->Dynamic.GnuHash;
	} else if (State->Dynamic.Hash) {
		State->LookupType = ELF_LOOKUP_HASH;
		State->HashTable  = State->Dynamic.Hash;
	}
	State->HashStrTab		   = State->Dynamic.StrTab;
	State->HashSymTab		   = State->Dynamic.SymTab;
	State->HashSymTabEntrySize = State->Dynamic.SymTabEntrySize;

	if (!State->FileName) State->FileName = State->Dynamic.SoName;
}

internal elf_error
Elf_LoadDynamicLoadSegment(
	elf_state		   *State,
	vptr				Base,
	elf_program_header *Header
)
{
	usize PageMask = State->PageSize - 1;

	usize Padding  = Header->VirtualAddress & PageMask;
	vptr  ProgBase = Base + (Header->VirtualAddress - State->VAddrOffset);

	s32 Prot = ((Header->Flags & ELF_SEGMENT_FLAG_READ) ? SYS_PROT_READ : 0)
			 | ((Header->Flags & ELF_SEGMENT_FLAG_WRITE) ? SYS_PROT_WRITE : 0)
			 | ((Header->Flags & ELF_SEGMENT_FLAG_EXEC) ? SYS_PROT_EXEC : 0);

	s32 Flags = SYS_MAP_PRIVATE | SYS_MAP_FIXED;

	// Try to map the file portion into memory
	if (Header->FileSize > 0) {
		vptr Addr = Sys_MemMap(
			(vptr) ((usize) ProgBase & ~PageMask),
			(Header->FileSize + Padding + PageMask) & ~PageMask,
			Prot,
			Flags,
			State->FileDescriptor,
			Header->Offset - Padding
		);
		if ((usize) Addr & PageMask) return ELF_ERROR_INVALID_MEM_MAP;
	}

	// Try to map the nobits portion into memory
	if (Header->MemSize > Header->FileSize) {
		vptr NoBitsStart = ProgBase + Header->FileSize;

		usize NoBitsPadding =
			State->PageSize - ((usize) NoBitsStart & PageMask);

		for (usize I = 0; I < NoBitsPadding; I++) ((u08 *) NoBitsStart)[I] = 0;

		if (Header->MemSize > Header->FileSize + NoBitsPadding) {
			vptr NoBitsBase =
				(vptr) (((usize) ProgBase + Header->FileSize + PageMask)
						& ~PageMask);
			usize NoBitsExtra =
				Header->MemSize - Header->FileSize - NoBitsPadding;
			vptr Addr = Sys_MemMap(
				NoBitsBase,
				NoBitsExtra,
				Prot,
				Flags | SYS_MAP_ANONYMOUS,
				-1,
				0
			);
			if ((usize) Addr & PageMask) return ELF_ERROR_INVALID_MEM_MAP;

			for (usize I = 0; I < NoBitsExtra; I++) ((u08 *) NoBitsBase)[I] = 0;
		}
	}

	return ELF_ERROR_SUCCESS;
}

internal elf_error
Elf_ReadSegments(elf_state *State)
{
	usize MinVAddr = (usize) -1;
	usize MaxVAddr = 0;

	for (usize I = 0; I < State->Header.ProgramHeaderCount; I++) {
		elf_program_header *Header = Elf_GetProgramHeader(State, I);
		switch (Header->Type) {
			case ELF_SEGMENT_TYPE_LOAD: {
				if (MinVAddr > Header->VirtualAddress)
					MinVAddr = Header->VirtualAddress;
				if (MaxVAddr < Header->VirtualAddress + Header->MemSize)
					MaxVAddr = Header->VirtualAddress + Header->MemSize;
			} break;
		}
	}

	if (MinVAddr == (usize) -1) return ELF_ERROR_INVALID_FORMAT;

	usize PageMask		= State->PageSize - 1;
	MinVAddr		   &= ~PageMask;
	State->VAddrOffset	= MinVAddr;
	State->ImageSize	= MaxVAddr - MinVAddr;

	return ELF_ERROR_SUCCESS;
}

internal elf_error
Elf_LoadSegments(elf_state *State)
{
	if (State->Header.Type == ELF_TYPE_DYNAMIC) {
		elf_error Error = Elf_ReadSegments(State);
		if (Error) return Error;

		usize PageMask = State->PageSize - 1;
		vptr  Base = State->ImageAddress = Sys_MemMap(
			 NULL,
			 (State->ImageSize + PageMask) & ~PageMask,
			 SYS_PROT_NONE,
			 SYS_MAP_PRIVATE | SYS_MAP_ANONYMOUS | SYS_MAP_NORESERVE,
			 -1,
			 0
		 );
		if ((ssize) Base == -1) return ELF_ERROR_OUT_OF_MEMORY;

		for (usize I = 0; I < State->Header.ProgramHeaderCount; I++) {
			elf_program_header *Header = Elf_GetProgramHeader(State, I);
			switch (Header->Type) {
				case ELF_SEGMENT_TYPE_LOAD: {
					Error = Elf_LoadDynamicLoadSegment(State, Base, Header);
					if (Error) return Error;
				} break;
			}
		}
	} else {
		return ELF_ERROR_NOT_SUPPORTED;
	}

	return ELF_ERROR_SUCCESS;
}

internal elf_error
Elf_FixPermsAfterReloc(elf_state *State)
{
	for (usize I = 0; I < State->Header.ProgramHeaderCount; I++) {
		elf_program_header *Header = Elf_GetProgramHeader(State, I);
		if (Header->Type == ELF_SEGMENT_TYPE_GNU_RELRO) {
			usize PageMask = State->PageSize - 1;
			u32	 *Data	   = (vptr) State->ImageAddress
					  - State->VAddrOffset
					  + Header->VirtualAddress;
			s32 Prot =
				0
				| ((Header->Flags & ELF_SEGMENT_FLAG_READ) ? SYS_PROT_READ : 0)
				| ((Header->Flags & ELF_SEGMENT_FLAG_WRITE) ? SYS_PROT_WRITE
															: 0)
				| ((Header->Flags & ELF_SEGMENT_FLAG_EXEC) ? SYS_PROT_EXEC : 0);
			Sys_MemProtect(
				(vptr) ((usize) Data & ~PageMask),
				(Header->MemSize + PageMask) & ~PageMask,
				Prot
			);
		}
	}

	return ELF_ERROR_SUCCESS;
}

internal elf_error
Elf_SetupAndValidate(elf_state *State, b08 IncludeSectionTable)
{
	State->Header = *(elf_header *) State->File;

	if (*(u32 *) State->Header.Ident.MagicNumber != *(u32 *) ELF_MAGIC_NUMBER)
		return ELF_ERROR_INVALID_FORMAT;
	if (State->Header.Ident.FileClass != ELF_EXPECTED_CLASS)
		return ELF_ERROR_NOT_SUPPORTED;
	if (State->Header.Ident.DataEncoding != ELF_EXPECTED_ENCODING)
		return ELF_ERROR_NOT_SUPPORTED;
	if (State->Header.Ident.FileVersion != ELF_EXPECTED_VERSION)
		return ELF_ERROR_NOT_SUPPORTED;

	if (State->Header.Type != ELF_TYPE_DYNAMIC) return ELF_ERROR_NOT_SUPPORTED;
	if (State->Header.Machine != ELF_EXPECTED_MACHINE)
		return ELF_ERROR_NOT_SUPPORTED;
	if (State->Header.Version != ELF_EXPECTED_VERSION)
		return ELF_ERROR_NOT_SUPPORTED;
	if (State->Header.ElfHeaderSize < sizeof(elf_header))
		return ELF_ERROR_INVALID_FORMAT;
	if (State->Header.SectionHeaderSize < sizeof(elf_section_header))
		return ELF_ERROR_INVALID_FORMAT;
	if (State->Header.ProgramHeaderSize < sizeof(elf_program_header))
		return ELF_ERROR_INVALID_FORMAT;

	State->SectionHeaderCount = State->Header.SectionHeaderCount;
	if (IncludeSectionTable) {
		if (State->SectionHeaderCount || State->Header.SectionHeaderTableOffset)
		{
			State->SectionHeaderTable =
				State->File + State->Header.SectionHeaderTableOffset;

			State->NullSectionHeader = Elf_GetSectionHeader(State, 0);
			if (State->NullSectionHeader->Name) return ELF_ERROR_INVALID_FORMAT;
			if (State->NullSectionHeader->Type != ELF_SECTION_TYPE_NULL)
				return ELF_ERROR_INVALID_FORMAT;
			if (State->NullSectionHeader->Flags)
				return ELF_ERROR_INVALID_FORMAT;
			if (State->NullSectionHeader->Address)
				return ELF_ERROR_INVALID_FORMAT;
			if (State->NullSectionHeader->Offset)
				return ELF_ERROR_INVALID_FORMAT;
			if (State->NullSectionHeader->Info) return ELF_ERROR_INVALID_FORMAT;
			if (State->NullSectionHeader->AddressAlignment)
				return ELF_ERROR_INVALID_FORMAT;
			if (State->NullSectionHeader->EntrySize)
				return ELF_ERROR_INVALID_FORMAT;

			if (!State->SectionHeaderCount)
				State->SectionHeaderCount = State->NullSectionHeader->Size;
			else if (State->NullSectionHeader->Size)
				return ELF_ERROR_INVALID_FORMAT;

			u64 Index = State->Header.SectionNameTableIndex;
			if (Index == ELF_SECTION_INDEX_EXTENDED)
				Index = State->NullSectionHeader->Link;
			else if (Index >= ELF_SECTION_INDEX_LORESERVE)
				return ELF_ERROR_INVALID_FORMAT;
			else if (State->NullSectionHeader->Link)
				return ELF_ERROR_INVALID_FORMAT;
			State->SectionNameSectionHeader =
				Elf_GetSectionHeader(State, Index);
			State->SectionNameTable =
				(c08 *) Elf_GetSection(State, State->SectionNameSectionHeader);

			for (usize I = 0; I < State->SectionHeaderCount; I++) {
				elf_section_header *Header = Elf_GetSectionHeader(State, I);
				c08 *Name = State->SectionNameTable + Header->Name;
				vptr Data = State->File + Header->Offset;

				if (Elf_CheckName(Name, ".text")) {
					State->TextVAddr = Header->Address;
				} else if (Elf_CheckName(Name, ".hash")) {
					if (State->LookupType < ELF_LOOKUP_HASH) {
						State->LookupType = ELF_LOOKUP_HASH;
						State->HashTable  = Data;
						elf_section_header *SymTabHeader =
							Elf_GetSectionHeader(State, Header->Link);
						State->HashSymTab =
							(vptr) State->File + SymTabHeader->Offset;
						State->HashSymTabEntrySize = SymTabHeader->EntrySize;
						elf_section_header *StrTabHeader =
							Elf_GetSectionHeader(State, SymTabHeader->Link);
						State->HashStrTab =
							(vptr) State->File + StrTabHeader->Offset;
					}
				} else if (Elf_CheckName(Name, ".gnu.hash")) {
					if (State->LookupType < ELF_LOOKUP_GNU_HASH) {
						State->LookupType	= ELF_LOOKUP_GNU_HASH;
						State->GnuHashTable = Data;
						elf_section_header *SymTabHeader =
							Elf_GetSectionHeader(State, Header->Link);
						State->HashSymTab =
							(vptr) State->File + SymTabHeader->Offset;
						State->HashSymTabEntrySize = SymTabHeader->EntrySize;
						elf_section_header *StrTabHeader =
							Elf_GetSectionHeader(State, SymTabHeader->Link);
						State->HashStrTab =
							(vptr) State->File + StrTabHeader->Offset;
					}
				}
			}
		} else if (State->Header.SectionNameTableIndex
				   != ELF_SECTION_INDEX_UNDEF)
		{
			return ELF_ERROR_INVALID_FORMAT;
		}
	}

	if (State->Header.ProgramHeaderCount) {
		State->ProgramHeaderTable =
			State->File + State->Header.ProgramHeaderTableOffset;

		b08 FoundLoadable = FALSE;
		b08 FoundPHDR	  = FALSE;
		b08 FoundInterp	  = FALSE;

		for (u32 I = 0; I < State->Header.ProgramHeaderCount; I++) {
			elf_program_header *Header = Elf_GetProgramHeader(State, I);
			switch (Header->Type) {
				case ELF_SEGMENT_TYPE_NULL: break;
				case ELF_SEGMENT_TYPE_LOAD:
					if (Header->FileSize > Header->MemSize)
						return ELF_ERROR_INVALID_FORMAT;
					break;
				case ELF_SEGMENT_TYPE_DYNAMIC: break;
				case ELF_SEGMENT_TYPE_INTERP:
					if (FoundLoadable) return ELF_ERROR_INVALID_FORMAT;
					if (FoundInterp) return ELF_ERROR_INVALID_FORMAT;
					FoundInterp = TRUE;
					break;
				case ELF_SEGMENT_TYPE_NOTE: break;
				case ELF_SEGMENT_TYPE_SHLIB:
					return ELF_ERROR_INVALID_FORMAT;
					break;
				case ELF_SEGMENT_TYPE_PHDR:
					if (FoundLoadable) return ELF_ERROR_INVALID_FORMAT;
					if (FoundPHDR) return ELF_ERROR_INVALID_FORMAT;
					FoundPHDR = TRUE;
					break;
				case ELF_SEGMENT_TYPE_TLS: break;
			}
		}
	} else {
		if (State->Header.ProgramHeaderTableOffset)
			return ELF_ERROR_INVALID_FORMAT;
	}

	return ELF_ERROR_SUCCESS;
}

/* ========== EXTERNAL API ========== */

internal elf_error
Elf_OpenWithDescriptor(elf_state *State, u32 FileDescriptor, usize PageSize)
{
	// TODO: Don't allocate the entire file, just map what's needed

	if (!State) return ELF_ERROR_INVALID_ARGUMENT;
	if (State->State != ELF_STATE_CLOSED) return ELF_ERROR_INVALID_OPERATION;

	State->FileDescriptor = FileDescriptor;

	sys_stat Stat;
	s32		 StatResult = Sys_FileStat(State->FileDescriptor, &Stat);
	if (StatResult < 0 && StatResult >= -4096) return ELF_ERROR_UNKNOWN;
	State->FileSize = Stat.Size;

	State->File = Sys_MemMap(
		NULL,
		State->FileSize,
		SYS_PROT_READ,
		SYS_MAP_PRIVATE,
		State->FileDescriptor,
		0
	);
	if ((usize) State->File >= 0xFFFFFFFFFFFFF000) return ELF_ERROR_UNKNOWN;

	elf_error Error = Elf_SetupAndValidate(State, TRUE);
	if (Error) return Error;

	State->State = ELF_STATE_OPENED;
	return ELF_ERROR_SUCCESS;
}

internal elf_error
Elf_Open(elf_state *State, c08 *FileName, usize PageSize)
{
	if (!State || !FileName) return ELF_ERROR_INVALID_ARGUMENT;
	if (State->State != ELF_STATE_CLOSED) return ELF_ERROR_INVALID_OPERATION;

	*State			= (elf_state) { 0 };
	State->FileName = FileName;
	State->PageSize = PageSize;

	u32 FD = Sys_Open(FileName, SYS_OPEN_READONLY, 0);
	if (FD >= 0xFFFFF000) return ELF_ERROR_UNKNOWN;

	return Elf_OpenWithDescriptor(State, FD, PageSize);
}

internal elf_error
Elf_ReadLoadedImage(elf_state *State, vptr BaseAddress, usize PageSize)
{
	if (!State || !BaseAddress) return ELF_ERROR_INVALID_ARGUMENT;
	if (State->State != ELF_STATE_CLOSED) return ELF_ERROR_INVALID_OPERATION;

	*State			= (elf_state) { 0 };
	State->File		= BaseAddress;
	State->PageSize = PageSize;

	elf_error Error = Elf_SetupAndValidate(State, FALSE);
	if (Error) return Error;

	Error = Elf_ReadSegments(State);
	if (Error) return Error;

	State->ImageAddress = BaseAddress;

	Elf_HandleDynamicRelocations(State);

	Error = Elf_FixPermsAfterReloc(State);
	if (Error) return Error;

	State->State = ELF_STATE_LOADED;
	return ELF_ERROR_SUCCESS;
}

internal elf_error
Elf_LoadProgram(elf_state *State)
{
	if (!State) return ELF_ERROR_INVALID_ARGUMENT;
	if (State->State != ELF_STATE_OPENED) return ELF_ERROR_INVALID_OPERATION;

	elf_error Error = Elf_LoadSegments(State);
	if (Error) return Error;

	Error = Elf_HandleSectionRelocations(State);
	if (Error) return Error;

	Error = Elf_FixPermsAfterReloc(State);
	if (Error) return Error;

	State->State = ELF_STATE_LOADED;
	return ELF_ERROR_SUCCESS;
}

internal elf_error
Elf_GetProcAddress(elf_state *State, c08 *ProcName, vptr *ProcOut)
{
	if (!State || !ProcName || !ProcOut) return ELF_ERROR_INVALID_ARGUMENT;
	if (State->State != ELF_STATE_LOADED) return ELF_ERROR_INVALID_OPERATION;

	vptr Addr = NULL;
	switch (State->LookupType) {
		case ELF_LOOKUP_LINEAR:
			Addr = Elf_LookupSymbol_Linear(State, ProcName);
			break;
		case ELF_LOOKUP_HASH:
			Addr = Elf_LookupSymbol_Hash(State, ProcName);
			break;
		case ELF_LOOKUP_GNU_HASH:
			Addr = Elf_LookupSymbol_GnuHash(State, ProcName);
			break;
	}

	if (!Addr) return ELF_ERROR_NOT_FOUND;

	*ProcOut = Addr;
	return ELF_ERROR_SUCCESS;
}

internal elf_error
Elf_Unload(elf_state *State)
{
	if (!State) return ELF_ERROR_INVALID_ARGUMENT;
	if (State->State != ELF_STATE_LOADED) return ELF_ERROR_INVALID_OPERATION;

	State->State = ELF_STATE_OPENED;
	return ELF_ERROR_SUCCESS;
}

internal elf_error
Elf_Close(elf_state *State)
{
	if (!State) return ELF_ERROR_INVALID_ARGUMENT;
	if (State->State != ELF_STATE_OPENED) return ELF_ERROR_INVALID_OPERATION;

	s32 SysError = Sys_MemUnmap(State->File, State->FileSize);
	if (SysError < 0 && SysError >= -4096) return ELF_ERROR_UNKNOWN;
	SysError = Sys_Close(State->FileDescriptor);
	if (SysError < 0 && SysError >= -4096) return ELF_ERROR_UNKNOWN;

	State->State = ELF_STATE_CLOSED;
	return ELF_ERROR_SUCCESS;
}

#endif	// INCLUDE_SOURCE
