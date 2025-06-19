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

global platform_state *Platform;

internal void Platform_Stub(void) { };

// These are simple implementations of various util functions that we need
// to have access to before we load the util module.

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

internal string
_CString(c08 *Chars)
{
   u32 Length = _Mem_BytesUntil((u08*) Chars, 0);
   return (string){
      .Text = Chars,
      .Length = Length,
      .Capacity = Length,
      .Resizable = FALSE
   };
}

// These are shared functions between the platforms

internal void
Platform_LoadUtilFuncs(platform_module *UtilModule)
{
   util_funcs Funcs = *(util_funcs*)UtilModule->Funcs;

   #define EXPORT(R, N, ...) N = Funcs.N;
   #define X UTIL_FUNCS
   #include <x.h>
}

internal platform_module *
Platform_GetModule(c08 *Name)
{
   u32 Len = _Mem_BytesUntil((u08*) Name, 0);
   u32 S = 0;
   u32 E = Platform->ModuleCount;
   platform_module *Module;
   while(S < E) {
      u32 I = S + (E-S)/2;

      Module = Platform->Modules+I;
      s08 Cmp = _Mem_Cmp((u08*) Module->Name, (u08*) Name, Len+1);

      if(Cmp == EQUAL) return Module;
      if(Cmp == LESS) S = I+1;
      else E = I;
   }

   return NULL;
}

internal platform_module *
Platform_LoadModule(c08 *Name)
{
   u32 NameLen = _Mem_BytesUntil((u08*) Name, 0);
   platform_module *Module = Platform_GetModule(Name);
   if (Module) return Module;

   u32 DesiredSize = (Platform->ModuleCount+1) * sizeof(platform_module);

   if(Platform->ModulesSize < DesiredSize) {
      vptr NewModules = Platform_AllocateMemory(DesiredSize*2);
      _Mem_Cpy(NewModules, (vptr)Platform->Modules, Platform->ModulesSize);
      Platform_FreeMemory(Platform->Modules, DesiredSize*2);
      Platform->ModulesSize = DesiredSize*2;
      Platform->Modules = NewModules;
   }

   //TODO: Use the binary search to get an index
   platform_module *Next = Platform->Modules;
   u32 I;
   for(I = 0; I < Platform->ModuleCount; I++) {
      s08 Cmp = _Mem_Cmp((u08*)Next->Name, (u08*)Name, NameLen+1);
      if(Cmp == LESS) {
         Next++;
         continue;
      }
      Assert(Cmp == GREATER, "Encountered a module that wasn't returned by Platform_GetModule");
      break;
   }

   u32 Delta = (Platform->ModuleCount-I)*sizeof(platform_module);
   _Mem_CpyRev((vptr)(Next+1), (vptr)Next, Delta);

   Module = Next;
   Platform->ModuleCount++;

   #ifdef _WIN32
      c08 *Path = Platform_AllocateMemory(NameLen+5);
      _Mem_Cpy((u08*) Path, (u08*) Name, NameLen);
      Path[NameLen+0] = '.';
      Path[NameLen+1] = 'd';
      Path[NameLen+2] = 'l';
      Path[NameLen+3] = 'l';
      Path[NameLen+4] = '\0';
   #else
      c08 *Path = Platform_AllocateMemory(NameLen+4);
      _Mem_Cpy((u08*) Path, (u08*) Name, NameLen);
      Path[NameLen+0] = '.';
      Path[NameLen+1] = 's';
      Path[NameLen+2] = 'o';
      Path[NameLen+4] = '\0';
   #endif

   _Mem_Set((vptr)Module, 0, sizeof(platform_module));
   Module->Name = Name;
   Module->FileName = Path;

   Platform_ReloadModule(Module);

   return Module;
}
