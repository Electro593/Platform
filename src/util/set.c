// /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
// **                                                                         **
// **  Author: Aria Seiler                                                    **
// **                                                                         **
// **  This program is in the public domain. There is no implied warranty,    **
// **  so use it at your own risk.                                            **
// **                                                                         **
// \* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef INCLUDE_HEADER

#define HASH_VACANT  0
#define HASH_DELETED 1

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

#define HASHMAP_FOREACH(I, Hash, key, Key, value, Value, Map) \
	for (usize I = 0; I < (Map)->Capacity; I++) \
		for (usize Hash = *(usize*) ((Map)->Data->Data + (Map)->EntrySize * I); Hash >= 2; ) \
		for (key Key = *(key*) ((Map)->Data->Data + (Map)->EntrySize * I + sizeof(usize)); Hash; ) \
		for (value Value = *(value*) ((Map)->Data->Data + (Map)->EntrySize * I + sizeof(usize) + (Map)->KeySize); Hash; Hash = 0)

#define SET_FUNCS \
   EXPORT(vptr,    BinarySearchArray,    vptr *Array, u32 Start, u32 End, vptr Target, type Type, cmp_func Func, vptr Param, u32 *IndexOut) \
   EXPORT(void,    QuickSort,            vptr Data, usize ElementSize, usize ElementCount, s08 (*Cmp)(vptr A, vptr B)) \
   EXPORT(hashmap, HashMap_InitCustom,   heap *Heap, u32 KeySize, u32 ValueSize, u32 InitialCapacity, r32 ResizeThresh, r32 ResizeRate, hash_func HashFunc, vptr HashParam, cmp_func CmpFunc, vptr CmpParam) \
   EXPORT(hashmap, HashMap_Init,         heap *Heap, u32 KeySize, u32 ValueSize) \
   EXPORT(b08,     HashMap_Get,          hashmap *Map, vptr Key, vptr ValueOut) \
   EXPORT(b08,     HashMap_Remove,       hashmap *Map, vptr Key, vptr ValueOut) \
   EXPORT(b08,     HashMap_Add,          hashmap *Map, vptr Key, vptr Value)

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
QuickSort(vptr Data, usize ElementSize, usize ElementCount, s08 (*Cmp)(vptr A, vptr B))
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
HashMap_MemHash(vptr Data, vptr Param)
{
	u32 Hash = 5381;
	for (usize I = 0; I < (usize) Param; I++) Hash = (Hash << 5) + Hash + ((u08 *) Data)[I];
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
HashMap_Init(heap *Heap, u32 KeySize, u32 ValueSize)
{
	return HashMap_InitCustom(Heap, KeySize, ValueSize, 64, 0.5f, 2.0f, NULL, NULL, NULL, NULL);
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
HashMap_Remove(hashmap *Map, vptr Key, vptr ValueOut)
{
	vptr Value = HashMap_GetRef(Map, Key);
	if (!Value) return FALSE;

	usize *Hash = (usize *) (Value - Map->KeySize - sizeof(usize));
	*Hash		= HASH_DELETED;

	if (ValueOut) Mem_Cpy(ValueOut, Value, Map->ValueSize);
	Mem_Set(Value - Map->KeySize, 0, Map->KeySize + Map->ValueSize);

	return TRUE;
}

internal b08
HashMap_AddWithHash(hashmap *Map, usize Hash, vptr Key, vptr Value)
{
	if (Hash < 2) Hash = 2;

	u32 Pow2Cap = U32_RoundUpPow2(Map->Capacity);
	for (usize I = 0; I < Pow2Cap; I++) {
		usize P = (Hash + (I + I * I) / 2) % Pow2Cap;
		if (P >= Map->Capacity) continue;

		vptr   Entry	 = Map->Data->Data + P * Map->EntrySize;
		usize *EntryHash = (usize *) Entry;
		if (*EntryHash == Hash) break;
		if (*EntryHash >= 2) continue;

		*(usize *) Entry = Hash;

		vptr EntryKey = Entry + sizeof(usize);
		Mem_Cpy(EntryKey, Key, Map->KeySize);

		vptr EntryValue = EntryKey + Map->KeySize;
		Mem_Cpy(EntryValue, Value, Map->ValueSize);

		return TRUE;
	}

	return FALSE;
}

internal void
HashMap_Grow(hashmap *Map)
{
	u32 OldCapacity		 = Map->Capacity;
	Map->Capacity		 = (u32) (Map->Capacity * Map->ResizeRate);
	heap_handle *OldData = Map->Data;

	Map->Data = Heap_Allocate(Heap_GetHeap(Map->Data), Map->Capacity * Map->EntrySize);
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

internal b08
HashMap_Add(hashmap *Map, vptr Key, vptr Value)
{
	Map->EntryCount++;
	if ((float) Map->EntryCount / Map->Capacity >= Map->ResizeThresh) HashMap_Grow(Map);

	usize Hash = Map->Hash(Key, Map->HashParam);
	return HashMap_AddWithHash(Map, Hash, Key, Value);
}

#endif
