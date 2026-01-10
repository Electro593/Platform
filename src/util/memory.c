/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                             *
*  Author: Aria Seiler                                                        *
*                                                                             *
*  This program is in the public domain. There is no implied warranty, so     *
*  use it at your own risk.                                                   *
*                                                                             *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef INCLUDE_HEADER

// TODO Make an actual document explaining how this mess works

#define HEAP(Type) heap_handle *

typedef struct heap_handle {
	vptr Data;
	u64	 Index	  : 16;
	u64	 Offset	  : 46;
	u64	 Free	  : 1;
	u64	 Anchored : 1;
	u32	 Size;
	u16	 PrevFree;
	u16	 NextFree;
	u16	 PrevUsed;
	u16	 NextUsed;
	u16	 PrevBlock;
	u16	 NextBlock;
} heap_handle;

typedef struct {
	u32 Mutex;

	heap_handle Handles[];
} heap;

typedef struct stack {
	u32	  Mutex;
	usize Size;
	u08	 *Cursor;
	vptr *FirstMarker;
} stack;

#define MEMORY_FUNCS \
   EXPORT(vptr,         Mem_Set,            vptr Dest, s32 Data, usize Size) \
   EXPORT(vptr,         Mem_Cpy,            vptr Dest, vptr Src, usize Size) \
   EXPORT(s32,          Mem_Cmp,            vptr A, vptr B, usize Size) \
   EXPORT(usize,        Mem_BytesUntil,     u08 *Data, u08 Byte) \
   \
   EXPORT(heap*,        Heap_GetHeap,       heap_handle *Handle) \
   EXPORT(heap_handle*, Heap_GetHandleA,    vptr Data) \
   EXPORT(heap*,        Heap_Init,          vptr MemBase, u64 Size) \
   EXPORT(void,         Heap_Defragment,    heap *Heap) \
   EXPORT(void,         Heap_AllocateBlock, heap *Heap, heap_handle *Handle, u32 Size) \
   EXPORT(void,         Heap_FreeBlock,     heap *Heap, heap_handle *Handle) \
   EXPORT(heap_handle*, _Heap_Allocate,     heap *Heap, u32 Size, b08 Anchored) \
   EXPORT(heap_handle*, Heap_Allocate,      heap *Heap, u64 Size) \
   EXPORT(vptr,         Heap_AllocateA,     heap *Heap, u64 Size) \
   EXPORT(void,         Heap_Resize,        heap_handle *Handle, u32 NewSize) \
   EXPORT(void,         Heap_ResizeA,       vptr *Data, u32 NewSize) \
   EXPORT(void,         Heap_Free,          heap_handle *Handle) \
   EXPORT(void,         Heap_FreeA,         vptr Data) \
   EXPORT(void,         Heap_Dump,          heap *Heap) \
   \
   EXPORT(stack*,       Stack_Init,         vptr Mem, usize Size) \
   EXPORT(stack,        Stack_Get,          void) \
   EXPORT(void,         Stack_Set,          stack Stack) \
   EXPORT(void,         Stack_Push,         void) \
   EXPORT(vptr,         Stack_GetCursor,    void) \
   EXPORT(void,         Stack_SetCursor,    vptr Cursor) \
   EXPORT(vptr,         Stack_Allocate,     u64 Size) \
   EXPORT(void,         Stack_Pop,          void)

#endif

#ifdef INCLUDE_SOURCE

