/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*                                                                            *
*  Author: Aria Seiler                                                       *
*                                                                            *
*  This program is in the public domain. There is no implied warranty, so    *
*  use it at your own risk.                                                  *
*                                                                            *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef INCLUDE_HEADER

typedef struct thread_handle thread_handle;

typedef struct tls {
	hashmap ThreadMap;
} tls;

#define TLS_FUNCS \
	EXPORT(tls,  Tls_Init,   heap *Heap, usize EntrySize) \
	EXPORT(vptr, Tls_Get,    tls *Tls, thread_handle *Thread) \
	EXPORT(b08,  Tls_Set,    tls *Tls, vptr Entry, thread_handle *Thread) \
	EXPORT(b08,  Tls_Remove, tls *Tls, thread_handle *Thread) \
	//

#endif

#ifdef INCLUDE_SOURCE

internal tls
Tls_Init(heap *Heap, usize EntrySize)
{
	Assert(Heap);
	Assert(EntrySize > 0);

	tls Tls		  = { 0 };
	Tls.ThreadMap = HashMap_Init(Heap, sizeof(s32), EntrySize);
	return Tls;
}

internal vptr
Tls_Get(tls *Tls, thread_handle *Thread)
{
	Assert(Tls);
	Assert(Tls->ThreadMap.Data);

	s32	 ThreadId = Platform_GetThreadId(Thread);
	vptr Entry	  = HashMap_GetRef(&Tls->ThreadMap, &ThreadId);
	if (Entry) return Entry;

	return HashMap_Add(&Tls->ThreadMap, &ThreadId, NULL);
}

internal b08
Tls_Set(tls *Tls, vptr Entry, thread_handle *Thread)
{
	Assert(Tls);
	Assert(Tls->ThreadMap.Data);
	Assert(Entry);

	s32 ThreadId = Platform_GetThreadId(Thread);
	return HashMap_Add(&Tls->ThreadMap, &ThreadId, Entry) != NULL;
}

internal b08
Tls_Remove(tls *Tls, thread_handle *Thread)
{
	Assert(Tls);
	Assert(Tls->ThreadMap.Data);

	s32 ThreadId = Platform_GetThreadId(Thread);
	return HashMap_Remove(&Tls->ThreadMap, &ThreadId, NULL, NULL);
}

#endif
