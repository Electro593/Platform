/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
**                                                                         **
**  Author: Aria Seiler                                                    **
**                                                                         **
**  This program is in the public domain. There is no implied warranty,    **
**  so use it at your own risk.                                            **
**                                                                         **
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

internal elf_section_header*
Elf_GetSectionHeader(elf_state *State, u64 Index)
{
	if (!State->SectionHeaderTable) return 0;
	return (vptr) (State->SectionHeaderTable + State->Header->SectionHeaderSize * Index);
}

internal u08*
Elf_GetSection(elf_state *State, elf_section_header *Header)
{
	if (!Header) return 0;
	if (Header->Type == ELF_SECTION_TYPE_NULL) return 0;
	if (Header->Type == ELF_SECTION_TYPE_NOBITS) return 0;
	return State->File + Header->Offset;
}

internal elf_program_header*
Elf_GetProgramHeader(elf_state *State, u64 Index)
{
	if (!State->ProgramHeaderTable) return 0;
	return (vptr) (State->ProgramHeaderTable + State->Header->ProgramHeaderSize * Index);
}

internal elf_error
Elf_SetupAndValidate(elf_state *State)
{
	State->Header = (vptr) State->File;

	if (_Mem_Cmp(State->Header->Ident.MagicNumber, (u08*) ELF_MAGIC_NUMBER, 4) != EQUAL) return ELF_ERROR_INVALID_FORMAT;
	if (State->Header->Ident.FileClass != ELF_EXPECTED_CLASS) return ELF_ERROR_NOT_SUPPORTED;
	if (State->Header->Ident.DataEncoding != ELF_EXPECTED_ENCODING) return ELF_ERROR_NOT_SUPPORTED;
	if (State->Header->Ident.FileVersion != ELF_EXPECTED_VERSION) return ELF_ERROR_NOT_SUPPORTED;

	if (State->Header->Type != ELF_TYPE_DYNAMIC) return ELF_ERROR_NOT_SUPPORTED;
	if (State->Header->Machine != ELF_EXPECTED_MACHINE) return ELF_ERROR_NOT_SUPPORTED;
	if (State->Header->Version != ELF_EXPECTED_VERSION) return ELF_ERROR_NOT_SUPPORTED;
	if (State->Header->ElfHeaderSize < sizeof(elf_header)) return ELF_ERROR_INVALID_FORMAT;
	if (State->Header->SectionHeaderSize < sizeof(elf_section_header)) return ELF_ERROR_INVALID_FORMAT;
	if (State->Header->ProgramHeaderSize < sizeof(elf_program_header)) return ELF_ERROR_INVALID_FORMAT;

	State->SectionHeaderCount = State->Header->SectionHeaderCount;
	if (State->SectionHeaderCount || State->Header->SectionHeaderTableOffset) {
		State->SectionHeaderTable = State->File + State->Header->SectionHeaderTableOffset;

		State->NullSectionHeader = Elf_GetSectionHeader(State, 0);
		if (State->NullSectionHeader->Name) return ELF_ERROR_INVALID_FORMAT;
		if (State->NullSectionHeader->Type != ELF_SECTION_TYPE_NULL) return ELF_ERROR_INVALID_FORMAT;
		if (State->NullSectionHeader->Flags) return ELF_ERROR_INVALID_FORMAT;
		if (State->NullSectionHeader->Address) return ELF_ERROR_INVALID_FORMAT;
		if (State->NullSectionHeader->Offset) return ELF_ERROR_INVALID_FORMAT;
		if (State->NullSectionHeader->Info) return ELF_ERROR_INVALID_FORMAT;
		if (State->NullSectionHeader->AddressAlignment) return ELF_ERROR_INVALID_FORMAT;
		if (State->NullSectionHeader->EntrySize) return ELF_ERROR_INVALID_FORMAT;

		if (!State->SectionHeaderCount) State->SectionHeaderCount = State->NullSectionHeader->Size;
		else if (State->NullSectionHeader->Size) return ELF_ERROR_INVALID_FORMAT;

		u64 Index = State->Header->SectionNameTableIndex;
		if (Index == ELF_SECTION_INDEX_EXTENDED) Index = State->NullSectionHeader->Link;
		else if (Index >= ELF_SECTION_INDEX_LORESERVE) return ELF_ERROR_INVALID_FORMAT;
		else if (State->NullSectionHeader->Link) return ELF_ERROR_INVALID_FORMAT;
		State->SectionNameSectionHeader = Elf_GetSectionHeader(State, Index);
		State->SectionNameTable = Elf_GetSection(State, State->SectionNameSectionHeader);
	} else {
		if (State->Header->SectionNameTableIndex != ELF_SECTION_INDEX_UNDEF) return ELF_ERROR_INVALID_FORMAT;
	}

	if (State->Header->ProgramHeaderCount) {
		State->ProgramHeaderTable = State->File + State->Header->ProgramHeaderTableOffset;

		u64 MaxLoadableVAddress = 0;
		b08 FoundLoadable = FALSE;
		b08 FoundPHDR = FALSE;
		b08 FoundInterp = FALSE;
		b08 NextMustBeLoadable = FALSE;

		for (u32 I = 0; I < State->Header->ProgramHeaderCount; I++) {
			elf_program_header *Header = Elf_GetProgramHeader(State, I);
			if (NextMustBeLoadable) {
				if (Header->Type != ELF_SEGMENT_TYPE_LOAD) return ELF_ERROR_INVALID_FORMAT;
				NextMustBeLoadable = FALSE;
			}
			switch (Header->Type) {
				case ELF_SEGMENT_TYPE_NULL:
					break;
				case ELF_SEGMENT_TYPE_LOAD:
					if (Header->FileSize > Header->MemSize) return ELF_ERROR_INVALID_FORMAT;
					if (Header->VirtualAddress < MaxLoadableVAddress) return ELF_ERROR_INVALID_FORMAT;
					MaxLoadableVAddress = Header->VirtualAddress;
					break;
				case ELF_SEGMENT_TYPE_DYNAMIC:
					break;
				case ELF_SEGMENT_TYPE_INTERP:
					if (FoundLoadable) return ELF_ERROR_INVALID_FORMAT;
					if (FoundInterp) return ELF_ERROR_INVALID_FORMAT;
					FoundInterp = TRUE;
					NextMustBeLoadable = TRUE;
					break;
				case ELF_SEGMENT_TYPE_NOTE:
					break;
				case ELF_SEGMENT_TYPE_SHLIB:
					return ELF_ERROR_INVALID_FORMAT;
					break;
				case ELF_SEGMENT_TYPE_PHDR:
					if (FoundLoadable) return ELF_ERROR_INVALID_FORMAT;
					if (FoundPHDR) return ELF_ERROR_INVALID_FORMAT;
					FoundPHDR = TRUE;
					NextMustBeLoadable = TRUE;
					break;
				case ELF_SEGMENT_TYPE_TLS:
					break;
			}
		}
	} else {
		if (State->Header->ProgramHeaderTableOffset) return ELF_ERROR_INVALID_FORMAT;
	}

	return ELF_ERROR_SUCCESS;
}

