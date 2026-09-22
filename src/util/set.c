/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
**                                                                         **
**  Author: Aria Seiler                                                    **
**                                                                         **
**  This program is in the public domain. There is no implied warranty,    **
**  so use it at your own risk.                                            **
**                                                                         **
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef INCLUDE_HEADER

typedef struct array_list {
	vptr Data;
	u32	 ValueSize;
	u32	 Capacity;
	u32	 Count;
} array_list;

#define ARRAY_LIST_FOREACH(I, value, Value, List) \
	for (u32 I = 0; I < (List)->Count; I = (List)->Count) \
		for (value Value; I < (List)->Count && (Value = *(value*) ArrayList_At(List, I), TRUE); I++)

typedef struct array_deque {
	vptr Data;
	u32	 ValueSize;
	u32	 Capacity;
	u32	 Count;
	u32	 Front;
} array_deque;

#define ARRAY_DEQUE_FOREACH(I, value, Value, Deque) \
	for (u32 I = 0; I < (Deque)->Count; I = (Deque)->Count) \
		for (value Value; I < (Deque)->Count && (Value = *(value*) ArrayDeque_At(Deque, I), TRUE); I++)

typedef struct hashmap {
	heap_handle *Data;
	u32			 Capacity;
	u32			 EntryCount;
	u32			 EntrySize;
	u32			 KeySize;
	u32			 ValueSize;
	r32			 ResizeRate;
	r32			 ResizeThresh;

	vptr	 CmpParam;
	cmp_func Cmp;

	vptr	  HashParam;
	hash_func Hash;
} hashmap;

#define HASH_VACANT  0
#define HASH_DELETED 1

#define HASHMAP_FOREACH(I, Hash, key, Key, value, Value, Map) \
	for (usize I = 0; I < (Map)->Capacity; I++) \
		for (usize Hash = *(usize*) ((Map)->Data->Data + (Map)->EntrySize * I); Hash >= 2; ) \
			for (key Key = *(key*) ((Map)->Data->Data + (Map)->EntrySize * I + sizeof(usize)); Hash; ) \
				for (value Value = *(value*) ((Map)->Data->Data + (Map)->EntrySize * I + sizeof(usize) + (Map)->KeySize); Hash; Hash = 0)

#define SET_FUNCS \
	EXPORT(vptr,        BinarySearchArray,     vptr *Array, u32 Start, u32 End, vptr Target, type Type, cmp_func Func, vptr Param, u32 *IndexOut) \
	EXPORT(void,        QuickSort,             vptr Data, usize ElementSize, usize ElementCount, s08 (*Cmp)(vptr A, vptr B)) \
	INTERN(usize,       AlignReservedSize,     u32 ValueSize, u32 Capacity) \
	EXPORT(array_list,  ArrayList_Init,        heap *Heap, u32 ValueSize, u32 InitialCapacity) \
	INTERN(b08,         ArrayList_Validate,    array_list *List) \
	EXPORT(void,        ArrayList_Reserve,     array_list *List, u32 MinCapacity) \
	EXPORT(vptr,        ArrayList_At,          array_list *List, u32 Index) \
	EXPORT(vptr,        ArrayList_Push,        array_list *List) \
	EXPORT(void,        ArrayList_Pop,         array_list *List, vptr ValueOut) \
	INTERN(b08,         ArrayDeque_Validate,   array_deque *Deque) \
	EXPORT(array_deque, ArrayDeque_Init,       heap *Heap, u32 ValueSize, u32 InitialCapacity) \
	EXPORT(void,        ArrayDeque_Reserve,    array_deque *Deque, u32 MinCapacity) \
	EXPORT(vptr,        ArrayDeque_At,         array_deque *Deque, u32 Index) \
	EXPORT(vptr,        ArrayDeque_PeekFront,  array_deque *Deque) \
	EXPORT(vptr,        ArrayDeque_PeekBack,   array_deque *Deque) \
	EXPORT(vptr,        ArrayDeque_PushFront,  array_deque *Deque, vptr Value) \
	EXPORT(vptr,        ArrayDeque_PushBack,   array_deque *Deque, vptr Value) \
	EXPORT(void,        ArrayDeque_PopFront,   array_deque *Deque, vptr ValueOut) \
	EXPORT(void,        ArrayDeque_PopBack,    array_deque *Deque, vptr ValueOut) \
	EXPORT(hashmap,     HashMap_InitCustom,    heap *Heap, u32 KeySize, u32 ValueSize, u32 InitialCapacity, r32 ResizeThresh, r32 ResizeRate, hash_func HashFunc, vptr HashParam, cmp_func CmpFunc, vptr CmpParam) \
	EXPORT(hashmap,     HashMap_InitStr,       heap *Heap, u32 ValueSize, u32 InitialCapacity) \
	EXPORT(hashmap,     HashMap_Init,          heap *Heap, u32 KeySize, u32 ValueSize) \
	EXPORT(vptr,        HashMap_GetRef,        hashmap *Map, vptr Key) \
	EXPORT(b08,         HashMap_Get,           hashmap *Map, vptr Key, vptr ValueOut) \
	EXPORT(b08,         HashMap_Remove,        hashmap *Map, vptr Key, vptr KeyOut, vptr ValueOut) \
	EXPORT(vptr,        HashMap_Add,           hashmap *Map, vptr Key, vptr Value) \
	EXPORT(void,        HashMap_Free,          hashmap *Map)