internal vptr
Mem_Set(vptr Dest, s32 Data, usize Size)
{
	u08 *DB = (u08 *) Dest;
	u08	 SB = Data;

	switch (Size) {
		case 7: DB[6] = SB;	 // fallthrough
		case 6: DB[5] = SB;	 // fallthrough
		case 5: DB[4] = SB;	 // fallthrough
		case 4: DB[3] = SB;	 // fallthrough
		case 3: DB[2] = SB;	 // fallthrough
		case 2: DB[1] = SB;	 // fallthrough
		case 1: DB[0] = SB;	 // fallthrough
		case 0: return Dest;
	}

	usize WB  = sizeof(usize);
	usize AB  = (WB - ((usize) DB % WB)) % WB;
	Size	 -= AB;
	switch (AB) {
		case 7: DB[6] = SB;	 // fallthrough
		case 6: DB[5] = SB;	 // fallthrough
		case 5: DB[4] = SB;	 // fallthrough
		case 4: DB[3] = SB;	 // fallthrough
		case 3: DB[2] = SB;	 // fallthrough
		case 2: DB[1] = SB;	 // fallthrough
		case 1: DB[0] = SB;
	}
	DB += AB;

	usize *DW = (usize *) DB;
	usize  SW = SB | (SB << 8) | (SB << 16) | (SB << 24);
	if (WB == 8) SW |= SW << 32;

	while (Size >= WB) {
		*DW++  = SW;
		Size  -= WB;
	}

	DB = (u08 *) DW;
	switch (Size) {
		case 7: DB[6] = SB;	 // fallthrough
		case 6: DB[5] = SB;	 // fallthrough
		case 5: DB[4] = SB;	 // fallthrough
		case 4: DB[3] = SB;	 // fallthrough
		case 3: DB[2] = SB;	 // fallthrough
		case 2: DB[1] = SB;	 // fallthrough
		case 1: DB[0] = SB;
	}

	return Dest;
}

internal vptr
Mem_Cpy(vptr Dest, vptr Src, usize Size)
{
	if (Size == 0) return Dest;
	if (Dest == Src) return Dest;

	s08	 Dir	= 1;
	u08 *Dest08 = (u08 *) Dest;
	u08 *Src08	= (u08 *) Src;

	if (Dest > Src) {
		Dir		= -1;
		Dest08 += Size - 1;
		Src08  += Size - 1;
	}

	while (Size--) {
		*Dest08	 = *Src08;
		Dest08	+= Dir;
		Src08	+= Dir;
	}

	return Dest;
}

internal s32
Mem_Cmp(vptr A, vptr B, usize Size)
{
	u08 *APtr = A;
	u08 *BPtr = B;
	while (Size && *APtr++ == *BPtr++) Size--;
	if (!Size) return EQUAL;
	if ((usize) APtr > (usize) BPtr) return GREATER;
	return LESS;
}

internal usize
Mem_BytesUntil(u08 *Data, u08 Byte)
{
	usize Length = 0;
	while (*Data++ != Byte) Length++;
	return Length;
}

internal heap *
Heap_GetHeap(heap_handle *Handle)
{
	vptr Handles = Handle - Handle->Index;
	return (heap *) (Handles - OFFSET_OF(heap, Handles));
}

// TODO Resizeable heap

internal heap_handle *
Heap_GetHandleA(vptr Data)
{
	return *((heap_handle **) Data - 1);
}

internal heap *
Heap_Init(vptr MemBase, u64 Size)
{
	u32 HeaderSize = sizeof(heap) + sizeof(heap_handle);

	Assert(MemBase);
	Assert(Size > HeaderSize);
	Assert(Size - HeaderSize < (1ULL << 46));

	heap *Heap	= MemBase;
	Heap->Mutex = 0;

	heap_handle *NullUsedHandle = Heap->Handles;
	NullUsedHandle->Data		= (u08 *) NullUsedHandle;
	NullUsedHandle->Size		= sizeof(heap_handle);
	NullUsedHandle->Offset		= Size - HeaderSize;
	NullUsedHandle->Index		= 0;
	NullUsedHandle->PrevFree	= 0;
	NullUsedHandle->NextFree	= 0;
	NullUsedHandle->PrevUsed	= 0;
	NullUsedHandle->NextUsed	= 0;
	NullUsedHandle->PrevBlock	= 0;
	NullUsedHandle->NextBlock	= 0;
	NullUsedHandle->Anchored	= TRUE;
	NullUsedHandle->Free		= FALSE;

	return Heap;
}

internal void
Heap_Defragment(heap *Heap)
{
	STOP;  // Hasn't been debugged

	Assert(Heap);

	heap_handle *Handles = Heap->Handles;

	u64			 Offset = 0;
	heap_handle *Block	= Handles;
	do {
		Block = Handles + Block->PrevBlock;

		if (Block->Anchored) {
			Block->Offset += Offset;
			Offset		   = 0;
			continue;
		}

		Offset		  += Block->Offset;
		Block->Offset  = 0;

		if (Offset) {
			Mem_Cpy((u08 *) Block->Data + Offset, Block->Data, Block->Size);
			Block->Data = (u08 *) Block->Data + Offset;
		}
	} while (Block->Index);
}

