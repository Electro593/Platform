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
   #include "main.c"
   #undef INCLUDE_HEADER

   #include <platform/platform.h>

   #define INCLUDE_SOURCE
   #include "main.c"
   #undef INCLUDE_SOURCE
#else
   #ifdef INCLUDE_SOURCE
      global struct module_state {
         stack *Stack;
      } _G;
      
      global util_funcs _F;
   #endif
   
   #include <util/intrin.h>
   #include <util/scalar.c>
   #include <util/vector.c>
   #include <util/memory.c>
   #include <util/bigint.c>
   #include <util/string.c>
   #include <util/set.c>
   #include <util/msdf.c>
   #include <util/font.c>
   #include <util/file.c>
   
   #ifdef INCLUDE_HEADER
      #define UTIL_FUNCS \
         SCALAR_FUNCS \
         VECTOR_FUNCS \
         MEMORY_FUNCS \
         BIGINT_FUNCS \
         STRING_FUNCS \
         SET_FUNCS \
         MSDF_FUNCS \
         FONT_FUNCS \
         FILE_FUNCS \

      typedef struct util_funcs {
         #define EXPORT(R, N, ...) \
            R (*N)(__VA_ARGS__);
         #define X UTIL_FUNCS
         #include <x.h>
      } util_funcs;

      #if defined(_UTIL_MODULE)
         #define EXPORT(R, N, ...) \
            internal R N(__VA_ARGS__);
         #define X UTIL_FUNCS
         #include <x.h>
      #else
         #define EXPORT(R, N, ...) \
            global R (*N)(__VA_ARGS__);
         #define X UTIL_FUNCS
         #include <x.h>
      #endif
   #endif
   
   #ifdef INCLUDE_SOURCE
      external API_EXPORT void
      Load(platform_state *Platform, platform_module *Module)
      {
         #define EXPORT(R, S, N, ...) S##_##N = Platform->Functions.S##_##N;
         #define X PLATFORM_FUNCS
         #include <x.h>
         
         _F = (util_funcs){
            #define EXPORT(R, N, ...) N,
            #define X UTIL_FUNCS
            #include <x.h>
         };
         
         Module->Data = &_G;
         Module->Funcs = &_F;
      }
   #endif
#endif