#endif

#ifdef INCLUDE_SOURCE

internal vptr
BinarySearchArray(
	vptr	*Array,
	u32		 Start,
	u32		 End,
	vptr	 Target,
	type	 Type,
	cmp_func Func,
	vptr	 Param,
	u32		*IndexOut
)
{
	u32 Offset = 0;
	switch (Type.ID & TYPEID_MOD_MASK) {
		case TYPEID_MEMBER: {
			Offset = (Type.ID & TYPEID_EXTRA_MASK) >> TYPEID_EXTRA_EXP;
		}
	}

	u32	 Index = Start;
	vptr Curr  = NULL;

	while (Start != End) {
		Index = Start + (End - Start) / 2;
		Curr  = (vptr) ((u08 *) (*Array) + Offset + Type.Size * Index);

		b08 Cmp;
		if (Func) {
			Cmp = Func(Curr, Target, Param);
		} else {
			// Default handler
			switch (Type.ID & TYPEID_TYPE_MASK) {
				case TYPEID_U32: {
					if (*(u32 *) Curr < *(u32 *) Target) Cmp = LESS;
					else if (*(u32 *) Curr > *(u32 *) Target) Cmp = GREATER;
					else Cmp = EQUAL;
				} break;

				case TYPEID_U64:
				case TYPEID_VPTR: {
					if (*(u64 *) Curr < *(u64 *) Target) Cmp = LESS;
					else if (*(u64 *) Curr > *(u64 *) Target) Cmp = GREATER;
					else Cmp = EQUAL;
				} break;

				default: {
					Assert(FALSE, "Type not supported in BinarySearchArray!");
					Cmp = EQUAL;
				}
			}
		}

		if (Cmp == EQUAL) break;
		if (Cmp == LESS) Start = Index + 1;
		else End = Index;
	}

	// If we didn't find a match, give the insertion index
	// (In case the caller was looking for a place to insert)
	if (Index < Start) Index = Start;

	if (IndexOut) *IndexOut = Index;

	if (Start == End) Curr = NULL;
	return Curr;
}