internal void
Heap_AllocateBlock(heap *Heap, heap_handle *Handle, u32 Size)
{
	heap_handle *Handles   = Heap->Handles;
	heap_handle *PrevBlock = Handles + Handles[0].PrevBlock;
	while (PrevBlock->Index && PrevBlock->Offset < Size)
		PrevBlock = Handles + PrevBlock->PrevBlock;
	if (PrevBlock->Offset < Size) {
		Heap_Defragment(Heap);
		Assert(
			PrevBlock->Offset >= Size,
			"Not enough memory for new heap block"
		);
	}
	Handle->Data =
		(u08 *) PrevBlock->Data + PrevBlock->Size + PrevBlock->Offset - Size;
	PrevBlock->Offset					 -= Size;
	Handle->PrevBlock					  = PrevBlock->Index;
	Handle->NextBlock					  = PrevBlock->NextBlock;
	Handles[Handle->NextBlock].PrevBlock  = Handle->Index;
	Handles[Handle->PrevBlock].NextBlock  = Handle->Index;
	Handle->Offset						  = 0;
	Handle->Size						  = Size;
}

internal void
Heap_FreeBlock(heap *Heap, heap_handle *Handle)
{
	heap_handle *Handles				  = Heap->Handles;
	Handles[Handle->PrevBlock].Offset	 += Handle->Size + Handle->Offset;
	Handles[Handle->PrevBlock].NextBlock  = Handle->NextBlock;
	Handles[Handle->NextBlock].PrevBlock  = Handle->PrevBlock;
}

internal heap_handle *
_Heap_Allocate(heap *Heap, u32 Size, b08 Anchored)
{
	Assert(Heap);
	Platform_LockMutex(&Heap->Mutex);

	heap_handle *Handles	  = Heap->Handles;
	heap_handle *Handle		  = NULL;
	b08			 Defragmented = FALSE;

	heap_handle *PrevUsed;
	if (Handles[0].NextFree == 0) {
		if (Handles[0].Offset < sizeof(heap_handle)) {
			Heap_Defragment(Heap);
			Defragmented = TRUE;
			Assert(
				Handles[0].Offset >= sizeof(heap_handle),
				"Not enough memory for new heap handle"
			);
		}

		u16 HandleCount = (u16) (Handles[0].Size / sizeof(heap_handle));
		Handle			= Handles + HandleCount;
		Handle->Index	= HandleCount;

		Handles[0].Size	  += sizeof(heap_handle);
		Handles[0].Offset -= sizeof(heap_handle);

		PrevUsed = Handles + Handles[0].PrevUsed;
	} else {
		Handle							   = Handles + Handles[0].NextFree;
		Handles[Handle->PrevFree].NextFree = Handle->NextFree;
		Handles[Handle->NextFree].PrevFree = Handle->PrevFree;

		PrevUsed = Handle;
		while (PrevUsed->Index && PrevUsed->Free) PrevUsed--;
	}
	Handle->PrevUsed				   = PrevUsed->Index;
	Handle->NextUsed				   = PrevUsed->NextUsed;
	Handle->PrevFree				   = 0;
	Handle->NextFree				   = 0;
	Handles[Handle->PrevUsed].NextUsed = Handle->Index;
	Handles[Handle->NextUsed].PrevUsed = Handle->Index;
	Handle->Anchored				   = Anchored;
	Handle->Free					   = FALSE;

	Heap_AllocateBlock(Heap, Handle, Size);

	Platform_UnlockMutex(&Heap->Mutex);
	return Handle;
}

internal heap_handle *
Heap_Allocate(heap *Heap, u64 Size)
{
	return _Heap_Allocate(Heap, Size, FALSE);
}

internal vptr
Heap_AllocateA(heap *Heap, u64 Size)
{
	heap_handle *Handle =
		_Heap_Allocate(Heap, Size + sizeof(heap_handle *), TRUE);
	*((heap_handle **) Handle->Data) = Handle;
	return (u08 *) Handle->Data + sizeof(heap_handle *);
}

