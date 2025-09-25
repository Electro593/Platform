/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                             *
*  Author: Aria Seiler                                                        *
*                                                                             *
*  This program is in the public domain. There is no implied warranty, so     *
*  use it at your own risk.                                                   *
*                                                                             *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

internal elf_section_header *
Elf_GetSectionHeader(elf_state *State, u64 Index)
{
	if (!State->SectionHeaderTable) return 0;
	return (vptr) (State->SectionHeaderTable
				   + State->Header->SectionHeaderSize * Index);
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
				   + State->Header->ProgramHeaderSize * Index);
}

internal b08
Elf_CheckName(c08 *Name, c08 *Expected)
{
	usize Bytes	 = _Mem_BytesUntil((u08 *) Expected, 0);
	b08	  Same	 = _Mem_Cmp((u08 *) Name, (u08 *) Expected, Bytes) == 0;
	b08	  Ending = Name[Bytes] == '.' || Name[Bytes] == '\0';
	return Same && Ending;
}

internal u32
Elf_GnuHash(c08 *Name)
{
	u32 Hash = 5381;
	while (*Name) {
		Hash = (Hash << 5) + Hash + *Name;
		Name++;
	}
	return Hash;
}

internal vptr
Elf_LookupSymbol_Linear(elf_state *State, c08 *Name)
{
	return NULL;
}