internal void
QuickSort(
	vptr  Data,
	usize ElementSize,
	usize ElementCount,
	s08 (*Cmp)(vptr A, vptr B)
)
{
#define QSORT_SWAP(A, B) \
	do { \
		Mem_Cpy(Stack + SP, A, ElementSize); \
		Mem_Cpy(A, B, ElementSize); \
		Mem_Cpy(B, Stack + SP, ElementSize); \
	} while (0)

	vptr   Cursor = Stack_GetCursor();
	ssize *Stack  = Cursor;
	usize  SP	  = 0;

	Stack[SP++] = 0;
	Stack[SP++] = ElementCount - 1;

	while (SP > 0) {
		// Retrieve high and low
		ssize H = Stack[--SP];
		ssize L = Stack[--SP];
		if (L < 0 || H < 0 || L >= H) continue;

		// Select middle element as pivot
		vptr First	= Data + ElementSize * L;
		vptr Last	= Data + ElementSize * H;
		vptr Middle = Data + ElementSize * (L + (H - L) / 2);
		if (Cmp(Middle, First) < 0) QSORT_SWAP(First, Middle);
		if (Cmp(Last, First) < 0) QSORT_SWAP(First, Last);
		if (Cmp(Middle, Last) < 0) QSORT_SWAP(Middle, Last);
		vptr Pivot = Last;

		// Partition
		ssize I = L - 1;
		ssize J = H + 1;
		while (1) {
			do I++;
			while (Cmp(Data + ElementSize * I, Pivot) < 0);
			do J--;
			while (Cmp(Data + ElementSize * J, Pivot) > 0);
			if (I >= J) break;
			QSORT_SWAP(Data + ElementSize * I, Data + ElementSize * J);
		}

		if (J - L < H - (J + 1)) {
			// Upper half (larger, runs second)
			Stack[SP++] = J + 1;
			Stack[SP++] = H;
			// Lower half (smaller, runs first)
			Stack[SP++] = L;
			Stack[SP++] = J;
		} else {
			// Lower half (larger, runs second)
			Stack[SP++] = L;
			Stack[SP++] = J;
			// Upper half (smaller, runs first)
			Stack[SP++] = J + 1;
			Stack[SP++] = H;
		}
	}

	Stack_SetCursor(Cursor);
#undef QSORT_SWAP
}

internal usize
AlignReservedSize(u32 ValueSize, u32 Capacity)
{
	usize MinSize = (usize) Capacity * ValueSize;
	Assert(MinSize / ValueSize == Capacity);

	u32 Index;
	if (Intrin_BitScanReverse(&Index, MinSize - 1)) Index++;
	else Index = 0;

	usize AlignedSize = (Index >= sizeof(usize) * 8) ? MinSize : 1ull << Index;
	Assert(AlignedSize / ValueSize >= Capacity);

	return AlignedSize;
}

internal array_list
ArrayList_Init(heap *Heap, u32 ValueSize, u32 InitialCapacity)
{
	array_list List = {
		.Data	   = Heap_AllocateA(Heap, (usize) InitialCapacity * ValueSize),
		.ValueSize = ValueSize,
		.Capacity  = InitialCapacity,
		.Count	   = 0,
	};
	Assert(ArrayList_Validate(&List));
	return List;
}

internal b08
ArrayList_Validate(array_list *List)
{
	return List
		&& List->Data
		&& List->ValueSize > 0
		&& List->Count <= List->Capacity
		// Verify that offsets won't overflow
		&& (usize) List->Capacity * List->ValueSize / List->ValueSize
			   == List->Capacity;
}

internal void
ArrayList_Reserve(array_list *List, u32 MinCapacity)
{
	Assert(ArrayList_Validate(List));
	if (List->Capacity >= MinCapacity) return;

	usize NewSize	  = AlignReservedSize(List->ValueSize, MinCapacity);
	u32	  NewCapacity = NewSize / List->ValueSize;

	heap_handle *OldHandle = Heap_GetHandleA(List->Data);
	vptr		 NewData   = Heap_AllocateA(Heap_GetHeap(OldHandle), NewSize);
	Assert(NewData);

	Mem_Cpy(NewData, List->Data, List->Count * List->ValueSize);
	Heap_Free(OldHandle);

	List->Data	   = NewData;
	List->Capacity = NewCapacity;
}

internal vptr
ArrayList_At(array_list *List, u32 Index)
{
	Assert(ArrayList_Validate(List));
	Assert(Index < List->Count);
	return List->Data + Index * List->ValueSize;
}

internal vptr
ArrayList_Push(array_list *List)
{
	Assert(ArrayList_Validate(List));
	Assert(List->Count + 1 > 0);

	ArrayList_Reserve(List, List->Count + 1);
	List->Count++;

	return ArrayList_At(List, List->Count - 1);
}

internal void
ArrayList_Pop(array_list *List, vptr ValueOut)
{
	Assert(ArrayList_Validate(List));

	vptr EntryRef = ArrayList_At(List, List->Count - 1);
	if (ValueOut) Mem_Cpy(ValueOut, EntryRef, List->ValueSize);
	Debug_FillDeleted(EntryRef, List->ValueSize);

	List->Count--;
}