internal void
Heap_Resize(heap_handle *Handle, u32 NewSize)
{
	Assert(Handle);

	heap *Heap = Heap_GetHeap(Handle);
	Platform_LockMutex(&Heap->Mutex);

	if (NewSize <= Handle->Size + Handle->Offset) {
		Handle->Offset += (s64) Handle->Size - (s64) NewSize;
		Handle->Size	= NewSize;
	} else {
		u08 *PrevData = Handle->Data;
		u32	 PrevSize = Handle->Size;
		Heap_FreeBlock(Heap, Handle);
		Heap_AllocateBlock(Heap, Handle, NewSize);
		Mem_Cpy(Handle->Data, PrevData, PrevSize);
	}

	Platform_UnlockMutex(&Heap->Mutex);
}

internal void
Heap_ResizeA(vptr *Data, u32 NewSize)
{
	heap_handle *Handle = Heap_GetHandleA(*Data);
	Heap_Resize(Handle, NewSize + sizeof(heap_handle *));
	*Data = (u08 *) Handle->Data + sizeof(heap_handle *);
}

internal void
Heap_Free(heap_handle *Handle)
{
	Assert(Handle);
	heap		*Heap	 = Heap_GetHeap(Handle);
	heap_handle *Handles = Heap->Handles;
	Platform_LockMutex(&Heap->Mutex);

	Heap_FreeBlock(Heap, Handle);

	Handles[Handle->NextUsed].PrevUsed = Handle->PrevUsed;
	Handles[Handle->PrevUsed].NextUsed = Handle->NextUsed;

	if (Handle->NextUsed == 0) {
		heap_handle *PrevFree = Handles + Handle->PrevUsed;
		while (PrevFree->Index && !PrevFree->Free) PrevFree--;
		Handles[0].PrevFree = PrevFree->Index;
		PrevFree->NextFree	= 0;

		u16 Offset		   = Handle->PrevUsed + 1;
		u64 NewSize		   = Offset * sizeof(heap_handle);
		u64 DeltaSize	   = Handles[0].Size - NewSize;
		Handles[0].Size	   = NewSize;
		Handles[0].Offset += DeltaSize;
		Mem_Set(Handles + Offset, 0, DeltaSize);
	} else {
		heap_handle *PrevFree = Handle;
		while (PrevFree->Index && !PrevFree->Free) PrevFree--;

		Handle->Data					   = NULL;
		Handle->Offset					   = 0;
		Handle->Free					   = TRUE;
		Handle->Anchored				   = FALSE;
		Handle->Size					   = 0;
		Handle->PrevFree				   = PrevFree->Index;
		Handle->NextFree				   = PrevFree->NextFree;
		Handle->PrevUsed				   = 0;
		Handle->NextUsed				   = 0;
		Handle->PrevBlock				   = 0;
		Handle->NextBlock				   = 0;
		Handles[Handle->NextFree].PrevFree = Handle->Index;
		Handles[Handle->PrevFree].NextFree = Handle->Index;
	}

	Platform_UnlockMutex(&Heap->Mutex);
}

internal void
Heap_FreeA(vptr Data)
{
	Heap_Free(Heap_GetHandleA(Data));
}

