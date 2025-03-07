/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
**                                                                         **
**  Author: Aria Seiler                                                    **
**                                                                         **
**  This program is in the public domain. There is no implied warranty,    **
**  so use it at your own risk.                                            **
**                                                                         **
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <shared.h>

#define INCLUDE_HEADER
#include <util/main.c>
#undef INCLUDE_HEADER

#include <platform/platform.h>

#ifdef _OPENGL
#define INCLUDE_HEADER
   #include <renderer_opengl/opengl.h>
   // #include <renderer/opengl/mesh.h>
#undef INCLUDE_HEADER
#endif

internal void Platform_Stub(void) { };

internal void
_Mem_Set(u08 *D, u08 B, u32 Bytes)
{
   while(Bytes--) *D++ = B;
}

internal void
_Mem_Cpy(u08 *D, u08 *S, u32 Bytes)
{
   while(Bytes--) *D++ = *S++;
}

internal void
_Mem_CpyRev(u08 *D, u08 *S, u32 Bytes)
{
   D += Bytes-1;
   S += Bytes-1;
   while(Bytes--) *D-- = *S--;
}

internal s08
_Mem_Cmp(u08 *A, u08 *B, u32 Bytes)
{
   while(Bytes) {
      if(*A > *B) return GREATER;
      if(*A < *B) return LESS;
      A++, B++, Bytes--;
   }
   
   return EQUAL;
}

internal u32
_Mem_BytesUntil(u08 *P, c08 B)
{
   u32 Count = 0;
   while(*P++ != B) Count++;
   return Count;
}