internal array_deque
ArrayDeque_Init(heap *Heap, u32 ValueSize, u32 InitialCapacity)
{
	array_deque Deque = {
		.Data	   = Heap_AllocateA(Heap, (usize) InitialCapacity * ValueSize),
		.ValueSize = ValueSize,
		.Capacity  = InitialCapacity,
		.Count	   = 0,
		.Front	   = 0,
	};
	ArrayDeque_Validate(&Deque);
	return Deque;
}

internal b08
ArrayDeque_Validate(array_deque *Deque)
{
	return Deque
		&& Deque->Data
		&& Deque->ValueSize > 0
		&& Deque->Count <= Deque->Capacity
		// Verify that Front is in bounds
		&& (!Deque->Capacity || Deque->Front < Deque->Capacity)
		// Verify that Back won't overflow
		&& ((usize) Deque->Capacity << 1) - 1 >= Deque->Capacity
		// Verify that offsets won't overflow
		&& (usize) Deque->Capacity * Deque->ValueSize / Deque->ValueSize
			   == Deque->Capacity;
}

internal void
ArrayDeque_Reserve(array_deque *Deque, u32 MinCapacity)
{
	Assert(ArrayDeque_Validate(Deque));
	if (Deque->Capacity >= MinCapacity) return;

	usize NewSize	  = AlignReservedSize(Deque->ValueSize, MinCapacity);
	u32	  NewCapacity = NewSize / Deque->ValueSize;

	heap_handle *OldHandle = Heap_GetHandleA(Deque->Data);
	vptr		 NewData   = Heap_AllocateA(Heap_GetHeap(OldHandle), NewSize);
	Assert(NewData);

	if (Deque->Count) {
		usize Front		  = Deque->Front;
		usize Back		  = (Front + Deque->Count - 1) % Deque->Capacity;
		usize FrontOffset = Front * Deque->ValueSize;
		usize BackSize	  = (Back + 1) * Deque->ValueSize;

		if (Front <= Back) {
			usize SpanSize = BackSize - FrontOffset;
			Mem_Cpy(NewData, Deque->Data + FrontOffset, SpanSize);
		} else {
			usize OldSize	= (usize) Deque->Capacity * Deque->ValueSize;
			usize FrontSize = OldSize - FrontOffset;
			Mem_Cpy(NewData, Deque->Data + FrontOffset, FrontSize);
			Mem_Cpy(NewData + FrontSize, Deque->Data, BackSize);
		}
	}

	Heap_Free(OldHandle);

	Deque->Data		= NewData;
	Deque->Capacity = NewCapacity;
	Deque->Front	= 0;
}

internal vptr
ArrayDeque_At(array_deque *Deque, u32 Index)
{
	Assert(ArrayDeque_Validate(Deque));
	Assert(Index < Deque->Count);
	usize Slot = ((usize) Deque->Front + Index) % Deque->Capacity;
	return Deque->Data + Slot * Deque->ValueSize;
}

internal vptr
ArrayDeque_PeekFront(array_deque *Deque)
{ return ArrayDeque_At(Deque, 0); }

internal vptr
ArrayDeque_PeekBack(array_deque *Deque)
{ return Assert(Deque), ArrayDeque_At(Deque, Deque->Count - 1); }

internal vptr
ArrayDeque_PushFront(array_deque *Deque, vptr Value)
{
	Assert(ArrayDeque_Validate(Deque));
	Assert(Deque->Count + 1 > 0);

	ArrayDeque_Reserve(Deque, Deque->Count + 1);
	if (Deque->Front == 0) Deque->Front = Deque->Capacity;
	Deque->Front--;
	Deque->Count++;

	vptr EntryRef = ArrayDeque_PeekFront(Deque);
	if (Value) Mem_Cpy(EntryRef, Value, Deque->ValueSize);
	return EntryRef;
}