internal void
Heap_Dump(heap *Heap)
{
	Platform_LockMutex(&Heap->Mutex);

	file_handle FileHandle;
	Platform_OpenFile(&FileHandle, "heap_dump.txt", FILE_WRITE);
	u32 FileOffset = 0;

	heap_handle *Handles	 = Heap->Handles;
	u32			 HandleCount = Handles[0].Size / sizeof(heap_handle);

	Stack_Push();
	{
		string NoneStr = CString("");

		string Line1 = NoneStr;
		string Line2 = NoneStr;
		string Line3 = NoneStr;
		string Line4 = NoneStr;

		heap_handle *Handle = Handles;
		do {
			if (Handle->Size == 0) goto next;

			u32 Index;
			Intrin_BitScanReverse32(&Index, Handle->Size);
			u32 SizeWidth = (Index < 3) ? 1 : Index - 1;

			b08 IsValid		= Intrin_BitScanReverse64(&Index, Handle->Offset);
			u32 OffsetWidth = IsValid * ((Index < 3) ? 1 : Index - 1);

			string Size	  = FStringL("%'d", Handle->Size);
			string Offset = FStringL("%'Ld", Handle->Offset);

			c08 BorderChar = (Handle->Anchored) ? '=' : '-';

			string PlusStr = IsValid ? CStringL("+") : NoneStr;
			string PipeStr = IsValid ? CStringL("|") : NoneStr;
			string Seg1	   = FStringL(
				   ".%*s%s%2$-*s.",
				   SizeWidth,
				   NoneStr,
				   PlusStr,
				   OffsetWidth
			   );
			string Seg2 = FStringL(
				":%*s%s%2$-*s:",
				SizeWidth,
				NoneStr,
				PipeStr,
				OffsetWidth
			);
			string Seg3 = FStringL(
				":%*s%-*s%s%*.*s%-*s:",
				(SizeWidth + Size.Length) / 2,
				Size,
				(SizeWidth - Size.Length + 1) / 2,
				NoneStr,
				PipeStr,
				(OffsetWidth + Offset.Length) / 2,
				OffsetWidth,
				Offset,
				(OffsetWidth - Offset.Length + 1) / 2,
				NoneStr
			);
			string Seg4 = FStringL(
				"'%*s%s%2$-*s'",
				SizeWidth,
				NoneStr,
				PlusStr,
				OffsetWidth
			);

			Mem_Set(Seg1.Text + 1, BorderChar, SizeWidth);
			Mem_Set(Seg1.Text + 1 + SizeWidth + 1, BorderChar, OffsetWidth);
			Mem_Set(Seg2.Text + 1, '#', SizeWidth);
			Mem_Set(Seg4.Text + 1, BorderChar, SizeWidth);
			Mem_Set(Seg4.Text + 1 + SizeWidth + 1, BorderChar, OffsetWidth);

			Line1 = FStringL("%s%s", Line1, Seg1);
			Line2 = FStringL("%s%s", Line2, Seg2);
			Line3 = FStringL("%s%s", Line3, Seg3);
			Line4 = FStringL("%s%s", Line4, Seg4);

		next:
			Handle = Handles + Handle->NextBlock;
		} while (Handle->Index);

		string String = FStringL(
			"\n%s\n%s\n%s\n%s\n%s\n",
			Line1,
			Line2,
			Line3,
			Line2,
			Line4
		);
		Platform_WriteFile(FileHandle, String.Text, String.Length, 0);
		FileOffset += String.Length;
	}
	Stack_Pop();

	Stack_Push();
	{
		string NoneStr = CStringL("");

		string DataStr	   = CStringL("Data:");
		string FlagsStr	   = CStringL("Status:");
		string HandlesStr  = CStringL("Handles:");
		string SizeStr	   = CStringL("Size:");
		string OffsetStr   = CStringL("Offset:");
		u32	   LabelWidthU = MAX(DataStr.Length, FlagsStr.Length);
		LabelWidthU		   = MAX(HandlesStr.Length, LabelWidthU);
		LabelWidthU		   = MAX(SizeStr.Length, LabelWidthU);
		LabelWidthU		   = MAX(OffsetStr.Length, LabelWidthU);
		s32 LabelWidth	   = -(s32) LabelWidthU;

		string FreeStr		 = CStringL("Free");
		string UsedStr		 = CStringL("Used");
		string IsAnchoredStr = CStringL(", Anchored");

		s32 Width = 0;
		s32 CurrWidth;

		string String = NoneStr;

		for (u32 I = 0; I < HandleCount; I++) {
			string Name, Data, Size, Offset, Flags;

			if (I == 0) Name = FStringL("HEAP (Handle 0)%n", &CurrWidth);
			else Name = FStringL("Handle %'d%n", Handles[I].Index, &CurrWidth);
			if (Width < CurrWidth) Width = CurrWidth;

			Data = FStringL(
				"%*s %p%n",
				LabelWidth,
				DataStr,
				Handles[I].Data,
				&CurrWidth
			);
			if (Width < CurrWidth) Width = CurrWidth;

			string UsageStr = (Handles[I].Free) ? FreeStr : UsedStr;
			string AnchoredStr =
				(Handles[I].Anchored) ? IsAnchoredStr : NoneStr;
			Flags = FStringL(
				"%*s %s%s%n",
				LabelWidth,
				FlagsStr,
				UsageStr,
				AnchoredStr,
				&CurrWidth
			);
			if (Width < CurrWidth) Width = CurrWidth;

			if (I == 0)
				Size = FStringL(
					"%*s %'u (%'u bytes)%n",
					LabelWidth,
					HandlesStr,
					HandleCount,
					Handles[I].Size,
					&CurrWidth
				);
			else
				Size = FStringL(
					"%*s %'u bytes%n",
					LabelWidth,
					SizeStr,
					Handles[I].Size,
					&CurrWidth
				);
			if (Width < CurrWidth) Width = CurrWidth;

			Offset = FStringL(
				"%*s %'u bytes%n",
				LabelWidth,
				OffsetStr,
				Handles[I].Offset,
				&CurrWidth
			);
			if (Width < CurrWidth) Width = CurrWidth;

			string NextHandleStr = FStringL(
				"---Next %s---> %'d",
				UsageStr,
				(Handles[I].Free) ? Handles[I].NextFree : Handles[I].NextUsed
			);
			string PrevHandleStr = FStringL(
				"---Prev %s---> %'d",
				UsageStr,
				(Handles[I].Free) ? Handles[I].PrevFree : Handles[I].PrevUsed
			);
			string NextBlockStr =
				FStringL("---Next Block--> %'d", Handles[I].NextBlock);
			string PrevBlockStr =
				FStringL("---Prev Block--> %'d", Handles[I].PrevBlock);

			string BorderTop	= FStringL(".-%*s-.", Width, NoneStr);
			string BorderBottom = FStringL("'-%*s-'", Width, NoneStr);
			Mem_Set(BorderTop.Text + 2, '-', Width);
			Mem_Set(BorderBottom.Text + 2, '-', Width);

			String = FStringL(
				"%s\n%s\n| %*4$s%*s | %12$s\n| %8$*3$s | %13$s\n| %9$*3$s | "
				"%14$s\n| %10$*3$s | %15$s\n| %11$*3$s |\n%16$s\n",
				String,
				BorderTop,
				-Width,
				(Width + Name.Length) / 2,
				Name,
				(Width - Name.Length + 1) / 2,
				NoneStr,
				Data,
				Size,
				Offset,
				Flags,
				NextHandleStr,
				PrevHandleStr,
				NextBlockStr,
				PrevBlockStr,
				BorderBottom
			);
		}

		Platform_WriteFile(FileHandle, String.Text, String.Length, FileOffset);
		Platform_CloseFile(FileHandle);
	}
	Stack_Pop();

	Platform_UnlockMutex(&Heap->Mutex);
}

