/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
**                                                                         **
**  Author: Aria Seiler                                                    **
**                                                                         **
**  This program is in the public domain. There is no implied warranty,    **
**  so use it at your own risk.                                            **
**                                                                         **
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if !defined(INCLUDE_SOURCE) && !defined(INCLUDE_HEADER)
   #include <shared.h>
   
   #define INCLUDE_HEADER
      #include <util/main.c>
   #undef INCLUDE_HEADER
   
   #include <platform/platform.h>
   
   #define INCLUDE_HEADER
      #include "main.c"
   #undef INCLUDE_HEADER
   
   #define INCLUDE_SOURCE
      #include "main.c"
   #undef INCLUDE_SOURCE
#else
   // #include <...>
   
   #ifdef INCLUDE_HEADER
      
   #endif
   
   #ifdef INCLUDE_SOURCE
      #define FUNCS \
      
      global struct module_state {
         vptr __dummy__;
      } _G;
      
      global struct module_funcs {
         #define EXPORT(R, N, ...) \
            R (*N)(__VA_ARGS__);
         #define X FUNCS
         #include <x.h>
         b08 __unused__;
      } _F = {
         #define EXPORT(R, N, ...) \
            N;
         #define X FUNCS
         #include <x.h>
         0
      };
      
      external API_EXPORT void
      Load(platform_state *Platform, platform_module *Module)
      {
         #define EXPORT(R, S, N, ...) S##_##N = Platform->Functions.S##_##N;
         #define X PLATFORM_FUNCS
         #include <x.h>
         
         platform_module *UtilModule = Platform_GetModule("util");
         util_funcs UtilFuncs = *(util_funcs*)UtilModule->Funcs;
         #define EXPORT(R, N, ...) N = UtilFuncs.N;
         #define X UTIL_FUNCS
         #include <x.h>
         
         Module->Data = &_G;
         Module->Funcs = &_F;
      }
      
      external API_EXPORT void
      Init(platform_state *Platform)
      {
         
      }
      
      external API_EXPORT void
      Update(platform_state *Platform)
      {
         
      }
      
      external API_EXPORT void
      Unload(platform_state *Platform)
      {
         
      }
   #endif
#endif