internal vptr
ArrayDeque_PushBack(array_deque *Deque, vptr Value)
{
	Assert(ArrayDeque_Validate(Deque));
	Assert(Deque->Count + 1 > 0);

	ArrayDeque_Reserve(Deque, Deque->Count + 1);
	Deque->Count++;

	vptr EntryRef = ArrayDeque_PeekBack(Deque);
	if (Value) Mem_Cpy(EntryRef, Value, Deque->ValueSize);
	return EntryRef;
}

internal void
ArrayDeque_PopFront(array_deque *Deque, vptr ValueOut)
{
	Assert(ArrayDeque_Validate(Deque));

	vptr EntryRef = ArrayDeque_PeekFront(Deque);
	if (ValueOut) Mem_Cpy(ValueOut, EntryRef, Deque->ValueSize);
	Debug_FillDeleted(EntryRef, Deque->ValueSize);

	Deque->Count--;
	Deque->Front++;
	if (Deque->Front == Deque->Capacity) Deque->Front = 0;
}

internal void
ArrayDeque_PopBack(array_deque *Deque, vptr ValueOut)
{
	Assert(ArrayDeque_Validate(Deque));

	vptr EntryRef = ArrayDeque_PeekBack(Deque);
	if (ValueOut) Mem_Cpy(ValueOut, EntryRef, Deque->ValueSize);
	Debug_FillDeleted(EntryRef, Deque->ValueSize);

	Deque->Count--;
}

internal usize
HashMap_MemHash(vptr Data, vptr Param)
{
	u32 Hash = 5381;
	for (usize I = 0; I < (usize) Param; I++)
		Hash = (Hash << 5) + Hash + ((u08 *) Data)[I];
	return Hash;
}

internal hashmap
HashMap_InitCustom(
	heap	 *Heap,
	u32		  KeySize,
	u32		  ValueSize,
	u32		  InitialCapacity,
	r32		  ResizeThresh,
	r32		  ResizeRate,
	hash_func HashFunc,
	vptr	  HashParam,
	cmp_func  CmpFunc,
	vptr	  CmpParam
)
{
	hashmap Map;
	Map.EntrySize = sizeof(usize) + KeySize + ValueSize;

	Map.Capacity = InitialCapacity;
	Map.Data	 = Heap_Allocate(Heap, Map.EntrySize * Map.Capacity);
	Mem_Set(Map.Data->Data, 0, Map.Data->Size);

	Map.KeySize		 = KeySize;
	Map.ValueSize	 = ValueSize;
	Map.EntryCount	 = 0;
	Map.ResizeThresh = ResizeThresh <= 0 ? 0.1f : ResizeThresh;
	Map.ResizeRate	 = ResizeRate <= 1 ? 1.1f : ResizeRate;

	Map.HashParam = HashFunc ? HashParam : (vptr) (usize) Map.KeySize;
	Map.Hash	  = HashFunc ? HashFunc : (hash_func) HashMap_MemHash;

	Map.CmpParam = CmpFunc ? CmpParam : (vptr) (usize) Map.KeySize;
	Map.Cmp		 = CmpFunc ? CmpFunc : (cmp_func) Mem_Cmp;

	return Map;
}

internal hashmap
HashMap_InitStr(heap *Heap, u32 ValueSize, u32 InitialCapacity)
{
	return HashMap_InitCustom(
		Heap,
		sizeof(string),
		ValueSize,
		InitialCapacity,
		0.5f,
		2.0f,
		(hash_func) String_HashPtr,
		NULL,
		(cmp_func) String_CmpPtr,
		NULL
	);
}

internal hashmap
HashMap_Init(heap *Heap, u32 KeySize, u32 ValueSize)
{
	return HashMap_InitCustom(
		Heap,
		KeySize,
		ValueSize,
		64,
		0.5f,
		2.0f,
		NULL,
		NULL,
		NULL,
		NULL
	);
}