/* ========== EXTERNAL API ========== */

internal elf_error
Elf_Open(elf_state *State, c08 *FileName)
{
	if (!State || !FileName) return ELF_ERROR_INVALID_ARGUMENT;
	if (State->State != ELF_STATE_CLOSED) return ELF_ERROR_INVALID_OPERATION;

	*State = (elf_state){0};

	State->FileDescriptor = Sys_Open(FileName, SYS_OPEN_READONLY, 0);
	if (!CHECK((s32) State->FileDescriptor)) return ELF_ERROR_UNKNOWN;

	sys_stat Stat;
	if (!CHECK(Sys_FileStat(State->FileDescriptor, &Stat))) return ELF_ERROR_UNKNOWN;
	State->FileSize = Stat.Size;

	State->File = Sys_MemMap(NULL, State->FileSize, SYS_PROT_READ, SYS_MAP_PRIVATE, State->FileDescriptor, 0);
	if (!CHECK(State->File)) return ELF_ERROR_UNKNOWN;

	elf_error Error = Elf_SetupAndValidate(State);
	if (Error) return Error;

	State->State = ELF_STATE_OPENED;
	return ELF_ERROR_SUCCESS;
}

internal elf_error
Elf_LoadProgram(elf_state *State)
{
	if (!State) return ELF_ERROR_INVALID_ARGUMENT;
	if (State->State != ELF_STATE_OPENED) return ELF_ERROR_INVALID_OPERATION;

	if (State->Header->Type == ELF_TYPE_DYNAMIC) {
		b08 FoundLoadable = FALSE;
		u64 MinVAddr;
		u64 MaxVAddr = 0;

		for (u32 I = 0; I < State->Header->ProgramHeaderCount; I++) {
			elf_program_header *Header = Elf_GetProgramHeader(State, I);
			switch (Header->Type) {
				case ELF_SEGMENT_TYPE_NULL: break;
				case ELF_SEGMENT_TYPE_LOAD: {
					if (!FoundLoadable) {
						FoundLoadable = TRUE;
						MinVAddr = Header->VirtualAddress;
					}
					if (MaxVAddr < Header->VirtualAddress + Header->MemSize)
						MaxVAddr = Header->VirtualAddress + Header->MemSize;
				} break;
				case ELF_SEGMENT_TYPE_DYNAMIC: break;
				case ELF_SEGMENT_TYPE_INTERP: break;
				case ELF_SEGMENT_TYPE_NOTE: break;
				case ELF_SEGMENT_TYPE_SHLIB: break;
				case ELF_SEGMENT_TYPE_PHDR: break;
				case ELF_SEGMENT_TYPE_TLS: break;
			}
		}

		if (!FoundLoadable) return ELF_ERROR_SUCCESS;

		State->ImageSize = MaxVAddr - MinVAddr;

		for (u32 I = 0; I < State->Header->ProgramHeaderCount; I++) {
			elf_program_header *Header = Elf_GetProgramHeader(State, I);
			switch (Header->Type) {
				case ELF_SEGMENT_TYPE_NULL: break;
				case ELF_SEGMENT_TYPE_LOAD: {
					if (!FoundLoadable) {
						FoundLoadable = TRUE;
						MinVAddr = Header->VirtualAddress;
					}
					if (MaxVAddr < Header->VirtualAddress + Header->MemSize)
						MaxVAddr = Header->VirtualAddress + Header->MemSize;
				} break;
				case ELF_SEGMENT_TYPE_DYNAMIC: break;
				case ELF_SEGMENT_TYPE_INTERP: break;
				case ELF_SEGMENT_TYPE_NOTE: break;
				case ELF_SEGMENT_TYPE_SHLIB: break;
				case ELF_SEGMENT_TYPE_PHDR: break;
				case ELF_SEGMENT_TYPE_TLS: break;
			}
		}
	} else {
		return ELF_ERROR_NOT_SUPPORTED;
	}

	State->State = ELF_STATE_LOADED;
	return ELF_ERROR_SUCCESS;
}

internal elf_error
Elf_GetProcAddress(elf_state *State, c08 *ProcName, vptr *ProcOut)
{
	if (!State || !ProcName || !ProcOut) return ELF_ERROR_INVALID_ARGUMENT;
	if (State->State != ELF_STATE_LOADED) return ELF_ERROR_INVALID_OPERATION;

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
	SysError |= Sys_Close(State->FileDescriptor);
	if (!CHECK(SysError)) return ELF_ERROR_UNKNOWN;

	State->State = ELF_STATE_CLOSED;
	return ELF_ERROR_SUCCESS;
}

#undef MAYBE_SWAP