internal stack *
Stack_Init(vptr Mem, usize Size)
{
	stack *Result = Mem;
	*Result		  = (stack) { 0 };

	Result->Size		= Size - sizeof(stack);
	Result->FirstMarker = NULL;
	Result->Cursor		= (u08 *) Mem + sizeof(stack);

	_G.Stack = Result;

	return Result;
}

internal void
Stack_Push(void)
{
	vptr *Marker		   = (vptr *) _G.Stack->Cursor;
	*Marker				   = _G.Stack->FirstMarker;
	_G.Stack->FirstMarker  = Marker;
	_G.Stack->Cursor	  += sizeof(vptr);
}

// For unbounded allocations
internal vptr
Stack_GetCursor(void)
{
	return _G.Stack->Cursor;
}
internal void
Stack_SetCursor(vptr Cursor)
{
	Assert((u08 *) Cursor <= (u08 *) (_G.Stack + 1) + _G.Stack->Size);
	_G.Stack->Cursor = Cursor;
}

internal stack
Stack_Get(void)
{
	return *_G.Stack;
}
internal void
Stack_Set(stack Stack)
{
	*_G.Stack = Stack;
}

internal vptr
Stack_Allocate(usize Size)
{
	Platform_LockMutex(&_G.Stack->Mutex);

	vptr Result		  = _G.Stack->Cursor;
	_G.Stack->Cursor += Size;
	Assert(_G.Stack->Cursor <= (u08 *) (_G.Stack + 1) + _G.Stack->Size);

	Platform_UnlockMutex(&_G.Stack->Mutex);
	return Result;
}

internal void
Stack_Pop(void)
{
	_G.Stack->Cursor	  = (u08 *) _G.Stack->FirstMarker;
	_G.Stack->FirstMarker = *_G.Stack->FirstMarker;
}

#endif