internal vptr
HashMap_GetRef(hashmap *Map, vptr Key)
{
	if (Map->EntryCount == 0) return NULL;

	usize Hash = Map->Hash(Key, Map->HashParam);
	if (Hash < 2) Hash = 2;

	u32 Pow2Cap = U32_RoundUpPow2(Map->Capacity);
	for (usize I = 0; I < Pow2Cap; I++) {
		usize P = (Hash + (I + I * I) / 2) % Pow2Cap;
		if (P >= Map->Capacity) continue;

		vptr   Entry	 = Map->Data->Data + P * Map->EntrySize;
		usize *EntryHash = (usize *) Entry;
		if (*EntryHash == HASH_VACANT) break;
		if (*EntryHash != Hash) continue;

		vptr EntryKey = Entry + sizeof(usize);
		if (Map->Cmp(Key, EntryKey, Map->CmpParam)) continue;

		vptr EntryValue = EntryKey + Map->KeySize;
		return EntryValue;
	}

	return NULL;
}

internal b08
HashMap_Get(hashmap *Map, vptr Key, vptr ValueOut)
{
	vptr Value = HashMap_GetRef(Map, Key);
	if (Value && ValueOut) Mem_Cpy(ValueOut, Value, Map->ValueSize);
	return !!Value;
}

internal b08
HashMap_Remove(hashmap *Map, vptr Key, vptr KeyOut, vptr ValueOut)
{
	vptr Value = HashMap_GetRef(Map, Key);
	if (!Value) return FALSE;

	vptr   StoredKey = Value - Map->KeySize;
	usize *Hash		 = (usize *) (StoredKey - sizeof(usize));
	*Hash			 = HASH_DELETED;

	if (KeyOut) Mem_Cpy(KeyOut, StoredKey, Map->KeySize);
	if (ValueOut) Mem_Cpy(ValueOut, Value, Map->ValueSize);
	Mem_Set(StoredKey, 0, Map->KeySize + Map->ValueSize);

	return TRUE;
}

internal vptr
HashMap_AddWithHash(hashmap *Map, usize Hash, vptr Key, vptr Value)
{
	if (Hash < 2) Hash = 2;

	u32 Pow2Cap = U32_RoundUpPow2(Map->Capacity);
	for (usize I = 0; I < Pow2Cap; I++) {
		usize P = (Hash + (I + I * I) / 2) % Pow2Cap;
		if (P >= Map->Capacity) continue;

		vptr   Entry	 = Map->Data->Data + P * Map->EntrySize;
		usize *EntryHash = (usize *) Entry;
		vptr   EntryKey	 = Entry + sizeof(usize);
		if (*EntryHash == Hash && Map->Cmp(Key, EntryKey, Map->CmpParam)) break;
		if (*EntryHash >= 2) continue;

		*(usize *) Entry = Hash;

		Mem_Cpy(EntryKey, Key, Map->KeySize);

		vptr EntryValue = EntryKey + Map->KeySize;
		if (Value) Mem_Cpy(EntryValue, Value, Map->ValueSize);
		else Mem_Set(EntryValue, 0, Map->ValueSize);

		return EntryValue;
	}

	return NULL;
}

internal void
HashMap_Grow(hashmap *Map)
{
	u32 OldCapacity		 = Map->Capacity;
	Map->Capacity		 = (u32) (Map->Capacity * Map->ResizeRate);
	heap_handle *OldData = Map->Data;

	Map->Data =
		Heap_Allocate(Heap_GetHeap(Map->Data), Map->Capacity * Map->EntrySize);
	Mem_Set(Map->Data->Data, 0, Map->Data->Size);

	for (usize I = 0; I < OldCapacity; I++) {
		vptr Hash = OldData->Data + Map->EntrySize * I;
		if (*(usize *) Hash < 2) continue;
		vptr Key   = Hash + sizeof(usize);
		vptr Value = Key + Map->KeySize;
		HashMap_AddWithHash(Map, *(usize *) Hash, Key, Value);
	}

	Heap_Free(OldData);
}

internal vptr
HashMap_Add(hashmap *Map, vptr Key, vptr Value)
{
	Map->EntryCount++;
	if ((float) Map->EntryCount / Map->Capacity >= Map->ResizeThresh)
		HashMap_Grow(Map);

	usize Hash = Map->Hash(Key, Map->HashParam);
	return HashMap_AddWithHash(Map, Hash, Key, Value);
}

internal void
HashMap_Free(hashmap *Map)
{
	Heap_Free(Map->Data);
	Mem_Set(Map, 0, sizeof(hashmap));
}

#endif