internal vptr
Elf_LookupSymbol_GnuHash(elf_state *State, c08 *Name)
{
	elf_gnu_hash_table *Table = &State->GnuHashTable;

	u32 Hash  = Elf_GnuHash(Name);
	u32 Hash2 = Hash >> Table->BloomShift;

	if (State->Header->Ident.FileClass == 32) {
		u32 N = Table->Bloom32[(Hash >> 5) & (Table->BloomSize - 1)];
		u32 B = (1 << (Hash & 31)) | (1 << (Hash2 & 31));
		if ((N & B) != B) return NULL;
	} else {
		u64 N = Table->Bloom64[(Hash >> 6) & (Table->BloomSize - 1)];
		u64 B = (1ull << (Hash & 63)) | (1ull << (Hash2 & 63));
		if ((N & B) != B) return NULL;
	}

	elf_section_header *SymtabHeader =
		Elf_GetSectionHeader(State, Table->Header->Link);
	elf_symbol *Symtab = (vptr) State->File + SymtabHeader->Offset;

	elf_section_header *StrtabHeader =
		Elf_GetSectionHeader(State, SymtabHeader->Link);
	c08 *Strtab = (vptr) State->File + StrtabHeader->Offset;

	u32	 SymIndex	= Table->Buckets[Hash % Table->BucketCount];
	u32 *ChainEntry = Table->Chain + SymIndex - Table->SymbolOffset;

	while (1) {
		if ((Hash >> 1) == (*ChainEntry >> 1)) {
			elf_symbol *Symbol	= Symtab + SymIndex;
			c08		   *SymName = Strtab + Symbol->Name;
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
	switch (State->Header->Machine) {
		case ELF_MACHINE_X86_64: return 0;	// X86_64 uses only Rela
		default				   : return 0;
	}
}

internal usize
Elf_ComputeRelocation(elf_state *State, usize Type, usize A, usize S, usize B)
{
	usize V = 0;
	switch (State->Header->Machine) {
		case ELF_MACHINE_X86_64: {
			switch (Type) {
				case ELF_RELOCATION_X86_64_64		: V = S + A; break;
				case ELF_RELOCATION_X86_64_GLOB_DAT : V = S; break;
				case ELF_RELOCATION_X86_64_JUMP_SLOT: V = S; break;
				case ELF_RELOCATION_X86_64_RELATIVE : V = B + A; break;
				default								: Assert(FALSE);
			}
		} break;
	}
	return V;
}

internal void
Elf_ApplyRelocation(elf_state *State, vptr Target, usize Type, usize Value)
{
	u64 *V64 = Target;
	switch (State->Header->Machine) {
		case ELF_MACHINE_X86_64: {
			switch (Type) {
				case ELF_RELOCATION_X86_64_64		: *V64 = Value; break;
				case ELF_RELOCATION_X86_64_GLOB_DAT : *V64 = Value; break;
				case ELF_RELOCATION_X86_64_JUMP_SLOT: *V64 = Value; break;
				case ELF_RELOCATION_X86_64_RELATIVE : *V64 = Value; break;
				default								: Assert(FALSE);
			}
		} break;
	}
}

internal elf_error
Elf_HandleRelocations(elf_state *State)
{
	for (usize I = 0; I < State->SectionHeaderCount; I++) {
		elf_section_header *Header = Elf_GetSectionHeader(State, I);
		vptr				Data   = State->File + Header->Offset;
		char *Name = (char *) (State->SectionNameTable + Header->Name);

		b08 IsRela = Elf_CheckName(Name, ".rela");
		b08 IsRel  = !IsRela && Elf_CheckName(Name, ".rel");

		if (!IsRela && !IsRel) continue;

		elf_section_header *SymtabHeader =
			Elf_GetSectionHeader(State, Header->Link);
		vptr Symtab = State->File + SymtabHeader->Offset;

		elf_section_header *TargetHeader =
			Elf_GetSectionHeader(State, Header->Info);
		vptr TargetBase = State->File + TargetHeader->Offset;

		elf_section_header *StrtabHeader =
			Elf_GetSectionHeader(State, SymtabHeader->Link);
		vptr Strtab = State->File + StrtabHeader->Offset;

		vptr		PrevTarget = NULL, Cursor = Data;
		usize		Value = 0, RelocType = 0;
		elf_symbol *Symbol = NULL;
		while (Cursor < Data + Header->Size) {
			elf_relocation	 *Rel  = Cursor;
			elf_relocation_a *Rela = Cursor;

			vptr Target;
			Target = State->ImageAddress - State->VAddrOffset + Rel->Offset;

			if (PrevTarget != Target) {
				if (PrevTarget) {
					c08 *Name = (c08 *) Strtab + Symbol->Name;
					Elf_ApplyRelocation(State, PrevTarget, RelocType, Value);
				}

				if (IsRela) Value = Rela->Addend;
				else Value = Elf_ExtractAddend(State, RelocType);
			}

			usize SymbolIndex;
			if (State->Header->Ident.FileClass == ELF_CLASS_32) {
				SymbolIndex = Rela->Info >> 8;
				RelocType	= Rela->Info & 0xFF;
			} else {
				SymbolIndex = Rela->Info >> 32;
				RelocType	= Rela->Info & 0xFFFFFFFF;
			}
			Symbol =
				(elf_symbol *) (Symtab + SymtabHeader->EntrySize * SymbolIndex);

			Value = Elf_ComputeRelocation(
				State,
				RelocType,
				Value,
				(usize) State->ImageAddress
					- State->VAddrOffset
					+ Symbol->Value,
				(usize) State->ImageAddress
			);

			PrevTarget	= Target;
			Cursor	   += Header->EntrySize;
		}

		if (PrevTarget)
			Elf_ApplyRelocation(State, PrevTarget, RelocType, Value);
	}

	return ELF_ERROR_SUCCESS;
}

internal elf_error
Elf_LoadSections(elf_state *State, vptr LoadAddress)
{
	if (State->Header->Type == ELF_TYPE_DYNAMIC) {
		usize MinVAddr = (usize) -1;
		usize MaxVAddr = 0;

		for (usize I = 0; I < State->Header->ProgramHeaderCount; I++) {
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

		// Reserve the overall space
		usize PageMask		= State->PageSize - 1;
		MinVAddr		   &= ~PageMask;
		State->VAddrOffset	= MinVAddr;
		State->ImageSize	= MaxVAddr - MinVAddr;
		vptr Base = State->ImageAddress = Sys_MemMap(
			LoadAddress,
			(State->ImageSize + PageMask) & ~PageMask,
			SYS_PROT_NONE,
			SYS_MAP_PRIVATE | SYS_MAP_ANONYMOUS | SYS_MAP_NORESERVE,
			-1,
			0
		);
		if ((usize) Base & PageMask) return ELF_ERROR_OUT_OF_MEMORY;
		if (Base && Base != LoadAddress) return ELF_ERROR_INVALID_MEM_MAP;

		for (usize I = 0; I < State->Header->ProgramHeaderCount; I++) {
			elf_program_header *Header = Elf_GetProgramHeader(State, I);
			switch (Header->Type) {
				case ELF_SEGMENT_TYPE_LOAD: {
					usize Padding  = Header->VirtualAddress & PageMask;
					vptr  ProgBase = Base + (Header->VirtualAddress - MinVAddr);
					s32	  Prot	   = 0
							 | ((Header->Flags & ELF_SEGMENT_FLAG_READ)
									? SYS_PROT_READ
									: 0)
							 | ((Header->Flags & ELF_SEGMENT_FLAG_WRITE)
									? SYS_PROT_WRITE
									: 0)
							 | ((Header->Flags & ELF_SEGMENT_FLAG_EXEC)
									? SYS_PROT_EXEC
									: 0);
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
						if ((usize) Addr & PageMask)
							return ELF_ERROR_INVALID_MEM_MAP;
					}

					// Try to map the nobits portion into memory
					if (Header->MemSize > Header->FileSize) {
						usize NoBitsPadding =
							State->PageSize
							- (((usize) ProgBase + Header->FileSize)
							   & PageMask);
						_Mem_Set(ProgBase + Header->FileSize, 0, NoBitsPadding);

						if (Header->MemSize > Header->FileSize + NoBitsPadding)
						{
							vptr  NoBitsBase  = (vptr) (((usize) ProgBase
														 + Header->FileSize
														 + PageMask)
														& ~PageMask);
							usize NoBitsExtra = Header->MemSize
											  - Header->FileSize
											  - NoBitsPadding;
							vptr Addr = Sys_MemMap(
								NoBitsBase,
								NoBitsExtra,
								Prot,
								Flags | SYS_MAP_ANONYMOUS,
								-1,
								0
							);
							if ((usize) Addr & PageMask)
								return ELF_ERROR_INVALID_MEM_MAP;
							_Mem_Set(NoBitsBase, 0, NoBitsExtra);
						}
					}
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
	for (usize I = 0; I < State->Header->ProgramHeaderCount; I++) {
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
Elf_SetupAndValidate(elf_state *State)
{
	State->Header = (vptr) State->File;

	if (_Mem_Cmp(State->Header->Ident.MagicNumber, (u08 *) ELF_MAGIC_NUMBER, 4)
		!= EQUAL)
		return ELF_ERROR_INVALID_FORMAT;
	if (State->Header->Ident.FileClass != ELF_EXPECTED_CLASS)
		return ELF_ERROR_NOT_SUPPORTED;
	if (State->Header->Ident.DataEncoding != ELF_EXPECTED_ENCODING)
		return ELF_ERROR_NOT_SUPPORTED;
	if (State->Header->Ident.FileVersion != ELF_EXPECTED_VERSION)
		return ELF_ERROR_NOT_SUPPORTED;

	if (State->Header->Type != ELF_TYPE_DYNAMIC) return ELF_ERROR_NOT_SUPPORTED;
	if (State->Header->Machine != ELF_EXPECTED_MACHINE)
		return ELF_ERROR_NOT_SUPPORTED;
	if (State->Header->Version != ELF_EXPECTED_VERSION)
		return ELF_ERROR_NOT_SUPPORTED;
	if (State->Header->ElfHeaderSize < sizeof(elf_header))
		return ELF_ERROR_INVALID_FORMAT;
	if (State->Header->SectionHeaderSize < sizeof(elf_section_header))
		return ELF_ERROR_INVALID_FORMAT;
	if (State->Header->ProgramHeaderSize < sizeof(elf_program_header))
		return ELF_ERROR_INVALID_FORMAT;

	State->SectionHeaderCount = State->Header->SectionHeaderCount;
	if (State->SectionHeaderCount || State->Header->SectionHeaderTableOffset) {
		State->SectionHeaderTable =
			State->File + State->Header->SectionHeaderTableOffset;

		State->NullSectionHeader = Elf_GetSectionHeader(State, 0);
		if (State->NullSectionHeader->Name) return ELF_ERROR_INVALID_FORMAT;
		if (State->NullSectionHeader->Type != ELF_SECTION_TYPE_NULL)
			return ELF_ERROR_INVALID_FORMAT;
		if (State->NullSectionHeader->Flags) return ELF_ERROR_INVALID_FORMAT;
		if (State->NullSectionHeader->Address) return ELF_ERROR_INVALID_FORMAT;
		if (State->NullSectionHeader->Offset) return ELF_ERROR_INVALID_FORMAT;
		if (State->NullSectionHeader->Info) return ELF_ERROR_INVALID_FORMAT;
		if (State->NullSectionHeader->AddressAlignment)
			return ELF_ERROR_INVALID_FORMAT;
		if (State->NullSectionHeader->EntrySize)
			return ELF_ERROR_INVALID_FORMAT;

		if (!State->SectionHeaderCount)
			State->SectionHeaderCount = State->NullSectionHeader->Size;
		else if (State->NullSectionHeader->Size)
			return ELF_ERROR_INVALID_FORMAT;

		u64 Index = State->Header->SectionNameTableIndex;
		if (Index == ELF_SECTION_INDEX_EXTENDED)
			Index = State->NullSectionHeader->Link;
		else if (Index >= ELF_SECTION_INDEX_LORESERVE)
			return ELF_ERROR_INVALID_FORMAT;
		else if (State->NullSectionHeader->Link)
			return ELF_ERROR_INVALID_FORMAT;
		State->SectionNameSectionHeader = Elf_GetSectionHeader(State, Index);
		State->SectionNameTable =
			(c08 *) Elf_GetSection(State, State->SectionNameSectionHeader);

		for (usize I = 0; I < State->SectionHeaderCount; I++) {
			elf_section_header *Header = Elf_GetSectionHeader(State, I);
			c08				   *Name   = State->SectionNameTable + Header->Name;
			vptr				Data   = State->File + Header->Offset;

			if (Elf_CheckName(Name, ".text")) {
				State->TextVAddr = Header->Address;
			} else if (Elf_CheckName(Name, ".gnu.hash")) {
				if (State->LookupType < ELF_LOOKUP_GNU_HASH) {
					State->LookupType = ELF_LOOKUP_GNU_HASH;

					State->GnuHashTable.Header		 = Header;
					State->GnuHashTable.BucketCount	 = ((u32 *) Data)[0];
					State->GnuHashTable.SymbolOffset = ((u32 *) Data)[1];
					State->GnuHashTable.BloomSize	 = ((u32 *) Data)[2];
					State->GnuHashTable.BloomShift	 = ((u32 *) Data)[3];
					State->GnuHashTable.Bloom32		 = (u32 *) Data + 4;

					usize BloomWords =
						State->GnuHashTable.BloomSize
						* ((State->Header->Ident.FileClass == ELF_CLASS_32)
							   ? 1
							   : 2);

					State->GnuHashTable.Buckets =
						State->GnuHashTable.Bloom32 + BloomWords;
					State->GnuHashTable.Chain = State->GnuHashTable.Buckets
											  + State->GnuHashTable.BucketCount;
				}
			}
		}
	} else {
		if (State->Header->SectionNameTableIndex != ELF_SECTION_INDEX_UNDEF)
			return ELF_ERROR_INVALID_FORMAT;
	}

	if (State->Header->ProgramHeaderCount) {
		State->ProgramHeaderTable =
			State->File + State->Header->ProgramHeaderTableOffset;

		b08 FoundLoadable	   = FALSE;
		b08 FoundPHDR		   = FALSE;
		b08 FoundInterp		   = FALSE;
		b08 NextMustBeLoadable = FALSE;

		for (u32 I = 0; I < State->Header->ProgramHeaderCount; I++) {
			elf_program_header *Header = Elf_GetProgramHeader(State, I);
			if (NextMustBeLoadable) {
				if (Header->Type != ELF_SEGMENT_TYPE_LOAD)
					return ELF_ERROR_INVALID_FORMAT;
				NextMustBeLoadable = FALSE;
			}
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
					FoundInterp		   = TRUE;
					NextMustBeLoadable = TRUE;
					break;
				case ELF_SEGMENT_TYPE_NOTE: break;
				case ELF_SEGMENT_TYPE_SHLIB:
					return ELF_ERROR_INVALID_FORMAT;
					break;
				case ELF_SEGMENT_TYPE_PHDR:
					if (FoundLoadable) return ELF_ERROR_INVALID_FORMAT;
					if (FoundPHDR) return ELF_ERROR_INVALID_FORMAT;
					FoundPHDR		   = TRUE;
					NextMustBeLoadable = TRUE;
					break;
				case ELF_SEGMENT_TYPE_TLS: break;
			}
		}
	} else {
		if (State->Header->ProgramHeaderTableOffset)
			return ELF_ERROR_INVALID_FORMAT;
	}

	return ELF_ERROR_SUCCESS;
}

/* ========== EXTERNAL API ========== */

internal elf_error
Elf_Open(elf_state *State, c08 *FileName, usize PageSize)
{
	// TODO: Don't allocate the entire file, just map what's needed

	if (!State || !FileName) return ELF_ERROR_INVALID_ARGUMENT;
	if (State->State != ELF_STATE_CLOSED) return ELF_ERROR_INVALID_OPERATION;

	*State			= (elf_state) { 0 };
	State->PageSize = PageSize;

	State->FileDescriptor = Sys_Open(FileName, SYS_OPEN_READONLY, 0);
	if (!CHECK((s32) State->FileDescriptor)) return ELF_ERROR_UNKNOWN;

	sys_stat Stat;
	if (!CHECK(Sys_FileStat(State->FileDescriptor, &Stat)))
		return ELF_ERROR_UNKNOWN;
	State->FileSize = Stat.Size;

	State->File = Sys_MemMap(
		NULL,
		State->FileSize,
		SYS_PROT_READ,
		SYS_MAP_PRIVATE,
		State->FileDescriptor,
		0
	);
	if (!CHECK(State->File)) return ELF_ERROR_UNKNOWN;

	elf_error Error = Elf_SetupAndValidate(State);
	if (Error) return Error;

	State->State = ELF_STATE_OPENED;
	return ELF_ERROR_SUCCESS;
}

internal elf_error
Elf_LoadProgram(elf_state *State, vptr LoadAddress)
{
	if (!State) return ELF_ERROR_INVALID_ARGUMENT;
	if (State->State != ELF_STATE_OPENED) return ELF_ERROR_INVALID_OPERATION;

	elf_error Error = Elf_LoadSections(State, LoadAddress);
	if (Error) return Error;

	Error = Elf_HandleRelocations(State);
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

	s32 SysError  = Sys_MemUnmap(State->File, State->FileSize);
	SysError	 |= Sys_Close(State->FileDescriptor);
	if (!CHECK(SysError)) return ELF_ERROR_UNKNOWN;

	State->State = ELF_STATE_CLOSED;
	return ELF_ERROR_SUCCESS;
}
